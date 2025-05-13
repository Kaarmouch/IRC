#include "Server.hpp"

Server::Server() : server_fd(-1), port(0), password("") {}

Server::Server(int port, std::string password)
    : server_fd(-1), port(port), password(password) {}

Server::Server(const Server& other) 
	: server_fd(other.server_fd), port(other.port), password(other.password), clients(other.clients), poll_fds(other.poll_fds) {} //add channels

Server& Server::operator=(const Server& other)
{
	if (this != &other)
	{
		server_fd = other.server_fd;
		port = other.port;
		password = other.password;
		channels = other.channels;
		clients = other.clients;
		poll_fds = other.poll_fds;
	}
	return *this;
}

Server::~Server()
{
	if (server_fd >= 0)
		close(server_fd);
	for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		close((*it)->getFd());  // Ferme le descripteur de fichier du client
		for (std::vector<pollfd>::iterator poll_it = poll_fds.begin(); poll_it != poll_fds.end(); ++poll_it) 
		{
			if (poll_it->fd == (*it)->getFd())
			{
				poll_fds.erase(poll_it); // Supprime la socket partie Server 
				break;
			}
		}
		delete *it;
	}
	clients.clear();
	std::cout << "Server Closed" << std::endl;
}

void Server::initializeServer() 
{
	struct sockaddr_in server_addr;
	
	// Créer le socket serveur
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		perror("socket");
		return;
	}
	// Rendre le socket non-bloquant
	int flags = fcntl(server_fd, F_GETFL, O_NONBLOCK);
	if (flags == -1 || fcntl(server_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		perror("fcntl");
		close(server_fd);
		return;
	}
	// Réutilisation d'adresse
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		perror("setsockopt");
		close(server_fd);
		return;
	}
	// Initialiser la structure sockaddr_in
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);
	// Lier le socket à l'adresse et au port
	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("bind");
		close(server_fd);
		return;
	}
	// Commencer à écouter les connexions entrantes
	if (listen(server_fd, 5) < 0) {
		perror("listen");
		close(server_fd);
		return;
	}
	std::cout << "Server listening on port " << port << std::endl;
	// Ajouter le socket serveur dans `poll_fds` pour surveiller les connexions entrantes
	pollfd server_pollfd;
	server_pollfd.fd = server_fd;
	server_pollfd.events = POLLIN;
	server_pollfd.revents = 0;
	poll_fds.push_back(server_pollfd);
}

void Server::sendPrompt(Client* clicli)
{
    std::string chan = clicli->getChanOn();
    std::string p = clicli->getNickn() + " ---------- IRC ---------- [" + chan + "]";

    if (chan != "No channel") 
	{
        std::map<std::string, Channel>::iterator it = channels.find(chan);
        if (it != channels.end()) 
		{
            int opCount = it->second.getOperatorCount();
			int memberCount = it->second.getMemberCount();
           	std::ostringstream oss;
			oss << " -> [Operators: " << opCount << " | Members: " << memberCount << " ]";
			p += oss.str();
        }
    }

    clicli->sendMessage(p);

    if (!clicli->getPass())
        clicli->sendMessage("Password :");
}
	
void Server::startListening() 
{
	while (true) 
	{

		int ret = poll(poll_fds.data(), poll_fds.size(), -1);
		if (ret < 0) 
		{
			perror("poll");
			close(server_fd);
			return;
		}
		// Gérer les événements de chaque fd surveillé
		for (size_t i = 0; i < poll_fds.size(); ++i) 
		{
			// Vérifier si l'événement POLLIN est survenu
			if (poll_fds[i].revents & POLLIN) 
			{
				if (poll_fds[i].fd == server_fd) 
					acceptNewClient();
			       	else 
					handleClientData(i);
			}
		}
	}
}


