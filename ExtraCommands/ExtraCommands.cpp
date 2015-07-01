/**
 * Copyright (C) 2014-2015 Justin James.
 *
 * This license must be preserved.
 * Any applications, libraries, or code which make any use of any
 * component of this program must not be commercial, unless explicit
 * permission is granted from the original author. The use of this
 * program for non-profit purposes is permitted.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * In the event that this license restricts you from making desired use of this program, contact the original author.
 * Written by Justin James <justin.aj@hotmail.com>
 */

#include <cstring>
#include "Jupiter/Functions.h"
#include "ExtraCommands.h"
#include "IRC_Bot.h"

using namespace Jupiter::literals;

// Select Command

SelectGenericCommand::SelectGenericCommand()
{
	this->addTrigger("select"_jrs);
	this->addTrigger("ircselect"_jrs);
}

GenericCommand::ResponseLine *SelectGenericCommand::trigger(const Jupiter::ReadableString &parameters)
{
	if (parameters.isEmpty())
	{
		if (IRCCommand::selected_server == nullptr)
			return new GenericCommand::ResponseLine("No IRC server is currently selected."_jrs, GenericCommand::DisplayType::PublicSuccess);
		return new GenericCommand::ResponseLine(IRCCommand::selected_server->getConfigSection() + " is currently selected."_jrs, GenericCommand::DisplayType::PublicSuccess);
	}
	if (IRCCommand::active_server == IRCCommand::selected_server)
		IRCCommand::active_server = nullptr;

	IRCCommand::selected_server = serverManager->getServer(parameters);
	if (IRCCommand::selected_server == nullptr)
		return new GenericCommand::ResponseLine("Error: IRC server \""_jrs + parameters + "\" not found. No IRC server is currently selected."_jrs, GenericCommand::DisplayType::PublicError);

	if (IRCCommand::active_server == nullptr)
		IRCCommand::active_server = IRCCommand::selected_server;
	return new GenericCommand::ResponseLine(IRCCommand::selected_server->getConfigSection() + " is now selected."_jrs, GenericCommand::DisplayType::PublicSuccess);
}

const Jupiter::ReadableString &SelectGenericCommand::getHelp(const Jupiter::ReadableString &)
{
	static Jupiter::ReferenceString defaultHelp = "Selects an IRC server if specified, responds with the currently selected server otherwise. Syntax: select [server]"_jrs;
	return defaultHelp;
}

GENERIC_COMMAND_INIT(SelectGenericCommand)
GENERIC_COMMAND_AS_CONSOLE_COMMAND(SelectGenericCommand)
GENERIC_COMMAND_AS_IRC_COMMAND_ACCESS_CREATE(SelectGenericCommand, 4)

// Deselect Command

DeselectGenericCommand::DeselectGenericCommand()
{
	this->addTrigger("deselect"_jrs);
	this->addTrigger("ircdeselect"_jrs);
	this->addTrigger("dselect"_jrs);
	this->addTrigger("ircdselect"_jrs);
	this->addTrigger("unselect"_jrs);
	this->addTrigger("ircunselect"_jrs);
}

GenericCommand::ResponseLine *DeselectGenericCommand::trigger(const Jupiter::ReadableString &parameters)
{
	if (IRCCommand::selected_server == nullptr)
		return new GenericCommand::ResponseLine("No IRC server is currently selected."_jrs, GenericCommand::DisplayType::PublicSuccess);

	GenericCommand::ResponseLine *ret = new GenericCommand::ResponseLine(IRCCommand::selected_server->getConfigSection() + " has been deselected."_jrs, GenericCommand::DisplayType::PublicSuccess);
	IRCCommand::selected_server = nullptr;
	IRCCommand::active_server = IRCCommand::selected_server;
	return ret;
}

const Jupiter::ReadableString &DeselectGenericCommand::getHelp(const Jupiter::ReadableString &)
{
	static Jupiter::ReferenceString defaultHelp = "Deselects the currently selected IRC server. Syntax: deselect"_jrs;
	return defaultHelp;
}

