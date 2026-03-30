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
                    runFrom(g, v, states); // Используем общий метод
                }
            }
        }

        // Обход конкретной компоненты
        // Теперь принимает states по ссылке, чтобы сохранять прогресс посещения
        void runFrom(const Graph& g, int startVertex, std::unordered_map<int, VertexState>& states) {
            if (!g.hasVertex(startVertex) || states[startVertex] != VertexState::White) return;

            states[startVertex] = VertexState::Gray;
            discoverVertex(startVertex);

            for (const auto& edge : g.getIncidentEdges(startVertex)) {
                int u = edge.to;
                examineEdge(startVertex, u);

                if (states[u] == VertexState::White) {
                    treeEdge(startVertex, u);
                    runFrom(g, u, states); // Рекурсия через runFrom
                    afterTreeEdge(startVertex, u); // Хук для мостов/точек сочленения
                } else if (states[u] == VertexState::Gray) {
                    backEdge(startVertex, u);
                } else {
                    forwardOrCrossEdge(startVertex, u);
                }
            }

            states[startVertex] = VertexState::Black;
            finishVertex(startVertex);
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
