//
// Created by 1alex on 31.03.2026.
//

#pragma once

#include "graph.hpp" // Путь к твоему файлу с классом Graph и DFSVisitor
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <random>
#include <stack>
#include <set>
#include <unordered_set>

namespace grapho {



class BiconnectivityAnalyzer : public DFSVisitor {
public:
    struct Result {
        std::vector<std::vector<int>> ebcs; // Реберно-двусвязные компоненты
        std::vector<std::vector<int>> vbcs; // Вершинно-двусвязные компоненты (блоки)
    };

    Result analyze(const Graph& g) {
        int max_v = 0;
        auto all_v = g.getAllVertices();
        if (all_v.empty()) return {};
        for (int v : all_v) max_v = std::max(max_v, v);

        tin.assign(max_v + 1, -1);
        low.assign(max_v + 1, -1);
        timer = 0;
        bridges.clear();
        while(!edgeStack.empty()) edgeStack.pop();
        result = Result();

        // Запуск DFS через твой базовый класс
        run(g);

        // После нахождения мостов, определяем компоненты реберной двусвязности
        findEBCs(g);
        return result;
    }

protected:
    int timer;
    std::vector<int> tin, low;
    std::set<std::pair<int, int>> bridges;
    std::stack<Edge> edgeStack;
    Result result;

    void discoverVertex(int v) override {
        tin[v] = low[v] = timer++;
    }

    void treeEdge(int u, int v) override {
        edgeStack.push(Edge(u, v, 1.0));
    }

    void afterTreeEdge(int u, int v) override {
        low[u] = std::min(low[u], low[v]);
        if (low[v] > tin[u]) {
            bridges.insert({std::min(u, v), std::max(u, v)});
        }
        // Нашли блок (вершинная двусвязность)
        if (low[v] >= tin[u]) {
            std::set<int> block_nodes;
            while (true) {
                Edge e = edgeStack.top();
                edgeStack.pop();
                block_nodes.insert(e.from);
                block_nodes.insert(e.to);
                if (e.from == u && e.to == v) break;
            }
            result.vbcs.push_back(std::vector<int>(block_nodes.begin(), block_nodes.end()));
        }
    }

    void backEdge(int u, int v) override {
        low[u] = std::min(low[u], tin[v]);
        // Если это не дубликат ребра дерева в неориентированном графе
        if (tin[v] < tin[u]) {
            edgeStack.push(Edge(u, v, 1.0));
        }
    }

private:
    void findEBCs(const Graph& g) {
        std::unordered_set<int> visited;
        for (int v : g.getAllVertices()) {
            if (visited.find(v) == visited.end()) {
                std::vector<int> component;
                std::vector<int> q = {v};
                visited.insert(v);
                int head = 0;
                while(head < q.size()){
                    int curr = q[head++];
                    component.push_back(curr);
                    for(int neighbor : g.getNeighbors(curr)){
                        if(visited.find(neighbor) == visited.end()){
                            // Если ребро не является мостом
                            if(bridges.find({std::min(curr, neighbor), std::max(curr, neighbor)}) == bridges.end()){
                                visited.insert(neighbor);
                                q.push_back(neighbor);
                            }
                        }
                    }
                }
                result.ebcs.push_back(component);
            }
        }
    }
};


class GraphUtils {
public:
    // Поиск случайного остовного дерева
    static std::vector<Edge> getRandomSpanningTree(const Graph& g) {
        std::vector<Edge> tree;
        std::unordered_set<int> visited;
        auto all_v = g.getAllVertices();
        if (all_v.empty()) return tree;

        std::mt19937 rng(std::random_device{}());
        std::vector<int> stack = {all_v[0]};
        visited.insert(all_v[0]);

        while (!stack.empty()) {
            int u = stack.back();
            auto neighbors = g.getNeighbors(u);
            std::shuffle(neighbors.begin(), neighbors.end(), rng);

            bool found = false;
            for (int v : neighbors) {
                if (visited.find(v) == visited.end()) {
                    visited.insert(v);
                    tree.push_back(Edge(u, v, 1.0));
                    stack.push_back(v);
                    found = true;
                    break;
                }
            }
            if (!found) stack.pop_back();
        }
        return tree;
    }
};


class GraphSerializer {
public:
    // Формат .edges для Progr@m4You
    static std::string toEdgesFormat(const Graph& g) {
        std::stringstream ss;
        std::set<std::pair<int, int>> seen;
        for (const auto& e : g.getAllEdges()) {
            int u = e.from, v = e.to;
            if (!g.isDirected()) {
                if (u > v) std::swap(u, v);
                if (seen.count({u, v})) continue;
                seen.insert({u, v});
            }
            ss << u << " " << v << " " << e.weight << "\n";
        }
        return ss.str();
    }

    // Формат GraphViz (DOT)
    static std::string toGraphViz(const Graph& g,
                                 const std::vector<std::vector<int>>& clusters = {},
                                 const std::vector<Edge>& highlightEdges = {}) {
        std::stringstream ss;
        ss << (g.isDirected() ? "digraph" : "graph") << " G {\n";
        ss << "  node [shape=circle, style=filled, fillcolor=white];\n";

        // Отрисовка кластеров (компонент)
        for (size_t i = 0; i < clusters.size(); ++i) {
            ss << "  subgraph cluster_" << i << " {\n";
            ss << "    label = \"Component " << (i + 1) << "\";\n";
            ss << "    color = blue;\n";
            for (int v : clusters[i]) ss << "    " << v << ";\n";
            ss << "  }\n";
        }

        // Отрисовка рёбер
        std::set<std::pair<int, int>> seen;
        for (const auto& e : g.getAllEdges()) {
            int u = e.from, v = e.to;
            if (!g.isDirected() && u > v) std::swap(u, v);
            if (seen.count({u, v})) continue;
            seen.insert({u, v});

            bool highlight = false;
            for (const auto& he : highlightEdges) {
                if ((he.from == e.from && he.to == e.to) ||
                    (!g.isDirected() && he.from == e.to && he.to == e.from)) {
                    highlight = true; break;
                }
            }

            ss << "  " << e.from << (g.isDirected() ? " -> " : " -- ") << e.to;
            if (highlight) ss << " [color=red, penwidth=2.5]";
            ss << ";\n";
        }
        ss << "}\n";
        return ss.str();
    }

    static void saveToFile(const std::string& path, const std::string& data) {
        std::ofstream out(path);
        out << data;
    }
};

} // namespace grapho
