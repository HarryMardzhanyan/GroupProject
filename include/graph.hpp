#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include <optional>
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace grapho {

enum class VertexState {
    White,  // Не посещена (по умолчанию)
    Gray,   // В процессе обработки
    Black   // Обработана
};


// Структура, которая описывает ребро графа
struct Edge {
    int from;          // Исходящая вершина
    int to;            // Конечная вершина
    double weight;     // Вес ребра

    Edge(int f, int t, double w): from(f), to(t), weight(w) {}
    // Может быть ещё что-то добавим
};

// Структура для хранения цвета вершины
struct Color {
    int white, grey, black;
    Color(int w = 0, int g = 0, int b = 0) : white(w), grey(g), black(b) {}
    std::string toHex() const;   // Функция, чтобы превратить внутреннее цвет вершины в формат, понятный сайтам, где рисуют графы
};

// Forward Declaration
class GraphBackend;

// Тип бекенда
enum class BackendType {
    AdjacencyList,
    AdjacencyMatrix
};

// ОСНОВНОЙ КЛАСС ГРАФА
class Graph {
private:
    std::unique_ptr<GraphBackend> backend_;
    std::unordered_map<int, VertexState> vertexColors_;
    BackendType backendType_;

public:
    ~Graph();
    explicit Graph(std::unique_ptr<GraphBackend> backend);
    // Конструктор графа
    // Бекенд создан для хранения структуры (по умолчанию список смежности)
    explicit Graph(bool directed = false, BackendType type = BackendType::AdjacencyList);
    Graph& operator=(const Graph& other);
    Graph(Graph&& other) noexcept;
    Graph& operator=(Graph&& other) noexcept;

    // РАБОТА С ВЕРШИНАМИ
    // Добавить вершину в граф
    void addVertex(int vertex);

    // Удалить вершину из графа
    void removeVertex(int vertex);
    
    // Проверка на наличие вершины в графе
    bool hasVertex(int vertex) const;

    // Получить количество вершин
    int vertexCount() const;

    // Получить все вершины графа
    std::vector<int> getAllVertices() const;


    // РАБОТА С РЁБРАМИ
    // Добавить ребро в граф
    void addEdge(int from, int to, double weight = 1.0);

    // Удалить ребро из графа
    void removeEdge(int from, int to);

    // Проверить наличие ребра
    bool hasEdge(int from, int to) const;

    // Получить количество рёбер
    int edgeCount() const;

    // Получить все рёбра графа
    std::vector<Edge> getAllEdges() const;


    // РАБОТА С ОКРЕСТНОСТЯМИ
    // Получить соседей вершины
    std::vector<int> getNeighbors(int vertex) const;

    // Получить инцидентные рёбра вершины
    std::vector<Edge> getIncidentEdges(int vertex) const;

    // Получить степень вершины
    int getDegree(int vertex) const;

    // Получить полустепень захода. Это для ориентированных графов
    int getInDegree(int vertex) const;

    // Получить полустепень исхода. И это для ориентированных графов
    int getOutDegree(int vertex) const;

    
    // СВОЙСТВА ВЕРШИН
    // Проверить является ли вершина листом
    bool isLeaf(int vertex) const;

    // Проверить является ли вершина изолированной
    bool isIsolated(int vertex) const;

    
    // РАБОТА С ЦВЕТАМИ
    // Установить цвет вершины
    void setVertexColor(int vertex, VertexState color);

    // Получить цвет вершины
    VertexState getVertexColor(int vertex) const;


    // БЕКЕНД
    BackendType getBackendType() const { return backendType_; }
    bool isDirected() const;
    bool isEmpty() const { return vertexCount() == 0; }

    // Получить доступ к бекенду
    const GraphBackend& getBackend() const { return *backend_; }


    // ОПЕРАЦИИ С ГРАФАМИ
    // Объединить с другим графом
    Graph unionWith(const Graph& other) const;

    // Перенумеровать вершины
    void renumberVertices(const std::unordered_map<int, int>& mapping);


    // ОЧИСТИТЬ ГРАФ
    void clear();

};


} // namespace grapho