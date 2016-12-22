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
		
		FMat(T *value, int r, int col) {
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
		
		void init(int r, int col) {
			data = new Vec<T>*[r];
            for(int i=0; i<r; i++) {
                data[i] = new Vec<T>(col);
            }
			row = r;
			column = col;
		}
		
		void setValueToColumn(int col, T value) {
            for(int i=0; i<row; i++) {
                (*(data[i]))[0] = value;
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
		
		void mulToTranspose(const FMat<T> &m2, FMat<T> &result) {
			int i, j;
			result.init(column, row);
			for(i=0; i<column; i++) {
				for(j=0; j<row; j++) {
					(*(result.operator[](i)))[j] = data[j]->operator[](i);
				}
			}
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
		
		FMat<T>* transpose() {
			int i, j;
			FMat<T> *t = new FMat<T>(column, row);
			for(i=0; i<column; i++) {
				for(j=0; j<row; j++) {
					(*(t->operator[](i)))[j] = data[j]->operator[](i);
				}
			}
			return t;
		}

		Vec<T>& operator [](int index) { 
			return *(data[index]);
		}

		FMat<T>& operator =(const FMat<T> &m1) {
			//memcpy could be faster
			int i;
			row = m1.row;
			column = m1.column;
			if(data == NULL) {
				data = new Vec<T>*[m1.row];
				for(i=0; i<row; i++) {
					data[i] = new Vec<T>(column);
				}
			}
			
			for(i=0; i<row; i++) {
				data[i]->copyFrom(m1.data[i]);
			}
			return *this;
		}

		FMat<T> operator *(const FMat<T> &m2) {
			FMat<T> result;
			int r, c, i, j;
			for(r=0; r<row; r++) {
				for(c=0; c<m2.column; c++) {
				}
			}
			
			return result;
		}
		
		Vec<T> operator *(const Vec<T> &v) {
			int i, j, k;
			int size = v.size();
			T s;
			Vec<T> result(row);
			
			for(i=0; i<row; i++) {
				s = (T)0;
				for(j=0; j<size; j++) {
					s += data[i]->operator[](j) * v[j];
				}
				result.setAt(s, i);
			}
			
			return result;
		}
		
		friend Vec<T> operator*(const Vec<T> &v, const FMat<T> &m2) {
			int col = m2.column;
			int l = v.size();
			Vec<T> result(col);
			T s;
			int i, j, k;
			
			for(i=0; i<col; i++) {
				s = (T)0;
				for(j=0; j<l; j++) {
					s += v[j] * m2.data[i]->operator[](j);
				}
				result.setAt(s, i);
			}
			
			return result;
		}
	};
}

#endif
