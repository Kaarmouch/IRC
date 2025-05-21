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

void CommandHandler::handleMode(Server& s, Client* c, const std::vector<std::string>& words)
{
	if (!requireArgs(c, words, 2, "MODE <type> [if needed]"))
		return;
	Channel* chanOn = c->getChanOn();
	if (!on->isOperator(c))
	{
		c->sendMessage("You're not channel operator");
		return;
	}
	if (word[1] == "-i" )
		chanOn->setIOnly(0);
	else if (word[1] == "+i")
		chanOn->setIOnly(1);
	else if (words[1] == "-t")
		chanOn->setRTopic(1);
	else if (words[1] == "+t")
		chanOn->setRTopic(0);
	else if (words[1] == "-k")
		chaOn->setPassword(NULL);
	if (!requireArgs(c, words, 3, "MODE <type> [needed]"))
                return;
	else if (words[1] == "+k" && !words[2])
		c->sendMessage("You need to add a value : +k");
	else if (words[1] == "+k" && words[2])
		chanOn->setPassword(words[2]);
	/*else if (words[1] = "-o")
	{
		chanOn
	else if (words[1] = "+o") verifier que word.size >=  3 peut etre reqpler requireArgs avec 3

	else if (words[1] "-l")
	else if (words[1] "+l")
}*/ 


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

