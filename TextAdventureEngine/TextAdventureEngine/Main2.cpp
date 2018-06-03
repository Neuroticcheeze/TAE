/*

REGIONS:



Formatted-Name -> eg "LOCALITY~REGION~PROVINCE"











*/

#include <SIGIL-0.9.0\sl.h>
#include <Framework/Graphics/Particles/ParticleSystem.hpp>
#include <Framework/Graphics/Particles/Emitter.hpp>
#include <Framework/Graphics/Particles/EmitterInfo.hpp>
#include <Framework/Containers/CString.hpp>
#include <Framework/GameStructure/Items/Inventory.hpp>
#include <Framework/GameStructure/Items/Item.hpp>
#include <Framework/GameStructure/Items/ItemGroup.hpp>
#include <Framework/GameStructure/Items/ItemRegistry.hpp>
#include <Framework/GameStructure/Player.hpp>
#include <Framework/Utils/Hash.hpp>
#include <Framework/Graphics\SpriteSheet.hpp>
#include <Framework/GameStructure/UI/PageItemInventory.hpp>
#include <Framework/GameStructure/UI/PageManager.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>
#include <Framework/Config/Config.hpp>
#include <Framework/Graphics/GraphicsManager.hpp>
#include <Framework/Graphics/NineSpriteSheet.hpp>
#include <Framework/Audio/AudioManager.hpp>
#include <Framework/File/Save/SaveManager.hpp>
#include <Framework/File/Save/ISaveLoadListener.hpp>

#include <Framework/GameStructure/UI/FadeManager.hpp>
#include <Framework/GameStructure/World/Nature/TimeFlow.hpp>
#include <Framework/Containers/Array.hpp>
#include <Framework/GameStructure/World/Nature/Meteorology2/Climatograph.hpp>
#include <Framework/GameStructure/World/Nature/Meteorology2/MeteorologyUtils.hpp>
#include <Framework/GameStructure/World/Nature/Meteorology2/WindSystem.hpp>

class GameSettingsSaveLoadListener final : public ISaveLoadListener
{
protected:

	bool Save(FileStream & fs)
	{
		fs.Write(Config::WINDOW_X);
		fs.Write(Config::WINDOW_Y);
		return true;
	}

	bool Load(FileStream & fs)
	{
		uint32_t k = 0;
		fs.Read(k);
		fs.Read(k);
		return true;
	}
};

#include <Framework/File/FileManager.hpp>
#include <Framework\GameStructure\World\Nature\Meteorology2\MeteorologyUtils.hpp>

int main(int args, char *argv[])
{
	Celcius c = GetTempFromWindChill(10, 10, 0.0F, 5.65F);

	FileManager::Instance().Init();
	FileManager::Instance().SetWorkingDirectory(FileManager::UserLocation::DOCUMENTS, "NeuroTextAdventureEngine/Game/");

	GameSettingsSaveLoadListener listener;
	SaveManager::Instance().Init({ { WSID("gamesettings"), "gamesettings.dat", 512, &listener } }, 9991);
	SaveManager::Instance().RequestSave(WSID("gamesettings"));
	SaveManager::Instance().RequestLoad(WSID("gamesettings"));

	WindSystem sys = WindSystem({ { Vector2(-1,0),1000 },{ Vector2(1,0),1000 },{ Vector2(0,1),1000 },{ Vector2(0,-1),1000 } });
	WindEvent we = sys.GenerateNextWindEvent();
	we = sys.GenerateNextWindEvent();

	// set up our window and a few resources we need
	slWindow(Config::WINDOW_X, Config::WINDOW_Y, "ABC", false);

	Climatograph abc = Climatograph({ 40, 5, 40 }, { 25, 0, 25 });
	for (uint32_t p = 0; p < 9 * 64; ++p)
	{
		PRINT("TEMP = %f", abc.Evaluate(p / TimeOfDayCount, (TimeOfDay)(p%TimeOfDayCount)));
	}

	GraphicsManager::Instance().LoadFont(WSID("DEFAULT"), "res/Redressed.ttf");
	GraphicsManager::Instance().LoadTexture(WSID("tex_background"), "res/widows_peak_twilight.png");
	GraphicsManager::Instance().LoadTextureAsSpriteSheet(WSID("ITEMS"), "res/items.png", Vector2(3, 3));
	GraphicsManager::Instance().LoadTexture(WSID("BG_INVENTORY_0"), "res/inventory_bg0.png");
	GraphicsManager::Instance().LoadTextureAsSpriteSheet(WSID("WEATHER_ICONS"), "res/weather.png", Vector2(4));
	GraphicsManager::Instance().LoadTexture(WSID("Button"), "res/button.png");

	slSetFont(GraphicsManager::Instance().GetFont(WSID("DEFAULT")), 32);
	int tex = GraphicsManager::Instance().GetTexture(WSID("tex_background"));

	AudioManager::Instance().Init();
	GraphicsManager::Instance().Init();
	InputManager::Instance().Init();
	PageManager::Instance().Init();
	ItemRegistry::Instance().Init();
	FadeManager::Instance().Init();
	TimeFlowManager::Instance().Init();

	PageItemInventory p123(Player::Instance().GetBag(), "Pouch");
	//PageManager::Instance().Push(&p123);
	//Player::Instance().GetBag().StoreItem(WSID("broken_lantern"));
	//Player::Instance().GetBag().StoreItem(WSID("apple"));
	//Player::Instance().GetBag().StoreItem(WSID("apple"));
	//Player::Instance().GetBag().StoreItem(WSID("item2"));
	//Player::Instance().GetBag().StoreItem(WSID("item3"));
	//Player::Instance().GetBag().StoreItem(WSID("item4"));

	TimePoint tp1(TimeOfDay::EVENING, TimeFlowManager::DayInSeasonToDayInYear(Season::SUMMER, 1), 1823);
	TimeFlowManager::Instance().SetupInitialTime(tp1);


	bool isFirstUpdate = true;
	while (!slShouldClose() && !slGetKey(SL_KEY_ESCAPE))
	{
		if (isFirstUpdate)
		{
			isFirstUpdate = false;
			continue;
		}

		AudioManager::Instance().Tick(slGetDeltaTime());
		FadeManager::Instance().Tick(slGetDeltaTime());
		InputManager::Instance().Tick(slGetDeltaTime());
		PageManager::Instance().Tick(slGetDeltaTime());

		slRender();
	}
	slClose();

	PROFILE_DUMP_RECORDINGS;
	return 0;
}