#include "server/Server.hpp"

int main(int ac, char **av)
{
	if(ac != 3)
	{
		std::cerr << "The format must be ./ircserv <port> <password>" << std::endl;
		return 1;
	}
	(void)av;
	
}