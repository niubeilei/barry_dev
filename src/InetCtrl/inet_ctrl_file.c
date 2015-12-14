///////////////////////////////////////////////////////////////////////
// Author: Zhang QuanLin
// Creation Time: 07/28/2006
// Description:
//
// Change History
// 07/28/2006 inetctrlKernelFile.c created
// 08/02/2006 is changed :
//	 1) modfiy inetctrl_fopen function to use "rwa" char mode
//	 2) add inetctrl_fscanf function
///////////////////////////////////////////////////////////////////////

#include "inet_ctrl_file.h"

#ifdef __KERNEL__
// 
// Library Functions 
// 

// Context : User 
// Parameter : 
// KFILEname : KFILEname to open 
// flags : 
// O_RDONLY, O_WRONLY, O_RDWR 
// O_CREAT, O_EXCL, O_TRUNC, O_APPEND, O_NONBLOCK, O_SYNC, ... 
// mode : KFILE creation permission. 
// S_IRxxx S_IWxxx S_IXxxx (xxx = USR, GRP, OTH), S_IRWXx (x = U, G, O) 
// Return : 
// KFILE pointer. if error, return NULL 

//KFILE *inetctrl_fileopen(const char *filename, int flags, int mode) 
//{ 
//	KFILE *filp = filp_open(filename, flags, mode); 
//	return (IS_ERR(filp)) ? NULL : filp; 
//} 

// Context : User 
// Parameter : 
// KFILEname : KFILEname to open 
// mode : KFILE creation permission. 
// 'r':readonly; 'w':write file; 'a':read and write a file use append mode. 
// Return : 
// KFILE pointer. if error, return NULL 

KFILE *inetctrl_fopen( const char *filename, const char *mode ) 
{ 
	int i=0,flags=0;
	int mlen=strlen(mode);
	KFILE *filp;
	
	for( i=0; i<mlen; i++ )
	{
		switch( mode[i] )
		{
			case 'r' :
				if( (flags& O_ACCMODE) & O_RDWR )break;
				if( (flags& O_ACCMODE) & O_WRONLY)
					flags = O_CREAT | O_RDWR ;
				else flags |= O_RDONLY;
				break;
			case 'w' :
				if( (flags& O_ACCMODE) & O_RDWR )break;
				if( (flags& O_ACCMODE) & O_RDONLY)
					flags = O_CREAT | O_RDWR ;
				else flags = O_CREAT | O_WRONLY | O_TRUNC ;
				break;
			case 'a' :
				flags  = O_CREAT | O_RDWR | O_APPEND;
				break;
			default :
				break;
		}
	}
	
	filp = filp_open( filename, flags, S_IRUSR | S_IWUSR ); 
	
	return (IS_ERR(filp)) ? NULL : filp; 
} 

// Context : User 
// Parameter : 
// filp : KFILE pointer 
// Return : 

void inetctrl_fclose(KFILE *filp) 
{ 
	if ( filp )fput( filp );//filp_close( filp, NULL ); 
} 

// Context : User 
// Parameter : 
// filp : KFILE pointer 
// offset : 
// whence : SEEK_SET, SEEK_CUR 
// Comment : 
// do not support SEEK_END 
// no boundary check (KFILE position may exceed KFILE size) 

int inetctrl_fseek(KFILE *filp, int offset, int whence) 
{ 
	int pos = filp->f_pos; 
	
	if (filp) { 
		if (whence == SEEK_SET)pos = offset; 
		else if (whence == SEEK_CUR)pos += offset; 
		if (pos < 0)pos = 0; 
		return (filp->f_pos = pos); 
	} else return -ENOENT; 
} 

// Context : User 
// Parameter : 
// buf : buffer to read into 
// len : number of bytes to read 
// filp : KFILE pointer 
// Return : 
// actually read number. 0 = EOF, negative = error 

int inetctrl_fread(char *buf, int len, KFILE *filp) 
{ 
	int readlen; 
	mm_segment_t savefs; 
	
	if (filp == NULL) return -ENOENT; 
	if (filp->f_op->read == NULL) return -ENOSYS; 
	if (((filp->f_flags & O_ACCMODE) & O_RDONLY) != 0) return -EACCES; 
	
	savefs = get_fs();
	set_fs( KERNEL_DS ); 
	readlen = filp->f_op->read(filp, buf, len, &filp->f_pos); 
	set_fs( savefs ); 
	
	return readlen; 
} 

// Context : User 
// Parameter : 
// filp : KFILE pointer 
// Return : 
// read character, EOF if end of KFILE 

int inetctrl_fgetc(KFILE *filp) 

{ 
	int len; 
	unsigned char buf[4]; 
	
	len = inetctrl_fread((char *) buf, 1, filp); 
	
	if (len > 0)return buf[0]; 
	else if (len == 0)return EOF; 
	else return len; 
} 

