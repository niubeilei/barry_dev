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
// 03/28/2011: Created by Phnix
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CodeComp_h
#define Aos_CodeComp_h

#include "Rundata/Ptrs.h"
#include "Security/Session.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include <vector>
#include <map>

using namespace std;


class AosCodeComp
{
private:
	struct DoubleVpdName 
	{
		OmnString oldVpdname;
		OmnString newVpdname;
		bool flag;
	};

	OmnString 					mObjidResolveType;
	OmnString 					mStrAdd;
	OmnString					mTags;
	OmnString					mContainer;
	vector<DoubleVpdName> 		mVpdsList;

public:
	AosCodeComp();
	~AosCodeComp();

	bool copyVpd(
			const AosRundataPtr &rdata,
			const OmnString &objid,
			const OmnString &fix_type,
			const OmnString &stradd, 
			const OmnString &tags, 
			const OmnString &container); 

private:
	bool retrieveMemberVpds(const AosXmlTagPtr &vpd, const AosRundataPtr &rdata);
	bool procCompoundPane(const AosXmlTagPtr &compoundPane, const AosRundataPtr &rdata);
	bool procPane(const AosXmlTagPtr &pane, const AosRundataPtr &rdata);
	bool procPanel(const AosXmlTagPtr &panel, const AosRundataPtr &rdata);
	bool procGic(const AosXmlTagPtr &gic, const OmnString &gic_type, const AosRundataPtr &);
	bool procAction(const AosXmlTagPtr &act, const OmnString &gtype, const AosRundataPtr &);
	bool doesVpdnameExist(DoubleVpdName &doubleVpdname);
	bool addToVpdList(DoubleVpdName &doubleVpdname, const AosRundataPtr &rdata);
	bool checkList();
	bool prepareVpd(const AosXmlTagPtr &vpd);

	//felicia, 2011-11-26, for htmlcomposer action
	void procComposerAction(
		    OmnString &cont,
		    const OmnString &gic_type,
		    const AosRundataPtr &rdata);
};

#endif

