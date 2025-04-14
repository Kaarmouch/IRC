#pragma once
#include <unistd.h>
#include <iostream>
#include <string>

class Client {
	private:
		int fd;  // Descripteur de fichier du client
		std::string name, addr, nickname;
		std::vector<Channel> channel;

	public:
		Client();
		Client(const Client&);
		Client& operator=(const Client&);
		Client(int client_fd);
		int getFd() const;      // Accesseur pour obtenir le descripteur de fichier
		void readMessage();     // Lire le message envoyé par le client
		void sendMessage(const std::string& message);  // Envoyer un message au client
		void setIpAdd(const std::string& addr)
		void setNickname(const std::string& nickname);
		void setUsername(const std::string& name);
		void joinChannel(Channel* channel);
		void leaveChannel(Channel* channel);
		void sendMessage(const std::string& message);
		~Client();
};
