#ifndef _FMAT4_H
#define _FMAT4_H

#include <stdlib.h>
#include <iostream>
#ifdef _WIN32
	#include <memory.h>
#else
	#include <string.h>
#endif
#include "vec4.hpp"

using namespace std;

namespace gm {
	template <typename T>
	struct FMat4 {
	private:
		//Column-major
		Vec4<T> data[4];

	public:
		FMat4() {
			data[0][0] = 1;
			data[0][1] = 0;
			data[0][2] = 0;
			data[0][3] = 0;

			data[1][0] = 0;
			data[1][1] = 1;
			data[1][2] = 0;
			data[1][3] = 0;

			data[2][0] = 0;
			data[2][1] = 0;
			data[2][2] = 1;
			data[2][3] = 0;

			data[3][0] = 0;
			data[3][1] = 0;
			data[3][2] = 0;
			data[3][3] = 1;
		}

		FMat4(T value) {
			int i, j;

			//could use memset
			for(i=0; i<4; i++)
				for(j=0; j<4; j++)
					data[i][j] = value;
		}

		~FMat4() {}
		
		friend ostream& operator <<(ostream &o, FMat4 &m) {
			int i;
			for(i=0; i<4; i++) {
				//o << m[i][0] << " \t" << m[i][1] << " \t" << m[i][2] << " \t" << m[i][3] << "\n";
				o << m[0][i] << " \t" << m[1][i] << " \t" << m[2][i] << " \t" << m[3][i] << "\n";
			}
			
			return o;
		}

		void setIdentity() {
			//column 0
			data[0][0] = 1;
			data[0][1] = 0;
			data[0][2] = 0;
			data[0][3] = 0;

			//column 1
			data[1][0] = 0;
			data[1][1] = 1;
			data[1][2] = 0;
			data[1][3] = 0;

			//column 2
			data[2][0] = 0;
			data[2][1] = 0;
			data[2][2] = 1;
			data[2][3] = 0;

			//column 3
			data[3][0] = 0;
			data[3][1] = 0;
			data[3][2] = 0;
			data[3][3] = 1;
		}

		Vec4<T>& operator [](int index)	{ return data[index]; }

		FMat4<T>& operator =(FMat4<T> const &m) {
			//memcpy could be faster
			memcpy(&data, &m.data, 16 * sizeof(T));
			//data[0] = m[0];
			//data[1] = m[1];
			//data[2] = m[2];
			//data[3] = m[3];
			return *this;
		}

		FMat4<T> operator *(FMat4<T> &m2) {
			FMat4<T> result;
			int i;
			
			result[0][0] = 0;
			result[1][1] = 0;
			result[2][2] = 0;
			result[3][3] = 0;
			for(i=0; i<4; i++) {
				result[0][0] += data[i][0] * m2[0][i];
				result[0][1] += data[i][1] * m2[0][i];
				result[0][2] += data[i][2] * m2[0][i];
				result[0][3] += data[i][3] * m2[0][i];
				
				result[1][0] += data[i][0] * m2[1][i];
				result[1][1] += data[i][1] * m2[1][i];
				result[1][2] += data[i][2] * m2[1][i];
				result[1][3] += data[i][3] * m2[1][i];
				
				result[2][0] += data[i][0] * m2[2][i];
				result[2][1] += data[i][1] * m2[2][i];
				result[2][2] += data[i][2] * m2[2][i];
				result[2][3] += data[i][3] * m2[2][i];
				
				result[3][0] += data[i][0] * m2[3][i];
				result[3][1] += data[i][1] * m2[3][i];
				result[3][2] += data[i][2] * m2[3][i];
				result[3][3] += data[i][3] * m2[3][i];
			}
			
			return result;
		}

		Vec4<T> operator *(Vec4<T> &v) {
			Vec4<T> result;
			int i, j;
			for(i=0; i<4; i++) {
				for(j=0; j<4; j++)
					result[i] += data[j][i] * v[j];
			}
			return result;
		}
		
		void operator /=(T divisor) {			
			int i, j;
			for(i=0; i<4; i++) {
				for(j=0; j<4; j++)
					data[j][i] = data[j][i] / divisor;
			}
		}
		
