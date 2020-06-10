#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <iostream>
#include <vector>
#include <map>

using namespace std;

typedef struct tag_pixel {
  int x;
  int y;
  float r;
  float g;
  float b;
} Pixel;

typedef struct tag_vertex {
  std::vector<Pixel> pixels;

} Vertex;

typedef struct tag_edge {
  Vertex* a;
  Vertex* b;
  float weight;

  tag_edge() {
    weight = 0;
  }
} Edge;

class Graph {
  private:
    std::vector<Vertex*> v;
    std::vector<Edge*> e;
    std::map<Vertex*, unsigned int> vertices;

    float threshold;

  public:
    Graph();
    Graph(float th);
    ~Graph();

    void addVertex(Vertex *v, unsigned int idx);
    void addEdge(Edge *edge);

    void setThreshold(float th) { this->threshold = th; }
    float getThreshold() { return this->threshold; }

    void segment();

    friend ostream& operator <<(ostream &o, const Graph &g) {
      o << "(Vertex, Edge) = (" << g.v.size() << ", " << g.e.size() << ") threshold: "<< g.threshold <<"\n";
      return o;
    }
};

#endif