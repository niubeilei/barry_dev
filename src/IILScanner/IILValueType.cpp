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
// 05/12/2012 Created by Chen Ding
// 2012/11/12 Moved From IILUtil By Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "IILScanner/IILValueType.h"

#include "API/AosApiG.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/IILScannerObj.h"
#include "SEInterfaces/IILSelector.h"
#include "Util/StrSplit.h"
#include "ValueSel/ValueRslt.h"


AosIILValueType::E
AosIILValueType::toEnum(const OmnString &name)
{
	if (name == AOSIILVALUETYPE_KEY) 		return eKey;
	if (name == AOSIILVALUETYPE_VALUE) 		return eValue;
	if (name == AOSIILVALUETYPE_PREFIX) 	return ePrefix;
	if (name == AOSIILVALUETYPE_POSTFIX) 	return ePostfix;
	if (name == AOSIILVALUETYPE_SUBSTRING) 	return eSubstr;
	if (name == AOSIILVALUETYPE_COMPOSE) 	return eCompose;
	if (name == AOSIILVALUETYPE_VALUESEL) 	return eValueSel;
	return eInvalid;
}
	

OmnString
AosIILValueType::toString(const AosIILValueType::E code)
{
	switch (code)
	{
	case eKey: 		return AOSIILVALUETYPE_KEY;
	case eValue: 	return AOSIILVALUETYPE_VALUE;
	case ePrefix: 	return AOSIILVALUETYPE_PREFIX;
	case ePostfix: 	return AOSIILVALUETYPE_POSTFIX;
	case eSubstr: 	return AOSIILVALUETYPE_SUBSTRING;
	case eCompose: 	return AOSIILVALUETYPE_COMPOSE;
	case eValueSel: return AOSIILVALUETYPE_VALUESEL;
	default:
		 break;
	}
	return AOSIILVALUETYPE_INVALID;
}


bool
AosIILValueType::getValue(
		const AosIILSelector &selector,
		AosValueRslt &result,
		const char *key, 
		const int key_len,
		const u64 &docid, 
		const AosIILScannerObjPtr &scanner,
		const AosRundataPtr &rdata)
{
	// This function retrieves the value based on 'type'
	switch (selector.mValueType)
	{
	case eKey:
		 result.setStr(OmnString(key, key_len));
		 return true;
		 
	case eValue:
		 result.setU64(docid);
		 return true;

	case ePrefix:
		 {
		 	const char *vv;
		 	int len;
		 	bool rslt = AosGetField(vv, len, key, key_len,
				selector.mSourceFieldIdx, selector.mSourceSeparator, rdata.getPtr());
			if(!rslt) return rslt;

			OmnString str(vv, len);
			result.setStr(str);
		 	return true;
		 }

	case ePostfix:
	case eSubstr:
		 OmnNotImplementedYet;
		 return false;

	case eCompose:
		 {
			 vector<OmnString> &elems = scanner->getComposorVector();
			 AosStrSplit::splitStrBySubstr(
				key, scanner->getOrigSep().data(), elems, elems.size());
			 vector<AosIILScannerObj::ComposorInfo> &composors = scanner->getComposorInfo();
			 OmnString v = "";
			 result.setStr(v);
			 OmnString vv = result.getStr();
			 const char *sep = scanner->getTargetSep().data();
			 for (u32 i=0; i<composors.size(); i++)
			 {
				 switch (composors[i].mType)
				 {
				 case AosStrElemType::eElem:
					  if (sep && i != 0) vv << sep;
					  vv << elems[composors[i].mIndex];
					  break;

				 case AosStrElemType::eConst:
					  if (sep && i != 0) vv << sep;
					  vv << composors[i].mConstants;
					  break;

				 case AosStrElemType::eDocid:
					  if (sep && i != 0) vv << sep;
					  vv << docid;
					  break;

				 case AosStrElemType::eKey:
					  if (sep && i != 0) vv << sep;
					  vv << key;
					  break;

				 case AosStrElemType::eDocidHigh:
					  if (sep && i != 0) vv << sep;
					  vv << (u32)(docid >> 32);
					  break;

				 case AosStrElemType::eDocidLow:
					  if (sep && i != 0) vv << sep;
					  vv << (u32)docid;
					  break;

				 default:
					  // It is an error.
					  AosSetErrorUser(rdata, "internal_error:") 
						  << selector.mSelectorType << enderr;
					  return false;
				 }
			 }
			 return true;
		 }
		 break;

	case eValueSel:
		 // This is done through a value selector.
		 if (!selector.mValueSel->run(result, rdata))
		 {
			 OmnAlarm << "Failed retrieve data: " << rdata->getErrmsg() << enderr;
			 return false;
		 }
		 return true; 

	default:
		 break;
	}

	AosSetErrorU(rdata, "internal_error:") << selector.mSelectorType;
	OmnAlarm << rdata->getErrmsg() << enderr;
	return false;
}

