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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SDocExportData_h
#define Aos_SmartDoc_SDocExportData_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Actions/ActAddAttr.h"
#include "Actions/SdocAction.h"
#include "XmlUtil/XmlTag.h"
#include <vector>

class AosSdocExportData : public AosSmartDoc
{
public:
	AosSdocExportData(const bool flag);
	~AosSdocExportData();

	// virtual bool procDoc(
	// 		const OmnString &siteid,
	// 		const AosWebRequestPtr &req,
	// 		const AosXmlTagPtr &sdoc,
	// 		const AosSessionPtr &session,
	// 		OmnString &contents, 
	// 		AosXmlRc &errcode,
	// 		OmnString &errmsg);

	virtual bool parseDate(const OmnString &contents){return true;}

	virtual AosSmartDocObjPtr	clone() {return OmnNew AosSdocExportData(false);}
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	// virtual bool runSmartdoc(
	 // 		const OmnString &siteid,
	 // 		const AosWebRequestPtr &req, 
	 // 		const AosXmlTagPtr &root, 
	 // 		const AosXmlTagPtr &userobj, 
	 // 		const OmnString &sdoc_objid,
	 // 		OmnString &contents, 
	 // 		AosXmlRc &errcode, 
	 // 		OmnString &errmsg);

private:
	OmnFilePtr mDataFile;
	bool checkValue(OmnString &value);
	bool composeFileName(OmnString &fname);

};
#endif
