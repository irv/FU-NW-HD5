#include "file.h"

File::File(const std::string &filename, const std::_Ios_Openmode& flags)
{
	file.open(filename.c_str(), flags);
}


File::~File()
{
	file.close();
}


std::string File::padding(const int length)
{
	std::string output;
	output.append(length,static_cast<char>(0x00));
	return output;
}

std::string File::nullPad(const std::string &input)
{
	std::string output(input);
	int len = input.length();
	for(int i=1; i <output.length();i += 2)
	{
		output.insert(i,std::string(1,static_cast<char>(0x00)));
	}

	return output;
}
unsigned long File::getFileLength()
{
	file.seekg(0,std::ios::end);
	unsigned long len = file.tellg();
	file.seekg(0,std::ios::beg);
	return len;
}
