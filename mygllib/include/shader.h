#ifndef SHADER_HPP
#define SHADER_HPP

#ifdef __ANDROID__
	#include "EGL/egl.h"
	#ifdef __GLES2__
		#include "GLES2/gl2.h"
		#include "GLES2/gl2ext.h"
	#else
		#include "GLES3/gl3.h"
		#include "GLES3/gl3ext.h"
	#endif
#else
    #include <GL/gl.h>
#endif

enum BUFFER_TYPE {
    ONE_BUFFER,
    POSITION_NORMAL_AND_COLOR
};

typedef struct tagLocation {
    GLuint positionLocation;
    GLuint normalLocation;
    GLuint colorLocation;
    GLuint textureLocation;
    
    //uniform variables
    GLint mvpMatrixId;
    GLuint viewMatrixId;
    GLuint modelMatrixId;
	GLuint modelViewMatrixId;
    GLuint useNormalLocation;
    GLuint useLightingLocation;
    GLuint pointSizeLocation;
    GLint lightPos1ID;
    GLuint lightPos2ID;
    GLuint lightColor1ID;
    GLuint lightColor2ID;
} ShaderVarLocation;


GLuint loadShaders(const char * vertex_file_path,const char * fragment_file_path);

#endif