GENERIC_COMMAND_INIT(DeselectGenericCommand)
GENERIC_COMMAND_AS_CONSOLE_COMMAND(DeselectGenericCommand)
GENERIC_COMMAND_AS_IRC_COMMAND_ACCESS_CREATE(DeselectGenericCommand, 4)

// Raw Command

RawGenericCommand::RawGenericCommand()
{
	this->addTrigger("raw"_jrs);
	this->addTrigger("sendraw"_jrs);
}

GenericCommand::ResponseLine *RawGenericCommand::trigger(const Jupiter::ReadableString &parameters)
{
	IRC_Bot *server;
	if (IRCCommand::selected_server != nullptr)
		server = IRCCommand::selected_server;
	else if (IRCCommand::active_server != nullptr)
		server = IRCCommand::active_server;
	else
		return new GenericCommand::ResponseLine("Error: No IRC server is currently selected."_jrs, GenericCommand::DisplayType::PublicError);

	if (parameters.isEmpty())
		return new GenericCommand::ResponseLine("Error: Too few parameters. Syntax: raw <message>"_jrs, GenericCommand::DisplayType::PrivateError);

	server->send(parameters);
	return new GenericCommand::ResponseLine("Data has been successfully sent to server."_jrs, GenericCommand::DisplayType::PublicSuccess);
}

const Jupiter::ReadableString &RawGenericCommand::getHelp(const Jupiter::ReadableString &)
{
	static STRING_LITERAL_AS_NAMED_REFERENCE(defaultHelp, "Sends a line of data to the selected IRC server. Syntax: raw <message>");
	return defaultHelp;
}

GENERIC_COMMAND_INIT(RawGenericCommand)
GENERIC_COMMAND_AS_CONSOLE_COMMAND(RawGenericCommand)
GENERIC_COMMAND_AS_IRC_COMMAND_ACCESS_CREATE(RawGenericCommand, 5)

// Message Command

IRCMessageGenericCommand::IRCMessageGenericCommand()
{
	this->addTrigger("ircmsg"_jrs);
	this->addTrigger("ircmessage"_jrs);
	this->addTrigger("privmsg"_jrs);
}

GenericCommand::ResponseLine *IRCMessageGenericCommand::trigger(const Jupiter::ReadableString &parameters)
{
	IRC_Bot *server;
	if (IRCCommand::selected_server != nullptr)
		server = IRCCommand::selected_server;
	else if (IRCCommand::active_server != nullptr)
		server = IRCCommand::active_server;
	else
		return new GenericCommand::ResponseLine("Error: No IRC server is currently selected."_jrs, GenericCommand::DisplayType::PublicError);

	if (parameters.wordCount(WHITESPACE) < 3)
		return new GenericCommand::ResponseLine("Error: Too few parameters. Syntax: ircmsg <destination> <message>"_jrs, GenericCommand::DisplayType::PrivateError);

	server->sendMessage(Jupiter::ReferenceString::getWord(parameters, 0, WHITESPACE), Jupiter::ReferenceString::gotoWord(parameters, 1, WHITESPACE));
	return new GenericCommand::ResponseLine("Message successfully sent."_jrs, GenericCommand::DisplayType::PublicSuccess);
}

const Jupiter::ReadableString &IRCMessageGenericCommand::getHelp(const Jupiter::ReadableString &)
{
	static STRING_LITERAL_AS_NAMED_REFERENCE(defaultHelp, "Sends a message to an IRC user or channel on the selected IRC server. Syntax: ircmsg <destination> <message>");
	return defaultHelp;
}

GENERIC_COMMAND_INIT(IRCMessageGenericCommand)
GENERIC_COMMAND_AS_CONSOLE_COMMAND(IRCMessageGenericCommand)
GENERIC_COMMAND_AS_IRC_COMMAND_ACCESS_CREATE(IRCMessageGenericCommand, 5)

