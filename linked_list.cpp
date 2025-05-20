#include "TimeSeries.h"
#include "linked_list.h"
#include "countries_static_arr.h"
#include "hash_project.h"
#include <string>

// constructor
/*
CITATION:
This destructor was modified by chat.openai.com with the prompt:
"My destructor is not properly deallocating memory in my linked list. Can you review it?"
The AI tool helped in ensuring that each node is correctly deleted to prevent memory leaks.
*/
linked_list::linked_list():head(nullptr), status("EMPTY"){}



// Destructor
linked_list::~linked_list() {
    dynamic_array* temp;
    while(head != nullptr){
        temp = head;
        head = head -> next;// move to the next node
        delete temp; //delete the current node
    }    
    head = nullptr;
}

std::string linked_list::get_status() const {
    return status;
}

void linked_list::set_status(const std::string& new_status) {
    status = new_status;
}

//function to LOAD
void linked_list::linked_list_LOAD(std::string country_name, const std::string& line) {
    std::stringstream theline(line);
    std::string current_country;
    std::getline(theline, current_country, ',');
    if (current_country != country_name) return;

    dynamic_array* new_series = new dynamic_array();
    new_series->LOAD(line);
    if (!head) {
        head = new_series;
    } else {
        dynamic_array* temp = head;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = new_series;
    }
}



//function to LIST
void linked_list::linked_list_LIST(){
/*
CITATION:
This function was modified by chat.openai.com with the prompt:
"My LIST function does not print series names correctly. How should I fix it?"
The AI tool suggested corrections for formatting and ensured all series names appear in order.
*/
    if (head == nullptr){
        std::cout<<"failure\n";
        return;
    }

    dynamic_array * temp = head;
    std::cout<<temp->country_name<<" "<<temp->country_code;

    while (temp != nullptr){
        std::cout<<" "<<temp->SEREIS_NAME;
        temp = temp ->next;
    }
    std::cout<<"\n";   
}


//function to ADD
bool linked_list::linked_list_ADD(std::string series_code, int add_year, double add_data){
 /*
CITATION:
This function was modified by chat.openai.com with the prompt:
"I am unfamiliar with adding elements in a linked list, is this correct?"
The AI tool helped found my syntax and grammar problems. It then fixed these problems.
*/   
    if (head == nullptr){
        std::cout<<"failure\n";
        return false;
    }
    dynamic_array * temp = head;
    while(temp != nullptr){
        if (temp -> SERIES_CODE == series_code ){
             bool added = temp->ADD(add_year, add_data);       
        if (added){
            std::cout<<"success\n";
        }else{
            std::cout<<"failure\n";
        }
        return added;
    }
    temp = temp -> next;
  }
  std::cout<<"failure\n";
  return false;
}//end ADD


//function to UPDATE
bool linked_list::linked_list_UPDATE(std::string series_code, int year, double data){
    if (head == nullptr){
        std::cout<<"failure\n";
        return false;
    }
/*
CITATION:
This function was modified by chat.openai.com with the prompt:
"How do I move on in a linked list after modifying one node?"
The AI wrote an example code to answer qustion.After I finished my code studying it,
gpt fixed my minor errors.
*/
    dynamic_array * temp = head;
    while(temp != nullptr){
        if (temp -> SERIES_CODE == series_code ){
             bool updated = temp->UPDATE(year, data);       
        if (updated){
            std::cout<<"success\n";
        }else{
            std::cout<<"failure\n";
        }
        return updated;
    }
    temp = temp -> next;
  }
  std::cout<<"failure\n";
  return false;
}//end update


//function to PRINT
void linked_list::linked_list_PRINT(std::string series_code){
    if (head == nullptr){
        std::cout<<"failure\n";
        return;
    }

    dynamic_array * temp = head;
    while(temp != nullptr){
        if (temp -> SERIES_CODE == series_code ){
/*
CITATION:
This function was modified by chat.openai.com with the prompt:
"What should I put in the bracket inside Print if I am trying to call it from other files?"
The AI tool told me to stick to the format in headfile and generated the correct code to
call a function.
*/
             temp->PRINT();  
             return;     
    }
    temp = temp -> next;
  }
  std::cout<<"failure\n";
}//end print

