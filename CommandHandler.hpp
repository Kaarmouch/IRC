#ifndef COMMAND_HANDLER_HPP
#define COMMAND_HANDLER_HPP

#include <string>
#include <vector>

#include "Server.hpp"
#include "Client.hpp"
#include "utils.hpp"

class Server;
class Client;

class CommandHandler 
{
public:
    static void execute(Server& server, Client* client, const std::string& str);

private:
    static void handleNick(Server& server, Client* client, const std::vector<std::string>& words);
    static void handleUser(Server& server, Client* client, const std::vector<std::string>& words);
    static void handleJoin(Server& server, Client* client, const std::vector<std::string>& words);
    static void handlePart(Server& server, Client* client, const std::vector<std::string>& words);
    static void handleTopic(Server& server, Client* client, const std::vector<std::string>& words);
    static void handleHelp(Client* client);

    static void handleMode(Server& server, Client* client, const std::vector<std::string>& words);
    static void ModeOperator(Server& server, Client* client, Channel& chan, const std::vector<std::string>& words);
    static void ModeTopic(Channel& chan, const std::string& mode, Client* client);
    static void ModeLimit(Channel& chan, const std::vector<std::string>& word, Client* client);
    static void ModeInvite(Channel& chan, const std::string& mode, Client* client);
    static void ModeKey(Channel& chan, const std::vector<std::string>& words, Client* client);
    static void handlePrivate(Server& server, Client* client, const std::vector<std::string>& words);
    static void handleKick(Server& s, Client* client, std::vector<std::string>& words);
    static void handleInvite(Server& s, Client* c, std::vector<std::string>& w);
};

#endif
