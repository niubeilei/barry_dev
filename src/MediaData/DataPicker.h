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
// 12/24/2010	Created by Jackie
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_MediaData_DataPicker_h
#define Aos_MediaData_DataPicker_h

#include "MediaData/Ptrs.h"
#include "MediaData/DataPickerIds.h"
#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosDataPicker : public OmnRCObject
{
	OmnDefineRCObject;
private:
	AosDataPickerId::E 	mType;

public:
	AosDataPicker(
				const OmnString &name, 
				const AosDataPickerId::E type, 
				const bool regflag);

	virtual bool pickData(
				const OmnString &data,
				OmnString &rslts, 
				const AosXmlTagPtr &sdoc,
				const AosRundataPtr &rdata){return true;}

	virtual bool pickData2(
				const OmnString &data,
				AosXmlTagPtr &rslts, 
				const AosXmlTagPtr &sdoc,
				const AosRundataPtr &rdata){return true;}

	static bool pickDataStatic2(
				const OmnString &algorithm,
				const OmnString &data,
				AosXmlTagPtr &rslts, 
				const AosXmlTagPtr &sdoc,
				const AosRundataPtr &rdata);

	static bool pickDataStatic(
				const OmnString &algorithm,
				const OmnString &data,
				OmnString &rslts, 
				const AosXmlTagPtr &sdoc,
				const AosRundataPtr &rdata);

private:
	bool registerDataPicker(const AosDataPickerPtr &picker, const OmnString &name);
};
#endif

