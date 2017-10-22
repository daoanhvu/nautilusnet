//
//  glUtil.h
//  3DVisualizer
//
//  Created by D.A. Vu on 8/15/17.
//  Copyright © 2017 D.A. Vu. All rights reserved.
//

#ifndef _GLUTIL_H
#define _GLUTIL_H

#if TARGET_IOS

#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

#else

#import <OpenGL/OpenGL.h>

// OpenGL 3.2 is only supported on MacOS X Lion and later
// CGL_VERSION_1_3 is defined as 1 on MacOS X Lion and later
#if CGL_VERSION_1_3
// Set to 0 to run on the Legacy OpenGL Profile
#define ESSENTIAL_GL_PRACTICES_SUPPORT_GL3 1
#else
#define ESSENTIAL_GL_PRACTICES_SUPPORT_GL3 0
#endif //!CGL_VERSION_1_3

#if ESSENTIAL_GL_PRACTICES_SUPPORT_GL3
#import <OpenGL/gl3.h>
#else
#import <OpenGL/gl.h>
#endif //!ESSENTIAL_GL_PRACTICES_SUPPORT_GL3

#endif // !TARGET_IOS


//The name of the VertexArrayObject are slightly different in
// OpenGLES, OpenGL Core Profile, and OpenGL Legacy
// The arguments are exactly the same across these APIs however
#if TARGET_IOS
#define glBindVertexArray glBindVertexArrayOES
#define glGenVertexArrays glGenVertexArraysOES
#define glDeleteVertexArrays glDeleteVertexArraysOES
#else
#if ESSENTIAL_GL_PRACTICES_SUPPORT_GL3
#define glBindVertexArray glBindVertexArray
#define glGenVertexArrays glGenVertexArrays
#define glGenerateMipmap glGenerateMipmap
#define glDeleteVertexArrays glDeleteVertexArrays
#else
#define glBindVertexArray glBindVertexArrayAPPLE
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glGenerateMipmap glGenerateMipmapEXT
#define glDeleteVertexArrays glDeleteVertexArraysAPPLE
#endif //!ESSENTIAL_GL_PRACTICES_SUPPORT_GL3
#endif //!TARGET_IOS


#endif /* _GLUTIL_H */
