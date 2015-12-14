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
// 2014/07/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoProg/JimoProg.h"

#include "API/AosApi.h"
#include "JimoProg/JimoProgLoadData.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/JimoLogicObj.h"
#include "Thread/Mutex.h"


/*
extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosJimoProg_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosJimoProg(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}

*/


AosJimoProg::AosJimoProg()
:
AosJimoProgObj(1),
mLock(OmnNew OmnMutex())
{
}

AosJimoProg::AosJimoProg(OmnMutexPtr lock)
:
AosJimoProgObj(1),
mLock(lock)
{
}

/*
AosJimoProg::AosJimoProg(const int version)
:
AosJimoProgObj(version),
mLock(OmnNew OmnMutex())
{
}
*/


AosJimoProg::~AosJimoProg()
{
}

AosJimoProgObjPtr
AosJimoProg::createJimoProg(
		const AosXmlTagPtr &def,
		AosRundata *rdata)
{
	OmnNotImplementedYet;
	return 0;
}

/*
bool
AosJimoProg::addJimoLogic(
		const AosXmlTagPtr &descriptor,
		AosRundata *rdata)
{
	AosJimoLogicObjPtr logic = AosCreateJimoLogic(descriptor, rdata);
	if (!logic)
	{
		AosSetErrorUser3(rdata, "syntax_error", rdata->getErrmsg());
		return false;
	}

	mLock->lock();
	mJimoLogic.push_back(logic);
	mLock->unlock();
	return true;
}
*/


bool
AosJimoProg::addDataProc(AosRundata *rdata, const OmnString &, const OmnString &)
{
	return false;
}


AosJimoProgObjPtr
AosJimoProg::createLoadDataProg(
		AosRundata *rdata,
		const AosXmlTagPtr &inputds,
		const AosXmlTagPtr &tabledoc,
		std::list<string> &fields,
		JQLTypes::OpType &op)
{
	return OmnNew AosJPLoadData(rdata, inputds, tabledoc, fields, op);
}


AosXmlTagPtr
AosJimoProg::createConfig()
{
	OmnShouldNeverComeHere;
	return 0;
}


