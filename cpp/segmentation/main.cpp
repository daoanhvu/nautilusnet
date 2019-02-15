#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <string>

#include "graph.h"

using namespace std;
using namespace cv;

int main(int argc, char **argv) {
  String imgName;

  if(argc < 2) {
    cout << "Not enough parameters!" << endl;
    return -1;
  }

  const float RF = 0.299f;
  const float GF = 0.587f;
  const float BF = 0.114f;

  imgName = argv[1];

  // Mat image = imread(imgName, IMREAD_COLOR);
  Mat image = imread("/home/vdao/data/images/person.png", IMREAD_COLOR);

  if(image.empty()) {
    cout << "Can't read image!" << endl;
    return -1;
  }

  cout << "Read image OK" << endl;
  cout << "Height: " << image.rows << endl;
  cout << "Width: " << image.cols << endl;
  cout << "Channels: " << image.channels() << endl;

  Vec3f imgPixel;
  Vertex *vertex;
  Vertex *v0;
  Edge *edge;
  Pixel pixel;
  Graph g(0.75f);
  std::vector<Vertex*> vertices;
  float dr, db, dg;
  for(int r = 0; r < image.rows; r++) {
    for(int c = 0; c < image.cols; c++) {
      imgPixel = image.at<Vec3f>(r, c);
      pixel.x = c;
      pixel.y = r;
      pixel.b = imgPixel[0];
      pixel.g = imgPixel[1];
      pixel.r = imgPixel[2];
      vertex = new Vertex;
      vertex->pixels.push_back(pixel);
      g.addVertex(vertex, image.cols * r + c);
      vertices.push_back(vertex);

      int r0 = r;
      int c0 = c -1;
      // https://www.w3.org/TR/AERT/#color-contrast
      if( c0 >= 0) {
        v0 = vertices[image.cols * r0 + c0];
        edge = new Edge;
        edge->a = v0;
        edge->b = vertex;
        db = abs(pixel.b - v0->pixels[0].b);
        dg = abs(pixel.g - v0->pixels[0].g);
        dr = abs(pixel.r - v0->pixels[0].r);
        edge->weight = RF * dr + GF * dg + BF * db;
        g.addEdge(edge);
      }

      r0 = r - 1;
      if( c0 >= 0 && r0 >=0 ) {
        v0 = vertices[image.cols * r0 + c0];
        edge = new Edge;
        edge->a = v0;
        edge->b = vertex;
        db = abs(pixel.b - v0->pixels[0].b);
        dg = abs(pixel.g - v0->pixels[0].g);
        dr = abs(pixel.r - v0->pixels[0].r);
        edge->weight = RF * dr + GF * dg + BF * db;
        g.addEdge(edge);
      }

      c0 = c;
      if(r0 >=0 ) {
        v0 = vertices[image.cols * r0 + c0];
        edge = new Edge;
        edge->a = v0;
        edge->b = vertex;
        db = abs(pixel.b - v0->pixels[0].b);
        dg = abs(pixel.g - v0->pixels[0].g);
        dr = abs(pixel.r - v0->pixels[0].r);
        edge->weight = RF * dr + GF * dg + BF * db;
        g.addEdge(edge);
      }

    }
  }

  cout << "Built graph OK!" << endl;
  cout << g << endl;

  g.segment();

  cout << g << endl;

  return 0;
}

