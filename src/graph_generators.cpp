//
// Created by 1alex on 31.03.2026.
//
#include <functional>
#include <numeric>
#include <random>

#include "graph.hpp"
#include "graph_backend.hpp"
#include "dfs_visitor.hpp"
namespace grapho {
    void fill_K_n(GraphBackend& backend, const std::vector<int>& vertices) {
    for (size_t i = 0; i < vertices.size(); ++i) {
        for (size_t j = i + 1; j < vertices.size(); ++j) {
            backend.addEdge(Edge(vertices[i], vertices[j], 1.0));
            }
        }
    }
    void fill_K_n_m(GraphBackend& backend, const std::vector<int>& part1, const std::vector<int>& part2) {
        for (int v1 : part1) {
            for (int v2 : part2) {
                backend.addEdge(Edge(v1, v2, 1.0));
            }
        }
    }
    void decode_Prufer(GraphBackend &backend,const std::vector<int> &prufer) {
        int n = static_cast<int>(prufer.size()) + 2;
        std::vector<int> degree(n, 1);
        for (int v : prufer) {
            degree[v]++;
        }
        for (int v_prufer : prufer) {
            for (int j = 0; j < n; ++j) {
                if (degree[j] == 1) {
                    backend.addEdge(Edge(j, v_prufer, 1.0));
                    degree[j]--;
                    degree[v_prufer]--;
                    break;
                }
            }
        }
        int u = -1, v = -1;
        for (int i = 0; i < n; ++i) {
            if (degree[i] == 1) {
                if (u == -1) u = i; else v = i;
            }
        }
        if (u != -1 && v != -1) {
            backend.addEdge(Edge(u, v, 1.0));
        }
    }
    void fillPath(GraphBackend& backend, const std::vector<int>& v) {
        for (size_t i = 0; i + 1 < v.size(); ++i) {
            backend.addEdge(Edge(v[i], v[i+1], 1.0));
        }
    }


    void fillCycles(GraphBackend& backend, const std::vector<int>& v) {
        if (v.size() < 3) return;
        fillPath(backend, v);
        backend.addEdge(Edge(v.back(), v.front(), 1.0));
    }

    void fillStar(GraphBackend& backend, const std::vector<int>& v) {
        if (v.size() < 2) return;
        int center = v[0];
        for (size_t i = 1; i < v.size(); ++i) {
            backend.addEdge(Edge(center, v[i], 1.0));
        }
    }

    void fillWheel(grapho::GraphBackend& backend, const std::vector<int>& v) {
        if (v.size() < 4) return;
        fillStar(backend, v);
        std::vector<int> rim(v.begin() + 1, v.end());
        fillCycles(backend, rim);
    }
    void fillComponents(GraphBackend& backend, const std::vector<int>& v, int k) {
        if (k <= 0 || v.size() < k) return;


        int mainSize = v.size() - (k - 1);
        std::vector<int> mainPart(v.begin(), v.begin() + mainSize);

        fillPath(backend, mainPart);
    }
    void fillBridges(GraphBackend& backend, const std::vector<int>& v, int m) {
        if (m < 0 || m >= v.size()) return;
        std::vector<int> bridgePart(v.begin(), v.begin() + m + 1);
        fillPath(backend, bridgePart);
        if (v.size() > m + 1) {
            std::vector<int> cyclePart(v.begin() + m, v.end());
            fillCycles(backend, cyclePart);
        }
    }
    void fillArticulationPoints(GraphBackend& backend, const std::vector<int>& v, int p) {
        if (p < 0 || p > (int)v.size() - 2) return;
        std::vector<int> pathPart(v.begin(), v.begin() + p + 2);
        fillPath(backend, pathPart);
        if (v.size() > p + 2) {
            std::vector<int> starPart = { v[1] }; // Центр звезды
            starPart.insert(starPart.end(), v.begin() + p + 2, v.end());
            fillStar(backend, starPart);
        }
    }
    void fillTwoBridges(GraphBackend& backend, const std::vector<int>& v, int b) {
        if (b < 1 || (int)v.size() < b + 2) return;
        std::vector<int> part1 = { v[0], v[1] };
        std::vector<int> part2(v.begin() + 2, v.begin() + 2 + b);
        fill_K_n_m(backend, part1, part2);
    }
    std::unique_ptr<GraphBackend> generate_T_n(int n,BackendType type = BackendType::AdjacencyList) {
        auto backend = BackendFactory::create(type, false);
        if (n <= 0) return backend;
        if (n == 1) {
            backend->addVertex(0);
            return backend;
        }
        std::vector<int> prufer(n - 2);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, n - 1);

        for (int i = 0; i < n - 2; ++i) {
            prufer[i] = dis(gen);
        }
        decode_Prufer(*backend,prufer);
        return backend;

    }
    std::unique_ptr<GraphBackend> simpleGenerator(int n, std::function<void(GraphBackend&, const std::vector<int>&)> fillFunc,BackendType type = BackendType::AdjacencyList)
    {
        auto backend = BackendFactory::create(type, false);
        if (n <= 0) return backend;
        std::vector<int> vertices(n);
        std::iota(vertices.begin(), vertices.end(), 0);
        fillFunc(*backend, vertices);
        return backend;
    }
    std::unique_ptr<GraphBackend> bipartiteGenerator(int n, int m, std::function<void(GraphBackend&, const std::vector<int>&, const std::vector<int>&)> fillFunc,BackendType type = BackendType::AdjacencyList)
    {
        auto backend = BackendFactory::create(type, false);
        if (n <= 0 || m <= 0) return backend;
        std::vector<int> part1(n);
        std::iota(part1.begin(), part1.end(), 0);
        std::vector<int> part2(m);
        std::iota(part2.begin(), part2.end(), n);
        fillFunc(*backend, part1, part2);
        return backend;
    }
    std::unique_ptr<GraphBackend> parametrizedGenerator(int n, int val,std::function<void(GraphBackend&, const std::vector<int>&, int)> fillFunc,BackendType type = BackendType::AdjacencyList)
    {
        auto backend = BackendFactory::create(type, false);
        if (n <= 0) return backend;
        std::vector<int> vertices(n);
        std::iota(vertices.begin(), vertices.end(), 0);
        fillFunc(*backend, vertices, val);
        return backend;
    }
    void fillHalin(GraphBackend& backend, const std::vector<int>& v) {
        int n = static_cast<int>(v.size());
        if (n < 4) return;

        int k = std::max(1, (n - 2) / 2);
        std::vector<int> internal(v.begin(), v.begin() + k);
        std::vector<int> leaves(v.begin() + k, v.end());

        fillPath(backend, internal);

        size_t leafIdx = 0;
        for (size_t i = 0; i < internal.size(); ++i) {
            int needed = (internal.size() == 1) ? (n - 1) :
                         (i == 0 || i == internal.size() - 1) ? 2 : 1;

            for (int j = 0; j < needed && leafIdx < leaves.size(); ++j) {
                backend.addEdge(Edge(internal[i], leaves[leafIdx++], 1.0));
            }
        }

        while (leafIdx < leaves.size()) {
            backend.addEdge(Edge(internal.back(), leaves[leafIdx++], 1.0));
        }

        fillCycles(backend, leaves);
    }

}