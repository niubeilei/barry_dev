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
// 2015/03/25 Created by Arvin Jiang
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoParser_Parserlet_H
#define AOS_JimoParser_Parserlet_H

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "SEInterfaces/JimoParserObj.h"
#include "SEInterfaces/Ptrs.h"
#include "JimoParser/Ptrs.h"
#include <hash_map>
#include <vector>


class OmnMutex;
class AosJimoParser;

class AosParserlet : virtual public OmnRCObject
{
	OmnDefineRCObject;

protected:
	typedef hash_map<const OmnString, AosParserletPtr, Omn_Str_hash, compare_str> map_t;
	typedef hash_map<const OmnString, AosParserletPtr, Omn_Str_hash, compare_str>::iterator itr_t;

	map_t 						mParserlets;
	vector<OmnString>			mKeywords;
	vector<AosJimoParserObjPtr>	mGenObjs;
	
	OmnMutex *					mLock;

public:
	AosParserlet();
	AosParserlet(const vector<OmnString> &keywords);
	// AosParserlet(const vector<OmnString> &word,
	// 				const AosJimoParserObjPtr &genobj);
	~AosParserlet();

//	bool parse(AosRundata *rdata, AosJimoParser *parser, bool &parsed);
	
	bool configJimoParserObjs(AosRundata *rdata, 
			const AosXmlTagPtr &def);

	bool config(AosRundata *rdata, const AosXmlTagPtr &conf);

	bool run(AosRundata *rdata,
						const OmnString &verb_name,
						AosJimoParserObj *parser);

	AosJimoParserObjPtr loadJimoParserObj(AosRundata *rdata, 
						const OmnString &jimo_name, 
						const int version);

	bool getParserlet(AosRundata *rdata, 
						const OmnString &word, 
						AosParserletPtr &parserlet, 
						vector<AosJimoParserObjPtr> &genobjs);

	AosParserletPtr  getParserlet(AosRundata *rdata, 
						const OmnString &word);

	bool configParserlets(AosRundata *rdata, const AosXmlTagPtr &def);
	bool sanityCheck(AosRundata *rdata, const vector<OmnString> &keywords);

	bool appendJimoParserObj(AosRundata *rdata, 
						const OmnString &jimo_name);

	bool appendJimoParserObj(AosRundata *rdata, 
						const AosJimoParserObjPtr &genobj);
	
	void appendKeyword(const OmnString &keyword)
	{
		mKeywords.push_back(keyword);
	}

	vector<OmnString> getKeywords(AosRundata *radata);
	
	bool registerParserlet(
						AosRundata *rdata, 
						const vector<OmnString> &keywords, 
						int &idx, 
						const OmnString &jimo_name, 
						const int version,
						const bool override_flag);

private:
	AosParserletPtr loadParserlet(AosRundata *rdata, const OmnString &jimo_name);
};

#endif
