#include "server/Server.hpp"
#include <csignal> 

Server* g_server = NULL;

int ft_port_check(const std::string& port)
{
	if (port.empty())
		return 1;
	for (size_t i = 0; i < port.length(); ++i)
    {
		if (!std::isdigit(static_cast<unsigned char>(port[i])))
			return 1;
	}
	unsigned long value = 0;
	for (size_t i = 0; i < port.length(); ++i)
    {
		value = value * 10 + static_cast<unsigned long>(port[i] - '0');
		if (value > 65535UL)
			return 1;
	}
	return (value >= 1UL && value <= 65535UL) ? 0 : 1;
}

int ft_password_check(const std::string& password)
{
	return password.empty() ? 1 : 0;
}

void signalHandler(int signum)
{
	(void)signum;
    if (g_server)
    {
        std::cout << "\nSIGINT received, stopping server..." << std::endl;
        g_server->stop();
    }
}

int main(int ac, char **av)
{
	if(ac != 3)
	{
		std::cerr << "The format must be ./ircserv <port> <password>" << std::endl;
		return 1;
	}
	if(ft_port_check(av[1]) != 0)
	{
		std::cerr << "The port must be a number between 1 and 65535" << std::endl;
		return 1;
	}
	if(ft_password_check(av[2]) != 0)
	{
		std::cerr << "The password must be a string" << std::endl;
		return 1;
	}
	try
	{
		Server srv(atoi(av[1]), av[2]);
		g_server = &srv;
        signal(SIGINT, signalHandler);
		srv.start();
	}
	catch(const std::exception& e)
	{
		std::cerr << "Fatal: " << e.what() << std::endl;
		return 1;
	}
}
