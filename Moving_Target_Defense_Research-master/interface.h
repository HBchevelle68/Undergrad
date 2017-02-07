#ifndef __interface_H  
#define __interface_H
#include <string>

/*
 *	@struct inteface - structure, contains data pertaining to individual interfaces on kernel
 */
struct interface
{
	std::string name;
	std::string cur_ip;
	std::vector<std::string> former_ips;
	time_t last_mov;
	unsigned short num_mtd; // number of times this interace choosen as MTD target
};


#endif  
