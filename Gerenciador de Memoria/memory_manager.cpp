/**
 * @file memory_manager.hpp
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

Memory_Manager::Memory_Manager(int physical_memory_size){
    tlb_count = 0;
    available_frame = 0;
    memory_used = 0;

    for(int i = 0; i < TLB_SIZE; i++){
        tlb[i][0] = -1;
        tlb[i][1] = -1;
    }

    for(int i = 0; i < PAGE_TABLE_SIZE; i++){
        page_table[i][0] = -1;
        page_table[i][1] = -1;
        page_table[i][2] = 0;
        used_pages_track[i] = -1;
    }

    this->physical_memory_size = physical_memory_size;
    physical_memory = new int*[physical_memory_size];
    for(int i = 0; i < physical_memory_size; i++){
        physical_memory[i] = new int[PAGE_SIZE];
    }
    for(int i = 0; i < physical_memory_size; i++){
        for(int j = 0; j < physical_memory_size; j++){
            physical_memory[i][j] = -1;
        }
    }

    for(int i = 0; i < TLB_SIZE; i++){
        tlb_timestamp[i] = 0;
    }

    total_page_faults = 0;
    total_tlb_hits = 0;
}

bool Memory_Manager::is_full(){
    return memory_used == physical_memory_size;
}

void Memory_Manager::shift_tlb(){
    for(int i = 0; i < TLB_SIZE - 1; i++){
        tlb[i][0] = tlb[i+1][0];
        tlb[i][1] = tlb[i+1][1];
    }
}

int Memory_Manager::get_tlb_index(int page_number){
    for(int i = 0; i < TLB_SIZE; i++){
        if(tlb[i][0] == page_number){
            return i;
        }
    }
    return -1;
}

int Memory_Manager::get_frame_number(int page_number){
    for(int i = 0; i < PAGE_TABLE_SIZE; i++){
        if(page_table[i][0] == page_number){
            return page_table[i][1];
        }
    }
    return -1;
}

void Memory_Manager::update_physical_memory(char buffer[PAGE_SIZE]){
    for(int i = 0; i < PAGE_SIZE; i++){
        physical_memory[available_frame][i] = buffer[i];
    }
}

void Memory_Manager::update_pages_track(int page_number){
    for(int i = 0; i < PAGE_TABLE_SIZE; i++){
        if(used_pages_track[i] == -1){
            used_pages_track[i] = page_number;
            break;
        }
    }
}

int Memory_Manager::handle_page_fault_fifo(int virtual_address){
    fstream backing_store;
    backing_store.open("BACKING_STORE.bin", ios::in | ios::binary);

    int page_number = GET_PAGE_NUMBER(virtual_address);
    int offset = GET_OFFSET(virtual_address);
    int frame = -1;

    char buffer[PAGE_SIZE];
    backing_store.seekg(page_number * PAGE_SIZE, ios::beg);
    backing_store.read(buffer, PAGE_SIZE);

    if(is_full() == false){
        update_physical_memory(buffer);

        page_table[page_number][0] = page_number;
        page_table[page_number][1] = available_frame;
        page_table[page_number][2] = 1;

        bool found = false;
        for(int i = 0; i < PAGE_TABLE_SIZE; i++){
            if(used_pages_track[i] == page_number){
                found = true;
                break;
            }

        }

        if(found == false){
            update_pages_track(page_number);
        }

        available_frame++;
        memory_used++;

        frame = page_table[page_number][1];
    }
    else{
        int page_to_remove = used_pages_track[0];

        page_table[page_number][0] = page_number;
        page_table[page_number][1] = page_table[page_to_remove][1];
        page_table[page_number][2] = 1;

        page_table[page_to_remove][2] = 0;

        frame = page_table[page_to_remove][1];

        int update_aux = 0;
        for(int i = 0; i < PAGE_TABLE_SIZE - 1; i++){
            if(used_pages_track[i + 1] == -1){
                update_aux = i;
                break;
            }

            used_pages_track[i] = used_pages_track[i + 1];
        }
        
        used_pages_track[update_aux + 1] = page_to_remove;

        for(int i = 0; i < PAGE_SIZE; i++){
            physical_memory[page_table[page_number][1]][i] = buffer[i];
        }
    }

    backing_store.close();

    return frame;
}

int Memory_Manager::handle_page_fault_lru(int virtual_address){
    fstream backing_store;
    backing_store.open("BACKING_STORE.bin", ios::in | ios::binary);

    int page_number = GET_PAGE_NUMBER(virtual_address);
    int offset = GET_OFFSET(virtual_address);
    int frame = -1;

    char buffer[PAGE_SIZE];
    backing_store.seekg(page_number * PAGE_SIZE, ios::beg);
    backing_store.read(buffer, PAGE_SIZE);


    if(is_full() == false){
        update_physical_memory(buffer);

        page_table[page_number][0] = page_number;
        page_table[page_number][1] = available_frame;
        page_table[page_number][2] = 1;
 
        bool found = false;
        for(int i = 0; i < PAGE_TABLE_SIZE; i++){
            if(used_pages_track[i] == page_number){
                int moving_value = used_pages_track[i];
                int update_aux = i;

                for(int j = i; j < PAGE_TABLE_SIZE - 1; j++){
                    if(used_pages_track[j] == -1){
                        update_aux = j;
                        break;
                    }

                    used_pages_track[j] = used_pages_track[j + 1];
                }

                used_pages_track[update_aux] = moving_value;

                found = true;
                break;
            }

        }

        if(found == false){
            update_pages_track(page_number);
        }

        available_frame++;
        memory_used++;

        frame = page_table[page_number][1];
    }
    else{
        int page_to_remove = used_pages_track[0];

        page_table[page_number][0] = page_number;
        page_table[page_number][1] = page_table[page_to_remove][1];
        page_table[page_number][2] = 1;

        page_table[page_to_remove][2] = 0;

        frame = page_table[page_to_remove][1];

        int update_aux = 0;
        for(int i = 0; i < PAGE_TABLE_SIZE - 1; i++){
            if(used_pages_track[i] == -1){
                update_aux = i;
                break;
            }

            used_pages_track[i] = used_pages_track[i + 1];
        }
        
        used_pages_track[update_aux + 1] = page_number;

        for(int i = 0; i < PAGE_SIZE; i++){
            physical_memory[page_table[page_number][1]][i] = buffer[i];
        }
    }

    backing_store.close();

    return frame;
}

int Memory_Manager::translate_address_fifo(int virtual_address){
    bool hit = false;

    int frame_number = -1;
    for(int i = 0; i < TLB_SIZE; i++){
        if (tlb[i][0] == GET_PAGE_NUMBER(virtual_address)){
            frame_number = tlb[i][1];
            hit = true;
            total_tlb_hits++;
            break;
        }
    }

    if (!hit){
        frame_number = get_frame_number(GET_PAGE_NUMBER(virtual_address));

        if(frame_number == -1){
            frame_number = handle_page_fault_fifo(virtual_address);
            total_page_faults++;
        }

        int index = get_tlb_index(GET_PAGE_NUMBER(virtual_address));

        if(index == -1){
            int update_index = 0;
            if(tlb_count == TLB_SIZE){
                shift_tlb();

                update_index = TLB_SIZE - 1;
                tlb[TLB_SIZE - 1][0] = GET_PAGE_NUMBER(virtual_address);
                tlb[TLB_SIZE - 1][1] = frame_number;
            }
            else{
                update_index = tlb_count;
                tlb_count++;
            }

            tlb[update_index][0] = GET_PAGE_NUMBER(virtual_address);
            tlb[update_index][1] = frame_number;
        }
    }

    int physical_address = (frame_number * PAGE_SIZE) + GET_OFFSET(virtual_address);

    return physical_address;
}

int Memory_Manager::translate_address_lru(int virtual_address){
    bool hit = false;
    int frame_number = -1;

    for(int i = 0; i < TLB_SIZE; i++){
        if (tlb[i][0] == GET_PAGE_NUMBER(virtual_address)){
            frame_number = tlb[i][1];
            hit = true;
            total_tlb_hits++;
            break;
        }
    }

    if (!hit){
        frame_number = get_frame_number(GET_PAGE_NUMBER(virtual_address));

        if(frame_number == -1){
            frame_number = handle_page_fault_fifo(virtual_address);
            total_page_faults++;
        }

        // Updating the TLB
        bool found = false;
        for(int i = 0; i < tlb_count; i++){
            if(tlb[i][0] == GET_PAGE_NUMBER(virtual_address)){
                int moving_value = tlb[i][0];

                for(int j = i; j < TLB_SIZE - 1; j++){
                    tlb[j][0] = tlb[j + 1][0];
                    tlb[j][1] = tlb[j + 1][1];
                }

                found = true;
                break;
            }
        }

        if(found == false){
            int update_index = 0;
            if(tlb_count == TLB_SIZE){
                shift_tlb();

                update_index = TLB_SIZE - 1;
                tlb[TLB_SIZE - 1][0] = GET_PAGE_NUMBER(virtual_address);
                tlb[TLB_SIZE - 1][1] = frame_number;
            }
            else{
                update_index = tlb_count;
                tlb_count++;
            }

            tlb[update_index][0] = GET_PAGE_NUMBER(virtual_address);
            tlb[update_index][1] = frame_number;

        }

    }

    int physical_address = (frame_number * PAGE_SIZE) + GET_OFFSET(virtual_address);
    
    return physical_address;
}


int Memory_Manager::get_value_from_address(int physical_address){
    return physical_memory[GET_PAGE_NUMBER(physical_address)][GET_OFFSET(physical_address)];
}

void Memory_Manager::print_tlb(){
    cout << "\n----- PRINTING TLB -----\n" << endl;
    for(int i = 0; i < TLB_SIZE; i++){
        cout << "At index " << i << ": ";
        cout << "Page number: " << tlb[i][0] << " | ";
        cout << "Frame number: " << tlb[i][1] << endl;
    }
    cout << "\n----- END OF TLB -----\n" << endl;
}

void Memory_Manager::print_unloaded_pages(){
    cout << "\n----- PRINTING UNLOADED PAGES -----\n" << endl;
    for(int i = 0; i < PAGE_TABLE_SIZE; i++){
        if (page_table[i][2] == 0){
            cout << "Page number: " << i << " | ";
            cout << "Frame number: " << page_table[i][1] << endl;
        }
    }
    cout << "\n----- END OF UNLOADED PAGES -----\n" << endl;
    cout << endl;
}

void Memory_Manager::print_loaded_pages(){
    cout << "\n----- PRINTING LOADED PAGES -----\n" << endl;
    for(int i = 0; i < PAGE_TABLE_SIZE; i++){
        if (page_table[i][2] == 1){
            cout << "Page number: " << i << " | ";
            cout << "Frame number: " << page_table[i][1] << endl;
        }
    }
    cout << "\n----- END OF LOADED PAGES -----\n" << endl;
}

int Memory_Manager::get_total_page_faults(){
    return total_page_faults;
}

int Memory_Manager::get_total_tlb_hits(){
    return total_tlb_hits;
}
