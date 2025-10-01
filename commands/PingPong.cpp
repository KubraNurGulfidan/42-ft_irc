#include "../server/Server.hpp"

void Server::Ping(std::vector<std::string> params, Client &client)
{
    if (params.empty())
    {
        std::string msg = ":server 409 " + client.getNickname() + " :No origin specified\r\n";
        send(client.getFd(), msg.c_str(), msg.size(), 0);
        return;
    }

    std::string token = params[0];
    std::string pongMsg = "PONG :" + token + "\r\n";
    send(client.getFd(), pongMsg.c_str(), pongMsg.size(), 0);
}

void Server::Pong(std::vector<std::string> params, Client &client)
{
    if (params.empty())
    {
        std::string msg = ":server 409 " + client.getNickname() + " :No origin specified\r\n";
        send(client.getFd(), msg.c_str(), msg.size(), 0);
        return;
    }

    std::cout << "Received PONG from " << client.getNickname()
              << " with token: " << params[0] << std::endl;
}

