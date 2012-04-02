#include "datfile.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <map>
DatFile::DatFile(const std::string &filename, const std::_Ios_Openmode& flags) : File(filename, flags)
{
}


DatFile::~DatFile()
{

}

void DatFile::readFile()
{
	file.seekg (0, std::ios::end);
	long filelen = file.tellg();
	file.seekg (0, std::ios::beg);

	// dat files have a 48 byte header. the file is then split into records
	// each record has a 16 byte header. the last 2 bytes specify how many
	// tags and the size of them, eg 05 80 would mean 5 tags, 128 bytes in
	// length (each).
	// read header
	// read through the file in the chunkc, pass strings to
	// createRecord and bob's your uncle
	std::string info;
	int buffer[16];
	char *blah;

	int numtags, lengths;
	std::stringstream ss;
	file.seekg (48, std::ios::beg);
	for (int i=48; (i+16)<filelen; i += 16)
	{
		file.read (reinterpret_cast<char*>(&buffer), 16);
		info.assign(reinterpret_cast<char*>(&buffer));
		std::cout << reinterpret_cast<char*>(&buffer)<<std::endl;
		ss <<info.at(info.length() - 2);
		ss >> numtags;
		ss.str(std::string());
		ss << info.at(info.length() - 1);
		ss >> lengths;
		std::cout << lengths<<std::endl;
		ss.str(std::string());
		blah = new char[lengths];
		for (int j=0; j<numtags; j++)
		{
			file.read (blah, lengths);
			std::cout<<blah<<std::endl;
		}
		delete [] blah;
	}

}
// need to distinguish between exists in memory and exists in the file
bool DatFile::recordExists(const std::map<std::string, std::string> &rec)
{
	return std::find(records.begin(),records.end(),rec)!=records.end();
}

void DatFile::writeRecord(std::map<std::string, std::string> &rec)
{
	if(recordExists(rec))
	{
		std::string data = serialise(rec);
		file.write (data.c_str(), data.size());
	}

}

std::string DatFile::serialise(std::map<std::string, std::string> &rec)
{
	char header[] = {0x00, 0x00, 0xFF, 0xFE, 0x03, 0x90, 0xDE,
		 			0x10, 0x00, 0x04, 0x64, 0x49, 0x00, 0x05,
					0x00, 0x80};
	std::string output;
	// fucking stl map auto sorts by key and not the order i create it
	// so simply using an iterator to loop through and do this won't
	// work because they have to be written out in this order.
	output.assign(header, header+sizeof(header));
	output.append(serialise("TIT2",std::string(rec["TIT2"])));
	output.append(serialise("TPE1",std::string(rec["TPE1"])));
	output.append(serialise("TALB",std::string(rec["TALB"])));
	output.append(serialise("TCON",std::string(rec["TCON"])));
	output.append(serialise("TSOP",std::string(rec["TSOP"])));
	return output;
}

std::string DatFile::serialise(const std::string &key, const std::string &value)
{
	std::string output;
	output.append(key);
	output.append(padding(1));
	output.append(std::string(1,char(2)));
	output.append(padding(1));
	output.append(nullPad(value));
	if(output.length() > 128)
		output = output.substr(0,128);
	else
		output.append(padding(128 - output.length()));
	return output;
}

std::map<std::string,std::string> DatFile::createRecord(const std::string &title, const std::string &artist,
					   const std::string &album, const std::string &genre)
{
	std::map<std::string, std::string> output;
	output["TIT2"] = title;
	output["TPE1"] = artist;
	output["TALB"] = album;
	output["TCON"] = genre;
	// no fucking idea what this
	output["TSOP"] = "";
	records.push_back(output);
	return output;
}

