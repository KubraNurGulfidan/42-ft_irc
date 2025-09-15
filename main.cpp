#include "server/Server.hpp"
#include "utils/utils.hpp"

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
		Server srv(av[1], av[2]);
		srv.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << "Fatal: " << e.what() << std::endl;
		return 1;
	}
	(void)av;
}