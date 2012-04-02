#ifndef FILE_H
#define FILE_H
#include <fstream>
/**
@author Andrew Irving, Karl Keeton
*/
class File{
protected:
	std::fstream file;
	std::string nullPad(const std::string &input);
    std::string padding(int length);
public:
	File(const std::string &filename,const std::_Ios_Openmode& flags);
    ~File();
	unsigned long getFileLength();

};

#endif
