#include "TimeSeries.h"
#include "linked_list.h"
#include "countries_static_arr.h"
#include "hash_project.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <vector> 
#include <cmath> 


// Helper function to delete the tree and free memory
/*
CITATION:
This function was modified by xAI's Grok with the prompt:
"Help me ensure my tree deletion is safe and complete."
The AI tool verified the recursive deletion logic, ensuring all nodes are properly freed to prevent memory leaks.
*/
void country_element::delete_tree(TreeNode* node) {
    if (node == nullptr) return;
    delete_tree(node->left);
    delete_tree(node->right);
    delete node;
}

// Helper function to recursively build the tree
/*
CITATION:
This function was modified by xAI's Grok with the prompt:
"Optimize my tree-building algorithm for balance."
The AI tool verified the midpoint splitting logic ensures a balanced BST and adjusted the tolerance check (1E-3) for efficient leaf node creation.
*/
TreeNode* country_element::build_tree(std::pair<int, double> countries[], int size, double min, double max) {
    if (size == 0) return nullptr;

    TreeNode* node = new TreeNode();
    node->min_range = min;
    node->max_range = max;

    if (size <= 1 || (size > 1 && std::abs(countries[0].second - countries[size - 1].second) <= 1E-3)) {
        node->country_count = size;
        for (int i = 0; i < size; ++i) {
            node->country_indices[i] = countries[i].first;
        }
        return node;
    }

    double midpoint = (min + max) / 2.0;
    std::pair<int, double> left_countries[512];
    std::pair<int, double> right_countries[512];
    int left_count = 0, right_count = 0;

    for (int i = 0; i < size; ++i) {
        if (countries[i].second < midpoint) {
            left_countries[left_count++] = countries[i];
        } else {
            right_countries[right_count++] = countries[i];
        }
    }

    node->left = build_tree(left_countries, left_count, min, midpoint);
    node->right = build_tree(right_countries, right_count, midpoint, max);
    return node;
}
// Function to build the tree for a given series code
/*
CITATION:
This function was modified by xAI's Grok with the prompt:
"Remove all the stuff for debugging and troubleshoot why BUILD fails."
The AI tool stripped debug outputs, added a 'success' output for successful builds, and diagnosed the failure of BUILD SH.STA.BRTC.ZS by analyzing calculate_mean and suggesting CSV data checks.
*/
void country_element::tree_build(std::string tre_series_code) {
    if (root) {
        delete_tree(root);
        root = nullptr;
    }
    currentSeriesCode = tre_series_code;
    std::pair<int, double> countries[array_size];
    double min_mean = 1e9, max_mean = -1e9;
    int valid_count = 0;

    for (int i = 0; i < array_size; i++) {
        double mean = country_list[i].calculate_mean(tre_series_code);
        //std::cout<<mean<<std::endl;
        if (mean != -1) {
            countries[valid_count++] = {i, mean};
            if (mean < min_mean) min_mean = mean;
            if (mean > max_mean) max_mean = mean;
        }
    }
    if (valid_count == 0) {
        std::cout << "failure\n";
        return;
    }

    for (int i = 0; i < valid_count - 1; i++) {
        for (int j = 0; j < valid_count - i - 1; j++) {
            if (countries[j].second > countries[j + 1].second) {
                std::pair<int, double> temp = countries[j];
                countries[j] = countries[j + 1];
                countries[j + 1] = temp;
            }
        }
    }

    root = build_tree(countries, valid_count, min_mean, max_mean);
    std::cout << "success\n";  // Added success output
}

