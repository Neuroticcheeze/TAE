#include "Globals.hpp"
#include <Framework/Graphics/GraphicsManager.hpp>
#include <Framework/Utils/Hash.hpp>

//=====================================================================================
void Globals::CreateBitmapFonts()
{
	GraphicsManager::Instance().LoadBitmapFont( BF_LETTERS_AND_DIGITS_DefaultBodyLarge,	" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789`~!@#$%^&*()_+-=,./;'[]\<>?:\"{}|", nullptr, 45.0F );
	GraphicsManager::Instance().LoadBitmapFont( BF_LETTERS_AND_DIGITS_DefaultBody,		" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789`~!@#$%^&*()_+-=,./;'[]\<>?:\"{}|", nullptr, 32.0F );
	GraphicsManager::Instance().LoadBitmapFont( BF_LETTERS_AND_DIGITS_DefaultHeading,	" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789`~!@#$%^&*()_+-=,./;'[]\<>?:\"{}|", nullptr, 90.0F );
	GraphicsManager::Instance().LoadBitmapFont( BF_LETTERS_AND_DIGITS_DefaultTiny,		" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789`~!@#$%^&*()_+-=,./;'[]\<>?:\"{}|", nullptr, 20.0F );
}

//=====================================================================================
void Globals::ReleaseBitmapFonts()
{
	GraphicsManager::Instance().UnloadBitmapFont( BF_LETTERS_AND_DIGITS_DefaultBodyLarge );
	GraphicsManager::Instance().UnloadBitmapFont( BF_LETTERS_AND_DIGITS_DefaultBody );
	GraphicsManager::Instance().UnloadBitmapFont( BF_LETTERS_AND_DIGITS_DefaultHeading );
	GraphicsManager::Instance().UnloadBitmapFont( BF_LETTERS_AND_DIGITS_DefaultTiny );
}