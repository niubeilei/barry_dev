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
// 07/16/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_HtmlModules_DclDb_h
#define Aos_HtmlModules_DclDb_h

#include "HtmlModules/DataCol.h"
#include "HtmlServer/HtmlReqProc.h"
#include "Util/OmnNew.h"
#include "Rundata/Rundata.h"


class AosDclDb : public AosDataCol
{
public:
	AosDclDb();
	~AosDclDb() {}

	virtual OmnString getJsonConfig(const AosXmlTagPtr &vpd);

	virtual AosXmlTagPtr retrieveData(
					const AosHtmlReqProcPtr &htmlPtr,
					const AosXmlTagPtr &vpd,
					const AosXmlTagPtr &obj,
					const OmnString &tagname);

	virtual bool retrieveContainers(
					const AosHtmlReqProcPtr &htmlPtr,
			 		AosXmlTagPtr &vpd,
			 		const AosXmlTagPtr &obj,
			 		const OmnString &parentid,
					const OmnString &tagname,
			 		AosHtmlCode &code);
	
	bool createQuery(
					const AosHtmlReqProcPtr &htmlPtr,
					const AosXmlTagPtr &vpd, 
					const AosXmlTagPtr &obj, 
					OmnString &cmd);

	bool deleteClipvar( OmnString &queryStr);
	bool parseLoginInfo(OmnString &queryStr, const AosHtmlReqProcPtr &htmlPtr);

	OmnString getContainer(
					const AosXmlTagPtr &vpd, 
					const AosXmlTagPtr &obj);

	OmnString getTags(
					const AosXmlTagPtr &vpd, 
					const AosXmlTagPtr &obj);

	OmnString getWords(
					const AosXmlTagPtr &vpd, 
					const AosXmlTagPtr &obj);

	// Tom 2011 06 07
	int parseFml(OmnString &str);
	int parseNum(OmnString &str);
	bool isNumStr(OmnString &str);
	int parseData(AosXmlTagPtr &datacal, AosXmlTagPtr &contents);
	bool parseExp(OmnString &exp,const OmnString &vbdname, AosXmlTagPtr &contents);
	bool parseSum(OmnString & exp, AosXmlTagPtr &contents);
	bool parseSumOfRow(OmnString & str,	const AosXmlTagPtr &record);
	bool parseBdStr(OmnString & str, AosXmlTagPtr &record);
	bool parseSumRecord(AosXmlTagPtr &datacol, AosXmlTagPtr &contents);	
	bool processCounters(const AosXmlTagPtr &conunters, AosXmlTagPtr &records, const AosRundataPtr &rdata);	
};

#endif
