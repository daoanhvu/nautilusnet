#ifndef _VBO_H_
#define _VBO_H_

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "reader/model3d.h"
#include "common/defines.h"

using namespace std;

class VBO {
	protected:
		GLuint drawType;
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
		GLsizei float_stride_in_byte;
		int colorIdx;
		int positionIdx;
		int normalIdx;
		int textureIdx;

		bool hasColor;
		bool hasNormal;
		bool hasTexture;

		Model3D *mModel;
		glm::mat4 modelMatrix;

	public:
		VBO():mModel(NULL){}
		VBO(Model3D *model, GLuint primitive_, GLuint drawType_);
		virtual ~VBO() {
			glDeleteBuffers(1, &buffer);

			if(useElementBuffer) {
				glDeleteBuffers(1, &element_buffer);
			}
		}

		void releaseBuffer() {
			glDeleteBuffers(1, &buffer);

			if(useElementBuffer) {
				glDeleteBuffers(1, &element_buffer);
			}	
		}

		GLint gotNormal() {
			return hasNormal?1:0;
		}

		void getComponentConfig(float *config) {
			config[0] = 0.0f;
			config[1] = 0.0f;

			if(hasNormal)
				config[0] = 1.0f;

			if(hasColor)
				config[1] = 1.0f;
		}

		void setDrawPrimitive(GLuint dp) {
			primitive = dp;
		}

		void setup();
		void setup(const float *vertices, int vc, int fstride, const unsigned short *indices, int idx_size);

		void draw(GLuint positionHandlerIndex, GLuint colorHandlerIndex, GLuint normalHandlerIndex);
};

#endif