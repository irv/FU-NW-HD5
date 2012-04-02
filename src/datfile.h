#ifndef DATFILE_H
#define DATFILE_H
#include <string>
#include <fstream>
#include <map>
#include <list>
#include "file.h"

/**
@author Andrew Irving, Karl Keeton
*/
class DatFile : public File {
protected:
	std::list< std::map<std::string,std::string> > records;
	std::string serialise(std::map<std::string, std::string> &rec);
	std::string serialise(const std::string &key, const std::string &value);
public:
	DatFile(const std::string &filename, const std::_Ios_Openmode& flags);

    ~DatFile();
	void readFile();
	std::map<std::string, std::string> createRecord(const std::string &title, const std::string &artist,
					  const std::string &album, const std::string &genre);
	void writeRecord(std::map<std::string, std::string> &rec);
	bool recordExists(const std::map<std::string, std::string> &rec);

};

#endif
