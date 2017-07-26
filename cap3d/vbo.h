#ifndef _VBO_H_
#define _VBO_H_

#include <vector>
#include <GL/glew.h>
#include "reader/model3d.h"

using namespace std;

class VBO {
	protected:
		GLuint drawType;
		GLuint bufferType;
		GLuint buffer;
		GLuint element_buffer;
		int bufferCount;

		int colorIdx;
		int positionIdx;
		int normalIdx;
		int textureIdx;

		bool hasColor;
		bool hasNormal;
		bool hasTexture;

	public:
		VBO(const Model3D *model);
		virtual ~VBO() {
			glDeleteBuffers(1, &buffer);
			glDeleteBuffers(1, &element_buffer);
		}

		void setup(int vertex_count, int float_stride, float* vertices_buf_data) {
			glGenBuffers(1, &buffer);
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(bufferType, vertex_count * float_stride, vertices_buf_data, drawType);

			glGenBuffers(1, &element_buffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size * sizeof(unsigned short), indices , drawType);
		}

		void draw() {
			int offset = 0;
			// int stride;
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
			glVertexAttribPointer(0, //Attribute index
				3,  //Number of component per vertex
				GL_FLOAT,
				GL_FALSE,
				float_stride,
				(void*)offset);
			offset += 3 * sizeof(float);

			if(hasColor) {
				glVertexAttribPointer(0, //Attribute index
					3,  //Number of component per vertex
					GL_FLOAT,
					GL_FALSE,
					float_stride,
					(void*)offset);
				offset += 3 * sizeof(float);
			}

			if(hasNormal) {
				glVertexAttribPointer(0, //Attribute index
					3,  //Number of component per vertex
					GL_FLOAT,
					GL_FALSE,
					float_stride,
					(void*)offset);
				offset += 3 * sizeof(float);
			}

			if(hasTexture) {

			}

			// Index buffer
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);

			if(primitive == GL_TRIANGLES) {

			} else if (primitive == GL_LINES) {

			} else if(primitive == GL_POINTS) {

			}


			glDisableVertexAttribArray(0);
		}
};

#endif