#ifndef _VEC4_HPP
#define _VEC4_HPP

#include <math.h>
#include <cstddef>
#include <iostream>
#include <cstring>

using namespace std;
namespace gm {
	template <typename T>
	struct Vec {
	private:
		T *data;
		int size;
        
	public:

		Vec<T>() {
			data = NULL;
            size = 0;
		}
		
		Vec<T>(int s) {
            data = new T[s];
            size = s;
		}
        
        Vec<T>(const Vec<T> &v) {
            data = new T[v.size];
            memcpy(data, v.data, v.size * sizeof(T));
            size = v.size;
        }

		Vec<T>(T *d, int s) {
            data = new T[s];
			memcpy(data, d, s * sizeof(T));
            size = s;
		}
        
        ~Vec() {
			cout << "Vec-Destructor " << (void*)data << endl;
            if(data != NULL) {
                delete data;
				data = NULL;
			}
        }
		
		void copyFrom(const Vec<T> &v) {
			if((data != NULL) && (size != v.size)){
                delete data;
                data = new T[v.size];
            }
			if(data == NULL)
				data = new T[v.size];
            memcpy(data, v.data, v.size * sizeof(T));
            size = v.size;
		}
		
		void release() {
            if(data != NULL) {
                delete data;
				data = NULL;
				size = 0;
			}
        }
        
        void set(T *d, int s) {
            if((data != NULL) && (size != s)){          
				cout << "Realloc data array " << endl;
                delete data;
                data = new T[s];
            }
			
			if(data == NULL) {
				data = new T[s];
				cout << "New data array at " << (void*)data << endl;
			}
			
            memcpy(data, d, s * sizeof(T));
			cout << "Address data array after copying " << (void*)data << endl;
            size = s;
        }

		friend ostream& operator <<(ostream &o, const Vec<T> &v) {
			for(int i=0; i<v.size; i++) {
                o << v.data[i] << " ";
            }
            o << endl;
			return o;
		}

		T operator [](int index)	{ return data[index]; }

		Vec<T>& operator =(const Vec<T> &v) {
			//memcpy could be faster
            if(data != NULL) {
                if(size != v.size) {
                    delete data;
                    data = new T[v.size];
                }
            }
            
			memcpy(data, v.data, v.size * sizeof(T));
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

		friend Vec<T> operator +(Vec<T> const &v1, Vec<T> const &v2) {
			Vec<T> r(v1);
			r.plus(v2);
			return r;
		}

        //??????????????
		Vec<T> operator +(T a) {
			Vec<T> r(this);
            r.plus(a);
			return r;
		}

		friend Vec<T> operator *(const Vec<T> &v, T a) {
			Vec<T> r(v);
            r.mul(a);
			return r;
		}
		
		friend Vec<T> operator *(T a, const Vec<T> &v) {
			Vec<T> r(v);
			r.mul(a);
			return r;
		}
	
		//Vec<T> operator *(T a) {
		//	Vec<T> r(this);
        //  r.mul(a);
		//	return r;
		//}

		Vec<T>& operator /=(T a) {
			for(int i=0; i<size; i++) {
				data[i] /= a;
			}
			return *this;
		}
	};
}

#endif