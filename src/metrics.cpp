#include "metrics.hpp"
#include <algorithm>

namespace grapho {

// --- ConnectivityCounter ---
int ConnectivityCounter::parts(const grapho::Graph& g) {
    int components = 0;
    std::unordered_map<int, grapho::VertexState> states;
    for (int v : g.getAllVertices()) states[v] = grapho::VertexState::White;

    for (int v : g.getAllVertices()) {
        if (states[v] == grapho::VertexState::White) {
            components++;
            runFrom(g, v, -1, states);
        }
    }
    return components;
}

// --- ArticulationPointsFinder ---
void ArticulationPointsFinder::discoverVertex(int v) {
    tin[v] = low[v] = ++timer;
}

void ArticulationPointsFinder::treeEdge(int v, int u) {
    if (isRoot[v]) {
        rootChildren[v]++;
    }
}

void ArticulationPointsFinder::afterTreeEdge(int v, int u) {
    low[v] = std::min(low[v], low[u]);
    if (!isRoot[v] && low[u] >= tin[v]) {
        points.insert(v);
    }
}

void ArticulationPointsFinder::backEdge(int v, int u) {
    low[v] = std::min(low[v], tin[u]);
}

std::set<int> ArticulationPointsFinder::find(const grapho::Graph& g) {
    points.clear(); tin.clear(); low.clear(); rootChildren.clear(); isRoot.clear();
    timer = 0;
    std::unordered_map<int, grapho::VertexState> states;
    for (int v : g.getAllVertices()) states[v] = grapho::VertexState::White;

    for (int v : g.getAllVertices()) {
        if (states[v] == grapho::VertexState::White) {
            isRoot[v] = true;
            rootChildren[v] = 0;
            runFrom(g, v, -1, states);
            if (rootChildren[v] > 1) points.insert(v);
        }
    }
    return points;
}

// --- BipartiteChecker ---
void BipartiteChecker::treeEdge(int v, int u) {
    colors[u] = 1 - colors[v];
}

void BipartiteChecker::backEdge(int v, int u) {
    if (colors[v] == colors[u]) isBipartite = false;
}

void BipartiteChecker::forwardOrCrossEdge(int v, int u) {
    if (colors[v] == colors[u]) isBipartite = false;
}

bool BipartiteChecker::check(const grapho::Graph& g) {
    isBipartite = true;
    colors.clear();
    std::unordered_map<int, grapho::VertexState> states;
    for (int v : g.getAllVertices()) states[v] = grapho::VertexState::White;

    for (int v : g.getAllVertices()) {
        if (states[v] == grapho::VertexState::White) {
            colors[v] = 0;
            runFrom(g, v, -1, states);
        }
    }
    return isBipartite;
}

// --- Density ---
double Density::density(const grapho::Graph& graph) {
    long long n = graph.vertexCount();
    if (n <= 1) return 0.0;
    long long e = graph.edgeCount();
    double maxEdges = graph.isDirected() ? (double)n * (n - 1) : (double)n * (n - 1) / 2.0;
    return (maxEdges == 0) ? 0.0 : (double)e / maxEdges;
}

// --- BridgeFinder ---
void BridgeFinder::discoverVertex(int v) {
    tin[v] = low[v] = ++timer;
}

void BridgeFinder::afterTreeEdge(int v, int u) {
    low[v] = std::min(low[v], low[u]);
    if (low[u] > tin[v]) {
        bridges.emplace_back(v, u, 1.0);
    }
}

void BridgeFinder::backEdge(int v, int u) {
    low[v] = std::min(low[v], tin[u]);
}

std::vector<grapho::Edge> BridgeFinder::find(const grapho::Graph& g) {
    bridges.clear(); tin.clear(); low.clear(); timer = 0;
    std::unordered_map<int, grapho::VertexState> states;
    for (int v : g.getAllVertices()) states[v] = grapho::VertexState::White;

    for (int v : g.getAllVertices()) {
        if (states[v] == grapho::VertexState::White) {
            runFrom(g, v, -1, states);
        }
    }
    return bridges;
}

// --- greedIsGood ---
int greedIsGood::chromeNum(const grapho::Graph &g, std::unordered_map<int, int>& vertexColors) {
    vertexColors.clear();
    auto vertices = g.getAllVertices();
    if (vertices.empty()) return 0;
    int maxColor = 0;
    for (int v : vertices) {
        std::set<int> neighborColors;
        for (int neighbor : g.getNeighbors(v)) {
            if (vertexColors.count(neighbor)) neighborColors.insert(vertexColors[neighbor]);
        }
        int color = 0;
        while (neighborColors.count(color)) color++;
        vertexColors[v] = color;
        maxColor = std::max(maxColor, color);
    }
    return maxColor + 1;
}

} // namespace grapho
