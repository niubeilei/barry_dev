CC=/usr/bin/g++
CXX=/usr/bin/c++
AR=/usr/bin/ar

TARGET = libhttpparser.a 
OBJS = aos_parser.o aos_httpparser_util.o 
TOOL = ../../tools

OBJDIR = ../../../obj

all:: $(TARGET)

$(OBJS): %.o: %.c
	$(CC) -g -c $< -o $@
	
aos_parser: ${TOOL}/aos_gen_parser.pl aos_parsers.pm
	/usr/bin/perl -I. ${TOOL}/aos_gen_parser.pl

$(TARGET): aos_parser $(OBJS)
	$(AR) -ruv $@ $(OBJS) 
	@mv $(TARGET) $(OBJDIR) 

clean:
	rm -rf *.o aos_parser aos_parser.c aos_parser.h test ../../../obj/$(TARGET)