// Join Command

JoinGenericCommand::JoinGenericCommand()
{
	this->addTrigger("Join"_jrs);
}

GenericCommand::ResponseLine *JoinGenericCommand::trigger(const Jupiter::ReadableString &parameters)
{
	IRC_Bot *server;
	if (IRCCommand::selected_server != nullptr)
		server = IRCCommand::selected_server;
	else if (IRCCommand::active_server != nullptr)
		server = IRCCommand::active_server;
	else
		return new GenericCommand::ResponseLine("Error: No IRC server is currently selected."_jrs, GenericCommand::DisplayType::PublicError);

	if (parameters.isEmpty())
		return new GenericCommand::ResponseLine("Error: Too Few Parameters. Syntax: join <channel> [password]"_jrs, GenericCommand::DisplayType::PublicError);

	if (parameters.wordCount(WHITESPACE) == 1)
		server->joinChannel(parameters);
	else
		server->joinChannel(Jupiter::ReferenceString::getWord(parameters, 0, WHITESPACE), Jupiter::ReferenceString::gotoWord(parameters, 1, WHITESPACE));

	return new GenericCommand::ResponseLine("Request to join channel has been sent."_jrs, GenericCommand::DisplayType::PublicSuccess);
}

const Jupiter::ReadableString &JoinGenericCommand::getHelp(const Jupiter::ReadableString &)
{
	static STRING_LITERAL_AS_NAMED_REFERENCE(defaultHelp, "Joins a channel. Syntax: join <channel> [password]");
	return defaultHelp;
}

GENERIC_COMMAND_INIT(JoinGenericCommand)
GENERIC_COMMAND_AS_CONSOLE_COMMAND(JoinGenericCommand)
GENERIC_COMMAND_AS_IRC_COMMAND_ACCESS_CREATE(JoinGenericCommand, 3)

// Part Command

PartGenericCommand::PartGenericCommand()
{
	this->addTrigger("Part"_jrs);
}

GenericCommand::ResponseLine *PartGenericCommand::trigger(const Jupiter::ReadableString &parameters)
{
	IRC_Bot *server;
	if (IRCCommand::selected_server != nullptr)
		server = IRCCommand::selected_server;
	else if (IRCCommand::active_server != nullptr)
		server = IRCCommand::active_server;
	else
		return new GenericCommand::ResponseLine("Error: No IRC server is currently selected."_jrs, GenericCommand::DisplayType::PublicError);

	if (parameters.isEmpty())
		return new GenericCommand::ResponseLine("Error: Too few parameters. Syntax: part <channel> [message]"_jrs, GenericCommand::DisplayType::PublicError);
	
	if (parameters.wordCount(WHITESPACE) == 1)
		server->partChannel(parameters);
	else
		server->partChannel(Jupiter::ReferenceString::getWord(parameters, 0, WHITESPACE), Jupiter::ReferenceString::gotoWord(parameters, 1, WHITESPACE));

	return new GenericCommand::ResponseLine("Part command successfuly sent."_jrs, GenericCommand::DisplayType::PublicSuccess);
}

const Jupiter::ReadableString &PartGenericCommand::getHelp(const Jupiter::ReadableString &)
{
	static STRING_LITERAL_AS_NAMED_REFERENCE(defaultHelp, "Parts from a channel. Syntax: part <channel> [message]");
	return defaultHelp;
}

GENERIC_COMMAND_INIT(PartGenericCommand)
GENERIC_COMMAND_AS_CONSOLE_COMMAND(PartGenericCommand)
GENERIC_COMMAND_AS_IRC_COMMAND_ACCESS_CREATE(PartGenericCommand, 3)

// DebugInfo Command

DebugInfoGenericCommand::DebugInfoGenericCommand()
{
	this->addTrigger("debuginfo"_jrs);
}

