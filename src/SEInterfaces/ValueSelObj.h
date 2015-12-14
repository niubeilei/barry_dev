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
// 05/22/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_ValueSelObj_h
#define Aos_SEInterfaces_ValueSelObj_h

#include "IILScanner/IILMatchType.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/ValueSelType.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/StrElemType.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

class AosValueRslt;

class AosValueSelObj : virtual public OmnRCObject
{
protected:
	OmnString			mName;
	AosValueSelType::E	mType;
	bool				mIsTemplate;

public:
	AosValueSelObj(
				const OmnString &name, 
				const AosValueSelType::E type, 
				const bool reg);
	~AosValueSelObj();

	virtual bool run(AosValueRslt &value, const AosRundataPtr &rdata) = 0;

	virtual bool run(
					AosValueRslt &valueRslt,
					const AosXmlTagPtr &item, 
					const AosRundataPtr &rdata) = 0;

	virtual bool run(
					AosValueRslt &valueRslt,
					const char *record,
					const int record_len,
					const AosRundataPtr &rdata) = 0;

	virtual AosValueSelObjPtr clone(
					const AosXmlTagPtr &sdoc, 
					const AosRundataPtr &rdata) = 0;

	static bool registerValueSel(AosValueSelObjPtr &actopr);

	static AosValueSelObjPtr getValueSelStatic(
					const AosValueSelType::E type, 
					const AosRundataPtr &rdata);

	static AosValueSelObjPtr getValueSelStatic(
					const AosXmlTagPtr &item, 
					const AosRundataPtr &rdata);

	static bool getValueStatic(
					AosValueRslt &valueRslt,
					const AosXmlTagPtr &sdoc, 
					const AosRundataPtr &rdata);

	static bool getValueStatic(
					AosValueRslt &valueRslt,
					const AosXmlTagPtr &sdoc,
					const OmnString &tagname,
					const AosRundataPtr &rdata);
	
	static bool composeValuesStatic(
					AosValueRslt &valueRslt,
					const AosXmlTagPtr &sdoc, 
					const AosRundataPtr &rdata);

	static bool getValueForCounterVarStatic(
					const OmnString &xpath, 
					OmnString &value, 
					const AosRundataPtr &rdata);

	static bool retrieveValuesStatic(
					vector<OmnString> &m_values,
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata);

	static AosValueSelObjPtr getValueSelectorStatic(
					const AosValueSelType::E type);
};
#endif

