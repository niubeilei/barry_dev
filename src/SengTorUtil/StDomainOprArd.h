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
// 01/18/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AosSengTorUtil_StDomainOprArd_h
#define AosSengTorUtil_StDomainOprArd_h

#include "Rundata/Ptrs.h"
#include "SEBase/SecReq.h"
#include "SengTorturer/Ptrs.h"
#include "SengTorUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/HashUtil.h"


class AosStDomainOprArd : virtual public OmnRCObject
{
public:
	static AosXmlTagPtr createDomainOprArd(
				const OmnString &objid,
				const AosSengTestThrdPtr &thread);
	static AosXmlTagPtr modifyDomainOprArd(
				const AosXmlTagPtr &doc,
				const AosSengTestThrdPtr &thread);
	static bool checkAccess(
				const AosXmlTagPtr &domain_oprard,
				const AosStDocPtr &local_doc,
				const OmnString &operation,
				bool &graned,
				bool &denied, 
				const AosSengTestThrdPtr &thread);
};
#endif