		/*
			m[0][0]	m[1][0]	m[2][0]	m[3][0]
			m[0][1]	m[1][1]	m[2][1]	m[3][1]
			m[0][2]	m[1][2]	m[2][2]	m[3][2]
			m[0][3]	m[1][3]	m[2][3]	m[3][3]
		*/
		T det() {
			T d = (T)0;
			
			/*
				m[1][1]	m[2][1]	m[3][1]
				m[1][2]	m[2][2]	m[3][2]
				m[1][3]	m[2][3]	m[3][3]
			*/
			T A00 = data[1][1]*data[2][2]*data[3][3] + data[2][1]*data[3][2]*data[1][3] + data[3][1]*data[1][2]*data[2][3] - 
					data[1][3]*data[2][2]*data[3][1] - data[2][3]*data[3][2]*data[1][1] - data[3][3]*data[1][2]*data[2][1];
			
			/*
				m[1][0]	m[2][0]	m[3][0]
				m[1][2]	m[2][2]	m[3][2]
				m[1][3]	m[2][3]	m[3][3]
			*/			
			T A01 = -(data[1][0]*data[2][2]*data[3][3] + data[2][0]*data[3][2]*data[1][3] + data[3][0]*data[1][2]*data[2][3] - 
					data[1][3]*data[2][2]*data[3][0] - data[2][3]*data[3][2]*data[1][0] - data[3][3]*data[1][2]*data[2][0]);
			
			/*
				m[1][0]	m[2][0]	m[3][0]
				m[1][1]	m[2][1]	m[3][1]
				m[1][3]	m[2][3]	m[3][3]
			*/			
			T A02 = data[1][0]*data[2][1]*data[3][3] + data[2][0]*data[3][1]*data[1][3] + data[3][0]*data[1][1]*data[2][3] - 
					data[1][3]*data[2][1]*data[3][0] - data[2][3]*data[3][1]*data[1][0] - data[3][3]*data[1][1]*data[2][0];
			
			/*
				m[1][0]	m[2][0]	m[3][0]
				m[1][1]	m[2][1]	m[3][1]
				m[1][2]	m[2][2]	m[3][2]
			*/			
			T A03 = -(data[1][0]*data[2][1]*data[3][2] + data[2][0]*data[3][1]*data[1][2] + data[3][0]*data[1][1]*data[2][2] - 
					data[1][2]*data[2][1]*data[3][0] - data[2][2]*data[3][1]*data[1][0] - data[3][2]*data[1][1]*data[2][0]);
					
			d = data[0][0] * A00 + data[0][1] * A01 + data[0][2] * A02 + data[0][3] * A03;
			return d;
		}
		
