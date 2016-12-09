#include "socket.hh"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <algorithm>
#include <stdexcept>

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
