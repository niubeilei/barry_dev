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
// 2011/02/19: Created by Ken
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GICs_GicHtmlGrid_h
#define Aos_GICs_GicHtmlGrid_h

#include "GICs/GIC.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include <vector>
#include <boost/regex.hpp>

class AosGicHtmlGrid : public AosGic
{

public:
	AosGicHtmlGrid(const bool flag);
	~AosGicHtmlGrid();

	virtual bool	
	generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentid,
		AosHtmlCode &code);
			
private:
	
	OmnString
	generateTemplate(OmnString &cont, const AosHtmlReqProcPtr &htmlPtr);


	OmnString
	getRecordValue(const AosXmlTagPtr &record, const OmnString &path, OmnString &dft);
	
	bool
	createEmptyGrid(const AosXmlTagPtr &vpd, OmnString &listHtmlCode,const OmnString &grid_css_random);

	bool
	createInnerHtml(const AosXmlTagPtr &column,const AosXmlTagPtr &record, OmnString &html, OmnString &value,OmnString &name,const AosXmlTagPtr &vpd);

	OmnString
	createHtmlCode(AosXmlTagPtr &vpd, 
				const AosHtmlReqProcPtr &htmlPtr,
				const AosXmlTagPtr &record, 
				const int row, 
				const OmnString &grid_css_random,
				vector<OmnString> &bandname
				);
	int parseFml(OmnString &str);
	int parseNum(OmnString &str, int ceil = 0);
	bool isNumStr(OmnString &str);
	int parseData(AosXmlTagPtr &datacal, AosXmlTagPtr &contents);
	bool parseExp(OmnString &exp,const OmnString &vbdname, AosXmlTagPtr &contents);
	bool parseSum(OmnString & exp, AosXmlTagPtr &contents);
	bool parseBdStr(OmnString & str, AosXmlTagPtr &record);
	bool createStaTab(const AosXmlTagPtr &vpd,
					   const AosXmlTagPtr &doc_cont,
					   OmnString &listHtmlcode,
				       const OmnString &grid_css_random,
					   const vector<OmnString> &bandname,
					   const AosHtmlReqProcPtr &htmlPtr);
};

#endif

