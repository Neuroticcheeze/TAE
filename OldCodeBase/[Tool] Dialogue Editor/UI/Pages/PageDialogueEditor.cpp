#include "PageDialogueEditor.hpp"
#include <Framework/UI/PageManager.hpp>
#include <Framework/Engine.hpp>
#include <[Tool] Dialogue Editor/DialogueToolGlobals.hpp>
#include <Framework/Utils/Uuid.hpp>

#include <Framework/UI/Views/ViewTextField.hpp>
#include <Framework/UI/Views/ViewSizer.hpp>
#include <Framework/UI/Views/ViewRectangles.hpp>
#include <Framework/UI/Views/ViewColour.hpp>
#include <[Tool] Dialogue Editor/UI/Views/ViewNodePlug.hpp>

#include <[Tool] Dialogue Editor/Structure/Objects/BlackboardValue.hpp>
#include <[Tool] Dialogue Editor/Structure/Objects/Dialogue.hpp>

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
			node.Position = Vector2( InputManager::Instance().GetMousePosition() + Vector2( 100.0F ) );
			node.Size = Vector2( 400.0F, 125.0F );
			node.Exposer = Object::Construct< DialogueObject >();


			uint32_t rndId = GenUniqueRND();
			DialogueEditor::Get()->AddEditorNode( rndId, node );
			AddUINode( rndId, node );
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
			node.Position = Vector2( InputManager::Instance().GetMousePosition() + Vector2( 100.0F ) );
			node.Size = Vector2( 640.0F, 480.0F );
			node.Exposer = Object::Construct< BlackboardValueObject >();

			uint32_t rndId = GenUniqueRND();
			DialogueEditor::Get()->AddEditorNode( rndId, node );
			AddUINode( rndId, node );
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
	InputManager::Instance().AttachListener( InputManager::InputMouseEvent::ON_MOUSE_PRESSED, this );
}

