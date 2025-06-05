#include "CommandHandler.hpp"

static bool requireArgs(Client* client, const std::vector<std::string>& words, size_t min, const std::string& usage) 
{
    if (words.size() < min || words[min - 1].empty()) 
    {
        client->sendMessage("Usage: " + usage);
        return false;
    }
    return true;
}

void CommandHandler::execute(Server& server, Client* client, const std::string& str) 
{
	std::vector<std::string> words = splitOnSpace(str);

	if (words.empty())
		return;

	std::string command = words[0];
	if (command == "NICK")
		handleNick(server, client, words);
	else if (command == "USER")
		handleUser(server, client, words);
	else if (command == "JOIN")
		handleJoin(server, client, words);
	else if (command == "PART")
		handlePart(server, client, words);
	else if (command == "INVITE")
		handleInvite(server, client, words);
	else if (command == "TOPIC")
		handleTopic(server, client, words);
	else if (command == "HELP")
		handleHelp(client);
	else if (command == "HELP")
		handleHelp(client);
	else if (command == "KICK")
		handleKick(server, client, words);
	else if (command == "PRIVMSG")
		handlePrivate(server, client, words);
	else if (command == "QUIT")
		server.disconnectClient(client->getFd());
	else if (command == "MODE")
		handleMode(server, client, words);
	else if (command != "WHO")
		server.handleMessage(client, str);
}

void CommandHandler::handleInvite(Server& s, Client* c, std::vector<std::string>& w)
{
	if(!requireArgs(c, w, 3, "INVITE target_nickname #channel"))
		return;
	Channel *chan = c->getChanOn();
        Client* target = s.findClientByNick(w[1]);
	if (!chan || chan->getName() != w[2] || !target)
        {
                c->sendMessage(":localhost :Misspelling problem INVITE target_nickname #channel");
                return;
        }
	if (!chan->getIOnly() || chan->isOperator(c))
	{
		if (!chan->isMember(target))
		{
			c->sendMessage(":localhost 341 "+c->getNickn()+" "+target->getNickn());
			target->sendMessage(":"+c->getFullMask()+" INVITE "+target->getNickn()+" "+chan->getName());
		}
		chan->addInvite(target);
	}
}

void CommandHandler::handleKick(Server& s,Client* client, std::vector<std::string>& words)
{
	if (!requireArgs(client, words, 3, "KICK #channel target_nickname"))
		return;

	Channel *chan = client->getChanOn();
	Client* target = s.findClientByNick(words[2]);

	if (!chan || chan->getName() != words[1] || !target)
	{
		client->sendMessage(":localhost :Misspelling problem");
		return;
	}
	if (chan->isOperator(client))
	{
		target->setChanOn(NULL);
		chan->removeMember(target);
		std::string reason = getReason(words);
		std::string msg = ":"+client->getFullMask()+" KICK "+chan->getName()+" "+words[2]+" :"+reason;
		chan->sendAll(client, msg);
		client->sendMessage(msg);
		target->sendMessage(msg);
		target->prompt();
	}
}

void CommandHandler::handlePrivate(Server& server, Client* client, const std::vector<std::string>& words)
{
	 if (words.size() <= 2)
	 {
		 client->sendMessage("Nothing to send");
		 return;
	 }

	 std::string message = ":" + client->getFullMask() ;

	 for (size_t i = 0; i < words.size(); ++i)
	 {
		 message += " ";
		 message += words[i];
	 }
	 if (words[1][0] == '#')
	 {
		 Channel* chan = server.findChanByName(words[1]);
		 if (!chan)
		 {
			 client->sendMessage("channel dont exist");
			 return;
		 }
		 chan->sendAll(client, message);
		 return;
	 }
	 Client* target = server.findClientByNick(words[1]);
	 if (target)
		 target->sendMessage(message);
	 else
		 client->sendMessage("No target found");
}

void CommandHandler::handleMode(Server& server, Client* client, const std::vector<std::string>& words)
{
	if (!requireArgs(client, words, 2, "MODE <channel> [modes]"))
		return;

	const std::string& channelName = words[1];
	std::map<std::string, Channel>& chans = server.getChannels();
	std::map<std::string, Channel>::iterator it = chans.find(channelName);

	if (it == chans.end())
	{
		client->sendMessage("No such channel");
		return;
	}

	Channel& chan = it->second;
	if (!chan.isOperator(client))
	{
		client->sendMessage("You're not channel operator");
		return;
	}
	if (words.size() == 2)
	{
		client->sendMessage("Channel modes: [not implemented yet]");
		return;
	}

	const std::string& mode = words[2];

	if (mode == "+o" || mode == "-o")
		ModeOperator(server, client, chan, words);
	else if (mode == "+i" || mode == "-i")
		ModeInvite(chan, mode, client);
	else if (mode == "+t" || mode == "-t")
		ModeTopic(chan, mode, client);
	else if (mode == "+k" || mode == "-k")
		ModeKey(chan, words, client);
	else if (mode == "+l" || mode == "-l")
		ModeLimit(chan, words, client);
	else
		client->sendMessage("Unknown mode: " + mode);
}

