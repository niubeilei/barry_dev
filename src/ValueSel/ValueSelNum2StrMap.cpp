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
#include "ValueSel/ValueSelNum2StrMap.h"

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
#if 0

AosValueSelNum2StrMap::AosValueSelNum2StrMap(const bool reg)
:
AosValueSel(AOSACTOPRID_NUM2STRMAP, AosValueSelType::eNum2StrMap, reg)
{
}


AosValueSelNum2StrMap::AosValueSelNum2StrMap(
        const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_NUM2STRMAP, AosValueSelType::eNum2StrMap, false)
{
}


AosValueSelNum2StrMap::~AosValueSelNum2StrMap()
{
}


bool
AosValueSelNum2StrMap::run(
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
	// 		AOSTAG_VALUE_TYPE=AOSACTOPRID_NUM2STRMAP
	// 		AOSTAG_WITH_DEFAULT="true|false"
	// 		AOSTAG_SEPARATOR="xxx"
	// 		AOSTAG_MAPTYPE=AOSVALUE_VALUELISTING|AOSVALUE_VALUESELS
	// 		AOSTAG_DATA_TYPE="xxx"><![CDATA[dft]]>
	//		<AOSTAG_VALUE a value selector to select the value to be mapped .../>
	//		<AOSTAG_MAP>
	//			<entry AOSTAG_KEY="xxx" AOSTAG_VALUE="xxx"/>
	//			<entry AOSTAG_KEY="xxx" AOSTAG_VALUE="xxx"/>
	//			...
	//			<entry .../>
	//		</AOSTAG_MAP>
	//	</zky_valuedef>
	
	aos_assert_r(sdoc, false);
	aos_assert_r(rdata, false);
	rdata->setArg1(AOSARG_CALC_VALUE, "");
	rdata->setArg1(AOSARG_VALUE_MAPPED, "false");

	// 1. Determine the data type
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

	// 2. Retrieve the map
	AosXmlTagPtr maptag = sdoc->getFirstChild(AOSTAG_MAP);
	if (!maptag)
	{
		AosSetError(rdata, AOSLT_MISSING_MAP_TAG);
		// AOSMONITORLOG_FINISH(rdata);
		return false;
	}

	// 3. Retrieve the value to be mapped
	AosValueRslt vv;
	bool rslt = AosValueSel::getValueStatic(vv, sdoc, AOSTAG_VALUE, rdata);
	if (!rslt) 
	{
		// AOSMONITORLOG_FINISH(rdata);
		return false;
	}
	OmnString value_to_map = vv.getStr();
	if (!rslt)
	{
		AosSetErrorUser(rdata, "valuesel_map_002") << sdoc->toString() << enderr;
		return false;
	}

	// 5. Retrieve the map type
	OmnString maptype = sdoc->getAttrStr(AOSTAG_MAPTYPE);
	if (maptype == AOSVALUE_VALUELISTING)
	{
		return getValueByValueListing(value_to_map, maptag, 
				datatype, valueRslt, sdoc, rdata);
	}

	if (maptype == AOSVALUE_VALUESELS)
	{
		return getValueByValueSelectors(value_to_map, maptag, 
				datatype, valueRslt, sdoc, rdata);
	}

	if (maptype == AOSVALUE_RANGES)
	{
		return getValueByRanges(value_to_map, maptag, 
				datatype, valueRslt, sdoc, rdata);
	}

	AosSetErrorUser(rdata, "valuesel_map_001") << maptype << enderr;
	return false;
}


