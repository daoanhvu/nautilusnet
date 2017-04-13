#include "camera.h"
#include <math.h>
#include <fstream>
#ifdef _ADEBUG
//#include <stdio.h>
//#include <iostream>
#include <android/log.h>
#define LOG_TAG "NativeCamera"
#define LOG_LEVEL 10
#define LOGI(level, ...) if (level <= LOG_LEVEL) {__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__);}
#define LOGE(level, ...) if (level <= LOG_LEVEL) {__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__);}
#endif

using namespace std;
using namespace fp;

Camera::Camera(): pitchAccum(0), yawAccum(0), rollAccum(0) {
	xAxis = gm::vec4(1, 0, 0, 0);
	yAxis = gm::vec4(0, 1, 0, 0);
}

void Camera::lookAt(float ex, float ey, float ez,
			float cx, float cy, float cz, float ux, float uy, float uz) {
	gm::vec3 f = gm::normalize(cx-ex, cy-ey, cz-ez);
	up = gm::vec3(ux, uy, uz);
	gm::vec3 s = gm::normalize(gm::cross(f, up));
	gm::vec3 u = gm::cross(s, f);
	gm::vec3 eye(ex, ey, ez);
	int i, j;

	centerX = cx;
	centerY = cy;
	centerZ = cz;

	eyeX = ex;
	eyeY = ey;
	eyeZ = ez;

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			view[i][j] = 1;

	view[0][0] = s[0]; //s.x
	view[1][0] = s[1]; //s.y
	view[2][0] = s[2]; //s.z
	view[0][1] = u[0];
	view[1][1] = u[1];
	view[2][1] = u[2];
	view[0][2] =-f[0];
	view[1][2] =-f[1];
	view[2][2] =-f[2];
	view[3][0] =-gm::dot(s, eye);
	view[3][1] =-gm::dot(u, eye);
	view[3][2] = gm::dot(f, eye);
}

void Camera::setPerspective(float fov, float nearPlane, float farPlane) {
	float aspect = (viewport[2] - viewport[0])/(viewport[3] - viewport[1]);
	int i, j;
	float tanHalfFov = tan(fov/2);

	//TODO: Should use memset
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			perspective[i][j] = 0;

	perspective[0][0] = 1 / (aspect * tanHalfFov);
	perspective[1][1] = 1 / tanHalfFov;
	perspective[2][2] = - (farPlane + nearPlane) / (farPlane - nearPlane);
	perspective[2][3] = -1;
	perspective[3][2] = -(2 * farPlane * nearPlane) / (farPlane - nearPlane);
}

void Camera::setOrtho(float left, float top, float right, float bottom) {
	int i, j;
	//TODO: Should use memset
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
			orthor[i][j] = 1;
	orthor[0][0] = 2 / (right - left);
	orthor[1][1] = 2 / (top - bottom);
	orthor[2][2] = -1;
	orthor[3][0] = - (right + left) / (right - left);
	orthor[3][1] = - (top + bottom) / (top - bottom);
}

void Camera::setViewport(int left, int top, int right, int bottom) {
	viewport[0] = left;
	viewport[1] = top;
	viewport[2] = right;
	viewport[3] = bottom;
}

/*
	yaw: rotate around right vector
	pitch: rotate around up vector
*/
void Camera::rotate(float yawR, float pitchR, float roll) {
//	std::ofstream f("storage/sdcard0/data_rotation.txt", std::ofstream::app);
	rotX = -1 * pitchR * 0.05f;
	rotY = -1 * yawR * 0.05f;

//	f << "\n New Rotation dx = " << yawR << ", dy = " << pitchR << "\n";
//	f << "model matrix: \n";
//	f << mModel;
	mModel.inverse2(inverted);
//	f << "\n inverted model matrix 1: \n";
//	f << inverted;
	rotationAxisX = inverted * xAxis;
//	f << "\n RotationAsixX: \n";
//	f << rotationAxisX;
	gm::rotateM(mModel, rotX, rotationAxisX);
//	rotation = gm::rotate(rotX, rotationAxisX);
//	f << "\n Rotation matrix about RotationAxisX: \n";
//	f << rotation;
//	temp = mModel * rotation;
//	f << "\n Model matrix after rotate by RotationAxisX: \n";
//	f << temp;

	mModel.inverse2(inverted);
	//temp.inverse2(inverted);
//	f << "\n inverted model matrix 2: \n";
//	f << inverted;
	rotationAxisY = inverted * yAxis;
//	f << "\n RotationAsixY: \n";
//	f << rotationAxisY;
	gm::rotateM(mModel, rotY, rotationAxisY);
//	rotation = gm::rotate(rotY, rotationAxisY);
//	f << "\n Rotation matrix about RotationAxisY: \n";
//	f << rotation;
//	mModel = temp * rotation;
//	f << "\n Final Model matrix : \n";
//	f << mModel << "\n\n";

//	f.close();
}

