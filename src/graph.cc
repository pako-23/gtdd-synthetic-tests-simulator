#include "graph.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iterator>
#include <random>
#include <set>

Graph::Graph(const std::vector<uint32_t>& nodes)
    : graph {}
{
    for (const uint32_t& node : nodes)
        graph[node] = std::unordered_set<uint32_t> {};
}

void Graph::remove_edge(uint32_t u, uint32_t v)
{
    graph[u].erase(v);
}

void Graph::add_edge(uint32_t u, uint32_t v)
{
    graph[u].insert(v);
}

std::vector<std::vector<uint32_t>> Graph::get_schedules() const
{
    std::vector<std::vector<uint32_t>> schedules;
    std::unordered_set<uint32_t> visited;

    std::vector<uint32_t> nodes;
    for (const auto& it : graph)
        nodes.push_back(it.first);
    std::sort(nodes.begin(), nodes.end());

    for (auto it = nodes.crbegin(); it != nodes.crend(); it++) {
        if (visited.find(*it) != visited.cend())
            continue;

        std::unordered_set<uint32_t> deps { get_dependencies(*it) };
        std::vector<uint32_t> schedule;
        schedule.push_back(*it);

        for (const uint32_t& test : deps) {
            visited.insert(test);
            schedule.push_back(test);
        }
        std::sort(schedule.begin(), schedule.end());

        schedules.push_back(schedule);
    }

    return schedules;
}

std::unordered_set<uint32_t>
Graph::get_dependencies(uint32_t u) const
{
    std::unordered_set<uint32_t> dep {};
    std::vector<uint32_t> s {};
    std::unordered_set<uint32_t> visited {};

    s.push_back(u);

    while (!s.empty()) {
        uint32_t v = s.back();
        s.pop_back();
        for (const uint32_t i : graph.find(v)->second)
            if (visited.find(i) == visited.cend()) {
                s.push_back(i);
                dep.insert(i);
            } else if (i == u)
                dep.insert(i);
        visited.insert(v);
    }
    return dep;
}

void Graph::transitive_reduction()
{
    for (auto& it : graph) {
        std::unordered_set<uint32_t> min_edges { it.second };

        for (uint32_t v : it.second) {
            std::unordered_set<uint32_t> deps = get_dependencies(v);
            for (uint32_t u : it.second)
                if (deps.find(u) != deps.end() && min_edges.find(u) != min_edges.end())
                    min_edges.erase(u);
        }

        it.second = min_edges;
    }
}

GraphGenerator::GraphGenerator(const std::vector<uint32_t>& nodes)
    : graph { nodes }
{
}

ErdosRenyiGenerator::ErdosRenyiGenerator(const std::vector<uint32_t>& nodes,
    double p)
    : GraphGenerator { nodes }
    , prob { p == std::numeric_limits<double>::infinity()
            ? log(nodes.size()) / nodes.size()
            : p }
{
}

void ErdosRenyiGenerator::generate_edges()
{
    std::uniform_real_distribution<double> real_dis(0.0, 1.0);
    std::mt19937 re(std::random_device {}());

    for (auto it1 = graph.begin(); it1 != graph.end(); ++it1)
        for (auto it2 = std::next(it1, 1); it2 != graph.end(); ++it2)
            if (real_dis(re) < prob)
                graph.add_edge(it2->first, it1->first);
    graph.transitive_reduction();
}

BarabasiAlbertGenerator::BarabasiAlbertGenerator(
    const std::vector<uint32_t>& nodes)
    : GraphGenerator { nodes }
{
}

void BarabasiAlbertGenerator::generate_edges()
{
    if (graph.size() < 2)
        return;

    std::uniform_real_distribution<double> real_dis(0.0, 1.0);
    std::mt19937 re(std::random_device {}());

    std::map<uint32_t, uint32_t> in_degrees;
    std::map<uint32_t, uint32_t> out_degrees;

    for (const auto& it : graph)
        in_degrees[it.first] = out_degrees[it.first] = 0;

    auto first = graph.begin();
    auto second = std::next(first);
    graph.add_edge(second->first, first->first);
    ++in_degrees[first->first];
    ++out_degrees[second->first];

    for (auto it1 = std::next(second); it1 != graph.end(); ++it1) {
        uint32_t s = 0;

        for (auto it2 = in_degrees.begin(); it2->first < it1->first; ++it2)
            s += it2->second + out_degrees.find(it2->first)->second;

        for (auto it2 = in_degrees.begin(); it2->first < it1->first; ++it2)
            if (real_dis(re) < ((double)(it2->second + out_degrees.find(it2->first)->second)) / s) {
                graph.add_edge(it1->first, it2->first);
                ++in_degrees[it2->first];
                ++out_degrees[it1->first];
            }
    }

    graph.transitive_reduction();
}

OutDegreeGenerator::OutDegreeGenerator(const std::vector<uint32_t>& nodes,
    uint32_t min_degree,
    uint32_t max_degree)
    : GraphGenerator { nodes }
    , min_degree { min_degree }
    , max_degree { max_degree }
{
}

void OutDegreeGenerator::generate_edges()
{
    std::vector<uint32_t> keys {};
    std::uniform_int_distribution<uint32_t> degree_dis(min_degree, max_degree);
    std::mt19937 re(std::random_device {}());

    for (const auto& it : graph)
        keys.push_back(it.first);

    auto end = std::next(keys.begin());
    for (auto it1 = std::next(graph.begin()); it1 != graph.end(); ++it1, ++end) {
        uint32_t degree { degree_dis(re) };
        std::shuffle(keys.begin(), end, re);
        auto it2 = keys.begin();
        for (uint32_t i = 0; it2 != end && i < degree; ++i, ++it2)
            graph.add_edge(it1->first, *it2);
    }

    graph.transitive_reduction();
}

std::ostream&
operator<<(std::ostream& os, const Graph& g)
{
    os << "digraph G {" << std::endl;

    for (const auto& it : g)
        os << "    " << it.first << ";" << std::endl;

    for (const auto& it : g)
        for (const auto& target : it.second)
            os << "    " << it.first << " -> " << target << ";" << std::endl;

    return os << "}" << std::endl;
}
