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
		addMags(player, "Mag_UMP_25Rnd", 7);

		return gun;
	}

	override void StartingEquipSetup(PlayerBase player, bool clothesChosen)
	{
		player.RemoveAllItems();

		player.GetInventory().CreateInInventory("USMCPants_Woodland");
		player.GetInventory().CreateInInventory("USMCJacket_Woodland");
		player.GetInventory().CreateInInventory("CombatBoots_Black");
		player.GetInventory().CreateInInventory("AssaultBag_Ttsko");
		player.GetInventory().CreateInInventory("PlateCarrierBlank");

		player.GetInventory().CreateInInventory("SodaCan_Pipsi");
		player.GetInventory().CreateInInventory("SpaghettiCan");
		player.GetInventory().CreateInInventory("HuntingKnife");
		player.GetInventory().CreateInInventory("Medical_TransfusionKit");
		ItemBase bandages = player.GetInventory().CreateInInventory("BandageDressing");
		bandages.SetQuantity(4);

		EntityAI primary;
		EntityAI axe = player.GetInventory().CreateInInventory("FirefighterAxe");

		switch (Math.RandomInt(0, 3)) {
			case 0: primary = m4Class(player); break;
			case 1: primary = akmClass(player); break;
			case 2: primary = smgClass(player); break;
		}

		player.LocalTakeEntityToHands(primary);
		player.SetQuickBarEntityShortcut(primary, 0, true);
		player.SetQuickBarEntityShortcut(axe, 3, true);
		player.SetQuickBarEntityShortcut(bandages, 4, true);
	}
};

Mission CreateCustomMission(string path)
{
	return new CustomMission();
}
