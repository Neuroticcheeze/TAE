#include "PageDialogueEditor.hpp"
#include <Framework/UI/PageManager.hpp>
#include <Framework/Engine.hpp>
#include <[Tool] Dialogue Editor/Globals.hpp>
#include <Framework/Utils/Uuid.hpp>

#include <Framework/UI/Views/ViewTextField.hpp>
#include <Framework/UI/Views/ViewSizer.hpp>
#include <Framework/UI/Views/ViewRectangles.hpp>
#include <Framework/Math/Curve/Bezier.hpp>

//=====================================================================================
void PageDialogueEditor::SetupUI()
{

	m_ToolboxMenu = new ViewFlyoutMenu( "toolboxMenu", this, GetRootView() );
	m_ToolboxMenu->SetBordersFromSizeAndOffset( Vector2( 315.0F, 0.0F ) );
	m_ToolboxMenu->SetTint( Colour::GRAY * 0.75F );

	
	Array< FlyoutContentTree > contents;

	FlyoutContentTree item1;
	item1.Name = "Add";

	FlyoutContentTree item1a;
	item1a.Name = "Dialogue Node";
	item1a.ExecuteAction = [ this ]()
		{
			DialogueEditor::EditorNode node;
			node.Name = "Node";
			node.Position = Vector2( InputManager::Instance().GetMousePosition() + Vector2( 100.0F ) );
			node.Size = Vector2( 400.0F, 125.0F );
			node.Type = DialogueEditor::EditorNode::DIALOGUE;

			DialogueEditor::Get()->AddEditorNode( node );
			AddUINode( GenUniqueRND(), node );
			m_ToolboxMenu->SetEnabled( false );
		};
	item1a.KeyShortcut.HeldKeyCode = KeyShortcut::CTRL;
	item1a.KeyShortcut.KeyCode = InputManager::KEYCODE_A;
	item1.Members.Append( item1a );

	FlyoutContentTree item1b;
	item1b.Name = "Blackboard Node";
	item1b.ExecuteAction = [ this ]()
		{
			DialogueEditor::EditorNode node;
			node.Name = "Blackboard #1";
			node.Position = Vector2( InputManager::Instance().GetMousePosition() + Vector2( 100.0F ) );
			node.Size = Vector2( 400.0F, 125.0F );
			node.Type = DialogueEditor::EditorNode::DIALOGUE;

			DialogueEditor::Get()->AddEditorNode( node );
			AddUINode( GenUniqueRND(), node );
			m_ToolboxMenu->SetEnabled( false );
		};
	item1b.KeyShortcut.HeldKeyCode = KeyShortcut::CTRL;
	item1b.KeyShortcut.KeyCode = InputManager::KEYCODE_B;
	item1.Members.Append( item1b );
	
	contents.Append( item1 );

	FlyoutContentTree item2;
	item2.Name = "Select All";
	contents.Append( item2 );

	m_ToolboxMenu->SetContentTree( contents );
}

//=====================================================================================
void PageDialogueEditor::OnMousePressed( InputManager::MouseButton a_MouseButton )
{
	if ( a_MouseButton == InputManager::MouseButton::RIGHT )
	{
		m_ToolboxMenu->SetBordersFromSizeAndOffset( m_ToolboxMenu->GetSize(), InputManager::Instance().GetMousePosition() - m_ToolboxMenu->GetSize() / 2.0F );
		m_ToolboxMenu->SetEnabled( true );
	}
}

//=====================================================================================
void PageDialogueEditor::OnEnterPage()
{
	PROFILE;

	SetupUI();
	GetRootView()->AddActionListener( this );
	InputManager::Instance().AttachListener( InputManager::InputMouseEvent::ON_MOUSE_PRESSED, this );
}

//=====================================================================================
void PageDialogueEditor::OnExitPage()
{
	InputManager::Instance().DetatchListener( InputManager::InputMouseEvent::ON_MOUSE_PRESSED, this );
	GetRootView()->RemoveActionListener( this );

	// Free all UI nodes
	while ( m_UIDraggableNodes.Count() > 0 )
	{
		RemoveUINode( m_UIDraggableNodes.GetEntries()[ 0U ].Key );
	}

	GetRootView()->DetatchChild( m_ToolboxMenu );
	Free( m_ToolboxMenu );
}

//=====================================================================================
void PageDialogueEditor::OnTick( float a_DeltaTime )
{
}

//=====================================================================================
void PageDialogueEditor::OnTickPost( float a_DeltaTime )
{
	GraphicsManager::Instance().SetColour( Colour::GRAY * 0.2F, GraphicsManager::COL_BACKGROUND );
}

