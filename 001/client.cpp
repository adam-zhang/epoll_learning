#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <memory>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include "share.h"

using namespace std;

shared_ptr<sockaddr_in> getAddress()
{
	auto p = make_shared<sockaddr_in>();
	p->sin_family = AF_INET;
	p->sin_port = htons(PORT);
	inet_pton(AF_INET, "127.0.0.1", &p->sin_addr);
	return p;
}

int main(int,char**)
{
	int client = socket(AF_INET, SOCK_STREAM, 0);
	connect(client, (sockaddr*)getAddress().get(), sizeof(sockaddr_in));
	string s = "hello server";
	write(client, s.c_str(), s.size() );
	vector<char> buffer(512);
	while(true)
	{
		int n = recv(client, &buffer[0], 512, 0);
		if (n <= 0 )
			break;
		string out(buffer.begin(), buffer.begin() + n);
		cout << out << endl;
	}
	close(client);
}