GenericCommand::ResponseLine *DebugInfoGenericCommand::trigger(const Jupiter::ReadableString &parameters)
{
	IRC_Bot *server;
	if (IRCCommand::selected_server != nullptr)
		server = IRCCommand::selected_server;
	else if (IRCCommand::active_server != nullptr)
		server = IRCCommand::active_server;
	else
		return new GenericCommand::ResponseLine("Error: No IRC server is currently selected."_jrs, GenericCommand::DisplayType::PublicError);

	Jupiter::IRC::Client::Channel *chan;
	Jupiter::IRC::Client::User *user;
	GenericCommand::ResponseLine *ret = new GenericCommand::ResponseLine("Prefixes: "_jrs + server->getPrefixes(), GenericCommand::DisplayType::PublicSuccess);
	GenericCommand::ResponseLine *line = new GenericCommand::ResponseLine("Prefix Modes: "_jrs + server->getPrefixModes(), GenericCommand::DisplayType::PublicSuccess);
	ret->next = line;
	line->next = new GenericCommand::ResponseLine(Jupiter::StringS::Format("Outputting data for %u channels...", server->getChannelCount()), GenericCommand::DisplayType::PublicSuccess);
	line = line->next;
	for (unsigned int index = 0; index < server->getChannelCount(); ++index)
	{
		chan = server->getChannel(index);
		if (chan != nullptr)
		{
			line->next = new GenericCommand::ResponseLine(Jupiter::StringS::Format("Channel %.*s - Type: %d", chan->getName().size(), chan->getName().ptr(), chan->getType()), GenericCommand::DisplayType::PublicSuccess);
			line = line->next;
			for (unsigned int j = 0; j != chan->getUserCount(); ++j)
			{
				user = chan->getUser(j)->getUser();
				line->next = new GenericCommand::ResponseLine(Jupiter::StringS::Format("User %.*s!%.*s@%.*s (prefix: %c) of channel %.*s (of %u shared)", user->getNickname().size(), user->getNickname().ptr(), user->getUsername().size(), user->getUsername().ptr(), user->getHostname().size(), user->getHostname().ptr(), chan->getUserPrefix(j) ? chan->getUserPrefix(j) : ' ', chan->getName().size(), chan->getName().ptr(), user->getChannelCount()), GenericCommand::DisplayType::PublicSuccess);
				line = line->next;
			}
		}
	}
	return ret;
}

const Jupiter::ReadableString &DebugInfoGenericCommand::getHelp(const Jupiter::ReadableString &)
{
	static STRING_LITERAL_AS_NAMED_REFERENCE(defaultHelp, "DEBUG COMMAND - Spits out some information about channels. Syntax: debuginfo");
	return defaultHelp;
}

GENERIC_COMMAND_INIT(DebugInfoGenericCommand)
GENERIC_COMMAND_AS_CONSOLE_COMMAND(DebugInfoGenericCommand)
GENERIC_COMMAND_AS_IRC_COMMAND_ACCESS_CREATE(DebugInfoGenericCommand, 4)

// Exit command

ExitGenericCommand::ExitGenericCommand()
{
	this->addTrigger("exit"_jrs);
}

GenericCommand::ResponseLine *ExitGenericCommand::trigger(const Jupiter::ReadableString &parameters)
{
	exit(0);
}

const Jupiter::ReadableString &ExitGenericCommand::getHelp(const Jupiter::ReadableString &)
{
	static STRING_LITERAL_AS_NAMED_REFERENCE(defaultHelp, "Closes the bot's application process. Syntax: exit");
	return defaultHelp;
}

GENERIC_COMMAND_INIT(ExitGenericCommand)
GENERIC_COMMAND_AS_CONSOLE_COMMAND(ExitGenericCommand)
GENERIC_COMMAND_AS_IRC_COMMAND_ACCESS_CREATE(ExitGenericCommand, 5)

// IRC Connect command