bool 
AosJimoProg::run(AosRundata *rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosJimoProg::addSchema(
		AosRundata *rdata, 
		const OmnString &name,
		const AosJimoLogicObjNewPtr &schema)
{
	mSchemas.push_back(SchemaEntry(name, schema));
	return false;
}

bool
AosJimoProg::addJimoLogicNew(
		AosRundata *rdata, 
		OmnString &name, 
		AosJimoLogicObjNewPtr &jimologic)
{
	//felicia, 2015/08/06 for bug JIMODB-252 
	map<OmnString, AosJimoLogicObjNewPtr>::iterator itr = mJimoLogics.find(name);
	if (itr != mJimoLogics.end())
	{
		// The jimo logic already exists.
		OmnString err = "[ERR] : ";
	    err << "Duplicated DataProc name " << name;
		rdata->setJqlMsg(err);
		return false;		
	}

	mJimoLogics[name] = jimologic;
	return true;
}

AosJimoLogicObjNew *
AosJimoProg::getJimoLogic(
		AosRundata *rdata, 
		const OmnString &name)
{
	map<OmnString, AosJimoLogicObjNewPtr>::iterator itr = mJimoLogics.find(name);
	if (itr == mJimoLogics.end()) return 0;
	return itr->second.getPtr();
}


bool
AosJimoProg::saveLogicDoc(
		AosRundata *rdata, 
		const OmnString &objid, 
		const OmnString &conf)
{
	aos_assert_r(conf != "", false);
	AosXmlTagPtr xml = AosXmlParser::parse(conf AosMemoryCheckerArgs);
	aos_assert_r(xml, false);
	mLogicConfs[objid] = xml;
	return true;
}


AosXmlTagPtr 
AosJimoProg::getLogicDoc(
		AosRundata *rdata, 
		const OmnString &objid)
{
	if (mLogicConfs.count(objid)) 
	{
		//return AosXmlParser::parse(mLogicConfs[objid] AosMemoryCheckerArgs);
		return mLogicConfs[objid];
	}
	return NULL;
}

bool 		
AosJimoProg::setUserVarValue(
		AosRundata *rdata, 
		const OmnString &varname, 
		const AosValueRslt &value)
{
	OmnNotImplementedYet;
	return true;
}


AosValueRslt 
AosJimoProg::getUserVarValue(
		AosRundata *rdata, 
		const OmnString &varname)
{
	OmnNotImplementedYet;
	return AosValueRslt();
}


bool        
AosJimoProg::addUserVar(AosRundata *rdata,               
		const OmnString &varname, 
		const AosJimoLogicObjNewPtr &logic)
{
	OmnNotImplementedYet;
	return true;
}

bool        
AosJimoProg::setConf(OmnString& confList)
{
	mConfList.push_back(confList);
	if(mConfList.size() > 0)
		return true;
	else
		return false;
}

vector<OmnString>
AosJimoProg::getConfList()
{
	return mConfList;
}
/*
bool        
AosJimoProg::setDataProcName(OmnString& name)
{
	mDataProcNameList.push_back(name);
	if(mDataProcNameList.size() > 0)
		return true;
	else
		return false;
}

vector<OmnString>
AosJimoProg::getDataProcNameList()
{
	return mDataProcNameList;
}

bool        
AosJimoProg::setDataProcSelect(OmnString& name)
{
	mDataProcSelectList.push_back(name);
	if(mDataProcSelectList.size() > 0)
		return true;
	else
		return false;
}

vector<OmnString>
AosJimoProg::getDataProcSelectList()
{
	return mDataProcSelectList;
}

bool        
AosJimoProg::setDataProcIndex(OmnString& name)
{
	mDataProcIndexList.push_back(name);
	if(mDataProcIndexList.size() > 0)
		return true;
	else
		return false;
}

vector<OmnString>
AosJimoProg::getDataProcIndexList()
{
	return mDataProcIndexList;
}

bool        
AosJimoProg::setDataProcIILBatchOpr(OmnString& name)
{
	mDataProcIILBatchOprList.push_back(name);
	if(mDataProcIILBatchOprList.size() > 0)
		return true;
	else
		return false;
}

vector<OmnString>
AosJimoProg::getDataProcIILBatchOprList()
{
	return mDataProcIILBatchOprList;
}

bool        
AosJimoProg::setDataProcDocBatchOpr(OmnString& name)
{
	mDataProcDocBatchOprList.push_back(name);
	if(mDataProcDocBatchOprList.size() > 0)
		return true;
	else
		return false;
}

vector<OmnString>
AosJimoProg::getDataProcDocBatchOprList()
{
	return mDataProcDocBatchOprList;
}

bool        
AosJimoProg::setDataProcIILJoin(OmnString& name)
{
	mDataProcIILJoinList.push_back(name);
	if(mDataProcIILJoinList.size() > 0)
		return true;
	else
		return false;
}

vector<OmnString>
AosJimoProg::getDataProcIILJoinList()
{
	return mDataProcIILJoinList;
}


bool        
AosJimoProg::setDataProcGroupBy(OmnString& name)
{
	mDataProcGroupByList.push_back(name);
	if(mDataProcGroupByList.size() > 0)
		return true;
	else
		return false;
}

vector<OmnString>
AosJimoProg::getDataProcGroupByList()
{
	return mDataProcGroupByList;
}

bool        
AosJimoProg::setDataProcStatJoin(OmnString& name)
{
	mDataProcStatJoinList.push_back(name);
	if(mDataProcStatJoinList.size() > 0)
		return true;
	else
		return false;
}

vector<OmnString>
AosJimoProg::getDataProcStatJoinList()
{
	return mDataProcStatJoinList;
}


bool        
AosJimoProg::setDataProcStatDoc(OmnString& name)
{
	mDataProcStatDocList.push_back(name);
	if(mDataProcStatDocList.size() > 0)
		return true;
	else
		return false;
}

vector<OmnString>
AosJimoProg::getDataProcStatDocList()
{
	return mDataProcStatDocList;
}

bool        
AosJimoProg::setDataProcStatKey(OmnString& name)
{
	mDataProcStatKeyList.push_back(name);
	if(mDataProcStatKeyList.size() > 0)
		return true;
	else
		return false;
}

vector<OmnString>
AosJimoProg::getDataProcStatKeyList()
{
	return mDataProcStatKeyList;
}

bool        
AosJimoProg::setDataProcIILKey(OmnString& name)
{
	mDataProcIILKeyList.push_back(name);
	if(mDataProcIILKeyList.size() > 0)
		return true;
	else
		return false;
}

vector<OmnString>
AosJimoProg::getDataProcIILKeyList()
{
	return mDataProcIILKeyList;
}

bool        
AosJimoProg::setDataset(OmnString& name)
{
	mDatasetList.push_back(name);
	if(mDatasetList.size() > 0)
		return true;
	else
		return false;
}

vector<OmnString>
AosJimoProg::getDatasetList()
{
	return mDatasetList;
}
*/
bool
AosJimoProg::setStat(OmnString& name)
{
	mStatName = name;
	return true;
}

bool
AosJimoProg::getStat(OmnString& name)
{
	name = mStatName;
	return true;
}
/*
void 
AosJimoProg::setUnionIndexName(const OmnString &name)
{
	mUnionIndexName = name;
}

OmnString 
AosJimoProg::getUnionIndexName()
{
	return mUnionIndexName;
}

void 
AosJimoProg::setUnionIILBatchName(const OmnString &name)
{
	mUnionIILBatchName = name;
}

OmnString 
AosJimoProg::getUnionIILBatchName()
{
	return mUnionIILBatchName;
}

void 
AosJimoProg::setUnionSelectName(const OmnString &name)
{
	mUnionSelectNameList.push_back(name);
}

vector<OmnString> 
AosJimoProg::getUnionSelectName()
{
	return mUnionSelectNameList;
}

bool
AosJimoProg::setSelectMap(map<OmnString, vector<OmnString> > &select_map)
{
	vector<OmnString> seList;
	seList.clear();
	map<OmnString, vector<OmnString> >::iterator itr;
	itr = select_map.begin();
	OmnString name = itr->first;
	vector<OmnString> selectList = itr->second;
	for(u32 i=0; i<selectList.size(); i++)
	{
		seList.push_back(selectList[i]);
	}
	mSelectMap[name] = seList;
	return true;
}

map<OmnString, vector<OmnString> >
AosJimoProg::getSelectMap()
{
	return mSelectMap;
}

bool
AosJimoProg::insertToSelectMap(OmnString &name, vector<OmnString> &selectList)
{
	map<OmnString, vector<OmnString> >::iterator itr;
	itr = mSelectMap.find(name);
	vector<OmnString> seList;
	seList.clear();
	if(itr != mSelectMap.end())
	{
		seList = mSelectMap[name];
		for(u32 i=0; i<selectList.size(); i++)
		{
			seList.push_back(selectList[i]);
		}
		mSelectMap[name] = seList;
	}
	else
	{
		for(u32 i=0; i<selectList.size(); i++)
		{
			seList.push_back(selectList[i]);
		}
		mSelectMap[name] = seList;
	}
	return true;
}


bool
AosJimoProg::setUnionMap(map<OmnString, vector<OmnString> > &union_map)
{
	vector<OmnString> uniList;
	uniList.clear();
	map<OmnString, vector<OmnString> >::iterator itr;
	itr = union_map.begin();
	OmnString name = itr->first;
	vector<OmnString> unionList= itr->second;
	for(u32 i=0; i<unionList.size(); i++)
	{
		uniList.push_back(unionList[i]);
	}
	mUnionMap[name] = uniList;
	return true;
}

map<OmnString, vector<OmnString> >
AosJimoProg::getUnionMap()
{
	return mUnionMap;
}
*/
bool
AosJimoProg::setJoinMap(map<OmnString, vector<OmnString> > &join_map)
{
	vector<OmnString> joList;
	joList.clear();
	map<OmnString, vector<OmnString> >::iterator itr;
	itr = join_map.begin();
	OmnString name = itr->first;
	vector<OmnString> joinList = itr->second;
	for(u32 i=0; i<joinList.size(); i++)
	{
		joList.push_back(joinList[i]);
	}
	mJoinMap[name] = joList;
	return true;
}

map<OmnString, vector<OmnString> >
AosJimoProg::getJoinMap()
{
	return mJoinMap;
}

/*
bool
AosJimoProg::setIILJoinMap(map<OmnString, vector<OmnString> > &join_map)
{
	vector<OmnString> joList;
	joList.clear();
	map<OmnString, vector<OmnString> >::iterator itr;
	itr = join_map.begin();
	OmnString name = itr->first;
	vector<OmnString> joinList = itr->second;
	for(u32 i=0; i<joinList.size(); i++)
	{
		joList.push_back(joinList[i]);
	}
	mIILJoinMap[name] = joList;
	return true;
}

map<OmnString, vector<OmnString> >
AosJimoProg::getIILJoinMap()
{
	return mIILJoinMap;
}

bool
AosJimoProg::setStatMap(map<OmnString, vector<OmnString> > &stat_map)
{
	vector<OmnString> statList;
	statList.clear();
	map<OmnString, vector<OmnString> >::iterator itr;
	itr = stat_map.begin();
	OmnString name = itr->first;
	vector<OmnString> stList = itr->second;
	for(u32 i=0; i<stList.size(); i++)
	{
		statList.push_back(stList[i]);
	}
	mStatMap[name] = statList;
	return true;
}

map<OmnString, vector<OmnString> >
AosJimoProg::getStatMap()
{
	return mStatMap;
}

bool
AosJimoProg::insertToStatMap(OmnString &name, vector<OmnString> &statList)
{
	map<OmnString, vector<OmnString> >::iterator itr;
	itr = mStatMap.find(name);
	if(itr != mSelectMap.end())
	{
		vector<OmnString> stList;
		stList = mStatMap[name];
		stList.clear();
		for(u32 i=0; i<statList.size(); i++)
		{
			stList.push_back(statList[i]);
		}
		mStatMap[name] = stList;
	}
	else
	{
		mStatMap[name] = statList;
	}

	return true;
}

bool
AosJimoProg::insertToSelectInputMap(OmnString input_name, OmnString join_name)
{
	map<OmnString, vector<OmnString> >::iterator itr;
	itr = mSelectInputMap.find(input_name);
	vector<OmnString> joinList;
	if(itr != mSelectInputMap.end())
	{
		joinList = mSelectInputMap[input_name];
		joinList.push_back(join_name);
	}
	else
	{
		joinList.push_back(join_name);
		mSelectInputMap[input_name] = joinList;
	}
	return true;
}

map<OmnString, vector<OmnString> >
AosJimoProg::getSelectInputMap()
{
	return mSelectInputMap;
}

bool
AosJimoProg::insertToDataProcMap(OmnString name, vector<OmnString> map_list)
{
	map<OmnString, vector<OmnString> >::iterator itr;
	itr = mDataProcMap.find(name);
	vector<OmnString> mapList;
	if(itr != mDataProcMap.end())
	{
		cout << "should never come here" << endl;
	}
	else
	{
		for(u32 i=0; i<map_list.size(); i++)
		{
			mapList.push_back(map_list[i]);
		}
		mDataProcMap[name] = mapList;

	}
	return true;
}

map<OmnString, vector<OmnString> >
AosJimoProg::getDataProcMap()
{
	return mDataProcMap;
}

bool
AosJimoProg::insertToDocMap(OmnString ds_name, vector<OmnString> doc_list)
{
	map<OmnString, vector<OmnString> >::iterator itr;
	itr = mDocMap.find(ds_name);
	vector<OmnString> docList;
	docList.clear();
	if(itr != mDocMap.end())
	{
		return false;
	}
	else
	{
		for(u32 i=0; i<doc_list.size(); i++)
		{
			docList.push_back(doc_list[i]);
		}
		mDocMap[ds_name] = docList;
	}
	return true;
}

map<OmnString, vector<OmnString> >
AosJimoProg::getDocMap()
{
	return mDocMap;
}

bool
AosJimoProg::insertToIndexMap(OmnString index_name, vector<OmnString> index_list)
{
	map<OmnString, vector<OmnString> >::iterator itr;
	itr = mIndexMap.find(index_name);
	vector<OmnString> indexList;
	indexList.clear();
	if(itr != mIndexMap.end())
	{
		return false;
	}
	else
	{
		for(u32 i=0; i<index_list.size(); i++)
		{
			indexList.push_back(index_list[i]);
		}
		mIndexMap[index_name] = indexList;
	}
	return true;
}

map<OmnString, vector<OmnString> >
AosJimoProg::getIndexMap()
{
	return mIndexMap;
}
*/
bool 
AosJimoProg::setDatasetMap(const OmnString &ds)
{
	mDatasetMap.insert(ds);
	return true;
}
/*
bool
AosJimoProg::insertToSelectConfMap(OmnString sel_name, OmnString sel_conf)
{
	map<OmnString, vector<OmnString> >::iterator itr;
	itr = mSelectConfMap.find(sel_name);
	vector<OmnString> selList;
	selList.clear();
	if(itr != mSelectConfMap.end())
	{
		selList = itr->second;
		selList.push_back(sel_conf);
		mSelectConfMap[sel_name] = selList;
	}
	else
	{
		selList.push_back(sel_conf);
		mSelectConfMap[sel_name] = selList;
	}
	return true;
}


map<OmnString, vector<OmnString> >
AosJimoProg::getSelectConfMap()
{
	return mSelectConfMap;
}
*/
bool 
AosJimoProg::insertToDataFlowMap(
		const OmnString &input_name, 
		const OmnString &output_name)
{
	map<OmnString, vector<OmnString> >::iterator itr = mDataFlowMap.find(input_name);
	if(itr == mDataFlowMap.end())
	{
		vector<OmnString> output_names;
		output_names.push_back(output_name);
		mDataFlowMap.insert(make_pair(input_name, output_names));
		return true;
	}

	(itr->second).push_back(output_name);
	return true;
}

void
AosJimoProg::setSelectMap(const OmnString &selname)
{
	mSelectMap.insert(selname);
}

bool
AosJimoProg::hasSelect(const OmnString &selectname)
{
	set<OmnString>::iterator itr = mSelectMap.find(selectname);
	if (itr != mSelectMap.end()) return true;
	return false;
}

bool 
AosJimoProg::insertToOtherNameMap(
		const OmnString &input_name, 
		const OmnString &output_name)
{
	map<OmnString, vector<OmnString> >::iterator itr = mOtherNameMap.find(input_name);
	if (itr == mOtherNameMap.end())
	{
		vector<OmnString> op_names;
		op_names.push_back(output_name);
		mOtherNameMap.insert(make_pair(input_name, op_names));
		return true;
	}

	(itr->second).push_back(output_name);
	return true;
}
	
bool 
AosJimoProg::insertToNameMap(
		const OmnString &input_name, 
		const OmnString &output_name)
{
	map<OmnString, vector<OmnString> >::iterator itr = mNameMap.find(input_name);
	if (itr == mNameMap.end())
	{
		vector<OmnString> op_names;
		op_names.push_back(output_name);
		mNameMap.insert(make_pair(input_name, op_names));
		return true;
	}

	(itr->second).push_back(output_name);
	return true;
}


