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
		int length;
        
	public:

		Vec<T>() {
			data = NULL;
            length = 0;
		}
		
		Vec<T>(int s) {
            data = new T[s];
            length = s;
			//cout << "Vec-Constructor: data addressed at " << (void*)data << endl;
		}
        
        Vec<T>(const Vec<T> &v) {
			//cout << "Vec: Copy constructor has been called " << endl;
            data = new T[v.length];
            memcpy(data, v.data, v.length * sizeof(T));
            length = v.length;
        }

		Vec<T>(T *d, int s) {
            data = new T[s];
			memcpy(data, d, s * sizeof(T));
            length = s;
		}
        
        ~Vec() {
			//cout << "Vec-Destructor " << (void*)data << endl;
            if(data != NULL) {
                delete data;
				data = NULL;
			}
        }
		
		void copyFrom(const Vec<T> &v) {
			if((data != NULL) && (length != v.length)){
                delete data;
                data = new T[v.length];
            }
			if(data == NULL)
				data = new T[v.length];
            memcpy(data, v.data, v.length * sizeof(T));
            length = v.length;
		}
		
		void release() {
            if(data != NULL) {
                delete data;
				data = NULL;
				length = 0;
			}
        }
        
        void set(const T *d, int s) {
            if((data != NULL) && (length != s)){          
				//cout << "Realloc data array " << endl;
                delete data;
                data = new T[s];
            }
			
			if(data == NULL) {
				data = new T[s];
				//cout << "New data array at " << (void*)data << endl;
			}
			
            memcpy(data, d, s * sizeof(T));
			//cout << "Address data array after copying " << (void*)data << endl;
            length = s;
        }
		
		void setValues(const T *d, int s, int offs = 0) {
            if(data == NULL) {
				throw "vector's data is NULL";
			}
			
			if( length < (s + offs) ){
				//cout << "Realloc data array " << endl;
				throw "index out of bound.";
                //delete data;
                //data = new T[s];
            }
			
            memcpy(data + offs, d, s * sizeof(T));
			
			/*
			int c = 0;
			while(c<s) {
				data[offs+c] = d[c++];
			}
			*/
        }
		
		void setAt(T v, int pos) {
            data[pos] = v;
        }
		
		ostream& print(ostream &o) {
			o <<"Vector: Size = " << length << endl;
			for(int i=0; i<length; i++) {
                o << data[i] << " ";
            }
            o << endl;
			return o;
		}

		friend ostream& operator <<(ostream &o, const Vec<T> &v) {
			for(int i=0; i<v.length; i++) {
                o << v.data[i] << " ";
            }
            o << endl;
			return o;
		}
		
		int size() const {
			return length;
		}

		T operator [](int index) const	{ return data[index]; }

		Vec<T>& operator =(const Vec<T> &v) {
			//cout << "Operator assignment has been called!" << endl;
			//memcpy could be faster
            if(data != NULL) {
                if(length != v.length) {
                    delete data;
                    data = new T[v.length];
                }
            } else {
				data = new T[v.length];
			}
			
			memcpy(data, v.data, v.length * sizeof(T));
            length = v.length;
			return *this;
		}
        
        void plus(const Vec<T> &v) {
            for(int i=0; i<length; i++) {
                data[i] += v.data[i];
            }
        }
        
        void plus(T v) {
            for(int i=0; i<length; i++) {
                data[i] += v;
            }
        }
        
        void mul(T v) {
            for(int i=0; i<length; i++) {
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
			for(int i=0; i<length; i++) {
				data[i] /= a;
			}
			return *this;
		}
	};
}

#endif