#ifndef _VEC4_HPP
#define _VEC4_HPP

#include <math.h>
#include "vec3.hpp"
#include <iostream>

using namespace std;
namespace gm {
	template <typename T>
	struct Vec {
		T *data;
		int size;
	public:

		Vec() {
			memset(&data, (T)0, 4 * sizeof(T));
		}

		Vec(int s, T *d) {
			
		}

		friend ostream& operator <<(ostream &o, Vec4 &v) {
			o << "(" <<v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ")";
			return o;
		}

		T& operator [](int index)	{ return data[index]; }

		Vec4<T>& operator =(Vec4<T> const &m) {
			//memcpy could be faster
			memcpy(&data, &m.data, 4 * sizeof(T));
			return *this;
		}

		Vec4<T> operator +(Vec4<T> &v2) {
			return Vec4<T>(data[0] + v2[0],
				data[1] + v2[1],
				data[2] + v2[2],
				data[3] + v2[3]);
		}

		Vec4<T> operator +(T a) {
			return Vec4<T>(data[0] + a, data[1] + a, data[2] + a, data[3] + a);
		}

		Vec4<T> operator *(T a) {
			return Vec4<T>(data[0] * a, data[1] * a, data[2] * a, data[3] * a);
		}

		Vec4<T>& operator /=(T a) {
			data[0] /= a;
			data[1] /= a;
			data[2] /= a;
			data[3] /= a;

			return *this;
		}
	};
	
	template <typename T>
	Vec4<T> operator +(Vec4<T> const &v1, Vec4<T> const &v2) {
		return Vec4<T>(v1.data[0] + v2.data[0], v1.data[1] + v2.data[1], v1.data[2] + v2.data[2], v1.data[3] + v2.data[3]);
	}

	template <typename T>
	Vec4<T> operator *(Vec4<T> const v, T a) {
		return Vec4<T>(v.data[0] * a, v.data[1] * a, v.data[2] * a, v.data[3] * a);
	}

	template <typename T>
	Vec4<T> operator *(T a, Vec3<T> const v) {
		return Vec4<T>(v.data[0] * a, v.data[1] * a, v.data[2] * a, v.data[3] * a);
	}
}

#endif