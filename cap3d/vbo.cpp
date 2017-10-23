#include "vbo.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

VBO::VBO(GLuint primitive_, GLuint drawType_): primitive(primitive_), drawType(drawType_)  {
	positionOffset = -1;
	colorOffset = -1;
	normalOffset = -1;
}

void VBO::setup(const Model3D *model, const ShaderVarLocation & location) {

	positionOffset = model->getPositionOffset() * sizeof(float);

	colorOffset = model->getColorOffset() * sizeof(float);

	normalOffset = model->getNormalOffset() * sizeof(float);

	// std::cout << "[DEBUG - VBO] primitive: " << primitive << std::endl;
	// std::cout << "[DEBUG - VBO] float_stride: " << model->getFloatStride() << std::endl;
	// std::cout << "[DEBUG - VBO] Color offset: " << colorOffset << ", hasNormal: " << normalOffset << std::endl;

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
				reinterpret_cast<void*>(positionOffset));

	if(colorOffset >= 0) {
		glEnableVertexAttribArray(location.colorLocation);
		glVertexAttribPointer(location.colorLocation, //Attribute index
			3,  //Number of component per vertex
			GL_FLOAT,
			GL_FALSE,
			float_stride_in_byte,
			// (void*)(uintptr_t)colorOffset);
			reinterpret_cast<void*>(colorOffset));
	}

	if(normalOffset >= 0) {
		glEnableVertexAttribArray(location.normalLocation);
		glVertexAttribPointer(location.normalLocation, //Attribute index
					3,  //Number of component per vertex
					GL_FLOAT,
					GL_FALSE,
					float_stride_in_byte,
					// (void*)(uintptr_t)normalOffset);
					reinterpret_cast<void*>(normalOffset));
	}

	if(textureOffset >= 0) {
		glEnableVertexAttribArray(location.textureLocation);
		glVertexAttribPointer(location.textureLocation, //Attribute index
					2,  //Number of component per vertex
					GL_FLOAT,
					GL_FALSE,
					float_stride_in_byte,
					// (void*)(uintptr_t)normalOffset);
					reinterpret_cast<void*>(textureOffset));
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

void VBO::setup(const float *vertices, int vc, int fstride, int posOffs, int colorOffs, int normalOffs, int textureOffs,
			const unsigned int *indices, 
			int idx_size, 
			const ShaderVarLocation &location) {
	float_stride = fstride;
	vertex_count = vc;
	float_stride_in_byte = float_stride * sizeof(float);

	// std::cout << "[DEBUG - VBO] primitive: " << primitive << std::endl;
	// std::cout << "[DEBUG - VBO] float_stride: " << float_stride << std::endl;
	// std::cout << "[DEBUG - VBO] hasColor: " << hasColor << " at offset " << colorIdx << ", hasNormal: " << hasNormal << " at offset " << normalIdx << std::endl;
	// std::cout << "[DEBUG - VBO] location.positionLocation: " << location.positionLocation << std::endl;

	glGenVertexArrays(1, &vertexArrayId);
	glBindVertexArray(vertexArrayId);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_count * float_stride_in_byte, vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(location.positionLocation);
	glVertexAttribPointer(location.positionLocation, //Attribute index
				3,  //Number of component per this attribute of vertex
				GL_FLOAT,
				GL_FALSE,
				float_stride_in_byte,
				reinterpret_cast<void*>(0));

	if(colorOffs >= 0) {
	 	colorOffset = colorOffs;
		glEnableVertexAttribArray(location.colorLocation);
		glVertexAttribPointer(location.colorLocation, //Attribute index
			3,  //Number of component per vertex
			GL_FLOAT,
			GL_FALSE,
			float_stride_in_byte,
			// (void*)(uintptr_t)colorOffset);
			reinterpret_cast<void*>(colorOffset));
	}

	if(normalOffs >= 0) {
		normalOffset = normalOffs;
		glEnableVertexAttribArray(location.normalLocation);
		glVertexAttribPointer(location.normalLocation, //Attribute index
					3,  //Number of component per vertex
					GL_FLOAT,
					GL_FALSE,
					float_stride_in_byte,
					// (void*)(uintptr_t)normalOffs);
					reinterpret_cast<void*>(normalOffs));
	}

	if(textureOffs >= 0) {
		textureOffset = textureOffs;
		glEnableVertexAttribArray(location.textureLocation);
		glVertexAttribPointer(location.textureLocation, //Attribute index
					2,  //Number of component per vertex
					GL_FLOAT,
					GL_FALSE,
					float_stride_in_byte,
					// (void*)(uintptr_t)normalOffset);
					reinterpret_cast<void*>(textureOffset));
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
	glm::vec3 object_center = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 translationMatrix = glm::translate(glm::mat4(), object_center);
	glm::mat4 invertTranslationMatrix = glm::translate(glm::mat4(), - object_center);
	glm::mat4 rotationMatrix = glm::mat4(1.0);
	glm::mat4 scalingMatrix = glm::mat4();
	// scalingMatrix = glm::scale(glm::mat4(), glm::vec3(.5f, .5f, .5f));
	// modelMatrix = translationMatrix * rotationMatrix * scalingMatrix;
	modelMatrix = translationMatrix * rotationMatrix * invertTranslationMatrix * scalingMatrix;
	glm::mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;

	glUniform1i(shaderVarLocation.useNormalLocation, (normalOffset>=0)?1:0);
	glUniform1i(shaderVarLocation.useLightingLocation, 1);

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

