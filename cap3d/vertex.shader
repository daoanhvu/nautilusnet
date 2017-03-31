layout(location=0) in vec3 vertex_position_modelspace;
uniform mat4 mvp;
void main() {
    gl_Position = mvp * vec4(vertex_position_modelspace, 1);
}