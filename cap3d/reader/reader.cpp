
#include "reader.h"

void Reader::fillModelAttributes(Model3D *model) {
	int field_count = fields.size();
	type::FieldType type;
	FieldCode code;
	VertexAttrib att;
	bool got_color;
	int idx;
	int float_stride = 0;

	for(int i=0; i<field_count; i++) {
		type = fields[i].type;
		code = fields[i].code;
		got_color = false;

		switch(code) {
			case X:
				att.code = POSITION;
				att.offset = float_stride;
				model->addAttrib(att);
				float_stride++;
			break;

			case RED:
				if(!got_color) {
					att.code = COLOR3;
					att.offset = float_stride;
					model->addAttrib(att);
					got_color = true;
				}
				float_stride++;
			break;

			case GREEN:
				if(!got_color) {
					att.code = COLOR3;
					att.offset = float_stride;
					model->addAttrib(att);
					got_color = true;
				}
				float_stride++;
			break;

			case BLUE:
				if(!got_color) {
					att.code = COLOR3;
					att.offset = float_stride;
					model->addAttrib(att);
					got_color = true;
				}
				float_stride++;
			break;

			case ALPHA:
				idx = model->getAttributeIndex(COLOR3);
				model->setAttributeByIndex(idx, COLOR4);
			break;

			case NORMAL_X:
				att.code = NORMAL;
				att.offset = float_stride;
				model->addAttrib(att);
				float_stride++;
			break;

			case IMX:
				att.code = TEXTURE;
				att.offset = float_stride;
				model->addAttrib(att);
				float_stride++;
			break;

			default:
				float_stride++;
			break;
		}

		model->setFloatStride(float_stride);
	}
}

int Reader::getVertexSize() {
	int size = 0;
	int c = fields.size();

	for(int i=0; i<c; i++) {
		size += fields[i].size;
	}

	return size;
}