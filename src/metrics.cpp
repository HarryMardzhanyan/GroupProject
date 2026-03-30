//
// Created by 1alex on 30.03.2026.
//
#include "graph.hpp"
#include "dfs_visitor.hpp"
class Metrics {
    grapho::Graph Graph;
    public:
    double graph_density();
    long graph_diam();
    double graph_transitivity();
    long graph_components();
    long graph_cpoints();
    long graph_bridges();
    bool is_graph_2comp();
    long greedy_chrome();
};
long Metrics::graph_diam() {

}