//=====================================================================================
void PageDialogueEditor::AddUINode( uint32_t a_Id, const DialogueEditor::EditorNode & a_NodeData )
{
	RemoveUINode( a_Id );

	ViewDraggable * uiNode = new ViewDraggable( CString().Format( "UINode_%u", a_Id ).Get(), this );
	m_UIDraggableNodes.Put( a_Id, uiNode );

	uiNode->SetBordersFromSizeAndOffset( a_NodeData.Size, a_NodeData.Position );
	uiNode->SetTint( Colour::WHITE );

	Colour _mainTint = Colour::GRAY;
	switch ( a_NodeData.Type )
	{
	case decltype(a_NodeData.Type)::DIALOGUE: _mainTint = Colour::RED;
		break;
	case decltype(a_NodeData.Type)::RESPONSE: _mainTint = Colour::COBALT;
		break;
	case decltype(a_NodeData.Type)::SELECTOR: _mainTint = Colour( 1.0F, 0.875F, 0.1F, 1.0F );
		break;
	default:
		break;
	}

	// Add elements
	{
		ViewRectangles * background = new ViewRectangles( "bg", this, uiNode );
		background->GetRectangleList().Append( { Vector2::ZERO, Vector2::ONE, 3.0F, ( Colour::GRAY * 0.4F ).WithAlpha( 1.0F ), ( Colour::GRAY * 0.9F ).WithAlpha( 1.0F ) } );
		background->SetBorder( View::Alignment::TOP, 0.0F );
		background->SetBorder( View::Alignment::LEFT, 0.0F );
		background->SetBorder( View::Alignment::RIGHT, 0.0F );
		background->SetBorder( View::Alignment::BOTTOM, 0.0F );
		
		ViewRectangles * tag = new ViewRectangles( "top", this, uiNode );
		tag->GetRectangleList().Append( { Vector2::ZERO, Vector2::ONE, 3.0F, ( _mainTint.Lerp( Colour::GRAY, 0.6F ) * 0.9F ).WithAlpha( 1.0F ), ( _mainTint.Lerp( Colour::GRAY, 0.6F ) * 0.5F ).WithAlpha( 1.0F ) } );
		tag->SetBorder( View::Alignment::TOP, 0.0F );
		tag->SetBorder( View::Alignment::LEFT, 0.0F );
		tag->SetBorder( View::Alignment::RIGHT, 0.0F );
		tag->SetBorderPx( View::Alignment::BOTTOM, 64.0F );
		tag->SetTint( Colour::WHITE );
		
		ViewTextField * name = new ViewTextField( "name", this, tag );
		name->SetEmptyTextString( "" );
		name->GetTextView().SetText( a_NodeData.Name.Get() );
		name->SetCursorToEnd();
		name->GetTextView().SetBitmapFont( GraphicsManager::Instance().GetBitmapFont( BF_LETTERS_AND_DIGITS_DefaultBodyLarge ) );
		name->GetTextView().SetVAlign( GraphicsManager::TextAlignment::TA_CENTER );
		name->SetBorder( View::Alignment::TOP, 0.0F );
		name->SetBorder( View::Alignment::LEFT, 0.0F );
		name->SetBorder( View::Alignment::RIGHT, 0.0F );
		name->SetBorder( View::Alignment::BOTTOM, 0.0F );

		ViewText * data = new ViewText( "data", this, tag );
		data->SetTint( Colour::WHITE );
		data->SetText( "String ID: " FONTCOLOUR_TAG(FF2233FF) "<none>", true );
		data->SetBitmapFont( GraphicsManager::Instance().GetBitmapFont( BF_LETTERS_AND_DIGITS_DefaultBody ) );
		data->SetBorder( View::Alignment::BOTTOM, -1.1F );
		data->SetBorder( View::Alignment::TOP, 1.1F );
		data->SetBorder( View::Alignment::LEFT, 0.03F );
		data->SetBorder( View::Alignment::RIGHT, 0.03F );

		ViewSizer * sizer = new ViewSizer( "sizer", this, uiNode );
		sizer->SetTint( Colour::WHITE );
		sizer->SetBorderPx( View::Alignment::TOP, 32.0F );
		sizer->SetBorderPx( View::Alignment::LEFT, 32.0F );
		sizer->SetBorder( View::Alignment::RIGHT, 0.0F );
		sizer->SetBorder( View::Alignment::BOTTOM, 0.0F );
		sizer->SetMinXYSizeF( Vector2( 0.5F, 0.2F ) );
		sizer->SetAxors( View::Alignment::RIGHT, View::Alignment::BOTTOM );
	}
}

//=====================================================================================
void PageDialogueEditor::RemoveUINode( uint32_t a_Id )
{
	if ( ViewDraggable ** uiNode = m_UIDraggableNodes[ a_Id ] )
	{
		GetRootView()->DetatchChild( *uiNode );
		Free( *uiNode );
		m_UIDraggableNodes.Remove( a_Id );
	}
}