#ifndef OMAFILE_H
#define OMAFILE_H
#include <string>
#include <iostream>
#include "file.h"
/**
@author Andrew Irving, Karl Keeton
*/
class OMAFile : public File {
protected:
	std::string headerDetail;
	void addHeader(const std::string &tagname, const std::string &value);
public:
	OMAFile(const std::string &filename, const std::_Ios_Openmode& flags);
    ~OMAFile();
	bool isOpen();
	void writeHeader(const std::string &title, const std::string &artist,
					const std::string &album, const std::string &genre,
					const int &track, const int &year, const int &tracklength);
	void writeBody(int data, const int len);
};

#endif
