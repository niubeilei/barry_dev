///////////////////////////////////////////////////////////////
// Created by:	wu xia
// Created:		09/19/2006	
// Comments:
//	   read and write *.ini file
//         note that only unix format file is supported in this version
// 
// Change History:
//	09/19/2006 File Created
///////////////////////////////////////////////////////////////
#ifndef AOS_INI_FILE_H
#define AOS_INI_FILE_H

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

//global type definitions
#define TRUE 1
#define FALSE 0
#define DELETE 10
#define INSERT 11
#define MODIFY 12

#define NAME_LEN 256               //the max len of all kinds of name
#define CONF_LINE_LEN 1024         //the max len of the line in the config file
#define PATH_NAME_LEN  CONF_LINE_LEN
#define DEFAULT_VALUE_LEN NAME_LEN //the maximum lengh of EACH value of the item.

//the open styte of the config file :
#define OPEN_RD   0
#define OPEN_RDWR 1

typedef unsigned short int USHORT;
typedef unsigned int  UINT;
typedef unsigned long ULONG;

typedef struct _CONFIG{
  FILE *fp;
  int   fd;
  char LastKey[NAME_LEN+2+1];
  int  KeyEnd;
} CONFIG;


//end
class AosINIFile
{
  public :
    AosINIFile(const char *ident, int oflag)
    {
      strcpy(mpFilePath, ident);
      mOflag = oflag;
    }
    ~AosINIFile()
    {
    }

  CONFIG *mpConfig;
  char mpFilePath[NAME_LEN];
  int mOflag;
  //interface definition      
 public:  
  bool OpenConfig();
  void  CloseConfig();
  void  RewindConfig();
  char  *ReadConfig(const char *key);
  //end interface definition
};
#endif
