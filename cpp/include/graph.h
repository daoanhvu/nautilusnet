#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <iostream>
#include <vector>
#include <string>

#include <opencv2/core.hpp>

typedef struct tagLocation {
	int x;
	int y;
} Location;

typedef struct tagVertex {
	int index;
	std::vector<int> pixels;
	float value;
	unsigned char intensity;
} Vertex;

typedef struct tagEdge {
	int v1;
	int v2;
	int weight;
} Edge;

class Graph {
	private:
		cv::Mat image;

		std::vector<Edge> edges;
		std::vector<Vertex> vertices;

	public:	
		Graph();
		~Graph();

		void readImage(std::string filename);
		
		void addVertex(const Vertex &v);
		void addEdge(int v1, int v2, int weight);
		void addEdge(const Edge &e);
		int getNumOfVertex();
		int getNumOfEdge();
		Vertex& getVertex(int idx);
		Edge& getEdge(int idx);

		void build();

		bool loadFromFile(std::string filename);


		friend std::ostream& operator <<(std::ostream& o, const Graph& g) {
			o << "Graph:" << std::endl; 
			o << "    Image size: (" << g.image.size().width << ", " << g.image.size().height << ")" << std::endl;
			o << "    Vertex count: " << g.vertices.size() << std::endl;
			o << "    Edge count: " << g.edges.size() << std::endl;
			return o;
		}
};

#endif