class AttachmentCategoriesRow: ClosableContainer
{
	protected string									m_CategoryIdentifier;
	protected ref array<ref SlotsContainer>				m_Ics;
	protected ref map<EntityAI, ref IconsContainer>		m_AttachmentCargoGrids;
	protected ref map<EntityAI, ref UICargoGrid>		m_AttachmentCargos;
	
	void AttachmentCategoriesRow( LayoutHolder parent, int sort = -1 )
	{
		ClosableHeader header	= ClosableHeader.Cast( m_Body.Get( 0 ) );
		m_Ics					= new array<ref SlotsContainer>;
		m_AttachmentCargoGrids	= new map<EntityAI, ref IconsContainer>;
		m_AttachmentCargos		= new map<EntityAI, ref UICargoGrid>;
		
		header.GetMainWidget().ClearFlags( WidgetFlags.DRAGGABLE );
	}
	
	string GetCategoryIdentifier()
	{
		return m_CategoryIdentifier;
	}
	
	EntityAI GetFocusedEntity()
	{
		if( m_FocusedRow < m_Ics.Count() )
		{
			ItemPreviewWidget ipw = ItemPreviewWidget.Cast( m_Ics.Get( m_FocusedRow ).GetMainWidget().FindAnyWidget( "Render" + m_FocusedColumn ) );
			return ipw.GetItem();
		}
		return null;
	}
	
	int GetFocusedSlot()
	{
		if( m_FocusedRow < m_Ics.Count() )
		{
			ItemPreviewWidget ipw = ItemPreviewWidget.Cast( m_Ics.Get( m_FocusedRow ).GetMainWidget().FindAnyWidget( "Render" + m_FocusedColumn ) );
			return ipw.GetUserID();
		}
		return -1;
	}
	
	override void SelectItem()
	{
		if( m_FocusedRow < m_Ics.Count() )
		{
			ItemBase item = ItemBase.Cast( GetFocusedEntity() );
			ItemManager.GetInstance().SetSelectedItem( item, null, m_Ics.Get( m_FocusedRow ).GetMainWidget().FindAnyWidget( "Cursor" + m_FocusedColumn ) );
		}
	}
	
	override void Select()
	{
		ItemBase prev_item = ItemBase.Cast( GetFocusedEntity() );
		Man player = GetGame().GetPlayer();
		
		if( ItemManager.GetInstance().IsItemMoving() )
		{
			ItemBase selected_item = ItemBase.Cast( ItemManager.GetInstance().GetSelectedItem() );
			int selected_slot = GetFocusedSlot();
			int stack_max;
			if( selected_item && selected_item.GetInventory().CanRemoveEntity() )
			{
				if( m_Entity.GetInventory().CanAddAttachmentEx( selected_item, selected_slot ) )
				{
					stack_max = InventorySlots.GetStackMaxForSlotId( selected_slot );
					float quantity = selected_item.GetQuantity();
					if( stack_max == 0 || stack_max >= quantity || !selected_item.CanBeSplit() )
					{
						player.PredictiveTakeEntityToTargetAttachmentEx( m_Entity, selected_item, selected_slot );
					}
					else
					{
						selected_item.SplitIntoStackMaxClient( m_Entity, selected_slot );
					}
				}
				else if( selected_slot != -1 )
				{
					if( prev_item )
					{
						InventoryLocation inv_loc = new InventoryLocation;
						prev_item.GetInventory().GetCurrentInventoryLocation( inv_loc );
						stack_max = InventorySlots.GetStackMaxForSlotId( inv_loc.GetSlot() );
						quantity = prev_item.GetQuantity();
						if( prev_item.CanBeCombined( ItemBase.Cast( selected_item ) ) )
						{
							prev_item.CombineItemsClient( selected_item, true );
						}
						else if( stack_max == 0 && GameInventory.CanSwapEntities( prev_item, selected_item ) )
						{
							player.PredictiveSwapEntities( selected_item, prev_item );
						}
						else if( m_AttachmentCargos.Contains( prev_item ) )
						{
							if( prev_item.GetInventory().CanAddEntityInCargo( selected_item ) )
							{
								TakeIntoCargo( PlayerBase.Cast( player ), prev_item, selected_item );
							}
						}
					}
					else
					{
						InventoryLocation inv_loc_src = new InventoryLocation;
						InventoryLocation inv_loc_dst = new InventoryLocation;
						selected_item.GetInventory().GetCurrentInventoryLocation( inv_loc_src );
						m_Entity.GetInventory().FindFreeLocationFor( selected_item, FindInventoryLocationType.ATTACHMENT, inv_loc_dst );
						
						if( inv_loc_dst.IsValid() && inv_loc_dst.GetType() == InventoryLocationType.ATTACHMENT )
						{
							stack_max = InventorySlots.GetStackMaxForSlotId( inv_loc_dst.GetSlot() );
							quantity = selected_item.GetQuantity();
							if( stack_max == 0 || stack_max >= quantity || !selected_item.CanBeSplit() )
							{
								GetGame().GetPlayer().PredictiveTakeEntityToTargetAttachmentEx( m_Entity, selected_item, inv_loc_dst.GetSlot() );
							}
							else if( stack_max >= 0 || !selected_item.CanBeSplit() )
							{
								selected_item.SplitIntoStackMaxClient( m_Entity, inv_loc_dst.GetSlot() );
							}
						}
					}
				}
				else
				{
					Icon selected_icon = ItemManager.GetInstance().GetSelectedIcon();
					if( selected_icon )
					{
						selected_icon.SetActive( false );
					}
					Widget selected_widget = ItemManager.GetInstance().GetSelectedWidget();
					if( selected_widget )
					{
						selected_widget.Show( false );
					}
				}
				
				if( m_Parent.IsInherited( PlayerContainer ) )
				{
					PlayerContainer player_container_parent = PlayerContainer.Cast( m_Parent );
					player_container_parent.UnfocusPlayerAttachmentsContainer();
				}
			}
		}
		else
		{
			if( prev_item && prev_item.GetInventory().CanRemoveEntity() )
			{
				EntityAI item_in_hands = GetGame().GetPlayer().GetHumanInventory().GetEntityInHands();
				if( item_in_hands && item_in_hands.GetInventory().CanRemoveEntity() )
				{
					if( GameInventory.CanSwapEntities( item_in_hands, prev_item ) )
					{
						player.PredictiveSwapEntities( item_in_hands, prev_item );
					}
				}
				else
				{
					if( player.GetHumanInventory().CanAddEntityInHands( prev_item ) )
					{
						player.PredictiveTakeEntityToHands( prev_item );
					}
				}
			}		
		}
	}
	
