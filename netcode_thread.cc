#include "netcode_thread.hh"
#include "socket.hh"
#include "variables.hh"

#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
#include <chrono>
#include <cstring>
#include <mutex>
#include <string>
#include <thread>
#include <cstdio>

void netcodeThread() {
    std::cout << "Netcode booting..." << std::endl;

    while (true) {
        Socket socket;
        char buf[2048];

        struct sockaddr_in server_address = getServerAddress();
        if (socket.connect(server_address) < 0) {
            std::puts("Connection error.\n");
            std::this_thread::sleep_for(
                std::chrono::milliseconds(200));
            continue;
        }

        std::puts("Connected to RIO\n");

        //Loop to handle all requests
        while (true) {
            //Read a request
            ssize_t bufferLength = socket.read(buf, sizeof(buf));

            if (bufferLength > 0) {
                buf[bufferLength] = 0;
                std::fputs("Got request: ", stdout);
                std::puts(buf);
            } else {
                break;
            }

            //Send a response
            //Retrieve respons from other thread
            {
                auto handle(socketSendXmlBuffer.lock());
                bufferLength = handle->size();
                std::memcpy(buf, handle->c_str(),
                            bufferLength);
            }
            socket.send(buf, bufferLength);
            buf[bufferLength] = 0;
            std::fputs("Sent: ", stdout);
            std::puts(buf);
        }

        std::puts("Disconnected\n");
    }

    std::puts("Client loop failed\n");
}
