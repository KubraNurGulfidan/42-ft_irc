// kanaldaki kullanıcıların bilgilerini içeren liste oluşturur

#include "../server/Server.hpp"

void Server::Who(std::vector<std::string> params, Client &client)
{
    if (params.empty())
    {
        for (size_t i = 0; i < clients.size(); ++i)
        {
            Client* c = clients[i];
            std::string msg = "352 " + client.getNickname() + " * " 
                            + c->getNickname() + " " + c->getUsername() + " "
                            + c->getHostname() + " * :0 " + c->getRealname() + "\r\n";
            send(client.getFd(), msg.c_str(), msg.size(), 0);
        }
    }
    else
    {
        std::string channelName = params[0];
        Channel* ch = getChannelByName(channelName);
        if (!ch)
        {
            std::string msg = "403 " + client.getNickname() + " " + channelName + " :No such channel\r\n";
            send(client.getFd(), msg.c_str(), msg.size(), 0);
            return;
        }

        std::vector<Client*>& members = ch->getMembers();
        for (size_t i = 0; i < members.size(); ++i)
        {
            Client* c = members[i];
            std::string msg = "352 " + client.getNickname() + " " + channelName + " "
                            + c->getUsername() + " " + c->getHostname() + " server " 
                            + c->getNickname() + " H :0 " + c->getRealname() + "\r\n";
            send(client.getFd(), msg.c_str(), msg.size(), 0);
        }
    }

    std::string endMsg = "315 " + client.getNickname() + " :End of WHO list\r\n";
    send(client.getFd(), endMsg.c_str(), endMsg.size(), 0);
}
