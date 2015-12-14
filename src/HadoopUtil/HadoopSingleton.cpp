////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HadoopSingleton.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "HadoopUtil/HadoopSingleton.h"

HadoopSingleton::HadoopSingleton()
:
isStarted(false)
{

}


HadoopSingleton::HadoopSingleton(const HadoopSingleton&){

}


HadoopSingleton& HadoopSingleton::operator=(const HadoopSingleton&){

}


HadoopSingleton* HadoopSingleton::instance = new HadoopSingleton();
HadoopSingleton* HadoopSingleton::getInstance(){
	return instance;
}

bool HadoopSingleton::init(string filename, string ip, int port, int64_t buffsize)
{	
	mFileName = filename;
	mIP = ip;
	mPort = port;
	mBlockSize = buffsize;
	if(!isStarted)
	{
		isStarted = true;
		connect();
		openFile();
	}
	return true;
}

int64_t HadoopSingleton::readFile(const int64_t start_pos, const int64_t bytes_to_read, char * buff)
{
	return hdfsPread(mFS, mFile, start_pos, (void*)buff, bytes_to_read); 
}

int HadoopSingleton::getLength()
{
	return hdfsAvailable(mFS, mFile);  
}

bool HadoopSingleton::connect()
{
	mFS = hdfsConnect("192.168.99.206", 9002);
	if (!mFS) {
    		std::cout <<  "Failed to connect to hdfs!\n " << endl;
    		return false;
	}
	return true;
}

bool HadoopSingleton::openFile()
{
	mFile = hdfsOpenFile(mFS, mFileName.data(), O_RDONLY, mBlockSize, 0, 0);
	if (!mFile) {
    		std::cout <<  "Failed to open this file!\n " << endl;
    		return false;
	}
	return true;
}
