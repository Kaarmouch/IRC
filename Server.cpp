#include "Server.hpp"

Server::Server() : server_fd(-1), port(0), password("") {}

Server::Server(int port, std::string password)
    : server_fd(-1), port(port), password(password) {}

Server::Server(const Server& other) 
	: server_fd(other.server_fd), port(other.port), password(other.password), clients(other.clients), poll_fds(other.poll_fds) {}

Server& Server::operator=(const Server& other)
{
	if (this != &other)
	{
		server_fd = other.server_fd;
		port = other.port;
		password = other.password;
		clients = other.clients;
		poll_fds = other.poll_fds;
	}
	return *this;
}

Server::~Server()
{
	if (server_fd >= 0)
		close(server_fd);
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		close(it->getFd());  // Ferme le descripteur de fichier du client
		for (std::vector<pollfd>::iterator poll_it = poll_fds.begin(); poll_it != poll_fds.end(); ++poll_it) 
		{
			if (poll_it->fd == it->getFd())
			{
				poll_fds.erase(poll_it); // Supprime la socket partie Server 
				break;
			}
		}
	}
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
	int flags = fcntl(server_fd, F_GETFL, 0);
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

void Server::startListening() 
{
	while (true) 
	{
		// poll kblo pour 500ms 
		int ret = poll(poll_fds.data(), poll_fds.size(), 500);
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

bool Server::authenticateClient(int client_fd) 
{
	const std::string prompt = "Please enter the password: ";
	const size_t buffer_size = 1024;
	char buffer[buffer_size];

	send(client_fd, prompt.c_str(), prompt.length(), 0);
	ssize_t bytes_read = read(client_fd, buffer, buffer_size - 1);
	if (bytes_read <= 0) 
	{
		perror("read");
		close(client_fd);
		return false;
	}
	buffer[bytes_read] = '\0';
	std::string input(buffer);
	input.erase(std::remove(input.begin(), input.end(), '\n'), input.end());
	input.erase(std::remove(input.begin(), input.end(), '\r'), input.end());
	if (password == buffer)
		return true; 
	else 
	{
		std::cout << "Incorrect password provided." << std::endl;
		send(client_fd, "Incorrect password. Connection closed.\n", 39, 0);
		close(client_fd);
		return false;
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
	if (!authenticateClient(client_fd)) 
		return;
	Client new_client(client_fd);
	new_client.setIpAdd(inet_ntoa((cliadd.sin_addr)))
	// Ajouter le descripteur du client dans la liste des descripteurs
	clients.push_back(new_client);
	pollfd client_pollfd;
	client_pollfd.fd = client_fd;
	client_pollfd.events = POLLIN;
	client_pollfd.revents = 0;
	poll_fds.push_back(client_pollfd);
}

void Server::handleClientData(int index) 
{
	int fd = poll_fds[index].fd;

	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it.getFd() == fd) 
		{
			it.readMessage();
			break;
		}
	}
}