	override void TransferItem()
	{
		EntityAI entity = GetFocusedEntity();
		if( entity )
		{
			PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
			GetGame().GetPlayer().PredictiveTakeEntityToInventory( FindInventoryLocationType.CARGO, entity );
		}
	}
	
	void TransferItemToVicinity()
	{
		EntityAI item = GetFocusedEntity();
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		player.PredictiveDropEntity( item );
	}
	
	void SetDefaultFocus()
	{
		m_FocusedRow = 0;
		m_FocusedColumn = 0;
		if( m_FocusedRow < m_Ics.Count() )
		{
			m_Ics.Get( 0 ).GetMainWidget().FindAnyWidget( "Cursor" + 0 ).Show( true );
			EntityAI focused_item = GetFocusedEntity();
			if( focused_item )
			{
				float x, y;
				m_Ics.Get( m_FocusedRow ).GetMainWidget().FindAnyWidget( "Cursor" + m_FocusedColumn ).GetScreenPos( x, y );
				ItemManager.GetInstance().PrepareTooltip( focused_item, x, y );
			}
		}
	}
	
	override bool IsLastContainerFocused()
	{
		return true;
	}
	
	override void MoveGridCursor( int direction )
	{
		ItemManager.GetInstance().HideTooltip();
		UnfocusAll();
		int max_row;
		Container cnt;
		UICargoGrid grid;
		if( direction == Direction.UP )
		{
			m_FocusedRow--;
			if( m_FocusedRow < 0 )
			{
				m_FocusedRow  = m_Ics.Count() - 1;
				cnt = Container.Cast( m_Parent.GetParent().GetParent() );
				if( cnt )
					{
						cnt.SetPreviousActive();
					}
					else
					{
						cnt = Container.Cast( m_Parent );
						cnt.SetPreviousActive();
					}
				return;
				
			}
			
			if( m_FocusedRow < m_Ics.Count() )
			{
				max_row = m_Ics.Get( m_FocusedRow ).GetColumnCount() - 1;
				if( max_row < m_FocusedColumn )
				{
					m_FocusedColumn = max_row;
				}
			}
			else if( m_FocusedRow < m_Ics.Count() + m_AttachmentCargos.Count() )
			{
				m_AttachmentCargos.GetElement( m_FocusedRow - m_Ics.Count() ).MoveGridCursor( direction );
			}
		}
	
		if( direction == Direction.DOWN )
		{
			m_FocusedRow++;
			if( m_FocusedRow == m_Ics.Count() + m_AttachmentCargos.Count() )
			{
				m_FocusedRow = 0;
				cnt = Container.Cast( m_Parent.GetParent().GetParent().GetParent() );
				if( cnt )
				{
					cnt.SetNextActive();
				}
				else
				{
					cnt = Container.Cast( m_Parent );
					cnt.SetNextActive();
				}
				return;
			}
			
			if( m_FocusedRow < m_Ics.Count() )
			{
				max_row = m_Ics.Get( m_FocusedRow ).GetColumnCount() - 1;
				if( max_row < m_FocusedColumn )
				{
					m_FocusedColumn = max_row;
				}
			}
			else if( m_FocusedRow < m_Ics.Count() + m_AttachmentCargos.Count() )
			{
				grid = m_AttachmentCargos.GetElement( m_FocusedRow - m_Ics.Count() );
				if( grid.IsActive() )
					grid.MoveGridCursor( direction );
				else
				{
					if( m_FocusedRow - 1 < m_Ics.Count() )
					{
						m_Ics.Get( m_FocusedRow - 1 ).SetActive( false );
					}
					SetFocusedContainer( m_AttachmentCargoGrids.GetElement( m_FocusedRow - m_Ics.Count() ) );
					grid.SetActive( true );
				}
			}
		}
	
		if( direction == Direction.RIGHT )
		{
			if( m_FocusedRow < m_Ics.Count() )
			{
				m_FocusedColumn++;
				if( m_FocusedColumn == m_Ics.Get( m_FocusedRow ).GetColumnCount() )
				{
					m_FocusedColumn = 0;
				}
			}
			else if( m_FocusedRow < m_Ics.Count() + m_AttachmentCargos.Count() )
			{
				m_AttachmentCargos.GetElement( m_FocusedRow - m_Ics.Count() ).MoveGridCursor( direction );
			}
		}
	
		if( direction == Direction.LEFT )
		{
			if( m_FocusedRow < m_Ics.Count() )
			{
				m_FocusedColumn--;
				if( m_FocusedColumn < 0 )
				{
					m_FocusedColumn = m_Ics.Get( m_FocusedRow ).GetColumnCount() - 1;
				}
			}
			else if( m_FocusedRow < m_Ics.Count() + m_AttachmentCargos.Count() )
			{
				m_AttachmentCargos.GetElement( m_FocusedRow - m_Ics.Count() ).MoveGridCursor( direction );
			}
		}
	
		if( m_FocusedRow < m_Ics.Count() )
		{
			m_Ics.Get( m_FocusedRow ).GetMainWidget().FindAnyWidget( "Cursor" + m_FocusedColumn ).Show( true );
			EntityAI focused_item = GetFocusedEntity();
			if( focused_item )
			{
				float x, y;
				m_Ics.Get( m_FocusedRow ).GetMainWidget().FindAnyWidget( "Cursor" + m_FocusedColumn ).GetScreenPos( x, y );
				ItemManager.GetInstance().PrepareTooltip( focused_item, x, y );
			}
		}
	}
	
