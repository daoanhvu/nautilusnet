#include <iostream>
#include <fstream>
#include <cstring>

void getline(std::ifstream &fs, char *s, int &len) {
	int pos = fs.tellg();

	
}

int main() {
	std::ifstream file("/home/davu/data/NP1_0.pcd", std::ios::binary);
	std::string line;
	std::getline(file, line);
	std::cout << line << std::endl;
	std::getline(file, line);
	std::cout << line << std::endl;
	std::getline(file, line);
	std::cout << line << std::endl;
	std::getline(file, line);
	std::cout << line << std::endl;
	std::getline(file, line);
	std::cout << line << std::endl;
	std::getline(file, line);
	std::cout << line << std::endl;
	std::getline(file, line);
	std::cout << line << std::endl;
	std::getline(file, line);
	std::cout << line << std::endl;
	std::getline(file, line);
	std::cout << line << std::endl;
	std::getline(file, line);
	std::cout << line << std::endl;
	std::getline(file, line);
	std::cout << line << std::endl;

	unsigned int pos = file.tellg();
	std::cout << "start data: " << pos << std::endl;

	float *v = new float[4];

	for(int i=0; i<4570; i++) {
		file.read((char*)v, sizeof(float) * 4);
		std::cout << "x: " << v[0] << ", y: " << v[1] << ", z: " << v[2] << ", rgb: " << v[3] << std::endl;
	}


	delete[] v;
	file.close();
	return 0;
}