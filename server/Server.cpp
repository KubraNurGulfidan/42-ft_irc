#include "Server.hpp"
#include "../utils/Utils.hpp"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cstring>
#include <cerrno>
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

Server::Server(int port, const std::string& password) : _listenFd(-1), _port(port), _password(password)
{
    initListeningSocket();
    struct pollfd p;
    p.fd = _listenFd;
    p.events = POLLIN;
    p.revents = 0;
    _pfds.push_back(p);
}

Server::~Server()
{
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        safeClose(it->first);
    safeClose(_listenFd);
}

void Server::initListeningSocket()
{
    _listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_listenFd == -1)
        throw std::runtime_error("socket() failed");
    int yes = 1;
    if (setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
        throw std::runtime_error("setsockopt(SO_REUSEADDR) failed");
    if (setNonBlocking(_listenFd) == -1)
        throw std::runtime_error("setNonBlocking(listen) failed");

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(_port);
    if (bind(_listenFd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
        throw std::runtime_error("bind() failed");
    if (listen(_listenFd, 128) == -1)
        throw std::runtime_error("listen() failed");
    std::cout << "[INFO] Listening on port " << _port << std::endl;
}

void Server::run()
{
    while(true)
    {
        int n = poll(&_pfds[0], _pfds.size(), -1);
        if (n == -1)
        {
            if (errno == EINTR)
                continue;
            throw std::runtime_error("poll() failed");
        }
        for (std::size_t i = 0; i < _pfds.size(); ++i)
        {
            if (_pfds[i].revents == 0)
                continue;
            if (_pfds[i].fd == _listenFd)
                handleListeningSocket(_pfds[i].revents);
            else
            {
                if (_pfds[i].revents & (POLLERR | POLLHUP | POLLNVAL))
                {
                    disconnectClient(_pfds[i].fd, "connection error");
                    continue;
                }
                if (_pfds[i].revents & POLLIN)
                    handleClientRead(_pfds[i].fd);
                if(_pfds[i].revents & POLLOUT)
                    handleClientWrite(_pfds[i].fd);
            }
            _pfds[i].revents = 0;
        }
    }
}

void Server::handleListeningSocket(short revents)
{
    if (!(revents & POLLIN))
        return;
    while (true)
    {
        struct sockaddr_in cli;
        socklen_t len = sizeof(cli);
        int cfd = accept(_listenFd, (struct sockaddr*)&cli, &len);
        if (cfd == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            else
            {
                std::cerr << "accept() failed: " << strerror(errno) << std::endl;
                break;
            }
        }
        if (setNonBlocking(cfd) == -1)
        {
            safeClose(cfd);
            continue;
        }

        Client c(cfd);
        c.hostname = inet_ntoa(cli.sin_addr);
        _clients.insert(std::make_pair(cfd, c));

        struct pollfd p;
        p.fd = cfd;
        p.events = POLLIN;
        p.revents = 0;
        _pfds.push_back(p);

        sendToFd(cfd, ":server NOTICE * :Welcome! Please PASS/NICK/USER to register\r\n");
    } 
}

void Server::handleClientRead(int fd)
{
    std::map<int, Client>::iterator it = _clients.find(fd);
    if (it == _clients.end())
        return;
    Client &c = it->second;
    char buf[4096];
    while (true)
    {
        ssize_t r = recv(fd, buf, sizeof(buf), 0);
        if (r > 0)
            c.inputBuffer.append(buf, r);
        else if (r == 0)
        {
            disconnectClient(fd, "client closed");
            return;
        }
        else
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            disconnectClient(fd, "recv error");
            return;
        }
    }
    processCompleteLines(c);
}

void Server::handleClientWrite(int fd)
{
    std::map<int, Client>::iterator it = _clients.find(fd);
    if (it == _clients.end())
        return;
    Client &c = it->second;
    while (c.hasPendingWrite())
    {
        std::string &front = c.frontOut();
        ssize_t n = send(fd, front.data(), front.size(), 0);
        if (n > 0)
        {
            front.erase(0, n);
            if (front.empty())
                c.popFrontOut();
        }
        else
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            disconnectClient(fd, "send error");
            return;
        }
    }
    //if queue is empty, close pollout
    if (!c.hasPendingWrite())
    {
        for (std::size_t i = 0; i < _pfds.size(); ++i)
        {
            if (_pfds[i].fd == fd)
            {
                _pfds[i].events &= ~POLLOUT;
                break;
            }
        }
    }
}

