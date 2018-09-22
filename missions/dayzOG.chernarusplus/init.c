
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
		player.RemoveAllItems();

		EntityAI itemEnt;
		ItemBase itemBs;

		switch (Math.RandomInt(0, 7)) { 
			case 0:
			player.GetInventory().CreateInInventory("TShirt_Beige");
			itemBs = ItemBase.Cast(itemEnt);
			break;

			case 1:
			player.GetInventory().CreateInInventory("TShirt_Black");
			itemBs = ItemBase.Cast(itemEnt);
			break;

			case 2:
			player.GetInventory().CreateInInventory("TShirt_Blue");
			itemBs = ItemBase.Cast(itemEnt);
			break;

			case 3:
			player.GetInventory().CreateInInventory("TShirt_Green");
			itemBs = ItemBase.Cast(itemEnt);
			break;

			case 4:
			player.GetInventory().CreateInInventory("TShirt_Grey");
			itemBs = ItemBase.Cast(itemEnt);
			break;

			case 5:
			player.GetInventory().CreateInInventory("TShirt_Red");
			itemBs = ItemBase.Cast(itemEnt);
			break;

			case 6:
			player.GetInventory().CreateInInventory("TShirt_White");
			itemBs = ItemBase.Cast(itemEnt);
			break;
		}

		switch (Math.RandomInt(0, 6)) { 
			case 0:
			player.GetInventory().CreateInInventory("Jeans_Black");
			itemBs = ItemBase.Cast(itemEnt);
			break;

			case 1:
			player.GetInventory().CreateInInventory("Jeans_Blue");
			itemBs = ItemBase.Cast(itemEnt);
			break;

			case 2:
			player.GetInventory().CreateInInventory("Jeans_BlueDark");
			itemBs = ItemBase.Cast(itemEnt);
			break;

			case 3:
			player.GetInventory().CreateInInventory("Jeans_Brown");
			itemBs = ItemBase.Cast(itemEnt);
			break;

			case 4:
			player.GetInventory().CreateInInventory("Jeans_Green");
			itemBs = ItemBase.Cast(itemEnt);
			break;

			case 5:
			player.GetInventory().CreateInInventory("Jeans_Grey");
			itemBs = ItemBase.Cast(itemEnt);
			break;
		}

		switch (Math.RandomInt(0, 5)) { 
			case 0:
			player.GetInventory().CreateInInventory("AthleticShoes_Black");
			itemBs = ItemBase.Cast(itemEnt);
			break;

			case 1:
			player.GetInventory().CreateInInventory("AthleticShoes_Blue");
			itemBs = ItemBase.Cast(itemEnt);
			break;

			case 2:
			player.GetInventory().CreateInInventory("AthleticShoes_Brown");
			itemBs = ItemBase.Cast(itemEnt);
			break;

			case 3:
			player.GetInventory().CreateInInventory("AthleticShoes_Green");
			itemBs = ItemBase.Cast(itemEnt);
			break;

			case 4:
			player.GetInventory().CreateInInventory("AthleticShoes_Grey");
			itemBs = ItemBase.Cast(itemEnt);
			break;
		}

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

		switch (Math.RandomInt(0, 3)) { 
			case 0:
			player.GetInventory().CreateInInventory("HuntingKnife");
			itemBs = ItemBase.Cast(itemEnt);
			break;

			case 1:
			player.GetInventory().CreateInInventory("KitchenKnife");
			itemBs = ItemBase.Cast(itemEnt);
			break;

			case 2:
			player.GetInventory().CreateInInventory("SteakKnife");
			itemBs = ItemBase.Cast(itemEnt);
			break;
		}

		switch (Math.RandomInt(0, 4)) { 
			case 0:
			player.GetInventory().CreateInInventory("SodaCan_Pipsi");
			itemBs = ItemBase.Cast(itemEnt);
			break;

			case 1:
			player.GetInventory().CreateInInventory("SodaCan_Cola");
			itemBs = ItemBase.Cast(itemEnt);
			break;

			case 2:
			player.GetInventory().CreateInInventory("SodaCan_Kvass");
			itemBs = ItemBase.Cast(itemEnt);
			break;

			case 3:
			player.GetInventory().CreateInInventory("SodaCan_Spite");
			itemBs = ItemBase.Cast(itemEnt);
			break;
		}

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