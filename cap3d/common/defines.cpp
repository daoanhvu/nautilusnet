#include "defines.h"

#include <iostream>

type::FieldType getTypeByCode(int code) {
	type::FieldType t;
	switch(code) {
		case CODE_INT8:
			t = type::INT8;
		break;

		case CODE_UINT8:
			t = type::UINT8;
		break;

		case CODE_INT16:
			t = type::INT16;
		break;

		case CODE_UINT16:
			t = type::UINT16;
		break;

		case CODE_INT32:
			t = type::INT32;
		break;

		case CODE_UINT32:
			t = type::UINT32;
		break;

		case CODE_INT64:
			t = type::INT64;
		break;

		case CODE_UINT64:
			t = type::UINT64;
		break;

		case CODE_FLOAT32:
			t = type::FLOAT32;
		break;
		case CODE_FLOAT64:
			t = type::FLOAT64;
		break;

		default:
			t = type::TYPE_UNKNOWN;
		break;
	}
	return t;
}

FieldCode getCodeByValue(int code) {
	FieldCode t;
	switch(code) {
		case CODE_COORD_X:
			t = X;
		break;

		case CODE_COORD_Y:
			t = Y;
		break;

		case CODE_COORD_Z:
			t = Z;
		break;

		case CODE_NORMAL_X:
			t = NORMAL_X;
		break;

		case CODE_NORMAL_Y:
			t = NORMAL_Y;
		break;

		case NORMAL_Z:
			t = NORMAL_Z;
		break;

		case CODE_RED:
			t = RED;
		break;

		case CODE_GREEN:
			t = GREEN;
		break;

		case CODE_BLUE:
			t = BLUE;
		break;

		case CODE_ALPHA:
			t = ALPHA;
		break;

		case CODE_IMX:
			t = IMX;
		break;

		case CODE_IMY:
			t = IMY;
		break;

		case CODE_RGB:
			t = RGB;
		break;

		case CODE_RGBA:
			t = RGBA;
		break;

		default:
			t = CODE_UNKNOWN;
		break;
	}
	return t;
}

// return the stride according to data_type
int readPointField(PointField &pf, int data_type) {
	int stride;
	// std::cout << "[DEBUG] data_type " << data_type << std::endl;
	switch(data_type) {
		case type::UINT8:
			stride = 1;
			pf.type = type::UINT8;
		break;

		case type::INT8:
			stride = 1;
			pf.type = type::INT8;
		break;

		case type::UINT16:
			stride = 2;
			pf.type = type::UINT16;
		break;

		case type::INT16:
			stride = 2;
			pf.type = type::INT16;
		break;

		case type::INT32:
			stride = 4;
			pf.type = type::INT32;
		break;

		case type::UINT32:
			stride = 4;
			pf.type = type::UINT32;
		break;

		case type::INT64:
			stride = 8;
			pf.type = type::INT64;
		break;

		case type::UINT64:
			stride = 8;
			pf.type = type::UINT64;
		break;

		// case type::FLOAT32:
		case CODE_FLOAT32:
			stride = 4;
			pf.type = type::FLOAT32;
		break;

		// case type::FLOAT64:
		case CODE_FLOAT64:
			stride = 8;
			pf.type = type::FLOAT64;
		break;

		default:
			stride = 0;
			pf.type = type::TYPE_UNKNOWN;
		break;
	}

	pf.size = stride;

	return stride;
}
