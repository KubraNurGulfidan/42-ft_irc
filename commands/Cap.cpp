
#include "../server/Server.hpp"

void Server::Cap(std::vector<std::string> params, Client &client)
{
    if (params.empty())
        return;

    if (params[0] == "LS")
    {
        std::string reply = ":server CAP * LS :multi-prefix sasl\r\n";
        send(client.getFd(), reply.c_str(), reply.size(), 0);
    }
    else if (params[0] == "REQ" && params.size() > 1)
    {
        std::string reply = ":server CAP * ACK :" + params[1] + "\r\n";
        send(client.getFd(), reply.c_str(), reply.size(), 0);
    }
    else if (params[0] == "END")
    {
        return;
    }
}