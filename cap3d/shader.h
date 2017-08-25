#ifndef SHADER_HPP
#define SHADER_HPP

typedef struct tagLocation {
    GLuint positionLocation;
    GLuint normalLocation;
    GLuint colorLocation;
    GLuint textureLocation;
    
    //uniform variables
    GLuint mvpMatrixId;
    GLuint viewMatrixId;
    GLuint modelMatrixId;
    GLuint useNormalLocation;
    GLuint useLightingLocation;
    GLuint pointSizeLocation;
    GLuint lightPos1ID;
    GLuint lightPos2ID;
    GLuint lightColor1ID;
    GLuint lightColor2ID;
} ShaderVarLocation;


GLuint loadShaders(const char * vertex_file_path,const char * fragment_file_path);

#endif
