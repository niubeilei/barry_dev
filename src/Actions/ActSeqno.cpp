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
#include "Actions/ActSeqno.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DocSelObj.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/OmnNew.h"
#include "Util/DynArray.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include <string>
#include <vector>
using namespace std;

const OmnString sgSeqnoOnly 		= "seqnoonly";
const OmnString sgWithPrefix 		= "withprefix";

AosActSeqno::AosActSeqno(const bool flag)
:
AosSdocAction(AOSACTTYPE_SEQNO, AosActionType::eSeqno, flag)
{
}


/*
AosActSeqno::AosActSeqno(const AosXmlTagPtr &def)
:
AosSdocAction(AosActionType::eSeqno, false),
mLock(OmnNew OmnMutex())
{
	config(def);
}


bool
AosActSeqno::config(const AosXmlTagPtr &def)
{
	// The 'def' should be in the form:
	// 	<seqno_act type="seqno"
	// 		AOSTAG_PREFIX_METHOD="xxx">
	// 		...
	// 	</seqno_act>
	aos_assert_r(def, false);
	mPrefixMethod = toPrefixEnum(def->getAttrStr(AOSTAG_PREFIX_METHOD));
	
	return true;
}
*/


AosActSeqno::~AosActSeqno()
{
}


bool	
AosActSeqno::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This action seqno to doc;
	// <seqno_act zky_initvalue="xxx" zky_prefxmd="withprefix" 
	// 	 seqno_aname"seqan" zky_seqnopx="LO">
	// 		<docselector AOSTAG_DOCSELECTOR_TYPE="targetdoc"/>
	// </seqno_act>
	if (!sdoc)
	{
		rdata->setError() << "Missing smartdoc: " << rdata->toString(1);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr selector = sdoc->xpathGetChild(AOSTAG_DOCSELECTOR);
	if (!selector)
	{
		rdata->setError() << "Missing doc selector";
		return false;
	}

	AosXmlTagPtr targetDoc = AosRunDocSelector(rdata, selector);
	if (!targetDoc)
	{
		rdata->setError() << "Missing source doc!";
		return false;
	}

	OmnString seqnoStr;
	bool rslt = getSeqno(seqnoStr, sdoc, rdata);
	aos_assert_r(rslt, false);
	OmnString target_aname = sdoc->getAttrStr(AOSTAG_SEQNO_TNAME, "");
	if (target_aname == "")
	{
		rdata->setError();
		rdata->setErrmsg("Missing the target attribute name!");
		return false;
	}

	targetDoc->setAttr(target_aname, seqnoStr);
	return true;
}


bool
AosActSeqno::getSeqno(
		OmnString &seqnoStr,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata,
		const int seq_num)
{
	PrefixMethod prefix_method = toPrefixEnum(sdoc->getAttrStr(AOSTAG_PREFIX_METHOD));
	switch (prefix_method)
	{
	case eSeqnoOnly:
		 return createSeqnoOnly(sdoc, seqnoStr, rdata, seq_num);

	case eWithPrefix:
		 return createSeqnoWithPrefix(sdoc, seqnoStr, rdata, seq_num);

	default:
		 rdata->setError() << "Unrecognized prefix type!";
		 return false;
	}

	return false;
}


bool
AosActSeqno::createSeqnoOnly(
		const AosXmlTagPtr &sdoc,
		OmnString &seqnoStr,
		const AosRundataPtr &rdata,
		const int seq_num)
{
	seqnoStr= "";
	u64 seqno = 0;
	bool rslt = retrieveSeqno(sdoc, seqno, rdata, seq_num);
	if (!rslt) return false;
	seqnoStr << seqno;
	rdata->setOk();
	return true;
}


bool
AosActSeqno::createSeqnoWithPrefix(
		const AosXmlTagPtr &sdoc,
		OmnString &seqnoStr,
		const AosRundataPtr &rdata,
		const int seq_num)
{
	if (!sdoc)
	{
		rdata->setError();
		rdata->setErrmsg("Missing smartdoc!");
		return false;
	}

	OmnString prefix = sdoc->getAttrStr(AOSTAG_SEQNO_PREFIX);
	if (prefix == "")
	{
		rdata->setError();
		rdata->setErrmsg("Missing seqno prefix!");
		return false;
	}
	seqnoStr = prefix;

	u64 seqno = 0;
	bool rslt = retrieveSeqno(sdoc, seqno, rdata, seq_num);
	if (!rslt) return false;

	seqnoStr << "_" << seqno;
	
	rdata->setOk();
	return true;
}


