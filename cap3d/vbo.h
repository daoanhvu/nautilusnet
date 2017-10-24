#ifndef _VBO_H_
#define _VBO_H_

#include <vector>
#include <GL/glew.h>
// #include "util/glUtil.h"
#include <glm/glm.hpp>
#include "shader.h"
#include "reader/model3d.h"
#include "common/defines.h"

using namespace std;

class VBO {
	protected:
		GLuint drawType;

		GLuint vertexArrayId;
		GLuint buffer;
		GLuint element_buffer;

		//GL_TRIANGLES
		//GL_POINTS..
		GLuint primitive;
		int bufferCount;

		bool useElementBuffer;
		unsigned int vertex_count;
		unsigned int index_size;

		int float_stride;
		GLsizei stride_in_byte;
		int colorOffset;
		int positionOffset;
		int normalOffset;
		int textureOffset;

		glm::mat4 modelMatrix;

	public:
		VBO();
		VBO(GLuint primitive_, GLuint drawType_);
		virtual ~VBO() {
			glDeleteBuffers(1, &buffer);

			if(useElementBuffer) {
				glDeleteBuffers(1, &element_buffer);
			}

			glDeleteVertexArrays(1, &vertexArrayId);
		}

		void releaseBuffer() {
			glDeleteBuffers(1, &buffer);

			if(useElementBuffer) {
				glDeleteBuffers(1, &element_buffer);
			}	

			glDeleteVertexArrays(1, &vertexArrayId);
		}

		GLint gotNormal() {
			return (normalOffset>=0)?1:0;
		}

		void getComponentConfig(float *config) {
			config[0] = 0.0f;
			config[1] = 0.0f;

			if(normalOffset>=0)
				config[0] = 1.0f;

			if(colorOffset>=0)
				config[1] = 1.0f;
		}

		void setDrawPrimitive(GLuint dp) {
			primitive = dp;
		}

		void setup(const Model3D *model, const ShaderVarLocation & location);
		void setup(const float *vertices, 
			int vc, 
			int fstride, 
			int posOffs, 
			int colorOffs, 
			int normalOffs, 
			int textureOffs,
			const unsigned int *indices, 
			int idx_size, 
			const ShaderVarLocation &location);

		void draw(const ShaderVarLocation &shaderVarLocation, const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix);
};

#endif