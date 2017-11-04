#ifndef _VIEWER_H_
#define _VIEWER_H_

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
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

		GLFWwindow *window;
		//Control variables
		double xpos, ypos;
		double last_xpos, last_ypos;
		int button_state;
		// Initial horizontal angle : toward -Z
		float horizontalAngle;
		// Initial vertical angle : none
		float verticalAngle;
		float speed; // 3 units / second
		float mouseSpeed;


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

		void setLocations(ShaderVarLocation loc) {
			shaderVarLocation = loc;
		}

		void setViewCoordinator(bool viewcoord) {
			viewCoordinator = viewcoord;
		}

		void setFW3Window(GLFWwindow *w) {
			this->window = w;
		}

		void setup(std::vector<Model3D*> m);
		void addModel(const Model3D *model);
		void drawCoordinator();
		void drawScene();

		GLint gotNormal(int index) {
			return models[index]->gotNormal();
		}

		void setupViewpointsArroundObject();
};

#endif