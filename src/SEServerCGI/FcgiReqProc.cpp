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
//
// Modification History:
// 10/16/2010: Created by Ketty
////////////////////////////////////////////////////////////////////////////
#include "SEServerCGI/FcgiReqProc.h"


#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"

#include "SEServerCGI/FcgiConn.h"
#include "SEServer/SeReqProc.h"
#include "Util/OmnNew.h"
#include "UtilComm/ConnBuff.h"

#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Util/File.h"
#include "Util/String.h"

#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "sys/stat.h"
#include "UtilComm/TcpClient.h"

AosFcgiReqProc::AosFcgiReqProc(const AosNetReqProcPtr &req)
:
mReqProc(req)
{
}


AosFcgiReqProc::~AosFcgiReqProc()
{
}


char 
AosFcgiReqProc::CharToInt(char ch)
{
	if(ch>='0' && ch<='9')return (char)(ch-'0');
	if(ch>='a' && ch<='f')return (char)(ch-'a'+10);
	if(ch>='A' && ch<='F')return (char)(ch-'A'+10);
	return -1;
}


char* 
AosFcgiReqProc::decodeUrl(const char* data)
{
	int i=0, j=0, len = strlen(data);
	char * newData = OmnNew char[len];

	while(i<len)
	{
		if(data[i] == '%')
		{
			char tempWord[2];
			char chn;
			tempWord[0] = CharToInt(data[i+1]);
			tempWord[1] = CharToInt(data[i+2]);
			chn = (tempWord[0] << 4) | tempWord[1];
			newData[j] = chn;
			j++;
			i += 3;
		}
		else if(data[i] == '+')
		{
			newData[j] = ' ';
			i++;
			j++;
		}
		else
		{
			newData[j] = data[i];
			i++;
			j++;
		}
	}
	newData[j] = '\0';
	//strcpy(data, newData);
	//delete []newData;
	return newData;
}


char*
AosFcgiReqProc::dataFindSeq(char* start, char* end,const char* sep)
{
	int sepLength = strlen(sep);
	
	for(; start < end; start++)
	{
		char* index = start;
		int sindex;
		for(sindex=0; sindex < sepLength; sindex++)
		{
			if(*index != sep[sindex]) break;
			
			index ++;
		}
		
		if(sindex == sepLength) //find
		{
			return start;
		}
	}
	
	return 0;
}

