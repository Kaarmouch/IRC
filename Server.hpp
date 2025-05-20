#pragma once
#include "utils.hpp"
#include "Channel.hpp"
#include "CommandHandler.hpp"

#include <vector>
#include <poll.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include "Client.hpp"
#include <cstdio>
#include <algorithm>
#include <list>

class Server
{
	private:
		int	server_fd, port;
		std::string password;
		std::vector<Client*> clients;
		std::vector<pollfd> poll_fds;

		std::map<std::string, Channel> channels;

	public:
		Server();
		Server(int port, std::string password);
		Server(const Server&);
		Server& operator=(const Server&);
		~Server();

		void disconnectClient(int fd);
		void handleClientData(int index);
		void handleMessage(Client* client, std::string& str);
		void clientToServ(Client* client, std::string& str);
		
		//Deplacer dans CommandHandler
		//void sendHelp(Client* cli);


		void startListening();
		void initializeServer();
		void acceptNewClient();

		//Command
		void Join_Command(Client* client, const std::string& channelName);
		void Part_Command(Client* client, const std::string& channelName);

		void Topic_Command(Client* client, const std::vector<std::string>& args);
		void displayTopic(Client* client, Channel& chan);
		void setupTopic(Client* client, Channel& chan, const std::vector<std::string>& args, const std::string& channelName);

		bool isNickOk(Client* cli, std::string& nick);

};
