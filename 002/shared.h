#ifndef __SHARED__H
#define __SHARED__H

#define PORT 3333

#define error_exit(value, message) \
	if (value == -1) \
		cout << message << endl;

#endif//__SHARED__H