	override void OnDropReceivedFromHeader( Widget w, int x, int y, Widget receiver )
	{
		ItemPreviewWidget iw = ItemPreviewWidget.Cast( w.FindAnyWidget("Render") );
		if(!iw)
		{
		  string name = w.GetName();
		  name.Replace("PanelWidget", "Render");
		  iw = ItemPreviewWidget.Cast( w.FindAnyWidget(name) );
		}
		if(!iw)
		{
		  iw = ItemPreviewWidget.Cast( w );
		}
		
		if( !iw.GetItem() )
			return;
		
		ItemBase receiver_item;
		name = receiver.GetName();
		name.Replace("PanelWidget", "Render");
		ItemPreviewWidget receiver_iw = ItemPreviewWidget.Cast( receiver.FindAnyWidget(name) );
		if( receiver_iw )
		{
			receiver_item = ItemBase.Cast( receiver_iw.GetItem() );
		}
		ItemBase item = ItemBase.Cast( iw.GetItem() );
		int stack_max;
		
		if( !item.GetInventory().CanRemoveEntity() )
			return;
		if( m_Entity.GetInventory().CanAddAttachmentEx( item, receiver.GetUserID() ) )
		{
			stack_max = InventorySlots.GetStackMaxForSlotId( receiver.GetUserID() );
			float quantity = item.GetQuantity();
			if( stack_max == 0 || stack_max >= quantity || !item.CanBeSplit() )
			{
				GetGame().GetPlayer().PredictiveTakeEntityToTargetAttachmentEx( m_Entity, item, receiver.GetUserID() );
			}
			else
			{
				item.SplitIntoStackMaxClient( m_Entity, receiver.GetUserID() );
			}
		}
		else if( receiver.GetUserID() != -1 )
		{
			if( receiver_item )
			{
				InventoryLocation inv_loc = new InventoryLocation;
				receiver_item.GetInventory().GetCurrentInventoryLocation( inv_loc );
				stack_max = InventorySlots.GetStackMaxForSlotId( inv_loc.GetSlot() );
				quantity = receiver_item.GetQuantity();
				if( receiver_item.CanBeCombined( ItemBase.Cast( iw.GetItem() ) ) )
				{
					receiver_item.CombineItemsClient( item, true );
				}
				else if( stack_max == 0 && GameInventory.CanSwapEntities( receiver_item, item ) )
				{
					if( receiver_item.GetInventory().CanRemoveEntity() )
						GetGame().GetPlayer().PredictiveSwapEntities( item, receiver_item );
				}
			}
			else
			{
				InventoryLocation inv_loc_src = new InventoryLocation;
				InventoryLocation inv_loc_dst = new InventoryLocation;
				item.GetInventory().GetCurrentInventoryLocation( inv_loc_src );
				m_Entity.GetInventory().FindFreeLocationFor( item, FindInventoryLocationType.ATTACHMENT, inv_loc_dst );
				
				if( inv_loc_dst.IsValid() && inv_loc_dst.GetType() == InventoryLocationType.ATTACHMENT )
				{
					stack_max = InventorySlots.GetStackMaxForSlotId( inv_loc_dst.GetSlot() );
					quantity = item.GetQuantity();
					if( stack_max == 0 || stack_max >= quantity || !item.CanBeSplit() )
					{
						GetGame().GetPlayer().PredictiveTakeEntityToTargetAttachmentEx( m_Entity, item, inv_loc_dst.GetSlot() );
					}
					else if( stack_max >= 0 || !item.CanBeSplit() )
					{
						item.SplitIntoStackMaxClient( m_Entity, inv_loc_dst.GetSlot() );
					}
				}
			}
		}
	}

