////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HadoopSingleton.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_HadoopSingleton_h
#define Omn_HadoopSingleton_h

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "HDFS/hdfs.h"
using namespace std;

class HadoopSingleton{
private:
	bool		isStarted;
	hdfsFile	mFile;
	hdfsFS		mFS;
	string		mIP;
	int		mPort;
	string		mFileName;
	int64_t		mBlockSize;
public:
	HadoopSingleton();
	static HadoopSingleton* getInstance();
	int64_t readFile(const int64_t start_pos, const int64_t bytes_to_read, char * buff);
	int getLength();
	bool init(string filename, string ip, int port, int64_t buffsize);

private:
	HadoopSingleton(const HadoopSingleton&);
	HadoopSingleton& operator=(const HadoopSingleton&);
	bool connect();
	bool openFile();

	static HadoopSingleton* instance;
};

#endif
