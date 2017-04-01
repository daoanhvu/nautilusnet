#version 330 core
layout(location=0) in vec3 vertex_position_modelspace;
//uniform mat4 mvp;
void main() {
    //gl_Position = mvp * vec4(vertex_position_modelspace, 1);
    gl_Position.xyz = vertex_position_modelspace;
    gl_Position.w = 1.0;
}
