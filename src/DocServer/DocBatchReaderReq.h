////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 04/07/2013 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_DocServer_DocBatchReaderReq_h
#define AOS_DocServer_DocBatchReaderReq_h

#include "Rundata/Rundata.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/XmlDoc.h"


struct AosDocBatchReaderReq : public OmnRCObject  
{                                     
private:                              
	OmnDefineRCObject;                

public:                               
	enum E 
	{
		eInvalid,
		eStart,
		eFinished,
		eData,
		eStartRead,
		eMax
	};


	E			 			type;
	OmnString 				scannerid;
	AosBitmapObjPtr    		bitmap;      
	u32 					client_id;
	u32						queue_size;
	u64 					block_size;
	int						total_num_data_msgs;
	OmnString				read_policy_str;
	OmnString				batch_type_str;
	AosXmlTagPtr			field_names;
	AosRundataPtr       	rdata;     
	bool					call_back;
	int						call_back_serverid;
};                                    


#endif

#endif
