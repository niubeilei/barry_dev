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
#ifndef Aos_SEServerCgi_FcgiReqProc_h
#define Aos_SEServerCgi_FcgiReqProc_h

#include "SEServer/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Proggie/ReqDistr/NetReqProc.h"
#include <vector>

#include "fcgio.h"
#include "fcgi_config.h"  
#include "fcgiapp.h"
#include <sys/types.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

using namespace std;


class AosFcgiReqProc : virtual public OmnRCObject
{
	OmnDefineRCObject;

protected:
	AosNetReqProcPtr mReqProc;
	//void  		decodeUrl(char* data);
	char*  		decodeUrl(const char* data);
	char*		dataFindSeq(char* start, char* end,const char* sep);
	bool		readData(FCGX_Request &request, char ** content, int &dataLength);
	bool	    upload(char* data, int &dataLength, char* content_type);
	bool		download(FCGX_Request &request, OmnString downloadPath);
	char 		CharToInt(char ch);
	bool        getRequest(FCGX_Request &request, char* data, OmnString &trans_id, OmnString &seRequest);

public:
	AosFcgiReqProc(const AosNetReqProcPtr &req);
	~AosFcgiReqProc();

    virtual bool procRequest(FCGX_Request &request) = 0;
};
#endif

