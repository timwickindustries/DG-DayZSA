
void main()
{

	Hive ce = CreateHive();
	if ( ce )
		ce.InitOffline();

	Weather weather = g_Game.GetWeather();

	weather.GetOvercast().SetLimits( 0.0 , 1.0 );
	weather.GetRain().SetLimits( 0.0 , 1.0 );
	weather.GetFog().SetLimits( 0.0 , 0.25 );

	weather.GetOvercast().SetForecastChangeLimits( 0.0, 0.2 );
	weather.GetRain().SetForecastChangeLimits( 0.0, 0.1 );
	weather.GetFog().SetForecastChangeLimits( 0.15, 0.45 );

	weather.GetOvercast().SetForecastTimeLimits( 1800 , 1800 );
	weather.GetRain().SetForecastTimeLimits( 600 , 600 );
	weather.GetFog().SetForecastTimeLimits( 1800 , 1800 );

	weather.GetOvercast().Set( Math.RandomFloatInclusive(0.0, 0.3), 0, 0);
	weather.GetRain().Set( Math.RandomFloatInclusive(0.0, 0.2), 0, 0);
	weather.GetFog().Set( Math.RandomFloatInclusive(0.0, 0.1), 0, 0);
	
	weather.SetWindMaximumSpeed(15);
	weather.SetWindFunctionParams(0.1, 0.3, 50);
}

class CustomMission: MissionServer
{	
	void SetRandomHealth(EntityAI itemEnt)
	{
		int rndHlt = Math.RandomInt(40,100);
		itemEnt.SetHealth("","",rndHlt);
	}

	override PlayerBase CreateCharacter(PlayerIdentity identity, vector pos, ParamsReadContext ctx, string characterName)
	{
		Entity playerEnt;
		playerEnt = GetGame().CreatePlayer(identity, characterName, pos, 0, "NONE");//Creates random player
		Class.CastTo(m_player, playerEnt);
		
		GetGame().SelectPlayer(identity, m_player);
		
		return m_player;
	}

	void addMags(PlayerBase player, string mag_type, int count)
	{
		if (count < 1)
			return;

		EntityAI mag;

		for (int i = 0; i < count; i++) {
			mag = player.GetInventory().CreateInInventory(mag_type);
		}

		player.SetQuickBarEntityShortcut(mag, 1, true);
	}

	EntityAI makarovClass(PlayerBase player)
	{
		EntityAI gun = player.GetHumanInventory().CreateInHands("MakarovIJ70");
		addMags(player, "Mag_IJ70_8Rnd", 2);

		return gun;
	}
	
	override void StartingEquipSetup(PlayerBase player, bool clothesChosen)
	{
		EntityAI itemEnt;
		ItemBase itemBs;

		switch (Math.RandomInt(0, 3)) { 
		
		case 0:
		player.GetInventory().CreateInInventory("ChildBag_Blue");
		itemBs = ItemBase.Cast(itemEnt);
		break;

		case 1:
		player.GetInventory().CreateInInventory("ChildBag_Green");
		itemBs = ItemBase.Cast(itemEnt);
		break;

		case 2:
		player.GetInventory().CreateInInventory("ChildBag_Red");
		itemBs = ItemBase.Cast(itemEnt);
		break;
		}

		player.GetInventory().CreateInInventory("HuntingKnife");
		player.GetInventory().CreateInInventory("SodaCan_Pipsi");
		player.GetInventory().CreateInInventory("BakedBeansCan");

		EntityAI primary;

		switch (Math.RandomInt(0, 1)) {
			case 0: primary = makarovClass(player); break;
		}

		player.LocalTakeEntityToHands(primary);

		player.SetQuickBarEntityShortcut(primary, 0, true);
		
		itemEnt = player.GetInventory().CreateInInventory("Rag");
		itemBs = ItemBase.Cast(itemEnt);
		itemBs.SetQuantity(4);
		SetRandomHealth(itemEnt);
	}
};
  
Mission CreateCustomMission(string path)
{
	return new CustomMission();
}