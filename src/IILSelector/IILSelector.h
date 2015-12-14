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
// 08/07/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IILSelector_IILSelector_h
#define Aos_IILSelector_IILSelector_h

#include "IILSelector/Ptrs.h"
#include "IILSelector/IILSelectorIds.h"
#include "IILSelector/IILSelectorNames.h"
#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"



class AosIILSelector : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	AosIILSelector();

	static bool		 resolveIIL(
						const AosXmlTagPtr &def, 
						OmnString &iilname, 
						const AosRundataPtr &rdata);

	static bool	resolveIIL(
				const AosXmlTagPtr &term,
				u64 &iilid,
				const AosRundataPtr &rdata);

	// static OmnString getIILName(
	// 			const OmnString &type, 
	// 			const OmnString &parm1, 
	// 			const OmnString &parm2, 
	// 			const AosRundataPtr &rdata);
private:
	bool registerIILSelector(
			const AosIILSelectorPtr &selector, 
			OmnString &errmsg);

	static bool selectLogIIL(
			OmnString &iilname,
			const AosXmlTagPtr &selector_tag,
			const AosRundataPtr &rdata);
};
#endif

