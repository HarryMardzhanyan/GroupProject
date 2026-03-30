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



// БЕКЕНД НА ОСНОВЕ МАТРИЦЫ СМЕЖНОСТИ
AdjacencyMatrixBackend::AdjacencyMatrixBackend(bool directed, int initialSize)
    : directed_(directed), edgeCount_(0) {
    matrix_.resize(initialSize, std::vector<double>(initialSize, 0.0));
}

AdjacencyMatrixBackend::AdjacencyMatrixBackend(const AdjacencyMatrixBackend& other): 
    matrix_(other.matrix_),
    vertexToIndex_(other.vertexToIndex_),
    indexToVertex_(other.indexToVertex_),
    directed_(other.directed_),
    edgeCount_(other.edgeCount_) {}

AdjacencyMatrixBackend& AdjacencyMatrixBackend::operator=(const AdjacencyMatrixBackend& other) {
    if (this != &other) {
        matrix_ = other.matrix_;
        vertexToIndex_ = other.vertexToIndex_;
        indexToVertex_ = other.indexToVertex_;
        directed_ = other.directed_;
        edgeCount_ = other.edgeCount_;
    }
    return *this;
}

int AdjacencyMatrixBackend::getIndex(int vertex) const {
    auto it = vertexToIndex_.find(vertex);
    if (it == vertexToIndex_.end()) return -1;
    return it->second;
}

int AdjacencyMatrixBackend::addIndex(int vertex) {
    auto it = vertexToIndex_.find(vertex);
    if (it != vertexToIndex_.end()) return it->second;
    
    int idx = static_cast<int>(vertexToIndex_.size());
    ensureCapacity(idx + 1);
    
    vertexToIndex_[vertex] = idx;
    indexToVertex_.push_back(vertex);
    return idx;
}

void AdjacencyMatrixBackend::ensureCapacity(int minSize) {
    if (minSize > static_cast<int>(matrix_.size())) {
        int newSize = std::max(minSize, static_cast<int>(matrix_.size() * 2));
        matrix_.resize(newSize, std::vector<double>(newSize, 0.0));
        for (auto& row : matrix_) {
            row.resize(newSize, 0.0);
        }
    }
}

void AdjacencyMatrixBackend::addVertex(int vertex) {
    if (hasVertex(vertex)) return;
    addIndex(vertex);
}

void AdjacencyMatrixBackend::removeVertex(int vertex) {
    int idx = getIndex(vertex);
    if (idx == -1) return;
    
    // Считаем удаляемые рёбра
    for (int j = 0; j < static_cast<int>(indexToVertex_.size()); j++) {
        if (matrix_[idx][j] != 0.0) edgeCount_--;
        if (matrix_[j][idx] != 0.0 && (!directed_ || j != idx)) edgeCount_--;
    }
    if (!directed_) edgeCount_ /= 2;
    edgeCount_ += (matrix_[idx][idx] != 0.0 ? 1 : 0); // Петля
    
    // Удаляем строку и столбец (помечаем нулями)
    for (int j = 0; j < static_cast<int>(matrix_.size()); j++) {
        matrix_[idx][j] = 0.0;
        matrix_[j][idx] = 0.0;
    }
    
    vertexToIndex_.erase(vertex);
    indexToVertex_.erase(indexToVertex_.begin() + idx);
    
    // Перестраиваем маппинг
    vertexToIndex_.clear();
    for (size_t i = 0; i < indexToVertex_.size(); i++) {
        vertexToIndex_[indexToVertex_[i]] = static_cast<int>(i);
    }
}

bool AdjacencyMatrixBackend::hasVertex(int vertex) const {
    return vertexToIndex_.find(vertex) != vertexToIndex_.end();
}

int AdjacencyMatrixBackend::vertexCount() const {
    return static_cast<int>(vertexToIndex_.size());
}

std::vector<int> AdjacencyMatrixBackend::getAllVertices() const {
    return indexToVertex_;
}

void AdjacencyMatrixBackend::addEdge(const Edge& edge) {
    int fromIdx = addIndex(edge.from);
    int toIdx = addIndex(edge.to);
    
    if (matrix_[fromIdx][toIdx] == 0.0) {
        matrix_[fromIdx][toIdx] = edge.weight;
        if (!directed_) {
            matrix_[toIdx][fromIdx] = edge.weight;
        }
        edgeCount_++;
    }
}

