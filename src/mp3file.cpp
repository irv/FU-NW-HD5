#include "mp3file.h"
// TagLib
#include <tag.h>
#include <fileref.h>
#include <mpegproperties.h>
#include <mpegfile.h>

MP3File::MP3File(const std::string &filename, const std::_Ios_Openmode& flags) : File(filename, flags)
{
	// get the track info. Uses TagLib.
	TagLib::FileRef f(filename.c_str());
	if (!f.file()->isValid())
		return;
	valid = true;
	artist = std::string(f.tag()->artist().toCString());
	title = std::string(f.tag()->title().toCString());
	album = std::string(f.tag()->album().toCString());
	genre = std::string(f.tag()->genre().toCString());
	trackno = f.tag()->track();
	year = f.tag()->year();
	TagLib::MPEG::Properties t(new TagLib::MPEG::File("file.mp3") );
	tracklen = t.length();
}


MP3File::~MP3File()
{
}

int MP3File::read(int *data, const int len)
{
	int ret=0;
	file.read(reinterpret_cast<char*>(data), len);
	return ret;

}