void Server::disconnectClient(int fd, const std::string& reason)
{
    (void)reason;
    std::map<int, Client>::iterator it = _clients.find(fd);
    if (it == _clients.end())
        return;
    //Kanallardan çıkarma ve duyurma basit bırakıldı; kanal komutları eklenince doldurulacak.
    _nickToFd.erase(it->second.nickname);
    //pollfd sil
    for (std::size_t i = 0; i < _pfds.size(); ++i)
    {
        if (_pfds[i].fd == fd)
        {
            _pfds.erase(_pfds.begin() + i);
            break;
        }
    }
    safeClose(fd);
    _clients.erase(it);
}

void Server::processCompleteLines(Client& c)
{
    std::string::size_type pos;
    while ((pos = c.inputBuffer.find("\r\n")) != std::string::npos)
    {
        std::string line = c.inputBuffer.substr(0, pos);
        c.inputBuffer.erase(0, pos + 2);
        if (!line.empty() && line.size() <= 512)
            dispatchCommand(c, line);
        else if (line.size() > 512)
        {
            //çok uzun satır: basitçe kırp veya hatayla sessizce geç.
        }
    }
}

static std::string trimLeft(const std::string& s)
{
    std::string::size_type i = 0;
    while (i < s.size() && (s[i] == ' ' || s[i] == '\t'))
        ++i;
    return s.substr(i);
}

void Server::splitIrcLine(const std::string& raw, std::string& cmd, std::vector<std::string>& params)
{
    params.clear();
    cmd.clear();
    std::string s = trimLeft(raw);
    //Prefix varsa (':' ile) atla
    if (!s.empty() && s[0] == ':')
    {
        std::string::size_type sp = s.find(' ');
        if (sp == std::string::npos)
            return;
        s = trimLeft(s.substr(sp));
    }
    //Komut
    std::string::size_type sp = s.find(' ');
    if (sp == std::string::npos)
    {
        cmd = s;
        return;
    }
    cmd = s.substr(0, sp);
    s = trimLeft(s.substr(sp));
    //Parametreler
    while (!s.empty())
    {
        if (s[0] == ':')
        {
            params.push_back(s.substr(1));
            break;
        }
        std::string::size_type sp2 = s.find(' ');
        if (sp2 == std::string::npos)
        {
            params.push_back(s);
            break;
        }
        params.push_back(s.substr(0, sp2));
        s = trimLeft(s.substr(sp2));
    }
}

std::string Server::toUpper(const std::string& x)
{
    std::string r = x;
    for (std::size_t i = 0; i < r.size(); ++i)
    {
        if (r[i] >= 'a' && r[i] <= 'z')
            r[i] = static_cast<char>(r[i] - 'a' + 'A');
    }
    return r;
}

void Server::dispatchCommand(Client& c, const std::string& line)
{
    std::string cmd;
    std::vector<std::string> args;
    splitIrcLine(line, cmd, args);
    cmd = toUpper(cmd);

    if (cmd == "PASS")
        cmdPASS(c, args);
    else if (cmd == "NICK")
        cmdNICK(c, args);
    else if (cmd == "USER")
        cmdUSER(c, args);
    else if (cmd == "PRIVMSG")
        cmdPRIVMSG(c, args);
    else if (cmd == "NOTICE")
        cmdNOTICE(c, args);
    else if (cmd == "QUIT")
        cmdQUIT(c, args);
    else if (cmd == "JOIN")
        cmdJOIN(c, args);
    else if (cmd == "PART")
        cmdPART(c, args);
    //kanal komutları join part topic mode invite kick sonraki aşamalarda eklenecek
    else
        sendTo(c, ":server NOTICE * :Unknow Command\r\n");
}

void Server::sendTo(Client& c, const std::string& msg)
{
    c.enqueue(msg);
    //bu fd için POLLOUT AÇ
    for (std::size_t i = 0; i < _pfds.size(); ++i)
    {
        if (_pfds[i].fd == c.getFd())
        {
            _pfds[i].events |= POLLOUT;
            break;
        }
    }
}

void Server::sendToFd(int fd, const std::string& msg)
{
    std::map<int, Client>::iterator it = _clients.find(fd);
    if (it == _clients.end())
        return;
    sendTo(it->second, msg);
}

