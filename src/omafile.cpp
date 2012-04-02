#include <sstream>
#include "omafile.h"
OMAFile::OMAFile(const std::string &filename, const std::_Ios_Openmode& flags): File(filename, flags)
{
	headerDetail = "";
}


OMAFile::~OMAFile()
{
	file.close();
}

bool OMAFile::isOpen()
{
	return file.is_open();
}

void OMAFile::addHeader(const std::string &tagname, const std::string &value)
{
	// Tags consist of a tag name, 3 null bytes for padding
	// followed by the padded string length + 2. followed by 2 bytes of null padding
	// followed by 0x02 (i guess this is the offset), and one more byte of padding
	headerDetail.append(tagname);
	headerDetail.append(padding(3));
	std::string writevalue = nullPad(value);
	unsigned int len = writevalue.length()+2;
	headerDetail.append(std::string(1,char(len))+padding(2)+std::string(1,char(2)));
	headerDetail.append(padding(1));
	headerDetail.append(writevalue);

}

void OMAFile::writeHeader(const std::string &title, const std::string &artist,
				const std::string &album, const std::string &genre,
				const int &track, const int &year, const int &tracklength)
{
	// create all the tags
	// add trailing stuff.
	char header[] = {0x65,0x61,0x33,0x03,0x00,0x00,0x00,0x00,0x17,0x76};
	std::string tmp;
	tmp.assign(header, header+sizeof(header));
	headerDetail.insert(0,tmp);
	// easy bits
	addHeader("TIT2",title);
	addHeader("TPE1",artist);
	addHeader("TALB",album);
	addHeader("TCON",genre);
	// now things get weird
	std::ostringstream ss;
	std::string txx("OMG_TRACK");
	//ss << track;
	if(track < 10)
		ss << 0;
	ss<< track;
	// actually requires 3 null bytes, but it gets padded on either side
	txx.append(padding(1));
	txx.append(ss.str());
	addHeader("TXXX",txx);
	ss.str(std::string());
	txx.clear();
	// and this one holds the date!
	txx = "OMG_TRLDA";
	txx.append(padding(1));
	ss << year;
	// uses a datetime string
	txx.append("01/01/"+ss.str()+" 00:00:00");
	addHeader("TXXX",txx);
	ss.str(std::string());
	ss << tracklength;
	if(tracklength < 100)
		ss << 0;
	addHeader("TLEN",ss.str()+"000");

	// maximum size of the header is 3152 bytes, 3070 bytes
	// up to this weird sequence
	headerDetail.append(padding(3072-headerDetail.length()));
	char magic [] = {
			0x45, 0x41, 0x33, 0x02, 0x00, 0x60, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x01, 0x0f, 0x50, 0x00,
			0x00, 0x04, 0x00, 0x00, 0x00, 0x1a, 0x5f, 0xc8, 0xbc, 0xa1, 0x15, 0x1f, 0x58, 0x4e, 0x10, 0x82,
			0x03, 0x90, 0xde, 0x10, 0x00, 0x03, 0x6e, 0x78, 0x00, 0x00, 0x21, 0xa1, 0x00, 0x00, 0x00, 0x00};
	std::string magicstring;
	magicstring.assign(magic,magic+sizeof(magic));
	headerDetail.append(magicstring);
	headerDetail.append(padding(48));
	file.write(headerDetail.c_str(),headerDetail.length());

}

void OMAFile::writeBody(int data, const int len)
{
	file.write(reinterpret_cast<char*>(&data), len);
}
