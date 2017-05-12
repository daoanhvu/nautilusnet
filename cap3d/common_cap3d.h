#ifndef __COMMON_CAP3D_H_
#define __COMMON_CAP3D_H_

typedef struct tagVertex {
	float *v;
	float normal[3];
	int *face_indices;
	unsigned int count; //number of faces this vertex belong to
	unsigned int log_face_size;

	tagVertex() {
			v = NULL;
			count = 0;
			face_indices = new int[8];
			log_face_size = 8;
	}
} Vertex;

typedef struct tagFace {
	unsigned char vertex_count;
	int vertex_indices[5];
	float normal[3];
} Face;

typedef struct tagBBox3D {
	float minx;
	float maxx;
	float miny;
	float maxy;
	float minz;
	float maxz;
} BBox3d;


#endif