	override void UnfocusAll()
	{
		Container cnt = Container.Cast( this );
		int i;
		for ( i = 1; i < cnt.Count(); i++ )
		{
			for ( int j = 0; j < ITEMS_IN_ROW; j++ )
			{
				Widget w = cnt.Get( i ).GetMainWidget().FindAnyWidget( "Cursor" + j );
				if( w )
					w.Show( false );
			}
		}
		for ( i = 0; i < m_AttachmentCargos.Count(); i++ )
		{
			m_AttachmentCargos.GetElement( i ).Unfocus();
			m_AttachmentCargos.GetElement( i ).SetActive( false );
		}
	}
	
	void ShowTooltip( Widget w, int x, int y )
	{
		if( w == NULL )
		{
			return;
		}
		ItemPreviewWidget iw = ItemPreviewWidget.Cast( w.FindAnyWidget("Render") );
		if( !iw )
		{
		  string name = w.GetName();
		  name.Replace("PanelWidget", "Render");
		  iw = ItemPreviewWidget.Cast( w.FindAnyWidget( name ) );
		}
		if( !iw )
		{
		  iw = ItemPreviewWidget.Cast( w );
		}
		
		if( iw && iw.GetItem() )
		{
			ItemManager.GetInstance().PrepareTooltip( iw.GetItem(), x, y );
		}
	}
	
	void HideTooltip( Widget w, Widget enterW, int x, int y )
	{
		ItemManager.GetInstance().HideTooltip();
	}

	override void DraggingOverHeader( Widget w, int x, int y, Widget receiver )
	{
		if( w == NULL )
		{
			return;
		}
		ItemPreviewWidget iw = ItemPreviewWidget.Cast( w.FindAnyWidget("Render") );
		if(!iw)
		{
		  string name = w.GetName();
		  name.Replace("PanelWidget", "Render");
		  iw = ItemPreviewWidget.Cast( w.FindAnyWidget(name) );
		}
		if(!iw)
		{
		  iw = ItemPreviewWidget.Cast( w );
		}
		
		if( !iw || !iw.GetItem() )
		{
			return;
		}
		
		ItemBase item;
		ItemBase receiver_item;
		name = receiver.GetName();
		name.Replace("PanelWidget", "Render");
		ItemPreviewWidget receiver_iw = ItemPreviewWidget.Cast( receiver.FindAnyWidget(name) );
		if(receiver_iw)
			receiver_item = ItemBase.Cast( receiver_iw.GetItem() );
		
		if( receiver_item )
		{
			int stack_max = InventorySlots.GetStackMaxForSlotId( receiver_item.GetInventory().GetSlotId(0) );
			int quantity = receiver_item.GetQuantity();
			bool combinable = ( quantity < stack_max ) && ( ItemBase.Cast( receiver_item ).CanBeCombined( ItemBase.Cast( iw.GetItem() ) ) );
			if( receiver_item.CanBeCombined( ItemBase.Cast( iw.GetItem() ) ) )
			{
				ItemManager.GetInstance().HideDropzones();
				ItemManager.GetInstance().GetRootWidget().FindAnyWidget( "LeftPanel" ).FindAnyWidget( "DropzoneX" ).SetAlpha( 1 );
				ColorManager.GetInstance().SetColor( w, ColorManager.COMBINE_COLOR );
			}
			else if( stack_max == 0 && GameInventory.CanSwapEntities( receiver_item, iw.GetItem() ) )
			{
				ItemManager.GetInstance().HideDropzones();
				ItemManager.GetInstance().GetRootWidget().FindAnyWidget( "LeftPanel" ).FindAnyWidget( "DropzoneX" ).SetAlpha( 1 );
				ColorManager.GetInstance().SetColor( w, ColorManager.SWAP_COLOR );
			}
			else
			{
				ItemManager.GetInstance().ShowSourceDropzone( iw.GetItem() );
				ColorManager.GetInstance().SetColor( w, ColorManager.RED_COLOR );
			}
		}
		else if( receiver.GetUserID() != -1 )
		{
			item = ItemBase.Cast( iw.GetItem() );
			
			if( m_Entity.GetInventory().CanAddAttachmentEx( item, receiver.GetUserID() ) )
			{
				ItemManager.GetInstance().HideDropzones();
				ItemManager.GetInstance().GetRootWidget().FindAnyWidget( "LeftPanel" ).FindAnyWidget( "DropzoneX" ).SetAlpha( 1 );
				ColorManager.GetInstance().SetColor( w, ColorManager.GREEN_COLOR );
			}
			else
			{
				ItemManager.GetInstance().ShowSourceDropzone( iw.GetItem() );
				ColorManager.GetInstance().SetColor( w, ColorManager.RED_COLOR );
			}
		}
		else
		{
			item = ItemBase.Cast( iw.GetItem() );
			InventoryLocation inv_loc_src = new InventoryLocation;
			InventoryLocation inv_loc_dst = new InventoryLocation;
			item.GetInventory().GetCurrentInventoryLocation( inv_loc_src );
			m_Entity.GetInventory().FindFreeLocationFor( item, FindInventoryLocationType.ATTACHMENT, inv_loc_dst );
			
			if( inv_loc_dst.IsValid() )
			{
				ItemManager.GetInstance().HideDropzones();
				ItemManager.GetInstance().GetRootWidget().FindAnyWidget( "LeftPanel" ).FindAnyWidget( "DropzoneX" ).SetAlpha( 1 );
				ColorManager.GetInstance().SetColor( w, ColorManager.GREEN_COLOR );
			}
			else
			{
				ItemManager.GetInstance().ShowSourceDropzone( iw.GetItem() );
				ColorManager.GetInstance().SetColor( w, ColorManager.RED_COLOR );
			}
		}
	}
	