/*
CITATION:
This function was modified by xAI's Grok with the prompt:
"Fix my LOAD function to handle duplicate country names correctly."
The AI tool reviewed the two-pass CSV loading, ensured unique country names are stored by tracking country_name_record, and maintained the success/failure output.
*/
//fuction  TREE_LOAD
void country_element::tree_load(const std::string& filename, hash_table &ht) 
{
    std::ifstream readin_file(filename);
    if (!readin_file.is_open()) {
        std::cout << "failure\n";
        return;
    }

    // Collect all CSV lines in memory so we read the file only once
    std::vector<std::string> all_lines;  // <-- CHANGED
    std::string csv_line;
    while (std::getline(readin_file, csv_line)) {
        all_lines.push_back(csv_line);
    }
    readin_file.close();  // Done reading the file

    // Arrays for storing unique country data
    std::string country_names[array_size];
    std::string country_codes[array_size];
    int country_indices[array_size];

    int numb_country = 0;
    std::string last_country_name; 
    bool data_loaded = false;

    // ============= First Pass =============
    // Identify unique countries by name, insert them into hash table.
    // We'll pass the (code, name) to hash_INSERT (no more file reading there).
    for (size_t line_idx = 0; line_idx < all_lines.size() && numb_country < array_size; ++line_idx) 
    {
        std::stringstream ss(all_lines[line_idx]);
        std::string country_name, country_code;
        std::getline(ss, country_name, ',');
        std::getline(ss, country_code, ',');

        // Skip duplicates if they have the same name as last line's name
        // (your original logic).
        if (country_name == last_country_name) {
            continue;
        }
        last_country_name = country_name;

        // Save them in the array
        country_names[numb_country] = country_name;
        country_codes[numb_country] = country_code;

        // Prepare to see if it already exists or do open-addressing
        int index = ht.overall_hash(country_code, 0);
        int i = 1;
        hash_element elem = ht.get_hash_data(index);
        while (elem.status == "OCCUPIED" &&
               elem.hash_country != nullptr &&
               elem.hash_country->getCountryCode() != country_code &&
               i < hash_size)
        {
            index = ht.overall_hash(country_code, i++);
            elem = ht.get_hash_data(index);
        }

        country_indices[numb_country] = index;

        // Instead of passing the filename (to re-read), we now pass country_code & country_name
        // so hash_INSERT does not open the file again. 
        bool inserted = ht.hash_INSERT(country_code, country_name);  // <-- CHANGED
        if (inserted) {
            // After insertion, let's grab the newly inserted element so
            // we can store its linked_list pointer if needed:
            hash_element inserted_elem = ht.get_hash_data(index);
            if (inserted_elem.status == "OCCUPIED" && inserted_elem.hash_country != nullptr) {
                country_list[index] = inserted_elem.hash_country->get_linked_list();
            }

            numb_country++;
            data_loaded = true;
        }
    }

    // ============= Second Pass =============
    // Now that the countries are in the hash table,
    // we load each line's data into the corresponding linked_list
    // (rather than re-reading the file).
    for (size_t line_idx = 0; line_idx < all_lines.size(); ++line_idx) 
    {
        std::stringstream ss(all_lines[line_idx]);
        std::string country_name, country_code;
        std::getline(ss, country_name, ',');
        std::getline(ss, country_code, ',');

        // Find which index this country is at in the hash table
        int index = -1;
        for (int i = 0; i < numb_country; i++) {
            if (country_name == country_names[i]) {
                index = country_indices[i];
                break;
            }
        }

        if (index != -1) {
            hash_element elem = ht.get_hash_data(index);
            if (elem.status == "OCCUPIED" && elem.hash_country != nullptr) {
                // Actually load the data into the linked list
                country_list[index].linked_list_LOAD(country_name, all_lines[line_idx]);
            }
        }
    }

    if (data_loaded) {
        std::cout << "success\n";
    } else {
        std::cout << "failure\n";
    }
}

//funtion TREE_LOAD ends
/*
CITATION:
This function was modified by xAI's Grok with the prompt:
"Ensure my LIST function calls the linked list method efficiently."
The AI tool optimized the search loop to exit after finding the country and verified the failure case for non-existent countries.
*/
void country_element::tree_list(std::string tre_country_name) {
    for (int i = 0; i < array_size; i++) { // Use array_size instead of 512
        if (country_list[i].get_head() && country_list[i].get_head()->country_name == tre_country_name) {
           // std::cout<<i<<std::endl;
            country_list[i].linked_list_LIST();
            return;
        }
    }
    std::cout << "failure\n";
}

