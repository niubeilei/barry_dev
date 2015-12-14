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
// 05/10/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_RandomValueGen_RVGStrEnum_h
#define Omn_RandomValueGen_RVGStrEnum_h

#include "CliTorturer/Ptrs.h"
#include "Random/Ptrs.h"
#include "Parms/RVGStr.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/ValList.h"
#include "Util/DynArray.h"


class AosRVGStrEnum : public AosRVGStr
{
	OmnDefineRCObject;

private:
	AosRandomSelStrPtr	mValueSelector;

public:
	AosRVGStrEnum(const AosCliCmdTorturerPtr &cmd);
	virtual ~AosRVGStrEnum();

	static AosRVGStrEnumPtr createInstance(
									const AosCliCmdTorturerPtr &cmd,
									const OmnXmlItemPtr &def, 
									const OmnString &cmdTag,
									OmnVList<AosGenTablePtr> &tables);
	virtual bool 	config(const OmnXmlItemPtr &def, 
							const OmnString &cmdTag,
							OmnVList<AosGenTablePtr> &tables);
	virtual AosRVGType getRVGType() const {return eAosRVGType_StrEnum;}
	virtual OmnString getNextValueFuncCall() const;

	bool	addGoodEntry(const OmnString &e);
	virtual OmnString genValue();
	virtual OmnString genIncorrectValue(
				AosRVGReturnCode &rcode,
            	OmnString &errmsg);
	bool	configSelector(const OmnXmlItemPtr &selectorDef);

private:
	// bool	createSelectors();
};

#endif