void Server::acceptNewClient() 
{
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
	
	if (client_fd < 0) 
	{
		perror("accept");
		return;
	}
	std::cout << "New client connected: FD " << client_fd << std::endl;
	Client* new_client = new Client(client_fd);
	new_client->setIpAdd(inet_ntoa((client_addr.sin_addr)));
	// Ajouter le descripteur du client dans la liste des descripteurs
	sendPrompt(new_client);
	clients.push_back(new_client);
	pollfd client_pollfd;
	client_pollfd.fd = client_fd;
	client_pollfd.events = POLLIN;
	client_pollfd.revents = 0;
	poll_fds.push_back(client_pollfd);
}

void Server::disconnectClient(int fd)
{
	std::cout << "Disconnecting client FD: " << fd << std::endl;
	close(fd);

	// Supprimer le client du vecteur clients
	for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if ((*it)->getFd() == fd)
		{
			delete *it; // lib memory
			clients.erase(it); //sup vector pointr
			break;
		}
	}
	// Supprimer du poll_fds
	for (std::vector<pollfd>::iterator it = poll_fds.begin(); it != poll_fds.end(); ++it)
	{
		if (it->fd == fd)
		{
			poll_fds.erase(it);
			break;
		}
	}
}

bool Server::isNickOk(Client* cli, std::string& str)
{
	std::string prompt = "Bad Nickname !";
	std::string prompti = "Already used nickname !";
	
	if (str.empty() ||str == "" || str.find(",") != std::string::npos || str.find("*") != std::string::npos || str.find("?") != std::string::npos 
			|| str.find("!") != std::string::npos || str.find("@") != std::string::npos || str.find(".") != std::string::npos 
			|| str.find("#") != std::string::npos || str.find("/") != std::string::npos
		       	|| str.find("$") != std::string::npos || str.find(":") != std::string::npos)
	{
		cli->sendMessage(prompt);
		return 0;
	}
	for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
        {
                if ((*it)->getNickn() == str)
                {
			cli->sendMessage(prompti);
                        return 0;
                }
        }
	return 1;
}
void Server::sendHelp(Client* cli)
{
	std::string prompt = "Commandes disponibles :\n"
		"/nick <nickname>        - Définir un pseudo 1\n"
		"/user <username>        - Définir un nom d’utilisateur 1\n"
		"/join <#channel>        - Rejoindre un canal\n"
		"/part <#channel>        - Quitter un canal\n"
		"/quite			 - Quitte le server irc 1\n"
		"/msg <user/#channel> <message> - Envoyer un message\n"
		"/kick <#channel> <user> - Expulser un utilisateur\n"
		"/invite <user> <#channel> - Inviter un utilisateur\n" 
		"/topic <#channel> [sujet] - Voir ou changer le sujet\n"
		"/mode <#channel> <mode> [param] - Changer les options du canal\n"
		"\n"
		"Modes de channel :\n"
		"+i : sur invitation  |  +t : sujet réservé aux opérateurs\n"
		"+k : mot de passe    |  +o : donner/retirer opérateur\n"
		"+l : limite d’utilisateurs\n";
	cli->sendMessage(prompt);
}

void Server::clientToServ(Client* cli, std::string& str)
{
	std::vector<std::string> words = splitOnSpace(str);

	if (words.empty())
		return;
	if (words[0] == "NICK") {
		if (words.size() < 2 || words[1].empty()) {
			cli->sendMessage("Usage: /nick <nickname>");
			return;
		}
		if (isNickOk(cli, words[1])) {
			cli->setNickname(words[1]);
			cli->sendMessage("Nickname set to : " + words[1]);
		}
	}
	else if (words[0] == "USER")
	{
		if (words.size() < 2 || words[1].empty())
		{
			cli->sendMessage("Usage: /user <username>");
			return;
		}
		cli->setUsername(words[1]);
		cli->sendMessage("Username set to : " + words[1]);
	}
	else if (words[0] == "HELP") 
		sendHelp(cli);

	else if (words[0] == "JOIN")
	{
		if (words.size() < 2 || words[1].empty())
		{
			cli->sendMessage("Usage: /join <Channel Name>");
			return;
		}
		std::string channelName = words[1];
		Join_Command(cli, channelName);
	}
	else if (words[0] == "PART")
	{
		if (words.size() < 2 || words[1].empty())
		{
			cli->sendMessage("Usage: /part <Channel Name>");
			return;
		}
		std::string channelName = words[1];
    	Part_Command(cli, cli->getChanOn());
	}
	else if (words[0] == "QUITE")
		disconnectClient(cli->getFd());
	else
		cli->sendMessage("Commande inconnue ou non implémentée.");
}

