methodmap Entity
{
	property int index
	{
		public get()
		{
			return view_as<int>(this);
		}
	}

	public Entity(int index)
	{
		return view_as<Entity>(index);
	}

	public void sdk_hook(SDKHookType type, SDKHookCB callback)
	{
		SDKHook(this.index, type, callback);
	}
	
	public void sdk_unhook(SDKHookType type, SDKHookCB callback)
	{
		SDKUnhook(this.index, type, callback);
	}

	public int GetProp(PropType type, const char[] key, int size = 4, int element = 0)
	{
		return GetEntProp(this.index, type, key, size, element);
	}
	public void SetProp(PropType type, const char[] key, int value, int size = 4, int element = 0)
	{
		SetEntProp(this.index, type, key, value, size, element);
	}

	public bool IsValidEd()
	{
		return IsValidEdict(this.index);
	}
}

methodmap Player < Entity
{
	public Player(int index)
	{
		return view_as<Player>(index);
	}

	//public ~Player() {};

	property bool vip
	{
		public get()
		{
			return g_bIsVip[this.index];
		}
		public set(bool status)
		{
			g_bIsVip[this.index] = status;
		}
	}

	property AdminId adminid
	{
		public get()
		{
			return GetUserAdmin(this.index);
		}
	}

	property bool disturbed
	{
		public get()
		{
			return g_bDisturbed[this.index];
		}
		public set(bool status)
		{
			g_bDisturbed[this.index] = status;
		}
	}

	property int primary_weapon
	{
		public get()
		{
			return g_iPrimWeap[this.index];
		}
		public set(int weapon)
		{
			g_iPrimWeap[this.index] = weapon;
		}
	}
	
	property int secondary_weapon
	{
		public get()
		{
			return g_iSecWeap[this.index];
		}
		public set(int weapon)
		{
			g_iSecWeap[this.index] = weapon;
		}
	}

	property bool not_changed_team
	{
		public get()
		{
			return g_bShowChangedTeamMsg[this.index];
		}
		public set(bool state)
		{
			g_bShowChangedTeamMsg[this.index] = state;
		}
	}

	property int team
	{
		public get()
		{
			return GetClientTeam(this.index);
		}
		public set(int team)
		{
			CS_SwitchTeam(this.index, team);
		}
	}

	public int IsConnected()
	{
		return IsClientInGame(this.index);
	}

	public int IsAlive()
	{
		return IsPlayerAlive(this.index);
	}

	public int IsFake()
	{
		return IsFakeClient(this.index);
	}

	public bool GetName(char[] name, int maxlen)
	{
		return GetClientName(this.index, name, maxlen);
	}

	public int GetWeaponInSlot(int slot)
	{
		return GetPlayerWeaponSlot(this.index, slot);
	}

	public bool RemoveItem(int item)
	{
		return RemovePlayerItem(this.index, item);
	}

	public int GiveItem(const char[] item)
	{
		return GivePlayerItem(this.index, item);
	}

	public void GetCookie(Handle cookie, char[] buffer, int maxlen)
	{
		GetClientCookie(this.index, cookie, buffer, maxlen);
	}
	
	public void SetCookie(Handle cookie, const char[] value)
	{
		SetClientCookie(this.index, cookie, value);
	}

	public void PrintChatPluginInfo()
	{
		PrintToChat(this.index, "\x01[VIP] \x04Plugin\x02 %s \x04stworzony przez\x02 %s\x04. Obecna wersja \x02 %s\x04.", szPlugin, szAuthor, szVersion);
	}
	public void PrintPluginInfoReplyToConsole()
	{
		if(GetCmdReplySource() != SM_REPLY_TO_CONSOLE)
			SetCmdReplySource(SM_REPLY_TO_CONSOLE);

		ReplyToCommand(this.index, "Plugin: %s\nWersja: %s\nAutor: %s\nStrona: %s\nPlugin skompilowany pod: %s\nLicencja: %s", szPlugin, szVersion, szAuthor, szURL, SOURCEMOD_VERSION, "GPLv3");
	}
	public void InfoChat(int type)
	{
		switch(type)
		{
			case 0: PrintToChat(this.index, "\x01[VIP] \x04Żeby włączyć ponownie menu VIPa napisz \x02/menuv\x04 lub \x02!menuv\x04.");
			case 1: PrintToChat(this.index, "\x01[VIP] \x02Czas kupowania minął!");
		}
	}
	public void VipUpdate()
	{
		this.vip = true;

		if(this.adminid == INVALID_ADMIN_ID)
			this.vip = false;

		if(!GetAdminFlag(this.adminid, Admin_Custom6, Access_Effective))
			this.vip = false;
	}

	public void VIPMenu()
	{
		if(!g_pRound.IntValue)
			return;

		g_hPlayerMenu = new Panel();
	
		g_hPlayerMenu.SetTitle("Menu VIP");
		g_hPlayerMenu.DrawItem("", ITEMDRAW_SPACER|ITEMDRAW_RAWLINE);
		g_hPlayerMenu.DrawText("Broń podstawowa");
		g_hPlayerMenu.DrawItem(g_szPrimaryWeapons[g_iPrimWeap[this.index]]);
		g_hPlayerMenu.DrawItem("", ITEMDRAW_SPACER|ITEMDRAW_RAWLINE);
		g_hPlayerMenu.DrawText("Broń drugorzędna");
		g_hPlayerMenu.DrawItem(g_szSecondaryWeapons[g_iSecWeap[this.index]]);

		g_hPlayerMenu.DrawItem("", ITEMDRAW_NOTEXT|ITEMDRAW_SPACER);
		g_hPlayerMenu.DrawItem("", ITEMDRAW_NOTEXT|ITEMDRAW_SPACER);

		g_hPlayerMenu.DrawItem("Weź bronie");

		g_hPlayerMenu.DrawItem("", ITEMDRAW_NOTEXT|ITEMDRAW_SPACER);
		g_hPlayerMenu.DrawItem("", ITEMDRAW_NOTEXT|ITEMDRAW_SPACER);
		g_hPlayerMenu.DrawItem("", ITEMDRAW_NOTEXT|ITEMDRAW_SPACER);

		g_hPlayerMenu.DrawItem("Wyjdź");

		g_hPlayerMenu.SetKeys((1<<0)|(1<<1)|(1<<4)|(1<<8));

		g_hPlayerMenu.Send(this.index, PlayerMenuHandler, MENU_TIME_FOREVER);

		delete g_hPlayerMenu;
	}

	public void OpenMenuCmd()
	{
		//Check if other menu disturbed player to choose weapon
		if(!this.disturbed)
			return;

		//If disturbed we set it to false
		this.disturbed = false;

		//Check if buytime has expired
		if(g_bBuyTimeExpired)
		{
			this.InfoChat(TextBuyTimeExp);
			return;
		}
		//If everything's ok we show menu
		this.VIPMenu();
	}

	public void ShowMOTD(const char[] title, const char[] msg, int type=MOTDPANEL_TYPE_INDEX)
	{
		char szNum[3];
		IntToString(type, szNum, sizeof(szNum));

		KeyValues kv = new KeyValues("data");
		kv.SetString("title", title);
		kv.SetString("type", szNum);
		kv.SetString("msg", msg);
		ShowVGUIPanel(this.index, "info", kv);
		delete kv;
	}
};