	void OnIconDrag( Widget w )
	{
		ItemManager.GetInstance().SetIsDragging( true );
		string name = w.GetName();
		name.Replace( "PanelWidget", "Render" );
		ItemPreviewWidget ipw = ItemPreviewWidget.Cast( w.FindAnyWidget(name) );
		float icon_x, icon_y, x_content, y_content;
		int m_sizeX, m_sizeY;

		InventoryItem i_item = InventoryItem.Cast( ipw.GetItem() );
		GetGame().GetInventoryItemSize( i_item, m_sizeX, m_sizeY );

		m_Parent.m_Parent.GetMainWidget().FindAnyWidget( "body" ).GetScreenSize( x_content, y_content );
		icon_x = x_content / 10;
		icon_y = x_content / 10;
		w.SetFlags( WidgetFlags.EXACTSIZE );
		w.SetSize( icon_x, icon_y );
		w.SetSize( icon_x * m_sizeX - 1 , icon_y * m_sizeY + m_sizeY - 1 );
		
		if( !ipw.GetItem() )
		{
			return;
		}
		
		name.Replace( "Render", "Col" );
		w.FindAnyWidget( name ).Show( true );
		name.Replace( "Col", "Temperature" );
		w.FindAnyWidget( name ).Show( false );
		name.Replace( "Temperature", "RadialIcon" );
		w.FindAnyWidget( name ).Show( false );
		ItemManager.GetInstance().SetDraggedItem( ipw.GetItem() );
	}
	
	void OnIconDrop( Widget w )
	{
		ItemManager.GetInstance().HideDropzones();
		
		ItemManager.GetInstance().SetIsDragging( false );
		w.ClearFlags( WidgetFlags.EXACTSIZE );
		w.SetSize( 1, 1 );
		string name = w.GetName();
		name.Replace( "PanelWidget", "Col" );
		w.FindAnyWidget( name ).Show( false );
		name.Replace( "Col", "Temperature" );
		w.FindAnyWidget( name ).Show( false );
		name.Replace( "Temperature", "GhostSlot" );
		w.GetParent().FindAnyWidget( name ).Show( true );
		ItemManager.GetInstance().SetDraggedItem( NULL );
	}
	
	void DoubleClick(Widget w, int x, int y, int button)
	{
		if( button == MouseState.LEFT )
		{
			if( w == NULL )
			{
				return;
			}
			ItemPreviewWidget iw = ItemPreviewWidget.Cast( w.FindAnyWidget( "Render" ) );
			if( !iw )
			{
			  string name = w.GetName();
			  name.Replace( "PanelWidget", "Render" );
			  iw = ItemPreviewWidget.Cast( w.FindAnyWidget( name ) );
			}
			if( !iw )
			{
			  iw = ItemPreviewWidget.Cast( w );
			}
			if( !iw.GetItem() )
			{
				return;
			}
			
			if( m_Entity.GetInventory().GetSlotLock( iw.GetUserID() ) )
			{
				return;
			}
			
			ItemBase item = ItemBase.Cast( iw.GetItem() );
			
			if( !item.GetInventory().CanRemoveEntity() )
				return;
			
			PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
			
			if( player.GetInventory().HasEntityInInventory( item ) && player.GetHumanInventory().CanAddEntityInHands( item ) )
			{
				player.PredictiveTakeEntityToHands( item );
			}
			else
			{
				if(player.GetInventory().CanAddEntityToInventory( item ) && item.GetInventory().CanRemoveEntity())
				{
					if( item.ConfigGetFloat("varStackMax") )
					{
						item.SplitIntoStackMaxClient( player, -1 );
					}
					else
						player.PredictiveTakeEntityToInventory( FindInventoryLocationType.ANY, item );
				}
				else
				{
					if( player.GetHumanInventory().CanAddEntityInHands( item ) )
					{
						player.PredictiveTakeEntityToHands( item );
					}
				}
			}
			
			ItemManager.GetInstance().HideTooltip();
			
			name = w.GetName();
			name.Replace( "PanelWidget", "Temperature" );
			w.FindAnyWidget( name ).Show( false );
		}
	}
	
