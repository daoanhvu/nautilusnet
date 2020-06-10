#include "vbo.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

VBO::VBO(): indexDataType(GL_UNSIGNED_SHORT), drawType(GL_STATIC_DRAW), 
			positionOffset(-1), 
			colorOffset(-1), 
			normalOffset(-1), 
			textureOffset(-1) {

	floatStride = 0;
	useElementBuffer = false;
	rotationMatrix = glm::mat4(1.0f);
}


VBO::VBO(GLenum indexDataType_, GLuint primitive_, GLuint drawType_): indexDataType(indexDataType_), 
		primitive(primitive_), drawType(drawType_)  {
	positionOffset = -1;
	colorOffset = -1;
	normalOffset = -1;
	textureOffset = -1;
	floatStride = 0;
	useElementBuffer = false;
	rotationMatrix = glm::mat4(1.0f);
}

VBO::~VBO() {
	glDeleteBuffers(1, &buffer);

	if(useElementBuffer) {
		glDeleteBuffers(1, &elementBuffer);
	}
#ifndef __GLES2__
	glDeleteVertexArrays(1, &vertexArrayId);
#endif
}

GLenum VBO::getIndexDataType() {
	return indexDataType;
}

void VBO::setIndexDataType(GLenum type) {
	indexDataType = type;
}

void VBO::getComponentConfig(float *config) {
	config[0] = 0.0f;
	config[1] = 0.0f;

	if(normalOffset>=0)
		config[0] = 1.0f;

	if(colorOffset>=0)
		config[1] = 1.0f;
}

GLuint VBO::getPrimitive() {
	return primitive;
}

void VBO::setPrimitive(GLuint dp) {
	primitive = dp;
}

GLuint VBO::getBuffer() {
	return buffer;
}

GLuint VBO::getElementBuffer() {
	return elementBuffer;
}

unsigned int VBO::getIndexSize() {
	return index_size;
}

#ifdef _HAS_MODEL3D_
void VBO::setup(const Model3D *model, const ShaderVarLocation & location) {

	positionOffset = model->getPositionOffset() * sizeof(float);

	colorOffset = model->getColorOffset() * sizeof(float);

	normalOffset = model->getNormalOffset() * sizeof(float);	

	unsigned int *indices;
	unsigned int buff_len;
	float_stride = model->getFloatStride();
	vertex_count = model->getVertexCount();
	float *vertices_buf_data = model->getVertexBuffer(buff_len);

	//Bind position, normal and color
	GLsizei stride_in_byte = float_stride * sizeof(float);

	glGenVertexArrays(1, &vertexArrayId);
	glBindVertexArray(vertexArrayId);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_count * stride_in_byte, vertices_buf_data, drawType);

	// std::cout << "[DEBUG - VBO] primitive: " << primitive << std::endl;
	std::cout << "[DEBUG - VBO] float_stride: " << model->getFloatStride() << std::endl;
	std::cout << "[DEBUG - VBO] float stride in byte: " << stride_in_byte << std::endl;
	std::cout << "[DEBUG - VBO] Position offset: " << positionOffset << std::endl;
	std::cout << "[DEBUG - VBO] Color offset: " << colorOffset << ", Normal Offset: " << normalOffset << std::endl;

	glEnableVertexAttribArray(location.positionLocation);
	glVertexAttribPointer(location.positionLocation, //Attribute index
				3,  //Number of component per this attribute of vertex
				GL_FLOAT,
				GL_FALSE,
				stride_in_byte,
				//(void*)(uintptr_t)0); //TODO:Hard code here!!!!!
				reinterpret_cast<void*>(positionOffset));

	if(colorOffset >= 0) {
		glEnableVertexAttribArray(location.colorLocation);
		glVertexAttribPointer(location.colorLocation, //Attribute index
			3,  //Number of component per vertex
			GL_FLOAT,
			GL_FALSE,
			stride_in_byte,
			// (void*)(uintptr_t)colorOffset);
			reinterpret_cast<void*>(colorOffset));
	}

	if(normalOffset >= 0) {
		glEnableVertexAttribArray(location.normalLocation);
		glVertexAttribPointer(location.normalLocation, //Attribute index
					3,  //Number of component per vertex
					GL_FLOAT,
					GL_FALSE,
					stride_in_byte,
					// (void*)(uintptr_t)normalOffset);
					reinterpret_cast<void*>(normalOffset));
	}

	if(textureOffset >= 0) {
		glEnableVertexAttribArray(location.textureLocation);
		glVertexAttribPointer(location.textureLocation, //Attribute index
					2,  //Number of component per vertex
					GL_FLOAT,
					GL_FALSE,
					stride_in_byte,
					// (void*)(uintptr_t)normalOffset);
					reinterpret_cast<void*>(textureOffset));
	}

	indices = model->getElementIndices(index_size);
	useElementBuffer = false;
	if(indices != NULL) {
		useElementBuffer = true;
		std::cout << "[DEBUG] useElementBuffer: " << useElementBuffer << std::endl;
		std::cout << "[DEBUG] Number of index: " << index_size << endl;
		glGenBuffers(1, &element_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size * sizeof(unsigned int), indices , drawType);
		delete[] indices;

		if(primitive == GL_UNKNOWN_PRIMITIVE) {
			primitive = GL_TRIANGLES;
		}

	} else if(primitive == GL_UNKNOWN_PRIMITIVE) {
		primitive = GL_POINTS;
	}

	//Now we can release buffer data 
	delete[] vertices_buf_data;
}
#endif