IRCConnectGenericCommand::IRCConnectGenericCommand()
{
	this->addTrigger("IRCConnect"_jrs);
	this->addTrigger("IRCReconnect"_jrs);
}

GenericCommand::ResponseLine *IRCConnectGenericCommand::trigger(const Jupiter::ReadableString &parameters)
{
	if (parameters.isEmpty())
	{
		IRC_Bot *server;
		if (IRCCommand::selected_server != nullptr)
			server = IRCCommand::selected_server;
		else if (IRCCommand::active_server != nullptr)
			server = IRCCommand::active_server;
		else
			return new GenericCommand::ResponseLine("Error: No IRC server is currently selected."_jrs, GenericCommand::DisplayType::PublicError);

		server->disconnect("Connect command used; reconnecting..."_jrs, false);
		return new GenericCommand::ResponseLine("Disconnected from IRC server."_jrs, GenericCommand::DisplayType::PublicSuccess);
	}
	IRC_Bot *server = serverManager->getServer(parameters);
	if (server != nullptr)
	{
		server->disconnect("Connect command used; reconnecting..."_jrs, false);
		return new GenericCommand::ResponseLine("Disconnected from IRC server."_jrs, GenericCommand::DisplayType::PublicSuccess);
	}
	if (serverManager->addServer(parameters))
		return new GenericCommand::ResponseLine("Connection successfully established; server added to server list."_jrs, GenericCommand::DisplayType::PublicSuccess);
	return new GenericCommand::ResponseLine("Error: Unable to find configuration settings for server, or connection refused."_jrs, GenericCommand::DisplayType::PublicError);
}

const Jupiter::ReadableString &IRCConnectGenericCommand::getHelp(const Jupiter::ReadableString &)
{
	static STRING_LITERAL_AS_NAMED_REFERENCE(defaultHelp, "Connects/reconnects to an IRC server, based on config entry. Syntax: IRCConnect [server=here]");
	return defaultHelp;
}

GENERIC_COMMAND_INIT(IRCConnectGenericCommand)
GENERIC_COMMAND_AS_CONSOLE_COMMAND(IRCConnectGenericCommand)
GENERIC_COMMAND_AS_IRC_COMMAND_ACCESS_CREATE(IRCConnectGenericCommand, 5)

// IRC Disconnect command

IRCDisconnectGenericCommand::IRCDisconnectGenericCommand()
{
	this->addTrigger("IRCDisconnect"_jrs);
}

GenericCommand::ResponseLine *IRCDisconnectGenericCommand::trigger(const Jupiter::ReadableString &parameters)
{
	IRC_Bot *server;
	if (IRCCommand::selected_server != nullptr)
		server = IRCCommand::selected_server;
	else if (IRCCommand::active_server != nullptr)
		server = IRCCommand::active_server;
	else
		return new GenericCommand::ResponseLine("Error: No IRC server is currently selected."_jrs, GenericCommand::DisplayType::PublicError);

	server->disconnect("Disconnect command used."_jrs, true);
	return new GenericCommand::ResponseLine("Disconnected from server."_jrs, GenericCommand::DisplayType::PublicSuccess);
}

const Jupiter::ReadableString &IRCDisconnectGenericCommand::getHelp(const Jupiter::ReadableString &)
{
	static STRING_LITERAL_AS_NAMED_REFERENCE(defaultHelp, "Disconnects from an IRC server, based on config entry. Syntax: IRCDisconnect");
	return defaultHelp;
}

GENERIC_COMMAND_INIT(IRCDisconnectGenericCommand)
GENERIC_COMMAND_AS_CONSOLE_COMMAND(IRCDisconnectGenericCommand)
GENERIC_COMMAND_AS_IRC_COMMAND_ACCESS_CREATE(IRCDisconnectGenericCommand, 5)

// Plugin instantiation and entry point.
FunCommandsPlugin pluginInstance;

extern "C" __declspec(dllexport) Jupiter::Plugin *getPlugin()
{
	return &pluginInstance;
}
