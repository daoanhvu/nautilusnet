#ifndef _VBO_H_
#define _VBO_H_

#include <vector>
#include <glm/glm.hpp>
#include "shader.h"

#if defined(__ANDROID__) || defined(__IOS__)
	#include "EGL/egl.h"
	#ifdef __GLES2__
		#include "GLES2/gl2.h"
		#include "GLES2/gl2ext.h"
	#else
		#include "GLES3/gl3.h"
		#include "GLES3/gl3ext.h"
	#endif
#else
	#include <GL/glew.h>
	// #include "util/glUtil.h"
	#include "reader/model3d.h"
	#include "common/defines.h"
#endif



using namespace std;

class VBO {
	protected:
		GLuint drawType;
		GLuint vertexArrayId;
		GLuint colorVertexArrayId;
		GLuint buffer;
		GLuint colorBuffer;
		GLuint elementBuffer;

		//GL_TRIANGLES
		//GL_POINTS..
		GLuint primitive;
		int bufferCount;

		bool useElementBuffer;
		unsigned int vertex_count;
		unsigned int index_size;

		//Must be one of GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, or GL_UNSIGNED_INT
		GLenum indexDataType;

		int floatStride;
		int colorOffset;
		int positionOffset;
		int normalOffset;
		int textureOffset;

		glm::mat4 modelMatrix;
		glm::mat4 translationMatrix;
		glm::mat4 rotationMatrix;

	public:
		VBO();
		VBO(GLenum _indexDataType, GLuint primitive_, GLuint drawType_);
		virtual ~VBO();

		void releaseBuffer();

		GLint gotNormal();

		GLuint getBuffer();

		GLuint getElementBuffer();

		void rotate(float alpha, glm::vec3 rotAxis);

		void getComponentConfig(float *config);

		unsigned int getIndexSize();

		GLuint getPrimitive();
		void setPrimitive(GLuint dp);

		GLenum getIndexDataType();
		void setIndexDataType(GLenum type);

		void setupColor(const float *data, unsigned int dataSize, int stride, int offset, int colorLocation);
#ifdef _HAS_MODEL3D_
		void setup(const Model3D *model, const ShaderVarLocation & location);
#endif
		void setupIndexElements(
#ifdef __GLES__
			const unsigned short *indices, 
#else
			const unsigned int *indices, 
#endif
			int idxSize);
		void setup(const float *vertices, 
			unsigned int vc, 
			int fstride, 
			int posOffs, 
			int colorOffs, 
			int normalOffs, 
			int textureOffs,
			const ShaderVarLocation &location);

		void drawElements(const ShaderVarLocation &shaderVarLocation, 
				const glm::mat4 &global_rotation_matrix,
				const glm::mat4 &projectionMatrix, 
				const glm::mat4 &viewMatrix);

		void draw(	const ShaderVarLocation &shaderVarLocation, 
					const glm::mat4 &global_rotation_matrix,
					const glm::mat4 &projectionMatrix, 
					const glm::mat4 &viewMatrix);
};

#endif