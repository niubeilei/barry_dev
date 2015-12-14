////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// Modification History:
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Jimold_JimoldReqProc_h
#define Aos_Jimold_JimoldReqProc_h

#include "Proggie/ReqDistr/NetReqProc.h"
#include "Proggie/ProggieUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/XmlTag.h"
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <grp.h>
#include <pwd.h>
#include <unistd.h>
#include <stdio.h>

using namespace std;
using namespace boost::filesystem;


struct FILEINFO
{
	string clientfile;
	string client_filename;
	string server_file;
    string server_dir;
    string file;
	int    filesize;
	string file_buf;
	boost::shared_ptr<char> filebuf;
};      
class AosJimoldReqProc : public AosNetReqProc
{
	private:
		struct FILEINFO FileInfo;

	OmnDefineRCObject;
public:
	AosJimoldReqProc();
	~AosJimoldReqProc();

	AosNetReqProcPtr	clone();
	map<string, string> getInfo(char *data);
    map<string,string> splitStr(string s);
	bool writeToFile(const string fileName,const string fileInfo);
	bool checkCluster(const string &cluster_name);
	bool getDirectory();
	void receiveInfo(const OmnTcpClientPtr &client);
	virtual bool procRequest(const OmnConnBuffPtr &buff);
	static bool config(const AosXmlTagPtr &config);
	bool procCommand(const OmnString &command, const OmnTcpClientPtr &client);
	bool procStart(OmnString &resp, const OmnString &args);
	string getfilebuf(char *data);
	string getBasedir(char *data);
	bool recordFile(char *data);
};
#endif

