//
// Created by 1alex on 30.03.2026.
//
#include <set>

#include "graph.hpp"
#include "dfs_visitor.hpp"
class ConnectivityCounter : public grapho::DFSVisitor {
public:
    int count(const grapho::Graph& g) {
        int components = 0;
        std::unordered_map<int, grapho::VertexState> states;
        for (int v : g.getAllVertices()) states[v] = grapho::VertexState::White;

        for (int v : g.getAllVertices()) {
            if (states[v] == grapho::VertexState::White) {
                components++;
                runFrom(g, v, states);
            }
        }
        return components;
    }
};
class ArticulationPointsFinder : public grapho::DFSVisitor {
    std::unordered_map<int, int> tin, low;
    std::set<int> result;
    int timer = 0;

protected:
    void discoverVertex(int v) override {
        tin[v] = low[v] = ++timer;
    }

    void treeEdge(int v, int u) override {

    }

    void afterTreeEdge(int v, int u) override {

        low[v] = std::min(low[v], low[u]);

        if (low[u] >= tin[v] && tin[v] != 1) {
            result.insert(v);
        }
    }

    void backEdge(int v, int u) override {
        low[v] = std::min(low[v], tin[u]);
    }

public:
    std::set<int> find(const grapho::Graph& g) {
        result.clear();
        tin.clear(); low.clear();
        timer = 0;

        std::unordered_map<int, grapho::VertexState> states;
        for (int v : g.getAllVertices()) states[v] = grapho::VertexState::White;

        for (int v : g.getAllVertices()) {
            if (states[v] == grapho::VertexState::White) {
                int children = 0;
                runFrom(g, v, states);
            }
        }
        return result;
    }
};
class BipartiteChecker : public grapho::DFSVisitor {
    std::unordered_map<int, int> colors;
    bool isBipartite = true;

protected:
    void discoverVertex(int v) override {
        if (colors.find(v) == colors.end()) colors[v] = 0;
    }

    void examineEdge(int v, int u) override {
        if (colors.find(u) == colors.end()) {
            colors[u] = 1 - colors[v];
        } else if (colors[u] == colors[v]) {
            isBipartite = false;
        }
    }

public:
    bool check(const grapho::Graph& g) {
        isBipartite = true;
        colors.clear();
        run(g);
        return isBipartite;
    }
};


