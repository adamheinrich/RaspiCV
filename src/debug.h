#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>

#ifdef DEBUG

#define DBG(msg)	do {		\
	std::cout << "[" << __FILE__ << "] " << msg << std::endl;	\
} while (0);

#else

#define DBG(...)

#endif

#define ERR(msg)	do {		\
	std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] " << msg << std::endl;	\
} while (0);

#endif