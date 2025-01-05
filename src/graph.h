#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED

#include <cstdint>
#include <iostream>
#include <map>
#include <unordered_set>
#include <vector>

class Graph {
public:
  friend std::istream &operator>>(std::istream &, Graph &);

  Graph(const std::vector<uint32_t> &nodes);
  Graph(void) : graph{} {};

  void add_edge(uint32_t u, uint32_t v);
  void invert_edge(uint32_t u, uint32_t v);
  void remove_edge(uint32_t u, uint32_t v);
  std::unordered_set<uint32_t> get_dependencies(uint32_t u) const;
  std::vector<std::vector<uint32_t>> get_schedules(void) const;
  void transitive_reduction(void);

  inline std::map<uint32_t, std::unordered_set<uint32_t>>::iterator
  begin(void) {
    return graph.begin();
  };

  inline std::map<uint32_t, std::unordered_set<uint32_t>>::iterator end(void) {
    return graph.end();
  };

  inline std::map<uint32_t, std::unordered_set<uint32_t>>::const_iterator
  begin(void) const {
    return graph.begin();
  };

  inline std::map<uint32_t, std::unordered_set<uint32_t>>::const_iterator
  end(void) const {
    return graph.end();
  };

  inline std::map<uint32_t, std::unordered_set<uint32_t>>::size_type
  size(void) const {
    return graph.size();
  };

protected:
  std::map<uint32_t, std::unordered_set<uint32_t>> graph;
};

class GraphMetrics {
public:
  explicit GraphMetrics(void);

  uint64_t longest_schedule;
  uint64_t total_cost;
};

GraphMetrics compute_graph_metrics(const Graph &graph);

class GraphGenerator {
public:
  GraphGenerator(Graph &graph);
  virtual ~GraphGenerator(void) {};

  virtual void generate_edges(void) = 0;

protected:
  Graph &graph;
};

class ErdosRenyiGenerator : public GraphGenerator {
public:
  ErdosRenyiGenerator(Graph &graph, double p);

  void generate_edges(void);

private:
  double prob;
};

class BarabasiAlbertGenerator : public GraphGenerator {
public:
  BarabasiAlbertGenerator(Graph &graph);

  void generate_edges(void);
};

class OutDegreeGenerator : public GraphGenerator {
public:
  OutDegreeGenerator(Graph &graph, uint32_t min_degree, uint32_t max_degree);

  void generate_edges(void);

private:
  uint32_t min_degree;
  uint32_t max_degree;
};

std::ostream &operator<<(std::ostream &os, const Graph &g);
std::istream &operator>>(std::istream &is, Graph &g);

#endif
