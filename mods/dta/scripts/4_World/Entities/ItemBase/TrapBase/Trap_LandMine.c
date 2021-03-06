class LandMineTrap extends TrapBase
{
	SoundOnVehicle m_TimerLoop;
	protected ref Timer m_DeleteTimer;
	
	void LandMineTrap()
	{
		m_DefectRate = 15;
		m_DamagePlayers = 0; 			//How much damage player gets when caught
		m_InitWaitTime = 10; 			//After this time after deployment, the trap is activated
		m_InfoActivationTime = "#STR_LandMineTrap0" + m_InitWaitTime.ToString() + "#STR_LandMineTrap1"; 
		
		m_AddDeactivationDefect = true;
	}
	
	/*override bool IsOneHandedBehaviour()
	{
		return true;
	}*/
	
	override void StartActivate( PlayerBase player )
	{
		super.StartActivate( player );
		
		if (GetGame().IsClient())
		{
			PlaySound("landmine_safetyPin", 3);
			m_TimerLoop = PlaySoundLoop("landmine_timer2", 3);
		}
	}
	
	override void OnActivate()
	{
		if (GetGame().IsClient())
		{
			PlaySound("landmine_ready", 3);
		}
		
		GetGame().ObjectDelete(m_TimerLoop);
		m_TimerLoop = NULL;
	}
	
	override bool CanExplodeInFire()
	{
		return true;
	}
	
	override void OnSteppedOn(EntityAI victim)
	{
		this.Explode();
		
		m_DeleteTimer = new Timer( CALL_CATEGORY_SYSTEM );
		m_DeleteTimer.Run( 2, this, "DeleteThis" );
	}
	
	void DeleteThis()
	{
		GetGame().ObjectDelete(this);
	}
	
	override void OnItemLocationChanged  ( EntityAI old_owner, EntityAI new_owner ) 
	{
		super.OnItemLocationChanged(old_owner, new_owner);
		
		if ( g_Game.IsServer() )
		{
			
		}
	}
	
	//================================================================
	// ADVANCED PLACEMENT
	//================================================================
		
	override void OnPlacementComplete( Man player ) 
	{
		super.OnPlacementComplete( player );
		
		if ( GetGame().IsServer() )
		{
			PlayerBase player_PB = PlayerBase.Cast( player );
			StartActivate( player_PB );
		}		
	}
	
	override bool IsDeployable()
	{
		return true;
	}
	
	override string GetLoopDeploySoundset()
	{
		return "landmine_deploy_SoundSet";
	}
}
