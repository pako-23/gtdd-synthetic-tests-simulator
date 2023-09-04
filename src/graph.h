#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED

#include <cstdint>
#include <iostream>
#include <map>
#include <unordered_set>
#include <vector>

struct Graph {
    Graph(const std::vector<uint32_t>&);

    void add_edge(uint32_t, uint32_t);
    void remove_edge(uint32_t, uint32_t);
    std::unordered_set<uint32_t> get_dependencies(uint32_t) const;
    std::vector<std::vector<uint32_t>> get_schedules() const;
    void transitive_reduction(void);
    inline std::map<uint32_t, std::unordered_set<uint32_t>>::iterator begin()
    {
        return graph.begin();
    };
    inline std::map<uint32_t, std::unordered_set<uint32_t>>::iterator end()
    {
        return graph.end();
    };
    inline std::map<uint32_t, std::unordered_set<uint32_t>>::const_iterator
    begin() const
    {
        return graph.begin();
    };
    inline std::map<uint32_t, std::unordered_set<uint32_t>>::const_iterator end()
        const
    {
        return graph.end();
    };
    inline std::map<uint32_t, std::unordered_set<uint32_t>>::size_type size()
        const
    {
        return graph.size();
    };

protected:
    std::map<uint32_t, std::unordered_set<uint32_t>> graph;
};

struct GraphGenerator {
    GraphGenerator(const std::vector<uint32_t>&);
    virtual ~GraphGenerator() {};

    virtual void generate_edges(void) = 0;
    inline const Graph& get_graph(void) const { return graph; };

protected:
    Graph graph;
};

struct ErdosRenyiGenerator : public GraphGenerator {
    ErdosRenyiGenerator(const std::vector<uint32_t>&, double);

    void generate_edges(void);

private:
    double prob;
};

struct BarabasiAlbertGenerator : public GraphGenerator {
    BarabasiAlbertGenerator(const std::vector<uint32_t>&);

    void generate_edges(void);
};

struct OutDegreeGenerator : public GraphGenerator {
    OutDegreeGenerator(const std::vector<uint32_t>&, uint32_t, uint32_t);

    void generate_edges(void);

private:
    uint32_t min_degree;
    uint32_t max_degree;
};

std::ostream&
operator<<(std::ostream&, const Graph&);

#endif
