#include "CommandHandler.hpp"

static bool requireArgs(Client* client, const std::vector<std::string>& words, size_t min, const std::string& usage) 
{
    // // Vérifie que le nombre d'arguments est suffisant et que l'argument requis n'est pas vide
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
    else if (command == "TOPIC")
        handleTopic(server, client, words);
    else if (command == "HELP")
        handleHelp(client);
    else if (command == "QUIT")
        server.disconnectClient(client->getFd());
    else if (command == "MODE")
	    handleMode(server, client, words);
    else
        server.handleMessage(client, command);
}

void CommandHandler::handleMode(Server& server, Client* client, const std::vector<std::string>& words)
{
	if (!requireArgs(client, words, 2, "MODE <channel> [modes]"))
		return;

	const std::string& channelName = words[1];
	std::map<std::string, Channel>& chans = server.getChannels();
	std::map<std::string, Channel>::iterator it = chans.find(channelName);
    //cherche le channel
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
    // Si Aucun mode précisé
	if (words.size() == 2)
	{
		client->sendMessage("Channel modes: [not implemented yet]");
		return;
	}

	const std::string& mode = words[2];

	if (mode == "+o" || mode == "-o")
		ModeOperator(server, client, chan, words);
    /*    
	else if (mode == "+i" || mode == "-i")
		handleModeInvite(chan, mode, client);
    */
	else if (mode == "+t" || mode == "-t")
		handleModeTopic(chan, mode, client);
    /*
	else if (mode == "+k" || mode == "-k")
		handleModeKey(chan, words, client);
        
	else if (mode == "+l" || mode == "-l")
		handleModeLimit(chan, words, client);
    */
	else
		client->sendMessage("Unknown mode: " + mode);
        // AJOUTER -> Message help des commande mode dispognible [+o/-o] [+t/-t] etc....
}

void CommandHandler::ModeOperator(Server& server, Client* client, Channel& chan, const std::vector<std::string>& words)
{
    // verif nickname
	if (words.size() < 4)
	{
		client->sendMessage("Usage: MODE <channel> [+o/-o] <nickname>");
		return;
	}
	const std::string& Nick = words[3];
	Client* target = server.findClientByNick(Nick);

    std::string msg1 = Nick + " is now an operator.";
    std::string msg2 = Nick + " is no longer an operator.";
    std::string msg3 = Nick + " is already an operator.";
	if (!target || !chan.isMember(target))
	{
		client->sendMessage("User not in channel");
		return;
	}
	if (words[2] == "+o")
	{
		if (!chan.isOperator(target))
		{
			chan.promoteToOperator(target);
			chan.sendAll(client, msg1);
		}
		else
			client->sendMessage(Nick + " is already an operator.");
	}
	else // "-o"
	{
		if (chan.isOperator(target))
		{
			chan.demoteOperator(target);
			chan.sendAll(client, msg1);
		}
		else
            chan.sendAll(client, msg2);
	}
}

void CommandHandler::handleModeTopic(Channel& chan, const std::string& mode, Client* client)
{
    std::string msg1 = chan.getName() + " is now topic-restricted (+t)";
    std::string msg2 = chan.getName() + " is no longer topic-restricted (-t)";

	if (mode == "+t")
	{
		chan.setRTopic(true);
		chan.sendAll(client, msg1);
	}
	else if (mode == "-t")
	{
		chan.setRTopic(false);
		chan.sendAll(client, msg2);
	}
}



void CommandHandler::handleNick(Server& server, Client* client, const std::vector<std::string>& words) 
{
    if (!requireArgs(client, words, 2, "NICK <nickname>"))
        return;

    std::string nickname = words[1];
    if (server.isNickOk(client, nickname)) 
    {
        client->setNickname(nickname);
        client->sendMessage("Nickname set to: " + nickname);
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

    server.Join_Command(client, words[1]);
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

