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

        virtual ~DFSVisitor() = default;

        // Обход ВСЕГО графа (всех компонент)
        void run(const Graph& g) {
            std::unordered_map<int, VertexState> states;
            for (int v : g.getAllVertices()) states[v] = VertexState::White;

            for (int v : g.getAllVertices()) {
                if (states[v] == VertexState::White) {
                    runFrom(g, v, -1,states);
                }
            }
        }


        void runFrom(const Graph& g, int u, int p, std::unordered_map<int, VertexState>& states) {
            states[u] = VertexState::Gray;
            discoverVertex(u);

            for (const auto& edge : g.getIncidentEdges(u)) {
                int v = edge.to;


                if (v == p) continue;

                examineEdge(u, v);

                if (states[v] == VertexState::White) {
                    treeEdge(u, v);
                    runFrom(g, v, u, states);
                    afterTreeEdge(u, v);
                } else if (states[v] == VertexState::Gray) {
                    backEdge(u, v);
                } else {
                    forwardOrCrossEdge(u, v);
                }
            }

            states[u] = VertexState::Black;
            finishVertex(u);
        }

    protected:
        virtual void discoverVertex(int v) {}
        virtual void examineEdge(int v, int u) {}
        virtual void treeEdge(int v, int u) {}
        virtual void afterTreeEdge(int v, int u) {} // Важно для алгоритмов на "обратном пути"
        virtual void backEdge(int v, int u) {}
        virtual void forwardOrCrossEdge(int v, int u) {}
        virtual void finishVertex(int v) {}
    };


} // namespace grapho
