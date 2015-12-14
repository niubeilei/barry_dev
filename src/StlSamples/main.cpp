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
// 01/03/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
// #include "Tracer/TraceEntry.h"
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <dirent.h>
#include <map>

using namespace std;

struct ltstr
{
	bool operator()(const char* s1, const char* s2) const
	{
		return strcmp(s1, s2) < 0;
	}
};

int main()
{
	map<const char*, int, ltstr> months;
	    
	months["january"] = 31;
	months["february"] = 28;
	months["march"] = 31;
	months["april"] = 30;
	months["may"] = 31;
	months["june"] = 30;
	months["july"] = 31;
	months["august"] = 31;
	months["september"] = 30;
	months["october"] = 31;
	months["november"] = 30;
	months["december"] = 31;
	
	cout << "june -> " << months["june"] << endl;
	cout << "notfound -> " << months["abc"] << endl;
	map<const char*, int, ltstr>::iterator cur  = months.find("june");
	map<const char*, int, ltstr>::iterator prev = cur;
	map<const char*, int, ltstr>::iterator next = cur;    
	++next;
	--prev;
	cout << "Previous (in alphabetical order) is " << (*prev).first << endl;
	cout << "Next (in alphabetical order) is " << (*next).first << endl;
	return 0;
}

