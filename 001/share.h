#ifndef __SHARE__H
#define __SHARE__H
#include <iostream>

#define PORT 33333


void error_process(const std::string& message)
{
	std::cout << message << std::endl;
	exit(0);
}

#endif//__SHARE__H
