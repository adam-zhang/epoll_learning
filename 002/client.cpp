#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <memory>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "shared.h"

using namespace std;

int main(int, char**)
{
	auto s = socket(AF_INET, SOCK_STREAM, 0);
	error_exit(s, "socket failed.");
	sockaddr_in address = {0};
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);
	inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
	auto ret  = connect(s, (sockaddr*)&address, sizeof(sockaddr_in));
	error_exit(ret, "connect failed.");
	string hello = "Hello Server.";
	auto n = write(s, hello.c_str(), hello.size());
	cout << n << " is written." << endl;
	close(s);
	return 0;
}

