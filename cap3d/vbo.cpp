#include "vbo.h"
#include <iostream>

VBO::VBO(GLuint primitive_, GLuint drawType_): primitive(primitive_), drawType(drawType_)  {
}

void VBO::setup(const Model3D *model, const ShaderVarLocation & location) {

	int offs = model->getPositionOffset();
	positionIdx = offs * sizeof(float);

	offs = model->getColorOffset();
	colorIdx = offs * sizeof(float);
	hasColor = offs>=0?true:false;

	offs = model->getNormalOffset();
	normalIdx = offs * sizeof(float);
	hasNormal = offs>=0?true:false;

	std::cout << "[DEBUG - VBO] primitive: " << primitive << std::endl;
	std::cout << "[DEBUG - VBO] float_stride: " << model->getFloatStride() << std::endl;
	std::cout << "[DEBUG - VBO] hasColor: " << hasColor << " at offset " << colorIdx << ", hasNormal: " << hasNormal << " at offset " << normalIdx << std::endl;

	unsigned int *indices;
	unsigned int buff_len;
	float_stride = model->getFloatStride();
	vertex_count = model->getVertexCount();
	float *vertices_buf_data = model->getVertexBuffer(buff_len);

	//Bind position, normal and color
	float_stride_in_byte = float_stride * sizeof(float);

	glGenVertexArrays(1, &vertexArrayId);
	glBindVertexArray(vertexArrayId);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, buff_len * sizeof(float), vertices_buf_data, drawType);

	glEnableVertexAttribArray(location.positionLocation);
	glVertexAttribPointer(location.positionLocation, //Attribute index
				3,  //Number of component per this attribute of vertex
				GL_FLOAT,
				GL_FALSE,
				float_stride_in_byte,
				//(void*)(uintptr_t)0); //TODO:Hard code here!!!!!
				reinterpret_cast<void*>(0));

	if(hasColor) {
		glEnableVertexAttribArray(location.colorLocation);
		glVertexAttribPointer(location.colorLocation, //Attribute index
			3,  //Number of component per vertex
			GL_FLOAT,
			GL_FALSE,
			float_stride_in_byte,
			// (void*)(uintptr_t)colorIdx);
			reinterpret_cast<void*>(colorIdx));
	}

	if(hasNormal) {
		glEnableVertexAttribArray(location.normalLocation);
		glVertexAttribPointer(location.normalLocation, //Attribute index
					3,  //Number of component per vertex
					GL_FLOAT,
					GL_FALSE,
					float_stride_in_byte,
					// (void*)(uintptr_t)normalIdx);
					reinterpret_cast<void*>(normalIdx));
	}

	if(hasTexture) {
		glEnableVertexAttribArray(location.textureLocation);
	}

	indices = model->getElementIndices(index_size);
	useElementBuffer = false;
	if(indices != NULL) {
		useElementBuffer = true;
		// std::cout << "[DEBUG] useElementBuffer: " << useElementBuffer << std::endl;
		// std::cout << "[DEBUG] Number of index: " << index_size << endl;
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

void VBO::setup(const float *vertices, int vc, int fstride, 
	const unsigned int *indices, int idx_size, const ShaderVarLocation &location) {
	float_stride = fstride;
	vertex_count = vc;
	float_stride_in_byte = float_stride * sizeof(float);

	glGenVertexArrays(1, &vertexArrayId);
	glBindVertexArray(vertexArrayId);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_count * float_stride_in_byte, vertices, drawType);

	glEnableVertexAttribArray(location.positionLocation);
	glVertexAttribPointer(location.positionLocation, //Attribute index
				3,  //Number of component per this attribute of vertex
				GL_FLOAT,
				GL_FALSE,
				float_stride_in_byte,
				//(void*)(uintptr_t)0); //TODO:Hard code here!!!!!
				reinterpret_cast<void*>(0));

	if(hasColor) {
		glEnableVertexAttribArray(location.colorLocation);
		glVertexAttribPointer(location.colorLocation, //Attribute index
			3,  //Number of component per vertex
			GL_FLOAT,
			GL_FALSE,
			float_stride_in_byte,
			// (void*)(uintptr_t)colorIdx);
			reinterpret_cast<void*>(colorIdx));
	}

	if(hasNormal) {
		glEnableVertexAttribArray(location.normalLocation);
		glVertexAttribPointer(location.normalLocation, //Attribute index
					3,  //Number of component per vertex
					GL_FLOAT,
					GL_FALSE,
					float_stride_in_byte,
					// (void*)(uintptr_t)normalIdx);
					reinterpret_cast<void*>(normalIdx));
	}

	if(hasTexture) {
		glEnableVertexAttribArray(location.textureLocation);
	}

	index_size = idx_size;
	useElementBuffer = false;
	if(indices != NULL) {
		useElementBuffer = true;
		glGenBuffers(1, &element_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size * sizeof(unsigned int), indices , drawType);
	}
}

void VBO::draw(const ShaderVarLocation &shaderVarLocation, const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix) {
	// glm::mat4 translationMatrix = glm::translate(glm::mat4(), object_center);
	// glm::mat4 invertTranslationMatrix = glm::translate(glm::mat4(), - object_center);
	glm::mat4 translationMatrix = glm::mat4();
	glm::mat4 invertTranslationMatrix = glm::mat4();
	glm::mat4 rotationMatrix = glm::mat4();
	glm::mat4 scalingMatrix = glm::mat4();
	// scalingMatrix = glm::scale(glm::mat4(), glm::vec3(.5f, .5f, .5f));
	// modelMatrix = translationMatrix * rotationMatrix * scalingMatrix;
	modelMatrix = translationMatrix * rotationMatrix * invertTranslationMatrix * scalingMatrix;
	glm::mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;


	glUniform1i(shaderVarLocation.useNormalLocation, hasNormal);

	// Send our transformation to the currently bound shader,
	// in the "MVP" uniform
	glUniformMatrix4fv(shaderVarLocation.mvpMatrixId, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(shaderVarLocation.modelMatrixId, 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(shaderVarLocation.viewMatrixId, 1, GL_FALSE, &viewMatrix[0][0]);

	glBindVertexArray(vertexArrayId);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	if(useElementBuffer) {
		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
		glDrawElements(
				primitive,      // mode
				index_size,    // count
				GL_UNSIGNED_INT,   // type
				reinterpret_cast<void*>(0)
			);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	} else {
		glDrawArrays(primitive, 0, vertex_count);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

