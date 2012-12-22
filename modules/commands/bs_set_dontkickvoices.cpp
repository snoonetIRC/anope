/* BotServ core functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandBSSetDontKickVoices : public Command
{
 public:
	CommandBSSetDontKickVoices(Module *creator, const Anope::string &sname = "botserv/set/dontkickvoices") : Command(creator, sname, 2, 2)
	{
		this->SetDesc(_("To protect voices against bot kicks"));
		this->SetSyntax(_("\037channel\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		AccessGroup access = source.AccessFor(ci);
		if (!source.HasPriv("botserv/administration") && !access.HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (Anope::ReadOnly)
		{
			source.Reply(_("Sorry, bot option setting is temporarily disabled."));
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			bool override = !access.HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to enable dontkickvoices"; 

			ci->botflags.SetFlag(BS_DONTKICKVOICES);
			source.Reply(_("Bot \002won't kick voices\002 on channel %s."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			bool override = !access.HasPriv("SET");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to disable dontkickvoices"; 

			ci->botflags.UnsetFlag(BS_DONTKICKVOICES);
			source.Reply(_("Bot \002will kick voices\002 on channel %s."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, source.command);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(_(" \n"
				"Enables or disables \002voices protection\002 mode on a channel.\n"
				"When it is enabled, voices won't be kicked by the bot\n"
				"even if they don't match the NOKICK level."));
		return true;
	}
};

class BSSetDontKickVoices : public Module
{
	CommandBSSetDontKickVoices commandbssetdontkickvoices;

 public:
	BSSetDontKickVoices(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandbssetdontkickvoices(this)
	{
		this->SetAuthor("Anope");
	}
};

MODULE_INIT(BSSetDontKickVoices)