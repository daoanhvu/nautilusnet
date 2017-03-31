#include <GL/glew.h>
#include <GL/glut.h>
#include <string>

#include <glm/glm.hpp>

using namespace std;

class GLRenderer {
	private:
		GLuint programId;
		GLuint vertexShaderId;
		GLuint fragmentShaderId;

		glm::mat4 view_matrix;
		glm::mat4 pp_matrix;
		

	public:	
		GLRenderer();
		~GLRenderer();

		void initGL();

		static GLuint createShader(GLenum shaderType, string source);

		int storeScene(GLuint x, GLuint y, GLuint width, GLuint height);

		
};