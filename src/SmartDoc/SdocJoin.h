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
#ifndef Aos_SDocJoin_SDocJoin_h
#define Aos_SDocJoin_SDocJoin_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Actions/ActAddAttr.h"
#include "Actions/SdocAction.h"
#include "XmlUtil/XmlTag.h"
#include "XmlInterface/Server/Ptrs.h"
#include <vector>



class AosSdocJoin : public AosSmartDoc
{
private:
	vector<AosActionObjPtr>	mActions;
	AosSdocOpr::E				mOperator;
	OmnString 					mJoinAttr;
	OmnString 					mJoinType;
	AosXmlTagPtr				mAccessedDoc;
	
	enum 
	{
		eMaxItems = 100
	};

public:
	AosSdocJoin(const bool flag);
	~AosSdocJoin();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocJoin(false);}
	virtual	bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

private:
	bool modifyDoc(const AosRundataPtr &rdata);
	bool parseJoin(
			const AosXmlTagPtr &sdoc, 
			const AosRundataPtr &rdata);

	bool query(
			const AosQueryReqObjPtr &query,
			const AosXmlTagPtr &query_xml,
			const AosRundataPtr &rdata);

	bool runPriv(const AosRundataPtr &rdata,
			bool tag);

	bool nextDoc(
		const AosXmlTagPtr &queryResultXml,
		AosXmlTagPtr &sourceXml,
		AosXmlTagPtr &targetXml,
		const AosRundataPtr &rdata);

	bool nextDoc(
			const AosXmlTagPtr &queryResultXml,
			AosXmlTagPtr &xml,
			const AosRundataPtr &rdata);

	bool init(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

	AosXmlTagPtr getAccessedDoc() const {return mAccessedDoc;}

};
#endif
