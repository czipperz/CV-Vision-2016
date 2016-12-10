#include "socket.hh"
#include "variables.hh"
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <thread>

Socket::Socket() noexcept(false)
    : _fd(socket(AF_INET, SOCK_STREAM, 0)) {
    if (_fd < 0) {
        throw std::runtime_error("Could not open a socket.");
    }
}

Socket::~Socket() noexcept {
    if (_fd != -1) {
        close(_fd);
    }
}

Socket::Socket(Socket&& other) noexcept : _fd(-1) {
    *this = std::move(other);
}
Socket& Socket::operator=(Socket&& other) noexcept {
    swap(*this, other);
    return *this;
}

int Socket::connect(struct sockaddr_in& sockaddr) noexcept {
    return ::connect(_fd,
                     reinterpret_cast<struct sockaddr*>(&sockaddr),
                     sizeof(sockaddr));
}

int Socket::fd() const noexcept { return _fd; }

ssize_t Socket::read(char* buffer, size_t size, int flags) noexcept {
    return recv(_fd, buffer, size, flags);
}

ssize_t
Socket::send(const char* message, size_t size, int flags) noexcept {
    return ::send(_fd, message, size, flags);
}

ssize_t
Socket::send(const std::string& message, int flags) noexcept {
    return send(message.c_str(), message.size(), flags);
}

void swap(Socket& a, Socket& b) noexcept {
    using std::swap;
    swap(a._fd, b._fd);
}

struct sockaddr_in getServerAddress() {
    //http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/client.c
    struct sockaddr_in serv_addr;
    struct hostent* server = gethostbyname(hostName);

    while (server == NULL) {
        std::cout << "No such host" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        server = gethostbyname(hostName);
    }

    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    std::memcpy(server->h_addr, &serv_addr.sin_addr.s_addr,
                server->h_length);
    //Convert small endian/big endian
    serv_addr.sin_port = htons(portno);
    return serv_addr;
}
