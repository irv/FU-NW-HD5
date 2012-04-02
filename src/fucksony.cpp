

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "omafile.h"
#include "datfile.h"
#include "mp3file.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <cstdlib>
// Unix dir stuff
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>


std::vector<std::string> getFileList(const std::string &directory, const std::string &type, const std::string &filter=0)
{
	std::vector<std::string> fileList;
	DIR *d;
	struct dirent *dir;
	struct stat st;
	d = opendir( directory.c_str() );
	if( d == NULL ) {
		std::stringstream ss;
		ss << errno;
		throw std::string("Unable to open directory "+directory+": "+ss.str());
	}
	while( ( dir = readdir( d ) ) ) {
		if( strcmp( dir->d_name, "." ) == 0 ||
			strcmp( dir->d_name, ".." ) == 0 ) {
			continue;
		}
		if(lstat(std::string(directory+"/").append(dir->d_name).c_str(), &st) < 0)
			throw std::string("Unable to open File ").append(std::string(directory+"/").append(dir->d_name));
		std::string tmp(dir->d_name);
		bool is_a;
		if(type == "file")
			is_a = S_ISREG(st.st_mode);
		else if (type == "dir")
			is_a = S_ISDIR(st.st_mode);
		if(!filter.empty())
		{
			if( tmp.substr(tmp.length()-3) == filter && is_a)
				fileList.push_back(std::string(dir->d_name));
		}
		else
		{
			if( is_a)
				fileList.push_back(std::string(dir->d_name));
		}
	}
	closedir( d );
	sort(fileList.begin(),fileList.end());
	return fileList;
}

std::string inttohex(int in)
{
	std::stringstream st;
	st << std::hex<<in;
	return st.str();
}
unsigned long hextoint(std::string in)
{

	std::stringstream ss(in);
	long number;
	ss >> std::hex >>number;
	return number;
}
std::string makedestinfilename(int fileNumber)
{
	std::string tmp = inttohex(fileNumber);
	//tmp.insert(8-tmp.length(),'0',0);
	tmp.insert(0,"1"+std::string(7-tmp.length(),'0'));
	tmp.append(".oma");
	return tmp;

}

void XOR(MP3File *mp3, OMAFile *oma)
{
	int key []= {0xe5,0x96,0x45,0x38};
	unsigned long pos,filelen,maxpos;
	unsigned int keylen,j;
	int buffer=0;
	int out=0,rem=0;


	keylen=sizeof(key)/sizeof(key[0]);
	filelen = mp3->getFileLength();
	// the file is XOR'd in 16 byte blocks
	maxpos = filelen;
	if ((rem = filelen %16) !=0)
		maxpos = filelen - rem;

	for(pos = 0L, j = 0; pos < maxpos; pos++, j++)
	{
		mp3->read(&buffer, 1);
		if(j ==keylen)
			j = 0;
		out = (key[j]^buffer&0xFF);
		oma->writeBody(out, 1);
	}
	// remaining bytes are not XOR'd
	if(rem)
	{
		buffer = mp3->read (&buffer, rem);
		oma->writeBody(buffer&0xFF, rem);
	}
}

int main(int argc, char *argv[])
{

	int missing[] = {0x0F,0x62,0xC8,0xBC,0xA1,0x15,0x1F,0xC1,0x49,0x0F,0x68};
// 	std::stringstream fuckit;
// 	fuckit << std::hex <<missing;
// 	std::cout<<fuckit.str()<<std::endl;
// 	return EXIT_SUCCESS;
	if(argc != 3)
	{
		std::cout << "You idiot, read the help"<<std::endl;
		return EXIT_FAILURE;
	}
	// see first of all if the two directories exist and if there are mp3's to be copied!

	std::vector<std::string>fileList;
	std::vector<std::string>playerList;
	std::vector<std::string>playerDirList;
	try
	{
		fileList = getFileList(std::string(argv[1]),"file", "mp3");
		//playerDirList = getFileList(std::string(argv[2]),"dir");
		//std::cout<<"here";
		// playerList = getFileList(playerDirList[playerDirList.size()-1],"file","oma");
		playerList = getFileList(std::string(argv[2])+"/10f01","file","oma");
	}
	catch(std::string error)
	{
		std::cerr<<"Error: "<< error << std::endl;
		return EXIT_FAILURE;
	}
	catch(const std::out_of_range range_error)
	{
		std::cerr<<"Error: "<< range_error.what() <<std::endl;
		return EXIT_FAILURE;
	}

	std::vector<std::string>::iterator it;
	std::string sourcefilename;
	std::string destinfilename;

	long fileNumber;
	long dirNumber;
	dirNumber = hextoint(playerDirList[playerDirList.size()-1].substr(4,2));
	fileNumber = hextoint(playerList[playerList.size()-1].substr(1,7));
	// next available file number...
	++fileNumber;

	MP3File *mp3;
	OMAFile *oma;
	DatFile *dat = new DatFile(std::string(argv[2])+"/04cntinf.dat",std::ios::out|std::ios::binary|std::ios::app);
	for (it = fileList.begin(); it != fileList.end(); it++) {
		sourcefilename = std::string(argv[1]).append("/"+*it);
		mp3 = new MP3File(sourcefilename, std::ios::in|std::ios::binary);
		if(!mp3->isValid())
			continue;
		// TODO: first check if this song/artist/album/year/genre etc already exist
		// if yes, break out this for loop.

		// get next file number
		destinfilename = std::string(argv[2]).append(makedestinfilename(fileNumber));
		//oma = new OMAFile(destinfilename, std::ios::out|std::ios::binary);
		oma = new OMAFile(std::string(argv[2])+"/10f01/"+makedestinfilename(fileNumber), std::ios::out|std::ios::binary);
		oma->writeHeader(mp3->getTitle(),mp3->getArtist(),mp3->getAlbum(),
			mp3->getGenre(),mp3->getTrackno(),mp3->getYear(),mp3->getTracklen());
		// XOR the mp3 file...
		XOR(mp3,oma);
		// check if file exists in datfile somewhere
// 		if(!dat->search(mp3->getTitle()))
// 			dat->insert();
		std::map<std::string, std::string> rec =
		dat->createRecord(mp3->getTitle(),mp3->getArtist(),mp3->getAlbum(),
						  mp3->getGenre());
		dat->writeRecord(rec);
		// write if it does
		fileNumber++;
		// 0xff files in each dir!
		if(fileNumber>255)
		{
			fileNumber = 1;
			dirNumber++;
		}
		delete mp3;
		delete oma;
		oma = 0;
		mp3 = 0;
	}
	delete dat;
	return EXIT_SUCCESS;
}