//function to delete
bool linked_list::linked_list_DELETE(std::string series_code){
    if (head == nullptr){
        std::cout<<"failure\n";
        return false;
    }
    
    dynamic_array * temp = head;
    dynamic_array * prev = nullptr;

    if(head->SERIES_CODE == series_code){
        head = head -> next;
        delete temp;
        std::cout <<"success\n";
        return true;
    }
/*
CITATION:
This function was modified by chat.openai.com with the prompt:
"I wrote  
while(temp != nullptr){
 if(temp -> SERIES_CODE != series_code ){
             prev = temp;
             temp = temp->next;    
    }
 }
 But it mulfunctions. What is my problem?"
The AI tool suggested changing my logic in the while function and said my old code
would keep checking the same node.
*/
    while(temp != nullptr && temp -> SERIES_CODE != series_code ){
             prev = temp;
             temp = temp->next;    
    }

    if(temp == nullptr){
        std::cout<<"failure\n";
        return false;
    }

    prev -> next = temp -> next;
    delete temp;
    std::cout<<"success\n";
    return true;
}


//function BIGGEST
bool linked_list::linked_list_BIGGEST(){
/*
CITATION:
This function was modified by chat.openai.com with the prompt:
"why is there error in my code?"
The AI tool fixed my syntax error and my grammar error.
*/
    if (head == nullptr){
        std::cout<<"failure\n";
        return false;
    }

    dynamic_array * temp = head;
    std::string output_mean;
    double max_mean = -1;

    while(temp != nullptr){
         double ave=temp -> mean();
         if(ave>max_mean){
            max_mean = ave;
            output_mean = temp -> SERIES_CODE;

         }       
        temp = temp -> next;
    }

    if(max_mean == -1){
        std::cout<<"failure\n";
        return false;
    }
    
    std::cout<<output_mean<<"\n";
    return true;
}

double linked_list::calculate_mean(std::string series_code) {
    dynamic_array* temp = head;
    while (temp != nullptr) {
        if (temp->SERIES_CODE == series_code) {
            double m = temp->mean();
            //std::cout << "[DEBUG] Mean for " << series_code << ": " << m << std::endl;
            return m;
        }
        temp = temp->next;
    }
    return -1; // Changed from 0
}

linked_list::linked_list(const linked_list& other)
  : head(nullptr),
    status(other.status)
{
    // 若对方是空链表，那直接空就行
    if (!other.head) {
        return;
    }

    // 深拷贝：先复制第一个节点
    head = new dynamic_array(*other.head);  // 这里调用 dynamic_array(const dynamic_array&)
    
    // 然后依次复制剩下节点
    dynamic_array* src = other.head->next;
    dynamic_array* dst = head;
    while (src) {
        dst->next = new dynamic_array(*src);
        dst = dst->next;
        src = src->next;
    }
    dst->next = nullptr;
}

linked_list& linked_list::operator=(const linked_list& other)
{
    if (this == &other) {
        return *this; // 避免自赋值
    }

    // 1) 先把自己原有链表清空
    while (head) {
        dynamic_array* temp = head;
        head = head->next;
        delete temp;
    }
    head = nullptr;

    // 2) 拷贝对方的状态
    status = other.status;
    if (!other.head) {
        return *this; // 对方空链表，那就完事
    }

    // 3) 深拷贝对方的所有节点
    head = new dynamic_array(*other.head);
    dynamic_array* src = other.head->next;
    dynamic_array* dst = head;
    while (src) {
        dst->next = new dynamic_array(*src);
        dst = dst->next;
        src = src->next;
    }
    dst->next = nullptr;

    return *this;
}
