#include <fcntl.h>
#include "INIFile.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
static int isFirst = 1;
static char lineBuff[CONF_LINE_LEN];
static int     _VScanf(const char* str, const char* fmt, va_list ap);
static char *  _Varread(CONFIG *cfg,const char* key, const char* item, 
			const char* fmt, va_list ap);
static const char* _ConvInt(const char* str, int limit, int* result) ;
static const char* _ConvStr(const char* str, int limit, char* result); 
static const char* _ConvRemains(const char* str, int limit, char* result);
static char* lowerstr(char *p);
static CONFIG *OpenConfig_pre(char *ident,int oflag);
static void  CloseConfig_pre(CONFIG *Config);
static void  RewindConfig_pre(CONFIG *Config);
static char  *ReadConfig_pre(CONFIG *Config,const char *key,const char *item,const char *fmt, ...);
static CONFIG *OpenConfig_pre(char *ident,int oflag)
{
  CONFIG *Config;

  if ((Config=(CONFIG *)malloc(sizeof(CONFIG)))==NULL) 
    return NULL;
   
  Config->fd=-1;
  Config->fp=NULL;
  Config->KeyEnd=TRUE;
  *(Config->LastKey)='\0';

  /* XXX modified by zyq */
  Config->fd = open(ident, oflag==OPEN_RDWR ? O_RDWR : O_RDONLY);
  if (Config->fd==-1)	goto err;
  Config->fp = fdopen(Config->fd, oflag==OPEN_RDWR ? "r+" : "r");
  if (!Config->fp) goto err;
  /*
    if ( (Config->fp=fopen(ident,(oflag==OPEN_RDWR)?"r+":"r"))==NULL )
    goto err;;                       

    if ( (Config->fd=fileno(Config->fp))<0 ) 
    goto err;
  */
  return(Config);
 err:
  CloseConfig_pre(Config);
  return(NULL); 
}
static void  CloseConfig_pre(CONFIG *Config)
{
  if (Config) {
    if (Config->fp) 
      fclose(Config->fp);
    free(Config);
  }
}
static void  RewindConfig_pre(CONFIG *Config)
{
  if (Config) {
    if (Config->fp) {
      rewind(Config->fp);
      Config->KeyEnd=TRUE;
      *(Config->LastKey)='\0';
    }
  }
}
static char  *ReadConfig_pre(CONFIG *Config,const char *key,const char *item,const char *fmt, ...)
{
  char *res;
  va_list ap;
	
  res=NULL;
  va_start(ap, fmt);
  res=_Varread(Config,key,item,fmt,ap);
  va_end(ap);

  return res;
}

static char * _Varread(CONFIG *cfg,const char* key, const char* item, const char* fmt, va_list ap)
{
  char *LastKey;
  //static char CurItem[NAME_LEN+1];
  char CurKey[NAME_LEN+2+1];
  char CurLine[CONF_LINE_LEN+1+1]; /*one byte for " "*/
  char *ptr;
  int  foundKey = 1;
  //int res;
    
  //invalid check
  if ( cfg==NULL || cfg->fp==NULL || key==NULL || *key=='\0' || strlen(key)>NAME_LEN ||
       (item && (*item=='\0' || strlen(item)>NAME_LEN) ) ) 
       {
       
    goto nofound;
    }
    
  LastKey=cfg->LastKey;
   

  sprintf(CurKey,"[%s]",key);
  lowerstr(CurKey);

  //retrieve the new key section
  if (strcmp(LastKey,CurKey))  {
      
    strcpy(LastKey,CurKey);
	  
    rewind(cfg->fp);
    while( !feof( cfg->fp ) ) {
        
      *CurLine='\0';
      fgets(CurLine, CONF_LINE_LEN, cfg->fp);
        
      ptr=strtok(CurLine," \t\r\n#");
        
      if ((ptr)&&!strcmp(lowerstr(ptr),CurKey))
      {
      	
	break;
	}
    }
      
    if (!feof(cfg->fp))
    {
    	foundKey = 1;
    }
    else
    {
    	foundKey = 0;
    }
    isFirst = 1;
  }
  else
  {
  	
  	isFirst = 0;
  }
    
  if (foundKey == 0) goto nofound;
   
  while(!feof(cfg->fp)) {
         
    *CurLine='\0';
    fgets(CurLine, CONF_LINE_LEN, cfg->fp);
     
   
    if ((ptr=strchr(CurLine,'#')) ||( ptr=strchr(CurLine, '/'))) //get  rid of notes
      continue;
    if (*CurLine == '[')
    	{
    		goto nofound;
    	}
      break;
     
  }
    if (!feof(cfg->fp))
       strcpy(lineBuff, CurLine);
    else
       return NULL;
  
  return lineBuff;  
 
 nofound:
  return NULL;
}