bool
AosValueSelNum2StrMap::getValueByValueSelectors(
		const OmnString &value_to_map,
		const AosXmlTagPtr &maptag,
		const AosDataType::E datatype,
		AosValueRslt &valueRslt,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	// This function assumes the values are listed by value selectors:
	// 	<map ...>
	// 		<valuesel AOSTAG_KEY="xxx" AOSTAG_VALUE="xxx"/>
	// 		<valuesel AOSTAG_KEY="xxx" AOSTAG_VALUE="xxx"/>
	// 		...
	// 	</map
	AosXmlTagPtr entry = maptag->getFirstChild();
	int guard = eMaxValues;
	while (guard-- && entry)
	{
		OmnString key = entry->getAttrStr(AOSTAG_KEY);
		if (value_to_map == key)
		{
			OmnString mapvalue = entry->getAttrStr(AOSTAG_VALUE);
			valueRslt.setStr(mapvalue);
			// AOSMONITORLOG_FINISH(rdata);
			rdata->setArg1(AOSARG_CALC_VALUE, mapvalue);
			rdata->setArg1(AOSARG_VALUE_MAPPED, "true");
			return true;
		}

		entry = maptag->getNextChild();
	}

	if (guard <= 0)
	{
		AosSetErrorUser(rdata, "valuesel_map_003") << sdoc->toString() << enderr;
		return false;
	}

	// Did not find it. Check whether it allows the default.
	if (!sdoc->getAttrBool(AOSTAG_WITH_DEFAULT, true))
	{
		rdata->setArg1(AOSARG_VALUE_MAPPED, "false");
		valueRslt.reset();
		// AOSMONITORLOG_FINISH(rdata);
		return true;
	}

	// Use the default:
	OmnString dft = sdoc->getNodeText();
	valueRslt.setStr(dft);
	rdata->setArg1(AOSARG_CALC_VALUE, dft);
	rdata->setArg1(AOSARG_VALUE_MAPPED, "true");
	// AOSMONITORLOG_FINISH(rdata);
	return true;
}


bool
AosValueSelNum2StrMap::getValueByValueListing(
		const OmnString &value_to_map,
		const AosXmlTagPtr &maptag,
		const AosDataType::E datatype,
		AosValueRslt &valueRslt,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	// This function assumes the values are listed by value selectors:
	// 	<map ...>
	// 		key,value,key,value,...
	// 	</map
	OmnString values = maptag->getNodeText();
	OmnString sep = sdoc->getAttrStr(AOSTAG_SEPARATOR);
	if (sep == "") sep = ",";
	OmnStrParser1 parser(values, sep);
	int guard = eMaxListedValues;
	OmnString key;
	while (guard-- && (key= parser.nextWord()) != "")
	{
		if (value_to_map == key)
		{
			OmnString mapvalue = parser.nextWord();
			valueRslt.setStr(mapvalue);
			// AOSMONITORLOG_FINISH(rdata);
			rdata->setArg1(AOSARG_CALC_VALUE, mapvalue);
			rdata->setArg1(AOSARG_VALUE_MAPPED, "true");
			return true;
		}
		parser.nextWord();
	}

	if (guard <= 0)
	{
		AosSetErrorUser(rdata, "valuesel_map_003") << sdoc->toString() << enderr;
		return false;
	}

	// Did not find it. Check whether it allows the default.
	if (!sdoc->getAttrBool(AOSTAG_WITH_DEFAULT, true))
	{
		rdata->setArg1(AOSARG_VALUE_MAPPED, "false");
		valueRslt.reset();
		// AOSMONITORLOG_FINISH(rdata);
		return true;
	}

	// Use the default:
	OmnString dft = sdoc->getNodeText();
	valueRslt.setValue(datatype, dft, rdata.getPtrNoLock());
	rdata->setArg1(AOSARG_CALC_VALUE, dft);
	rdata->setArg1(AOSARG_VALUE_MAPPED, "true");
	// AOSMONITORLOG_FINISH(rdata);
	return true;
}


