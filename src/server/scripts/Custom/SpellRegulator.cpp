#include "SpellRegulator.h"
#include "RBAC.h"
#include "Chat.h"

// Comand stuff
class custom_reload_spellregulator_commands : public CommandScript
{
public:
    custom_reload_spellregulator_commands() : CommandScript("custom_reload_spellregulator_commands") { }

    std::vector<ChatCommand> GetCommands() const
    {
        static std::vector<ChatCommand> customCommandTable =
        {
            { "reload_custom_spellregulator",          rbac::RBAC_PERM_COMMAND_RELOAD, true, &HandleReloadSpellRegulator, "" },
        };
        return customCommandTable;
    }

    static bool HandleReloadSpellRegulator(ChatHandler * handler, const char * args)
    {
        TC_LOG_INFO("misc", "Reloading spellregulator...");
        sSpellRegulator->LoadFromDB();
        handler->SendGlobalGMSysMessage("Spell regulator has been reloaded");
        return true;
    }
};

void AddSC_SpellRegulator()
{
    new custom_reload_spellregulator_commands();
	new RegulatorLoader;
}
