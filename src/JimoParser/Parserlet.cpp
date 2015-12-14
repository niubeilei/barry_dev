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
#include "JimoParser/Parserlet.h"
#include "JimoParser/JimoParser.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"

#include "DataProc/DataProc.h"



AosParserlet::AosParserlet()
:
mLock(OmnNew OmnMutex())
{
}

AosParserlet::AosParserlet(const vector<OmnString> &keywords)
:
mLock(OmnNew OmnMutex())
{
	mKeywords = keywords;
}


/*
AosParserlet::AosParserlet(
		const vector<OmnString> &keywords, 
		const AosJimoParserObjPtr &genobj)
:
mLock(OmnNew OmnMutex())
{
	mKeywords = keywords;
	genobj->setKeywords(keywords);
	mGenObjs.push_back(genobj);
}
*/


AosParserlet::~AosParserlet()
{
}


bool
AosParserlet::config(
		AosRundata *rdata, 
		const AosXmlTagPtr &def) 
{
	// 'def' is in the form:
	// Each parserlet is defined as:
	// 	<parserlet keyword="xxx">
	// 		<generic_objs>
	// 			<generic_obj jimo_name="xxx" version="ddd"/>
	// 			<generic_obj jimo_name="xxx" version="ddd"/>
	// 			...
	// 		</generic_objs>
	// 		<parserlets>
	// 			<parserlet .../>
	// 			<parserlet .../>
	// 			...
	// 		</parserlets>
	// 	</parserlet>
	//
	//
	// Example:
	// 	Create DataProc name(name=value, ...);
	//
	// Its configurations are:
	// 	<parserlet keyword="create">
	// 		<parserlets>
	// 			<parserlet keyword="dataproc">
	// 				<generic_objs>
	// 					<generic_obj jimo_name="create_dataproc"/>
	// 				</generic_objs>
	// 			</parserlet>
	// 		</parserlGets>
	// 	</parserlet>
//	OmnString keyword = def->getAttrStr("keyword");
//	if (keyword != "")
//	{
//		mKeywords.push_back(keyword);
//	}
	bool rslt = configJimoParserObjs(rdata, def);
	if (!rslt)
	{
		AosLogError(rdata, true, "invalid_configurations") 
			<< AosFN("Config") << def << enderr;
		return false;
	}

	rslt = configParserlets(rdata, def);
	if (!rslt)
	{
		AosLogError(rdata, true, "invalid_configurations") 
			<< AosFN("Config") << def << enderr;
		return false;
	}
	return true;
}


bool
AosParserlet::configParserlets(
		AosRundata *rdata, 
		const AosXmlTagPtr &def)
{
	AosXmlTagPtr parserlets = def->getFirstChild("parserlets");
	if (!parserlets) return true;

	AosXmlTagPtr tag = parserlets->getFirstChild();
	if (!tag)
	{
		AosLogError(rdata, true, "missing_parserlet_tag")
			<< AosFN("Definition") << def << enderr;
		return false;
	}

	while (tag)
	{
		// Each parserlet is defined as:
		// 	<parserlet keyword="xxx">
		// 		<generic_objs>
		// 			...
		// 		</generic_objs>
		// 		<parserlets>
		// 			...
		// 		</parserlets>
		// 	</parserlet>
		OmnString keyword = tag->getAttrStr("keyword");
		if (keyword == "")
		{
			AosLogError(rdata, true, "missing_keyword") 
				<< AosFN("Definition") << def << enderr;
			return false;
		}

		AosParserletPtr parserlet = getParserlet(rdata, keyword);
		if (parserlet)
		{
			AosLogError(rdata, true, "parserlet_already_exist") 
				<< AosFN("Configuration") << def << enderr;
			return false;
		}

		// This is a new Parserlet Node
			
		parserlet = OmnNew AosParserlet();
		for(size_t i = 0 ; i < mKeywords.size(); i++ )
		{
			parserlet->appendKeyword(mKeywords[i].toLower());
		}
		parserlet->appendKeyword(keyword.toLower());
		if (!parserlet->config(rdata, tag))
		{
			AosLogError(rdata, true, "failed_config_parserlet")
				<< AosFN("Configuration") << def << enderr;
			return false;
		}
		mParserlets[keyword] = parserlet;
		tag = parserlets->getNextChild();
	}

	return true;
}


