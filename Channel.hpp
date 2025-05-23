#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>

#include "Client.hpp"
#include "utils.hpp"

class Client;

class Channel 
{
private:
    std::string name;                               // Nom du canal
    std::string topic;                              // Sujet du canal
    std::map<Client*, bool> members;                // Liste des membres (nom, est-opérateur ?)
    std::set<Client*> invited;                      // Client invité
    bool inviteOnly;                                // +i -> [Channel sur invitation]
    bool topicRestricted;                           // +t -> operateur peuvent changer le topic
    std::string password;                           // +k -> Mot de pase channel
    bool hasPassword;                               // Pour vérifier si une clé est définie
    int maxUsers;                                   // Mode +l : Limite d'utilisateurs
    bool limitSet;		// si limitSet est la pour savoir si le mode +l est mis, on peut juste inititer maxUsers a -1 et consciderer ca comme non set, same pour pasword ?

    


public:
    Channel();
    Channel(const std::string& channelName, const std::string& channelPassword = "", int maxUsers = 50); // genre initier maxUser a -1
    Channel(const Channel& other);
    Channel& operator=(const Channel& other);
    ~Channel();

    // Getters / Setters
    std::string getName() const;                                                     
    bool getIOnly() const;
    bool getRTopic() const;
    int getMaxUsers() const;
    std::string getPassword() const;
    std::string getTopic() const;
    int getOperatorCount() const;

    void setTopic(const std::string& newTopic);
    void setIOnly(const bool mode);
    void setRTopic(const bool mode);
    void setMaxUsers(const unsigned int maxU);
    void setPassword(const std::string &pass);

    // Membres
    bool addMember(Client* client, bool isOperator = false);
    bool removeMember(Client* client);                                 // retire un membre
    bool isEmpty() const;                                              // return true si plus de membre dans un channel
    bool isMember(Client* client) const;                               // Savoir si il est deja dans un channel
    bool isOperator(Client* client) const;
    void sendAll(Client *cli, std::string& msg);
    
    // Modes
    
    bool promoteToOperator(Client* client);                 // +o
    bool demoteOperator(Client* client);                    // -o


    // Utilitaires
    int getMemberCount() const;
    std::map<Client*, bool> getMembers();
    bool checkPassword(const std::string& inputPassword) const;
    
};

#endif
