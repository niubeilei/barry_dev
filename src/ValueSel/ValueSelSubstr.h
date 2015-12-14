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
// 05/30/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ValueSel_ValueSelSubstr_h
#define Aos_ValueSel_ValueSelSubstr_h

#include "ValueSel/ValueSel.h"

class AosValueSelSubstr : public AosValueSel
{
public:
	enum Type
	{
		eInvalid,

		eSubstr,
		ePick,
		eCompose,

		eMax
	};

private:
	int			mStartPos;
	int			mLength;
	bool		mCopyFlag;
	Type		mType;

public:
	AosValueSelSubstr(const bool reg);
	AosValueSelSubstr(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	~AosValueSelSubstr();

	virtual bool run(
					AosValueRslt    &valueRslt,
					const AosXmlTagPtr &item,
					const AosRundataPtr &rdata);

	virtual bool run(
					AosValueRslt &value,
					const char *record, 
					const int record_len, 
					const AosRundataPtr &rdata);

	virtual AosValueSelObjPtr clone(
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata);
					
private:
	bool getValuePriv(
					const char *data, 
					const int data_len,
					const OmnString &separator, 
					const int field_idx, 
					AosValueRslt &value, 
					const bool copy_memory_flag,
					const AosRundataPtr &rdata);

	bool getValuePriv(
					const char *data, 
					const int data_len,
					const int start_pos, 
					const int len, 
					const bool copy_memory_flag,
					AosValueRslt &value, 
					const AosRundataPtr &rdata);

	bool config(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	bool configPick(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	bool configCompose(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	Type toEnum(const OmnString &name)
	{
		if (name == "substr") return eSubstr;
		if (name == "pick") return ePick;
		if (name == "compose") return eCompose;
		return eInvalid;
	}
	bool isValidType(const Type t) {return t > eInvalid && t < eMax;}
};
#endif