void Server::broadcastToChannel(const std::string& chan, const std::string& msg, int exceptFd)
{
    std::map<std::string, Channel>::iterator it = _channels.find(chan);
    if (it == _channels.end())
        return;
    std::set<Client*> snap;
    it->second.membersSnapshot(snap);
    for (std::set<Client*>::iterator ci = snap.begin(); ci != snap.end(); ++ci)
    {
        if ((*ci)->getFd() == exceptFd)
            continue;
        sendTo(**ci, msg);
    }
}

//Kayıt ve Temel Komutlar

void Server::cmdPASS(Client& c, const std::vector<std::string>& args)
{
    if (c.isRegistered)
    {
        sendTo(c, ":server NOTICE * :Already registered\r\n");
        return;
    }
    if (args.empty())
    {
        sendTo(c, ":server NOTICE * :Need more parameters\r\n");
        return;
    }
    if (args[0] != _password)
    {
        disconnectClient(c.getFd(), "bad password");
        return;
    }
    c.hasPassed = true;
}

void Server::cmdNICK(Client& c, const std::vector<std::string>& args)
{
    if (args.empty())
    {
        sendTo(c, ":server NOTICE * :No nickname given\r\n");
        return;
    }
    std::string newNick = args[0];
    if (_nickToFd.count(newNick))
    {
        sendTo(c, ":server NOTICE * :Nickname is already is use\r\n");
        return;
    }
    //nick değişimi yayını
    if (!c.nickname.empty())
        _nickToFd.erase(c.nickname);
    c.nickname = newNick;
    _nickToFd[newNick] = c.getFd();
    c.hasNick = true;
    if (!c.isRegistered && c.hasPassed && c.hasUser)
    {
        c.isRegistered = true;
        sendTo(c, ":server 001 " + c.nickname + " :Welcome to ft_irc\r\n");
    }
}

void Server::cmdUSER(Client& c, const std::vector<std::string>& args)
{
    if (c.hasUser)
    {
        sendTo(c, ":server NOTICE * :You may not reregister\r\n");
        return;
    }
    if (args.size() < 4)
    {
        sendTo(c, ":server NOTICE * :Need more parameters\r\n");
        return;
    }
    c.username = args[0];
    c.realname = args[3];//trailing alanı splitIrcLine ile tek parça geldi
    c.hasUser = true;
    if (!c.isRegistered && c.hasPassed && c.hasNick)
    {
        c.isRegistered = true;
        sendTo(c, ":server 001 " + c.nickname + " :Welcome to ft_irc\r\n");
    }
}

void Server::cmdPRIVMSG(Client& c, const std::vector<std::string>& args)
{
    if (!c.isRegistered)
    {
        sendTo(c, ":server NOTICE * :You have not registered\r\n");
        return;
    }
    if (args.size() < 2)
    {
        sendTo(c, ":server NOTICE * :Need more parameters\r\n");
        return;
    }
    const std::string& target = args[0];
    const std::string& text = args[1];

    if (!target.empty() && target[0] == '#')
    {
        //Kanal mesajı - join henüz yazılmadıysa burada sessizce reddediyoruz
        if (_channels.count(target) == 0)
            return;
        std::string msg = ":" + c.nickname + "!" + c.username + "@host PRIVMSG " + target + " :" + text + "\r\n";
        broadcastToChannel(target, msg, c.getFd());
    }
    else
    {
        //kişiye özel
        std::map<std::string, int>::iterator it = _nickToFd.find(target);
        if (it == _nickToFd.end())
        {
            sendTo(c, ":server NOTICE * :No such nick\r\n");
            return;
        }
        std::string msg = ":" + c.nickname + "!" + c.username + "@host PRIVMSG " + target + " :" + text + "\r\n";
        sendToFd(it->second, msg);
    }
}

void Server::cmdNOTICE(Client& c, const std::vector<std::string>& args)
{
    // RFC'ye göre NOTICE için genelde hata cevapları verilmez.
    if (!c.isRegistered || args.size() < 2)
        return;

    const std::string& target = args[0];
    const std::string& text = args[1];
    
    if (!target.empty() && target[0] == '#')
    {
        // Kanal bildirimi: kanalda değilse bile sessizce geçebiliriz.
        if (_channels.count(target) == 0)
            return;
        std::string msg = ":" + c.nickname + "!" + c.username + "@host NOTICE " + target + " :" + text + "\r\n";
        broadcastToChannel(target, msg, c.getFd());
        return;
    }

    // Kişiye özel NOTICE
    std::map<std::string, int>::iterator it = _nickToFd.find(target);
    if (it == _nickToFd.end())
        return; // NOTICE'ta hata döndürme
    std::string msg = ":" + c.nickname + "!" + c.username + "@host NOTICE " + target + " :" + text + "\r\n";
    sendToFd(it->second, msg);
}

