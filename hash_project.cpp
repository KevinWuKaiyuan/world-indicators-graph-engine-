#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <vector>  
#include <cmath> 
#include "hash_project.h"
#include "TimeSeries.h"
#include "linked_list.h"
#include "countries_static_arr.h"

hash_table::hash_table(){
    for(int i=0; i < hash_size; i++){
        hash_country_array[i].status = "EMPTY";
        hash_country_array[i].hash_country = nullptr;
        hash_country_array[i].hashed_time = 0;
    }
}
/*
CITATION:
Modified with the prompt:
"Help me avoid memory leaks when deleting hash entries."
The AI ensured deletion of only OCCUPIED entries and safely avoids dangling pointers.
*/
hash_table::~hash_table(){
    for(int i=0; i < hash_size; i++){
        //bool h_status = true;
        if(hash_country_array[i].status == "OCCUPIED"){
            delete hash_country_array[i].hash_country;
        }
    }
}

int hash_table::primary_hash(std::string hash_country_code){
    int w = 0;
    w += (hash_country_code[0] - 'A') * 26 * 26;
    w += (hash_country_code[1] - 'A') * 26;
    w += (hash_country_code[2] - 'A');
    return w % hash_size;
   // std::cout<<"hashone"<<w % hash_size;
}

/*
CITATION:
Based on guidance from GPT prompt:
"Add a secondary hash to reduce primary clustering in double hashing."
The AI verified this always returns a non-zero, odd number.
*/
int hash_table::secondary_hash(std::string hash_country_code){
    int w = 0;
    w += (hash_country_code[0] - 'A') * 26 * 26;
    w += (hash_country_code[1] - 'A') * 26;
    w += (hash_country_code[2] - 'A');
    int mod = (w/hash_size)%hash_size; // Avoid 0 ??
    // std::cout<<"two"<<(mod % 2 == 0) ? (mod + 1) : mod;
    return (mod % 2 == 0) ? (mod + 1) : mod; // Ensure odd ??
}

int hash_table::overall_hash(std::string hash_country_code, int i){
    int h1 = primary_hash(hash_country_code);
    int h2 = secondary_hash(hash_country_code);
    // std::cout<<"overall"<<(h1 + i * h2) % hash_size;
    return (h1 + i * h2) % hash_size;
}