/*
CITATION:
This function was modified by xAI's Grok with the prompt:
"Improve my RANGE function to handle edge cases better."
The AI tool added the 'No valid data' output for when no means are calculated and ensured min/max initialization handles all valid data scenarios.
*/
void country_element::tree_range(std::string tre_series_code) {
    double min_mean = 1e9, max_mean = -1e9;
    bool found_valid_data = false;

    for (int i = 0; i < array_size; i++) { // Use array_size
        double country_mean = country_list[i].calculate_mean(tre_series_code);
        if (country_mean > 0) {
            found_valid_data = true;
            if (country_mean < min_mean) min_mean = country_mean;
            if (country_mean > max_mean) max_mean = country_mean;
        }
    }

    if (found_valid_data) {
        std::cout << min_mean << " " << max_mean << "\n";
    }
    // } else {
    //     std::cout << "No valid data\n";
    // }
}

/*
CITATION:
This function was modified by xAI's Grok with the prompt:
"Clean up my FIND function and check its output format."
The AI tool removed debug statements, ensured space-separated output of country names, and validated the failure cases for empty trees or no matches.
*/
void country_element::tree_find(double mean, std::string operation) {
    //std::cout << "[DEBUG] Operation received: '" << operation << "'\n";
    if (!root) {
        std::cout << "failure\n";
        return;
    }
    std::vector<std::string> result;
    find_helper(root, mean, operation, result);
    if (root == nullptr){
        std::cout << "failure\n";                  
        return;
    }
    if (result.empty()) {
        std::cout << "\n";                  //??????????????? changed it 
        return;
    }
    for (size_t i = 0; i < result.size(); i++) {
        std::cout << result[i];
        if (i < result.size() - 1) std::cout << " ";
    }
    std::cout << "\n";
}



// Helper function (Recursive Search)
 void country_element::find_helper(TreeNode* node, double mean, std::string operation, std::vector<std::string>& result) {
    if (!node) return;
    for (int i = 0; i < node->country_count; i++) {
        int idx = node->country_indices[i];
        double countryMean = country_list[idx].calculate_mean(currentSeriesCode);
        //std::cout << "[DEBUG] Checking " << country_list[idx].get_head()->country_name 
                  //<< ": Mean = " << countryMean << " vs " << mean << " (" << operation << ")\n";
        if (operation == "greater" && countryMean > mean) {
            //std::cout << "[DEBUG] Match found: " << country_list[idx].get_head()->country_name << "\n";
            result.push_back(country_list[idx].get_head()->country_name);
        }
        if (operation == "equal") {
            double diff = fabs(countryMean - mean);
            //std::cout << "[DEBUG] Equal check: diff = " << diff << " (threshold = 1E-3)\n";
            if (diff < 1E-3) {
                //std::cout << "[DEBUG] Match found: " << country_list[idx].get_head()->country_name << "\n";
                result.push_back(country_list[idx].get_head()->country_name);
            }
        }
        // Add "less" if needed
        if (operation == "less" && countryMean < mean) {
            //std::cout << "[DEBUG] Match found: " << country_list[idx].get_head()->country_name << "\n";
            result.push_back(country_list[idx].get_head()->country_name);
        }
    }
    find_helper(node->left, mean, operation, result);
    find_helper(node->right, mean, operation, result);
}
/*
CITATION:
This function was modified by xAI's Grok with the prompt:
"Make my DELETE function more robust for tree updates."
The AI tool ensured the function correctly handles empty trees and uses the helper to propagate deletions, maintaining success/failure consistency.
*/
void country_element::tree_delete(std::string countryName) {
    if (!root) {
        std::cout << "failure\n";
        return;
    }

    bool deleted = delete_helper(root, countryName);
    std::cout << (deleted ? "success" : "failure") << "\n";
}


/*
CITATION:
This function was modified by xAI's Grok with the prompt:
"Enhance my DELETE helper to clean up empty nodes."
The AI tool added logic to delete nodes with zero countries and no children, ensuring the tree structure remains valid after deletions.
*/
// Helper function (Recursive Deletion)
bool country_element::delete_helper(TreeNode*& node, std::string countryName) {
    if (!node) return false;

    int newCount = 0;
    bool found = false;
    for (int i = 0; i < node->country_count; i++) {
        if (country_list[node->country_indices[i]].get_head()->country_name != countryName) {
            node->country_indices[newCount++] = node->country_indices[i];
        } else {
            found = true;
        }
    }
    node->country_count = newCount;

    if (node->country_count == 0 && !node->left && !node->right) {
        delete node;
        node = nullptr;
        return true;
    }
    bool leftDeleted = delete_helper(node->left, countryName);
    bool rightDeleted = delete_helper(node->right, countryName);
    return found || leftDeleted || rightDeleted;
}

