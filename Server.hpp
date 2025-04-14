#pragma once
#include <vector>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include "Client.hpp"
#include <cstdio>
#include <algorithm>

class Server
{
	private:
		int	server_fd, port;
		std::string password;
		std::vector<Client> clients;
		std::vector<pollfd> poll_fds;
	public:
		Server();
		Server(const Server&);
		Server& operator=(const Server&);
		~Server();

		Server(int port, std::string password);
		bool authenticateClient(int client_fd);
		void startListening();
		void initializeServer();
		void acceptNewClient();
		void handleClientData(int index);
};




