////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 11/16/2010: Copy by Brian
////////////////////////////////////////////////////////////////////////////
#include "ShortMsgUtil/ShortMsgUtil.h"

#include <stdio.h>
#include <string.h>  
#include <iconv.h>  
#include <stdlib.h>  
#include <iostream>
using namespace std;
 
const int  sgShortMsgStrMaxSize =1023;  


AosShortMsgUtil::AosShortMsgUtil()
{
}


AosShortMsgUtil::~AosShortMsgUtil()
{
}

	
void 
AosShortMsgUtil::ltrim(
		AosUCharPtr_t outbuff,
		AosUCharPtr_t inbuff) //截去左边的空格  
{  
	// This function trim the left side of inbuff.
    int l;  
    AosUCharPtr_t p = inbuff; 
 
    while(*p != '\0')  
    {  
        if(32!=(AosUChar_t)*p)break;  
        p++;  
    } 

    l = strlen((char *)inbuff) - (p - inbuff);  
    strncpy((char *)outbuff, (const char *)p, l); 
    outbuff[l]='\0';  
} 
 

void 
AosShortMsgUtil::rtrim(
		AosUCharPtr_t outbuff,
		AosUCharPtr_t inbuff)
{  
	// This function trim the right side of inbuff.
    AosUCharPtr_t p = inbuff;  
 
    p += (strlen((char *)inbuff) - 1);  
    while(p > inbuff)  
    {  
        if(32!=(AosUChar_t)*p)break;  
        p--;  
    }  
    strcpy((char *)outbuff, (const char *)inbuff);  
    outbuff[p-inbuff+1]='\0';  
}  


void 
AosShortMsgUtil::alltrim(
		AosUCharPtr_t outbuff,
		AosUCharPtr_t inbuff) //截去左右的空格  
{  
    ltrim(outbuff, inbuff);  
    rtrim(outbuff, inbuff);  
}  
 

void 
AosShortMsgUtil::substr(
		AosUCharPtr_t outbuff, 
		AosUCharPtr_t inbuff, 
		int start, 
		int length) //取子字符串  
{  
    if(start < 0) start = 0;  
    if(length >(int)(strlen((char *)inbuff) - start))
	{
		length = strlen((char *)inbuff) - start;
	}
    strncpy((char *)outbuff, (const char *)inbuff+start, (int)length);  
    outbuff[length]='\0';  
}  
 

int 
AosShortMsgUtil::instr(
		int start,
		AosUCharPtr_t inbuff,
		AosUCharPtr_t strfind)
{  
    AosUCharPtr_t p;  
	char * q;
	q = (char *)inbuff;
 
    if(strlen((char *)strfind)==1)  
    {  
        p=(AosUCharPtr_t)strchr((char*)inbuff+start, (int)strfind[0]);  
        if(p!=NULL)  
            return p-inbuff;  
        else 
            return -1;  
    }  
    else 
    {  
        p=(AosUCharPtr_t)strstr((char *)(inbuff+start),(char *)strfind);  
        if(p!=NULL)  
            return p-inbuff;  
        else 
            return -1;  
    }  
}  


void 
AosShortMsgUtil::replace(
		AosUCharPtr_t outbuff,
		AosUCharPtr_t inbuff,
		AosUCharPtr_t strfind,
		AosUCharPtr_t strreplace)
{  
    AosUChar_t buff[sgShortMsgStrMaxSize];  
    AosUCharPtr_t p;  
 
    sprintf((char *)buff, "%s%s", inbuff, strreplace);  
    p=(AosUCharPtr_t)strtok((char *)buff,(const char *)strfind);  
    outbuff[0]='\0';  
    while(p)  
    {  
        sprintf((char *)outbuff,"%s%s%s",outbuff,p,strreplace);  
        p=(AosUCharPtr_t)strtok(NULL,(const char *)strfind);  
    }  
    outbuff[strlen((char *)outbuff)-strlen((char *)strreplace)*2]='\0';  
}  
 

