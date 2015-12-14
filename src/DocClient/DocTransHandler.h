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
// 2014/11/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocClient_DocTransHandler_h
#define AOS_DocClient_DocTransHandler_h

#include "IdGen/Ptrs.h"
#include "IdGen/U64IdGen.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/SeTypes.h"


class AosDocClient;

class AosDocTransHandler : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	AosDocTransHandler();
	~AosDocTransHandler();

	bool createDoc( 	AosRundata *rdata, 
						const u64 userid,
						AosDocClient *doc_client, 
						const AosXmlTagPtr &root, 
						const AosXmlTagPtr &newdoc, 
						const u64 docid);

	bool createDoc( 	AosRundata *rdata, 
						AosDocClient *doc_client, 
						const AosXmlTagPtr &root, 
						const AosXmlTagPtr &newdoc, 
						const bool save_doc_flag,
						const u64 docid);
};
#endif
