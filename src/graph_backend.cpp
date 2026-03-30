#include "graph_backend.hpp"
#include <algorithm>

namespace grapho {

// БЕКЕНД НА ОСНОВЕ СПИСКА СМЕЖНОСТИ
AdjacencyListBackend::AdjacencyListBackend(bool directed): directed_(directed) {}

void AdjacencyListBackend::addVertex(int vertex) {
    if (vertices_.find(vertex) == vertices_.end()) {
        vertices_.insert(vertex);
        adjacencyList_[vertex] = std::vector<Edge>();
    }
}    

void AdjacencyListBackend::removeVertex(int vertex) {
    if (vertices_.find(vertex) == vertices_.end()) return;
    
    // Удаляем все исходящие рёбра
    adjacencyList_.erase(vertex);
    
    // Удаляем все входящие рёбра
    for (auto& [v, edges] : adjacencyList_) {
        edges.erase(
            std::remove_if(edges.begin(), edges.end(),
                [vertex](const Edge& e) { return e.to == vertex; }),
            edges.end()
        );
    }
    
    vertices_.erase(vertex);
}

bool AdjacencyListBackend::hasVertex(int vertex) const {
    return vertices_.find(vertex) != vertices_.end();
}

int AdjacencyListBackend::vertexCount() const {
    return static_cast<int>(vertices_.size());
}

std::vector<int> AdjacencyListBackend::getAllVertices() const {
    return std::vector<int>(vertices_.begin(), vertices_.end());
}

void AdjacencyListBackend::addEdge(const Edge& edge) {
    // Автоматически добавляем вершины если их нет
    addVertex(edge.from);
    addVertex(edge.to);
    
    // Проверяем существует ли уже такое ребро
    if (!hasEdge(edge.from, edge.to)) {
        adjacencyList_[edge.from].push_back(edge);
        
        // Для неориентированного графа добавляем обратное ребро
        if (!directed_) {
            adjacencyList_[edge.to].push_back(Edge(edge.to, edge.from, edge.weight));
        }
    }
}

void AdjacencyListBackend::removeEdge(int from, int to) {
    if (!hasVertex(from)) return;
    
    auto& edges = adjacencyList_[from];
    edges.erase(
        std::remove_if(edges.begin(), edges.end(),
            [to](const Edge& e) { return e.to == to; }),
        edges.end()
    );
    
    // Для неориентированного графа удаляем обратное ребро
    if (!directed_ && hasVertex(to)) {
        auto& backEdges = adjacencyList_[to];
        backEdges.erase(
            std::remove_if(backEdges.begin(), backEdges.end(),
                [from](const Edge& e) { return e.to == from; }),
            backEdges.end()
        );
    }
}

bool AdjacencyListBackend::hasEdge(int from, int to) const {
    if (adjacencyList_.find(from) == adjacencyList_.end()) return false;
    
    const auto& edges = adjacencyList_.at(from);
    return std::any_of(edges.begin(), edges.end(),
        [to](const Edge& e) { return e.to == to; });
}

int AdjacencyListBackend::edgeCount() const {
    int count = 0;
    for (const auto& [vertex, edges] : adjacencyList_) {
        count += static_cast<int>(edges.size());
    }
    // Для неориентированного графа каждое ребро посчитано дважды
    return directed_ ? count : count / 2;
}

std::vector<Edge> AdjacencyListBackend::getAllEdges() const {
    std::vector<Edge> edges;
    std::unordered_set<std::string> added;
    
    for (const auto& [vertex, vertexEdges] : adjacencyList_) {
        for (const auto& edge : vertexEdges) {
            // Для неориентированного графа избегаем дубликатов
            if (!directed_) {
                std::string key = std::to_string(std::min(edge.from, edge.to)) + 
                                  "-" + 
                                  std::to_string(std::max(edge.from, edge.to));
                if (added.find(key) != added.end()) continue;
                added.insert(key);
            }
            edges.push_back(edge);
        }
    }
    return edges;
}

std::vector<int> AdjacencyListBackend::getNeighbors(int vertex) const {
    std::vector<int> neighbors;
    if (adjacencyList_.find(vertex) == adjacencyList_.end()) return neighbors;
    
    for (const auto& edge : adjacencyList_.at(vertex)) {
        neighbors.push_back(edge.to);
    }
    return neighbors;
}

std::vector<Edge> AdjacencyListBackend::getIncidentEdges(int vertex) const {
    if (adjacencyList_.find(vertex) == adjacencyList_.end()) return {};
    return adjacencyList_.at(vertex);
}

int AdjacencyListBackend::getDegree(int vertex) const {
    return static_cast<int>(getNeighbors(vertex).size());
}

int AdjacencyListBackend::getInDegree(int vertex) const {
    if (directed_) {
        int count = 0;
        for (const auto& [v, edges] : adjacencyList_) {
            for (const auto& edge : edges) {
                if (edge.to == vertex) count++;
            }
        }
        return count;
    }
    return getDegree(vertex);
}

int AdjacencyListBackend::getOutDegree(int vertex) const {
    return getDegree(vertex);
}

void AdjacencyListBackend::setDirected(bool directed) {
    directed_ = directed;
}

void AdjacencyListBackend::clear() {
    adjacencyList_.clear();
    vertices_.clear();
}

std::unique_ptr<GraphBackend> AdjacencyListBackend::clone() const {
    return std::make_unique<AdjacencyListBackend>(*this);
}


// ФАБРИКА ДЛЯ СОЗДАНИЯ БЕКЕНДОВ
std::unique_ptr<GraphBackend> BackendFactory::create(BackendType type, bool directed) {
    switch (type) {
        case BackendType::AdjacencyList:
            return std::make_unique<AdjacencyListBackend>(directed);
        default:
            throw std::runtime_error("Unknown backend type");
    }
}

std::unique_ptr<GraphBackend> BackendFactory::create(const std::string& name, bool directed) {
    if (name == "AdjacencyList" || name == "list") {
        return std::make_unique<AdjacencyListBackend>(directed);
    }
    throw std::runtime_error("Unknown backend: " + name);
}

} // namespace