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
/*
// ne pas suprimer pour l'instant (seul back_up)
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
*/	
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
	new_client->prompt();
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
	// Appeler une Channel::fonction qui libere pointeur client (et reorganise les operators ?)
	//recup chann avec le fd ? donner directement &Client a la fonction pour link avec le channel
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
	std::string STR = str_toupper(str);
	for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
        {
                if (str_toupper((*it)->getNickn()) == STR)
                {
			cli->sendMessage(prompti);
                        return 0;
                }
        }
	return 1;
}

// CommandHandler.hpp
void Server::clientToServ(Client* cli, std::string& str)
{
    CommandHandler::execute(*this, cli, str);
	// parser la commande
	// identifier commande  (ex: NICK, JOIN, TOPIC, etc.),
	// appeler la fonction associée (handleNick, handleJoin, etc.).
}

void Server::handleMessage(Client* cli, std::string& msg)
{	
	Channel* chanOn = cli->getChanOn();

	if (!chanOn)
	{
		cli->sendMessage("You have no channel, please JOIN #channel");
	}
	else
	{
		std::string m = cli->getNickn() + " said : " + msg;
		chanOn->sendAll(cli, m);
	}
}

void Server::handleClientData(int index) 
{
	int fd = poll_fds[index].fd;
	//int flag = 0;
	std::string str = "";

	for (std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if ((*it)->getFd() == fd) 
		{
			Client * cli = *it;
			str = cli->readMessage();
			std::cout << "Received from "<< cli->getNickn() << " " <<cli->getPass() << " : "<< str;
			if (str == "")
			{
				disconnectClient(fd);
				return;
			}
			std::vector<std::string> messages = splitByCRLF(str);
			for (size_t i = 0; i < messages.size(); i++)
			{
				std::string msg = messages[i];
				if (!(cli->getPass()))
				{
					std::string pass = get_pass(msg);
					std::cout << "pass " << "'" <<pass<< "'" << std::endl; 
					if (pass == password)
					{
						cli->sendMessage("You are now authentificated");
						cli->setPass();
					}
					if (!(cli->getPass()) && pass != "CAP")
						cli->sendMessage("Wrong password");
				}
				else
					clientToServ(cli, msg);
			}
			cli->prompt();
			break;
		}
	}
}

void Server::Join_Command(Client* client, const std::string& channelName) 
{
	std::map<std::string, Channel>::iterator it = channels.find(channelName);
	std::string prompt = client->getNickn() + " joined : " + channelName;

	if (!isValidChannelName(channelName)) 
	{
        client->sendMessage("Invalid channel name. Must start with # and not contain invalid characters.");
        return;
    }
    //Verif si le client est déjà dans un channel
    if (!isClientFree(client)) //client->getChanOn != "No channel"
	{
        client->sendMessage("You are already in a channel. Use PART first.");
        return;
    }
    if (it == channels.end()) 
    {
        std::pair<std::map<std::string, Channel>::iterator, bool> result =
            channels.insert(std::make_pair(channelName, Channel(channelName)));
        
		Channel& newChannel = result.first->second;
        newChannel.addMember(client, true);
        client->setChanOn(&newChannel);
		// Mise à jour du topic pour prompt principal client
        client->sendMessage("You created and joined " + channelName);
        std::cout << client->getNickn() << " created and joined channel " << channelName << std::endl;
    } 
    else 
    {
        Channel& existing_channel = it->second;

        if (existing_channel.addMember(client)) 
        {
            client->setChanOn(&existing_channel);
            client->sendMessage("You joined " + channelName);
            std::cout << client->getNickn() << " joined channel " << channelName << std::endl;

			std::string joinMsg = client->getNickn() + " has joined the channel.";
			existing_channel.sendAll(client, prompt);
        } 
        else 
            client->sendMessage("Unable to join " + channelName);
    }
}

void Server::Part_Command(Client* client, const std::string& channelName)
{
	std::map<std::string, Channel>::iterator it = channels.find(channelName);
	std::string prompt = client->getNickn() + " left : " + channelName;

	// est-il dans un channel + verif correspond-il à celui où il est réellement
    if (!canPartChannel(client, channelName))
        return;
    // si channel n’existe pas dans le serveur
	if (it == channels.end()) 
    {
        client->sendMessage("Channel " + channelName + " not found.");
        client->setChanOn(NULL);
        return;
    }
    Channel& existing_channel = it->second;
    if (existing_channel.removeMember(client)) 
    {
        client->sendMessage("You left " + channelName);
        std::cout << client->getNickn() << " left channel " << channelName << std::endl;
        client->setChanOn(NULL);
		// Mise à jour du topic pour prompt principal client

		std::string partMsg = client->getNickn() + " has left the channel.";
		existing_channel.sendAll(client, prompt);

        // Plus personne dans le channel = suprimer de map Channels + log
		if (existing_channel.isEmpty()) 
        {
            channels.erase(it);
            std::cout << "Channel " << channelName << " has been deleted (empty)." << std::endl;
        }
    }
    else 
        client->sendMessage("You were not a member of " + channelName);
}

void Server::Topic_Command(Client* client, const std::vector<std::string>& args) 
{
	if (args.size() < 2) 
	{
		client->sendMessage("Not enough parameters");
		return;
	}
	std::string channelName = args[1];
	std::map<std::string, Channel>::iterator it = channels.find(channelName);
	if (it == channels.end()) 
	{
		client->sendMessage("No such channel");
		return;
	}
	Channel& chan = it->second;
	if (!chan.isMember(client)) 
	{
		client->sendMessage("You're not on that channel");
		return;
	}
	if (args.size() == 2)
		displayTopic(client, chan);
	else
		setupTopic(client, chan, args, channelName);
}

void Server::displayTopic(Client* client, Channel& chan)
{
	const std::string& topic = chan.getTopic();
	if (topic.empty()) 
		client->sendMessage("No topic is set");
	else
		client->sendMessage("topic : " + topic);
}

void Server::setupTopic(Client* client, Channel& chan, const std::vector<std::string>& args, const std::string& channelName)
{
	// prompt message -> Creation topic [OK]
	// prompt message -> Change name Topic [a faire]

	// Vérification des droits
	// Add isTopicRestricted -> command MODE -t
	if (!chan.isOperator(client)) 
	{
		client->sendMessage("You're not channel operator");
		return;
	}
	// Construction du nouveau topic (tout à partir de args[2])
	std::string newTopic;
	for (size_t i = 2; i < args.size(); ++i) 
	{
		newTopic += args[i];
		if (i != args.size() - 1)
			newTopic += " ";
	}
	chan.setTopic(newTopic);
	std::string msg = client->getNickn() + " -> " + channelName + " ADD TOPIC : " + newTopic;
	chan.sendAll(client, msg);

}
