#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "TimeSeries.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

class dynamic_array; // Forward declaration

class linked_list{

private://linked_list private
    
    dynamic_array* head; //pointer to the firts node
    std::string status; ////

public: //linked_list public

    //constructor using initiallizer list
    linked_list();
    //destructor
    ~linked_list();
    
    std::string get_status() const;
    void set_status(const std::string& new_status);
    
    void linked_list_LOAD(std::string country_name, const std::string& filename);
    void linked_list_LIST();
    bool linked_list_ADD(std::string series_code, int year, double data);
    bool linked_list_UPDATE(std::string series_code, int year, double data);
    void linked_list_PRINT(std::string series_code);
    bool linked_list_DELETE(std::string series_code);
    bool linked_list_BIGGEST();

    double calculate_mean(std::string series_code);
    dynamic_array* get_head() const { return head; }  // ✅ Getter function

    // 新增：拷贝构造函数
    linked_list(const linked_list& other);

    // 新增：赋值运算符
    linked_list& operator=(const linked_list& other);

};//end of linked list


#endif