bool
AosFcgiReqProc::readData(FCGX_Request &request, char ** content, int &dataLength)
{
	char* method = FCGX_GetParam("REQUEST_METHOD", request.envp);

	if(strcmp(method, "POST") == 0 ) //method post
	{
		char* clenstr = FCGX_GetParam("CONTENT_LENGTH", request.envp);
		
		if(!clenstr) return false;

        int clen = strtol(clenstr, &clenstr, 10);
		aos_assert_r(clen >= 0, false);
		dataLength = clen + 1;

		*content = OmnNew char[clen+1];

		FCGX_GetStr(*content, clen, request.in);
		(*content)[clen] = '\0';
	}
	else if(strcmp(method, "GET") == 0) //method get
	{
    	char* querystr = FCGX_GetParam("QUERY_STRING", request.envp);
		if(querystr && querystr[0] != 0) //query中有传数值
		{
			*content = querystr;
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool
AosFcgiReqProc::upload(char* data, int &dataLength, char* content_type)
{
	 
	OmnStrParser parser(content_type, "; ");
	
	if(parser.nextWord() != "multipart/form-data") return false;
	
	OmnString boundaryNV = parser.nextWord();
		
	//获得消息分隔标记
	OmnStrParser boundaryParser(boundaryNV, "=");
	OmnString bName = boundaryParser.nextWord();
	OmnString bValue = "--";  //实际分隔符要多两个--
	bValue << boundaryParser.nextWord();
		
	const char* sep = bValue.data();
	int sepLength = strlen(sep);
		
	char* subDataStart=&data[0];
	char* subDataEnd=&data[0];

	for(int index= sepLength; index < dataLength - sepLength -2; index++)
	//数据是以分隔符开始的,所以从第一个分隔符后开始查找,结束分隔符又会多两个--
	{
		subDataStart = subDataEnd + sepLength;
		int dindex = index;
		int sindex;
		for(sindex=0; sindex < sepLength; sindex++)
		{
			if(data[dindex] != sep[sindex]) break;
			
			dindex ++;
		}
		
		if(sindex < sepLength) continue;
		
		//find the sep, index point to the begin sep
		subDataEnd = &data[index];
	
		const char* valueSep = "\r\n\r\n";
		char* valueStart = dataFindSeq(subDataStart, subDataEnd, valueSep);
		
		if (!valueStart) return false;

		//get the fileName
		const char* fileNameSep = "filename=\"";
		char* fileNameStart = dataFindSeq(subDataStart, valueStart, fileNameSep);
		if(!fileNameStart) return false;
		fileNameStart += strlen(fileNameSep);
		char* fileNameEnd = dataFindSeq(fileNameStart, valueStart, "\"");
		int fileNameLen = abs(fileNameEnd - fileNameStart);
		char fileName[fileNameLen+1];
		memset(fileName, 0, fileNameLen+1);
		for(int i=0;fileNameStart < fileNameEnd; fileNameStart++,i++)
		{
			fileName[i] = *fileNameStart;
		}
		fileName[fileNameLen] = '\0';

		if(fileName[0])
		{
			char* fileStart, *fileEnd;
			fileStart = valueStart + strlen(valueSep);
			fileEnd = subDataEnd - 2;   //subDataEnd前面会有\r\n
			
			int fileLength = abs(fileEnd - fileStart);
			
			OmnFilePtr ff = OmnNew OmnFile(fileName, OmnFile::eCreate);
			ff->put(0, fileStart, fileLength, true);
			return true;
		}
	}
	return false;
}

bool
AosFcgiReqProc::download(FCGX_Request &request, OmnString downloadPath)
{
	OmnFilePtr ff = OmnNew OmnFile(downloadPath, OmnFile::eReadOnly);

	OmnString fileName = ff->getFileName();
	u64 fileLength = ff->getLength();
	u64 fileStart = 0;

	char* data = 0;
	ff->readToBuff(fileStart,(u32)fileLength, data);

	FCGX_FPrintF(request.out, "Content-type: application/octet-stream\r\n");
	FCGX_FPrintF(request.out, "Content-Disposition: attachment; filename=%s\r\n", fileName.data());
				
	FCGX_FPrintF(request.out, "\r\n");
	FCGX_FPrintF(request.out, "%s", data);
				
	FCGX_Finish_r(&request);

	return true;
}

bool
AosFcgiReqProc::getRequest(FCGX_Request &request, char* data, OmnString &trans_id, OmnString &seRequest)
{
	aos_assert_r(data, false);
	OmnString content = data;
	seRequest << "<request>";

	OmnStrParser parser(content, "&");
	OmnString param;

	while( (param = parser.nextWord()) !="" )
	{
		OmnStrParser subParser(param, "=");
		OmnString name = subParser.nextWord();
		OmnString value = subParser.nextWord();
			
		//解码
		char *cname = decodeUrl(name.data());
		name = OmnString(cname);
		delete cname;
		char *cvalue = decodeUrl(value.data());
		value = OmnString(cvalue);
		delete cvalue;
		
		//name = decodeUrl(name.data());
		//value = decodeUrl(value.data());
			
		if(name == "trans_id")
		{
			trans_id = value;
			continue;
		}

		if(name == "command" || name == "objdef" || name == "xmlobj")
		{
			seRequest << "<" << name << ">" << value << "</" << name << ">";
		}
		else
		{
			seRequest << "<item name=\"" << name << "\"><![CDATA[" << value << "]]></item>";
		}
	}
	
	//get Cookie
	OmnString cookies = FCGX_GetParam("HTTP_COOKIE", request.envp);		
	if(cookies)
	{
		seRequest << "<zky_cookies>";

		OmnStrParser parser(cookies, ";");
		OmnString subcookie;

		while( (subcookie = parser.nextWord()) !="" )
		{
			OmnStrParser subparser(subcookie, "=");
			OmnString cookieName = subparser.nextWord();
			OmnString cookieValue = subparser.nextWord();
			seRequest << "<cookie zky_name=\"" << cookieName << "\"><![CDATA[" << cookieValue << "]]></cookie>"; 
		}

		seRequest << "</zky_cookies>";
	}

	seRequest << "</request>";

	return true;
}