void 
AosShortMsgUtil::str2hex(
		AosUCharPtr_t outbuff,
		AosUCharPtr_t inbuff)  
{  
    int i;  
    unsigned long lASCII;  
    AosUChar_t szbuff[sgShortMsgStrMaxSize];  
    memset(szbuff, 0, sgShortMsgStrMaxSize);  
    
	for(i=0; i<(int)strlen((char *)inbuff); i++)  
    {  
        lASCII=0;  
        if(inbuff[i] < 0x80) //on BYTE of UTF8  
        {  
            lASCII = inbuff[i];  
            if(lASCII > 0xf)  
                sprintf((char *)szbuff, "%s00%2X", szbuff, (unsigned int)lASCII);  
            else 
                sprintf((char *)szbuff, "%s000%1X", szbuff, (unsigned int)lASCII);  
        }  
        else if((0xc0 <= inbuff[i]) && (inbuff[i] < 0xe0)) //tow BYTE of UTF8  
        {  
            lASCII = inbuff[i] & 0x1f;  
            lASCII = lASCII << 6;  
            lASCII = lASCII | (inbuff[i+1] & 0x3f);  
            sprintf((char *)szbuff, "%s%2X", szbuff, (unsigned int)lASCII);  
            i++;  
        }  
        else if((0xe0 <= inbuff[i]) && (inbuff[i] < 0xf0)) //three BYTE of UTF8  
        {  
            lASCII = inbuff[i] & 0x0f;  
            lASCII = lASCII << 6;  
            lASCII = lASCII | (inbuff[i+1] & 0x3f);  
            lASCII = lASCII <<6;  
            lASCII = lASCII | (inbuff[i+2] & 0x3f);  
            sprintf((char *)szbuff, "%s%2X", szbuff, (unsigned int)lASCII);  
            i+=2;  
        }  
        else //if((0x80 <=inbuff[i]) && (inbuff[i] < 0xc0)) //not the first byte of UTF8 BYTEacter  
        {  
            break;  
        }  
    }  
    strcpy((char *)outbuff, (const char *)szbuff); 
}  
 

void 
AosShortMsgUtil::hex2str(
		AosUCharPtr_t outbuff,
		AosUCharPtr_t inbuff) //将16进制转为字符串  
{  
    int i,iLen=strlen((char *)inbuff);  
    AosUChar_t szASCII[5];  
    AosUChar_t szbuff[iLen];  
    unsigned long lASCII; //4位  
 
    if(((iLen) % 4)!=0) //如果长度不为4的倍数，则退出  
    {  
        outbuff[0]='\0';  
        return;  
    }  
    szbuff[0]='\0';  
    for(i=0; i<iLen; i+=4)  
    {  
        szASCII[0] = inbuff[i];  
        szASCII[1] = inbuff[i+1];  
        szASCII[2] = inbuff[i+2];  
        szASCII[3] = inbuff[i+3];  
        szASCII[4] = '\0';  
        lASCII = strtol((char *)szASCII, '\0', 16);  
        if(lASCII <= 0x007f) // on BYTE of UTF8  
        {  
            szASCII[0] = (AosUChar_t)lASCII;  
            szASCII[1] = '\0';  
        }  
        else if(lASCII <= 0x07ff) // two BYTE of UTF8  
        {  
            szASCII[1] = 0x80 | (AosUChar_t)(lASCII & 0x003f);  
            szASCII[0] = 0xc0 | (AosUChar_t)((lASCII >> 6) & 0x001f);  
            szASCII[2] = '\0';  
        }  
        else // three BYTE of UTF8  
        {  
            szASCII[2] = 0x80 | (AosUChar_t)(lASCII & 0x003f);  
            szASCII[1] = 0x80 | (AosUChar_t)((lASCII >> 6) & 0x003f);  
            szASCII[0] = 0xe0 | (AosUChar_t)((lASCII >> 12) & 0x001f);  
            szASCII[3] = '\0';  
        }  
        strcat((char *)szbuff,(const char *)szASCII);  
    }  
    strcpy((char *)outbuff,(const char *)szbuff); 
}  
 

