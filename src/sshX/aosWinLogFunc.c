///////////////////////////////////////////////////////////////////////
// Author: Pownall.Zhang
// Creation Time: 10/27/2006
// Description:
// * File only for windows sshX program test using.
//
// Change History
// 10/27/2006 aosWinLogFunc.c created
///////////////////////////////////////////////////////////////////////

#include "aosWinLogFunc.h"
#include "putty.h"
#include "terminal.h"
#include "storage.h"
#include "win_res.h"

#include <imm.h>
#include <commctrl.h>
#include <richedit.h>
#include <mmsystem.h>

static KFILE *ssh_bin_filp = NULL;
u64 session_id=0;
char system_hostname[]="ZQLtestSSHX1.0";

//aos time struct
struct aos_tm
{  
    int tm_sec;  
    int tm_min;  
    int tm_hour;  
    int tm_day;  
    int tm_mon;  
    int tm_year;  
};

void aos_GetCurrentTime( struct aos_tm *curtm, u32 seconds )
{
	u32 days;
	int leap_y_count;
	u32 months[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

    curtm->tm_sec  = seconds%60;	//get second
	seconds/= 60;
	curtm->tm_min  =  seconds%60;  //get minute
	seconds/= 60;
	curtm->tm_hour = (seconds+17)%24;	//get hour +17 ????
	days    = seconds / 24;	//get tatol days
	//how many leap year(4 years for one leap year)
	leap_y_count = (days+365)/1461;

	if( ((days+366)%1461) == 0) //the last of leap year
	{	
		curtm->tm_year = (days/366)+1970;//get year
		curtm->tm_mon  = 12;              //modfiy month
		curtm->tm_day  = 31;
		return;
	}
	
	days -= leap_y_count;
	curtm->tm_year = (days/365)+1970;   //get year
	days %= 365;                //the day of this year
	days = 1 + days;
	if( (curtm->tm_year%4) == 0 )
	{
		if(days > 60)--days;    //modfiy leap year
		else {
			if(days == 60)
			{
				curtm->tm_mon = 2;
				curtm->tm_day = 29;
				return;
			}
		}
	}
	for( curtm->tm_mon = 0; months[ curtm->tm_mon ] < days; curtm->tm_mon++)
	{
		days -= months[ curtm->tm_mon ];
	}
	++curtm->tm_mon;              //get month
	curtm->tm_day = days;         //get day
	
}

char * aos_LogGetTimeStr(u32 secs)
{
	static char buf[64];
	struct aos_tm curtm;
	
	aos_GetCurrentTime( &curtm, secs );
	
	sprintf(buf,"%d-%02d-%02d %02d:%02d:%02d",\
			curtm.tm_year, curtm.tm_mon, curtm.tm_day,\
			curtm.tm_hour, curtm.tm_min, curtm.tm_sec );
	return buf;
}

char *aos_LogGetSessionID( u64 sid )
{
	static char buf[96];

	sprintf( buf, "%d%05d", (u32)(sid>>32), sid );
	
	return buf;
}

//
// This function used to get current time
// The caller must allocate more than 20 bytes buffer space for this function.
//
//void inline aos_LogGetTimeStr(char *buf)
//void inline aos_LogGetTimeStr(char *buf)
u64 get_seconds()
{
	time_t ltime;

    /* Set time zone from TZ environment variable. If TZ is not set,
     * the operating system is queried to obtain the default value 
     * for the variable. 
     */
    //_tzset();

    /* Get UNIX-style time and display as number and string. */
    time( &ltime );
	
	return (u64)(ltime+3600l*8);
}

KFILE *aos_openvsfile( KFILE *filp, char *fname_fmt )
{
	char	filename[128];
	struct	tm *today;
	time_t  ltime;
	
	if( filp != NULL )fclose( filp );

	/* Set time zone from TZ environment variable. If TZ is not set,
     * the operating system is queried to obtain the default value 
     * for the variable. 
     */
    _tzset();
	
	/* Get UNIX-style time and display as number and string. */
    time( &ltime );

	// build file name according fname_fmt and current date time.
	/* Use time structure to build a customized time string. */
    today = localtime( &ltime );

    /* Use strftime to build a customized time string. */
    strftime( filename, 128, fname_fmt, today );

	if (( filp = fopen( filename, "ab" )) == NULL )
	{ 
		char msgstr[256];
		sprintf(msgstr,"Aos_openvsfile \"%s\" error !\n",\
			filename );
		MessageBox(NULL, "Open file error",
		   msgstr, MB_OK | MB_ICONEXCLAMATION);
		exit(0);
	}
	return filp;
}

void  write_ssh_bin_data(u8 *app_data, u32 app_data_len, u8 direct )
{
	BINDATAHEADER dataHeader;
	char	*cmd1[2]={"-CMD","-RSP"};
	int wlen=0,i=0;
	
	///tmp
	//app_data_len = strlen( system_hostname );
	//
	if( !ssh_bin_filp )
		ssh_bin_filp = aos_openvsfile( ssh_bin_filp, SSH_BIN_LOGNAME );

	if( !session_id )
		session_id = ((u64)get_seconds()<<32) | 0x750803;
	//conn->csock->ops->getname(conn->csock,(struct sockaddr*)&client_addr,&len,1);
	//conn->dsock->ops->getname(conn->dsock,(struct sockaddr*)&server_addr,&len,0);
	memset(&dataHeader, 0, sizeof(BINDATAHEADER) );
	dataHeader.direct       = direct;
	dataHeader.timeSec      = (u32)get_seconds();
	dataHeader.sessionId    = session_id;
	dataHeader.clientIP     = 0xC0A80801;//ntohl(client_addr.sin_addr.s_addr);
	dataHeader.destIP       = 0xC0A80803;//ntohl(server_addr.sin_addr.s_addr);
	dataHeader.hostnameLen  = (u8)strlen( system_hostname );
	dataHeader.dataLen		= (u32)app_data_len;

	/*{
		FILE *tfp=NULL;
		tfp=fopen("log\\log.txt","ab");
		if(tfp){
			fprintf(tfp,"%s %s %s kernel: SSH%s (%d,%d,[%d,%d(ftell)]).\n",
				aos_LogGetTimeStr( dataHeader.timeSec ),\
				system_hostname,\
				aos_LogGetSessionID( dataHeader.sessionId ),\
				cmd1[ dataHeader.direct ],\
				sizeof(BINDATAHEADER),\
				dataHeader.hostnameLen,\
				app_data_len,\
				ftell(ssh_bin_filp));
				
			fprintf(tfp,"|===> %d <=|\n",app_data_len);
			for( i=0; i<app_data_len; i++ )
			{
				fprintf(tfp,"%02x ",app_data[i]&0xff);
				if(((i+1)%16==0))fprintf(tfp,"\n");
			}
			fprintf(tfp,"\n");
			//aos_fwrite( app_data, app_data_len, tfp );
			fclose(tfp);
		}
	}*/

	//write dataHeader¡¢hostname and packet data in turns
	fwrite( (char *)&dataHeader, sizeof(BINDATAHEADER), 1, ssh_bin_filp );
	fwrite( system_hostname, 1, dataHeader.hostnameLen, ssh_bin_filp );
	wlen=fwrite( app_data, 1, dataHeader.dataLen, ssh_bin_filp );
	
	fclose(ssh_bin_filp);
	ssh_bin_filp=NULL;
}
