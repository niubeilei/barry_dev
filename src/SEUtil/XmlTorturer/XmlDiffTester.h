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
// 2010/10/23	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEUtil_XmlTorturer_XmlDiffTester_h
#define Aos_SEUtil_XmlTorturer_XmlDiffTester_h

#include "SEUtil/XmlTag.h"
#include "Util/Opr.h"
#include "SearchEngine/Ptrs.h"
#include "Tester/TestPkg.h"
#include <map>
#include <vector>
#include <deque>

using namespace std;


class AosXmlDiffTester : public OmnTestPkg
{

public:
	enum 
	{
		eNodeName = 'n',
		eNodeText = 't'
	};
	AosXmlDiffTester();
	~AosXmlDiffTester() {};

	virtual bool		start();
	AosXmlTagPtr        createXml();

private:
	AosXmlTagPtr mRandTag;
	int mRandNum;
	int mXmlTagNum;
	map<OmnString, pair<OmnString, OmnString> > difftemp;
	map<OmnString, pair<OmnString, OmnString> > diffproc;
	bool basicTest();
	AosXmlTagPtr modifyXmlRand(AosXmlTagPtr oldxml);
	AosXmlTagPtr modifyTagName(const AosXmlTagPtr oldxml, AosXmlTagPtr newxml);     
	AosXmlTagPtr modifyNodeText(const AosXmlTagPtr oldxml, AosXmlTagPtr newxml);
	AosXmlTagPtr modifyAttr(const AosXmlTagPtr oldxml, AosXmlTagPtr newxml);
	AosXmlTagPtr modifyNode(const AosXmlTagPtr oldxml, AosXmlTagPtr newxml);
	bool checkXmlsDiff(AosXmlTagPtr oldxml, AosXmlTagPtr newxml);
	void getPathRand(OmnString &path, char type, const AosXmlTagPtr oldxml);
	void getElementList(const AosXmlTagPtr parentxml, int stopNum);
	void AosRandomLetterStr11(const int len, char *data);
};
#endif