int
AosShortMsgUtil::strconv(
		AosUCharPtr_t outbuff,
		const AosUCharPtr_t inbuff,
		AosUCharPtr_t to_Charset,
		AosUCharPtr_t from_Charset) //字符集转换(实现各种编码)  
{  
    iconv_t it;  
    size_t inleft = strlen((char *)inbuff)+1;  
    size_t outleft = sgShortMsgStrMaxSize;  
    char buff[sgShortMsgStrMaxSize],*pbuff;  
 
    //打开需要转换的字符集  
    it = iconv_open((const char *)to_Charset, (const char *)from_Charset);  
    if((unsigned long)it==-1)  
        return -1;  
 
    //进行转换  
    pbuff=buff;  
    memset(buff, 0, sgShortMsgStrMaxSize);  
    if((int)iconv(it, (char**)&inbuff, &inleft, (char**)&pbuff, &outleft)== -1)  
        return -2;  
    else 
        strcpy((char *)outbuff, (const char *)buff);  
 
    //转换结束  
    iconv_close(it);  
 
    return sgShortMsgStrMaxSize-outleft;  
}

	
void 
AosShortMsgUtil::pduToTel(AosUCharPtr_t pSrcCode,AosUCharPtr_t pDstCode)
{ 
	// This function convert pdu to string of telephone.
    int i;  
    AosUChar_t szBuff[16]; //临时变量  
 
    memset(szBuff, 0, 16); 
    //奇偶对调  
    for(i=0; i<(int)strlen((char *)pSrcCode); i+=2)  
    {  
        szBuff[i] = pSrcCode[i+1];  
        szBuff[i+1] = pSrcCode[i];  
    }  
 
    //截去后缀字符F(如果有)  
    i = strlen((char *)szBuff);  
    if(szBuff[i-1]=='F')szBuff[i-1]='\0';  
 
    strcpy((char *)pDstCode, (const char *)szBuff); 
}

	
void 
AosShortMsgUtil::telToPdu(
		AosUCharPtr_t pSrcCode, 
		AosUCharPtr_t pDstCode, 
		AosUCharPtr_t pAreaCode)
{  
	// This function convert a string of telephone to pdu
    int i,l;  
    AosUChar_t szBuff[16];//临时变量  
 
    //根据是否有区号判断号码是否为小灵通  
    if(pSrcCode[0]=='0') //如果是,则加上106前缀  
        sprintf((char *)szBuff, "106%s", pSrcCode);   
    else 
    {  
        //根据长度是否小于11位判断是否为小灵通  
        l = strlen((char *)pSrcCode);  
        if(l<11) //如果长度小于11位,则为未加区号的小灵通  
            sprintf((char *)szBuff, "106%s%s", pAreaCode, pSrcCode);  
        else //否则为手机号码  
        {  
            if((pSrcCode[0]!='8')||(pSrcCode[1]!='6'))  
			{
				sprintf((char *)szBuff, "86%s", pSrcCode);  
			}
            else 
			{
				strcpy((char *)szBuff, (const char *)pSrcCode);
			}
        }  
    }  
    //判断加上前缀后的号码长度是否为奇数,如果是,则在最后补"F"字符  
    l = strlen((char *)szBuff)+1;  
    if((l % 2)==0)  
        sprintf((char *)szBuff, "%sF", szBuff);  
    //对号码进行PDU编码,即奇偶对调  
    for(i=0; i<l; i+=2)  
    {  
        pDstCode[i] = szBuff[i+1];  
        pDstCode[i+1] = szBuff[i];  
    }  
    pDstCode[l] = '\0';  
}  

