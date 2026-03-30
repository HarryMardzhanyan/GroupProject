#pragma once

#include "graph.hpp"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include <optional>

namespace grapho {

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

    virtual bool isDirected() const = 0;
    virtual void setDirected(bool directed) = 0;

    virtual void clear() = 0;
    virtual std::unique_ptr<GraphBackend> clone() const = 0;
};


// БЕКЕНД НА ОСНОВЕ СПИСКА СМЕЖНОСТИ
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

    bool isDirected() const override {return directed_;}
    void setDirected(bool directed) override;

    void clear() override;
    std::unique_ptr<GraphBackend> clone() const override;
};


// ФАБРИКА ДЛЯ СОЗДАНИЯ БЕКЕНДОВ
class BackendFactory {
public:
    static std::unique_ptr<GraphBackend> create(BackendType type, bool directed = false);
    static std::unique_ptr<GraphBackend> create(const std::string& name, bool directed = false);
};



} // Конец namespace