/*void Server::handleMessage(Client& cli, std::string& msg)
{	
	std::string chanOn = cli.getChanOn;

	if (chanOn = "No channel")
		cli.sendMessage("You have no chanel, please /join #channel");
	else
	{
		std::map<std::string, Channel>::iterator it = channels.find(chanOn);
		if (it == channels.end())
		{
			cli.sendMessage("Your channel is busted wtf ?");
			return cli.setChanOn("No channel");
		}
		it.sendAll(cli&msg);


	}
}
*/

void Server::handleClientData(int index) 
{
	int fd = poll_fds[index].fd;
	std::string str = "";

	for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if ((*it)->getFd() == fd) 
		{
			Client * cli = *it;
			str = cli->readMessage();
			std::cout << "Received from "<< cli->getNickn() << " " <<cli->getPass() << " : "<< str;
			if (!(cli->getPass()))
			{
				if (trimCRLF(str) == password)
				{
					cli->sendMessage("You are now authentificated");
					cli->setPass();
				}
				else
					cli->sendMessage("Wrong password");
				sendPrompt(cli);
				break;
			}
			else if (str != "")
				clientToServ(cli, str);
			sendPrompt(cli);
			break;
		}
	}
	if (str == "" )
		disconnectClient(fd);
}

void Server::Join_Command(Client* client, const std::string& channelName) 
{
    std::map<std::string, Channel>::iterator it = channels.find(channelName);

    if (it == channels.end()) 
    {
        // Le canal n'existe pas → on le crée
        Channel newChannel(channelName);
        newChannel.addMember(client, true);
        channels[channelName] = newChannel;

        client->setChanOn(channelName);
        client->sendMessage("You created and joined " + channelName);
        std::cout << client->getNickn() << " created and joined channel " << channelName << std::endl;
    } 
    else 
    {
        // Canal existe → tentative de join
        Channel& existing_channel = it->second;

        if (existing_channel.addMember(client)) 
        {
            client->setChanOn(channelName);
            client->sendMessage("You joined " + channelName);
            std::cout << client->getNickn() << " joined channel " << channelName << std::endl;
        } 
        else 
        {
            client->sendMessage("Unable to join " + channelName);
        }
    }
}


void Server::Part_Command(Client* client, const std::string& channelName)
{
    std::string currentChan = client->getChanOn();
    if (currentChan == "No channel") 
    {
        client->sendMessage("You are not in any channel.");
        return;
    }
    if (channelName != currentChan) 
    {
        client->sendMessage("You are not in " + channelName + ".");
        return;
    }
    std::map<std::string, Channel>::iterator it = channels.find(currentChan);
    if (it == channels.end()) 
    {
        client->sendMessage("Channel " + currentChan + " not found.");
        client->setChanOn("No channel");
        return;
    }
    Channel& chan = it->second;
    if (chan.removeMember(client)) 
    {
        client->sendMessage("You left " + currentChan);
        std::cout << client->getNickn() << " left channel " << currentChan << std::endl;
        std::cout << "Remaining members: " << chan.getMemberCount() << std::endl;
    } 
    else 
    {
        client->sendMessage("You were not a member of " + currentChan);
    }

    if (chan.isEmpty()) 
    {
        channels.erase(it);
        std::cout << "Channel " << currentChan << " has been deleted (empty)." << std::endl;
    }
    client->setChanOn("No channel");
}
