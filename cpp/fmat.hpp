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
		
		FMat(const FMat<T> &m) {
			//cout << "Copying constructor" << endl;
            data = new Vec<T>*[m.row];
            for(int i=0; i<m.row; i++) {
                data[i] = new Vec<T>(m.getVec(i));
            }
			row = m.row;
			column = m.column;
		}
        
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
			//cout << "FMat-Destructor: started - data address:" << (void*)data << endl;
			if(data != NULL) {
				for(int i=0; i<row; i++) {
					delete data[i];
				}
				//cout << "FMat-Destructor: end " << endl;
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
		
		void setValues(const T *d) {
            if(data == NULL) {
				throw "matrix's data is NULL";
			}
			
			for(int i=0; i<row; i++) {
				data[i]->setValues(d + (i*column), column, 0);
			}
        }
		
		ostream& print(ostream &o) {
			int i, j;
            cout << "[" << row << "x" << column << "]" << endl;
			for(i=0; i<row; i++) {
				o << *(data[i]) << " ";
			}
			
			return o;
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
		
		FMat<T> transpose() {
			int i, j;
			FMat<T> t(column, row);
			for(i=0; i<column; i++) {
				for(j=0; j<row; j++) {
					t.operator[](i).setAt(this->data[j]->operator[](i), j);
					//(*(t.operator[](i)))[j] = data[j]->operator[](i);
				}
			}
			return t;
		}
        
        Vec<T>& getVec(int index) const {
            return *(data[index]);
        }

		Vec<T>& operator [](int index) { 
			return *(data[index]);
		}

		FMat<T>& operator =(const FMat<T> &m1) {
			//memcpy could be faster
			//cout << "Copy operator matrix" << endl;
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
			
			if(l != m2.row) {
				throw "Vec * FMat: Operands are not match!";
			}
			
			Vec<T> result(col);
			T s;
			int i, j;
			
			for(i=0; i<col; i++) {
				s = (T)0;
				for(j=0; j<l; j++) {
					s += v[j] * m2.data[j]->operator[](i);
				}
				result.setAt(s, i);
			}
			
			return result;
		}
		
		friend Vec<T> mulToTranspose(const Vec<T> &v, const FMat<T> &m2) {
			int row = m2.row;
			int l = v.size();
			int i, j;
			if(l != m2.column) {
				throw "Vec * FMat: Operands are not match!";
			}
			Vec<T> result(row);
			T s;
			for(i=0; i<row; i++) {
				s = (T)0;
				for(j=0; j<l; j++) {
					s += v[j] * m2.data[i]->operator[](j);
				}
				result.setAt(s, i);
			}
			return result;
		}
		
		friend Vec<T> mulTransposeTo(const FMat<T> &m2, const Vec<T> &v) {
			int row = m2.row;
			int l = v.size();
			int i, j;
			if(l != m2.row) {
				throw "Vec * FMat: Operands are not match!";
			}
			Vec<T> result(m2.column);
			T s;
			for(i=0; i<m2.column; i++) {
				s = (T)0;
				for(j=0; j<l; j++) {
					s += v[j] * m2.data[j]->operator[](i);
				}
				result.setAt(s, i);
			}
			return result;
		}
	};
}

#endif
