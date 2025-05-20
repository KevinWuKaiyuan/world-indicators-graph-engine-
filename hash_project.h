#ifndef HASH_PROJECT_H
#define HASH_PROJECT_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "TimeSeries.h"
#include "linked_list.h"
#include "countries_static_arr.h"

//#define HASH_SIZE 512

class linked_list;
class country_element;

const int hash_size = 512;

struct hash_element{
    country_element* hash_country;
    std::string status; //EMPTY, OCCUPIED, PREVIOUSLY_OCCUPIED
    //std::string hash_country_code=" "; // country code
    int hashed_time = 0; // i
    hash_element() : hash_country(nullptr), status("EMPTY"), hashed_time(0) {}
};

class hash_table{
        private:
            hash_element hash_country_array[hash_size];

        public:

        hash_element get_hash_data(int index) const {
            if (index >= 0 && index < hash_size) {
                return hash_country_array[index];
            }
            return hash_element();  // ✅ Return default empty element if out of bounds
        }
        
        
            hash_table();                  
            ~hash_table();
            void hash_LOOKUP(std::string hash_country_code);
            bool hash_REMOVE(std::string hash_country_code);
            bool hash_INSERT(const std::string& hash_country_code, 
                const std::string& hash_country_name);
            
            bool real_hash_Insert(std::string hash_country_code, const std::string& hash_filename, country_element& country);
            
            void hash_EXIT();
            //helper functions
            int primary_hash(std::string hash_country_code);
            int secondary_hash(std::string hash_country_code);
            int overall_hash(std::string hash_country_code, int i);

        hash_element *get_hash_element(){return this->hash_country_array;}
        // return hash_element(); // Return default empty element if out of bounds
    };

#endif