/*
	@param inPoint 3D point in world
	@param out 3D point on screen
*/
void Camera::project(float *out, const float *obj) {
	gm::vec4 tmp(obj[0], obj[1], obj[2], 1);
	tmp = (view * mModel) * tmp;
	tmp = perspective * tmp;
	tmp /= tmp[3]; //tmp.w

	tmp = tmp * 0.5f + 0.5f;
	out[0] = tmp[0] * viewport[2] + viewport[0];
	out[1] = tmp[1] * viewport[3] + viewport[1];
	out[2] = tmp[2];
}

/**
 * @param out output parameter, must be 3 in length array (or more)
 */
void Camera::project(float *out, float objX, float objY, float objZ) {
	float result;
	gm::vec4 tmp(objX, objY, objZ, 1);
	tmp = (view * mModel) * tmp;
	tmp = perspective * tmp;
	tmp /= tmp[3]; //tmp.w

	tmp = tmp * 0.5f + 0.5f;
	out[0] = tmp[0] * viewport[2] + viewport[0];
	out[1] = tmp[1] * viewport[3] + viewport[1];
	out[2] = tmp[2];
}

void Camera::projectOrthor(float *out, const float *obj) {
	gm::vec4 tmp(obj[0], obj[1], obj[2], 1);
	tmp = (view * mModel) * tmp;
	tmp = orthor * tmp;
	tmp /= tmp[3];

	//Should use memcpy
	tmp = tmp * 0.5f + 0.5f;
	out[0] = tmp[0] * viewport[2] + viewport[0];
	out[1] = tmp[1] * viewport[3] + viewport[1];
	out[2] = tmp[2];
}

void Camera::projectOrthor(float *out, float objX, float objY, float objZ) {
	float result;
	gm::vec4 tmp(objX, objY, objZ, 1);
	tmp = (view * mModel) * tmp;
	tmp = orthor * tmp;
	tmp /= tmp[3]; //tmp.w

	tmp = tmp * 0.5f + 0.5f;
	out[0] = tmp[0] * viewport[2] + viewport[0];
	out[1] = tmp[1] * viewport[3] + viewport[1];
	out[2] = tmp[2];
}

/**
 * on testing method
 */
void Camera::moveAlongForward(float d) {
	gm::vec3 f = gm::normalize(centerX - eyeX, centerY-eyeY, centerZ-eyeZ);
	gm::vec3 translate = f*d - f;

	eyeX += translate[0];
	eyeY += translate[1];
	eyeZ += translate[2];

	centerX += translate[0];
	centerY += translate[1];
	centerZ += translate[2];

	gm::vec3 eye(eyeX, eyeY, eyeZ);
	gm::vec3 s = gm::normalize(gm::cross(f, up));
	gm::vec3 u = gm::cross(s, f);

	view[0][0] = s[0]; //s.x
	view[1][0] = s[1]; //s.y
	view[2][0] = s[2]; //s.z
	view[0][1] = u[0];
	view[1][1] = u[1];
	view[2][1] = u[2];
	view[0][2] =-f[0];
	view[1][2] =-f[1];
	view[2][2] =-f[2];
	view[3][0] =-gm::dot(s, eye);
	view[3][1] =-gm::dot(u, eye);
	view[3][2] = gm::dot(f, eye);

}
