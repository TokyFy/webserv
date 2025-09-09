#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/epoll.h>

int main() {
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);

	int opt = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(server_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == -1)
		return 1;

	listen(server_fd, 5);

	int epoll = epoll_create(1);
	struct epoll_event server_event;
	server_event.events = EPOLLIN;
	server_event.data.fd = server_fd;

	epoll_ctl(epoll, EPOLL_CTL_ADD, server_fd , &server_event);

	struct epoll_event all_events[0xA];

	while (true) {
		int queue = epoll_wait(epoll, all_events, 0xA, -1);

		for(int i = 0 ; i < queue ; i++)
		{
			if(all_events[i].data.fd == server_fd)
			{
				int client_fd = accept(server_fd, nullptr, nullptr);
				struct epoll_event client_event;
				client_event.events = EPOLLOUT;
				client_event.data.fd = client_fd;
				epoll_ctl(epoll, EPOLL_CTL_ADD, client_fd, &client_event);
			}
			else
			{
				send(all_events[i].data.fd , "~~", 2 , MSG_DONTWAIT | MSG_NOSIGNAL);
			}
		}
	}

	close(server_fd);
	return 0;
}
