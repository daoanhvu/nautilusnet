#ifndef __GLRENDERER_H_
#define __GLRENDERER_H_

#include <GL/glew.h>

#ifdef __APPLE__
	#include <GLUT/glut.h>
#else
	#include <GL/glut.h>
#endif

#include <string>

#include <glm/glm.hpp>

// #include <camera.h>

#include <plyfile.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>


using namespace std;

class GLRenderer {
	private:
		GLuint programId;
		GLuint vertexShaderId;
		GLuint fragmentShaderId;

		//Light position
		float lx;
		float ly;
		float lz;

		//light color
		float lcr;
		float lcg;
		float lcb;

		//Camera position
		float ex;
		float ey;
		float ez;

		glm::mat4 view_matrix;
		glm::mat4 pp_matrix;

	public:
		GLRenderer();
		~GLRenderer();

		int initGL();

		GLuint getProgramId() const {
			return programId;
		}

		static GLuint createShader(GLenum shaderType, string source);

		int storeScene(GLuint x, GLuint y, GLuint width, GLuint height);


		void moveCameraTo(float ex, float ey, float ez, float cx, float cy, float cz, const PlyFile *model, const char *exportedFile);
};

#endif