bool
AosParserlet::configJimoParserObjs(
		AosRundata *rdata, 
		const AosXmlTagPtr &def)
{
	AosXmlTagPtr genobjs = def->getFirstChild("generic_objs");
	if (!genobjs) 
	{
		// This parserlet does not have generic objs, which is ok
		return true;
	}

	AosXmlTagPtr tag = genobjs->getFirstChild();
	if (!tag)
	{
		AosLogError(rdata, true, "missing_generic_obj_tag") 
			<< AosFN("Definition") << def << enderr;
		return false;
	}

	while (tag)
	{
		OmnString jimo_name = tag->getAttrStr("jimo_name");
		if (jimo_name == "")
		{
			AosLogError(rdata, true, "missing_jimo_name") 
				<< AosFN("Definition") << def << enderr;
			return false;
		}

		int version = tag->getAttrInt("version", 1);

		// This is a leave node. Need to add:
		// 		[keyword, jimo]
		AosJimoParserObjPtr genobj = loadJimoParserObj(rdata, jimo_name, version);
		if (!genobj)
		{
			AosLogError(rdata, true, "generic_obj_not_defined") 
				<< AosFN("Definition") << def << enderr;
			return false;
		}
		genobj->setKeywords(mKeywords);
		mGenObjs.push_back(genobj);
		tag = genobjs->getNextChild();
	}
	return true;
}


/*
AosParserletPtr
AosParserlet::addParserlet(
		AosRundata *rdata, 
		const OmnString &prefix,
		const OmnString &keyword)
{
	mLock->lock();
	itr_t itr = mParserlets.find(keyword);
	if (itr != mParserlets.end())
	{
		AosParserlet *parserlet = itr->second.getPtr();
		mLock->unlock();
		return parserlet;
	}

	AosParserlet *parserlet = OmnNew AosParserlet(prefix);
	mParserlets[keyword] = parserlet;
	mLock->unlock();
	return parserlet;
}
*/


bool 
AosParserlet::sanityCheck(AosRundata *rdata, const vector<OmnString> &keywords)
{
	// 'keywords' should contain the same keywords with mKeywords except that
	// 'keywords' has one more keyword. 
	aos_assert_rr(mKeywords.size() + 1 == keywords.size(), rdata, false); 
	for (u32 i=0; i<mKeywords.size(); i++)
	{
		aos_assert_rr(mKeywords[i] == keywords[i], rdata, false);
	}
	return true;
}


bool
AosParserlet::appendJimoParserObj(
		AosRundata *rdata, 
		const OmnString &jimo_name)
{
	AosJimoParserObjPtr genobj = loadJimoParserObj(rdata, jimo_name,1);
	if (!genobj)
	{
		mLock->unlock();
		AosLogError(rdata, true, "generic_obj_not_found") 
			<< AosFN("Jimo Name") << jimo_name << enderr;
		return false;
	}
	
	mLock->lock();
	genobj->setKeywords(mKeywords);
	mGenObjs.push_back(genobj);
	mLock->unlock();
	return true;
}


bool
AosParserlet::appendJimoParserObj(
		AosRundata *rdata, 
		const AosJimoParserObjPtr &genobj)
{
	mLock->lock();
	genobj->setKeywords(mKeywords);
	mGenObjs.push_back(genobj);
	mLock->unlock();
	return true;
}


/*
bool
AosParserlet::addJimoParserObj(
		AosRundata *rdata, 
		const vector<OmnString> &keywords,
		const OmnString &jimo_name) 
{
	// This function adds a generic obj to this node. 
	
	aos_assert_rr(keywords.size() > 0, rdata, false);
	bool rslt = sanityCheck(rdata, keywords);
	aos_assert_rr(rslt, rdata, false);

	AosJimoParserObjPtr genobj = loadJimoParserObj(rdata, jimo_name,1);
	if (!genobj)
	{
		mLock->unlock();
		AosLogError(rdata, true, "generic_obj_not_found") 
			<< AosFN("Jimo Name") << jimo_name << enderr;
		return false;
	}

	OmnString word = keywords[keywords.size() - 1];
	mLock->lock();
	itr_t itr = mParserlets.find(word);
	if (itr != mParserlets.end())
	{
		itr->second->appendJimoParserObj(rdata, genobj);
		mLock->unlock();
		return true;
	}

	AosParserlet *parserlet = OmnNew AosParserlet(keywords, genobj);
	mParserlets[word] = parserlet;
	mLock->unlock();
	return true;
}
*/