		/*
			m[0][0]	m[1][0]	m[2][0]	m[3][0]
			m[0][1]	m[1][1]	m[2][1]	m[3][1]
			m[0][2]	m[1][2]	m[2][2]	m[3][2]
			m[0][3]	m[1][3]	m[2][3]	m[3][3]
		*/
		bool inverse(FMat4<T> &invOut) {
			FMat4<T> inv;
			T det;
			int i;
			//T factor2233 = data[2][2] * data[3][3];
			//T factor1123 = data[1][1] * data[2][3];

			/*
				m[1][1]	m[2][1]	m[3][1]
				m[1][2]	m[2][2]	m[3][2]
				m[1][3]	m[2][3]	m[3][3]
			*/
			inv[0][0] = data[1][1] * data[2][2] * data[3][3] + data[2][1] * data[3][2] * data[1][3] + data[3][1] * data[1][2] * data[2][3]
						- data[1][3] * data[2][2] * data[3][1] - data[2][3] * data[3][2] * data[1][1] - data[3][3] * data[1][2] * data[2][1];

			/*
				m[0][1]	m[2][1]	m[3][1]
				m[0][2]	m[2][2]	m[3][2]
				m[0][3]	m[2][3]	m[3][3]
			*/
			inv[1][0] = -(data[0][1] * data[2][2] * data[3][3] + data[2][1] * data[3][2] * data[0][3] + data[3][1] * data[0][2] * data[2][3] 
						- data[0][3] * data[2][2] * data[3][1] - data[2][3] * data[3][2] * data[0][1] - data[3][3] * data[0][2] * data[2][1]);

			/*
				m[0][1]	m[1][1]	m[3][1]
				m[0][2]	m[1][2]	m[3][2]
				m[0][3]	m[1][3]	m[3][3]
			*/
			inv[2][0] = data[0][1] * data[1][2] * data[3][3] + data[1][1] * data[3][2] * data[0][3] + data[3][1] * data[0][2] * data[1][3] 
						- data[0][3] * data[1][2] * data[3][1] - data[1][3] * data[3][2] * data[0][1] - data[3][3] * data[0][2] * data[1][1];

			/*
				m[0][1]	m[1][1]	m[2][1]
				m[0][2]	m[1][2]	m[2][2]
				m[0][3]	m[1][3]	m[2][3]
			*/
			inv[3][0] = -(data[0][1] * data[1][2] * data[2][3] + data[1][1] * data[2][2] * data[0][3] + data[2][1] * data[0][2] * data[1][3] 
						- data[0][3] * data[1][2] * data[2][1] - data[1][3] * data[2][2] * data[0][1] - data[2][3] * data[0][2] * data[1][1]);

			/*
				m[1][0]	m[2][0]	m[3][0]
				m[1][2]	m[2][2]	m[3][2]
				m[1][3]	m[2][3]	m[3][3]
			*/
			inv[0][1] = -(data[1][0] * data[2][2] * data[3][3] + data[2][0] * data[3][2] * data[1][3] + data[3][0] * data[1][2] * data[2][3] 
						- data[1][3] * data[2][2] * data[3][0] - data[2][3] * data[3][2] * data[1][0] - data[3][3] * data[1][2] * data[2][0]);

			/*
				m[0][0]	m[2][0]	m[3][0]
				m[0][2]	m[2][2]	m[3][2]
				m[0][3]	m[2][3]	m[3][3]
			*/
			inv[1][1] = data[0][0] * data[2][2] * data[3][3] + data[2][0] * data[3][2] * data[0][3] + data[3][0] * data[0][2] * data[2][3] 
						- data[0][3] * data[2][2] * data[3][0] - data[2][3] * data[3][2] * data[0][0] - data[3][3] * data[0][2] * data[2][0];

			/*
				m[0][0]	m[1][0]	m[3][0]
				m[0][2]	m[1][2]	m[3][2]
				m[0][3]	m[1][3]	m[3][3]
			*/
			inv[2][1] = -(data[0][0] * data[1][2] * data[3][3] + data[1][0] * data[3][2] * data[0][3] + data[3][0] * data[0][2] * data[1][3] 
						- data[0][3] * data[1][2] * data[3][0] - data[1][3] * data[3][2] * data[0][0] - data[3][3] * data[0][2] * data[1][0]);

			/*
				m[0][0]	m[1][0]	m[2][0]
				m[0][2]	m[1][2]	m[2][2]
				m[0][3]	m[1][3]	m[2][3]
			*/
			inv[3][1] = data[0][0] * data[1][2] * data[2][3] + data[1][0] * data[2][2] * data[0][3] + data[2][0] * data[0][2] * data[1][3] 
						- data[0][3] * data[1][2] * data[2][0] - data[1][3] * data[2][2] * data[0][0] - data[2][3] * data[0][2] * data[1][0];

			/*
				m[1][0]	m[2][0]	m[3][0]
				m[1][1]	m[2][1]	m[3][1]
				m[1][3]	m[2][3]	m[3][3]
			*/
			inv[0][2] = data[1][0]  * data[2][1] * data[3][3] + data[2][0]  * data[3][1] * data[1][3] + data[3][0]  * data[1][1] * data[2][3] 
						- data[1][3]  * data[2][1] * data[3][0] - data[2][3] * data[3][1] * data[1][0] - data[3][3] * data[1][1] * data[2][0];

			/*
				m[0][0]	m[2][0]	m[3][0]
				m[0][1]	m[2][1]	m[3][1]
				m[0][3]	m[2][3]	m[3][3]
			*/
			inv[1][2] = -(data[0][0] * data[2][1] * data[3][3] + data[2][0] * data[3][1] * data[0][3] + data[3][0] * data[0][1] * data[2][3] 
						- data[0][3] * data[2][1] * data[3][0] - data[2][3] * data[3][1] * data[0][0] - data[3][3] * data[0][1] * data[2][0]);

			/*
				m[0][0]	m[1][0]	m[3][0]
				m[0][1]	m[1][1]	m[3][1]
				m[0][3]	m[1][3]	m[3][3]
			*/
			inv[2][2] = data[0][0]  * data[1][1] * data[3][3] + data[1][0]  * data[3][1] * data[0][3] + data[3][0]  * data[0][1] * data[1][3] 
						- data[0][3]  * data[1][1] * data[3][0] - data[1][3] * data[3][1] * data[0][0] - data[3][3] * data[0][1] * data[1][0];

			/*
				m[0][0]	m[1][0]	m[2][0]
				m[0][1]	m[1][1]	m[2][1]
				m[0][3]	m[1][3]	m[2][3]
			*/
			inv[3][2] = -(data[0][0] * data[1][1] * data[2][3] + data[1][0] * data[2][1] * data[0][3] + data[2][0] * data[0][1] * data[1][3] 
						- data[0][3] * data[1][1] * data[2][0] - data[1][3] * data[2][1] * data[0][0] - data[2][3] * data[0][1] * data[1][0]);

			/*
				m[1][0]	m[2][0]	m[3][0]
				m[1][1]	m[2][1]	m[3][1]
				m[1][2]	m[2][2]	m[3][2]
			*/
			inv[0][3] = -(data[1][0] * data[2][1] * data[3][2] + data[2][0] * data[3][1] * data[1][2] + data[3][0] * data[1][1] * data[2][2] 
						- data[1][2] * data[2][1] * data[3][0] - data[2][2] * data[3][1] * data[1][0] - data[3][2] * data[1][1] * data[2][0]);

			/*
				m[0][0]	m[2][0]	m[3][0]
				m[0][1]	m[2][1]	m[3][1]
				m[0][2]	m[2][2]	m[3][2]
			*/
			inv[1][3] = data[0][0] * data[2][1] * data[3][2] + data[2][0] * data[3][1] * data[0][2] + data[3][0] * data[0][1] * data[2][2] 
						- data[0][2] * data[2][1] * data[3][0] - data[2][2] * data[3][1] * data[0][0] - data[3][2] * data[0][1] * data[2][0];

			/*
				m[0][0]	m[1][0]	m[3][0]
				m[0][1]	m[1][1]	m[3][1]
				m[0][2]	m[1][2]	m[3][2]
			*/
			inv[2][3] = -(data[0][0] * data[1][1] * data[3][2] + data[1][0] * data[3][1] * data[0][2] + data[3][0] * data[0][1] * data[1][2] 
						- data[0][2] * data[1][1] * data[3][0] - data[1][2] * data[3][1] * data[0][0] - data[3][2] * data[0][1] * data[1][0]);

			/*
				m[0][0]	m[1][0]	m[2][0]
				m[0][1]	m[1][1]	m[2][1]
				m[0][2]	m[1][2]	m[2][2]
			*/
			inv[3][3] = data[0][0] * data[1][1] * data[2][2] + data[1][0] * data[2][1] * data[0][2] + data[2][0] * data[0][1] * data[1][2] 
						- data[0][2] * data[1][1] * data[2][0] - data[1][2] * data[2][1] * data[0][0] - data[2][2] * data[0][1] * data[1][0];

			det = data[0][0] * inv[0][0] + data[0][1] * inv[0][1] + data[0][2] * inv[0][2] + data[0][3] * inv[0][3];

			if (det == 0)
				return false;

			det = 1.0f / det;

			for (i = 0; i < 4; i++) {
				invOut[i][0] = inv[i][0] * det;
				invOut[i][1] = inv[i][1] * det;
				invOut[i][2] = inv[i][2] * det;
				invOut[i][3] = inv[i][3] * det;
			}

			return true;
		}
		