void CommandHandler::ModeLimit(Channel& chan, const std::vector<std::string>& word, Client* client)
{
	std::string msg = ":"+client->getFullMask() +" MODE "+ chan.getName()+" "+ word[2];

	if (word[2] == "-l")
	{
		chan.setMaxUsers(-1);
		chan.sendAll(client, msg);
		client->sendMessage(msg);
		return;
	}

	if (word[2] == "+l")
	{
		int limit;
		if (word.size() <= 3 || word[3].empty() || !isPositiveInt(word[3], limit))
		{
			client->sendMessage("Usage : mode +l number > 0");
			return;
		}
		msg += " " + word[3];
		chan.setMaxUsers(limit);
		chan.sendAll(client, msg);
		client->sendMessage(msg);
	}
}

void CommandHandler::ModeInvite(Channel& chan, const std::string& mode, Client* client)
{
	std::string msg = ":"+client->getFullMask()+" MODE "+chan.getName() + " "+mode;
        if (mode == "+i")
                chan.setIOnly(true);
        else if (mode == "-i")
                chan.setIOnly(false);
	chan.sendAll(client, msg);
	client->sendMessage(msg);
}

void CommandHandler::ModeOperator(Server& server, Client* client, Channel& chan, const std::vector<std::string>& words)
{
	if (words.size() < 4)
	{
		client->sendMessage("Usage: MODE <channel> [+o/-o] <nickname>");
		return;
	}

	const std::string& Nick = words[3];
	Client* target = server.findClientByNick(Nick);

	if (!target || !chan.isMember(target))
	{
		client->sendMessage("User not in channel");
		return;
	}

	std::string msg = ":"+client->getFullMask()+" MODE "+chan.getName()+" "+words[2]+" "+target->getNickn();

	if (words[2] == "+o")
	{
		if (!chan.isOperator(target))
		{
			chan.promoteToOperator(target);
			chan.sendAll(client, msg);
			client->sendMessage(msg);
		}
		else
			client->sendMessage(Nick + " is already an operator.");
	}
	else 
	{
		if (chan.isOperator(target))
		{
			chan.demoteOperator(target);
			chan.sendAll(client, msg);
			client->sendMessage(msg);
		}
		else
			client->sendMessage(Nick + " is already a non operator.");
	}
}

void CommandHandler::ModeTopic(Channel& chan, const std::string& mode, Client* client)
{
	std::string msg = ":"+client->getFullMask()+" MODE "+chan.getName() + " "+mode;

	if (mode == "+t")
		chan.setRTopic(true);
	else if (mode == "-t")
		chan.setRTopic(false);
	chan.sendAll(client, msg);
	client->sendMessage(msg);
}

void CommandHandler::ModeKey(Channel& chan, const std::vector<std::string>& words, Client* client)
{
	std::string msg = ":"+client->getFullMask()+" MODE "+chan.getName() +" "+words[2];
	if (words[2] == "+k")
	{
		if (words.size() < 4)
		{
			client->sendMessage("Usage: MODE <channel> +k <password>");
			return;
		}
		chan.setPassword(words[3]);
		msg +=  " "+words[3];
		chan.sendAll(client, msg);
		client->sendMessage(msg);
	}
	else if (words[2] == "-k")
	{
		chan.setPassword("");
		chan.sendAll(client, msg);
		client->sendMessage(msg);
	}
}

void CommandHandler::handleNick(Server& server, Client* client, const std::vector<std::string>& words) 
{
	if (!requireArgs(client, words, 2, "NICK <nickname>"))
		return;
	std::string newNick = words[1];
	if (server.isNickOk(client, newNick)) 
	{
		std::string oldNick = ":"+ client->getFullMask();
		client->setNickname(newNick);
		client->sendMessage(oldNick +" NICK :" + newNick);
		if (client->getChanOn())
			(client->getChanOn())->sendAll(client, oldNick+" NICK :"+newNick);
	}
}

void CommandHandler::handleUser(Server& server, Client* client, const std::vector<std::string>& words) 
{
    (void)server;
    if (!requireArgs(client, words, 2, "USER <username>"))
        return;

    client->setUsername(words[1]);
    client->sendMessage("Username set to: " + words[1]);
}

void CommandHandler::handleJoin(Server& server, Client* client, const std::vector<std::string>& words) 
{
    if (!requireArgs(client, words, 2, "JOIN <#channel>"))
        return;

    std::string password = (words.size() >= 3) ? words[2] : "";
    server.Join_Command(client, words[1], password);
}

void CommandHandler::handlePart(Server& server, Client* client, const std::vector<std::string>& words) 
{
    if (!requireArgs(client, words, 2, "PART <#channel>"))
        return;

    server.Part_Command(client, words[1]);
}

void CommandHandler::handleTopic(Server& server, Client* client, const std::vector<std::string>& words) 
{
    if (!requireArgs(client, words, 2, "TOPIC <#channel> [topic]"))
        return;
            
    server.Topic_Command(client, words);
}

void CommandHandler::handleHelp(Client* client) 
{
    std::string help =
        "/nick <nickname>        - Set a nickname\n"
        "/user <username>        - Set a username\n"
        "/join <#channel>        - Join a channel\n"
        "/part <#channel>        - Leave a channel\n"
        "/msg <user/#channel> <message> - Send a message\n"
        "/kick <#channel> <user> - Kick a user from a channel\n"
        "/invite <user> <#channel> - Invite a user\n"
        "/topic <#channel> [topic] - View or change topic\n"
        "/mode <#channel> <mode> [param] - Set channel modes\n";

    client->sendMessage(help);
}