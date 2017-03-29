#ifndef _QUAT_HPP
#define _QUAT_HPP

#include <math.h>
#include "vec3.hpp"

namespace gm {
	template <typename T>
	struct Quat {
		T x;
		T y;
		T z;
		T w;
	public:
		Quat<T>() {
			x = 0;
			y = 0;
			z = 0;
			w = 0;
		}

		Quat<T>(T _x, T _y, T _z, T _w) {
			x = _x;
			y = _y;
			z = _z;
			w = _w;
		}

		Quat<T>(T w, Vec3<T> const &v) {
			x = v.data[0];
			y = v.data[1];
			z = v.data[2];
			w = w;
		}

		Quat<T>& operator =(Quat<T> const &q) {
			this->x = q.x;
			this->y = q.y;
			this->z = q.z;
			this->w = q.w;

			return *this;
		}
	};

	template <typename T>
	Quat<T> operator *(Quat<T> const &p, Quat<T> const &q) {
		T w = p.w * q.w - p.x * q.x - p.y * q.y - p.z * q.z;
		T x = p.w * q.x + p.x * q.w + p.y * q.z - p.z * q.y;
		T y = p.w * q.y + p.y * q.w + p.z * q.x - p.x * q.z;
		T z = p.w * q.z + p.z * q.w + p.x * q.y - p.y * q.x;
		return Quat<T>(x, y, z, w);
	}

	template <typename T>
	FMat4<T> toMat4(Quat<T> const &q) {

		FMat4<T> mr(1);

		T qxx(q.x * q.x);
		T qyy(q.y * q.y);
		T qzz(q.z * q.z);
		T qxz(q.x * q.z);
		T qxy(q.x * q.y);
		T qyz(q.y * q.z);
		T qwx(q.w * q.x);
		T qwy(q.w * q.y);
		T qwz(q.w * q.z);

		mr[0][0] = 1 - 2 * (qyy +  qzz);
		mr[0][1] = 2 * (qxy + qwz);
		mr[0][2] = 2 * (qxz - qwy);
		mr[0][3] = 0;

		mr[1][0] = 2 * (qxy - qwz);
		mr[1][1] = 1 - 2 * (qxx +  qzz);
		mr[1][2] = 2 * (qyz + qwx);
		mr[1][3] = 0;

		mr[2][0] = 2 * (qxz + qwy);
		mr[2][1] = 2 * (qyz - qwx);
		mr[2][2] = 1 - 2 * (qxx +  qyy);
		mr[2][3] = 0;

		//this->value[0] = col_type(m[0], value_type(0));
		//this->value[1] = col_type(m[1], value_type(0));
		//this->value[2] = col_type(m[2], value_type(0));
		mr[3] = Vec4<T>(0, 0, 0, 1);

		return mr;
	}

	template <typename T>
	Quat<T> angleAxis(T const & a, Vec3<T> const & v ) {
		Quat<T> result;
		T s = sin(a * 0.5f);
		result.w = cos(a * 0.5f);
		result.x = v.data[0] * s;
		result.y = v.data[1] * s;
		result.z = v.data[2] * s;
		return result;
	}
}

#endif