/*
CITATION:
Built with GPT guidance under:
"Implement INSERT with double hashing, preventing duplicates and memory leaks."
GPT included file parsing and time series loading logic from project 3 restructured into hash format.
*/
bool hash_table::hash_INSERT(const std::string& hash_country_code,  // this is a helper function for LOAD command 
    const std::string& hash_country_name)  // <-- CHANGED
{
int i = 0;
int initial_index = overall_hash(hash_country_code, i);

// Check if that same code is already at the initial index
if (hash_country_array[initial_index].status == "OCCUPIED" &&
hash_country_array[initial_index].hash_country->getCountryCode() == hash_country_code)
{
return false;
}

// Allocate a new country_element without re-reading the CSV file
// Because we already know the country name from tree_load's first pass
country_element* new_country = new country_element();  // <-- CHANGED
new_country->setCountryCode(hash_country_code);
new_country->setCountryName(hash_country_name);

// Attempt to place it in the open-addressing hash array
if (hash_country_array[initial_index].status == "EMPTY" ||
hash_country_array[initial_index].status == "PREVIOUSLY_OCCUPIED")
{
hash_country_array[initial_index].hash_country = new_country;
hash_country_array[initial_index].status = "OCCUPIED";
hash_country_array[initial_index].hashed_time = i;
return true;
}

// Keep probing
i = 1;
while (i < hash_size) {
int index = overall_hash(hash_country_code, i);
if (hash_country_array[index].status == "OCCUPIED" &&
hash_country_array[index].hash_country != nullptr &&
hash_country_array[index].hash_country->getCountryCode() == hash_country_code)
{
// Duplicate
delete new_country;
return false;
}
if (hash_country_array[index].status == "EMPTY" ||
hash_country_array[index].status == "PREVIOUSLY_OCCUPIED")
{
hash_country_array[index].hash_country = new_country;
hash_country_array[index].status = "OCCUPIED";
hash_country_array[index].hashed_time = i;
return true;
}
i++;
}
// If we never found a spot:
delete new_country;
return false;
}
/*
bool hash_table::real_hash_Insert(std::string hash_country_code, const std::string& hash_filename, country_element& country){  // COMMAND INSERT
    //
    int i = 0;
    int initial_index = overall_hash(hash_country_code, i);
    
    if(hash_country_array[initial_index].status=="OCCUPIED"&&hash_country_array[initial_index].hash_country->getCountryCode()==hash_country_code){
        //std::cout<<"failure\n";
        return false;
    }

    std::ifstream file(hash_filename);
    if (!file.is_open()) {
        std::cout << "failure\n";
        return false;
    }

    country_element* new_country = new country_element(); //decalred a new country element
    new_country->setCountryCode(hash_country_code);
    new_country->setCountryName("");

    std::string line;
    bool found = false;
    std::string full_country_name;
    while (std::getline(file,line)){
        std::stringstream ss(line);
        std::string code, name;
        std::getline(ss, name, ',');
        std::getline(ss, code, ',');
        if (code==hash_country_code){
            if(!found){
                full_country_name = name;
                new_country ->setCountryName(full_country_name);
                found = true;
            }
            new_country->load_time_series(full_country_name, line);
        }
    }

    // insert into the country_list
    int index = 0;
    while(index < 512){  // trying to find an empty place for the new country in the country_list
        if(country.country_list[index].get_status() == "EMPTY"){
            break;
        }
        index ++;
    }

    //forget to update valid_count
    file.close();
    if(!found){
        delete new_country;
        //std::cout<<"failure\n";
        return false;
    }

    if (hash_country_array[initial_index].status == "EMPTY" || 
        hash_country_array[initial_index].status == "PREVIOUSLY_OCCUPIED") {
        //    //////
        //    country_list[index] = elem.hash_country->get_linked_list();  // ✅ Only copy the linked list

        hash_country_array[initial_index].hash_country = new_country;
        hash_country_array[initial_index].status = "OCCUPIED";
        hash_country_array[initial_index].hashed_time = i;
       // std::cout << "success[1]\n";
        return true;
    }
    i=1;
    while (i < hash_size) {
        int index = overall_hash(hash_country_code, i);
        // Check for duplicate during probing
        if (hash_country_array[index].status == "OCCUPIED" &&
            hash_country_array[index].hash_country != nullptr &&
            hash_country_array[index].hash_country->getCountryCode() == hash_country_code) {
            delete new_country;
            //std::cout << "failure\n";
            return false;
        }
        if (hash_country_array[index].status == "EMPTY" || 
            hash_country_array[index].status == "PREVIOUSLY_OCCUPIED") {
                // 
                // country_list[index] = elem.hash_country;  // ✅ Now just copy the pointer

            hash_country_array[index].hash_country = new_country;
            hash_country_array[index].status = "OCCUPIED";
            hash_country_array[index].hashed_time = i;
            //std::cout << "success[2]\n";
            return true;
        }
        i++;
    }
    delete new_country;
   // std::cout << "failure\n";
    return false;
}*/
bool hash_table::real_hash_Insert(std::string hash_country_code, 
    const std::string& hash_filename, 
    country_element& country) 
{
int i = 0;
int initial_index = overall_hash(hash_country_code, i);

// 若初始位置就有相同代码
if (hash_country_array[initial_index].status == "OCCUPIED" &&
hash_country_array[initial_index].hash_country->getCountryCode() == hash_country_code) 
{
return false;
}

// 读取文件
std::ifstream file(hash_filename);
if (!file.is_open()) {
std::cout << "failure\n";
return false;
}

// 创建新的 country_element
country_element* new_country = new country_element();
new_country->setCountryCode(hash_country_code);
new_country->setCountryName("");

std::string line;
bool found = false;
std::string full_country_name;
while (std::getline(file, line)) {
std::stringstream ss(line);
std::string name, code;
std::getline(ss, name, ',');
std::getline(ss, code, ',');

if (code == hash_country_code) {
if (!found) {
full_country_name = name;
new_country->setCountryName(full_country_name);
found = true;
}
// 将 CSV 当前行的数据加载进 new_country 的 linked_list
new_country->load_time_series(full_country_name, line);
}
}
file.close();

// 如果在文件中没找到对应的 hash_country_code，就失败
if (!found) {
delete new_country;
return false;
}

// =============== 尝试放进哈希表 ===============
// 先检查初始槽位
if (hash_country_array[initial_index].status == "EMPTY" || 
hash_country_array[initial_index].status == "PREVIOUSLY_OCCUPIED") 
{
hash_country_array[initial_index].hash_country = new_country;
hash_country_array[initial_index].status = "OCCUPIED";
hash_country_array[initial_index].hashed_time = i;

// -------- 关键：同时把其链表挂到 country.country_list[initial_index] ----------
country.country_list[initial_index] = new_country->get_linked_list();
country.country_list[initial_index].set_status("OCCUPIED");

return true;
}

// 若初始槽位不行，则二次哈希探测
i = 1;
while (i < hash_size) {
int index = overall_hash(hash_country_code, i);

// 若探测发现该槽已存在同样 code，算重复
if (hash_country_array[index].status == "OCCUPIED" &&
hash_country_array[index].hash_country != nullptr &&
hash_country_array[index].hash_country->getCountryCode() == hash_country_code) 
{
delete new_country;
return false;
}

// 找到空槽或曾经占用过但现在释放了的槽
if (hash_country_array[index].status == "EMPTY" || 
hash_country_array[index].status == "PREVIOUSLY_OCCUPIED") 
{
hash_country_array[index].hash_country = new_country;
hash_country_array[index].status = "OCCUPIED";
hash_country_array[index].hashed_time = i;

// -------- 关键：同时把其链表挂到 country.country_list[index] ----------
country.country_list[index] = new_country->get_linked_list();
country.country_list[index].set_status("OCCUPIED");

return true;
}
i++;
}

// 若遍历整个哈希表都没能放进去，就失败
delete new_country;
return false;
}


