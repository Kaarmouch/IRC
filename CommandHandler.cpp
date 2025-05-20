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
    else if (command == "QUIT" || command == "QUIT\r")
        server.disconnectClient(client->getFd());
    else
        handleUnknown(client, command);
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

void CommandHandler::handleUnknown(Client* client, const std::string& command) 
{
    client->sendMessage("Unknown command: " + command);
}




