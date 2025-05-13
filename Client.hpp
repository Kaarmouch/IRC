#pragma once
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <sys/socket.h>

class Client {
	private:
		int fd; 
		std::string realname, username, nickname; // nickname -> unique
		bool authentificated;
		std::string chanOn;
		std::vector<std::string> channels;


	public:
		Client();
		Client(const Client&);
		Client& operator=(const Client&);
		Client(int client_fd);

		int getFd() const;  // Accesseur pour obtenir le descripteur de fichier
		std::string getRealn() const;  // Accesseur pour obtenir le descripteur de fichier
		std::string getUsern() const;  // Accesseur pour obtenir le descripteur de fichier
		std::string getNickn() const;  // Accesseur pour obtenir le descripteur de fichier
		bool getPass() const;
		std::string getChanOn() const;
		
		std::string readMessage();     // Lire le message envoyé par le client
		void sendMessage(const std::string& message);  // Envoyer un message au client
		// prompt -> mieux de la mettre dans utils.hpp
		void prompt(void);	
		void setIpAdd(const std::string& addr);
		void setChanOn(const std::string& chanOn);
		void setPass(void);
		void setNickname(const std::string& nickname);
		void setUsername(const std::string& name);
		/*void joinChannel(std::string channelName);
		void leaveChannel(std::string channelName);*/
		~Client();
};