		/*
			m[0][0]	m[1][0]	m[2][0]	m[3][0]
			m[0][1]	m[1][1]	m[2][1]	m[3][1]
			m[0][2]	m[1][2]	m[2][2]	m[3][2]
			m[0][3]	m[1][3]	m[2][3]	m[3][3]
			
			under testing
		*/
		bool inverse2(FMat4<T> &invOut) {
			FMat4<T> inv;
			T det;
			int i;
			//T factor2233 = data[2][2] * data[3][3];
			//T factor1123 = data[1][1] * data[2][3];

			/*
				m[1][1]	m[2][1]	m[3][1]
				m[1][2]	m[2][2]	m[3][2]
				m[1][3]	m[2][3]	m[3][3]
			*/
			inv[0][0] = data[1][1] * data[2][2] * data[3][3] + data[2][1] * data[3][2] * data[1][3] + data[3][1] * data[1][2] * data[2][3]
						- data[1][3] * data[2][2] * data[3][1] - data[2][3] * data[3][2] * data[1][1] - data[3][3] * data[1][2] * data[2][1];

			/*
				m[0][1]	m[2][1]	m[3][1]
				m[0][2]	m[2][2]	m[3][2]
				m[0][3]	m[2][3]	m[3][3]
			*/
			inv[1][0] = -(data[0][1] * data[2][2] * data[3][3] + data[2][1] * data[3][2] * data[0][3] + data[3][1] * data[0][2] * data[2][3] 
						- data[0][3] * data[2][2] * data[3][1] - data[2][3] * data[3][2] * data[0][1] - data[3][3] * data[0][2] * data[2][1]);

			/*
				m[0][1]	m[1][1]	m[3][1]
				m[0][2]	m[1][2]	m[3][2]
				m[0][3]	m[1][3]	m[3][3]
			*/
			inv[2][0] = data[0][1] * data[1][2] * data[3][3] + data[1][1] * data[3][2] * data[0][3] + data[3][1] * data[0][2] * data[1][3] 
						- data[0][3] * data[1][2] * data[3][1] - data[1][3] * data[3][2] * data[0][1] - data[3][3] * data[0][2] * data[1][1];

			/*
				m[0][1]	m[1][1]	m[2][1]
				m[0][2]	m[1][2]	m[2][2]
				m[0][3]	m[1][3]	m[2][3]
			*/
			inv[3][0] = -(data[0][1] * data[1][2] * data[2][3] + data[1][1] * data[2][2] * data[0][3] + data[2][1] * data[0][2] * data[1][3] 
						- data[0][3] * data[1][2] * data[2][1] - data[1][3] * data[2][2] * data[0][1] - data[2][3] * data[0][2] * data[1][1]);

			/*
				m[1][0]	m[2][0]	m[3][0]
				m[1][2]	m[2][2]	m[3][2]
				m[1][3]	m[2][3]	m[3][3]
			*/
			inv[0][1] = -(data[1][0] * data[2][2] * data[3][3] + data[2][0] * data[3][2] * data[1][3] + data[3][0] * data[1][2] * data[2][3] 
						- data[1][3] * data[2][2] * data[3][0] - data[2][3] * data[3][2] * data[1][0] - data[3][3] * data[1][2] * data[2][0]);

			/*
				m[0][0]	m[2][0]	m[3][0]
				m[0][2]	m[2][2]	m[3][2]
				m[0][3]	m[2][3]	m[3][3]
			*/
			inv[1][1] = data[0][0] * data[2][2] * data[3][3] + data[2][0] * data[3][2] * data[0][3] + data[3][0] * data[0][2] * data[2][3] 
						- data[0][3] * data[2][2] * data[3][0] - data[2][3] * data[3][2] * data[0][0] - data[3][3] * data[0][2] * data[2][0];

			/*
				m[0][0]	m[1][0]	m[3][0]
				m[0][2]	m[1][2]	m[3][2]
				m[0][3]	m[1][3]	m[3][3]
			*/
			inv[2][1] = -(data[0][0] * data[1][2] * data[3][3] + data[1][0] * data[3][2] * data[0][3] + data[3][0] * data[0][2] * data[1][3] 
						- data[0][3] * data[1][2] * data[3][0] - data[1][3] * data[3][2] * data[0][0] - data[3][3] * data[0][2] * data[1][0]);

			/*
				m[0][0]	m[1][0]	m[2][0]
				m[0][2]	m[1][2]	m[2][2]
				m[0][3]	m[1][3]	m[2][3]
			*/
			inv[3][1] = data[0][0] * data[1][2] * data[2][3] + data[1][0] * data[2][2] * data[0][3] + data[2][0] * data[0][2] * data[1][3] 
						- data[0][3] * data[1][2] * data[2][0] - data[1][3] * data[2][2] * data[0][0] - data[2][3] * data[0][2] * data[1][0];

			/*
				m[1][0]	m[2][0]	m[3][0]
				m[1][1]	m[2][1]	m[3][1]
				m[1][3]	m[2][3]	m[3][3]
			*/
			inv[0][2] = data[1][0]  * data[2][1] * data[3][3] + data[2][0]  * data[3][1] * data[1][3] + data[3][0]  * data[1][1] * data[2][3] 
						- data[1][3]  * data[2][1] * data[3][0] - data[2][3] * data[3][1] * data[1][0] - data[3][3] * data[1][1] * data[2][0];

			/*
				m[0][0]	m[2][0]	m[3][0]
				m[0][1]	m[2][1]	m[3][1]
				m[0][3]	m[2][3]	m[3][3]
			*/
			inv[1][2] = -(data[0][0] * data[2][1] * data[3][3] + data[2][0] * data[3][1] * data[0][3] + data[3][0] * data[0][1] * data[2][3] 
						- data[0][3] * data[2][1] * data[3][0] - data[2][3] * data[3][1] * data[0][0] - data[3][3] * data[0][1] * data[2][0]);

			/*
				m[0][0]	m[1][0]	m[3][0]
				m[0][1]	m[1][1]	m[3][1]
				m[0][3]	m[1][3]	m[3][3]
			*/
			inv[2][2] = data[0][0]  * data[1][1] * data[3][3] + data[1][0]  * data[3][1] * data[0][3] + data[3][0]  * data[0][1] * data[1][3] 
						- data[0][3]  * data[1][1] * data[3][0] - data[1][3] * data[3][1] * data[0][0] - data[3][3] * data[0][1] * data[1][0];

			/*
				m[0][0]	m[1][0]	m[2][0]
				m[0][1]	m[1][1]	m[2][1]
				m[0][3]	m[1][3]	m[2][3]
			*/
			inv[3][2] = -(data[0][0] * data[1][1] * data[2][3] + data[1][0] * data[2][1] * data[0][3] + data[2][0] * data[0][1] * data[1][3] 
						- data[0][3] * data[1][1] * data[2][0] - data[1][3] * data[2][1] * data[0][0] - data[2][3] * data[0][1] * data[1][0]);

			/*
				m[1][0]	m[2][0]	m[3][0]
				m[1][1]	m[2][1]	m[3][1]
				m[1][2]	m[2][2]	m[3][2]
			*/
			inv[0][3] = -(data[1][0] * data[2][1] * data[3][2] + data[2][0] * data[3][1] * data[1][2] + data[3][0] * data[1][1] * data[2][2] 
						- data[1][2] * data[2][1] * data[3][0] - data[2][2] * data[3][1] * data[1][0] - data[3][2] * data[1][1] * data[2][0]);

			/*
				m[0][0]	m[2][0]	m[3][0]
				m[0][1]	m[2][1]	m[3][1]
				m[0][2]	m[2][2]	m[3][2]
			*/
			inv[1][3] = data[0][0] * data[2][1] * data[3][2] + data[2][0] * data[3][1] * data[0][2] + data[3][0] * data[0][1] * data[2][2] 
						- data[0][2] * data[2][1] * data[3][0] - data[2][2] * data[3][1] * data[0][0] - data[3][2] * data[0][1] * data[2][0];

			/*
				m[0][0]	m[1][0]	m[3][0]
				m[0][1]	m[1][1]	m[3][1]
				m[0][2]	m[1][2]	m[3][2]
			*/
			inv[2][3] = -(data[0][0] * data[1][1] * data[3][2] + data[1][0] * data[3][1] * data[0][2] + data[3][0] * data[0][1] * data[1][2] 
						- data[0][2] * data[1][1] * data[3][0] - data[1][2] * data[3][1] * data[0][0] - data[3][2] * data[0][1] * data[1][0]);

			/*
				m[0][0]	m[1][0]	m[2][0]
				m[0][1]	m[1][1]	m[2][1]
				m[0][2]	m[1][2]	m[2][2]
			*/
			inv[3][3] = data[0][0] * data[1][1] * data[2][2] + data[1][0] * data[2][1] * data[0][2] + data[2][0] * data[0][1] * data[1][2] 
						- data[0][2] * data[1][1] * data[2][0] - data[1][2] * data[2][1] * data[0][0] - data[2][2] * data[0][1] * data[1][0];

			det = data[0][0] * inv[0][0] + data[0][1] * inv[0][1] + data[0][2] * inv[0][2] + data[0][3] * inv[0][3];

			if (det == 0)
				return false;

			det = 1.0f / det;

			for (i = 0; i < 4; i++) {
				invOut[0][i] = inv[i][0] * det;
				invOut[1][i] = inv[i][1] * det;
				invOut[2][i] = inv[i][2] * det;
				invOut[3][i] = inv[i][3] * det;
			}

			return true;
		}
	};
}

#endif
