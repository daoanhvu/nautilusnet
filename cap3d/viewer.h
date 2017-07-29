#ifndef _VIEWER_H_
#define _VIEWER_H_

#include <vector>
#include <GL/glew.h>
#include "camera.h"
#include "vbo.h"
#include "reader/model3d.h"

using namespace std;

typedef struct tagVBOModel {
	Model3D *model;
	VBO *vbo;
} VBOModel;

class Viewer {
	protected:
		int positionLocation;
		int colorLocation;
		int normalLocation;
		int textureLocation;

		int width;
		int height;

		bool viewCoordinator;
		float pointSize;
		int needLighting;

		float bgColor[3];
		Camera camera;

		VBO coordinatorVBO;
		vector<VBOModel> models;

		void setupCoordinator();

	public:
		Viewer();
		virtual ~Viewer() {
			int s = models.size();
			for(int i=0; i<s; i++) {
				delete models[i].model;
				delete models[i].vbo;
			}
		}

		void setLocations(int pl, int cl, int nl, int tl) {
			positionLocation = pl;
			colorLocation = cl;
			normalLocation = nl;
			textureLocation = tl;
		}

		void setup();
		void drawCoordinator();
		void drawScene();
};

#endif