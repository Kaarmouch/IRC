#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>

class Channel 
{
private:
    std::string name;                               // Nom du canal
    std::string topic;                              // Sujet du canal
    std::map<std::string, bool> members;            // Liste des membres (nom, est-opérateur ?)
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

    // Membres
    bool addMember(const std::string& user, bool isOperator = false);
    bool removeMember(const std::string& user);
    bool isMember(const std::string& user) const;

    // Permissions
    bool promoteToOperator(const std::string& user);
    bool demoteOperator(const std::string& user);

    // Modes
    void setInviteOnly(bool isInviteOnly);
    void setPassword(const std::string& newPassword);
    bool checkPassword(const std::string& inputPassword) const;

    // Bannissement
    void KickUser(const std::string& user);


    int getMemberCount() const;
};

#endif
