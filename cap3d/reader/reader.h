#ifndef __READER_H_
#define __READER_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "../common/defines.h"
#include "model3d.h"

using namespace std;

struct OpenFileException : public exception {
   const char * what () const throw () {
      return "Could not open this file.";
   }
};

template<typename T> T endian_swap(const T & v) { return v; }
template<> inline uint16_t endian_swap(const uint16_t & v) { return (v << 8) | (v >> 8); }
template<> inline uint32_t endian_swap(const uint32_t & v) { return (v << 24) | ((v << 8) & 0x00ff0000) | ((v >> 8) & 0x0000ff00) | (v >> 24); }
template<> inline uint64_t endian_swap(const uint64_t & v) {
		return (((v & 0x00000000000000ffLL) << 56) |
			((v & 0x000000000000ff00LL) << 40) |
			((v & 0x0000000000ff0000LL) << 24) |
			((v & 0x00000000ff000000LL) << 8) |
			((v & 0x000000ff00000000LL) >> 8) |
			((v & 0x0000ff0000000000LL) >> 24) |
			((v & 0x00ff000000000000LL) >> 40) |
			((v & 0xff00000000000000LL) >> 56));
}

template<> inline int16_t endian_swap(const int16_t & v) { uint16_t r = endian_swap(*(uint16_t*)&v); return *(int16_t*)&r; }
template<> inline int32_t endian_swap(const int32_t & v) { uint32_t r = endian_swap(*(uint32_t*)&v); return *(int32_t*)&r; }
template<> inline int64_t endian_swap(const int64_t & v) { uint64_t r = endian_swap(*(uint64_t*)&v); return *(int64_t*)&r; }
inline float endian_swap_float(const uint32_t & v) { uint32_t r = endian_swap(v); return *(float*)&r; }
inline double endian_swap_double(const uint64_t & v) { uint64_t r = endian_swap(v); return *(double*)&r; }

template<typename T>
void type_cast(void * dest, const char * src, bool be) {
	*(static_cast<T *>(dest)) = (be) ? endian_swap(*(reinterpret_cast<const T *>(src))) : *(reinterpret_cast<const T *>(src));
}

template<typename T>
void type_cast_float(void * dest, const char * src, bool be)	{
	*(static_cast<T *>(dest)) = (be) ? endian_swap_float(*(reinterpret_cast<const uint32_t *>(src))) : *(reinterpret_cast<const T *>(src));
}

template<typename T>
void type_cast_double(void * dest, const char * src, bool be) {
	*(static_cast<T *>(dest)) = (be) ? endian_swap_double(*(reinterpret_cast<const uint64_t *>(src))) : *(reinterpret_cast<const T *>(src));
}

class Reader {
	protected:
		vector<PointField> fields;

		void fillModelAttributes(Model3D *);
	public:
		virtual ~Reader() {}

		virtual int load(const char *filename, float scale, Model3D &){
			return -1;
		};
		virtual Model3D* load(const char *filename, float scale, bool should_add_normal, bool should_add_color) {
			return NULL;
		};
		/*
			Load without scaling
		*/
		virtual Model3D* load(const char *filename){
			return NULL;
		};
		virtual void load(const char *filename, Model3D &){};
		virtual int save(const Model3D *, const char *filename, int format){
			std::cout << "[DEBUG] Reader::save" << std::endl;
			return -1;
		};
		virtual int parse_line2(string line, vector<Token> &v) = 0;

		int getVertexSize();
};

#endif
