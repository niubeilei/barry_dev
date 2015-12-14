# File: makefile
#
# This is a makefile for use with g++
#
CC = g++

.SUFFIXES: .cpp .h

FLAGS = -g -Wall -DOMN_PLATFORM_UNIX $(Optimize) -DAOS_USERLAND

INCDIRS = -I../ -I/usr/local/scws/include -I/usr/local/include/ImageMagick

OUTPUTDIR = ../../obj

LIBS = -L/usr/local/scws/lib -lnsl -lpthread -lstdc++ -lscws -lboost_date_time 

#
# Rules for normal comile and link 
#
all:: lib

lib: 
	$(CC) -fpic -shared -c $(FLAGS) $< $(INCDIRS) DLLDataProcStatBasic.cpp -o DLLDataProcStatBasic.so
	mv DLLDataProcStatBasic.so $(OUTPUTDIR)

clean:
	rm -rf $(OBJECTS) $(OUTPUTDIR)/DLLDataProc*.so
