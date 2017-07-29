#ifndef _VIEWER_H_
#define _VIEWER_H_

#include <vector>
#include "camera.h"
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

		float pointSize;
		int needLighting;

		float bgColor[3];
		Camera camera;

		vector<VBOModel> models;

	public:
		Viewer();
		virtual ~Viewer() {
			int s = models.size();
			for(int i=0; i<s; i++) {
				delete models[i].model;
				delete models[i].vbo;
			}
		}

		void setup();
		void drawCoordinator();
		void drawScene();
};

#endif