#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include "graph.hpp"
#include "graph_backend.hpp"
#include "dfs_visitor.hpp"
#include "parsers.hpp"

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