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
		glm::mat4 translationMatrix;
		glm::mat4 rotationMatrix;

	public:
		VBO();
		VBO(GLuint primitive_, GLuint drawType_);
		virtual ~VBO();

		void releaseBuffer();

		GLint gotNormal();

		void rotate(float alpha, glm::vec3 rotAxis);

		void getComponentConfig(float *config);

		void setDrawPrimitive(GLuint dp);
#ifdef _HAS_MODEL3D_
		void setup(const Model3D *model, const ShaderVarLocation & location);
#endif
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

		void draw(	const ShaderVarLocation &shaderVarLocation, 
					const glm::mat4 &global_rotation_matrix,
					const glm::mat4 &projectionMatrix, 
					const glm::mat4 &viewMatrix);
};

#endif