/*
CITATION:
This function was modified by xAI's Grok with the prompt:
"Validate my LIMITS function for accurate extreme values."
The AI tool confirmed the highest/lowest mean logic, adjusted output formatting for multiple matches, and removed debugging clutter.
*/
void country_element::tree_limits(std::string condition) {                 
    if (!root) {
        std::cout << "failure\n";
        return;
    }
///////////
    std::vector<std::string> result;
    double targetMean = (condition == "highest") ? -1 : 1E9;  // Max for highest, min for lowest
  

    if (condition == "lowest") {
        TreeNode* leftmost = find_leftmost(root);
        if (!leftmost || leftmost->country_count == 0) {
            //std::cout << "count: " << leftmost->country_count << std::endl;
            std::cout << "failure\n";
            return;
        }
        for (int i = 0; i < leftmost->country_count; i++) {
            int idx = leftmost->country_indices[i];
            result.push_back(country_list[idx].get_head()->country_name);
        }
    }
    else if (condition == "highest") {
        collect_limits(root, condition, targetMean, result); // Pass condition string
    }
    else {
        std::cout << "failure\n";
        return;
    }
//////
    //collect_limits(root, condition == "highest", targetMean, result);

    if (result.empty()) {
        std::cout << "failure\n";
        return;
    }

    for (size_t i = 0; i < result.size(); i++) {
        std::cout << result[i];
        if (i < result.size() - 1) std::cout << " ";
    }
    std::cout << "\n";
}

/*
CITATION:
This function was modified by xAI's Grok with the prompt:
"Improve my LIMITS helper to handle ties in mean values."
The AI tool implemented the tolerance check (1E-3) for near-equal means, ensuring all tied countries are collected accurately.
*/
void country_element::collect_limits(TreeNode* node, std::string highest, double& targetMean, std::vector<std::string>& result) {
    if (!node) return;

    for (int i = 0; i < node->country_count; i++) {
        int idx = node->country_indices[i];
        double mean = country_list[idx].calculate_mean(currentSeriesCode);
        
        if (highest=="highest") {
            if (mean > targetMean) {
                targetMean = mean;
                result.clear();
                result.push_back(country_list[idx].get_head()->country_name);
            }
            else if (fabs(mean - targetMean) < 1E-3) {
                result.push_back(country_list[idx].get_head()->country_name);
            }
        }
        else {  // lowest
            if (mean < targetMean) {
                targetMean = mean;
                result.clear();
                result.push_back(country_list[idx].get_head()->country_name);
            }
            else if (fabs(mean - targetMean) < 1E-3) {
                result.push_back(country_list[idx].get_head()->country_name);
            }
        }
    }

    collect_limits(node->left, highest, targetMean, result);
    collect_limits(node->right, highest, targetMean, result);
}

/*
CITATION:
This function was modified by xAI's Grok with the prompt:
"Simplify my leftmost node finder for readability."
The AI tool streamlined the while loop, ensuring it safely navigates to the leftmost node with minimal complexity.
*/
/// Helper: Find leftmost leaf node
TreeNode* country_element::find_leftmost(TreeNode* node) {
    while (node && node->left) node = node->left; // Safer: checks node is non-null
    return node;
}

// Helper: Find rightmost leaf node
TreeNode* country_element::find_rightmost(TreeNode* node) {
    while (node && node->right) node = node->right; // Consistent null check
    return node;
}


/*
CITATION:
This function was modified by xAI's Grok with the prompt:
"Finalize my EXIT function to properly terminate the program."
The AI tool ensured the function fully deallocates the tree and resets the root, preparing the object for clean program exit.
*/
void country_element::tree_exit() {
    if (root) {
        delete_tree(root); // Recursively delete the tree
        root = nullptr;    // Reset root pointer to avoid dangling pointer
    }
}





