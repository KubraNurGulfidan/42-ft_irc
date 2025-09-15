#include "Utils.hpp"

#include <string>
#include <cctype>

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

int setNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return -1;
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
        return -1;
    return 0;
}

void safeClose(int fd)
{
    if (fd >= 0)
        while (close(fd) == -1 && errno == EINTR) {}
}

std::string nowTimeString()
{
    char buf[64];
    std::time_t t = std::time(0);
    std::tm tmv = *std::localtime(&t);
	std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tmv);
	return std::string(buf);
}