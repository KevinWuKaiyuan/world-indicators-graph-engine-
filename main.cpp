#include <iostream>
#include <sstream>
#include <cmath>
#include <string>
#include "TimeSeries.h"
#include "linked_list.h"
#include "countries_static_arr.h"
#include "hash_project.h"
// 这是我们新增的头文件(见上)
#include "graph.h"

/*
    说明:
    - 和你原先的 main.cpp 基本一致, 保留对 Project 3/4 命令的处理:
      LOAD, LIST, RANGE, BUILD, FIND, DELETE, LIMITS, LOOKUP, REMOVE, INSERT, EXIT
    - 额外插入 Project 5 的命令: INITIALIZE, UPDATE_EDGES, ADJACENT, PATH, RELATIONSHIPS
    - 其余文件( .cpp/.h )保持不变, 只要保证hash和tree的代码一致.
*/

// 全局或局部静态对象
// country_element: 存放512个country_list[] + BST root
// hash_table: 用于HASH插入、LOOKUP等
// Graph: 用于Project 5
int main() {
    country_element country;
    hash_table ht;
    Graph g; // 新增图

    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        std::string command;
        iss >> command;

        // -------------------
        //  1) 下面是 Project 3/4 的命令(保持你原来的逻辑):
        // -------------------
        if (command == "LOAD") {
            std::string filename;
            if (!(iss >> filename)) {
                std::cout << "failure\n";
                continue;
            }
            country.tree_load(filename, ht); 
            // 里面会输出 "success" or "failure"
        }
        else if (command == "LIST") {
            std::string country_name;
            if (!std::getline(iss >> std::ws, country_name)) {
                std::cout << "failure\n";
                continue;
            }
            country.tree_list(country_name);
        }
        else if (command == "RANGE") {
            std::string series_code;
            if (!(iss >> series_code)) {
                std::cout << "failure\n";
                continue;
            }
            country.tree_range(series_code);
        }
        else if (command == "BUILD") {
            std::string series_code;
            if (!(iss >> series_code)) {
                std::cout << "failure\n";
                continue;
            }
            country.tree_build(series_code);
        }
        else if (command == "FIND") {
            double mean;
            std::string operation;
            if (!(iss >> mean >> operation)) {
                std::cout << "failure\n";
                continue;
            }
            country.tree_find(mean, operation);
        }
        else if (command == "DELETE") {
            std::string country_name;
            if (!(iss >> country_name)) {
                std::cout << "failure\n";
                continue;
            }
            country.tree_delete(country_name);
        }
        else if (command == "LIMITS") {
            std::string condition;
            if (!(iss >> condition) || (condition != "lowest" && condition != "highest")) {
                std::cout << "failure\n";
                continue;
            }
            country.tree_limits(condition);
        }
        else if (command == "LOOKUP") {
            std::string hash_country_code;
            if (!(iss >> hash_country_code)) {
                std::cout << "failure\n";
                continue;
            }
            ht.hash_LOOKUP(hash_country_code);
        }
        else if (command == "REMOVE") {
            std::string hash_country_code;
            if (!(iss >> hash_country_code)) {
                std::cout << "failure\n";
                continue;
            }
            ht.hash_REMOVE(hash_country_code);
        }
        else if (command == "INSERT") {
            std::string hash_country_code;
            std::string hash_filename;
            if (!(iss >> hash_country_code >> hash_filename)) {
                std::cout << "failure\n";
                continue;
            }
            bool insert_result = ht.real_hash_Insert(hash_country_code, hash_filename, country);
            if(insert_result){
                std::cout << "success\n";
            } else {
                std::cout << "failure\n";
            }
        }

        // -------------------
        //  2) 下面是 Project 5 新增命令:
        // -------------------
        else if (command == "INITIALIZE") {
            // 清空并重新构建图节点(所有国家)
            // 先把图清空(节点+边),或者只清空边再重建节点
            g.clear();

            // 从 country.country_list[] 里寻找有效国家(即 head != nullptr)
            // 拿到 code+name, 在图里 add_node
            for(int i = 0; i < 512; i++){
                auto head_ptr = country.country_list[i].get_head();
                if (head_ptr != nullptr) {
                    std::string ccode = head_ptr->country_code;
                    std::string cname = head_ptr->country_name;
                    g.add_node(ccode, cname);
                }
            }
            std::cout << "success\n";
        }
        else if (command == "UPDATE_EDGES") {
            // 形如: UPDATE_EDGES <Series_Code> <threshold> <relation>
            std::string series_code, relation;
            double threshold;
            if (!(iss >> series_code >> threshold >> relation)) {
                std::cout << "failure\n";
                continue;
            }
            // 收集所有符合条件的国家 => (mean与threshold比较)
            std::vector<int> matching_ids;
            // 遍历512
            bool found_any_country = false;
            for(int i=0; i<512; i++){
                auto head_ptr = country.country_list[i].get_head();
                if(!head_ptr) continue; // 空
                double val = country.country_list[i].calculate_mean(series_code);
                if(val < 0) continue; // -1=无效

                bool ok = false;
                if (relation == "greater") {
                    if (val > threshold) ok = true;
                } else if (relation == "less") {
                    if (val < threshold) ok = true;
                } else if (relation == "equal") {
                    double diff = std::fabs(val - threshold);
                    if (diff < 1e-3) ok = true;
                } else {
                    // relation非法
                }

                if(ok) {
                    std::string ccode = head_ptr->country_code;
                    if(g.has_country(ccode)) {
                        int node_id = g.get_id(ccode);
                        if (node_id >= 0) {
                            matching_ids.push_back(node_id);
                            found_any_country = true;
                        }
                    }
                }
            }

            if(!found_any_country) {
                // 没有任何国家符合 => 不可能新增任何边 => failure
                std::cout << "failure\n";
                continue;
            }

            // 两两连接 => edges
            Relationship R{series_code, threshold, relation};
            bool added_new = false;
            for(int i=0; i<(int)matching_ids.size(); i++){
                for(int j=i+1; j<(int)matching_ids.size(); j++){
                    bool ret = g.add_relationship(matching_ids[i], matching_ids[j], R);
                    if(ret) {
                        added_new = true;
                    }
                }
            }
            if(added_new) {
                std::cout << "success\n";
            } else {
                // 所有关系都已存在 => failure
                std::cout << "failure\n";
            }
        }
        else if (command == "ADJACENT") {
            // 输出给定country_code的所有邻居(以名称)
            std::string code;
            if(!(iss >> code)) {
                std::cout << "failure\n";
                continue;
            }
            if(!g.has_country(code)) {
                std::cout << "failure\n";
                continue;
            }
            int u = g.get_id(code);
            auto neighbors = g.get_neighbors(u);
            if(neighbors.empty()) {
                std::cout << "none\n";
                continue;
            }
            // 输出
            bool printed_any = false;
            for(auto v : neighbors) {
                if(v == u) continue; // 自己
                if(printed_any) std::cout << " ";
                std::cout << g.get_name(v);
                printed_any = true;
            }
            if(!printed_any) {
                std::cout << "none\n";
            } else {
                std::cout << "\n";
            }
        }
        else if (command == "PATH") {
            // 判断俩国家是否连通 => true / false
            std::string c1, c2;
            if(!(iss >> c1 >> c2)) {
                std::cout << "failure\n";
                continue;
            }
            // PDF说: 可以假设这俩都在图里
            int id1 = g.get_id(c1);
            int id2 = g.get_id(c2);
            if(id1<0 || id2<0) {
                // 虽然PDF说可以假设他们都在图里, 这里还是判断一下
                std::cout << "false\n";
                continue;
            }
            bool isConnected = g.connected(id1, id2);
            if(isConnected) {
                std::cout << "true\n";
            } else {
                std::cout << "false\n";
            }
        }
        else if (command == "RELATIONSHIPS") {
            // 若两国间有直接边, 列出所有(Series_Code threshold relation),否则none
            std::string c1, c2;
            if(!(iss >> c1 >> c2)) {
                std::cout << "none\n";
                continue;
            }
            int id1 = g.get_id(c1);
            int id2 = g.get_id(c2);
            if(id1<0 || id2<0) {
                // 不在图中
                std::cout << "none\n";
                continue;
            }
            auto rels = g.get_relationships(id1, id2);
            if(rels.empty()) {
                std::cout << "none\n";
            } else {
                bool first = true;
                for(auto &r : rels) {
                    if(!first) std::cout << " ";
                    first = false;
                    // (Series_Code Threshold Relation)
                    std::cout << "(" << r.series_code << " " << r.threshold 
                              << " " << r.relation << ")";
                }
                std::cout << "\n";
            }
        }

        // -------------------
        //  3) 退出命令(老的)
        // -------------------
        else if (command == "EXIT") {
            // project 4: 清空哈希
            ht.hash_EXIT();
            // project 5: 不要求输出, 直接break
            break;
        }
        else {
            // 未知命令
            std::cout << "failure\n";
        }
    }

    return 0;
}