bool 
AosActSeqno::retrieveSeqno(
		const AosXmlTagPtr &sdoc,
		u64 &seqno,
		const AosRundataPtr &rdata,
		const int seq_num)
{
	// It creates a pure seqno. The seqno is stored in the IIL 
	// AOSZTG_SEQNO. The seqno ID is stored in the smartdoc
	// through the attribute AOSTAG_SEQNOONLY_ANAME. If it is
	// not there, it will create the seqno and modify the sdoc.
	//
	// In the current implementations, each smartdoc will define
	// a new, unique seqno, that is, different smartdocs 
	// will create different seqno.
	//
	// new feature, smtdoc support more seqno...
	// <sdoc xxxx>
	//   <seqno seq_idx="1" iilKey="$Systime"></seqno>
	//   <seqno seq_idx="2" iilkey="$Systime"></seqno>
	// </sdoc>
	
	// Chen Ding, 03/02/2012
	AosXmlTagPtr root = sdoc->getRoot();
	if (!root) root = sdoc;
	// AosXmlTagPtr sdocRoot = sdoc->getRoot()->getFirstChild();
	u64 iilkey = root->getAttrU64(AOSTAG_DOCID, 0);	
	//u64 iilkey = sdoc->getAttrU64(AOSTAG_DOCID, 0);
	u64 initValue = sdoc->getAttrU64(AOSTAG_INITVALUE, 0);
	u64 incValue = sdoc->getAttrU64(AOSTAG_INCVALUE, 1);

	bool saveTag = false;
	if (seq_num != 0)
	{
		AosXmlTagPtr seqnoxml = sdoc->getFirstChild("seqno");
		OmnString iilKeyTmp;
		while (seqnoxml)
		{
			int seq_idx = seqnoxml->getAttrInt("seq_idx", -1);
			if (seq_num == seq_idx)
			{
				iilkey = seqnoxml->getAttrU64("iilKey", 0);
				if (iilkey == 0)
				{
					saveTag = true;
					struct timeval tt;
					gettimeofday(&tt, 0);
					iilkey = tt.tv_sec;
					sleep(1);
					iilKeyTmp << iilkey;
					seqnoxml = seqnoxml->clone(AosMemoryCheckerArgsBegin);
					seqnoxml->setAttr("iilKey", iilKeyTmp);
				}
				initValue = seqnoxml->getAttrU64(AOSTAG_INITVALUE, 0);
			}
			seqnoxml = sdoc->getNextChild();
		}
		if (saveTag)
		{
			AosXmlTagPtr ssdoc = sdoc;
			if (sdoc->getAttrU64(AOSTAG_DOCID, 0) == 0)
			{
				ssdoc = sdoc->getRoot();
				if (ssdoc)
				{
					ssdoc = ssdoc->getFirstChild();
				}
			}
			aos_assert_r(ssdoc, false);
			AosDocClientObj::getDocClient()->modifyAttrStr1(rdata,
					          	ssdoc->getAttrU64(AOSTAG_DOCID, 0), 
							  	ssdoc->getAttrStr(AOSTAG_OBJID), 
								"iilKey", iilKeyTmp, "", false, false, true);
		}
	}

	//bool rslt = AosIILClient::getSelf()->incrementDocid(AOSZTG_SDOC_SEQNO,
	//		false, iilkey, incValue, initValue, seqno, rdata);
	bool rslt = AosIncrementKeyedValue(AOSZTG_SDOC_SEQNO,
			iilkey, seqno, true, incValue, initValue, true, 0, rdata);
	if (!rslt)
	{
		rdata->setError();
		rdata->setErrmsg("Failed to update the database!");
		return false;
	}

	return true;
}


AosActSeqno::PrefixMethod
AosActSeqno::toPrefixEnum(const OmnString &name)
{
	if (name == sgSeqnoOnly) 	return eSeqnoOnly;
	if (name == sgWithPrefix)	return eWithPrefix;
	return eInvalid;
}


AosActionObjPtr
AosActSeqno::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActSeqno(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


