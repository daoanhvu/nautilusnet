#ifndef _FMAT_H
#define _FMAT_H

#include <stdlib.h>
#include <cstddef>
#include <iostream>
#ifdef _WIN32
#include <memory.h>
#endif

using namespace std;

namespace gm {
	template <typename T>
	struct FMat {
	private:
		//Column-major
		T **m;
		int row;
		int column;

	public:
		FMat() {
			m = NULL;
		}

		FMat(int r, int col, T **value) {
			m = value;
			row = r;
			column = col;
		}

		~FMat() {}
		
		friend ostream& operator <<(ostream &o, FMat &m) {
			int i, j;
			for(i=0; i<m.row; i++) {
				for(j=0; j<m.column; j++) {
					o << m.m[i][j] << " \t";
				}
				o << "\n";
			}
			
			return o;
		}

		void setIdentity() {
			int i, j;
			for(i=0; i<row; i++) {
				for(j=0; j<column; j++) {
					if(i==j) m[i][j] = (T)0;
				}
			}
		}

		FMat<T>& operator [](int index)	{ 
			return ( new FMat<T>(&m[index]) );
		}
		
		T get(int idx1, int idx2)	{ 
			return m[idx1][idx2];
		}

		FMat<T>& operator =(FMat<T> const &m1) {
			//memcpy could be faster
			int i;
			for(i=0; i<row; i++) {
				memcpy(&m[i], &m1.m[i], column * sizeof(T));
			}
			return *this;
		}

		FMat<T> operator *(FMat<T> &m2) {
			FMat<T> result = new FMat<T>();
			int i, j;
			for(i=0; i<row; i++) {
				for(j=0; j<column; j++) {
				}
			}
			
			return result;
		}
	};
}

#endif