static int _VScanf(const char* str, const char* fmt, va_list ap)
{
  const char* nextPos;
  char ch;
  int limit, count = 0;
    
  //if ((str==NULL)||(fmt==NULL)) return -1;
  if (str==NULL) 
    return -1;
  else if (!fmt)
    return 0;

  while (*fmt) {
    /* Fetch a format char
     */
    ch = *fmt++;

    /* Check to see if it is a conversion indication?
     */
    if ('%' != ch) {
      if ((' ' == ch)||('\t' == ch))
	continue;
      /*else if (*str++ == ch)
	continue;*/
      else
	return -1; /* the covertion was terminated by illegal characters */
    }

    /* Process limit
     */
    limit = 0;
    ch = *fmt++; if (ch == '\0') return count;
    while (isdigit(ch)) {
      limit = limit * 10 + ch - '0';
      ch = *fmt++;
      if (ch == '\0') return count;
    }

    /* Process type conversion
     */
    switch (ch) {
    case 'd':
      nextPos = _ConvInt(str, limit, va_arg(ap, int*));
      break;
    case 's':
      nextPos = _ConvStr(str, limit, va_arg(ap, char*));
      break;
    case 't':
      nextPos = _ConvRemains(str,limit,va_arg(ap, char*));
      break;
    default: /* Format string error! */
      //return count;
      return -1;
    }
        
    if (nextPos) {
      count++;
      str = nextPos;
    }
    else {
      return -1;  //nextPos==null means error
    }
  } /* while */

  return count;
}


static const char* _ConvInt(const char* str, int limit, int* result)
{
  int p=0;

  if (result==NULL) return str;

  if (limit == 0) {
    //limit = INT_MAX;
    limit = DEFAULT_VALUE_LEN;
  }
    
  /* Skip the leading space
   */
  while (*str && isspace(*str)) str++;
    
  *result = 0;
  while (*str && isdigit(*str)) {
    if (p++<limit)
      *result = *result * 10 + *str - '0';
    str++;
  }
    
  if (!p) return NULL;   //error
    
  return str;
}

static const char* _ConvStr(const char* str, int limit, char* result)
{
  int p=0;
	
  if (result==NULL) return str;

  if (limit == 0) {
    //limit = INT_MAX;
    limit=DEFAULT_VALUE_LEN;
  }

  /* Skip the leading space
   */
  while (*str && isspace(*str)) str++;

  while (*str && !isspace(*str)) {
    if (p++<limit)
      *result++ = *str;
    str++;
  }
  *result = '\0';

  if (!p)  return NULL; //error

  return str;
}


static const char* _ConvRemains(const char* str, int limit, char* result)
{
  int p=0;
	
  if (result==NULL) return str;

  if (limit == 0) {
    limit = CONF_LINE_LEN;
  }

  /* Skip the leading space
   */
  while (*str && isspace(*str)) str++;

  /* Get remains
   */
  while (*str) {
    if (p++<limit)
      *result++ = *str;
    str++;
  }
  *result = '\0';

  //if (!p)  return NULL; //error

  return str;
}

static char* lowerstr(char *p)
{
  int len = strlen(p);
  int i = 0;
  for (i = 0; i < len; i++)
    {
      p[i] = tolower(p[i]);
    }
  return p;
}


bool AosINIFile::OpenConfig()
{
  
  mpConfig = OpenConfig_pre(mpFilePath, mOflag);
  if (!mpConfig)
    {
      return false;
    }
  return true;
}
void  AosINIFile::CloseConfig()
{
  CloseConfig_pre(mpConfig);
}
void  AosINIFile::RewindConfig()
{
  RewindConfig_pre(mpConfig);
}
char  *AosINIFile::ReadConfig(const char *key)
{
  //char *item;
  static char buff[CONF_LINE_LEN];
  strcpy(buff, "");
  return ReadConfig_pre(mpConfig, key, NULL, "%t", buff);
}
