#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cmath>

/*
 说明：
 - Relationship: 用来存储(Series_Code, threshold, relation)三元组
 - Edge: 无向边，内部用一个unordered_set来存多条关系
 - Node: 存储country code和country name等信息
 - Graph: 整个图的类，支持清空、增边、邻居查询、连通性检查、获取某条边的所有relationships
*/

// 存储单个“关系”
struct Relationship {
    std::string series_code;
    double threshold;
    std::string relation; // "greater", "less", or "equal"

    // 定义相等判断：必须三项都一致
    bool operator==(const Relationship &other) const {
        // 题意：Threshold可以直接做==比较即可
        // 这里示例中，用fabs(...)<1e-12来避免浮点误差
        // 也可直接==看课程要求
        return (series_code == other.series_code) &&
               (std::fabs(threshold - other.threshold) < 1e-12) &&
               (relation == other.relation);
    }
};

// 为了能将 Relationship 放到 unordered_set，需要自定义哈希
struct RelationshipHash {
    size_t operator()(const Relationship &r) const {
        // 1) 针对 series_code 和 relation 做字符串 hash
        std::hash<std::string> str_hash;
        size_t h1 = str_hash(r.series_code);
        size_t h2 = str_hash(r.relation);

        // 2) 针对 threshold 做一点简易hash
        //   这里把 double 按64位long long重新interpret
        //   或者也可以将其转成 string 再hash
        auto tmp = *reinterpret_cast<const long long*>(&r.threshold);
        size_t h3 = std::hash<long long>()(tmp);

        // 做一个简单的组合
        // 具体的组合常量可以自定义
        size_t combined = h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1<<6) + (h1>>2)) ^ h3;
        return combined;
    }
};

// 无向边
struct Edge {
    int nodeA, nodeB;  // 两端节点ID(在Graph::nodes里的索引)
    // 用unordered_set存多条关系，不重复
    std::unordered_set<Relationship, RelationshipHash> rel_set;

    Edge(int a, int b) : nodeA(a), nodeB(b) {}
    // ★ 新增默认构造函数
    Edge() : nodeA(-1), nodeB(-1) {
        // 这样就能满足 operator[] 需要的默认构造
    }
};

// 节点(对应一个国家)
struct Node {
    std::string code;  // 国家代码
    std::string name;  // 国家名字

    // 你也可以加 country_element* / linked_list*
    // 这里仅演示存基本信息
};

class Graph {
private:
    // code -> 节点ID
    std::unordered_map<std::string, int> code_to_id;
    // 节点数组
    std::vector<Node> nodes;
    /*
       adjacency[u] 是一个 map:  v -> Edge
       表示节点u与节点v之间的边(无向)，故 adjacency[u][v] == adjacency[v][u]
     */
    std::vector< std::unordered_map<int, Edge> > adjacency;

public:
    Graph() {}

    // 清空整个图(节点和边)
    void clear() {
        code_to_id.clear();
        nodes.clear();
        adjacency.clear();
    }

    // 仅清空所有边，但保留节点
    void clear_edges() {
        for (auto &mp : adjacency) {
            mp.clear();
        }
    }

    // 添加一个新节点(若要重复添加同code可自行决定是否允许/忽略)
    void add_node(const std::string &country_code, const std::string &country_name) {
        // 若已存在，则不再重复添加
        if (code_to_id.find(country_code) != code_to_id.end()) {
            return; // 或者可以抛错
        }
        int id = (int)nodes.size();
        Node n;
        n.code = country_code;
        n.name = country_name;
        nodes.push_back(n);
        code_to_id[country_code] = id;

        // adjacency也要多一行
        adjacency.emplace_back(); // 空map
    }

    bool has_country(const std::string &code) const {
        return (code_to_id.find(code) != code_to_id.end());
    }

    int get_id(const std::string &code) const {
        auto it = code_to_id.find(code);
        if (it == code_to_id.end()) {
            return -1;
        }
        return it->second;
    }

    // 获取节点名字
    std::string get_name(int id) const {
        if (id < 0 || id >= (int)nodes.size()) return "";
        return nodes[id].name;
    }

    // 添加一条 relationship 到 (u,v) 的边上
    // 如果边不存在，先创建；如果relationship已存在，不算新增
    // 返回：true=这次有新增关系；false=已经存在，没新增
    bool add_relationship(int u, int v, const Relationship &r) {
        if (u == v) {
            // 不考虑自环
            return false;
        }
        // 保证小ID放在前面(便于 adjacency[u][v] = adjacency[v][u] 同步)
        if (u > v) {
            int temp = u; 
            u = v; 
            v = temp;
        }

        // 查找 adjacency[u].find(v)
        auto it = adjacency[u].find(v);
        if (it == adjacency[u].end()) {
            // 还没有边 => 创建
            Edge e(u, v);
            e.rel_set.insert(r);
            adjacency[u][v] = e;
            // 无向图，对称
            adjacency[v][u] = adjacency[u][v];
            return true;
        } else {
            // 边已存在 => 往 rel_set 里插入
            Edge &edge_uv = it->second;
            size_t old_size = edge_uv.rel_set.size();
            edge_uv.rel_set.insert(r);
            size_t new_size = edge_uv.rel_set.size();
            // 如果size变大 => 插入了新关系
            if (new_size > old_size) {
                // 同步 adjacency[v][u]
                adjacency[v][u] = edge_uv;
                return true;
            } else {
                return false;
            }
        }
    }

    // 获取一个节点所有邻居(直接相连的节点)
    std::vector<int> get_neighbors(int u) const {
        std::vector<int> result;
        if (u < 0 || u >= (int)adjacency.size()) {
            return result;
        }
        // adjacency[u] 是一个 map<v, Edge>
        for (auto &kv : adjacency[u]) {
            result.push_back(kv.first);
        }
        return result;
    }

    // 判断两个节点是否连通(BFS或者DFS)
    bool connected(int start, int goal) const {
        if (start < 0 || start >= (int)nodes.size()) return false;
        if (goal < 0 || goal >= (int)nodes.size()) return false;
        if (start == goal) return true;

        std::vector<bool> visited(nodes.size(), false);
        std::vector<int> stack;
        stack.push_back(start);
        visited[start] = true;

        while (!stack.empty()) {
            int cur = stack.back();
            stack.pop_back();

            auto &mp = adjacency[cur];
            for (auto &pair : mp) {
                int nxt = pair.first;
                if (!visited[nxt]) {
                    if (nxt == goal) {
                        return true;
                    }
                    visited[nxt] = true;
                    stack.push_back(nxt);
                }
            }
        }
        return false;
    }

    // 获取 (u,v) 间所有关系(若无边则空)
    std::vector<Relationship> get_relationships(int u, int v) const {
        std::vector<Relationship> res;
        if (u == v) return res;
        if (u > v) {
            int tmp = u; u = v; v = tmp;
        }
        if (u < 0 || u >= (int)adjacency.size() || 
            v < 0 || v >= (int)adjacency.size()) {
            return res;
        }
        auto it = adjacency[u].find(v);
        if (it == adjacency[u].end()) {
            // 没有边
            return res;
        }
        // 把 set 转成 vector
        for (auto &rel : it->second.rel_set) {
            res.push_back(rel);
        }
        return res;
    }
};

#endif