AosJimoParserObjPtr
AosParserlet::loadJimoParserObj(
		AosRundata *rdata, 
		const OmnString &jimo_name, 
		const int version)
{
	AosJimoPtr jimo = AosCreateJimoByClassname(rdata, jimo_name, version);
	if (!jimo) 
	{
		AosLogError(rdata, true, "jimo_not_found")
			<< AosFN("JimoName") << jimo_name << enderr;
		return 0;
	}

	AosJimoParserObjPtr genobj = dynamic_cast<AosJimoParserObj *>(jimo.getPtr());
	if (!genobj)
	{
		AosLogError(rdata, true, "not_generic_obj")
			<< AosFN("JimoName") << jimo_name << enderr;
		return 0;
	}
	genobj->setJimoName(jimo_name);
	return genobj;
}


bool
AosParserlet::getParserlet(
		AosRundata *rdata, 
		const OmnString &word, 
		AosParserletPtr &parserlet, 
		vector<AosJimoParserObjPtr> &genobjs)
{
	parserlet = 0;

	mLock->lock();
	itr_t itr = mParserlets.find(word);
	if (itr == mParserlets.end())
	{
		mLock->unlock();
		return true;
	}

	parserlet = itr->second.getPtr();
	if(parserlet)
	{
		AosJimoParserObjPtr genobj = NULL;
		for(size_t i = 0; i < parserlet->mGenObjs.size(); i++)
		{
			//genobj = dynamic_cast<AosJimoParser*>((parserlet->mGenObjs)[i]->cloneJimo().getPtr());
			genobj = dynamic_cast<AosJimoParserObj*>((parserlet->mGenObjs)[i]->cloneJimo().getPtr());
			genobjs.push_back(genobj);
		}
		//genobjs = parserlet->mGenObjs;
	}	
	mLock->unlock();
	return true;
}

AosParserletPtr
AosParserlet::getParserlet(AosRundata *rdata, 
						const OmnString &word)
{
	mLock->lock();
	itr_t itr = mParserlets.find(word);
	if(itr == mParserlets.end())
	{
		mLock->unlock();
		return 0;
	}
	AosParserletPtr parserlet = itr->second.getPtr();
	mLock->unlock();
	return parserlet;
}

vector<OmnString> 
AosParserlet::getKeywords(AosRundata *radata)
{
	return mKeywords;
}


bool
AosParserlet::registerParserlet(
		AosRundata *rdata, 
		const vector<OmnString> &keywords, 
		int &idx, 
		const OmnString &jimo_name, 
		const int version,
		const bool override_flag)
{
	aos_assert_rr(idx >= 0 && (u32)idx <= keywords.size(), rdata, false);

	if ((u32)idx == keywords.size())
	{
		// It is to add the jimo.
		aos_assert_rr(jimo_name != "", rdata, false);
		aos_assert_rr(version >= 0, rdata, false);
		AosJimoParserObjPtr genobj = loadJimoParserObj(rdata, jimo_name, version);
		if (!genobj)
		{
			AosLogError(rdata, true, "generic_obj_not_defined") 
				<< AosFN("JimoName") << jimo_name << enderr;
			return false;
		}

		mLock->lock();
		// Check whether it has been defined
		for (u32 i=0; i<mGenObjs.size(); i++)
		{
			if (mGenObjs[i]->getJimoName() == jimo_name)
			{
				if (!override_flag)
				{
					AosLogError(rdata, true, "generic_obj_already_exist") 
						<< AosFN("JimoName") << enderr;
					mLock->unlock();
					return false;
				}
				mGenObjs[i] = genobj;
				mLock->unlock();
				return true;
			}
		}

		genobj->setKeywords(mKeywords);
		mGenObjs.push_back(genobj);
		mLock->unlock();
		return true;
	}

	mLock->lock();
	itr_t itr = mParserlets.find(keywords[idx]);
	AosParserletPtr parserlet;
	if(itr == mParserlets.end())
	{
		parserlet = OmnNew AosParserlet(mKeywords);
		mParserlets[keywords[idx]] = parserlet;
	}
	else
	{
		parserlet = itr->second;
	}
	mLock->unlock();

	idx++;
	return parserlet->registerParserlet(rdata, keywords, idx, 
				jimo_name, version, override_flag);
}


