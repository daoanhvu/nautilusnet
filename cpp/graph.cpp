#include <graph.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <opencv2/imgcodecs.hpp>

#ifdef _WINDOWS
	#include<Windows.h>
#endif

// typedef struct tagThreadParam {
// 	cv::Image *image;
// 	int gridIndex;
// 	int gridWidth;
// 	int gridHeight;
// } ThreadParam;

/**
	This is utility function and will run in its own thread
*/
// void buildGraph0(void *params) {
// 	ThreadParam* p = (ThreadParam*)params;
// 	cv::Image *img = p->image;
// 	int i, j;
// 	const int idx = p->gridIndex;
// 	std::vector<Edge> edges;
// 	Edge edge;
// 	for(i=0; i < p->gridHeight; ++i) {
// 		for(j=0; j < p->gridWidth; ++j) {
// 			if( i == 0 ) {
// 				edge.v1 = 
// 			} else {

// 			}
// 		}
// 	}
// }

Graph::Graph() {
}

Graph::~Graph() {
	image.release();

#ifdef DEBUG
	printf("Released image memory!");
#endif
}

void Graph::build() {
	int r, c;
	Vertex v;
	Edge edge;
	int height = image.size().height;
	int width = image.size().width;


#ifdef _WINDOWS

#endif

	int lastRow = height - 1;
	for(int i=0; i<height; ++i) {
		for(int j=0; j<width; ++j) {
			//TODO: Please optimize this, calculate index many times
			v.pixels.push_back(i * width + j);
			cv::Vec3b pixel = image.at<cv::Vec3b>(i, j);
			v.intensity = image[i * width + j].intensity;
			vertices.push_back(v);

			if(i==0) {
				//first neighbor
				c = j + 1;
				r = i;
				if(c < width) {
					edge.v1 = r * width + c;
					edge.v2 = i * width + j;
					edge.weight = image[edge.v2].intensity - image[edge.v1].intensity;
					edges.push_back(edge);
				}

				//second neighbor
				r = i + 1;
				if( (c < width) && (r < height) ) {
					edge.v1 = r * width + c;
					edge.v2 = i * width + j;
					edge.weight = image[edge.v2].intensity - image[edge.v1].intensity;
					edges.push_back(edge);
				}

				//3rd neighbor
				c = j;
				if( r < height )  {
					edge.v1 = r * width + c;
					edge.v2 = i * width + j;
					edge.weight = image[edge.v2].intensity - image[edge.v1].intensity;
					edges.push_back(edge);
				}
			} else if(i==lastRow) {
				//first neighbor
				c = j + 1;
				r = i - 1;
				if(c < width) {
					edge.v1 = r * width + c;
					edge.v2 = i * width + j;
					edge.weight = image[edge.v2].intensity - image[edge.v1].intensity;
					edges.push_back(edge);
				}

				//second neighbor
				c = j + 1;
				r = i;
				if(c < width) {
					edge.v1 = r * width + c;
					edge.v2 = i * width + j;
					edge.weight = image[edge.v2].intensity - image[edge.v1].intensity;
					edges.push_back(edge);
				}
			} else {
				//first neighbor
				c = j + 1;
				r = i - 1;
				if(c < width) {
					edge.v1 = r * width + c;
					edge.v2 = i * width + j;
					edge.weight = image[edge.v2].intensity - image[edge.v1].intensity;
					edges.push_back(edge);
				}

				//second neighbor
				c = j + 1;
				r = i;
				if(c < width) {
					edge.v1 = r * width + c;
					edge.v2 = i * width + j;
					edge.weight = image[edge.v2].intensity - image[edge.v1].intensity;
					edges.push_back(edge);
				}

				//3rd neighbor
				r = i + 1;
				if( (c < width) && (r < height) ) {
					edge.v1 = r * width + c;
					edge.v2 = i * width + j;
					edge.weight = image[edge.v2].intensity - image[edge.v1].intensity;
					edges.push_back(edge);
				}

				//4th neighbor
				c = j;
				if( r < height )  {
					edge.v1 = r * width + c;
					edge.v2 = i * width + j;
					edge.weight = image[edge.v2].intensity - image[edge.v1].intensity;
					edges.push_back(edge);
				}
			}
		}
	}
}

void Graph::readImage(std::string filename) {
	image = cv::imread(filename, cv::IMREAD_COLOR);
}

bool Graph::loadFromFile(std::string filename) {
	std::string line;
	int row;
	int temp;

	std::ifstream f;
	f.open(filename, std::ifstream::in | std::ifstream::binary);

	if( f.is_open() ) {
		f >> row;

		Vertex v;
		for(int i=0; i<row; ++i) {
			v.index = i;
			v.value = i;
			vertices.push_back(v);
		}

		Edge e;
		for(int i=0; i<row; ++i) {
			std::getline(f, line);
			std::istringstream s(line);
			for( int j=0; j<row; ++j ) {
				s >> temp;
				e.v1 = i;
				e.v2 = j;
				e.weight = temp;
				edges.push_back(e);
			}
		}

		f.close();
		return true;
	}

	return false;
}

void Graph::addVertex(const Vertex &v) {
	vertices.push_back(v);
}

void Graph::addEdge(int v1, int v2, int weight) {
	Edge e;
	e.v1 = v1;
	e.v2 = v2;
	e.weight = weight;
	edges.push_back(e);
}

void Graph::addEdge(const Edge &e) {
	edges.push_back(e);
}

int Graph::getNumOfVertex() {
	return vertices.size();
}

int Graph::getNumOfEdge() {
	return edges.size();
}

Vertex& Graph::getVertex(int idx) {
	return vertices[idx];
}

Edge& Graph::getEdge(int idx) {
	return edges[idx];
}