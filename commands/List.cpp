// mevcut kanalların listesini ve kullancı sayılarını gösterir

#include "../server/Server.hpp"

void Server::List(std::vector<std::string> params, Client &client)
{
	std::string header = "321 " + client.getNickname() + " Channel :Users  Name\r\n";
	std::cout << header;
	// send(client.getFd(), header.c_str(), header.size(), 0);

	if (params.empty()) 
	{
		for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
		{
            Channel* channel = it->second;
            std::ostringstream oss;
            oss << "322 " << client.getNickname() 
                << " " << channel->getChannelName() 
                << " " << channel->getMembers().size() 
                << " :" << channel->getTopic() << "\r\n";
			std::string msg = oss.str();
			// send(client.getFd(), msg.c_str(), msg.size(), 0);
        }
	}
	else
	{
		for (size_t i = 0; i < params.size(); i++)
		{
            std::string chName = params[i];
            Channel* channel = getChannelByName(chName);
            if (channel)
			{
                std::ostringstream oss;
                oss << "322 " << client.getNickname() 
                    << " " << channel->getChannelName() 
                    << " " << channel->getMembers().size() 
                    << " :" << channel->getTopic() << "\r\n";
                std::string msg = oss.str();
				// send(client.getFd(), msg.c_str(), msg.size(), 0);
			}
        }
	}
	std::string endMsg = "323 " + client.getNickname() + " :End of /LIST\r\n";
    std::cout << endMsg;
	// send(client.getFd(), endMsg.c_str(), endMsg.size(), 0);
}