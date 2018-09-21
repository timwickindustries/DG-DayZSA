void main()
{
	Hive ce = CreateHive();
	if (ce)
		ce.InitOffline();

	Weather weather = g_Game.GetWeather();

	weather.GetOvercast().SetLimits(0.0, 1.0);
	weather.GetRain().SetLimits(0.0, 1.0);
	weather.GetFog().SetLimits(0.0, 0.25);

	weather.GetOvercast().SetForecastChangeLimits(0.0, 0.2);
	weather.GetRain().SetForecastChangeLimits(0.0, 0.1);
	weather.GetFog().SetForecastChangeLimits(0.15, 0.45);

	weather.GetOvercast().SetForecastTimeLimits(1800, 1800);
	weather.GetRain().SetForecastTimeLimits(600, 600);
	weather.GetFog().SetForecastTimeLimits(1800, 1800);

	weather.GetOvercast().Set(Math.RandomFloatInclusive(0.0, 0.3), 0, 0);
	weather.GetRain().Set(Math.RandomFloatInclusive(0.0, 0.2), 0, 0);
	weather.GetFog().Set(Math.RandomFloatInclusive(0.0, 0.1), 0, 0);

	weather.SetWindMaximumSpeed(15);
	weather.SetWindFunctionParams(0.1, 0.3, 50);
}

class CustomMission: MissionServer
{
	override PlayerBase CreateCharacter(PlayerIdentity identity, vector pos, ParamsReadContext ctx, string characterName)
	{
		Entity playerEnt = GetGame().CreatePlayer(identity, characterName, pos, 0, "NONE");
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

	EntityAI m4Class(PlayerBase player)
	{
		EntityAI gun = player.GetHumanInventory().CreateInHands("M4A1");
		gun.GetInventory().CreateAttachment("M4_RISHndgrd_Black");
		gun.GetInventory().CreateAttachment("M4_MPBttstck_Black");
		gun.GetInventory().CreateAttachment("ACOGOptic");
		addMags(player, "Mag_STANAG_30Rnd", 7);

		return gun;
	}

	EntityAI akmClass(PlayerBase player)
	{
		EntityAI gun = player.GetHumanInventory().CreateInHands("AKM");
		gun.GetInventory().CreateAttachment("PSO1Optic");
		gun.GetInventory().CreateAttachment("AK_WoodBttstck");
		gun.GetInventory().CreateAttachment("AK_WoodHndgrd");
		addMags(player, "Mag_AKM_30Rnd", 7);

		return gun;
	}

	EntityAI smgClass(PlayerBase player)
	{
		EntityAI gun = player.GetHumanInventory().CreateInHands("UMP45");
		gun.GetInventory().CreateAttachment("PistolSuppressor");
		gun.GetInventory().CreateAttachment("ACOGOptic");
		addMags(player, "Mag_UMP_25Rnd", 10);

		return gun;
	}

	override void StartingEquipSetup(PlayerBase player, bool clothesChosen)
	{
		player.RemoveAllItems();

		EntityAI itemEnt;
		ItemBase itemBs;

		switch (Math.RandomInt(0, 4)) { 
		case 0:
		// USMC
		player.GetInventory().CreateInInventory("USMCPants_Woodland");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("USMCJacket_Woodland");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("CombatBoots_Black");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("AssaultBag_Ttsko");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("PlateCarrierBlank");
		itemBs = ItemBase.Cast(itemEnt); 
		break;

		case 1:
		// TTSKO
		player.GetInventory().CreateInInventory("TTSKOPants");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("TTsKOJacket_Camo");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("CombatBoots_Black");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("AssaultBag_Ttsko");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("PlateCarrierBlank");
		itemBs = ItemBase.Cast(itemEnt); 
		break;

		case 2:
		// Gorka
		player.GetInventory().CreateInInventory("GorkaPants_Autumn");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("GorkaEJacket_Autumn");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("CombatBoots_Black");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("AssaultBag_Green");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("PlateCarrierBlank");
		itemBs = ItemBase.Cast(itemEnt); 
		break;

		case 3:
		// OREL
		player.GetInventory().CreateInInventory("PolicePantsOrel");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("PoliceJacketOrel");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("CombatBoots_Black");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("AssaultBag_Black");
		itemBs = ItemBase.Cast(itemEnt);
		player.GetInventory().CreateInInventory("PlateCarrierBlank");
		itemBs = ItemBase.Cast(itemEnt); 
		break;

		}

		// Generic Items
		player.GetInventory().CreateInInventory("SodaCan_Pipsi");
		player.GetInventory().CreateInInventory("SpaghettiCan");
		player.GetInventory().CreateInInventory("HuntingKnife");
		player.GetInventory().CreateInInventory("SalineBag");
		player.GetInventory().CreateInInventory("StartKitIV");
		player.GetInventory().CreateInInventory("BandageDressing");
		player.GetInventory().CreateInInventory("BandageDressing");
		player.GetInventory().CreateInInventory("BandageDressing");
		player.GetInventory().CreateInInventory("Morphine");

		EntityAI primary;
		EntityAI axe = player.GetInventory().CreateInInventory("FirefighterAxe");

		switch (Math.RandomInt(0, 3)) {
			case 0: primary = m4Class(player); break;
			case 1: primary = akmClass(player); break;
			case 2: primary = smgClass(player); break;
		}

		player.LocalTakeEntityToHands(primary);
		player.SetQuickBarEntityShortcut(primary, 0, true);
		player.SetQuickBarEntityShortcut(axe, 1, true);
	}
};

Mission CreateCustomMission(string path)
{
	return new CustomMission();
}