	int GetCargoHeight()
	{
		int count;
		foreach( UICargoGrid grid : m_AttachmentCargos )
		{
			count += grid.GetCargoHeight();
		}
		return count;
	}
	
	Widget GetLastRowWidget()
	{
		if( m_AttachmentCargos.Count() > 0 )
			return m_AttachmentCargos.GetElement( m_AttachmentCargos.Count() - 1 ).GetLastRowWidget();	
		else
			return null;
	}
	
	bool DraggingOverGrid( Widget w,  int x, int y, Widget reciever, IconsContainer cargo )
	{
		if( w == NULL )
		{
			return false;
		}
		
		EntityAI item = GetItemPreviewItem( w );
		
		if( !item )
		{
			return false;
		}

		int color;
		int idx = 0;
		int c_x, c_y;
		
		EntityAI target_entity;
		CargoBase target_cargo;
		
		target_entity	= m_AttachmentCargoGrids.GetKeyByValue( cargo );
		if( target_entity )
		{
			target_cargo 	= target_entity.GetInventory().GetCargo();
		}
		else
			return false;
		
		if( target_cargo && target_entity )
		{
			c_x = target_cargo.GetHeight();
			c_y = target_cargo.GetWidth();
		}
		else
			return false;
		
		if( c_x > x && c_y > y && target_entity.GetInventory().CanAddEntityInCargoEx( item, idx, x, y ) )
		{
			color = ColorManager.GREEN_COLOR;
		}
		else
		{
			color = ColorManager.RED_COLOR;
		}

		if( w.FindAnyWidget("Color") )
		{
			w.FindAnyWidget("Color").SetColor( color );
		}
		else
		{
			string name = w.GetName();
			name.Replace( "PanelWidget", "Col" );
			if( w.FindAnyWidget( name ) )
			{
				w.FindAnyWidget( name ).SetColor( color );
			}
		}

		return true;
	}

