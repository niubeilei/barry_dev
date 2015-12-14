////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "Alarm/AlarmMgr.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h" 
#include "JQLParser/JQLParser.h"
#include "SQLDatabase/SqlIndex.h"
#include "NMS/HouseKp.h"
#include "NMS/Ptrs.h"
#include "Porting/GetTime.h"
#include "Rundata/Rundata.h"
#include "Random/RandomUtil.h"
#include "SingletonClass/SingletonMgr.h"    
#include "SEInterfaces/ValueObj.h"    
#include "SEServer/SeReqProc.h"    
//#include "Value/Value.h"    
#include "Timer/Timer.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/ExeReport.h"
#include "Util/MemoryChecker.h"
#include "Util/CompareFun.h"
#include "Util/BuffArray.h"
#include "XmlParser/XmlItem.h"
#include "GetLine/getline.h"
#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <iostream>
#include <fstream>
using namespace std;
  
extern int AosParseJQL(
				char *data, 
				const bool debug_flag); 
extern AosJQLParser gAosJQLParser;

char *welcome = \
	"*********************************************************************\n" 
	"*              Welcome to JimoSQL 0.1                             \n" 
	"*  Copyright (c) 2009-2014 Zykie Networks Inc. All rights reserved.\n"
	"*  Type \"?\" or \"help\" to list all the commands.\n"
	"************************************************************i********\n";
string helpFile = "help.txt";
char *prompt = "AOS> ";

map<string, string> map_help;

void initHelpMap(const string &filename);
string findname(const char *src, size_t n);
string filterSpecialChar(const char *src, size_t n);
OmnString printHelp(OmnString name);

int 
main(int argc, char **argv)
{
	OmnApp theApp(argc, argv);

	OmnAlarmMgr::config();

	int index = 1;
	AosXmlTagPtr app_config;
	bool debug_flag = false;
	char *buff;
	
	initHelpMap(helpFile);


	while (index < argc)
	{
		if (strcmp(argv[index], "-d") == 0)
		{
			debug_flag = true;
			index++;
			continue;
		}

		if (strcmp(argv[index], "-config") == 0)
		{
			// '-config fname'
			app_config = OmnApp::readConfig(argv[index+1]);
			OmnApp::setConfig(app_config);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-f") == 0)
		{
			// -f file
			OmnFile ff(argv[index+1], OmnFile::eReadOnly AosMemoryCheckerArgs);
			if (!ff.isGood())
			{
				cout << "*********** file not found: " << argv[index+1] << endl;
				exit(-1);
			}

			OmnString str;
			ff.readToString(str);
			vector<AosJqlStatementPtr> statements;
			//AosParseJQL((char *)str.data(), debug_flag, statements);
			AosParseJQL((char *)str.data(), debug_flag);

			OmnScreen << "Parsing finished" << endl;
			for (u32 i=0; i<statements.size(); i++)
			{
				//statements[i]->dump();
			}
		}

		index++;
	}

	cout << welcome << endl;
	while (1)
	{
		//char *buff[1000];
		//cin.getline(buff,1000);
		buff = getlineplus(prompt, "");
		gl_histadd(buff);

		if (strncmp(buff, "exit", 4) == 0) exit(0);

		if (strncmp(buff, "help", 4) == 0 || buff[0] == '?') 
		{
			OmnString helpcmd(&buff[5]);
			OmnString helpinfo = printHelp(helpcmd);
			if(helpinfo == "")
			{
				cout << "Nothing found" << endl;
			}
			else
			{
				cout << helpinfo << endl;
			}
			continue;
		}

		vector<AosJqlStatementPtr> statements;
		//AosParseJQL(buff, debug_flag, statements);
		//cout << buff << endl;
		AosParseJQL(buff, debug_flag);
		for (u32 i=0; i<statements.size(); i++)
		{
			//statements[i]->dump();
		}
		gAosJQLParser.run(0);
		cout << endl;
	}
	return 0;
} 

void initHelpMap(const string &filename)
{
	string a[100]; 
    int i=0;
    ifstream infile; 
    infile.open(filename.data(), ios::in); 
	string text;
	string name;
    while(!infile.eof())
    {
    	getline(infile, a[i], '\n');
		if (strncmp(a[i].data(), "Name:", 5) == 0) 
		{
			name = findname(a[i].data(), a[i].length());
			text = a[i] + "\n";
		}
		else
		{
			text += a[i] + "\n";
		}
		if(name.length() > 0 )
		{
			map_help[name] = text;
		}	
      	i++;
   	} 
}

string
findname(const char *src, size_t n)
{
    size_t i;
	string dest;
	int start = -1;
	int end = -1;
    for (i = 0; i < n && src[i] != '\0'; i++)
	{
		if(start == -1 && src[i] == '\'')
		{
			i++;
			start = i;
		}
		if(start != -1 && src[i] == '\'')
		{
			end = i;
			break;
		}
		if(start != -1 && end == -1)
		{
    		dest += src[i];
		}
	}
    return dest;
}

string 
filterSpecialChar(const char *src, size_t n)
{
    size_t i;
	string dest;
	bool startwhite = true;
    for (i = 0; i < n && src[i] != '\0'; i++)
	{
		if( src[i] == '\t' || src[i] == '\0' ||
			src[i] == '\n' || src[i] == '\r')
		{
			continue;
		}
		if(src[i] == ' ')
		{
			if(!startwhite && i != n-1)
			{
				dest += ' ';
				startwhite = true;
			}
		}
		else
		{
			startwhite = false;
    		dest += src[i];
		}
	}
	if(dest.substr(dest.length()-1, 1) == " ")
	{
		dest.erase(dest.length()-1, 1);
	}
	if(dest.substr(dest.length()-1, 1) == ";")
	{
		dest.erase(dest.length()-1, 1);
		if( dest.substr(dest.length()-1, 1) == " ")
		{
			dest.erase(dest.length()-1, 1);
		}
	}
    return dest;
}

OmnString printHelp(OmnString name)
{
	OmnString upper_str = name.toUpper();
	string search = filterSpecialChar(upper_str.data(), upper_str.length());
	map<string, string>::iterator itr = map_help.find(search);
	if (itr != map_help.end())
	{
		return (*itr).second;
	}
	return "";
}

