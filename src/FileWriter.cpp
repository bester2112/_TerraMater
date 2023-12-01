#include "FileWriter.h"

FileWriter::FileWriter() {}

FileWriter::~FileWriter() {}

bool FileWriter::writeGreyScalePPM(std::string const &fileName, int const width, int const height, std::vector<float> const &values) {
	std::ofstream file;

	file.open(fileName);

	if (!file.is_open()) {
		std::cout << "Couldn't open file " << fileName << std::endl;
		return false;
	}

	file << "P2\n";
	file << width;
	file << " ";
	file << height;
	file << "\n";
	file << "255\n";

	int size = values.size();

	for (int i = 0; i < size; ++i) {
		file << (unsigned short)(values[i] * 255) << "\n";
	}

	file.close();

	return true;
}
