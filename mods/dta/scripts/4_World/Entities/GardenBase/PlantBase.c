class PlantBase extends ItemBase
{
	// Plant states
	static const int STATE_DRY 					= 0;
	static const int STATE_GROWING 				= 1;
	static const int STATE_MATURE 				= 2;
	static const int STATE_SPOILED 				= 3;

	private float 	m_SprayUsage; // How much spray is needed to stop infestation of plant
	
	private float 	m_InfestationChance;
	
	private int 	m_GrowthStagesCount; 
	private int 	m_CropsCount;
	private bool 	m_HasCrops;
	private string 	m_CropsType; 
	private float 	m_PlantMaterialMultiplier; 
	
	private int 	m_PlantState;
	private int 	m_PlantStateIndex;
	private float 	m_CurrentPlantMaterialQuantity;
	
	private bool 	m_IsInfested;
	private float 	m_SprayQuantity;
	
	int 	m_DeleteDryPlantTime; 			// For how long in seconds can an unwatered plant exist before it disappears
	int 	m_SpoiledRemoveTime;			// For how long in seconds a spoiled plant will exist
	int 	m_FullMaturityTime;				// How much time needs plant to be full grown in seconds
	int 	m_SpoilAfterFullMaturityTime;	// How long in seconds it takes for plant to be spoiled after it is full grown
	int 	m_StateChangeTime;				// For how long in seconds will plant stay in one state before its going to next state
	
    ref Timer m_GrowthTimer = NULL;
    ref Timer m_InfestationTimer = NULL;
    ref Timer m_SpoilAfterFullMaturityTimer = NULL;
    ref Timer m_SpoiledRemoveTimer = NULL;
	ref Timer m_DeleteDryPlantTimer = NULL;
	
	private GardenBase m_GardenBase = NULL;
	private ref Slot m_Slot = NULL;
	
	private PluginHorticulture m_ModuleHorticulture;
	
	void PlantBase()
	{
		m_ModuleHorticulture = PluginHorticulture.Cast( GetPlugin( PluginHorticulture ) );
		
		m_SprayUsage = 5;
		m_DeleteDryPlantTime = (60 * 10) + Math.RandomInt(0, 60 * 2);
		m_SpoiledRemoveTime = (60 * 20) + Math.RandomInt(0, 60 * 5);
		
		m_InfestationChance = 0.0; // Temporarily disabled until its fixed. Infestation is not visualy persistent over server restarts and m_SpoiledRemoveTimer crashes when it's meant to delete the plant.
		
		string plant_type = this.GetType();
		m_GrowthStagesCount = GetGame().ConfigGetInt( "cfgVehicles " + plant_type + " Horticulture GrowthStagesCount" );
		m_CropsCount = GetGame().ConfigGetInt( "cfgVehicles " + plant_type + " Horticulture CropsCount" );
		GetGame().ConfigGetText( "cfgVehicles " + plant_type + " Horticulture CropsType", m_CropsType );

		m_PlantStateIndex = -1;
		m_CurrentPlantMaterialQuantity = 0;
		m_IsInfested = false;
		m_SprayQuantity = 0.0;
		m_HasCrops = true;
		
		
		RegisterNetSyncVariableBool("m_HasCrops");
		RegisterNetSyncVariableInt("m_PlantState");
		RegisterNetSyncVariableInt("m_PlantStateIndex");
	}

	void ~PlantBase()
	{
		RemovePlant();
	}
	
	void Init( GardenBase garden_base, float fertility, float harvesting_efficiency, float water )
	{
		m_GardenBase = garden_base;
		
		float divided = (float) ((60 * 27) + Math.RandomInt(0, 60 * 6)) / fertility;
		Print(divided);
		m_FullMaturityTime = divided;
		
		//divided = (float)((60 * 30) + Math.RandomInt(0, 60 * 30)) * fertility;
		m_SpoilAfterFullMaturityTime = divided*5;

		divided = (float)((float)m_FullMaturityTime / ((float)m_GrowthStagesCount - 2.0));
		m_StateChangeTime = divided;

		float count = m_CropsCount * fertility * harvesting_efficiency;
		m_CropsCount = (int)Math.Ceil( count );

		m_PlantMaterialMultiplier = 0.1 * harvesting_efficiency;
		
		float rain_intensity = GetGame().GetWeather().GetRain().GetActual();
		if ( rain_intensity > 0.0 )
		{
			CheckWater();
		}
		else
		{
			CheckWater();
			
			if ( NeedsWater() )
			{
				SetPlantState(STATE_DRY);
				
				if (GetGame().IsServer())
				{
					m_DeleteDryPlantTimer = new Timer( CALL_CATEGORY_SYSTEM );
					m_DeleteDryPlantTimer.Run( m_DeleteDryPlantTime, this, "DeleteDryPlantTick", NULL, false );
				}
			}
		}
	}

	
	override void OnStoreLoad( ParamsReadContext ctx, int version )
	{
		super.OnStoreLoad( ctx, version );
		
		Print("Plant - OnStoreLoad - ");
		
		GardenBase garden = GardenBase.Cast( GetHierarchyParent() );
		Print(garden);
		
		int slot_index = -1;
		ctx.Read( slot_index );
		
		Print(slot_index);
		
		Slot slot = garden.GetSlotByIndex(slot_index);
		Print(slot);
		
		SetSlot(slot);
		
		OnStoreLoadCustom( ctx );
	}

	override void OnStoreSave( ParamsWriteContext ctx )
	{
		super.OnStoreSave( ctx );
		
		Print("Plant - OnStoreSave - ");
		Slot slot = GetSlot();
		Print(slot);
		if (slot)
		{
			int slot_index = slot.GetSlotIndex();
			Print(slot_index);
			ctx.Write( slot_index );
			
			OnStoreSaveCustom( ctx );
		}
		else
		{
			GetGame().ObjectDelete(this); // Plants that exist without a garden must be deleted. Otherwise they might cause problems.
			Print("Warning! A plant existed without a garden. Therefore it was deleted from the world to prevent issues!");
		}
	}
	
	void OnStoreLoadCustom( ParamsReadContext ctx )
	{
		int loadInt;
		ctx.Read( loadInt );
		m_SprayUsage = loadInt;
		
		loadInt = 0;
		ctx.Read( loadInt );
		m_DeleteDryPlantTime = loadInt;
		
		loadInt = 0;
		ctx.Read( loadInt );
		m_SpoiledRemoveTime = loadInt;
		
		loadInt = 0;
		ctx.Read( loadInt );
		m_FullMaturityTime = loadInt;
		
		loadInt = 0;
		ctx.Read( loadInt );
		m_SpoilAfterFullMaturityTime = loadInt;
		
		loadInt = 0;
		ctx.Read( loadInt );
		m_StateChangeTime = loadInt;
		
		float loadFloat = 0.0;
		ctx.Read( loadFloat );
		m_InfestationChance = loadFloat;
		
		loadInt = 0;
		ctx.Read( loadInt );
		m_GrowthStagesCount = loadInt;
		
		loadInt = 0;
		ctx.Read( loadInt );
		m_CropsCount = loadInt;
		
		string loadString = "";
		ctx.Read( loadString );
		m_CropsType = loadString;
		
		loadFloat = 0.0;
		ctx.Read( loadFloat );
		m_PlantMaterialMultiplier = loadFloat;
		
		loadInt = 0;
		ctx.Read( loadInt );
		m_PlantState = loadInt;
		
		loadInt = 0;
		ctx.Read( loadInt );
		m_PlantStateIndex = loadInt;
		
		loadFloat = 0.0;
		ctx.Read( loadFloat );
		m_CurrentPlantMaterialQuantity = loadFloat;
		
		bool loadBool = false;
		ctx.Read( loadBool );
		m_IsInfested = loadBool;
		 
		loadFloat = 0.0;
		ctx.Read( loadFloat );
		m_SprayQuantity = loadFloat;
		
		
		
		loadBool = false;
		ctx.Read( loadBool );
		if ( loadBool )
		{
			if (GetGame().IsServer())
			{
				m_GrowthTimer = new Timer( CALL_CATEGORY_SYSTEM );
				m_GrowthTimer.Run( m_StateChangeTime, this, "GrowthTimerTick", NULL, true );
			}
		}
		
		loadFloat = 0.0;
		ctx.Read( loadFloat );
		if ( loadFloat > 0.0 )
		{
			if (GetGame().IsServer())
			{
				m_InfestationTimer = new Timer( CALL_CATEGORY_SYSTEM );
				m_InfestationTimer.Run( loadFloat, this, "InfestationTimerTick", NULL, false );
			}
		}
		
		loadFloat = 0.0;
		ctx.Read( loadFloat );
		if ( loadFloat > 0.0 )
		{
			if (GetGame().IsServer())
			{
				m_SpoilAfterFullMaturityTimer = new Timer( CALL_CATEGORY_SYSTEM );
				m_SpoilAfterFullMaturityTimer.Run( loadFloat, this, "SetSpoiled", NULL, false );
			}
		}
		
		loadFloat = 0.0;
		ctx.Read( loadFloat );
		if ( loadFloat > 0.0 )
		{
			if (GetGame().IsServer())
			{
				if (!m_SpoiledRemoveTimer)
					m_SpoiledRemoveTimer = new Timer( CALL_CATEGORY_SYSTEM );
				
				m_SpoiledRemoveTimer.Run( loadFloat, this, "SpoiledRemoveTimerTick", NULL, false );
			}
		}
		
		loadFloat = 0.0;
		ctx.Read( loadFloat );
		if ( loadFloat > 0.0 )
		{
			if (GetGame().IsServer())
			{
				m_DeleteDryPlantTimer = new Timer( CALL_CATEGORY_SYSTEM );
				m_DeleteDryPlantTimer.Run( loadFloat, this, "DeleteDryPlantTick", NULL, false );
			}
		}
		
		UpdatePlant();
	}

	void OnStoreSaveCustom( ParamsWriteContext ctx )
	{
		ctx.Write( m_SprayUsage );
		
		ctx.Write( m_DeleteDryPlantTime );
		
		ctx.Write( m_SpoiledRemoveTime );
		
		ctx.Write( m_FullMaturityTime );
		
		ctx.Write( m_SpoilAfterFullMaturityTime );
		
		ctx.Write( m_StateChangeTime );
		
		ctx.Write( m_InfestationChance );
		
		ctx.Write( m_GrowthStagesCount );
		
		ctx.Write( m_CropsCount );
		
		ctx.Write( m_CropsType );
		
		ctx.Write( m_PlantMaterialMultiplier );
		
		ctx.Write( m_PlantState );
		
		ctx.Write( m_PlantStateIndex );
		
		ctx.Write( m_CurrentPlantMaterialQuantity );
		
		ctx.Write( m_IsInfested );
		 
		ctx.Write( m_SprayQuantity );
		
		bool saveBool = false;
		if ( m_GrowthTimer != NULL )
		{
			saveBool = true;
		}
		ctx.Write( saveBool );
		
		float saveFloat = 0.0;
		if ( m_InfestationTimer != NULL )
		{
			saveFloat = m_InfestationTimer.GetRemaining();
		}
		ctx.Write( saveFloat );
		
		saveFloat = 0.0;
		if ( m_SpoilAfterFullMaturityTimer != NULL )
		{
			saveFloat = m_SpoilAfterFullMaturityTimer.GetRemaining();
		}
		ctx.Write( saveFloat );
		
		saveFloat = 0.0;
		if ( m_SpoiledRemoveTimer != NULL )
		{
			saveFloat = m_SpoiledRemoveTimer.GetRemaining();
		}
		ctx.Write( saveFloat );
		
		saveFloat = 0.0;
		if ( m_DeleteDryPlantTimer != NULL )
		{
			saveFloat = m_DeleteDryPlantTimer.GetRemaining();
		}
		ctx.Write( saveFloat );
	}
	
	void PrintValues()
	{
		Print("PRINT ALL VALUES OF PLANT...");
		Print(this);
		
		Print(m_HasCrops);
		Print(m_PlantState);
		Print(m_PlantStateIndex);
		Print(m_CurrentPlantMaterialQuantity);
		Print(m_IsInfested);
		Print(m_SprayQuantity);
		Print(m_Slot);
		Print(m_GardenBase);
		Print("----------------------------------------------------------");
	}
	
	override bool CanPutInCargo( EntityAI parent )
	{
		if( !super.CanPutInCargo(parent) ) {return false;}
		return false;
	}

	override bool CanPutIntoHands( EntityAI player )
	{
		if( !super.CanPutIntoHands( parent ) )
		{
			return false;
		}
		return false;
	}

	override bool CanRemoveFromHands( EntityAI player )
	{
		return false;
	}

	void ChangeInfestation( bool is_infested )
	{
		m_IsInfested = is_infested;
		
		string plant_type = GetType();
		PlantMaterialHealth material = m_ModuleHorticulture.GetPlantMaterial( plant_type );
		
		if ( m_IsInfested )
		{
			if ( material.m_InfestedTex != "" )
			{
				SetObjectTexture( 0, material.m_InfestedTex );
			}
			if ( material.m_InfestedMat != "" )
			{
				SetObjectMaterial( 0, material.m_InfestedMat );
			}
		}
		else 
		{
			if ( material.m_HealthyTex != "" )
			{
				SetObjectTexture( 0, material.m_HealthyTex );
			}
			if ( material.m_HealthyMat != "" )
			{
				SetObjectMaterial( 0, material.m_HealthyMat );
			}
		}
	}

	void UpdatePlant()
	{
		if ( m_PlantStateIndex > 0 )
		{
			string plant_state_index = m_PlantStateIndex.ToStringLen(2); 
			string prev_plant_state_index = ( m_PlantStateIndex - 1 ).ToStringLen( 2 );
			
			// HIDING PREVIOUS PLANT STATE AND SHOWING THE CURRENT ONE
			ShowSelection( "plantStage_" + plant_state_index ); // SHOW!
			HideSelection( "plantStage_" + prev_plant_state_index ); // HIDE!
			
			// HIDING PREVIOUS CROPS STATE AND SHOWING THE CURRENT ONE
			
			if ( HasCrops() )
			{
				ShowSelection( "plantStage_" + plant_state_index + "_crops" ); // SHOW!
				HideSelection( "plantStage_" + prev_plant_state_index + "_crops" ); // HIDE!
			}
			else
			{
				HideSelection( "plantStage_" + plant_state_index + "_crops" ); // HIDE!
				HideSelection( "plantStage_" + prev_plant_state_index + "_crops" ); // HIDE!
			}

			// HIDING PREVIOUS SHADOW STATE AND SHOWING THE CURRENT ONE
			ShowSelection( "plantStage_" + plant_state_index + "_shadow" ); // SHOW!
			HideSelection( "plantStage_" + prev_plant_state_index + "_shadow" ); // HIDE!
		}
		
		float float_plant_state_index = (float)m_PlantStateIndex;
		m_CurrentPlantMaterialQuantity = m_PlantMaterialMultiplier * float_plant_state_index;
	}

	void GrowthTimerTick()
	{
		if ( IsGrowing() )
		{
			if ( m_PlantStateIndex < m_GrowthStagesCount - 2 )
			{
				m_PlantStateIndex++;
				UpdatePlant();
				SetSynchDirty();
					
				if ( m_PlantStateIndex == 0 )
				{
					float infestation_time_min = (float)m_FullMaturityTime * 0.2;
					int int_infestation_time_min = (int)infestation_time_min;
					
					float infestation_time_max = (float)m_FullMaturityTime * 0.6;
					int int_infestation_time_max = (int)infestation_time_max;
					
					if (GetGame().IsServer())
					{
						if (!m_InfestationTimer)
							m_InfestationTimer = new Timer( CALL_CATEGORY_SYSTEM );
						
						m_InfestationTimer.Run( Math.RandomInt(int_infestation_time_min, int_infestation_time_max), this, "InfestationTimerTick", NULL, false );
					}
				}
					
				if ( m_PlantStateIndex == m_GrowthStagesCount - 2 )
				{
					if ( m_IsInfested )
					{
						SetSpoiled();
					}
					else
					{
						SetPlantState(STATE_MATURE);
					}
				}
			}
		}
		else if ( IsMature() )
		{
			if (GetGame().IsServer())
			{
				if (!m_SpoilAfterFullMaturityTimer)
					m_SpoilAfterFullMaturityTimer = new Timer( CALL_CATEGORY_SYSTEM );
				
				m_SpoilAfterFullMaturityTimer.Run( m_SpoilAfterFullMaturityTime, this, "SetSpoiled", NULL, false );
			}
		}
	}

	void InfestationTimerTick()
	{
		float infestation_rnd = Math.RandomFloat01();
		
		if ( m_InfestationChance > infestation_rnd )
		{
			ChangeInfestation( true );
		}
	}

	void SpoiledRemoveTimerTick()
	{
		if ( m_GrowthTimer != NULL )
		{
			m_GrowthTimer.Stop();
		}
		
		RemoveSlot();
	}
	
	void DeleteDryPlantTick()
	{
		/*if ( IsDry() )
		{
			RemoveSlot();
		}*/
	}

	void SetSpoiled()
	{
		if ( IsSpoiled() == false )
		{
			m_PlantStateIndex++;
			SetPlantState(STATE_SPOILED);
			UpdatePlant();
			SetSynchDirty();
			
			if (GetGame().IsServer())
			{
				if (!m_SpoiledRemoveTimer)
					m_SpoiledRemoveTimer = new Timer( CALL_CATEGORY_SYSTEM );
				
				if (!m_SpoiledRemoveTimer.IsRunning())
					m_SpoiledRemoveTimer.Run( m_SpoiledRemoveTime, this, "SpoiledRemoveTimerTick", NULL, false );
			}
		}
	}

	void CheckWater()
	{
		if ( !IsMature()  &&  !NeedsWater() )
		{
			if ( m_DeleteDryPlantTimer )
			{
				m_DeleteDryPlantTimer.Stop();
			}
			
			SetPlantState(STATE_GROWING);
			
			if (GetGame().IsServer())
			{
				m_GrowthTimer = new Timer( CALL_CATEGORY_SYSTEM );
				m_GrowthTimer.Run( m_StateChangeTime, this, "GrowthTimerTick", NULL, true );
			}
		}
	}

	string StopInfestation( float consumed_quantity )
	{
		m_SprayQuantity += consumed_quantity;
		
		if ( !NeedsSpraying() )
		{
			if ( m_InfestationTimer != NULL )
			{
				m_InfestationTimer.Stop();
			}
			
			m_IsInfested = false;
			m_InfestationChance = 0;
			
			ChangeInfestation( false );
			UpdatePlant();
			
			return "I've sprayed the plant a bit. Now it is enough spayed.";
		}
		else
		{
			return "I've sprayed the plant a bit.";
		}
	}

	void RemovePlant()
	{
		if ( GetGame()  &&  GetGame().IsServer() )
		{
			UnlockFromParent();
			
			if ( m_CurrentPlantMaterialQuantity > 0.0 )
			{
				vector pos = GetPosition();
				ItemBase item = ItemBase.Cast( GetGame().CreateObject( "PlantMaterial", pos ) );
				item.SetQuantity( m_CurrentPlantMaterialQuantity * 1000.0 );
			}
			
			RemoveSlot();
		}
	}

	void Harvest( PlayerBase player )
	{
		//TODO Boris: Add soft skill 2.0
		//PluginExperience module_exp = GetPlugin(PluginExperience);
		//float harvesting_efficiency = module_exp.GetExpParamNumber(player, PluginExperience.EXP_FARMER_HARVESTING, "efficiency");
		
		//m_CropsCount = m_CropsCount * harvesting_efficiency;
		
		for ( int i = 0; i < m_CropsCount; i++ )
		{
			vector pos = player.GetPosition();
			ItemBase item = ItemBase.Cast( GetGame().CreateObject( m_CropsType, pos ) );
			item.SetQuantity( item.GetQuantityMax() );
			
			if ( IsSpoiled() )
			{
				Edible_Base food_item = Edible_Base.Cast( item );
				if ( food_item )
				{
					food_item.ChangeFoodStage( FoodStageType.ROTTEN );
				}
			}
		}
		
		m_HasCrops = false;
		SetSynchDirty();
		UpdatePlant();
	}
	
	void SetPlantState(int state)
	{
		m_PlantState = state;
		SetSynchDirty();
	}
	
	int GetPlantState()
	{
		return m_PlantState;
	}
	
	float GetWater()
	{
		if ( GetSlot() )
			return GetSlot().GetWater();

		return 0;
	}
	
	float GetWaterMax()
	{
		if ( GetSlot() )
			return GetSlot().GetWaterUsage();

		return 0;
	}
	
	bool NeedsWater()
	{
		Slot slotPlant = m_Slot;
		
		if ( IsDry()  &&  slotPlant  &&  slotPlant.GetWater() < slotPlant.GetWaterUsage() )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool NeedsSpraying()
	{
		if ( m_SprayQuantity < m_SprayUsage )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	float GetSprayQuantity()
	{
		return m_SprayQuantity;
	}	

	float GetSprayUsage()
	{
		return m_SprayUsage;
	}	
	
	void RemoveSlot()
	{
		if ( m_GardenBase )
		{
			if (m_SpoiledRemoveTimer)
			{
				m_SpoiledRemoveTimer.Stop();
				m_SpoiledRemoveTimer = NULL;
			}
			
			m_GardenBase.RemoveSlotPlant( this );
		}
	}
	
	void SetSlot(Slot slot)
	{
		if ( slot )
		{
			m_Slot = slot;
		}
	}
	
	Slot GetSlot()
	{
		return m_Slot;
	}

	bool IsDry()
	{
		if ( GetPlantState() == STATE_DRY )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool IsGrowing()
	{
		if ( GetPlantState() == STATE_GROWING )
		{
			return true;
		}
		else
		{
			return false;
		}
	}


	bool IsMature()
	{
		if ( GetPlantState() == STATE_MATURE )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool IsSpoiled()
	{
		if ( GetPlantState() == STATE_SPOILED )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
	bool HasCrops()
	{
		return m_HasCrops;
	}
}