void VBO::setupColor(const float *data, unsigned int dataSize, int stride,
	int offset, int colorLocation) {

	GLsizei stride_in_byte = stride * sizeof(float);
#ifndef __GLES2__
	glGenVertexArrays(1, &colorVertexArrayId);
	glBindVertexArray(colorVertexArrayId);
#endif
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, dataSize * stride_in_byte, data, drawType);

	glEnableVertexAttribArray(colorLocation);
	glVertexAttribPointer(colorLocation, //Attribute index
			4,  //Number of component per vertex
			GL_FLOAT,
			GL_FALSE,
			stride_in_byte,
			0);
}

void VBO::setupIndexElements(
#ifdef __GLES__
			const unsigned short *indices,
#else
			const unsigned int *indices, 
#endif
			int idxSize) {

	this->index_size = idxSize;
	this->useElementBuffer = false;
	
#ifdef __GLES__
	GLsizeiptr indexDataSize = this->index_size * sizeof(unsigned short);
#else
	GLsizeiptr indexDataSize = this->index_size * sizeof(unsigned int);
#endif		
	this->useElementBuffer = true;
	glGenBuffers(1, &elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSize, indices , drawType);
}

void VBO::setup(const float *vertices, unsigned int vc, int fstride,
			int posOffs, int colorOffs, int normalOffs, int textureOffs,
			const ShaderVarLocation &location) {

	this->floatStride = fstride;
	this->vertex_count = vc;
	GLsizei stride_in_byte = floatStride * sizeof(float);

#if defined(_LINUX_)|| defined(__WIN32)
	std::cout << "[DEBUG - VBO] primitive: " << primitive << std::endl;
	std::cout << "[DEBUG - VBO] float_stride: " << float_stride << std::endl;
	std::cout << "[DEBUG - VBO] hasColor: " << colorOffs << ", hasNormal: " << normalOffs << std::endl;
	std::cout << "[DEBUG - VBO] location.positionLocation: " << location.positionLocation << std::endl;
#endif

#ifndef __GLES2__
	glGenVertexArrays(1, &vertexArrayId);
	glBindVertexArray(vertexArrayId);
#endif
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_count * stride_in_byte, vertices, drawType);

	this->positionOffset = posOffs;
	glEnableVertexAttribArray(location.positionLocation);
	glVertexAttribPointer(location.positionLocation, //Attribute index
				3,  //Number of component per this attribute of vertex
				GL_FLOAT,
				GL_FALSE,
				stride_in_byte,
				reinterpret_cast<void*>(this->positionOffset));

	if(colorOffs >= 0) {
	 	this->colorOffset = colorOffs;
		glEnableVertexAttribArray(location.colorLocation);
		glVertexAttribPointer(location.colorLocation, //Attribute index
			4,  //Number of component per vertex
			GL_FLOAT,
			GL_FALSE,
			stride_in_byte,
			reinterpret_cast<void*>(colorOffset));
	}

	if(normalOffs >= 0) {
		this->normalOffset = normalOffs;
		glEnableVertexAttribArray(location.normalLocation);
		glVertexAttribPointer(location.normalLocation, //Attribute index
					3,  //Number of component per vertex
					GL_FLOAT,
					GL_FALSE,
					stride_in_byte,
					reinterpret_cast<void*>(normalOffset));
	}

	if(textureOffs >= 0) {
		this->textureOffset = textureOffs;
		glEnableVertexAttribArray(location.textureLocation);
		glVertexAttribPointer(location.textureLocation, //Attribute index
					2,  //Number of component per vertex
					GL_FLOAT,
					GL_FALSE,
					stride_in_byte,
					reinterpret_cast<void*>(textureOffset));
	}
}

void VBO::releaseBuffer() {
	glDeleteBuffers(1, &buffer);

	if(useElementBuffer) {
		glDeleteBuffers(1, &elementBuffer);
	}
#ifndef __GLES2__
	glDeleteVertexArrays(1, &vertexArrayId);
#endif
}

void VBO::rotate(float alpha, glm::vec3 rotAxis) {
	rotationMatrix = glm::rotate(rotationMatrix, alpha, rotAxis);
}

