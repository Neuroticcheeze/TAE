#include "DialogueToolGlobals.hpp"
#include <Framework/Graphics/GraphicsManager.hpp>
#include <Framework/Utils/Hash.hpp>

//=====================================================================================
void DialogueToolGlobals::CreateBitmapFonts()
{
	GraphicsManager::Instance().LoadBitmapFont( BF_LETTERS_AND_DIGITS_DefaultBodyLarge,	" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789`~!@#$%^&*()_+-=,./;'[]\<>?:\"{}|", nullptr, 42.0F );
	GraphicsManager::Instance().LoadBitmapFont( BF_LETTERS_AND_DIGITS_DefaultBody,		" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789`~!@#$%^&*()_+-=,./;'[]\<>?:\"{}|", nullptr, 28.0F );
	GraphicsManager::Instance().LoadBitmapFont( BF_LETTERS_AND_DIGITS_DefaultHeading,	" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789`~!@#$%^&*()_+-=,./;'[]\<>?:\"{}|", nullptr, 90.0F );
	GraphicsManager::Instance().LoadBitmapFont( BF_LETTERS_AND_DIGITS_DefaultTiny,		" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789`~!@#$%^&*()_+-=,./;'[]\<>?:\"{}|", nullptr, 20.0F );
}

//=====================================================================================
void DialogueToolGlobals::ReleaseBitmapFonts()
{
	GraphicsManager::Instance().UnloadBitmapFont( BF_LETTERS_AND_DIGITS_DefaultBodyLarge );
	GraphicsManager::Instance().UnloadBitmapFont( BF_LETTERS_AND_DIGITS_DefaultBody );
	GraphicsManager::Instance().UnloadBitmapFont( BF_LETTERS_AND_DIGITS_DefaultHeading );
	GraphicsManager::Instance().UnloadBitmapFont( BF_LETTERS_AND_DIGITS_DefaultTiny );
}