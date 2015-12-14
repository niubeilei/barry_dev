////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 05/27/2011	Created by Wynn
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocTXTImport_h
#define Aos_SmartDoc_SdocTXTImport_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"
#include <vector>
#include <map>
using namespace std;

class AosSdocTXTImport : public AosSmartDoc
{
	struct AosColumn
	{
		OmnString type;
		OmnString attrname;
	};

private:
	typedef vector<AosColumn> 		AosColumnVect;
	AosColumnVect					mColumns;
public:
	AosSdocTXTImport(const bool flag);
	~AosSdocTXTImport();

	// Smartdoc Interface
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

	virtual AosSmartDocObjPtr clone(){return OmnNew AosSdocTXTImport(false);} 

private:
	bool createRecord(
		const AosXmlTagPtr &sdoc,
		vector<AosColumn> &columns,
		const OmnString &buff,
		AosXmlTagPtr &recordtag,
		const AosRundataPtr &rdata);
	bool createXml(
		const AosXmlTagPtr &sdoc,
		const AosXmlTagPtr &doc,
		const bool ispublic,
		const AosRundataPtr &rdata);
};
#endif
