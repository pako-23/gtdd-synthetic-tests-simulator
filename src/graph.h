#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED

#include <cstdint>
#include <iostream>
#include <map>
#include <unordered_set>
#include <vector>

class Graph {
public:
    friend std::istream& operator>>(std::istream&, Graph&);

    Graph(const std::vector<uint32_t>&);
    Graph()
        : graph {} {};

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

class GraphGenerator {
public:
    GraphGenerator(Graph&);
    virtual ~GraphGenerator() {};

    virtual void generate_edges(void) = 0;

protected:
    Graph& graph;
};

class ErdosRenyiGenerator : public GraphGenerator {
public:
    ErdosRenyiGenerator(Graph&, double);

    void generate_edges(void);

private:
    double prob;
};

class BarabasiAlbertGenerator : public GraphGenerator {
public:
    BarabasiAlbertGenerator(Graph&);

    void generate_edges(void);
};

class OutDegreeGenerator : public GraphGenerator {
public:
    OutDegreeGenerator(Graph&, uint32_t, uint32_t);

    void generate_edges(void);

private:
    uint32_t min_degree;
    uint32_t max_degree;
};

std::ostream&
operator<<(std::ostream&, const Graph&);

std::istream&
operator>>(std::istream&, Graph&);

#endif
