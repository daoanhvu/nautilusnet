#ifndef _VIEWER_H_
#define _VIEWER_H_

#include <vector>
#include <GL/glew.h>
#include "camera.h"
#include "vbo.h"
#include "reader/model3d.h"

using namespace std;

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

		glm::mat4 rotationMatrix;

		//Camera setup
		glm::vec3 eye_center;
		glm::vec3 cam_pos;
		glm::mat4 projectionMatrix;
		glm::mat4 viewMatrix;

		VBO coordinatorVBO;
		vector<VBO*> models;

	public:
		Viewer();
		virtual ~Viewer() {
			int s = models.size();
			for(int i=0; i<s; i++) {
				if(models[i] != NULL)
					delete models[i];
			}
		}

		void setupCoordinator();
		void setupCamera(int viewWidth, int viewHeight, glm::vec3 campos, glm::vec3 ec);

		/**
			Bind locations from shader to this viewer
		*/
		void setLocations(int pl, int cl, int nl, int tl) {
			shaderVarLocation.positionLocation = pl;
			shaderVarLocation.colorLocation = cl;
			shaderVarLocation.normalLocation = nl;
			shaderVarLocation.textureLocation = tl;
		}

		void setLocations(ShaderVarLocation loc) {
			shaderVarLocation = loc;
		}

		void setViewCoordinator(bool viewcoord) {
			viewCoordinator = viewcoord;
		}

		void setup(std::vector<Model3D*> m);
		void addModel(const Model3D *model);
		void drawCoordinator();
		void drawScene();

		GLint gotNormal(int index) {
			return models[index]->gotNormal();
		}
};

#endif