#ifndef _VIEWER_H_
#define _VIEWER_H_

#include <vector>
#include "camera.h"
#include "reader/model3d.h"

using namespace std;

class Viewer {
	protected:
		int width;
		int height;

		float bgColor[3];
		Camera camera;
		vector<Model3D*> models;

	public:
		Viewer();
		virtual ~Viewer() {
			int s = models.size();
			for(int i=0; i<s; i++) {
				if(models[i] != NULL) {
					delete models[i];
				}
			}
		}

		void setup();

		void drawCoordinator();
		void drawScene();
};

#endif