void AdjacencyMatrixBackend::removeEdge(int from, int to) {
    int fromIdx = getIndex(from);
    int toIdx = getIndex(to);
    
    if (fromIdx == -1 || toIdx == -1) return;
    
    if (matrix_[fromIdx][toIdx] != 0.0) {
        matrix_[fromIdx][toIdx] = 0.0;
        if (!directed_) {
            matrix_[toIdx][fromIdx] = 0.0;
        }
        edgeCount_--;
    }
}

bool AdjacencyMatrixBackend::hasEdge(int from, int to) const {
    int fromIdx = getIndex(from);
    int toIdx = getIndex(to);
    
    if (fromIdx == -1 || toIdx == -1) return false;
    return matrix_[fromIdx][toIdx] != 0.0;
}

int AdjacencyMatrixBackend::edgeCount() const {
    return edgeCount_;
}

std::vector<Edge> AdjacencyMatrixBackend::getAllEdges() const {
    std::vector<Edge> edges;
    int n = vertexCount();
    
    for (int i = 0; i < n; i++) {
        int startJ = directed_ ? 0 : i;
        for (int j = startJ; j < n; j++) {
            if (matrix_[i][j] != 0.0) {
                edges.push_back(Edge(indexToVertex_[i], indexToVertex_[j], matrix_[i][j]));
            }
        }
    }
    return edges;
}


std::vector<int> AdjacencyMatrixBackend::getNeighbors(int vertex) const {
    std::vector<int> neighbors;
    int idx = getIndex(vertex);
    if (idx == -1) return neighbors;
    
    int n = vertexCount();
    for (int j = 0; j < n; j++) {
        if (matrix_[idx][j] != 0.0) {
            neighbors.push_back(indexToVertex_[j]);
        }
    }
    return neighbors;
}

std::vector<Edge> AdjacencyMatrixBackend::getIncidentEdges(int vertex) const {
    std::vector<Edge> edges;
    int idx = getIndex(vertex);
    if (idx == -1) return edges;
    
    int n = vertexCount();
    for (int j = 0; j < n; j++) {
        if (matrix_[idx][j] != 0.0) {
            edges.push_back(Edge(indexToVertex_[idx], indexToVertex_[j], matrix_[idx][j]));
        }
    }
    return edges;
}

int AdjacencyMatrixBackend::getDegree(int vertex) const {
    return static_cast<int>(getNeighbors(vertex).size());
}

int AdjacencyMatrixBackend::getInDegree(int vertex) const {
    if (directed_) {
        int idx = getIndex(vertex);
        if (idx == -1) return 0;
        
        int count = 0;
        int n = vertexCount();
        for (int i = 0; i < n; i++) {
            if (matrix_[i][idx] != 0.0) count++;
        }
        return count;
    }
    return getDegree(vertex);
}

int AdjacencyMatrixBackend::getOutDegree(int vertex) const {
    return getDegree(vertex);
}

void AdjacencyMatrixBackend::setDirected(bool directed) {
    directed_ = directed;
}

void AdjacencyMatrixBackend::clear() {
    matrix_.clear();
    vertexToIndex_.clear();
    indexToVertex_.clear();
    edgeCount_ = 0;
}

std::unique_ptr<GraphBackend> AdjacencyMatrixBackend::clone() const {
    return std::make_unique<AdjacencyMatrixBackend>(*this);
}



// ФАБРИКА ДЛЯ СОЗДАНИЯ БЕКЕНДОВ
std::unique_ptr<GraphBackend> BackendFactory::create(BackendType type, bool directed) {
    switch (type) {
        case BackendType::AdjacencyList:
            return std::make_unique<AdjacencyListBackend>(directed);
        case BackendType::AdjacencyMatrix:
            return std::make_unique<AdjacencyMatrixBackend>(directed);
        default:
            throw std::runtime_error("Unknown backend type");
    }
}

std::unique_ptr<GraphBackend> BackendFactory::create(const std::string& name, bool directed) {
    if (name == "AdjacencyList" || name == "list") {
        return std::make_unique<AdjacencyListBackend>(directed);
    } else if (name == "AdjacencyMatrix" || name == "matrix") {
        return std::make_unique<AdjacencyMatrixBackend>(directed);
    }
    throw std::runtime_error("Unknown backend: " + name);
}

} // namespace