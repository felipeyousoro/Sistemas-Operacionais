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

using namespace std;

const int PAGE_TABLE_SIZE = 256;
const int PAGE_SIZE = 256;
const int TLB_SIZE = 16;
const int MASK = 255;

#define GET_PAGE_NUMBER(x) ( (x >> 8) & MASK )
#define GET_OFFSET(x) ( x & MASK )

class Memory_Manager{
    private:
            int tlb[TLB_SIZE][2];
            int tlb_count;

            int page_table[PAGE_TABLE_SIZE][3];
            // page_table[][0] = page number
            // page_table[][1] = frame number
            // page_table[][2] = valid bit
            int used_pages_track[PAGE_TABLE_SIZE];

            int physical_memory_size;
            int **physical_memory;

            // This is to keep track of the last used frame
            int available_frame;
            int memory_used;
            
            // This is to keep track of the least recently used page
            // to use in the LRU algorithm
            int tlb_timestamp[TLB_SIZE];
    
            int total_page_faults;
            int total_tlb_hits;
    public:
            /** 
             * @brief Class constructor
             * @param Number of frames in the physical memory
             */
            Memory_Manager(int physical_memory_size);

            /**
             * @brief Check if the memory is full
             * @return true : if the memory is full
             * @return false : otherwise
             */
            bool is_full();

            /**
             * @brief Shift the TLB to the right
             */
            void shift_tlb();

            /**
             * @brief Gets the index of the page in the TLB
             * @param page_number : the page number to be searched
             * @return int : the index of the page in the TLB
             */
            int get_tlb_index(int page_number);
    
            /**
             * @brief Gets the frame number of the page
             * @param page_number : the page number to be searched
             * @return int : the frame number of the page
             */
            int get_frame_number(int page_number);

            /**
             * @brief Updates the physical memory
             * @param buffer : the buffer to be written in the physical memory
             */ 
            void update_physical_memory(char buffer[PAGE_SIZE]);

            /**
             * @brief Updates the pages track
             * @param page_number : the page number that will be changed in the track
             */
            void update_pages_track(int page_number);


            /**
             * @brief Function used to handle pages fault using the FIFO algorithm.
             * 
             * It will access the file BACKING_STORE.bin and read the page, then it will store
             * it in the physical memory.
             *
             * @param virtual_address : virtual address of the missing page
             * @return int : frame number of the page
             */
            int handle_page_fault_fifo(int virtual_address);

            /**
             * @brief Function used to handle pages fault using the LRU algorithm.
             * 
             * It will access the file BACKING_STORE.bin and read the page, then it will store
             * it in the physical memory.
             *
             * @param virtual_address : virtual address of the missing page
             * @return int : frame number of the page
             */
            int handle_page_fault_lru(int virtual_address);

            /**
             * @brief Translate a virtual address into a physical address using 
             * the FIFO algorithm to handle page faults.
             * 
             * It will first check if the page is in the TLB, if not, it will check if it is in the page table.
             * If it is not in the page table, it will call the handle_page_fault function.
             * 
             * @param virtual_address : virtual address of the page
             * @return int : physical address of the page
             */
            int translate_address_fifo(int virtual_address);

            /** 
             * @brief Translate a virtual address into a physical address using 
             * the LRU algorithm to handle page faults.
             * 
             * It will first check if the page is in the TLB, if not, it will check if it is in the page table.
             * If it is not in the page table, it will call the handle_page_fault function.
             * 
             * @param virtual_address : virtual address of the page
             * @return physical address of the page
             */
            int translate_address_lru(int virtual_address);

            /**
             * @brief Get in the memory the value of a physical address
             * @param physical_address : physical address of the page
             * @return value in memory of the physical address
             */
            int get_value_from_address(int physical_address);
            
            /**
             * @brief Print the content of the TLB
             */
            void print_tlb();

            /**
             * @brief Print the content of unloaded pages
             */
            void print_unloaded_pages();

            /**
             * @brief Print the content of loaded pages
             */
            void print_loaded_pages();

            /**
             * @brief Get the total number of page faults
             * @return total number of page faults
             */
            int get_total_page_faults();

            /**
             * @brief Get the total number of TLB hits
             * @return total number of TLB hits
             */
            int get_total_tlb_hits();
};