//=====================================================================================
void PageDialogueEditor::OnExitPage()
{
	InputManager::Instance().DetatchListener( InputManager::InputMouseEvent::ON_MOUSE_PRESSED, this );
	
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

	ViewDraggable * uiNode = new ViewDraggable( CString::Format( "UINode_%u", a_Id ).Get(), this );
	m_UIDraggableNodes.Put( a_Id, uiNode );

	uiNode->SetBordersFromSizeAndOffset( a_NodeData.Size, a_NodeData.Position );
	uiNode->SetTint( Colour::WHITE );

	Colour _mainTint = a_NodeData.Exposer->m_Tint;

	// Add elements
	{
		ViewRectangles * background = new ViewRectangles( "bg", this, uiNode );
		background->GetRectangleList().Append( { Vector2::ZERO, Vector2::ONE, 3.0F, ( Colour::GRAY * 0.4F ).WithAlpha( 1.0F ), ( Colour::GRAY * 0.9F ).WithAlpha( 1.0F ) } );
		background->SetBorder( View::Alignment::TOP, 0.0F );
		background->SetBorder( View::Alignment::LEFT, 0.0F );
		background->SetBorder( View::Alignment::RIGHT, 0.0F );
		background->SetBorder( View::Alignment::BOTTOM, 0.0F );
		
		ViewRectangles * tag = new ViewRectangles( "top", this, uiNode );
		tag->GetRectangleList().Append( { Vector2::ZERO, Vector2::ONE, 3.0F, ( _mainTint * 0.9F ).WithAlpha( 1.0F ), ( _mainTint * 0.5F ).WithAlpha( 1.0F ) } );
		tag->SetBorder( View::Alignment::TOP, 0.0F );
		tag->SetBorder( View::Alignment::LEFT, 0.0F );
		tag->SetBorder( View::Alignment::RIGHT, 0.0F );
		tag->SetBorderPx( View::Alignment::BOTTOM, 64.0F );
		tag->SetTint( Colour::WHITE );
		
		ViewTextField * name = new ViewTextField( "name", this, tag );
		name->SetEmptyTextString( "" );
		name->GetTextView().SetText( a_NodeData.Exposer->m_Name.Get() );
		name->SetCursorToEnd();
		name->GetTextView().SetBitmapFont( GraphicsManager::Instance().GetBitmapFont( BF_LETTERS_AND_DIGITS_DefaultBodyLarge ) );
		name->GetTextView().SetVAlign( GraphicsManager::TextAlignment::TA_CENTER );
		name->SetBorder( View::Alignment::TOP, 0.0F );
		name->SetBorder( View::Alignment::LEFT, 0.0F );
		name->SetBorder( View::Alignment::RIGHT, 0.0F );
		name->SetBorder( View::Alignment::BOTTOM, 0.0F );

		
		auto CreateInputPlug = [ tag, a_Id ]( const Exposer::Pkt & a_Input, float & a_YOffset, uint32_t & a_Sub )
		{
			// Calculate alignments
			const float h = 48.0F / tag->GetSize().y; // 48 px in height
			float f = 1.25F + a_YOffset;
			a_YOffset += h + 0.25F;

			ViewNodePlug * plug = new ViewNodePlug( a_Id, a_Sub++, ViewNodePlug::Type::INPUT, "uiinputplug", tag->GetContainerPage(), tag );
			plug->SetBorder( View::Alignment::BOTTOM, 1.0F - f - h );
			plug->SetBorder( View::Alignment::TOP,	0.0F + f );
			plug->SetBorder( View::Alignment::LEFT, 0.0F );
			plug->SetBorderPx( View::Alignment::RIGHT, 16.0F );
			plug->SetTint( Colour::GRAY );

			plug->OnConnectEventFunctor = [ a_Input ]( const ViewNodePlug & a_This )
			{
				const uint32_t nodeId = a_This.GetConnection()->GetNodeId();
				const uint8_t idx = a_This.GetConnection()->GetSub();

				DialogueEditor::EditorNode * eNode = DialogueEditor::Get()->GetEditorNode( nodeId );
				if ( ASSERT( eNode, "There is no editor node with id=%u - This is a bad state since if this widget exists, the editor node MUST also exist.", nodeId ) )
				{
					*reinterpret_cast< Ref* >( a_Input.m_OMAddress ) = Ref( eNode->Exposer->GetUnderlyingObject(), idx );
				}
			};
		};
		auto CreateOutputPlug = [ tag, a_Id ]( const Exposer::Pkt & a_Output, float & a_YOffset, uint32_t & a_Sub )
		{
			// Calculate alignments
			const float h = 48.0F / tag->GetSize().y; // 48 px in height
			float f = 1.25F + a_YOffset;
			a_YOffset += h + 0.25F;

			ViewNodePlug * plug = new ViewNodePlug( a_Id, a_Sub++, ViewNodePlug::Type::OUTPUT, "uioutputplug", tag->GetContainerPage(), tag );
			plug->SetBorder( View::Alignment::BOTTOM, 1.0F - f - h );
			plug->SetBorder( View::Alignment::TOP,	0.0F + f );
			plug->SetBorder( View::Alignment::RIGHT, 0.0F );
			plug->SetBorderPx( View::Alignment::LEFT, 48.0F * 1.5F );
			plug->SetTint( ( Colour::WHITE * 0.8F ).WithAlpha( 1.0F ) );

			plug->OnConnectEventFunctor = [ a_Output ]( const ViewNodePlug & a_This )
			{
				const uint32_t nodeId = a_This.GetConnection()->GetNodeId();
				const uint8_t idx = a_This.GetConnection()->GetSub();

				DialogueEditor::EditorNode * eNode = DialogueEditor::Get()->GetEditorNode( nodeId );
				if ( ASSERT( eNode, "There is no editor node with id=%u - This is a bad state since if this widget exists, the editor node MUST also exist.", nodeId ) )
				{
					*reinterpret_cast< Ref* >( a_Output.m_OMAddress ) = Ref( eNode->Exposer->GetUnderlyingObject(), idx );
				}
			};
		};

		auto CreateStringUIHandler = [ tag ]( const Exposer::Pkt & a_Field, float & a_YOffset )
		{
			ViewRectangles * inputContainer = new ViewRectangles( "uihandle", tag->GetContainerPage(), tag );
			ViewRectangles::Info rect0;
			rect0.Position = Vector2::ZERO;
			rect0.Size = Vector2::ONE;
			rect0.Thickness = 3.0F;
			rect0.TintA = ( Colour::WHITE * 0.25F ).WithAlpha( 1.0F );
			rect0.TintB = ( Colour::WHITE * 0.75F ).WithAlpha( 1.0F );
			inputContainer->GetRectangleList().Append( rect0 );

			// Calculate alignments
			const float h = 48.0F / tag->GetSize().y; // 48 px in height
			float f = 1.25F + a_YOffset;
			a_YOffset += h + 0.25F;

			inputContainer->SetBorder( View::Alignment::BOTTOM, 1.0F - f - h );
			inputContainer->SetBorder( View::Alignment::TOP,	0.0F + f );
			inputContainer->SetBorder( View::Alignment::LEFT, 0.3F );
			inputContainer->SetBorder( View::Alignment::RIGHT, 0.15F );

			ViewTextField * data = new ViewTextField( "uihandle", tag->GetContainerPage(), inputContainer );
			data->SetTint( Colour::WHITE );
			data->GetTextView().SetBitmapFont( GraphicsManager::Instance().GetBitmapFont( BF_LETTERS_AND_DIGITS_DefaultBody ) );
			data->SetBorder( View::Alignment::BOTTOM, 0.0F );
			data->SetBorder( View::Alignment::TOP, 0.0F );
			data->SetBorder( View::Alignment::LEFT, 0.0F );
			data->SetBorder( View::Alignment::RIGHT, 0.0F );
			data->InputChangedEventFunctor = [ a_Field ]( const CString & a_Text )
			{
				*reinterpret_cast< CString* >( a_Field.m_OMAddress ) = a_Text;
			};

			ViewText * label = new ViewText( "uihandle_label", tag->GetContainerPage(), data );
			label->SetTint( Colour::GRAY );
			label->SetText( a_Field.m_Name.Get() );
			label->SetHAlign( GraphicsManager::TextAlignment::TA_RIGHT );
			label->SetVAlign( GraphicsManager::TextAlignment::TA_CENTER );
			label->SetBitmapFont( GraphicsManager::Instance().GetBitmapFont( BF_LETTERS_AND_DIGITS_DefaultBody ) );
			label->SetBorder( View::Alignment::BOTTOM, 0.0F );
			label->SetBorder( View::Alignment::TOP, 0.0F );
			label->SetBorder( View::Alignment::LEFT, -0.05F );
			label->SetBorder( View::Alignment::RIGHT, 1.05F );
		};
		
		auto CreateColourUIHandler = [ tag ]( const Exposer::Pkt & a_Field, float & a_YOffset )
		{
			ViewRectangles * inputContainer = new ViewRectangles( "uihandle", tag->GetContainerPage(), tag );
			ViewRectangles::Info rect0;
			rect0.Position = Vector2::ZERO;
			rect0.Size = Vector2::ONE;
			rect0.Thickness = 3.0F;
			rect0.TintA = ( Colour::WHITE * 0.25F ).WithAlpha( 1.0F );
			rect0.TintB = ( Colour::WHITE * 0.75F ).WithAlpha( 1.0F );
			inputContainer->GetRectangleList().Append( rect0 );

			// Calculate alignments
			const float h = 128.0F / tag->GetSize().y; // 48 px in height
			float f = 1.25F + a_YOffset;
			a_YOffset += h + 0.25F;

			inputContainer->SetBorder( View::Alignment::BOTTOM, 1.0F - f - h );
			inputContainer->SetBorder( View::Alignment::TOP,	0.0F + f );
			inputContainer->SetBorder( View::Alignment::LEFT, 0.3F );
			inputContainer->SetBorder( View::Alignment::RIGHT, 0.4F );

			ViewColour * data = new ViewColour( "uihandle", tag->GetContainerPage(), inputContainer );
			data->SetBorder( View::Alignment::BOTTOM, 0.0F );
			data->SetBorder( View::Alignment::TOP, 0.0F );
			data->SetBorder( View::Alignment::LEFT, 0.0F );
			data->SetBorder( View::Alignment::RIGHT, 0.0F );
			//data->InputChangedEventFunctor = [ a_Field ]( const CString & a_Text )
			//{
			//	*reinterpret_cast< CString* >( a_Field.m_OMAddress ) = a_Text;
			//};

			ViewText * label = new ViewText( "uihandle_label", tag->GetContainerPage(), data );
			label->SetTint( Colour::GRAY );
			label->SetText( a_Field.m_Name.Get() );
			label->SetHAlign( GraphicsManager::TextAlignment::TA_RIGHT );
			label->SetVAlign( GraphicsManager::TextAlignment::TA_CENTER );
			label->SetBitmapFont( GraphicsManager::Instance().GetBitmapFont( BF_LETTERS_AND_DIGITS_DefaultBody ) );
			label->SetBorder( View::Alignment::BOTTOM, 0.0F );
			label->SetBorder( View::Alignment::TOP, 0.0F );
			label->SetBorder( View::Alignment::LEFT, -0.05F );
			label->SetBorder( View::Alignment::RIGHT, 1.05F );
		};

		uint32_t sub = 0;

		float yOffset = 0.0F;
		const auto & fields = a_NodeData.Exposer->m_Fields.GetEntries();
		for ( uint32_t i = 0; i < fields.Count(); ++i )
		{
			if ( fields[ i ].Value.m_OMType == Exposer::Pkt::OMTYPE_STRING )
			{
				CreateStringUIHandler( fields[ i ].Value, yOffset );
			}

			else if ( fields[ i ].Value.m_OMType == Exposer::Pkt::OMTYPE_COLOUR )
			{
				CreateColourUIHandler( fields[ i ].Value, yOffset );
			}
		}

		sub = 0;
		yOffset = 0.0F;
		const auto & inputs = a_NodeData.Exposer->m_Inputs.GetEntries();
		for ( uint32_t i = 0; i < inputs.Count(); ++i )
		{
			CreateInputPlug( inputs[ i ].Value, yOffset, sub );
		}

		sub = 0;
		yOffset = 0.0F;
		const auto & outputs = a_NodeData.Exposer->m_Outputs.GetEntries();
		for ( uint32_t i = 0; i < outputs.Count(); ++i )
		{
			CreateOutputPlug( outputs[ i ].Value, yOffset, sub );
		}

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