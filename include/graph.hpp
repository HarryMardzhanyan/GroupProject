#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include <optional>

namespace grapho {

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


// Абстрактный класс для хранения структуры графа (Основной Backend)
class GraphBackend {
public:
    virtual ~GraphBackend() = default;
    
    virtual void addVertex(int vertex) = 0;
    virtual void removeVertex(int vertex) = 0;
    virtual bool hasVertex(int vertex) const = 0;
    virtual int vertexCount() const = 0;
    virtual std::vector<int> getAllVertices() const = 0;
    
    virtual void addEdge(const Edge& edge) = 0;
    virtual void removeEdge(int from, int to) = 0;
    virtual bool hasEdge(int from, int to) const = 0;
    virtual int edgeCount() const = 0;
    virtual std::vector<Edge> getAllEdges() const = 0;
    
    virtual std::vector<int> getNeighbors(int vertex) const = 0;
    virtual std::vector<Edge> getIncidentEdges(int vertex) const = 0;
    
    virtual int getDegree(int vertex) const = 0;
    virtual int getInDegree(int vertex) const = 0;
    virtual int getOutDegree(int vertex) const = 0;
};


// Бекенд на основе матрицы смежности (мб лучше в отдельный парчер вынести)
class AdjacencyListBackend : public GraphBackend {
private:
    std::unordered_map<int, std::vector<Edge>> adjacencyList_;
    std::unordered_set<int> vertices_;
    bool directed_;
    
public:
    explicit AdjacencyListBackend(bool directed = false);
    
    void addVertex(int vertex) override;
    void removeVertex(int vertex) override;
    bool hasVertex(int vertex) const override;
    int vertexCount() const override;
    std::vector<int> getAllVertices() const override;
    
    void addEdge(const Edge& edge) override;
    void removeEdge(int from, int to) override;
    bool hasEdge(int from, int to) const override;
    int edgeCount() const override;
    std::vector<Edge> getAllEdges() const override;
    
    std::vector<int> getNeighbors(int vertex) const override;
    std::vector<Edge> getIncidentEdges(int vertex) const override;
    
    int getDegree(int vertex) const override;
    int getInDegree(int vertex) const override;
    int getOutDegree(int vertex) const override;
};



// ОСНОВНОЙ КЛАСС ГРАФА
class Graph {
private:
    std::unique_ptr<GraphBackend> backend_;
    std::unordered_map<int, Color> vertexColors_;
    std::unordered_map<std::string, Color> edgeColors_;
    bool directed_;

public:
    // Конструктор графа
    // Бекенд создан для хранения структуры (по умолчанию список смежности)
    explicit Graph(bool directed = false, std::unique_ptr<GraphBackend> backend = nullptr);

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
    void setVertexColor(int vertex, const Color& color);

    // Получить цвет вершины
    std::optional<Color> getVertexColor(int vertex) const;


    // ОПЕРАЦИИ С ГРАФАМИ
    // Объединить с другим графом
    Graph unionWith(const Graph& other) const;

    // Перенумеровать вершины
    void renumberVertices(const std::unordered_map<int, int>& mapping);


    // ОТДЕЛЬНЫЕ СВОЙСТВА ГРАФА
    // Является ли граф направленным
    bool isDirected() const { return directed_; }

    // Является ли граф пустым
    bool isEmpty() const { return vertexCount() == 0; }

    void clear();
};


} // Конец namespace