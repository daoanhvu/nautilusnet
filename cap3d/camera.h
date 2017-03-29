#ifndef _CAMERA_H
#define _CAMERA_H

#include <gm.hpp>

namespace fp {
	class Camera {
	private:
		gm::mat4 mvp;
		gm::mat4 view;
		gm::mat4 mModel;
		gm::mat4 perspective;
		gm::mat4 orthor;
		float viewport[4];

		float centerX;
		float centerY;
		float centerZ;

		float eyeX;
		float eyeY;
		float eyeZ;

		gm::vec3 up;

		float pitchAccum;
		float yawAccum;
		float rollAccum;

		gm::mat4 inverted;
		gm::mat4 rotation;
		gm::mat4 temp;
		gm::vec4 xAxis, yAxis;
		gm::vec4 rotationAxisX, rotationAxisY;
		float rotX;
		float rotY;

	public:
		Camera();
		~Camera() {}

		//void setModelMatrix(float *mm);
		//void setViewMatrix(float *vm);

		void lookAt(float ex, float ey, float ez,
					float cx, float cy, float cz, float ux, float uy, float uz);
		void setPerspective(float fov, float nearPlane, float farPlane);
		void setOrtho(float left, float top, float right, float bottom);
		void rotate(float yaw, float pitch, float roll);
		void setViewport(int left, int top, int right, int bottom);
		void project(float *out, const float *obj);
		void project(float *out, float objX, float objY, float objZ);
		void projectOrthor(float *out, const float *obj);
		void projectOrthor(float *out, float objX, float objY, float objZ);

		//on testing method
		void moveAlongForward(float d);
		
		void getView(float *v) { 
			for(int i=0; i<4; i++) { //column
				for(int j=0; j<4; j++) { //row
					v[i*4 + j] = view[i][j];
				}
			}
		}
		void getPerspective(float *v) {
			for(int i=0; i<4; i++) { //column
				for(int j=0; j<4; j++) { //row
					v[i*4 + j] = perspective[i][j];
				}
			}
		}
	};
}

#endif
