#ifndef _VEC3_H
#define _VEC3_H

#include <stdlib.h>

namespace gm {

	template <typename T>
	struct Vec3{
		T data[3];
	public:

		Vec3() {
			data[0] = 0;
			data[1] = 0;
			data[2] = 0;
		}

		Vec3(T v0, T v1, T v2) {
			data[0] = v0;
			data[1] = v1;
			data[2] = v2;
		}

		T& operator [](int index)	{ return data[index]; }

		Vec3<T>& operator =(Vec3<T> const &v) {
			//memcpy could be faster
			memcpy(&data, &v.data, 3 * sizeof(T));
			return *this;
		}

		Vec3<T> operator +(Vec3<T> &v2) {
			return Vec3<T>(data[0] + v2[0],
				data[1] + v2[1],
				data[2] + v2[2]);
		}

		Vec3<T> operator -(Vec3<T> &v2) {
			return Vec3<T>(data[0] - v2[0],
				data[1] - v2[1],
				data[2] - v2[2]);
		}

		Vec3<T> operator +(T a) {
			return Vec3<T>(data[0] + a, data[1] + a, data[2] + a);
		}

		Vec3<T>& operator /=(T a) {
			data[0] /= a;
			data[1] /= a;
			data[2] /= a;

			return *this;
		}
	};

	template <typename T>
	Vec3<T> operator *(Vec3<T> const &v, T a) {
		return Vec3<T>(v.data[0] * a, v.data[1] * a, v.data[2] * a);
	}

	template <typename T>
	Vec3<T> operator *(T a, Vec3<T> const &v) {
		return Vec3<T>(v.data[0] * a, v.data[1] * a, v.data[2] * a);
	}

	template <typename T>
	T dot(Vec3<T> const &v1, Vec3<T> const &v2) {
		return (v1.data[0]*v2.data[0] + v1.data[1]*v2.data[1] + v1.data[2]*v2.data[2]);
	}

	template <typename T>
	Vec3<T> cross(Vec3<T> const &v1, Vec3<T> const &v2) {
		return Vec3<T>(v1.data[1]*v2.data[2]-v1.data[2]*v2.data[1],
					v1.data[2]*v2.data[0]-v1.data[0]*v2.data[2],
					v1.data[0]*v2.data[1]-v1.data[1]*v2.data[0]);
	}
}
#endif