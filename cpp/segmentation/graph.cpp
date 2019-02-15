#include "graph.h"

#include <iostream>

Graph::Graph() {
}

Graph::Graph(float th): threshold(th) {
}

Graph::~Graph() {
  size_t s = this->v.size();
  for(unsigned int i = 0; i < s; i++) {
    if(this->v[i] != NULL) {
      delete v[i];
    }
  }

  s = this->e.size();
  for(unsigned int i = 0; i < s; i++) {
    if(this->e[i] != NULL) {
      delete e[i];
    }
  }
}


void Graph::addVertex(Vertex *v, unsigned int idx) {
  this->v.push_back(v);
  vertices.insert(std::pair<Vertex*,unsigned int>(v,idx));
}

void Graph::addEdge(Edge *edge) {
  this->e.push_back(edge);
}

// void Graph::segment() {
//   unsigned int i = 0;
//   while(i < e.size()) {
//     if(e[i]->weight < this->threshold) {
//       Vertex *va = e[i]->a;
//       Vertex *vb = e[i]->b;

//       size_t pixel_count_b = vb->pixels.size();
//       for(int j=0; j<pixel_count_b; j++) {
//         va->pixels.push_back(vb->pixels[j]);
//       }
//       delete e[i];
//       e[i] = NULL;
//       e.erase(e.begin() + i);

//       for(unsigned int j=0; j<e.size(); j++) {
//         if(e[j]->a == vb) {
//           e[j]->a = va;
//         } else if(e[j]->b == vb) {
//           e[j]->b = va;
//         }
//       }

//       std::map<Vertex*,unsigned int>::iterator it;
//       it = vertices.find(vb);
//       vertices.erase(vb);
//       delete vb;
//       unsigned int d = std::distance(vertices.begin(), it);
//       v.erase(v.begin() + d);

//     } else {
//       i++;
//     }
//   }
// }

void Graph::segment() {
  unsigned int i = 0;
  int c = 0;
  while(i < e.size()) {
    if(e[i]->weight < this->threshold) {
      c++;
      std::cout << " " << e[i]->weight;
      // Vertex *va = e[i]->a;
      // Vertex *vb = e[i]->b;

      // size_t pixel_count_b = vb->pixels.size();
      // for(int j=0; j<pixel_count_b; j++) {
      //   va->pixels.push_back(vb->pixels[j]);
      // }
      // delete e[i];
      // e[i] = NULL;
      // e.erase(e.begin() + i);

      // for(unsigned int j=0; j<e.size(); j++) {
      //   if(e[j]->a == vb) {
      //     e[j]->a = va;
      //   } else if(e[j]->b == vb) {
      //     e[j]->b = va;
      //   }
      // }
    }
    i++;
  }

  std::cout << "\nNumber of edge have weight greater than threshold: " << c << std::endl;
}