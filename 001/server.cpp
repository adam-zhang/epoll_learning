#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <memory>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include "share.h"

using namespace std;


shared_ptr<sockaddr_in> getServerAddress()
{
	auto p = make_shared<sockaddr_in>();
	p->sin_family = AF_INET;
	p->sin_port = htons(PORT);
	p->sin_addr.s_addr = 0;
	return p;
}

void processClient(int fd)
{
	vector<char> buffer(512);
	int n = read(fd, &buffer[0], 512);
	string s = string("welcome to server:") + &buffer[0];
	write(fd, s.c_str(), s.size());
	close(fd);
}

string readClient(int client)
{
	vector<char> buffer(512);
	string out;
	cout << "before read\n";
	int n = 0;
	if(n = read(client, &buffer[0], 512) != 0)
	{
		cout << n << " byte is read.\n";
		copy(buffer.begin(), buffer.begin() + n, back_inserter(out));
	}
	cout << "errno:" << errno << endl;
	while(errno == EAGAIN)
	{
		cout << "EAGAIN\n";
		n = read(client, &buffer[0], 512);
		copy(buffer.begin(), buffer.begin() + n, back_inserter(out));
	}
	cout << "out size:" << out.size() << endl;
	return out;
}

int setnonblocking(int sockfd)  
{ 
	if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK) == -1)
		return -1; 
	return 0; 
} 

int main(int,char**)
{
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1)
		error_process("create socket failed.");	
	socklen_t size = sizeof(sockaddr_in);
	auto address = getServerAddress();
	int opt = 0;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	int ret = bind(serverSocket, (sockaddr*)address.get(), size);
	if (ret == -1)
		error_process("bind address failed.");
	struct epoll_event event, events[1024];
	ret = listen(serverSocket, 5);
	if (ret == -1)
		error_process("listen failed.");
	int epoll = epoll_create(1);
	if (ret == -1)
		error_process("epoll failed.");
	event.events = EPOLLIN;
	event.data.fd = serverSocket;
	if (epoll_ctl(epoll, EPOLL_CTL_ADD, serverSocket, &event) == -1)
		error_process("epoll_ctl failed.");
	for(;;)
	{
		int wait = epoll_wait(epoll, events, 1024, -1);
		if (wait == -1)
			error_process("epoll wait failed.");
		for(auto i = 0; i != wait; ++i)
		{
			if (events[i].data.fd == serverSocket)
			{
				int client = accept(serverSocket, (sockaddr*)address.get(), &size );
				if (client == -1)
					error_process("accept failed.");
				setnonblocking(client);
				event.events = EPOLLIN|EPOLLET;
				event.data.fd = client;
				if (epoll_ctl(epoll, EPOLL_CTL_ADD, client, &event) == -1)
					error_process("epoll_ctl process client failed.");

			}
			else
			{
				cout << "process client\n";
				int client = events[i].data.fd;
				cout << "process epollin." << endl;

				string read = readClient(client);
				cout << "read:" << read << endl;
				string out = "welcome:" + read;
				cout << out << endl;
				write(client, out.c_str(), out.size());
				epoll_ctl(epoll, EPOLL_CTL_DEL, client, NULL);

			}
		}

		//int client = accept(serverSocket, (sockaddr*)getServerAddress().get(),& size );
		//processClient(client);
	}
	close(serverSocket);
	return 0;
}
