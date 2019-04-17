/*
*  @script made by Sylica
*  @project name: Project Firestorm
*/

#include "World.h"
#include "ScriptMgr.h"
#include "Player.h"
#include "Chat.h"

class login_announcer : public PlayerScript
{
public:
	login_announcer() : PlayerScript("login_announcer") { }

	void OnLogin(Player* player, bool firstLogin) override
	{
		std::ostringstream message;
		if (firstLogin)
		{
			message << "|cff3ADF00Please welcome " << player->GetName() << " to our server!|r";
			sWorld->SendGlobalText(message.str().c_str(), nullptr);
		}
		else
		{
			message << "|cff3ADF00Glad to see you back " << player->GetName() << "!|r";
			ChatHandler(player->GetSession()).PSendSysMessage(message.str().c_str());
		}
	}
};

void AddSC_login_announcer()
{
	new login_announcer();
}