// Context : User 
// Parameter : 
// str : string 
// size : size of str buffer 
// filp : KFILE pointer 
// Return : 
// read string. NULL if end of KFILE 
// Comment : 
// Always append trailing null character 

char *inetctrl_fgets(char *str, int size, KFILE *filp) 
{ 
	char *cp; 
	int len, readlen; 
	mm_segment_t savefs; 
	
	if ( filp && ((filp->f_flags & O_ACCMODE) & O_WRONLY) == 0) { 
		savefs = get_fs(); 
		set_fs(KERNEL_DS); 
		
		for (cp = str, len = -1, readlen = 0; readlen < size - 1; ++cp, ++readlen) { 
			if ((len = filp->f_op->read(filp, cp, 1, &filp->f_pos)) <= 0) 
				break; 
			if (*cp == '\n') { 
				++cp; 
				++readlen; 
				break; 
			} 
		} 
		
		*cp = 0; 
		set_fs( savefs ); 
		return (len < 0 || readlen == 0) ? NULL : str; 
	} else 
		return NULL; 
} 

// Context : User 
// Parameter : 
// buf : buffer containing data to write 
// len : number of bytes to write 
// filp : KFILE pointer 
// Return : 
// actually written number. 0 = retry, negative = error 

int inetctrl_fwrite(char *buf, int len, KFILE *filp) 
{ 
	int writelen; 
	mm_segment_t savefs; 
	
	if (filp == NULL) {
		return -ENOENT;
	}	
	if (filp->f_op->write == NULL) return -ENOSYS; 
	
	if (((filp->f_flags & O_ACCMODE) & (O_WRONLY | O_RDWR)) == 0) 
		return -EACCES; 
	
	savefs = get_fs(); 
	set_fs( KERNEL_DS ); 
	writelen = filp->f_op->write(filp, buf, len, &filp->f_pos); 
	set_fs( savefs ); 
	return writelen; 
} 

// Context : User 
// Parameter : 
// filp : KFILE pointer 
// Return : 
// written character, EOF if error 

int inetctrl_fputc(int ch, KFILE *filp) 
{ 
	int len; 
	unsigned char buf[4]; 
	
	buf[0] = (unsigned char) ch; 
	len = inetctrl_fwrite(buf, 1, filp); 
	
	if (len > 0)return buf[0]; 
	else return EOF; 
} 

// Context : User 
// Parameter : 
// str : string 
// filp : KFILE pointer 
// Return : 
// count of written characters. 0 = retry, negative = error 

int inetctrl_fputs(char *str, KFILE *filp) 
{ 
	return inetctrl_fwrite(str, strlen(str), filp); 
} 

// Context : User 
// Parameter : 
// filp : KFILE pointer 
// fmt : printf() style formatting string 
// Return :
// Comment:
// Save fmt information to s_buf that add up to 1024 bytes length
// same as inetctrl_fputs() 

int inetctrl_fprintf(KFILE *filp, const char *fmt, ...) 
{ 
	static char s_buf[LINEMAXLENTH]; 
	va_list args; 
	
	va_start(args, fmt); 
	vsprintf(s_buf, fmt, args); 
	va_end(args); 
	return inetctrl_fputs(s_buf, filp); 
} 

// Context : User 
// Parameter : 
// filp : KFILE pointer 
// fmt : scanf style formatting string 
// Comment:
// Scanf s_buf information into fmt that add up to 1024 bytes length
// same as inetctrl_fputs() 

void inetctrl_fscanf(KFILE *filp, const char *fmt, ...) 
{ 
	static char s_buf[LINEMAXLENTH]; 
	va_list args;
	
	if( inetctrl_fgets( s_buf, LINEMAXLENTH, filp ) == NULL )return;
	
	va_start(args, fmt); 
	sscanf(s_buf, fmt, args); 
	va_end(args); 
} 
// 
// My Printk Functions 
//

//#define  AOS_LOG_FILENAME	"/var/log/inetctrlMessager"

int inetctrl_file_printk(const char *logfile, const char *fmt, ...) 
{ 
	KFILE *inetctrl_file=NULL;
	static char s_buf[LINEMAXLENTH]; 
	va_list args; 
	
	if (( inetctrl_file = inetctrl_fopen( logfile, "a" )) == NULL ){
				//O_CREAT | O_WRONLY | O_APPEND,
				//0 )) == NULL) { 
				//S_IRUSR | S_IWUSR)) == NULL) { 
		printk("Can't open file \"%s\" !\n",logfile ); 
		return 1; 
	} 
	va_start(args, fmt); 
	vsprintf(s_buf, fmt, args); 
	va_end(args); 
	inetctrl_fputs( s_buf, inetctrl_file ); 

	inetctrl_fclose( inetctrl_file ); 

	return 1; 
} 

#endif


