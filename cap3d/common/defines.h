#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <string>

//Vertex's properties starting from 0
#define CODE_COORD_X 	        0
#define CODE_COORD_Y 	        1
#define CODE_COORD_Z 	        2
#define CODE_NORMAL_X 	        3
#define CODE_NORMAL_Y 	        4
#define CODE_NORMAL_Z 	        5
#define CODE_RED	          	6
#define CODE_GREEN	          	7
#define CODE_BLUE	          	8
#define CODE_ALPHA	          	9
#define CODE_IMX	          	10
#define CODE_IMY	          	11
#define CODE_RGB	          	12
#define CODE_RGBA	          	13

// format type start from 30
#define CODE_ASCII 		    		30
#define CODE_BINARY					31
#define CODE_BINARY_LITTLE_ENDIAN 	32
#define CODE_BINARY_BIG_ENDIAN 		33

//define token codes for PLY start from 40
#define CODE_PLY 			  	40
#define CODE_PROPERTY 	      	41
#define CODE_NUMBER 		    42
#define CODE_LIST 			    43
#define CODE_FORMAT 		    44
#define CODE_FACE 			    45
#define CODE_END_HEADER       	46
#define CODE_COMMENT 	        47
#define CODE_ELEMENT 	        48
#define CODE_VERTEX		        49
#define CODE_VERTEX_INDICES		50
#define CODE_CAMERA 		    51

//define token codes for PCD start from 60
#define CODE_PCD_VERSION	60
#define CODE_PCD_FIELDS     61
#define CODE_PCD_SIZE       62
#define CODE_PCD_TYPE       63
#define CODE_PCD_COUNT      64
#define CODE_PCD_WIDTH      65
#define CODE_PCD_HEIGHT     66
#define CODE_PCD_VIEWPOINT  67
#define CODE_PCD_POINTS     68
#define CODE_PCD_DATA       69
#define CODE_PCD_SIGNED		70
#define CODE_PCD_UNSIGNED	71

//Start data_type start from 100
#define CODE_UINT8	          100  // also CODE_UCHAR
#define CODE_UCHAR	          100  // also CODE_UCHAR
#define CODE_INT8	          101  // also CODE_CHAR
#define CODE_INT16 		      102
#define CODE_UINT16	          103
#define CODE_INT32 		      104
#define CODE_UINT32	          105
#define CODE_INT64 		      106
#define CODE_UINT64	          107
#define CODE_FLOAT32 	      108
#define CODE_FLOAT64 	      109

namespace type {
	enum FieldType {
		UINT8 = CODE_UINT8,
		INT8,
		INT16,
		UINT16,
		INT32,
		UINT32,
		INT64,
		UINT64,
		FLOAT32,
		FLOAT64,
		TYPE_UNKNOWN
	};
}

enum FieldCode {
		X = CODE_COORD_X,
		Y,
		Z,
		NORMAL_X,
		NORMAL_Y,
		NORMAL_Z,
		RED,
		GREEN,
		BLUE,
		ALPHA,
		IMX,
		IMY,
		RGB,
		RGBA,
		CODE_UNKNOWN
};

typedef struct tagPointField {
	FieldCode code;
	type::FieldType type;
	short index;
	short size; // This is size in byte

	public:
		tagPointField(){}
		tagPointField(FieldCode c, type::FieldType t){
			code = c;
			type = t;
		}
} PointField;

typedef struct tagTK {
	int code;

	//In case of code = CODE_NUMBER
	float value;
	std::string str;

	tagTK() {
		code = 0;
		value = 0.0f;
	}
	tagTK(int cd) {
		code = cd;
		value = 0.0f;
	}
	tagTK(int cd, float v) {
		code = cd;
		value = v;
	}
} Token;

type::FieldType getTypeByCode(int code);
FieldCode getCodeByValue(int code);
int readPointField(PointField &pf, int data_type);


#endif