#ifndef _FMAT_H
#define _FMAT_H

#include <stdlib.h>
#include <cstddef>
#include <iostream>
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
		T **data;
		int row;
		int column;
		
	public:
		FMat() {
			data = NULL;
			row = column = 0;
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
            data = new T*[m.row];
            for(int i=0; i<m.row; i++) {
                data[i] = new T[m.column];
				memcpy(data[i], m.data[i], m.column * sizeof(T));
            }
			row = m.row;
			column = m.column;
		}
        
        FMat(int r, int col) {
			data = new T*[r];
            for(int i=0; i<r; i++) {
                data[i] = new T[col];
            }
			row = r;
			column = col;
		}
		
		FMat(T *value, int r, int col) {
			data = new T*[r];
            for(int i=0; i<r; i++) {
                data[i] = new T[col];//(value+(i*col), col);
				memcpy(data[i], value + i*col, col * sizeof(T));
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
			if(r == row && column == col) {
				return;
			}
			
			if(data != NULL) {
				for(int i=0; i<row; i++) {
					delete data[i];
				}
				delete data;
			}
			
			data = new T*[r];
            for(int i=0; i<r; i++) {
                data[i] = new T[col];
            }
			row = r;
			column = col;
		}
		
		void setValueToColumn(int col, T value) {
            for(int i=0; i<row; i++) {
                data[i][col] = value;
            }
		}
		
		void setValues(const T *d) {
            if(data == NULL) {
				throw "matrix's data is NULL";
			}
			
			for(int i=0; i<row; i++) {
				memcpy(data[i], d + (i*column), column * sizeof(T));
			}
        }
		
		void setAt(int r, int col, T d) {
            if( (data == NULL) || (r<0 || r>=row) || (col <0 || col >= column) ) {
				throw "matrix's data is NULL OR index is out of bound!";
			}
			
			data[r][col] = d;
        }
		
		int getRow(){ return row; }
		
		int getColumn() { return column; };
		
		ostream& print(ostream &o) {
			int i, j;
            cout << "[" << row << "x" << column << "]" << endl;
			for(i=0; i<row; i++) {
				for(j=0; j<column; j++) {
					o << data[i][j] << " ";
				}
				o << endl;
			}
			
			return o;
		}
		
		friend ostream& operator <<(ostream &o, const FMat &m) {
			int i, j;
			for(i=0; i<m.row; i++) {
				for(j=0; j<m.column; j++) {
					o << m.data[i][j] << " \t";
				}
				o << "\n";
			}
			
			return o;
		}
		
		/*
		void mulToTranspose(const FMat<T> &m2, FMat<T> &result) {
			int i, j;
			result.init(column, row);
			for(i=0; i<column; i++) {
				for(j=0; j<row; j++) {
					//(*(result.operator[](i)))[j] = data[j]->operator[](i);
					result[].setAt(i, j, data[i][j]);
				}
			}
		}
		*/

		void setIdentity() {
			int i, j;
			for(i=0; i<row; i++) {
				for(j=0; j<column; j++) {
                    data[i][j] = (T)0;
					if(i==j) {
                        data[i][j] = (T)1;
					}
				}
			}
		}
		
		FMat<T> transpose() {
			int i, j;
			FMat<T> t(column, row);
			for(i=0; i<column; i++) {
				for(j=0; j<row; j++) {
					//t.operator[](i).setAt(j, this->data[j]->operator[](i));
					t[i][j] = data[j][i][i];
				}
			}
			return t;
		}
        
        T* getRow(int index) const {
            return data[index];
        }

		T* operator [](int index) { 
			return data[index];
		}
		
		FMat<T>& operator +=(const FMat<T> &m) {
			int i, j;
			if(this->row != m.row || this->column != m.colum) {
				throw "Indexes not match!";
			}
			double t;
			for(i=0; i<row; i++) {
				for(j=0; j<column; j++) {
					data[i][j] = data[i][j] + m[i][j];
				}
			}
			
			return *this;
		}

		FMat<T>& operator =(const FMat<T> &m1) {
			//memcpy could be faster
			//cout << "Copy operator matrix" << endl;
			int i;
			row = m1.row;
			column = m1.column;
			if(data == NULL) {
				data = new T*[m1.row];
				for(i=0; i<row; i++) {
					data[i] = new T[column];
				}
			}
			
			for(i=0; i<row; i++) {
				//data[i]->copyFrom(m1.data[i]);
				memcpy(data[i], m1[i], column * sizeof(T));
			}
			return *this;
		}

		FMat<T> operator *(const FMat<T> &m2) {
			if(column != m2.row) {
				throw "Vec * FMat: Operands are not match!";
			}
			
			FMat<T> result(row, m2.column);
			int r, c, i, j;
			int newSize = row * m2.column;
			T s;
			
			i = 0;
			for(i=0; i<row; i++) {
				for(j=0; j<m2.column; j++) {
					s = (T)0;
					for(r=0; r<row; r++) {
						for(c=0; c<m2.column; c++) {
							s += data[r][c] * m2[c][j];
						}
					}
					result[i][j] = s;				
				}
			}
			
			return result;
		}
		
		FMat<T> mulToTranspose(const FMat<T> &m2) {			
			int i, j;
			int r, c;
			if(column != m2.column) {
				throw "FMat * transpose(FMat): Operands are not match in dimensions!";
			}
			
			FMat<T> result(row, m2.row);
			T s;
			for(i=0; i<row; i++) {
				for(j=0; j<m2.row; j++) {
					s = (T)0;
					for(r=0; r<row; r++) {
						for(c=0; c<column; c++) {
							s += data[r][c] * m2[j][c];
						}
						result[i][j] = s;
					}
				}
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
				result.setAt(i, s);
			}
			return result;
		}
	};
}

#endif
