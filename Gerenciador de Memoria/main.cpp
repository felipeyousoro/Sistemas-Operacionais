/**
 * @file main.cpp
 * @author Felipe Yousoro
 * @brief 
 * @version 1.0
 * @date 2022-12-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */


#include <iostream>
#include <fstream>
#include <string>
#include <bits/stdc++.h>
#include "memory_manager.hpp"

int main(int argc, char *argv[]){
    fstream file;
    file.open(argv[1], ios::in);

    char* algorithm = argv[3];
    for(int i = 0; algorithm[i]; i++){
        algorithm[i] = toupper(algorithm[i]);
    }

    Memory_Manager memory_manager = Memory_Manager(atoi(argv[2]));

    int address;
    int total_inputs = 0;
    while(file >> address){
        if(address == -1){
            memory_manager.print_tlb();
        }
        else if(address == -2){
            memory_manager.print_unloaded_pages();
        }
        else if(address == -3){
            memory_manager.print_loaded_pages();
        }
        else{
            total_inputs++;
            int physical_address = 0;

            if(!strcmp(algorithm, "LRU")){
                physical_address = memory_manager.translate_address_lru(address);
            }
            else if (!strcmp(algorithm, "FIFO")){
                physical_address = memory_manager.translate_address_fifo(address);
            }

            cout << "Virtual address: " << address << " Physical address: " << physical_address << " Value: " << memory_manager.get_value_from_address(physical_address) << endl;
        }
    }

    //print rates
    cout << "\n----- PRINTING STATISTICS -----\n" << endl;
    cout << "Page fault rate: " << 100 * (double) memory_manager.get_total_page_faults() / (double)total_inputs << "%" << endl;
    cout << "TLB hit rate: " << 100 * (double) memory_manager.get_total_tlb_hits() / (double)total_inputs << "%" << endl;
    cout << "Total page faults: " << memory_manager.get_total_page_faults() << endl;
    cout << "Total TLB hits: " << memory_manager.get_total_tlb_hits() << endl;
}
