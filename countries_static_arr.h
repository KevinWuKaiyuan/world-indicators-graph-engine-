#ifndef COUNTRIES_STATIC_ARR_H
#define COUNTRIES_STATIC_ARR_H

#include "TimeSeries.h"
#include "linked_list.h"
#include "hash_project.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
/*
CITATION:
This header file was modified by xAI's Grok with the prompt:
"Review my country_element class definition for completeness and consistency."
The AI tool ensured proper inclusion of dependencies, validated the TreeNode struct initialization, added a destructor for memory cleanup, and confirmed the interface supports all required tree operations (LOAD, BUILD, LIST, RANGE, FIND, DELETE, LIMITS, EXIT).
*/
class linked_list;
class hash_table;
const int array_size = 512;

struct TreeNode {
    double min_range, max_range;
    int country_indices[512];
    int country_count;
    TreeNode* left;
    TreeNode* right;

    TreeNode() : min_range(0), max_range(0), country_count(0), left(nullptr), right(nullptr) {}
};

class country_element {
private:
    
    TreeNode* root;
    TreeNode* build_tree(std::pair<int, double> countries[], int size, double min, double max);
    void delete_tree(TreeNode* node);
    std::string country_name;
    std::string country_code;
    linked_list time_series_data;
    void find_helper(TreeNode* node, double mean, std::string operation, std::vector<std::string>& result);
    bool delete_helper(TreeNode*& node, std::string country_name);
    std::string currentSeriesCode;
    TreeNode* find_leftmost(TreeNode* node);
    TreeNode* find_rightmost(TreeNode* node);
    
    // New helper function for LIMITS
    void collect_limits(TreeNode* node, std::string highest, double& targetMean, std::vector<std::string>& result);

public:

linked_list country_list[array_size];  ////// EDITED

    country_element() : root(nullptr) {}
    ~country_element() { delete_tree(root); }

    void tree_load(const std::string& filename, hash_table &ht);
    void tree_list(std::string tre_country_name);
    void tree_range(std::string tre_series_code);
    void tree_build(std::string tre_series_code);
    void tree_find(double mean, std::string tre_operation);
    void tree_delete(std::string countryName);
    void tree_limits(std::string condition);
    void tree_exit();

    std::string getCountryName() const { return country_name; }
    std::string getCountryCode() const { return country_code; }
    void setCountryName(const std::string& name) { country_name = name; }
    void setCountryCode(const std::string& code) { country_code = code; }

    // int primary_hash(std::string hash_country_code);
    // int secondary_hash(std::string hash_country_code);
    // int overall_hash(std::string hash_country_code, int i);
    linked_list get_linked_list() const { return time_series_data; }
    // Add this method
    void load_time_series(const std::string& country_name, const std::string& line) {
        time_series_data.linked_list_LOAD(country_name, line);
    }



    
};

#endif