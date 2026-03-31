#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include "graph.hpp"
#include "graph_backend.hpp"
#include "dfs_visitor.hpp"
#include "parsers.hpp"
#include "metrics.hpp"
#include "graph_generators.hpp"
#include <sstream>

using namespace grapho;

// ТЕСТЫ ДЛЯ КЛАССА Graph

TEST_CASE("Graph: добавление вершин", "[graph]") {
    Graph graph;
    
    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(3);
    
    REQUIRE(graph.vertexCount() == 3);
    REQUIRE(graph.hasVertex(1));
    REQUIRE(graph.hasVertex(2));
    REQUIRE(graph.hasVertex(3));
    REQUIRE(!graph.hasVertex(4));
}

TEST_CASE("Graph: удаление вершин", "[graph]") {
    Graph graph;
    
    graph.addVertex(1);
    graph.addVertex(2);
    graph.addVertex(3);
    
    graph.removeVertex(2);
    
    REQUIRE(graph.vertexCount() == 2);
    REQUIRE(graph.hasVertex(1));
    REQUIRE(!graph.hasVertex(2));
    REQUIRE(graph.hasVertex(3));
}

TEST_CASE("Graph: добавление рёбер", "[graph]") {
    Graph graph;
    
    graph.addEdge(1, 2);
    graph.addEdge(2, 3);
    graph.addEdge(1, 3, 2.5); // Ребро с весом
    
    REQUIRE(graph.edgeCount() == 3);
    REQUIRE(graph.hasEdge(1, 2));
    REQUIRE(graph.hasEdge(2, 3));
    REQUIRE(graph.hasEdge(1, 3));
}

TEST_CASE("Graph: неориентированный граф", "[graph]") {
    Graph graph(false);
    
    graph.addEdge(1, 2);
    
    // Для неориентированного графа ребро должно быть в обе стороны
    REQUIRE(graph.hasEdge(1, 2));
    REQUIRE(graph.hasEdge(2, 1));
    REQUIRE(graph.getDegree(1) == 1);
    REQUIRE(graph.getDegree(2) == 1);
}

TEST_CASE("Graph: ориентированный граф", "[graph]") {
    Graph graph(true);
    
    graph.addEdge(1, 2);
    
    // Для ориентированного графа ребро только в одну сторону
    REQUIRE(graph.hasEdge(1, 2));
    REQUIRE(!graph.hasEdge(2, 1));
    REQUIRE(graph.getOutDegree(1) == 1);
    REQUIRE(graph.getInDegree(2) == 1);
}

TEST_CASE("Graph: получение соседей", "[graph]") {
    Graph graph;
    
    graph.addEdge(1, 2);
    graph.addEdge(1, 3);
    graph.addEdge(1, 4);
    
    auto neighbors = graph.getNeighbors(1);
    
    REQUIRE(neighbors.size() == 3);
    REQUIRE(std::find(neighbors.begin(), neighbors.end(), 2) != neighbors.end());
    REQUIRE(std::find(neighbors.begin(), neighbors.end(), 3) != neighbors.end());
    REQUIRE(std::find(neighbors.begin(), neighbors.end(), 4) != neighbors.end());
}

TEST_CASE("Graph: проверка листа", "[graph]") {
    Graph graph;
    
    graph.addEdge(1, 2);
    graph.addEdge(2, 3);
    graph.addEdge(2, 4);
    
    REQUIRE(graph.isLeaf(1)); // Степень 1
    REQUIRE(graph.isLeaf(3)); // Степень 1
    REQUIRE(graph.isLeaf(4)); // Степень 1
    REQUIRE(!graph.isLeaf(2)); // Степень 3
}

TEST_CASE("Graph: объединение графов", "[graph]") {
    Graph graph1, graph2;
    
    graph1.addEdge(1, 2);
    graph1.addEdge(2, 3);
    
    graph2.addEdge(3, 4);
    graph2.addEdge(4, 5);
    
    Graph result = graph1.unionWith(graph2);
    
    REQUIRE(result.vertexCount() == 5);
    REQUIRE(result.edgeCount() == 4);
}

// ТЕСТЫ ПАРСЕРОВ

TEST_CASE("Парсер: список рёбер", "[parsers]") {
    EdgeListParser parser;
    
    std::string content = R"(
1 2
2 3
3 4
# Комментарий
4 5 2.5
)";
    
    Graph graph = parser.parseFromString(content);
    
    REQUIRE(graph.vertexCount() == 5);
    REQUIRE(graph.edgeCount() == 4);
    REQUIRE(graph.hasEdge(1, 2));
    REQUIRE(graph.hasEdge(4, 5));
}

#include "graph_generators.hpp"
#include "metrics.hpp"

TEST_CASE("Connectivity metrics", "[metrics]") {
    ConnectivityCounter counter;

    SECTION("Complete graph K_5 is connected") {

        Graph g(simpleGenerator(5, fill_K_n));
        REQUIRE(counter.parts(g) == 1);
    }

    SECTION("Isolated vertices") {
        Graph g(false);
        g.addVertex(1); g.addVertex(2);
        REQUIRE(counter.parts(g) == 2);
    }
}

