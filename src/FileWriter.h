#ifndef FILEWRITER_H
#define FILEWRITER_H

#include <fstream>
#include <iostream>
#include <vector>

class FileWriter {
public:
	FileWriter();
	~FileWriter();

	static bool writeGreyScalePPM(std::string const &fileName, int const width, int const height, std::vector<float> const &values);

private:

};

#endif // !FILEWRITER_H
