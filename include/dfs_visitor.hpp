//
// Created by 1alex on 29.03.2026.
//


#pragma once

#include "graph.hpp"
#include <unordered_map>
#include <vector>

namespace grapho {


class DFSVisitor {
public:
    enum class VertexState { White, Gray, Black };

    virtual ~DFSVisitor() = default;


    void run(const Graph& g) {
        std::unordered_map<int, VertexState> states;
        for (int v : g.getAllVertices()) {
            states[v] = VertexState::White;
        }

        for (int v : g.getAllVertices()) {
            if (states[v] == VertexState::White) {
                dfsRecursive(g, v, states);
            }
        }
    }


    void runFrom(const Graph& g, int startVertex) {
        if (!g.hasVertex(startVertex)) return;

        std::unordered_map<int, VertexState> states;
        for (int v : g.getAllVertices()) {
            states[v] = VertexState::White;
        }

        dfsRecursive(g, startVertex, states);
    }

protected:
    // --- Виртуальные методы (Hooks) ---

    // Вызывается при первом посещении вершины (стала серой)
    virtual void discoverVertex(int v) {}

    // Вызывается перед тем, как перейти по ребру (v -> u)
    virtual void examineEdge(int v, int u) {}

    // Вызывается, если ребро ведет в белую вершину (новое дерево DFS)
    virtual void treeEdge(int v, int u) {}

    // Вызывается, если ребро ведет в серую вершину (найден цикл в орграфе)
    virtual void backEdge(int v, int u) {}

    // Вызывается, если ребро ведет в черную вершину (перекрестное ребро)
    virtual void forwardOrCrossEdge(int v, int u) {}

    // Вызывается, когда вершина и все её потомки полностью обработаны (стала черной)
    virtual void finishVertex(int v) {}

private:
    void dfsRecursive(const Graph& g, int v, std::unordered_map<int, VertexState>& states) {
        states[v] = VertexState::Gray;
        discoverVertex(v);

        for (const auto& edge : g.getIncidentEdges(v)) {
            int u = edge.to;
            examineEdge(v, u);

            if (states[u] == VertexState::White) {
                treeEdge(v, u);
                dfsRecursive(g, u, states);
            } else if (states[u] == VertexState::Gray) {
                backEdge(v, u);
            } else {
                forwardOrCrossEdge(v, u);
            }
        }

        states[v] = VertexState::Black;
        finishVertex(v);
    }
};

} // namespace grapho
