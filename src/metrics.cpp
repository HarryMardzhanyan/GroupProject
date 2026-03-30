//
// Created by 1alex on 30.03.2026.
//
#include <set>
#include "graph.hpp"
#include "dfs_visitor.hpp"
#include <vector>
#include <algorithm>
#include <unordered_map>
class ConnectivityCounter : public grapho::DFSVisitor {
public:
    int parts(const grapho::Graph& g) {
        int components = 0;
        std::unordered_map<int, grapho::VertexState> states;
        for (int v : g.getAllVertices()) states[v] = grapho::VertexState::White;

        for (int v : g.getAllVertices()) {
            if (states[v] == grapho::VertexState::White) {
                components++;
                runFrom(g, v, -1,states);
            }
        }
        return components;
    }
};
class ArticulationPointsFinder : public grapho::DFSVisitor {
    std::unordered_map<int, int> tin, low;
    std::set<int> result;
    std::set<int> points;
    int timer=0;
    std::unordered_map<int, int> rootChildren;
    std::unordered_map<int, bool> isRoot;

    protected:
        void discoverVertex(int v) override {
            tin[v] = low[v] = ++timer;
        }

        void treeEdge(int v, int u) override {
            if (isRoot[v]) {
                rootChildren[v]++;
            }
        }

    void afterTreeEdge(int v, int u) override {
            low[v] = std::min(low[v], low[u]);

            if (!isRoot[v] && low[u] >= tin[v]) {
                points.insert(v); // Вставляем в points
            }
        }

        void backEdge(int v, int u) override {
            low[v] = std::min(low[v], tin[u]);
        }

    public:
        std::set<int> find(const grapho::Graph& g) {
            points.clear();
            tin.clear();
            low.clear();
            rootChildren.clear();
            isRoot.clear();
            timer = 0;
            std::unordered_map<int, grapho::VertexState> states;
            for (int v : g.getAllVertices()) states[v] = grapho::VertexState::White;

            for (int v : g.getAllVertices()) {
                if (states[v] == grapho::VertexState::White) {
                    isRoot[v] = true;
                    rootChildren[v] = 0;
                    runFrom(g, v, -1,states);
                    if (rootChildren[v] > 1) {
                        points.insert(v);
                    }
                }
            }
            return points;
        }
};
class BipartiteChecker : public grapho::DFSVisitor {
    std::unordered_map<int, int> colors;
    bool isBipartite = true;

protected:
    void treeEdge(int v, int u) override {
        colors[u] = 1 - colors[v];
    }

    void backEdge(int v, int u) override {
        if (colors[v] == colors[u]) isBipartite = false;
    }


    void forwardOrCrossEdge(int v, int u) override {
        if (colors[v] == colors[u]) isBipartite = false;
    }

public:
    bool check(const grapho::Graph& g) {
        isBipartite = true;
        colors.clear();

        std::unordered_map<int, grapho::VertexState> states;
        for (int v : g.getAllVertices()) states[v] = grapho::VertexState::White;

        for (int v : g.getAllVertices()) {
            if (states[v] == grapho::VertexState::White) {
                colors[v] = 0; // Начинаем красить новую компоненту
                runFrom(g, v, -1, states);
            }
        }
        return isBipartite;
    }
};

class Density{
public:
    static double density(const grapho::Graph& graph) {
        long long n = graph.vertexCount();
        if (n <= 1) return 0.0;

        long long e = graph.edgeCount();
        double maxEdges;

        if (graph.isDirected()) {

            maxEdges = static_cast<double>(n * (n - 1));
        } else {

            maxEdges = static_cast<double>(n * (n - 1)) / 2.0;
        }

        if (maxEdges == 0) return 0.0;
        return static_cast<double>(e) / maxEdges;
    }
};


class BridgeFinder : public grapho::DFSVisitor {
    std::unordered_map<int, int> tin, low;
    std::vector<grapho::Edge> bridges;
    int timer = 0;

protected:
    void discoverVertex(int v) override {
        tin[v] = low[v] = ++timer;
    }


    void afterTreeEdge(int v, int u) override {
        low[v] = std::min(low[v], low[u]);
        if (low[u] > tin[v]) {
            bridges.emplace_back(v, u, 1.0);
        }
    }

    void backEdge(int v, int u) override {

        low[v] = std::min(low[v], tin[u]);
    }

public:
    std::vector<grapho::Edge> find(const grapho::Graph& g) {
        bridges.clear();
        tin.clear();
        low.clear();
        timer = 0;

        std::unordered_map<int, grapho::VertexState> states;
        for (int v : g.getAllVertices()) {
            states[v] = grapho::VertexState::White;
        }

        for (int v : g.getAllVertices()) {
            if (states[v] == grapho::VertexState::White) {
                runFrom(g, v, -1,states);
            }
        }
        return bridges;
    }
};
class greedIsGood {
public:
    static int chromeNum(const grapho::Graph &g,std::unordered_map<int, int>& vertexColors) {
        vertexColors.clear();
        auto vertices = g.getAllVertices();
        if (vertices.empty()) return 0;
        int chromeNum=0;
        for (int v : vertices) {
            std::set<int> neighborColors;
            for (int neighbor : g.getNeighbors(v)) {
                if (vertexColors.count(neighbor)) {
                    neighborColors.insert(vertexColors[neighbor]);
                }
            }

            int color = 0;
            while (neighborColors.count(color)) {
                color++;
            }

            vertexColors[v] = color;
            chromeNum = std::max(chromeNum, color);
        }
        return chromeNum + 1;
    }

};
