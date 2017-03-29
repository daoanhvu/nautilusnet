#include <iostream>
#include <fstream>

#include <string>
#include <cstring>
#include <sstream>
#include <gm.hpp>
#include <camera.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

#define RAD(x) (x*3.14159f/180.0f)
#define BYTES_PER_PROP 4

using namespace std;
using namespace gm;
using namespace fp;

typedef struct tagModel {
	int stride;
	char hasColor;
	int vertices_count;
	int face_count;
	int property_count;
	int *faces;
	float *vertices;

	tagModel() {
		property_count = 3;
		faces = 0;
		vertices = 0;
	}
} TModel;

typedef struct tagBBox3D {
	float minx;
	float maxx;
	float miny;
	float maxy;
	float minz;
	float maxz;
} BBox3d;

unsigned char* readObject(const char* filename, int &size);
int readPly(const char* filename, TModel *model, BBox3d *bbox);

void moveCameraTo(float ex, float ey, float ez, float cx, float cy, float cz, const TModel *model);

void exportImage(const TModel* model);

int main(int argc, char* args[]) {
	
	if(argc < 2) {
		cout << "Not enough parameters. \n";
		cout << "USAGE: cap3d <input>.ply OR cap3d <input>.im \n";
		return 1;
	}

	BBox3d bbox;
	TModel model;

	readPly(args[1], &model, &bbox);

	if(model.vertices != 0) {

		cout << "Min x: " << bbox.minx << "; Max x: " << bbox.maxx << std::endl;
		cout << "Min y: " << bbox.miny << "; Max y: " << bbox.maxy << std::endl;
		cout << "Min z: " << bbox.minz << "; Max z: " << bbox.maxz << std::endl;

		moveCameraTo(1.0f, 0.0f, -1.0f, 0.5f, 0.5f, 0.5f, &model);

		delete[] model.vertices;
	}

	return 0;
}

void exportImage(const TModel* model){
	cv::Mat a;
}

unsigned char* readObject(const char* filename, int &size) {
	unsigned char *data;
	ifstream f(filename, ios::binary);

	if(f.fail()) {
		return 0;
	}

	f.close();

	return data;
}

void moveCameraTo(float ex, float ey, float ez, float cx, float cy, float cz, const TModel *model){
	Camera cam;
	
	cam.setViewport(0, 0, 200, 200);
	cam.lookAt(ex, ey, ez, cx, cy, cz, 0.0f, 1.0f, 0.0f);
	float fov = RAD(45.0f) ;
	cam.setPerspective(fov, 0.1f, 99.0f);

	cv::Vec3b color;
	color[0] = 0;
	color[0] = 0;
	color[0] = 255;

	cv::Mat img(200, 200, CV_8UC3, cv::Scalar(0, 0, 0));
	float *out = new float[model->property_count * model->vertices_count];
	float p[3];
	int i, offs;
	int x, y;
	for(i=0; i<model->vertices_count; i++) {
		offs = i * model->property_count;
		cam.project(out + offs, model->vertices + offs);
		//cout << "x= " << out[offs] << "; y= " << out[offs + 1] << std::endl;

		x = (int) out[offs];
		y = (int) out[offs + 1];
		img.at<cv::Vec3b>(x, y) = color;
	}


	//write image matrice to file
	imwrite("pose2.jpg", img);



	//release memories
	delete[] out;
}

int readPly(const char* filename, TModel *model, BBox3d *bbox) {
	ifstream f(filename);
	char line[128];
	int i;
	float x, y, z;

	bbox->minx = 999999.0f;
	bbox->maxx = -9999.0f;
	bbox->miny = 999999.0f;
	bbox->maxy = -9999.0f;
	bbox->minz = 999999.0f;
	bbox->maxz = -9999.0f;

	if(f.fail()) {
		cout << "Fail to open the input file.\n";
		return 0;
	}

	while(!f.eof()) {
		f.getline(line, 128);
		istringstream ls(line);
		string token;
		ls >> token;

		if(token == "end_header") {
			
			model->vertices = new float[model->vertices_count * model->property_count];
			int offs;
			for(i=0; i<model->vertices_count; i++) {
				offs = i * model->property_count;
				f.getline(line, 64);
				istringstream ls1(line);
				ls1 >> x;
				ls1 >> y;
				ls1 >> z;

				if(x < bbox->minx)
					bbox->minx = x;
				if(x > bbox->maxx)
					bbox->maxx = x;

				if(y < bbox->miny)
					bbox->miny = y;
				if(y > bbox->maxy)
					bbox->maxy = y;

				if(z < bbox->minz)
					bbox->minz = z;
				if(z > bbox->maxz)
					bbox->maxz = z;

				model->vertices[offs] = x/150.0f;
				model->vertices[offs + 1] = y/150.0f;
				model->vertices[offs + 2] = z/150.0f;
			}

			//read faces
			for(i=0; i<model->face_count; i++) {
				
			}

			break;
		} else if(token == "element") {
			string tk2;
			ls >> tk2;
			if(tk2 == "vertex"){
				ls >> model->vertices_count;
				cout << "Number of vertex " << model->vertices_count << std::endl;
			} else if(tk2 == "face") {
				ls >> model->face_count;
			}
			cout << " " << tk2 << std::endl;
		} else if(token == "property") {

		}
		//cout << line << std::endl;
	}


	cout << "Going to close file. \n";
	f.close();
	return 1;
}