	void DropReceived( Widget w, int x, int y, IconsContainer cargo )
	{
		EntityAI item = GetItemPreviewItem( w );
		if( !item )
		{
			return;
		}

		int idx = 0;
		int c_x, c_y;
		
		EntityAI target_entity;
		CargoBase target_cargo;
		
		target_entity	= m_AttachmentCargoGrids.GetKeyByValue( cargo );
		if( target_entity )
		{
			target_cargo 	= target_entity.GetInventory().GetCargo();
		}
		else
			return;
		
		if( target_cargo && target_entity )
		{
			c_x = target_cargo.GetHeight();
			c_y = target_cargo.GetWidth();
		}
		else
			return;
		
		if( c_x > x && c_y > y && target_entity.GetInventory().CanAddEntityInCargoEx( item, idx, x, y ) )
		{
			PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );

			TakeIntoCargo( player, target_entity, item, idx, x, y );

			Icon icon = cargo.GetIcon( item.GetID() );
			
			if( icon )
			{
				//w.FindAnyWidget("Color").SetColor( ColorManager.BASE_COLOR );
				icon.RefreshPos( x, y );
				icon.Refresh();
				Refresh();
			}
		}
	}
	
	void TakeIntoCargo( notnull PlayerBase player, notnull EntityAI entity, notnull EntityAI item, int idx = -1, int row = 0, int col = 0 )
	{
		ItemBase item_base = ItemBase.Cast( item );
		float stackable = item_base.ConfigGetFloat("varStackMax");
		
		if( !item.GetInventory().CanRemoveEntity() )
			return;
		
		if( stackable == 0 || stackable >= item_base.GetQuantity() )
		{
			if( idx != -1 )
				player.PredictiveTakeEntityToTargetCargoEx( entity, item, idx, row, col );
			else
				player.PredictiveTakeEntityToTargetCargo(entity, item);
		}
		else if( stackable != 0 && stackable < item_base.GetQuantity() )
		{
			item_base.SplitIntoStackMaxCargoClient( entity, idx, row, col );
		}
	}
	
	EntityAI GetItemPreviewItem( Widget w )
	{
		ItemPreviewWidget ipw = ItemPreviewWidget.Cast( w.FindAnyWidget( "Render" ) );
		if( !ipw )
		{
		  string name = w.GetName();
		  name.Replace( "PanelWidget", "Render" );
		  ipw = ItemPreviewWidget.Cast( w.FindAnyWidget( name ) );
		}
		if( !ipw )
		{
		  ipw = ItemPreviewWidget.Cast( w );
		}
		if( !ipw || !ipw.IsInherited( ItemPreviewWidget ) )
		{
			return NULL;
		}
		return ipw.GetItem();
	}
	
	void Init(int attachments_categories_count, int i, string attachment_category, string config_path_attachment_categories, EntityAI entity, int parent_m_Body_count )
	{
		m_Entity = entity;
		Header he = Header.Cast( this.Get(0) );
		
		m_CategoryIdentifier = attachment_category;
		
		ref array<string> player_ghosts_slots2 = new array<string>;
		string category_name;
		
		string config_path_ghosts_slots3 = config_path_attachment_categories + " " + attachment_category + " attachmentSlots";
		string config_path_category_name = config_path_attachment_categories + " " + attachment_category + " name";
		GetGame().ConfigGetTextArray( config_path_ghosts_slots3, player_ghosts_slots2 );
		GetGame().ConfigGetText( config_path_category_name, category_name );
		he.SetName( category_name );
		
		array<string> slots = new array<string>;
		foreach( string slot : player_ghosts_slots2 )
		{
			if( m_Entity.CanDisplayAttachmentSlot( slot ) )
			{
				slots.Insert( slot );
			}
		}
		
		int count = slots.Count();
		int number_of_rows = ( count / 7 );
		if( count % 7 == 0 )
		{
			number_of_rows--;
		}
		
		if( parent_m_Body_count < attachments_categories_count + 2 + attachments_categories_count/7 )
		{
			for ( int j = 0; j < (count/7)+1; j++ )
			{
				SlotsContainer ic = new SlotsContainer(this);
				if( j == number_of_rows && count % 7 != 0 )
				{
					ic.SetColumnCount( count % 7 );
				}
				else
				{
					ic.SetColumnCount( 7 );
				}
				
				this.Insert(ic);
				m_Ics.Insert( ic );
				
				ic.GetRootWidget().SetSort( m_Ics.Count() );
				
				for(int k = 0; k < 7; k++)
				{
					WidgetEventHandler.GetInstance().RegisterOnDropReceived( ic.GetMainWidget().FindAnyWidget("Icon"+k),  this, "OnDropReceivedFromHeader" );
					WidgetEventHandler.GetInstance().RegisterOnDropReceived( ic.GetMainWidget().FindAnyWidget("GhostSlot"+k),  this, "OnDropReceivedFromHeader" );
					WidgetEventHandler.GetInstance().RegisterOnDropReceived( ic.GetMainWidget().FindAnyWidget("PanelWidget"+k),  this, "OnDropReceivedFromHeader" );
					
					WidgetEventHandler.GetInstance().RegisterOnDraggingOver( ic.GetMainWidget().FindAnyWidget("Icon"+k),  this, "DraggingOverHeader" );
					WidgetEventHandler.GetInstance().RegisterOnDraggingOver( ic.GetMainWidget().FindAnyWidget("GhostSlot"+k),  this, "DraggingOverHeader" );
					WidgetEventHandler.GetInstance().RegisterOnDraggingOver( ic.GetMainWidget().FindAnyWidget("PanelWidget"+k),  this, "DraggingOverHeader" );
					
					WidgetEventHandler.GetInstance().RegisterOnMouseEnter( ic.GetMainWidget().FindAnyWidget("PanelWidget"+k),  this, "ShowTooltip" );
					WidgetEventHandler.GetInstance().RegisterOnMouseLeave( ic.GetMainWidget().FindAnyWidget("PanelWidget"+k),  this, "HideTooltip" );
				}
			}
		}
		
		array<EntityAI> cargo_attachments = new array<EntityAI>;
		for ( j = 0; j < count; j++ )
		{
			string slot_name = slots.Get ( j );
			string icon_name2;
			string path = "CfgSlots" + " Slot_" + slot_name;
			GetGame().ConfigGetText(path + " ghostIcon", icon_name2);
			//icon_name2 = "missing";
			
			ImageWidget ghost_icon;
			if( ghost_icon )
			{
				ghost_icon.LoadImageFile(0,"set:dayz_inventory image:" + icon_name2 );
			}
			
			GetGame().ConfigGetText(path + " name", slot_name);
			int slot_id = InventorySlots.GetSlotIdFromString( slot_name );
			int id = j%7;
			ItemPreviewWidget item_preview = ItemPreviewWidget.Cast( this.Get((j/7)+1).GetMainWidget().FindAnyWidget("Icon"+id) );
			ItemPreviewWidget item_preview2 = ItemPreviewWidget.Cast( item_preview.FindAnyWidget("Render"+id) );
			ImageWidget image_widget2 = ImageWidget.Cast( item_preview.FindAnyWidget("GhostSlot"+id) );
			Widget temp_widget = item_preview2.FindAnyWidget("Temperature"+id);
			item_preview2.GetParent().Show( true );
			if(!entity.GetInventory())
				return;
			EntityAI ent = entity.GetInventory().FindAttachment(slot_id);
			if( !entity.GetInventory().FindAttachment(slot_id) )
			{
				image_widget2.Show(true);
				if( temp_widget )
					temp_widget.Show(false);
			}
			else
			{
				image_widget2.Show(false);
				if( temp_widget )
					temp_widget.Show(true);
			}
			
			image_widget2.SetUserID( slot_id );
			item_preview2.SetUserID( slot_id );
			
			image_widget2.LoadImageFile(0,"set:dayz_inventory image:" + icon_name2 );
			item_preview2.SetItem(entity.GetInventory().FindAttachment(slot_id));
			item_preview2.SetModelOrientation( Vector(0, 0, 0) );
			item_preview2.SetView( entity.GetViewIndex() );
			
			Widget item_w = item_preview2;
			EntityAI item = m_Entity.GetInventory().FindAttachment(slot_id);
			if( !item )
			{
				item_w.FindAnyWidget("QuantityPanel"+j%7).Show(false);
			}
			
			ItemManager.GetInstance().SetTemperature( item, item_preview2 );
			item_w.FindAnyWidget("Mounted"+j%7).Show(false);
			bool draggable = true;
			if( item_w && item )
			{			
				if( m_Entity.GetInventory().GetSlotLock( slot_id ) && ItemManager.GetInstance().GetDraggedItem() != item )
				{
					item_w.FindAnyWidget("Mounted"+j%7).Show( true );
					draggable = false;
				}
				else
				{
					item_w.FindAnyWidget("Mounted"+j%7).Show( false );
				}
				
				if( !m_Entity.CanReleaseAttachment( item ) )
				{
					draggable = false;
				}
				
				int has_quantity = QuantityConversions.HasItemQuantity( item );
				int stack_max	= InventorySlots.GetStackMaxForSlotId( slot_id );
				Widget quantity_panel = item_w.FindAnyWidget("QuantityPanel"+j%7);
				TextWidget item_quantity = TextWidget.Cast( item_w.FindAnyWidget("Quantity"+j%7) );
				ProgressBarWidget quantity_progress = ProgressBarWidget.Cast( item_w.FindAnyWidget("QuantityBar"+j%7) );
				Widget quantity_stack = item_w.FindAnyWidget("QuantityStackPanel"+j%7);
				if( has_quantity == QUANTITY_HIDDEN )
				{
					quantity_panel.Show( false );
				}
				else
				{
					quantity_panel.Show( true );
					if( has_quantity == QUANTITY_COUNT || stack_max > 1 )
					{
						item_quantity.SetText( QuantityConversions.GetItemQuantityText( item ) );
						quantity_stack.Show( true );
						quantity_progress.Show( false );
					}
					else if( has_quantity == QUANTITY_PROGRESS )
					{
						float progress_max = quantity_progress.GetMax();
						int max = item.ConfigGetInt("varQuantityMax");
						int count2 = item.ConfigGetInt("count");
						float quantity = QuantityConversions.GetItemQuantity( InventoryItem.Cast( item ) );
						if( count2 > 0 )
						{
							max = count2;
						}
						if( max > 0 )
						{

							float value = Math.Round( ( quantity / max ) * 100 );
							quantity_progress.SetCurrent( value );
						}
						quantity_stack.Show( false );
						quantity_progress.Show( true );
					}
				}
				
				if( item.GetInventory().GetCargo() )
				{
					cargo_attachments.Insert( item );
				}
			}
			
			WidgetEventHandler.GetInstance().RegisterOnDrag( item_preview2.GetParent(),  this, "OnIconDrag" );
			WidgetEventHandler.GetInstance().RegisterOnDrop( item_preview2.GetParent(),  this, "OnIconDrop" );
			WidgetEventHandler.GetInstance().RegisterOnDoubleClick( item_preview2.GetParent(),  this, "DoubleClick" );
			
			if( item )
			{
				ImageWidget image_widget4 = ImageWidget.Cast( item_preview2.GetParent().FindAnyWidget("OutOfReach"+j%7) );
				bool in_hands_condition		= m_Entity.GetHierarchyRoot() && item.GetInventory().CanRemoveEntity();
				bool in_vicinity_condition	= !m_Entity.GetHierarchyRoot() && AttachmentsOutOfReach.IsAttachmentReachable( m_Entity, slot_name );
				if( in_hands_condition || in_vicinity_condition )
				{
					if( image_widget4 )
						image_widget4.Show(false);
				}
				else if( image_widget4 )
				{
					if( image_widget4 )
						image_widget4.Show(true);
					draggable = false;
				}
			}
			
			if( draggable )
			{
				item_preview2.GetParent().SetFlags( WidgetFlags.DRAGGABLE );
			}
			else
			{
				item_preview2.GetParent().ClearFlags( WidgetFlags.DRAGGABLE );
			}
		}
		
		if( cargo_attachments && m_AttachmentCargoGrids )
		{
			for( i = 0; i < m_AttachmentCargoGrids.Count(); i++ )
			{
				EntityAI e			= m_AttachmentCargoGrids.GetKey( i );
				
				if( e )
				{
					if( cargo_attachments.Find( e ) == -1 )
					{
						IconsContainer old_cont = m_AttachmentCargoGrids.GetElement( i );
						m_Body.RemoveItem( old_cont );
						m_AttachmentCargoGrids.Remove( e );
						m_AttachmentCargos.Remove( e );
					}
					else
					{
						m_AttachmentCargos.Get( e ).UpdateInterval();
					}
				}
			}
			
			for( i = 0; i < cargo_attachments.Count(); i++ )
			{
				if( !m_AttachmentCargoGrids.Contains( cargo_attachments.Get( i ) ) )
				{
					ref IconsContainer cont = new IconsContainer( this );
					cont.GetRootWidget().SetSort( i + 10 );
					this.Insert( cont );
					
					ref UICargoGrid att_grid = new UICargoGrid( cargo_attachments.Get( i ), cont );
					m_AttachmentCargos.Insert( cargo_attachments.Get( i ), att_grid );
					att_grid.SetParent( this );
					
					m_AttachmentCargoGrids.Insert( cargo_attachments.Get( i ), cont );
				}
			}
		}
	}
}
