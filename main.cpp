#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>

int main() {
    // Open a regular file
    int fd = open("serv.cpp", O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // Create epoll instance
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        return 1;
    }

    // Add file to epoll
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        perror("epoll_ctl");
        return 1;
    }

    std::cout << "Waiting for events on regular file..." << std::endl;

    // Wait for events
    struct epoll_event events[10];
    int n = epoll_wait(epoll_fd, events, 10, 5000); // 5 seconds timeout
    if (n == -1) {
        perror("epoll_wait");
    } else if (n == 0) {
        std::cout << "Timeout, no events." << std::endl;
    } else {
        std::cout << "Got " << n << " events!" << std::endl;
        for (int i = 0; i < n; i++) {
            std::cout << "FD " << events[i].data.fd << " is ready to read." << std::endl;
        }
    }

    close(fd);
    close(epoll_fd);
    return 0;
}

