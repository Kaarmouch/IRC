#pragma once
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <sys/socket.h>

#include "Channel.hpp"
#include "utils.hpp"

class Channel;

class Client {
	private:
		int fd; 
		std::string realname, username, nickname, _buff; // nickname -> unique
		bool authentificated;
		Channel* chanOn;
		std::vector<std::string> channels;


	public:
		Client();
		Client(const Client&);
		Client& operator=(const Client&);
		Client(int client_fd);

		int getFd() const;
		std::string getRealn() const;
		std::string getUsern() const;
		std::string getNickn() const;
		bool getPass() const;
		Channel* getChanOn() const;
		std::string getFullMask() const;

		
		std::string readMessage();
		void sendMessage(const std::string& message);
		void appendToBuffer(const std::string& data);
		std::vector<std::string> extractCompleteCommands();

		void prompt(void);
		void setIpAdd(const std::string& addr);
		void setChanOn(Channel* chan);
		void setPass(void);
		void setNickname(const std::string& nickname);
		void setUsername(const std::string& name);

		~Client();
};
