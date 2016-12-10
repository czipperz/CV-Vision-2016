#ifndef HEADER_GUARD_SOCKET_H
#define HEADER_GUARD_SOCKET_H

#include <string>

class Socket {
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    int _fd;
public:
    Socket() noexcept(false);
    ~Socket() noexcept;

    Socket(Socket&&) noexcept;
    Socket& operator=(Socket&&) noexcept;

    int connect(struct sockaddr_in&) noexcept;

    int fd() const noexcept;

    ssize_t read(char* buffer, size_t size, int flags = 0) noexcept;

    ssize_t
    send(const char* message, size_t size, int flags = 0) noexcept;

    ssize_t
    send(const std::string& message, int flags = 0) noexcept;

    friend void swap(Socket&, Socket&) noexcept;
};

void swap(Socket&, Socket&) noexcept;

struct sockaddr_in getServerAddress();

#endif