OmnString
AosValueSelNum2StrMap::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const OmnString &valuesel,
		const OmnString &values,
		const AosRundataPtr &rdata)
{
	// 	<zky_valuedef 
	// 		AOSTAG_VALUE_TYPE=AOSACTOPRID_NUM2STRMAP
	// 		AOSTAG_WITH_DEFAULT="true|false"
	// 		AOSTAG_SEPARATOR="xxx"
	// 		AOSTAG_MAPTYPE=AOSVALUE_VALUELISTING|AOSVALUE_VALUESELS
	// 		AOSTAG_DATA_TYPE="xxx"><![CDATA[dft]]>
	//		<AOSTAG_VALUE a value selector to select the value to be mapped .../>
	//		<AOSTAG_MAP>
	//			<entry AOSTAG_KEY="xxx" AOSTAG_VALUE="xxx"/>
	//			<entry AOSTAG_KEY="xxx" AOSTAG_VALUE="xxx"/>
	//			...
	//			<entry .../>
	//		</AOSTAG_MAP>
	//	</zky_valuedef>
	aos_assert_r(tagname != "", "");
	OmnString docstr = "<";
	docstr << tagname << " " << AOSTAG_VALUE_TYPE << "=\""
		<< AOSACTOPRID_NUM2STRMAP << "\" ";
	if (OmnRandom::percent(50))
	{
		docstr << AOSTAG_WITH_DEFAULT << "=\"true\" ";
	}

	OmnString sep = OmnRandom::pickEntrySeparator();
	if (sep == "") sep = ",";
	docstr << AOSTAG_SEPARATOR << "=\"" << sep << "\" ";

	int maptype = 0;
	if (OmnRandom::percent(50))
	{
		maptype = 1;
		docstr << AOSTAG_MAPTYPE << "=\"" << AOSVALUE_VALUELISTING << "\" ";
	}
	else
	{
		docstr << AOSTAG_MAPTYPE << "=\"" << AOSVALUE_VALUESELS << "\" ";
	}

	OmnString datatype = OmnRandom::pickValueDataType();
	docstr << AOSTAG_DATA_TYPE << "=\"" << datatype << "\">";

	docstr << valuesel << "<" << AOSTAG_MAP << ">";
	vector<OmnString> vv;
	int nn = AosStrSplit::splitStrBySubstr(values, ",", vv, eMaxListedValues);
	aos_assert_r(nn > 0, "");
	if (maptype == 0)
	{
		// It is through a value selector
		for (u32 i=0; i<(u32)nn; i+=2)
		{
			if (i+2 < vv.size())
			{
				docstr << "<entry " << AOSTAG_KEY << "=\"" << vv[i]
					<< "\" " << AOSTAG_VALUE << "=\"" << vv[i+1] << "\"/>";
			}
		}
	}
	else
	{
		for (int i=0; i<nn; i++)
		{
			if (i != 0) docstr << sep;
			docstr << vv[i];
		}
	}
	docstr << "</" << AOSTAG_MAP << "></" << tagname << ">";
	return docstr;
}


OmnString 
AosValueSelNum2StrMap::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


bool
AosValueSelNum2StrMap::getValueByRanges(
		const OmnString &value_to_map,
		const AosXmlTagPtr &maptag,
		const AosDataType::E datatype,
		AosValueRslt &valueRslt,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	// This function assumes the values are listed by value selectors:
	// 	<map AOSTAG_ORDER="num_alpha">
	// 		<valuesel start="xxx" end="xxx" 
	// 			incstart="true|false" 
	// 			incend="true|false">value</valuesel>
	// 		...
	// 	</map
	AosXmlTagPtr entry = maptag->getFirstChild();
	bool isNumAlpha = (maptag->getAttrStr(AOSTAG_ORDER) == "nm_alpha");
	int guard = eMaxValues;
	while (guard-- && entry)
	{
		OmnString start = entry->getAttrStr("start");
		OmnString end = entry->getAttrStr("end");
		bool include_start = entry->getAttrBool("incstart", true);
		bool include_end = entry->getAttrBool("incend", true);
		int start_comp = AosIILUtil::valueMatch(value_to_map, start, isNumAlpha);;
		int end_comp = AosIILUtil::valueMatch(value_to_map, end, isNumAlpha);;

		if (((include_start && start_comp >= 0) || (!include_start && start_comp > 0)) &&
			((include_end && end_comp <= 0) || (!include_end && end_comp < 0)))
		{
			OmnString vv = entry->getNodeText();
			valueRslt.setStr(vv);
			return true;
		}

		entry = maptag->getNextChild();
	}

	if (guard <= 0)
	{
		AosSetErrorUser(rdata, "valuesel_map_003") << sdoc->toString() << enderr;
		return false;
	}

	// Did not find it. Check whether it allows the default.
	if (!sdoc->getAttrBool(AOSTAG_WITH_DEFAULT, true))
	{
		rdata->setArg1(AOSARG_VALUE_MAPPED, "false");
		valueRslt.reset();
		// AOSMONITORLOG_FINISH(rdata);
		return true;
	}

	// Use the default:
	OmnString dft = sdoc->getNodeText();
	valueRslt.setValue(datatype, dft, rdata.getPtrNoLock());
	rdata->setArg1(AOSARG_CALC_VALUE, dft);
	rdata->setArg1(AOSARG_VALUE_MAPPED, "true");
	// AOSMONITORLOG_FINISH(rdata);
	return true;
}


AosValueSelObjPtr
AosValueSelNum2StrMap::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelNum2StrMap(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}


#endif
