/*
*  @script made by Sylica
*  @project name: Project Firestorm
*/

#include "World.h"
#include "Player.h"
#include "Chat.h"
#include "GuildMgr.h"
#include "Guild.h"

class start_guild : public PlayerScript
{
public:
    start_guild() : PlayerScript("StartGuild") { }

    void OnLogin(Player* player, bool firstLogin) override
    {
        if (firstLogin) {
            const uint32 GUILD_ID = 1;
            ObjectGuid targetGuid = player->GetGUID();
            Guild* targetGuild = sGuildMgr->GetGuildById(GUILD_ID);

            SQLTransaction trans(nullptr);
            targetGuild->AddMember(trans, targetGuid);

            // Inform the player they have joined the guild	
            std::ostringstream ss;
            ss << "Welcome to the " << player->GetGuildName() << " guild " << player->GetName() << "!";
            ChatHandler(player->GetSession()).SendSysMessage(ss.str().c_str());
        }
	}
};

void AddSC_start_guild()
{
	new start_guild();
}
