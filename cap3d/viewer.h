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
		ShaderVarLocation shaderVarLocation;

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
				if(models[i].model != NULL)
					delete models[i].model;
				if(models[i].vbo != NULL)
					delete models[i].vbo;
			}
		}

		void setLocations(int pl, int cl, int nl, int tl) {
			shaderVarLocation.positionLocation = pl;
			shaderVarLocation.colorLocation = cl;
			shaderVarLocation.normalLocation = nl;
			shaderVarLocation.textureLocation = tl;
		}

		void setViewCoordinator(bool viewcoord) {
			viewCoordinator = viewcoord;
		}

		void setup();
		void drawCoordinator();
		void drawScene();
};

#endif