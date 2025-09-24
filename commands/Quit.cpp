// bağlantıyı keser

#include "../server/Server.hpp"

void Server::Quit(std::vector<std::string> params, Client &client)
{
    std::string reason = "Client Quit";
    if (!params.empty())
        reason = params[0];

    std::string quitMsg = ":" + client.getNickname() + " QUIT :" + reason + "\r\n";

    std::vector<Channel*> channels = client.getChannels();
    for (size_t i = 0; i < channels.size(); ++i)
    {
        Channel* channel = channels[i];
        std::vector<Client*>& members = channel->getMembers();
        for (size_t j = 0; j < members.size(); ++j)
        {
            if (members[j]->getFd() != client.getFd())
                send(members[j]->getFd(), quitMsg.c_str(), quitMsg.size(), 0);
        }
        channel->removeClient(&client);
    }

    close(client.getFd());
    removeClient(&client);
}
