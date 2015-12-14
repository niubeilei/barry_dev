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
// Modification History:
// 01/17/2014	Created by Young
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlStmtSequence.h"

#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"

AosJqlStmtSequence::AosJqlStmtSequence()
:
mSequenceInfo(0)
{
}


AosJqlStmtSequence::~AosJqlStmtSequence()
{
}


bool
AosJqlStmtSequence::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	// 2015/7/28 JIMODB-77
	bool rslt = checkDatabase(rdata);
	if (!rslt) return false;

	if (mOp == JQLTypes::eCreate) return doCreate(rdata);

	return true;
}



AosJqlStatement *
AosJqlStmtSequence::clone()
{
	return OmnNew AosJqlStmtSequence(*this);
}


void 
AosJqlStmtSequence::dump()
{
}


bool
AosJqlStmtSequence::doCreate(const AosRundataPtr &rdata)
{
	if (!mSequenceInfo)
	{
		rdata->setJqlMsg("Not OK");
		return false;
	}

	OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eSequenceDoc, mName);

	map<string, string> attrs = mSequenceInfo->getAttrs();	
	OmnString conf;
	conf << "<doc zky_objid=\"" << objid << "\" zky_name=\"" << mName << "\" ";
	for (map<string, string>::iterator itr = attrs.begin();
			itr != attrs.end(); itr++)
	{
		conf << " " << itr->first << "=\"" << itr->second << "\"";
	}
	conf << ">";
	conf << "</doc>";

	OmnScreen << "===============================================" << endl;
	OmnScreen << conf << endl;

	AosXmlTagPtr doc = AosXmlParser::parse(conf AosMemoryCheckerArgs);
	if ( !createDoc(rdata, JQLTypes::eSequenceDoc, mName, doc))
	{
		rdata->setJqlMsg("Not OK");
		return false;
	}

	rdata->setJqlMsg("OK");
	return true;
}


void
AosJqlStmtSequence::setSequenceInfo(AosJQLSequenceInfo* info)
{
	mSequenceInfo = info;
}
