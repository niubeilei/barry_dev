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
#ifndef Aos_SmartDoc_SdocExport_h
#define Aos_SmartDoc_SdocExport_h

#include "SEInterfaces/QueryRsltObj.h"
#include "SmartDoc/SmartDoc.h"

#include <vector>
using namespace std;


class AosSdocExport : public AosSmartDoc
{
	OmnString 			mFilePath;
	vector<OmnString>	mHeader;
	vector<OmnString>	mHeaderBd;

public:
	AosSdocExport(const bool flag);
	~AosSdocExport();

	// Smartdoc Interface
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

	virtual AosSmartDocObjPtr clone(){return OmnNew AosSdocExport(false);}
private:
	AosQueryRsltObjPtr 	queryData(
							AosXmlTagPtr &qry_xml,
							const AosRundataPtr &rdata);
/*	bool 				exportToFile(
							const vector<OmnString> &objids,
							OmnFilePtr &mDataFile,
							const AosRundataPtr &rdata);
							*/
	bool 				exportToFile(
							const AosQueryRsltObjPtr &qrslt,
							OmnFilePtr &mDataFile,
							const AosRundataPtr &rdata);
	bool 				composeFileName(OmnString &fname, OmnString &objid);

};
#endif
