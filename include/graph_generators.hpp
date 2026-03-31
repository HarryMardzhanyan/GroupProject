//
// Created by 1alex on 31.03.2026.
//

#ifndef GRAPHDRO4_GRAPH_GENERATORS_HPP
#define GRAPHDRO4_GRAPH_GENERATORS_HPP
#include <vector>
#include <memory>
#include <functional>

#include "graph.hpp"
#include "graph_backend.hpp"

namespace grapho {

    void fill_K_n(GraphBackend& backend, const std::vector<int>& vertices);

    void fill_K_n_m(GraphBackend& backend, const std::vector<int>& part1, const std::vector<int>& part2);

    void decode_Prufer(GraphBackend& backend, const std::vector<int>& prufer);

    void fillPath(GraphBackend& backend, const std::vector<int>& v);

    void fillCycles(GraphBackend& backend, const std::vector<int>& v);

    void fillStar(GraphBackend& backend, const std::vector<int>& v);

    void fillWheel(GraphBackend& backend, const std::vector<int>& v);

    void fillComponents(GraphBackend& backend, const std::vector<int>& v, int k);

    void fillBridges(GraphBackend& backend, const std::vector<int>& v, int m);

    void fillArticulationPoints(GraphBackend& backend, const std::vector<int>& v, int p);

    void fillTwoBridges(GraphBackend& backend, const std::vector<int>& v, int b);

    void fillHalin(GraphBackend& backend, const std::vector<int>& v);



    std::unique_ptr<GraphBackend> generate_T_n(int n, BackendType type = BackendType::AdjacencyList);

    std::unique_ptr<GraphBackend> simpleGenerator(
        int n,
        std::function<void(GraphBackend&, const std::vector<int>&)> fillFunc,
        BackendType type = BackendType::AdjacencyList
    );

    std::unique_ptr<GraphBackend> bipartiteGenerator(
        int n, int m,
        std::function<void(GraphBackend&, const std::vector<int>&, const std::vector<int>&)> fillFunc,
        BackendType type = BackendType::AdjacencyList
    );

    std::unique_ptr<GraphBackend> parametrizedGenerator(
        int n, int val,
        std::function<void(GraphBackend&, const std::vector<int>&, int)> fillFunc,
        BackendType type = BackendType::AdjacencyList
    );

} // namespace grapho
#endif //GRAPHDRO4_GRAPH_GENERATORS_HPP