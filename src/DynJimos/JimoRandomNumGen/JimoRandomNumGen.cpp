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
// 2013/05/29 Created by Young Pan
////////////////////////////////////////////////////////////////////////////
#include "DynJimos/JimoRandomNumGen/JimoRandomNumGen.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosJimoRandomNumGen(
 		const AosRundataPtr &rdata, 
 		const AosXmlTagPtr &sdoc) 
{
	rdata->setDLLObj(0);
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosJimoRandomNumGen(rdata, sdoc);
		aos_assert_rr(jimo, rdata, 0);
		rdata->setJimo(jimo);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosJimoRandomNumGen::AosJimoRandomNumGen(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &sdoc)
:
AosJimo(rdata, sdoc)
{
	if (!config(rdata, sdoc))
	{                                         
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosJimoRandomNumGen::~AosJimoRandomNumGen()
{
}


bool
AosJimoRandomNumGen::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	// <phrasemaker ranges="xxx" datatype="xxx" num="xxx" makertype="sequence|set|atomic">
	// 		<ranges>
	// 			<range type”range” weight=”200” min="xx" max="xxx" rangetype="xx"></range>
	//			<range type=”sequence” weight=”10” start=”ddd” end=”ddd” step=”xxx” rangetype=”xxx”/>
	//			<range type=”enum” weight=”xxx”>dd,dd,dd,...</range>
	//			<range type=”rpick” weight=”xxx”>dd,dd,dd,...</range>
	//		</ranges>
	//	</phrasemaker>

	if (!def) return true;

	AosXmlTagPtr pmXml = def->getFirstChild("phrasemaker");
	aos_assert_r(pmXml, false);
	mPharseNum = pmXml->getAttrInt("num", 0);
	aos_assert_r(mPharseNum, false);
	AosXmlTagPtr ranges = pmXml->getFirstChild("ranges");
	aos_assert_r(ranges, false);

	AosXmlTagPtr curRange = ranges->getFirstChild("range");
	OmnString type = curRange->getAttrStr("type", "");
	aos_assert_r(type != "", false);
	setRangeAttr(curRange, type);

	while (curRange = ranges->getNextChild("range"))
	{
		OmnString type = curRange->getAttrStr("type", "");
		aos_assert_r(type != "", false);
		setRangeAttr(curRange, type);
	}
	
	return true;
}


bool
AosJimoRandomNumGen::setRangeAttr(AosXmlTagPtr &range, OmnString type)
{
	aos_assert_r(type != "", false);
	
	if (type == "range")
	{
		mRange.mType = range->getAttrStr("type");
		mRange.mRangeType = range->getAttrStr("rangetype");
		mRange.mWeight = range->getAttrInt("weight", 0);
		mRange.mMin = range->getAttrInt("min", 0);
		mRange.mMax = range->getAttrInt("max", 0);

		mapRange.insert(make_pair(type, mRange));		
	}
	else if (type == "sequence")
	{
		mSequence.mType = range->getAttrStr("type");
		mSequence.mRangeType = range->getAttrStr("rangetype");
		mSequence.mWeight = range->getAttrInt("weight", 0);
		mSequence.mStart = range->getAttrInt("start", 0);
		mSequence.mEnd = range->getAttrInt("end", 0);
		mSequence.mStep = range->getAttrInt("step", 0);
		
		mapRange.insert(make_pair(type, mSequence));		
	}
	else if (type == "enum")
	{
		mEnum.mType = range->getAttrStr("type");
		mEnum.mRangeType = range->getAttrStr("rangetype");
		mEnum.mWeight = range->getAttrInt("weight", 0);
		
		mapRange.insert(make_pair(type, mEnum));		
	}
	else if (type == "rpick")
	{
		mRpick.mType = range->getAttrStr("type");
		mRpick.mRangeType = range->getAttrStr("rangetype");
		mRpick.mWeight = range->getAttrInt("weight", 0);
		
		mapRange.insert(make_pair(type, mRpick));		
	}


	return true;
}


int
AosJimoRandomNumGen::dataPhrase(OmnString type)
{
	aos_assert_r(type != "", false);
	int rslt;

	if (type == "range")
	{
		int rMax = mRange.mMax;		
		int rMin = mRange.mMin;		

		rslt = rand() % (rMax+1);
		while (rslt < rMin) 
		{
			rslt = rand() % (rMax + 1);
		}
		
		OmnScreen << "range : " << rslt << endl;
		return rslt;
	}
	else if (type == "sequence")
	{
		int rStart = mSequence.mStart;
		int rEnd = mSequence.mEnd;
		int rStep = mSequence.mStep;

		rslt = rand() % (rEnd + 1);
		int comp = (rslt - rStart) % rStep;
		while (comp != 0 || rslt < rStart)
		{
			rslt = rand() % (rEnd+1);
		}

		return rslt;
	}
	else if (type == "enum")
	{
		return true;
	}
	else if (type == "rpick")
	{
		return true;
	}

	return true;
}


OmnString
AosJimoRandomNumGen::toString() const
{
	OmnString ss = AosJimo::toString();
	return ss;
}


bool 
AosJimoRandomNumGen::serializeTo(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff) const
{
	bool rslt = AosJimo::serializeTo(rdata, buff);
	aos_assert_rr(rslt, rdata, false);

	return true;
}


bool 
AosJimoRandomNumGen::serializeFrom(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	bool rslt = AosJimo::serializeFrom(rdata, buff);
	aos_assert_rr(rslt, rdata, false);

	return true;
}


AosJimoPtr 
AosJimoRandomNumGen::clone(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &sdoc) const
{
	try
	{
		AosJimoRandomNumGen *obj = OmnNew AosJimoRandomNumGen(*this);
		bool rslt = obj->config(rdata, sdoc);
		aos_assert_rr(rslt, rdata, 0);
		return obj;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_cloning_jimo") << toString() << enderr;
		return 0;
	}
}


bool 
AosJimoRandomNumGen::run(		
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &sdoc)
{
	// <phrasemaker ranges="xxx" datatype="xxx" num="xxx" makertype="sequence|set|atomic">
	// 		<ranges>
	// 			<range type”range” weight=”200” min="xx" max="xxx" rangetype="xx"></range>
	//			<range type=”sequence” weight=”10” start=”ddd” end=”ddd” step=”xxx” rangetype=”xxx”/>
	//			<range type=”enum” weight=”xxx”>dd,dd,dd,...</range>
	//			<range type=”rpick” weight=”xxx”>dd,dd,dd,...</range>
	//		</ranges>
	//	</phrasemaker>
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosJimoRandomNumGen::supportInterface(
		const AosRundataPtr &rdata, 
		const OmnString &interface_objid) const
{
	AosXmlTagPtr doc = AosGetDocByObjid(interface_objid, rdata);
	if (!doc)
	{
		return false;
	}

	int id = doc->getAttrInt(AOSTAG_INTERFACE_ID, -1);
	return supportInterface(rdata, id);
}


bool 
AosJimoRandomNumGen::supportInterface(
		const AosRundataPtr &rdata, 
		const int interface_id) const
{
	switch (interface_id)
	{
	case eRunWithSmartDoc:
		 return true;

	default:
		 break;
	}
	return false;
}


bool
AosJimoRandomNumGen::runWithSmartdoc(		
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &sdoc)
{
	u32 pTotalWeight = 0;                                                 
	map<OmnString, pharseType>::iterator itr = mapRange.begin();          
	for (u32 i=0; i<mapRange.size(); i++)                                 
	{                                                                     
		pTotalWeight += itr->second.mWeight;                              
		itr++;                                                            
	}                                                                     

	OmnString tmpType;                                                    
	OmnString myResult;                                                   
	for (u32 i=0; i<mPharseNum; i++)                                      
	{                                                                     
		itr = mapRange.begin();                                           
		int tmpWeight = 0;                                                
		map<OmnString, pharseType>::iterator tmpItr = mapRange.begin();   
		int idx = rand()%pTotalWeight + 1;                                
		for (u32 j=0; j<mapRange.size(); j++)                             
		{                                                                 
			for (u32 z=0; z<j; z++)                                       
			{                                                             
				tmpWeight += tmpItr->second.mWeight;                      
				tmpItr++;                                                 
			}                                                             

			if (idx <= tmpWeight)                                         
			{                                                             
				tmpType = itr->first;                                     
				myResult << dataPhrase(tmpType);                          
				break;                                                    
			}                                                             
			itr++;                                                        
		}                                                                 
	}                                                                     


	OmnScreen << "-------- : " << myResult << " : --------" << endl;
	return true;
}


