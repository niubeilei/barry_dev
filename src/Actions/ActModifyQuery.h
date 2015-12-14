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
// 04/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActModifyQuery_h
#define Aos_SdocAction_ActModifyQuery_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "Rundata/Rundata.h"
#include <vector>


class AosActModifyQuery : virtual public AosSdocAction
{
	struct modifyquery
	{
		AosXmlTagPtr 	config;
		AosXmlTagPtr 	valuesel;
		AosXmlTagPtr 	dest_query_config;
		OmnString		arith;
		OmnString		valuefrom;
		OmnString		dest_attrname;
		OmnString		source_attrname;
	};

	enum Type 
	{
		eInvalid,

		eAdd,
		eSubtract,
		eMultiply,
		eDivide,
		
		eMax
	};

public:
	AosActModifyQuery(const bool flag);
	~AosActModifyQuery();

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;
private:
	bool			parseConfig(const AosXmlTagPtr &config, struct modifyquery &bq, const AosRundataPtr &rdata);
	bool			suntractFunc(AosXmlTagPtr &dest, const OmnString &attrname, const int &value, const AosRundataPtr &rdata);
	bool			modifyDoc(const struct modifyquery &mq, AosXmlTagPtr &dest, const int &value, const AosRundataPtr &rdata);
	AosXmlTagPtr	getQueryRslt(const AosXmlTagPtr &xml, const AosRundataPtr &rdata);
	int				getValue(const struct modifyquery &mq, const AosRundataPtr &rdata);
	
private:
	Type toEnum(const OmnString &opr)
	{
		if (opr == "+") return eAdd;
		if (opr == "-") return eSubtract;
		if (opr == "*") return eMultiply;
		if (opr == "/") return eDivide;
		return eInvalid;
	}
};
#endif

