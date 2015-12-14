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
// 08/09/2012	Created by Chong Shen
////////////////////////////////////////////////////////////////////////////
#include "ValueSel/ValueSelCondMap.h"
#include "Actions/ActUtil.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "IILUtil/IILUtil.h"
#include "Random/RandomUtil.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "Util/HashUtil.h"
#include "Util/OmnNew.h"
#include "Util/StrParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "Conds/Condition.h"
#include <map> 
#if 0
AosValueSelCondMap::AosValueSelCondMap(const bool reg)
:
AosValueSel(AOSACTOPRID_CONDMAP, AosValueSelType::eCondMap, reg)
{
}
AosValueSelCondMap::AosValueSelCondMap(
        const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_CONDMAP, AosValueSelType::eCondMap, false)
{
}

AosValueSelCondMap::~AosValueSelCondMap()
{
}

bool
AosValueSelCondMap::run(
		AosValueRslt &valueRslt,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	// This value selector defines a map that maps keys to values. It retrieves
	// the value to be mapped, uses the map to translate the value to a new
	// value, and returns the value through 'valueRslt'. If no map entry is found,
	// it checks whether AOSTAG_WITH_DEFAULT is true (default is true). If yes, 
	// it uses the default value. Otherwise, it returns an empty value.
	//
	// rdata arguments:
	// 	AOSARGNAME_CALC_VALUE:		the mapped value
	// 	AOSARGNAME_VALUE_MAPPED:	'true' or 'false'
	//
	// 'sdoc' format:
	// 	<zky_valuedef 
	// 		AOSTAG_WITH_DEFAULT="true|false"
	// 		AOSTAG_DATA_TYPE="xxx" process_type="free|fix"><![CDATA[dft]]>
	//		<conds>
	//			<cond >
	// 				<lhs confirm_code="xxx"/>
	// 				<rhs msg="xxx"/>
	// 			</cond>
	// 			...
	//			<cond >
	// 				<lhs confirm_code="xxx"/>
	// 				<rhs msg="xxx"/>
	// 			</cond>
	//		</conds>
	//		<AOSTAG_MAP>
	//			<entry conds="1,2,3" condnum="3">
	//				<valuesel/>
	//			</entry>
	//			<entry conds="2,3|1,2"  condnum="2">
	//				<valuesel/>
	//			</entry>
	//			...
	//			<entry conds="1"  condnum="1">
	//				<valuesel/>
	//			</entry>
	//		</AOSTAG_MAP>
	//	</zky_valuedef>
	
	aos_assert_r(sdoc, false);
	aos_assert_r(rdata, false);
	AosDataType::E datatype = AosDataType::toEnum(sdoc->getAttrStr(AOSTAG_DATA_TYPE));
	if (!AosDataType::isValid(datatype))
	{
    	datatype = AosDataType::eString;
	}

	if (datatype == AosDataType::eXmlDoc)
	{
   		AosSetError(rdata, AOSLT_INVALID_DATA_TYPE);
   		// AOSMONITORLOG_FINISH(rdata);	
	    return false;
	}
	if(!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	AosXmlTagPtr conds = sdoc->getFirstChild("conds");
	AosXmlTagPtr cond = conds->getFirstChild("cond");
	map<int, bool> condRslt;
	int counter=0;
	while(cond)
	{
		bool r = AosCondition::evalCondStatic(cond,rdata);
		condRslt[counter]=r;
		counter++;
		cond =conds->getNextChild("cond");
	}

	OmnString procType = sdoc->getAttrStr("process_type");
	AosXmlTagPtr MAP  = sdoc->getFirstChild(AOSTAG_MAP);
	AosXmlTagPtr entry = MAP->getFirstChild("entry");
	bool entry_rslt=false;
	if(procType=="fix")
	{	
		vector<OmnString> valueList,valueFinal;
		while(entry)
		{
			valueList.clear();
			OmnString value = entry->getAttrStr("conds");
			AosStrSplit::splitStrByChar(value, "|", valueList, 10000);	
			int n = valueList.size();
			for(int i =0; i < n; i++)
			{
				bool temp = false;
				valueFinal.clear();
				AosStrSplit::splitStrByChar(valueList[i], ",", valueFinal, 10000);	
				int fnum = valueFinal.size();
				for(int j =0; j < fnum; j++)
				{
					map<int,bool>::iterator iter = condRslt.find(atoi(valueFinal[j].data()));	
					if(iter == condRslt.end())
					{
						AosSetError(rdata, "Failed to get the condition!");
						OmnAlarm << rdata->getErrmsg() << enderr;
						return false;
					}
					if(j==0)
					{
						temp = iter->second;
					}
					else
					{
						temp = temp&&iter->second;
					}
					if(j == fnum-1 && temp)
					{
						AosXmlTagPtr vsl = entry->getFirstChild("valuesel");
						AosValueRslt vv;
						bool rslt = AosValueSel::getValueStatic(vv, vsl, rdata);
						if(rslt)
						{
							entry_rslt=true;
							valueRslt.setStr(vv.getStr());
							return true;
						}
						else
						{
							AosSetError(rdata, "Fail to get the value!");
							OmnAlarm << rdata->getErrmsg() << enderr;
							return false;
						}
					}
				}	
				entry = MAP->getNextChild("entry");
			}
		}
	}
	else
	{
		while(entry)
		{
			int value = atoi(entry->getAttrStr("condnum").data());
			int n = condRslt.size();
			int count = 0;
			for(int i = 0;i< n;i++)
			{
				map<int,bool>::iterator iter = condRslt.find(i);	
				if(iter != condRslt.end())
				{
					bool y = iter->second;
					if (y)
					{
						count++;
					}
				}
				if(value<=count)
				{	
					AosXmlTagPtr vsl = entry->getFirstChild("valuesel");
					AosValueRslt vv;
					bool rslt = AosValueSel::getValueStatic(vv, vsl, rdata);
					if(rslt)
					{
						entry_rslt=true;
						valueRslt.setValue(datatype, vv.getStr());
						return true;
					}
					else
					{
						AosSetError(rdata, "Fail to get the value!");
						OmnAlarm << rdata->getErrmsg() << enderr;
						return false;
					}
				}
			}
			entry = MAP->getNextChild("entry");
		}
	}	
	if(!entry_rslt)
	{
		OmnString def = sdoc->getAttrStr(AOSTAG_WITH_DEFAULT);
		valueRslt.setValue(datatype, def, rdata.getPtrNoLock());
	}
	return true;
}

AosValueSelObjPtr
AosValueSelCondMap::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelCondMap(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}


#endif
