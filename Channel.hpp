#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include "Client.hpp"

class Channel 
{
	private:
		std::string name;                               // Nom du canal
		std::string topic;                              // Sujet du canal
		std::map<Client*, bool> members;                // Liste des membres (nom, est-opérateur ?)
		bool inviteOnly;                                // Mode "sur invitation uniquement"
		std::string password;                           // Mot de passe du canal (optionnel)

		int maxUsers;

	public:
		// Constructeur par défaut + paramétré + copie + affectation + Destructeur 
		Channel();
		Channel(const std::string& channelName, const std::string& channelPassword = "", int maxUsers = 50);
		Channel(const Channel& other);
		Channel& operator=(const Channel& other);
		~Channel();

		// Getters et setters
		std::string getName() const;                                                     
		std::string getTopic() const;
		void setTopic(const std::string& newTopic);
		int getOperatorCount() const;

		// Membres (update for Client*)
		bool addMember(Client* client, bool isOperator = false);
		bool removeMember(Client* client);
		bool isEmpty() const;
		bool isMember(Client* client) const;
		void sendAll(Client *cli, std::string& msg);

		// Permissions
		bool promoteToOperator(Client* user);
		bool demoteOperator(Client*  user);

		// Modes
		void setInviteOnly(bool isInviteOnly);
		void setPassword(const std::string& newPassword);
		bool checkPassword(const std::string& inputPassword) const;

		int getMemberCount() const;
		std::map<Client*, bool> getMembers();
    
};

#endif
