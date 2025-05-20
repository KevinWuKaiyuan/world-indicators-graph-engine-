#include "TimeSeries.h"
#include "linked_list.h"
#include "countries_static_arr.h"

// 拷贝构造函数：深拷贝
dynamic_array::dynamic_array(const dynamic_array& other) {
    size = other.size;
    capacity = other.capacity;

    // 重新开辟数组
    years = new int[capacity];
    datas = new double[capacity];

    for (int i = 0; i < size; i++) {
        years[i] = other.years[i];
        datas[i] = other.datas[i];
    }

    // 拷贝其余字段
    country_name = other.country_name;
    country_code = other.country_code;
    SEREIS_NAME = other.SEREIS_NAME;
    SERIES_CODE = other.SERIES_CODE;

    // next 指针不做跨-链表连接，而是置空 
    // （因为外部的 linked_list 会统一处理链表链接）
    next = nullptr;
}

// 赋值运算符：深拷贝
dynamic_array& dynamic_array::operator=(const dynamic_array& other) {
    if (this == &other) {
        return *this; // 自赋值直接返回
    }

    // 先释放自身旧数据
    delete[] years;
    delete[] datas;

    // 再按 other 的信息深拷贝
    size = other.size;
    capacity = other.capacity;

    years = new int[capacity];
    datas = new double[capacity];

    for (int i = 0; i < size; i++) {
        years[i] = other.years[i];
        datas[i] = other.datas[i];
    }

    country_name = other.country_name;
    country_code = other.country_code;
    SEREIS_NAME = other.SEREIS_NAME;
    SERIES_CODE = other.SERIES_CODE;
    next = nullptr;

    return *this;
}


// Constructor
dynamic_array::dynamic_array() {
 /*
    CITATION:
This function was developed with guidance from an example provided by chat.openai.com.
Prompt: "Teach me how to write a typical dynamic array. Give me an example."
The AI example helped me understand the structure of a dynamic array, and I wrote this 
constructor independently based on that understanding.
*/
    capacity = 2;
    size = 0;
    years = new int[capacity];
    datas = new double[capacity];
}

// Destructor
dynamic_array::~dynamic_array() {
    delete[] years;
    delete[] datas;
}

// Resize Array
void dynamic_array::resize_array(int new_capacity) {
/*
    CITATION:
    This function was written with the assistance of ChatGPT. 
    Prompt: "Write a C++ function to resize a dynamic array while preserving its contents."
    The AI provided the structure for resizing and copying data.
*/
    if (new_capacity < 2) {
        new_capacity = 2; // Minimum capacity is 2
    }

    int* new_years = new int[new_capacity];
    double* new_datas = new double[new_capacity];

    for (int i = 0; i < size; i++) {
        new_years[i] = years[i];
        new_datas[i] = datas[i];
    }

    delete[] years;
    delete[] datas;

    years = new_years;
    datas = new_datas;
    capacity = new_capacity;
}

// ADD Function

bool dynamic_array::ADD(int add_year, double add_data) {
    if(add_data==-1){
        //std::cout<<"failure\n";
        return false;
    }
    for (int i = 0; i < size; i++) {
        if (years[i] == add_year) {
            //std::cout << "failure\n";
            return false;
        }
    }
/*
    CITATION:
    This function was modified by chat.openai.com with the prompt:
    "My ADD function always report error when operating. Can you point out my
    problem?"
    The AI tool pointed out that I didn't resize my array, and it fixed a few
    syntax problems for me after generating the following block that doubled
    array size.
*/

    if (size == capacity) {
        resize_array(capacity * 2);
    }

    int position=size;
    for(int i=0; i<size; i++){
        if (add_year < years[i]){
            position = i;
            break;
        }
    }

    for(int i=size;i>position;i--){
        years[i]=years[i-1];
        datas[i]=datas[i-1];
    }  

    years[position] = add_year;
    datas[position] = add_data;
    size++;
    return true;
}

// LOAD Function
 
