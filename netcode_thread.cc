#include "netcode_thread.hh"
#include "socket.hh"
#include "variables.hh"

#include <netdb.h>
#include <sys/types.h>
#include <unistd.h>
#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

void netcodeThread() {
    std::cout << "Netcode booting..." << std::endl;

    while (true) {
        Socket socket;
        char buf[2048];

        struct sockaddr_in server_address = getServerAddress();
        if (socket.connect(server_address) < 0) {
            std::cout << "Connect error" << std::endl;
            std::this_thread::sleep_for(
                std::chrono::milliseconds(200));
            continue;
        }

        std::cout << "Connected to RIO\n" << std::endl;

        //Loop to handle all requests
        while (true) {
            //Read a request
            //memset(buf,0,strlen(buf));

            ssize_t bufferLength = socket.read(buf, sizeof(buf));

            if (bufferLength > 0) {
                buf[bufferLength] = 0;
                std::cout << "Got Request: " << buf << std::endl;
            } else {
                break;
            }

            //Send a response
            {
                std::lock_guard<std::mutex> lock(mutex);
                bufferLength = socketSendXmlBuffer.size();
                std::memcpy(buf, socketSendXmlBuffer.c_str(),
                            bufferLength);
            }
            socket.send(buf, bufferLength);
            buf[bufferLength] = 0;
            std::cout << "Sent: " << buf << std::endl;
        }

        std::cout << "Disconnected\n" << std::endl;
    }

    std::cout << "Client loop failed\n" << std::endl;
}