TEST_CASE("Structural metrics", "[metrics]") {
    SECTION("Articulation points in Star graph") {
        ArticulationPointsFinder finder;
        Graph g(simpleGenerator(5, fillStar));
        auto points = finder.find(g);
        REQUIRE(points.size() == 1);
        REQUIRE(points.count(0) == 1); // Центр звезды
    }

    SECTION("Bridges in Path graph") {
        BridgeFinder finder;
        Graph g(simpleGenerator(4, fillPath));
        auto bridges = finder.find(g);
        REQUIRE(bridges.size() == 3);
    }
}

TEST_CASE("Bipartite metrics", "[metrics]") {
    BipartiteChecker checker;

    SECTION("K_3_3 is bipartite") {
        Graph g(bipartiteGenerator(3, 3, fill_K_n_m));
        REQUIRE(checker.check(g) == true);
    }

    SECTION("Triangle is not bipartite") {
        Graph g(simpleGenerator(3, fillCycles));
        REQUIRE(checker.check(g) == false);
    }
}

TEST_CASE("Coloring and Density", "[metrics]") {
    SECTION("Density of K_4") {
        Graph g(simpleGenerator(4, fill_K_n));
        // Плотность полного графа всегда 1.0
        REQUIRE_THAT(Density::density(g), Catch::Matchers::WithinRel(1.0, 0.0001));
    }

    SECTION("Greedy coloring of K_5") {
        std::unordered_map<int, int> colors;
        Graph g(simpleGenerator(5, fill_K_n));
        int chi = greedIsGood::chromeNum(g, colors);
        REQUIRE(chi == 5);
    }
}
TEST_CASE("Basic Generators: Structural Correctness", "[generators]") {

    SECTION("Complete Graph K_n") {
        int n = 5;
        auto backend = simpleGenerator(n, fill_K_n);
        REQUIRE(backend->vertexCount() == n);
        // В полном графе n*(n-1)/2 ребер. Для n=5 это 10.
        REQUIRE(backend->edgeCount() == 10);
    }

    SECTION("Complete Bipartite Graph K_n_m") {
        int n = 3, m = 2;
        auto backend = bipartiteGenerator(n, m, fill_K_n_m);
        REQUIRE(backend->vertexCount() == (n + m));
        // В полном двудольном n*m ребер. 3*2 = 6.
        REQUIRE(backend->edgeCount() == 6);
    }

    SECTION("Path Graph P_n") {
        int n = 4;
        auto backend = simpleGenerator(n, fillPath);
        REQUIRE(backend->vertexCount() == n);
        // В пути n-1 ребро.
        REQUIRE(backend->edgeCount() == 3);
    }

    SECTION("Cycle Graph C_n") {
        int n = 4;
        auto backend = simpleGenerator(n, fillCycles);
        REQUIRE(backend->vertexCount() == n);
        // В цикле n ребер.
        REQUIRE(backend->edgeCount() == 4);
    }
}

TEST_CASE("Complex Generators: Star and Wheel", "[generators]") {

    SECTION("Star Graph") {
        int n = 6;
        auto backend = simpleGenerator(n, fillStar);
        REQUIRE(backend->vertexCount() == n);
        REQUIRE(backend->edgeCount() == n - 1);


        REQUIRE(backend->getDegree(0) == n - 1);
    }

    SECTION("Wheel Graph") {
        int n = 5;
        auto backend = simpleGenerator(n, fillWheel);
        REQUIRE(backend->vertexCount() == n);

        REQUIRE(backend->edgeCount() == 8);
    }
}

TEST_CASE("Tree Generators: Prufer Code", "[generators]") {

    SECTION("Random Tree T_n") {
        int n = 10;
        auto backend = generate_T_n(n);
        REQUIRE(backend->vertexCount() == n);

        REQUIRE(backend->edgeCount() == n - 1);
    }

    SECTION("Small Tree n=2") {
        auto backend = generate_T_n(2);
        REQUIRE(backend->vertexCount() == 2);
        REQUIRE(backend->edgeCount() == 1);
    }
}

TEST_CASE("Parametrized Generators", "[generators]") {

    SECTION("Bridges Generator") {
        int n = 6, m = 2; // 2 моста
        auto backend = parametrizedGenerator(n, m, fillBridges);
        REQUIRE(backend->vertexCount() == n);

        REQUIRE(backend->edgeCount() >= n - 1);
    }

    SECTION("Two Bridges Generator") {
        int n = 6, b = 3;
        auto backend = parametrizedGenerator(n, b, fillTwoBridges);

        REQUIRE(backend->edgeCount() == 2 * b);
    }

    SECTION("Halin Graph") {
        int n = 7;
        auto backend = simpleGenerator(n, fillHalin);
        REQUIRE(backend->vertexCount() == n);

        REQUIRE(backend->edgeCount() >= n);
    }
}

TEST_CASE("Generators Edge Cases", "[generators]") {

    SECTION("Empty or tiny graphs") {
        REQUIRE(simpleGenerator(0, fillPath)->vertexCount() == 0);
        REQUIRE(simpleGenerator(1, fill_K_n)->edgeCount() == 0);


        auto wheel = simpleGenerator(3, fillWheel);
        REQUIRE(wheel->edgeCount() == 0);
    }
}