// TODO: This is for GLES only, please update it
void VBO::drawElements(const ShaderVarLocation &shaderVarLocation, 
				const glm::mat4 &global_rotation_matrix,
				const glm::mat4 &projectionMatrix, 
				const glm::mat4 &viewMatrix) {

//	 glm::vec3 objectCenter = glm::vec3(0.0f, 0.0f, 0.0f);
//	 translationMatrix = glm::translate(glm::mat4(), objectCenter);
//	 glm::mat4 invertTranslationMatrix = glm::translate(glm::mat4(), -objectCenter);
//	 glm::mat4 scalingMatrix = glm::mat4();
//	 modelMatrix = translationMatrix * rotationMatrix * invertTranslationMatrix * scalingMatrix;
//	 /* Apply global rotation to this object */
//	 modelMatrix = modelMatrix * global_rotation_matrix;
//	 glm::mat4 modelView = viewMatrix * modelMatrix;
//	 glm::mat4 MVP = projectionMatrix * modelView;
//	 /* Send our transformation to the currently bound shader,
//	 	in the "MVP" uniform
//	 */
//	 glUniformMatrix4fv(shaderVarLocation.mvpMatrixId, 1, GL_FALSE, &MVP[0][0]);
//	 glUniformMatrix4fv(shaderVarLocation.modelViewMatrixId, 1, GL_FALSE, &modelView[0][0]);


	auto stride_in_byte = 6 * sizeof(float);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glEnableVertexAttribArray(shaderVarLocation.positionLocation);
	glVertexAttribPointer(shaderVarLocation.positionLocation, //Attribute index
				3,  //Number of component per this attribute of vertex
				GL_FLOAT,
				GL_FALSE,
				stride_in_byte,
				reinterpret_cast<void*>(0));

	glEnableVertexAttribArray(shaderVarLocation.normalLocation);
	glVertexAttribPointer(shaderVarLocation.normalLocation, //Attribute index
				3,  //Number of component per normal vector
				GL_FLOAT,
				GL_FALSE,
				stride_in_byte,
				reinterpret_cast<void*>(3));

	stride_in_byte = 4 * sizeof(float);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glEnableVertexAttribArray(shaderVarLocation.colorLocation);
	glVertexAttribPointer(shaderVarLocation.colorLocation, //Attribute index
			4,  //Number of component per color. We have red, green, blue and alpha
			GL_FLOAT,
			GL_FALSE,
			stride_in_byte,
			0);
	// Bind buffer for indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
	glDrawElements(
			primitive,      // mode
			index_size,    // count
			indexDataType,   // type
			NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(shaderVarLocation.positionLocation);
    glDisableVertexAttribArray(shaderVarLocation.normalLocation);
    glDisableVertexAttribArray(shaderVarLocation.colorLocation);
}

void VBO::draw(const ShaderVarLocation &shaderVarLocation, 
				const glm::mat4 &global_rotation_matrix,
				const glm::mat4 &projectionMatrix, 
				const glm::mat4 &viewMatrix) {
	glm::vec3 object_center = glm::vec3(0.0f, 0.0f, 0.0f);
	translationMatrix = glm::translate(glm::mat4(), object_center);
	glm::mat4 invertTranslationMatrix = glm::translate(glm::mat4(), - object_center);
	glm::mat4 scalingMatrix = glm::mat4();
	// scalingMatrix = glm::scale(glm::mat4(), glm::vec3(.5f, .5f, .5f));
	// modelMatrix = translationMatrix * rotationMatrix * scalingMatrix;
	modelMatrix = translationMatrix * rotationMatrix * invertTranslationMatrix * scalingMatrix;


	//Apply global rotation to this object
	modelMatrix = modelMatrix * global_rotation_matrix;

	glm::mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;

	glUniform1i(shaderVarLocation.useNormalLocation, (normalOffset>=0)?1:0);
	glUniform1i(shaderVarLocation.useLightingLocation, 1);

	// Send our transformation to the currently bound shader,
	// in the "MVP" uniform
	glUniformMatrix4fv(shaderVarLocation.mvpMatrixId, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(shaderVarLocation.modelMatrixId, 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(shaderVarLocation.viewMatrixId, 1, GL_FALSE, &viewMatrix[0][0]);

#ifndef __GLES2__
	glBindVertexArray(vertexArrayId);
#endif
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	if(useElementBuffer) {
		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
		glDrawElements(
				primitive,      // mode
				index_size,    // count
				indexDataType,   // type
				reinterpret_cast<void*>(0)
			);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	} else {
		glDrawArrays(primitive, 0, vertex_count);
	}
#ifndef __GLES2__
	glBindVertexArray(0);
#endif
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLint VBO::gotNormal() {
	return (normalOffset>=0)?1:0;
}
