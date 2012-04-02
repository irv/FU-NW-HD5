#ifndef MP3FILE_H
#define MP3FILE_H

#include "file.h"
/**
@author Andrew Irving, Karl Keeton
*/
class MP3File : public File {
protected:
	std::string artist;
	std::string title;
	std::string album;
	std::string genre;
	unsigned int year;
	unsigned int trackno;
	unsigned int tracklen;
	bool valid;

public:
	MP3File(const std::string &filename, const std::_Ios_Openmode& flags);
    ~MP3File();
	std::string getArtist() const { return artist; }
	std::string getAlbum() const { return album; }
	std::string getTitle() const { return title; }
	std::string getGenre() const { return genre; }
	unsigned int getYear() const { return year; }
	unsigned int getTrackno() const { return trackno; }
	unsigned int getTracklen() const { return tracklen; }
	int read(int *data, const int len);
	bool isValid() { return valid; }

};

#endif
