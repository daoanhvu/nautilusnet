#ifndef _FMAT_H
#define _FMAT_H

#include <stdlib.h>
#include <cstddef>
#include <iostream>
#include "vec.hpp"
#ifdef _WIN32
#include <memory.h>
#endif

using namespace std;

namespace gm {
	template <typename T>
	struct FMat {
	private:
		//Column-major
		Vec<T> *data;
		int row;
		int column;

	public:
		FMat() {
			m = NULL;
		}

		FMat(int r, int col, T **value) {
            data = new Vec<T>[r];
            for(int i=0; i<r; i++) {
                data[i].set(value[i], col);
            }
			row = r;
			column = col;
		}
        
        FMat(int r, int col, T *value) {
			data = new Vec<T>[r];
            for(int i=0; i<r; i++) {
                data[i].set(value+(i*col), col);
            }
			row = r;
			column = col;
		}

		~FMat() {}
		
		friend ostream& operator <<(ostream &o, FMat &m) {
			int i, j;
			for(i=0; i<m.row; i++) {
				for(j=0; j<m.column; j++) {
					o << m.data[i][j] << " \t";
				}
				o << "\n";
			}
			
			return o;
		}

		void setIdentity() {
			int i, j;
			for(i=0; i<row; i++) {
				for(j=0; j<column; j++) {
                    data[i][j] = (T)0;
					if(i==j) 
                        data[i][j] = (T)1;
				}
			}
		}

		Vec<T>& operator [](int index)	{ 
			return data[index];
		}

		FMat<T>& operator =(FMat<T> const &m1) {
			//memcpy could be faster
			int i;
			for(i=0; i<row; i++) {
				memcpy(&data[i], &m1.data[i], column * sizeof(T));
			}
			return *this;
		}

		FMat<T>& operator *(const FMat<T> &m2) {
			FMat<T> *result = new FMat<T>();
			int r, c, i, j;
			for(r=0; r<row; r++) {
                
				for(c=0; c<m2.column; c++) {
				}
			}
			
			return result;
		}
	};
}

#endif
