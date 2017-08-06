#ifndef __PLYREADER_H_
#define __PLYREADER_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "plymodel3d.h"
#include "reader.h"
#include "../lexer_parser.h"

#ifdef __linux__
	#include <cstdint>
#endif

using namespace std;

class PLYReader: public Reader {
	protected:
		bool isBigEndian;
		void readpoints(std::ifstream& file, unsigned int offs, int format_type, int vertex_count, vector<Vertex>& vertices, int float_stride);
	public:
		PLYReader(): isBigEndian(false) {
		}

		virtual ~PLYReader() {}

		virtual int load(const char *filename, float scale, Model3D &);
		virtual Model3D* load(const char *filename, float scale);
		/*
			Load without scaling
		*/
		virtual Model3D* load(const char *filename);
		virtual void load(const char *filename, Model3D &);
		virtual int save(const Model3D *, const char *filename);
		int parse_line2(string line, vector<Token> &v);
		void readpoints(std::ifstream& file, unsigned int offs, int format_type, int vertex_count, vector<Vertex>& vertices, int float_stride);
};

template<typename T> T endian_swap(const T & v) { return v; }
	template<> inline uint16_t endian_swap(const uint16_t & v) { return (v << 8) | (v >> 8); }
	template<> inline uint32_t endian_swap(const uint32_t & v) { return (v << 24) | ((v << 8) & 0x00ff0000) | ((v >> 8) & 0x0000ff00) | (v >> 24); }
	template<> inline uint64_t endian_swap(const uint64_t & v)
	{
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
void ply_cast_float(void * dest, const char * src, bool be)	{
	*(static_cast<T *>(dest)) = (be) ? endian_swap_float(*(reinterpret_cast<const uint32_t *>(src))) : *(reinterpret_cast<const T *>(src));
}

#endif
