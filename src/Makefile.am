bin_PROGRAMS = fucksony
fucksony_SOURCES = fucksony.cpp omafile.cpp datfile.cpp file.cpp mp3file.cpp

# set the include path found by configure
INCLUDES= $(all_includes) `taglib-config --cflags`

# the library search path.
fucksony_LDFLAGS = $(all_libraries) `taglib-config --libs`
noinst_HEADERS = omafile.h datfile.h file.h mp3file.h
