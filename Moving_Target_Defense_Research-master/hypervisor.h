#ifndef __hypervisor_H  
#define __hypervisor_H   
#include <vector>
#include <ctime>
#include <string>
#include <cstring>
#include "interface.h"


class Hypervisor{
	unsigned char k_id[3];
	unsigned short num_ip_mtd;
	std::vector<interface> ifs;
public:
	Hypervisor();
	Hypervisor(unsigned char id[]);
	void set_id(unsigned char id[]);
	void add_interface(std::string name, std::string c_ip);
	void remove_interface(std::string name);
	void print_cur_stats();
	void update_interface(std::string n_name, std::string new_ip);
	int comp_id(unsigned char id_cmpr[]); 
};

#endif 