/*
    CITATION:
    This function was modified by chat.openai.com with the prompt:
    "Teach me how to access a dynamic array as I intend to load the csv file into 
    a new string, and then transfer the string into double"
    The AI tool generated an example of a load function, and it showed example
    of std::stod(element). Using the example code as guide, I wrote the other
    part of this function.
*/
void dynamic_array::LOAD(const std::string& line) {
    size = 0;
    std::stringstream theline(line);
    std::string element;

    // We read the first four columns
    std::getline(theline, element, ','); // country_name
    country_name = element;

    std::getline(theline, element, ','); // country_code
    country_code = element;

    std::getline(theline, element, ','); // SEREIS_NAME
    SEREIS_NAME = element;

    std::getline(theline, element, ','); // SERIES_CODE
    SERIES_CODE = element;

    int year = year_begin;
    // Then parse the numeric columns
    while (std::getline(theline, element, ',')) {
        double value = std::stod(element);
        if (value != -1) {
            ADD(year, value);
        }
        year++;
    }
}

// UPDATE Function
bool dynamic_array::UPDATE(int update_year, double update_data) {
    if(update_data==-1){
        //std::cout<<"failure\n";
        return false;
    }
    for (int i = 0; i < size; i++) {
        if (years[i] == update_year) {
            datas[i] = update_data;
            //std::cout << "success\n";
            return true;
        }
    }
    //std::cout << "failure\n";
    return false;
}

// Mean Function
double dynamic_array::mean() {
    if (size == 0) {
        //std::cout << "failure\n";
        return 0.0;
    }
    double sum = 0.0;
    for (int i = 0; i < size; i++) {
/*
    CITATION:
    This function was modified by chat.openai.com with the prompt:
    "why is sum += sum not working here?"
    The AI tool changed my line into sum += datas[i] and fixed this minor error
*/
        sum += datas[i];
    }
    double avg = sum / size;
    //std::cout << "mean is " << avg << "\n";
    return avg;
}

// Monotonic Check
bool dynamic_array::is_monotonic() {
    if (size == 0) {
        std::cout << "failure\n";
        return false;
    }
/*
    CITATION:
    This function was modified by chat.openai.com with the prompt:
    "I have problems with monotonic test. My code could only interpret if
    a function is monotonic as init has a positive slope. Give me 3 ideas of
    how people would normally judge if a series of number keeps increasing
    or decreasing"
    The AI tool suggested that boolean function is a good idea.
*/
    bool increasing = true, decreasing = true;
    for (int i = 1; i < size; i++) {
        if (datas[i] < datas[i - 1]) increasing = false;
        if (datas[i] > datas[i - 1]) decreasing = false;
    }
    if (increasing || decreasing) {
        std::cout << "series is monotonic\n";
        return true;
    } else {
        std::cout << "series is not monotonic\n";
        return false;
    }
}

// Best Fit Function
void dynamic_array::best_fit(double &m, double &b) {
    if (size == 0) {
        std::cout << "failure\n";
        return;
    }
    double sum_x = 0, sum_y = 0, sum_xy = 0, sum_x2 = 0;
/*
    CITATION:
    This function was modified by chat.openai.com with the prompt:
    "I have trouble changing these two math functions into cpp code. Can 
    you help me fix my wrong equations?"
    The AI tool corrected my mistake in the math functions that did the calculation wrong
*/

    for (int i = 0; i < size; i++) {
        sum_x += years[i];
        sum_y += datas[i];
        sum_xy += years[i] * datas[i];
        sum_x2 += years[i] * years[i];
    }
    double n = size;
    m = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x);
    b = (sum_y - m * sum_x) / n;
    std::cout << "slope is " << m << " intercept is " << b << "\n";
}

// PRINT Function
void dynamic_array::PRINT() {
/*
    CITATION:
    This function was modified by chat.openai.com with the prompt:
    "This PRINT function has errors I couldn't find. I think it may be about 
    the for loop I wrote. Can you please correct the mistake and point out 
    in which part I did wrong? How can I correct the mistake?"
    The AI tool changed the index of the original for loop 
*/

    if (size == 0) {
        //std::cout << "failure\n";
        return;
    }
    for (int i = 0; i < size; i++) {
        if(datas[i] != -1){
        std::cout << "(" << years[i] << "," << datas[i] << ") ";
        }
    }
    std::cout << "\n";
}
