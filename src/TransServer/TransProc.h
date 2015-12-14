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
// 2011/01/22	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TransServer_TransProc_h
#define Omn_TransServer_TransProc_h

#include "Rundata/Rundata.h"
#include "TransServer/Ptrs.h"
#include "TransBasic/Trans.h"
#include "TransUtil/Ptrs.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"



class AosTransProc : virtual public OmnRCObject
{
public:
	//virtual bool proc(
	//				const AosXmlTransPtr &trans, 
	//				const AosXmlTagPtr &trans_doc,
	//				const AosRundataPtr &rdata) = 0;
	//virtual bool proc(const AosTinyTransPtr &trans, const AosRundataPtr &rdata) = 0;
	//virtual bool proc(const AosBuffTransPtr &trans, const AosRundataPtr &rdata) = 0;
	
	//virtual bool proc(const AosIILTransPtr &trans, const AosRundataPtr &rdata) = 0;
	//virtual bool proc(const AosBigTransPtr &trans, 
	//				const AosBuffPtr &content,
	//				const AosRundataPtr &rdata) = 0;
};
#endif

