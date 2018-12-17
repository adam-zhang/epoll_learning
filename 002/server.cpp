#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <memory>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include "shared.h"

using namespace std;

shared_ptr<sockaddr_in> getServerAddress()
{
	static auto p = make_shared<sockaddr_in>();
	p->sin_family = AF_INET;
	p->sin_port = htons(PORT);
	p->sin_addr.s_addr = 0;
	return p;
}

int main(int, char**)
{
	auto s = socket(AF_INET, SOCK_STREAM, 0);
	error_exit(s, "socket failed.");
	auto address = getServerAddress();
	int ret = bind(s, (struct sockaddr*)address.get(), sizeof(sockaddr_in));
	error_exit(ret, "bind failed.");
	ret = listen(s, 5);
	error_exit(ret, "listen failed.");
	for(;;)
	{
		socklen_t length = 0;
		auto client = accept(s, (sockaddr*)address.get(), &length);
		cout << "length:" << length << endl;
		error_exit(client, "accept failed.");
		char buffer[512] = {0};
		string result;
		int n = 0;
		while(n = read(client, buffer, 512) > 0)
		{
			cout << n << " charaters read.\n";
			copy(begin(buffer), begin(buffer) + n, back_inserter(result));
		}
		cout << result << endl;
	}
	close(s);
	return 0;
}
