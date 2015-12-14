//////////////////////////////////////////////////////////////////////////// //
// Copyright (C) 2010
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// Modification History:
// Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAgentUtil_Configuration_h
#define Aos_JimoAgentUtil_Configuration_h

#include <fstream>
#include <iostream>
#include <map>
using namespace std;

#define COMMENT_CHAR '#'

class AosConfiguration
{
public:
	AosConfiguration();
	~AosConfiguration();

	static bool IsSpace(char c);
	static bool IsCommentChar(char c);
	static void Trim(string &str);
	static bool AnalyseLine(const string &line, string &key, string &value);
	static bool ReadConfig(const string &filename, map<string, string> &m);
	static void PrintConfig(const map<string, string> &m);
};
#endif

