#ifndef _VEC4_HPP
#define _VEC4_HPP

#include <math.h>
#include "vec3.hpp"
#include <cstddef>
#include <iostream>

using namespace std;
namespace gm {
	template <typename T>
	struct Vec {
		T *data;
		int size;
        
	public:

		Vec<T>() {
			data = NULL;
            size = 0;
		}
        
        Vec<T>(const Vec<T> &v) {
            data = new T[v.size];
            memcpy(&data, &v.data, v.size * sizeof(T));
            size = v.size;
        }

		Vec<T>(int s, T *d) {
            data = new T[s];
			memcpy(&data, &d, s * sizeof(T));
            size = s;
		}
        
        ~Vec() {
            if(data != NULL)
                delete data;
        }
        
        void set(T *d, int s) {
            if((data != null) && (size != s)){                
                delete data;
                data = new T[s];
            }
            memcpy(&data, &d, s * sizeof(T));
            size = s;
        }

		friend ostream& operator <<(ostream &o, Vec<T> &v) {
			for(int i=0; i<size; i++) {
                o << data[i] << " ";
            }
            o << endl;
			return o;
		}

		T& operator [](int index)	{ return data[index]; }

		Vec<T>& operator =(Vec<T> const &v) {
			//memcpy could be faster
            if(data != null) {
                if(size != v.size) {
                    delete data;
                    data = new T[v.size];
                }
            }
            
			memcpy(&data, &v.data, v.size * sizeof(T));
            size = v.size;
			return *this;
		}
        
        void plus(const Vec<T> &v) {
            for(int i=0; i<size; i++) {
                data[i] += v.data[i];
            }
        }
        
        void plus(T v) {
            for(int i=0; i<size; i++) {
                data[i] += v;
            }
        }
        
        void mul(T v) {
            for(int i=0; i<size; i++) {
                data[i] *= v;
            }
        }

		Vec<T> operator +(const Vec<T> &v2) {
            Vec<T> r(this);
            
            r.plus(v2);
            
			return r;
		}

        //??????????????
		Vec<T> operator +(T a) {
			Vec<T> r(this);
            r.plus(a);
			return r;
		}

		Vec<T> operator *(T a) {
			Vec<T> r(this);
            r.mul(a);
			return r;
		}

		Vec<T>& operator /=(T a) {
			data[0] /= a;
			data[1] /= a;
			data[2] /= a;
			data[3] /= a;

			return *this;
		}
	};
	
	template <typename T>
	Vec<T> operator +(Vec<T> const &v1, Vec<T> const &v2) {
		Vec<T> r(v1);
        r.plus(v2);
        return r;
	}

	//template <typename T>
	//Vec<T> operator *(Vec4<T> const v, T a) {
	//	return Vec4<T>(v.data[0] * a, v.data[1] * a, v.data[2] * a, v.data[3] * a);
	//}

	template <typename T>
	Vec<T> operator *(T a, Vec3<T> const v) {
		Vec<T> r(this);
        r.mul(a);
		return r;
	}
}

#endif