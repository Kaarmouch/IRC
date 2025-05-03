#include "Server.hpp"
#include "utils.hpp"
#include <iostream>

int	main(int ac,const char **av)
{
	int port;
	if (ac < 3 || ((port = IsValidPort(av[1])) == 0))
		return std::cout << "./exe <port> <password> "<< std::endl, 1;
	std::string	password = av[2];
	Server server(port, password);
	std::cout << "Serveur IRC démarré sur le port " << port << std::endl;
	try 
	{
		server.initializeServer();
		server.startListening(); // Boucle principale avec poll()
	} 
	catch (const std::exception& e) 
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
