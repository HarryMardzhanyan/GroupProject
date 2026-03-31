//
// Created by 1alex on 31.03.2026.
//

#ifndef GRAPHDRO4_METRICS_HPP
#define GRAPHDRO4_METRICS_HPP
#include <vector>
#include <set>
#include <unordered_map>
#include <algorithm>

#include "graph.hpp"
#include "dfs_visitor.hpp"

namespace grapho {
    class ConnectivityCounter : public grapho::DFSVisitor {
    public:
        int parts(const grapho::Graph& g);
    };


    class ArticulationPointsFinder : public grapho::DFSVisitor {
        std::unordered_map<int, int> tin, low;
        std::set<int> points;
        int timer = 0;
        std::unordered_map<int, int> rootChildren;
        std::unordered_map<int, bool> isRoot;

    protected:
        void discoverVertex(int v) override;
        void treeEdge(int v, int u) override;
        void afterTreeEdge(int v, int u) override;
        void backEdge(int v, int u) override;

    public:
        std::set<int> find(const grapho::Graph& g);
    };


    class BipartiteChecker : public grapho::DFSVisitor {
        std::unordered_map<int, int> colors;
        bool isBipartite = true;

    protected:
        void treeEdge(int v, int u) override;
        void backEdge(int v, int u) override;
        void forwardOrCrossEdge(int v, int u) override;

    public:
        bool check(const grapho::Graph& g);
    };


    class Density {
    public:
        static double density(const grapho::Graph& graph);
    };


    class BridgeFinder : public grapho::DFSVisitor {
        std::unordered_map<int, int> tin, low;
        std::vector<grapho::Edge> bridges;
        int timer = 0;

    protected:
        void discoverVertex(int v) override;
        void afterTreeEdge(int v, int u) override;
        void backEdge(int v, int u) override;

    public:
        std::vector<grapho::Edge> find(const grapho::Graph& g);
    };
    class Diameter {
    public:
        static int find(const grapho::Graph& g);
    };

    class Transitivity {
    public:
        static double calculate(const grapho::Graph& g);
    };
    class greedIsGood {
    public:
        static int chromeNum(const grapho::Graph &g, std::unordered_map<int, int>& vertexColors);
    };
}
#endif //GRAPHDRO4_METRICS_HPP