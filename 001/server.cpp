#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <memory>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
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


int main(int,char**)
{
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1)
		error_process("create socket failed.");	
	socklen_t size = sizeof(sockaddr_in);
	int ret = bind(serverSocket, (sockaddr*)getServerAddress().get(), size);
	if (ret == -1)
		error_process("bind address failed.");
	ret = listen(serverSocket, 5);
	if (ret == -1)
		error_process("listen failed.");
	for(;;)
	{
		int client = accept(serverSocket, (sockaddr*)getServerAddress().get(),& size );
		processClient(client);
	}
	close(serverSocket);
	return 0;
}
