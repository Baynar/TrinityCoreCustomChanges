/*
<--------------------------------------------------------------------------->
- Developer(s): WiiZZy
- Complete: 100%
- ScriptName: 'World chat'
- Comment: Fully tested
<--------------------------------------------------------------------------->
*/

#include "ScriptPCH.h"
#include "Config.h"
#include "Log.h"
#include "Chat.h"
#include "Common.h"
#include "World.h"
#include "WorldSession.h"
#include "RBAC.h"
#include <unordered_map>

// LOADS CONFIG -- !IMPORTANT! Don't edit unless you know what you are doing!
class World_Chat_Config : public WorldScript
{
	public: World_Chat_Config() : WorldScript("World_Chat_Config") { };
		virtual void OnConfigLoad(bool /*reload*/)
		{
			TC_LOG_INFO("server.loading", "___________________________________");
			TC_LOG_INFO("server.loading", "|     WiiZZy World Chat Script    |");
			TC_LOG_INFO("server.loading", "|-                               -|");
			//TC_LOG_INFO("server.loading", "|____________VER:%.2f_____________|", ver);

			//config_ChatEnabled = sConfigMgr->GetIntDefault("World_Chat.Enable", 1);
			//config_ChatCrossFactions = sConfigMgr->GetIntDefault("World_Chat.CrossFactions", 0);

			TC_LOG_INFO("server.loading", "|    WiiZZy World Chat : Loaded   |");
		}
};

// SETS UP CLASSES FOR CUSTOM SCRIPTS -- !IMPORTANT! Don't edit unless you know what you are doing!
void AddSC_world_chat()
{
	new World_Chat_Config;
	//new world_chat;
}
