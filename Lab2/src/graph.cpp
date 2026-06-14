#include "graph.hpp"
#include "graph_backend.hpp"

#include <algorithm>
#include <stdexcept>

namespace grapho {

Graph::~Graph() = default;

// Конструкторы и прочее
Graph::Graph(bool directed, BackendType type) : backendType_(type) {
    backend_ = BackendFactory::create(type, directed);
}

Graph& Graph::operator=(const Graph& other) {
    if (this != &other) {
        backend_ = other.backend_->clone();
        backendType_ = other.backendType_;
        vertexColors_ = other.vertexColors_;
    }
    return *this;
}
    VertexState grapho::Graph::getVertexColor(int vertex) const {
    auto it = vertexColors_.find(vertex);
    return (it != vertexColors_.end()) ? it->second : VertexState::White;
}
Graph::Graph(Graph&& other) noexcept: backend_(std::move(other.backend_)), vertexColors_(std::move(other.vertexColors_)), backendType_(other.backendType_) {}

Graph& Graph::operator=(Graph&& other) noexcept {
    if (this != &other) {
        backend_ = std::move(other.backend_);
        vertexColors_ = std::move(other.vertexColors_);
        backendType_ = other.backendType_;
    }
    return *this;
}


// ВЕРШИНЫ
void Graph::addVertex(int vertex) {
    backend_ -> addVertex(vertex);
    if (vertexColors_.find(vertex) == vertexColors_.end()) {
        vertexColors_[vertex] = VertexState::White;
    }
}

void Graph::removeVertex(int vertex) {
    backend_->removeVertex(vertex);
    vertexColors_.erase(vertex);
}

bool Graph::hasVertex(int vertex) const { return backend_->hasVertex(vertex); }

int Graph::vertexCount() const { return backend_->vertexCount(); }

std::vector<int> Graph::getAllVertices() const { return backend_->getAllVertices(); }


// РЁБРА
void Graph::addEdge(int from, int to, double weight) { backend_->addEdge(Edge(from, to, weight)); }

void Graph::removeEdge(int from, int to) { backend_->removeEdge(from, to); }

bool Graph::hasEdge(int from, int to) const { return backend_->hasEdge(from, to); }

int Graph::edgeCount() const { return backend_->edgeCount(); }

std::vector<Edge> Graph::getAllEdges() const { return backend_->getAllEdges(); }


// ОКРЕСТНОСТИ
std::vector<int> Graph::getNeighbors(int vertex) const { return backend_->getNeighbors(vertex); }

std::vector<Edge> Graph::getIncidentEdges(int vertex) const { return backend_->getIncidentEdges(vertex); }

int Graph::getDegree(int vertex) const { return backend_->getDegree(vertex); }

int Graph::getInDegree(int vertex) const { return backend_->getInDegree(vertex); }

int Graph::getOutDegree(int vertex) const { return backend_->getOutDegree(vertex); }


// СВОЙСТВА (Не факт, что понадобятся, но все же)
bool Graph::isLeaf(int vertex) const { return getDegree(vertex) == 1; }

bool Graph::isIsolated(int vertex) const { return getDegree(vertex) == 0; }


// ЦВЕТА
void grapho::Graph::setVertexColor(int vertex, VertexState color) { vertexColors_[vertex] = color; }




// БЕКЕНД
bool Graph::isDirected() const { return backend_->isDirected(); }

    std::unique_ptr<GraphBackend> Graph::releaseBackend() {
    return std::move(backend_);
}
// ОПЕРАЦИИ С ГРАФАМИ
Graph Graph::unionWith(const Graph& other) const {
    Graph result(isDirected() || other.isDirected(), backendType_);
    
    for (int v : getAllVertices()) {
        result.addVertex(v);
        result.setVertexColor(v, getVertexColor(v));
    }
    for (int v : other.getAllVertices()) {
        if (!result.hasVertex(v)) {
            result.addVertex(v);
            result.setVertexColor(v, other.getVertexColor(v));
        }
    }
    
    for (const auto& edge : getAllEdges()) result.addEdge(edge.from, edge.to, edge.weight);
    for (const auto& edge : other.getAllEdges()) {
        if (!result.hasEdge(edge.from, edge.to)) {
            result.addEdge(edge.from, edge.to, edge.weight);
        }
    }
    
    return result;
}

void Graph::renumberVertices(const std::unordered_map<int, int>& mapping) {
    Graph newGraph(isDirected(), backendType_);
    
    for (int v : getAllVertices()) {
        auto it = mapping.find(v);
        int newV = (it != mapping.end()) ? it->second : v;
        newGraph.addVertex(newV);
        newGraph.setVertexColor(newV, getVertexColor(v));
    }
    
    for (const auto& edge : getAllEdges()) {
        auto itFrom = mapping.find(edge.from);
        auto itTo = mapping.find(edge.to);
        int newFrom = (itFrom != mapping.end()) ? itFrom->second : edge.from;
        int newTo = (itTo != mapping.end()) ? itTo->second : edge.to;
        newGraph.addEdge(newFrom, newTo, edge.weight);
    }
    
    *this = std::move(newGraph);
}

// ОЧИСТКА
void Graph::clear() {
    backend_->clear();
    vertexColors_.clear();
}
    Graph::Graph(std::unique_ptr<GraphBackend> backend)
        : backend_(std::move(backend)), backendType_(BackendType::AdjacencyList) {
    for (int v : backend_->getAllVertices()) { // Цикл должен быть внутри скобок {}
        vertexColors_[v] = VertexState::White;
    }
}


} // namespace grapho