void Server::cmdQUIT(Client& c, const std::vector<std::string>& args)
{
    (void)args;
    disconnectClient(c.getFd(), "quit");
}

void Server::cmdJOIN(Client& c, const std::vector<std::string>& args)
{
    if (!c.isRegistered)
    {
        sendTo(c, ":server NOTICE * :You have not registered\r\n");
        return;
    }
    if (args.empty())
    {
        sendTo(c, ":server NOTICE * :Need more parameters\r\n");
        return;
    }

    std::string chanName = args[0];
    std::string providedKey = (args.size() >= 2) ? args[1] : "";

    if(chanName.empty() || chanName[0] != '#')
    {
        sendTo(c, ":server NOTICE * :Invalid channel name\r\n");
        return;
    }

    Channel* ch = 0;
    std::map<std::string, Channel>::iterator it = _channels.find(chanName);
    if (it == _channels.end())
    {
        _channels.insert(std::make_pair(chanName, Channel(chanName)));
        ch = &_channels.find(chanName)->second;
    }
    else
        ch = &it->second;

    if (ch->isMember(&c))
        return;
    
    if (!ch->key.empty() && ch->key != providedKey)
    {
        sendTo(c, ":server NOTICE * :Cannot join channel (+k)\r\n");
        return;
    }
    if (ch->userLimit != -1 && (int)ch->memberCount() >= ch->userLimit)
    {
        sendTo(c, ":server NOTICE * :Cannot join channel (+l)\r\n");
        return;
    }
    if (ch->inviteOnly && ch->invited.count(&c) == 0)
    {
        sendTo(c, ":server NOTICE * :Cannot join channel (+i)\r\n");
        return;
    }

    bool makeOperator = (ch->memberCount() == 0);
    ch->addMember(&c, makeOperator);
    c.joinedChannels.insert(chanName);
    ch->invited.erase(&c);

    std::string joinMsg = ":" + c.nickname + "!" + c.username + "@host JOIN " + chanName + "\r\n";
    broadcastToChannel(chanName, joinMsg, -1);

    if (!ch->topic.empty())
        sendTo(c, ":server 332 " + c.nickname + " " + chanName + " :" + ch->topic + "\r\n");
    else
        sendTo(c, ":server 331 " + c.nickname + " " + chanName + " :No topic is set\r\n");

    std::set<Client*> snap;
    ch->membersSnapshot(snap);
    std::string names;
    for (std::set<Client*>::iterator ci = snap.begin(); ci != snap.end(); ++ci)
    {
        if (!names.empty())
            names += " ";
        names += (*ci)->nickname;
    }
    sendTo(c, ":server 353 " + c.nickname + " = " + chanName + " :" + names + "\r\n");
    sendTo(c, ":server 366 " + c.nickname + " " + chanName + " :End of /NAMES list.\r\n");
}

void Server::cmdPART(Client& c, const std::vector<std::string>& args)
{
    if (!c.isRegistered)
    {
        sendTo(c, ":server NOTICE * :You have not registered\r\n");
        return;
    }
    if (args.empty())
    {
        sendTo(c, ":server NOTICE * :Need more parameters\r\n");
        return;
    }

    std::string chanName = args[0];
    std::string partMsg = (args.size() >= 2) ? args[1] : "";

    std::map<std::string, Channel>::iterator it = _channels.find(chanName);
    if (it == _channels.end())
    {
        sendTo(c, ":server NOTICE * :No such channel\r\n");
        return;
    }
    Channel& ch = it->second;

    if (!ch.isMember(&c))
    {
        sendTo(c, ":server NOTICE * :You're not on that channel\r\n");
        return;
    }

    std::string msg = ":" + c.nickname + "!" + c.username + "@host PART " + chanName;
    if (!partMsg.empty())
        msg += " :" + partMsg;
    msg += "\r\n";
    broadcastToChannel(chanName, msg, -1);

    ch.removeMember(&c);
    c.joinedChannels.erase(chanName);

    if (ch.memberCount() == 0)
        _channels.erase(it);
}