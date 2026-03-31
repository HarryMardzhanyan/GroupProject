#include "parsers.hpp"
#include <stdexcept>
#include <memory>
#include <string>

namespace grapho {
    
// EdgeListParser
Graph EdgeListParser::parseFromString(const std::string& content, bool directed) {
    Graph graph(directed);
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Пропускаем пустые строки и комментарии
        if (line.empty() || line[0] == '#') continue;
        
        std::istringstream lineStream(line);
        int from, to;
        double weight = 1.0;
        
        if (lineStream >> from >> to) {
            // Пробуем прочитать вес если есть
            lineStream >> weight;
            graph.addEdge(from, to, weight);
        }
    }
    
    return graph;
}

// AdjacencyMatrixParser
Graph AdjacencyMatrixParser::parseFromString(const std::string& content, bool directed) {
    Graph graph(directed);
    std::istringstream stream(content);
    
    int n;
    stream >> n;
    
    // Добавляем вершины 0..n-1
    for (int i = 0; i < n; i++) {
        graph.addVertex(i);
    }
    
    // Читаем матрицу
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double weight;
            stream >> weight;
            if (weight != 0) {
                graph.addEdge(i, j, weight);
            }
        }
    }
    
    return graph;
}

// DIMACS Coloring Instances Parser
Graph DIMACSParser::parseFromString(const std::string& content, bool directed) {
    Graph graph(directed);
    std::istringstream stream(content);
    std::string line;
    
    int n = 0, m = 0;
    
    while (std::getline(stream, line)) {
        // Пропускаем пустые строки и комментарии (начинаются с симола "c")
        if (line.empty() || line[0] == 'c') continue;
        
        if (line[0] == 'p') {
            // Строка проблемы (problem line): p edge n m
            std::istringstream lineStream(line);
            std::string p, type;
            lineStream >> p >> type >> n >> m;
            
            // Добавляем вершины
            for (int i = 1; i <= n; i++) {
                graph.addVertex(i);
            }
        } else if (line[0] == 'e') {
            // Ребро (edge line): e from to
            std::istringstream lineStream(line);
            std::string e;
            int from, to;
            lineStream >> e >> from >> to;
            graph.addEdge(from, to);
        }
    }

    return graph;
}

// SNAPParser
Graph SNAPParser::parseFromString(const std::string& content, bool directed) {
    Graph graph(directed);
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Пропускаем пустые строки и комментарии (начинаются с #)
        if (line.empty() || line[0] == '#') continue;
        
        std::istringstream lineStream(line);
        int from, to;
        
        if (lineStream >> from >> to) {
            graph.addEdge(from, to);
        }
    }
    
    return graph;
}


// ParserFactory
std::unique_ptr<GraphParser> ParserFactory::create(const std::string& formatName) {
    if (formatName == "EdgeList" || formatName == "edgelist") {
        return std::make_unique<EdgeListParser>();
    } else if (formatName == "AdjacencyMatrix" || formatName == "matrix") {
        return std::make_unique<AdjacencyMatrixParser>();
    } else if (formatName == "DIMACS" || formatName == "dimacs") {
        return std::make_unique<DIMACSParser>();
    } else if (formatName == "SNAP" || formatName == "snap") {
        return std::make_unique<SNAPParser>();
    }
    
    throw std::runtime_error("Unknown parser format: " + formatName);
}

std::vector<std::string> ParserFactory::getSupportedFormats() {
    return {"EdgeList", "AdjacencyMatrix", "DIMACS", "SNAP"};
}

} // namespace grapho
