#include <vector>
#include <ctime>
#include <string>
#include <cstring>
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include "hypervisor.h"
#include "interface.h"

Hypervisor::Hypervisor(){
	num_ip_mtd = 0;
}

Hypervisor::Hypervisor(unsigned char id[]){
	k_id[0] = id[0];
	k_id[1] = id[1];
	k_id[2] = id[2];
	num_ip_mtd = 0;
}
void Hypervisor::add_interface(std::string name, std::string c_ip){
	struct interface temp_if;
	temp_if.name = name;
	temp_if.cur_ip = c_ip;
	temp_if.num_mtd = 0;
	ifs.push_back(temp_if);
}
	
void Hypervisor::print_cur_stats(){
	std::cout << std::setw(12) << std::left << "IF NAME";
    std::cout << std::setw(18) << std::left << "CURRENT IP";
    std::cout << std::setw(12) << std::left << "NUM MTD"<<std::endl;
	for (int i = 0; i < ifs.size(); i++){
		std::cout << std::setw(12) << std::left << ifs[i].name.c_str();
    	std::cout << std::setw(20) << std::left << ifs[i].cur_ip;
    	std::cout << std::setw(12) << std::left << ifs[i].num_mtd<<std::endl;
	}
	printf("\nNumber of IP-MTD's Perfomed on this Hypervisor: %d\n", num_ip_mtd);
}
void Hypervisor::set_id(unsigned char id[]){
	k_id[0] = id[0];
	k_id[1] = id[1];
	k_id[2] = id[2];
}
int Hypervisor::comp_id(unsigned char id_cmpr[]){
	return memcmp(k_id, id_cmpr, 3);
} 

void Hypervisor::update_interface(std::string n_name, std::string new_ip){
	int index;
	for(int i = 0; i < ifs.size(); i++){
		if(n_name == ifs[i].name){
			index = i;
			break;
		}
	}
	ifs[index].num_mtd++;
	ifs[index].former_ips.push_back(ifs[index].cur_ip);
	ifs[index].cur_ip = new_ip;
	ifs[index].name = n_name;
	num_ip_mtd++;
}
