#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <set>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
#endif

#include "graph.hpp"
#include "metrics.hpp"
#include "parsers.hpp"
#include "graph_backend.hpp"
#include "graph_generators.hpp"
#include "serial.hpp"
using namespace grapho;

void setupConsole() {
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#endif
//std::setlocale(LC_ALL, "Russian"); если не работает то попробуйте включить
}



void showGeneratorsMenu(std::unique_ptr<GraphBackend>& current) {
    std::cout << "\n--- ГЕНЕРАЦИЯ ГРАФА ---\n"
              << "1. K_n (Полный)       5. Cycle (Цикл)       9. Компоненты\n"
              << "2. K_n,m (Двудольный)  6. Star (Звезда)      10. Мосты\n"
              << "3. Tree (Прюфер)      7. Wheel (Колесо)     11. Точки сочленения\n"
              << "4. Path (Путь)        8. Halin (Халин)      12. 2-мосты\n"
              << "Выбор: ";

    int type, n, m, val;
    if (!(std::cin >> type)) return;
    if (type != 3) { std::cout << "Введите n: "; std::cin >> n; }

    switch (type) {
        case 1:  current = simpleGenerator(n, fill_K_n); break;
        case 2:  std::cout << "Введите m: "; std::cin >> m; current = bipartiteGenerator(n, m, fill_K_n_m); break;
        case 3:  std::cout << "Введите n: "; std::cin >> n; current = generate_T_n(n); break;
        case 4:  current = simpleGenerator(n, fillPath); break;
        case 5:  current = simpleGenerator(n, fillCycles); break;
        case 6:  current = simpleGenerator(n, fillStar); break;
        case 7:  current = simpleGenerator(n, fillWheel); break;
        case 8:  current = simpleGenerator(n, fillHalin); break;
        case 9:  std::cout << "Введите k: "; std::cin >> val; current = parametrizedGenerator(n, val, fillComponents); break;
        case 10: std::cout << "Введите m: "; std::cin >> val; current = parametrizedGenerator(n, val, fillBridges); break;
        case 11: std::cout << "Введите p: "; std::cin >> val; current = parametrizedGenerator(n, val, fillArticulationPoints); break;
        case 12: std::cout << "Введите b: "; std::cin >> val; current = parametrizedGenerator(n, val, fillTwoBridges); break;
        default: return;
    }
    std::cout << "Граф создан.\n";
}

void showParsingMenu(std::unique_ptr<GraphBackend>& current) {
    auto formats = ParserFactory::getSupportedFormats();
    std::cout << "\n--- ПАРСИНГ ---\n";
    for (size_t i = 0; i < formats.size(); ++i) std::cout << i + 1 << ". " << formats[i] << "\n";

    int fIdx;
    if (!(std::cin >> fIdx)) return;
    if (fIdx < 1 || fIdx > (int)formats.size()) return;

    std::cout << "Вставьте данные (END для завершения):\n";
    std::string data, line;
    std::getline(std::cin, line); // Очистка буфера после cin >> fIdx

    while (std::getline(std::cin, line) && line != "END") {
        data += line + "\n";
    }

    try {
        auto parser = ParserFactory::create(formats[fIdx - 1]);
        Graph g = parser->parseFromString(data, false);
        current = g.releaseBackend();
        if (current) {
            std::cout << "Успешно загружено! Вершин: " << current->vertexCount() << "\n";
        }
    } catch (const std::exception& e) {
        std::cout << "Ошибка: " << e.what() << "\n";
    }
}

void showMetricsMenu(const Graph& g) {
    std::cout << "\n--- МЕТРИКИ И АНАЛИЗ ---\n"
              << "1. Базовые (Мосты, Точки сочленения, Циклы)\n"
              << "2. Компоненты реберной двусвязности (EBC)\n"
              << "3. Блоки (Вершинная двусвязность - VBC)\n"
              << "0. Назад\n"
              << "Выбор: ";

    int m;
    std::cin >> m;
    if (m == 0) return;

    BiconnectivityAnalyzer analyzer;
    auto result = analyzer.analyze(g);

    if (m == 1) {
        std::cout << "• Мостов: " << BridgeFinder().find(g).size() << "\n";
        std::cout << "• Точек сочленения: " << ArticulationPointsFinder().find(g).size() << "\n";
    } else if (m == 2) {
        std::cout << "--- Реберно-двусвязные компоненты ---\n";
        for (size_t i = 0; i < result.ebcs.size(); ++i) {
            std::cout << "EBC #" << i + 1 << ": ";
            for (int v : result.ebcs[i]) std::cout << v << " ";
            std::cout << "\n";
        }
    } else if (m == 3) {
        std::cout << "--- Блоки (VBC) ---\n";
        for (size_t i = 0; i < result.vbcs.size(); ++i) {
            std::cout << "Block #" << i + 1 << ": ";
            for (int v : result.vbcs[i]) std::cout << v << " ";
            std::cout << "\n";
        }
    }
}

void showExportMenu(const Graph& g) {
    std::cout << "\n--- ЭКСПОРТ И ВИЗУАЛИЗАЦИЯ ---\n"
              << "1. Сохранить в .edges (Progr@m4You)\n"
              << "2. Генерация DOT (GraphViz) + Случайное дерево\n"
              << "0. Назад\n"
              << "Выбор: ";

    int choice;
    std::cin >> choice;

    if (choice == 1) {
        std::string data = GraphSerializer::toEdgesFormat(g);
        GraphSerializer::saveToFile("graph_output.edges", data);
        std::cout << "Сохранено в graph_output.edges\n";
    } else if (choice == 2) {
        auto tree = GraphUtils::getRandomSpanningTree(g);
        // Визуализируем с выделением ребер случайного дерева красным цветом
        std::string dot = GraphSerializer::toGraphViz(g, {}, tree);
        GraphSerializer::saveToFile("graph.dot", dot);
        std::cout << "Сохранено в graph.dot (ребра остовного дерева выделены)\n";
    }
}
int main() {
    setupConsole();
    std::unique_ptr<GraphBackend> currentBackend;
    int choice = -1;

    while (choice != 0) {
        std::cout << "\n--- GRAPHO CLI v1.1 ---\n1. Генерация\n2. Парсинг\n3. Анализ\n4. Экспорт\n0. Выход\n>> ";
        if (!(std::cin >> choice)) break;

        if (choice == 1) showGeneratorsMenu(currentBackend);
        else if (choice == 2) showParsingMenu(currentBackend);
        else if (choice == 3 || choice == 4) {
            if (!currentBackend) {
                std::cout << "Сначала сгенерируйте или загрузите граф.\n";
                continue;
            }
            Graph g(std::move(currentBackend));

            if (choice == 3) showMetricsMenu(g);
            else showExportMenu(g);

            currentBackend = g.releaseBackend(); // Возвращаем владение
        }
    }
    return 0;
}
