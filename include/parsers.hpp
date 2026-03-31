#pragma once

#include "graph.hpp"
#include <string>
#include <memory>
#include <stdexcept>

namespace grapho {
class GraphParser {
public:
    virtual ~GraphParser() = default;

    // Распарсить граф из строки
    virtual Graph parseFromString(const std::string& content, bool directed = false) = 0;

    // Получить название формата парсера
    virtual std::string getFormatName() const = 0;
};

// Парсер формата "Список рёбер"
class EdgeListParser : public GraphParser {
public:
    Graph parseFromString(const std::string& content, bool directed = false) override;
    std::string getFormatName() const override { return "EdgeList"; }
};

// Парсер формата "матрица смежности"
class AdjacencyMatrixParser : public GraphParser {
public:
    Graph parseFromString(const std::string& content, bool directed = false) override;
    std::string getFormatName() const override { return "AdjacencyMatrix"; }
};

// Парсер формата DIMACS Coloring Instances
class DIMACSParser : public GraphParser {
public:
    Graph parseFromString(const std::string& content, bool directed = false) override;
    std::string getFormatName() const override { return "DIMACS"; }
};

// Парсер формата SNAP Dataset
class SNAPParser : public GraphParser {
public:
    Graph parseFromString(const std::string& content, bool directed = false) override;
    std::string getFormatName() const override { return "SNAP"; }
};


class ParserFactory {
public:
    // Создать парсер по названию формата
    static std::unique_ptr<GraphParser> create(const std::string& formatName);

    // Получить список поддерживаемых форматов
    static std::vector<std::string> getSupportedFormats();
};

} // namespace grapho