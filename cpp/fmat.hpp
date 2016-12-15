#ifndef _FMAT_H
#define _FMAT_H

#include <stdlib.h>
#include <cstddef>
#include <iostream>
#include "vec.hpp"
#ifdef _WIN32
#include <memory.h>
#else
#include <cstring>
#endif

using namespace std;

namespace gm {
	template <typename T>
	struct FMat {
	private:
		Vec<T> **data;
		int row;
		int column;
		
		int logRow;

	public:
		FMat() {
			data = NULL;
			logRow = row = column = 0;
		}

		/*
		FMat(int r, int col, T **value) {
            data = new Vec<T>[r];
            for(int i=0; i<r; i++) {
                data[i].set(value[i], col);
            }
			row = r;
			column = col;
		}
		*/
        
        FMat(int r, int col) {
			data = new Vec<T>*[r];
            for(int i=0; i<r; i++) {
                data[i] = new Vec<T>(col);
            }
			row = r;
			column = col;
		}
		
		FMat(int r, int col, T *value) {
			data = new Vec<T>*[r];
            for(int i=0; i<r; i++) {
                data[i] = new Vec<T>(value+(i*col), col);
            }
			row = r;
			column = col;
		}

		~FMat() {
			if(data != NULL) {
				for(int i=0; i<row; i++) {
					delete data[i];
				}
				//cout << "FMat-Destructor: data address:" << (void*)data << endl;
				delete data;
			}
		}
		
		friend ostream& operator <<(ostream &o, const FMat &m) {
			int i, j;
			for(i=0; i<m.row; i++) {
				for(j=0; j<m.column; j++) {
					o << (*(m.data[i]))[j] << " \t";
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
                        (*(data[i]))[j] = (T)1;
				}
			}
		}

		Vec<T>& operator [](int index)	{ 
			return *(data[index]);
		}

		FMat<T>& operator =(const FMat<T> &m1) {
			//memcpy could be faster
			int i;
			for(i=0; i<row; i++) {
				data[i]->copyFrom(m1.data[i]);
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
