#ifndef UTILS_HPP
#define UTILS_HPP

int ft_port_check(const std::string& port);
int ft_password_check(const std::string& password);
int setNonBlocking(int fd);
void safeClose(int fd);
std::string nowTimeString();

#endif