/*
CITATION:
This function was reviewed by chat.openai.com with the prompt:
"How can I use double hashing to search for a country code efficiently?"
The AI tool helped ensure early termination on EMPTY slots and correct comparison of codes for fast lookup.
*/
void hash_table::hash_LOOKUP(std::string hash_country_code){
    int i = 0;
    int steps=0;
    while(i<hash_size){
        int index = overall_hash(hash_country_code, i);
        //std::cout<<index<<std::endl;
        steps++;
        if(hash_country_array[index].status == "EMPTY"){
            //std::cout<<"hi"<<std::endl;
            std::cout<<"failure\n";
            return;
        }
        if(hash_country_array[index].status=="OCCUPIED"&&hash_country_array[index].hash_country != nullptr &&
            hash_country_array[index].hash_country->getCountryCode() == hash_country_code){
               std::cout << "index " << index << " searches " << steps << "\n";
                return;
        }
        i++;    
    }
        std::cout<<"failure\n";
        return;
    }
    
 /*
CITATION:
This function was modified by chat.openai.com with the prompt:
"Implement removal from a hash table using PREVIOUSLY_OCCUPIED markers."
The AI tool validated memory cleanup and status updates during linear probing.
*/
bool hash_table::hash_REMOVE(std::string hash_country_code){
    int i = 0;
    while (i<hash_size){
        int index = overall_hash(hash_country_code, i);
        if(hash_country_array[index].status == "EMPTY"){
            std::cout<<"failure\n";
            return false;
        }
        if (hash_country_array[index].status == "OCCUPIED" &&
            hash_country_array[index].hash_country != nullptr &&
            hash_country_array[index].hash_country->getCountryCode() == hash_country_code){
            
            delete hash_country_array[index].hash_country;
            hash_country_array[index].hash_country = nullptr;
            hash_country_array[index].status = "PREVIOUSLY_OCCUPIED";
            hash_country_array[index].hashed_time = 0;
            std::cout << "success\n";
            return true;
            }
        i++;
    }
    std::cout<<"failure\n";
    return false;
}
/*
CITATION:
This function was refined by chat.openai.com with the prompt:
"I want to clear all hash table data and reset its state."
The AI tool ensured the table is reset by deleting dynamic memory and marking entries as EMPTY.
*/
void hash_table::hash_EXIT() {
    // Iterate through the hash table and clean up
    for (int i = 0; i < hash_size; i++) {
        if (hash_country_array[i].status == "OCCUPIED" && hash_country_array[i].hash_country != nullptr) {
            delete hash_country_array[i].hash_country;
            hash_country_array[i].hash_country = nullptr;
        }
        // Reset the slot to its initial state
        hash_country_array[i].status = "EMPTY";
        hash_country_array[i].hashed_time = 0;
    }
    // No output required per specification
}