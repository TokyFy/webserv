#include <iostream>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sstream>
#include <fcntl.h>
#include <iomanip>
#include <signal.h>
#include <iostream>

#define PORT 8080

int main() {
    signal(SIGPIPE, SIG_IGN);
    int server_fd, client_fd;

    // Create IPv6 socket
    server_fd = socket(AF_INET6, SOCK_STREAM, 0);

    // Allow address reuse
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind to [::]:PORT
    struct sockaddr_in6 addr;
    addr.sin6_family = AF_INET6;
    addr.sin6_addr = in6addr_any;
    addr.sin6_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    

    listen(server_fd, 10); 

    std::cout << "Listening on [::]:" << PORT << std::endl;

    while (true) {
        struct sockaddr_in6 client_addr;
        socklen_t client_len = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);


        char buffer[1024] = {0};
        recv(client_fd, buffer, sizeof(buffer) - 1, 0);

        const char *response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: multipart/form-data\r\n"
            "Transfer-Encoding: chunked\r\n"
            "Connection: close\r\n"
            "\r\n";

        write(client_fd, response, strlen(response));

        int i = 0;
        while (true) { 
            std::string chunk = "18\r\n424242424242424242424242\r\n";

            if(write(client_fd, chunk.c_str(), chunk.size()) == -1)
                    break;
            i++;
        }
        close(client_fd);
    }

    close(server_fd);
    return 0;
}

