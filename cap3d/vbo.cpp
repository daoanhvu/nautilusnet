#include "vbo.h"
#include <iostream>

VBO::VBO(Model3D *model, GLuint primitive_, GLuint drawType_): mModel(model), primitive(primitive_),
	drawType(drawType_)	  {

	int offs = model->getPositionOffset();
	positionIdx = offs * sizeof(float);

	offs = model->getColorOffset();
	colorIdx = offs * sizeof(float);
	hasColor = offs>=0?true:false;

	offs = model->getNormalOffset();
	normalIdx = offs * sizeof(float);
	hasNormal = offs>=0?true:false;

	std::cout << "[DEBUG - VBO] hasColor: " << hasColor << ", hasNormal: " << hasNormal << std::endl;
}

void VBO::setup() {

	unsigned short *indices;
	float_stride = mModel->getFloatStride();
	float *vertices_buf_data = mModel->getVertexBuffer(vertex_count);

	float_stride_in_byte = float_stride * sizeof(float);
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_count * float_stride_in_byte, vertices_buf_data, drawType);

	indices = mModel->getElementIndices(index_size);
	useElementBuffer = false;
	if(indices != NULL) {
		useElementBuffer = true;
		glGenBuffers(1, &element_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size * sizeof(unsigned short), indices , drawType);

		delete[] indices;
	}

	//Now we can release buffer data 
	delete[] vertices_buf_data;
}

void VBO::setup(const float *vertices, int vc, int fstride, const unsigned short *indices, int idx_size) {
	float_stride = fstride;
	vertex_count = vc;
	float_stride_in_byte = float_stride * sizeof(float);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_count * float_stride_in_byte, vertices, drawType);

	index_size = idx_size;
	useElementBuffer = false;
	if(indices != NULL) {
		useElementBuffer = true;
		glGenBuffers(1, &element_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size * sizeof(unsigned short), indices , drawType);
	}
}

void VBO::draw(GLuint positionHandlerIndex, GLuint colorHandlerIndex, GLuint normalHandlerIndex) {
	// int stride;
	glEnableVertexAttribArray(positionHandlerIndex);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glVertexAttribPointer(positionHandlerIndex, //Attribute index
				3,  //Number of component per vertex
				GL_FLOAT,
				GL_FALSE,
				float_stride_in_byte,
				(void*)(uintptr_t)0); //TODO:Hard code here!!!!!

	if(hasColor) {
		glEnableVertexAttribArray(colorHandlerIndex);
		glVertexAttribPointer(colorHandlerIndex, //Attribute index
			3,  //Number of component per vertex
			GL_FLOAT,
			GL_FALSE,
			float_stride_in_byte,
					(void*)(uintptr_t)colorIdx);
	}

	if(hasNormal) {
		glEnableVertexAttribArray(normalHandlerIndex);
		glVertexAttribPointer(normalHandlerIndex, //Attribute index
					3,  //Number of component per vertex
					GL_FLOAT,
					GL_FALSE,
					float_stride_in_byte,
					(void*)(uintptr_t)normalIdx);
	}

	if(hasTexture) {
	}

	if(useElementBuffer) {
		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
		glDrawElements(
				primitive,      // mode
				index_size,    // count
				GL_UNSIGNED_SHORT,   // type
				(void*)0           // element array buffer offset
			);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	} else {
		glDrawArrays(primitive, 0, vertex_count);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(positionHandlerIndex);
	glDisableVertexAttribArray(colorHandlerIndex);
	glDisableVertexAttribArray(normalHandlerIndex);

}

