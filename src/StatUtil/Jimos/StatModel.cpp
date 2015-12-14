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
// 2015/02/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "StatUtil/Jimos/StatModel.h"
#include "JQLStatement/JqlStatement.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Rundata/Rundata.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosStatModel_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosStatModel(version);
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

AosJimoPtr 
AosStatModel::cloneJimo() const
{
	return OmnNew AosStatModel(*this);
}



AosStatModel::AosStatModel(const int version)
:
AosStatModelObj(version)
{
}

AosStatModel::AosStatModel(const OmnString &statModelName)
:
AosStatModelObj(1),
mName(statModelName),
mCondStr(""),
mStatTime(NULL)
{
	AosRundataPtr rdata = OmnApp::getRundata();
	OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eStatisticsDoc, mName);
	AosXmlTagPtr doc = AosJqlStatement::getDocByObjid(rdata.getPtr(), objid);
	if(!doc)
	{
		OmnAlarm << "stat model " << mName << "doesn't exist!"<< enderr;  
	}
	config(doc);
}

AosStatModel::~AosStatModel()
{
}

bool 
AosStatModel::config(const AosXmlTagPtr &conf)
{
	AosXmlTagPtr keyfields_doc = conf->getFirstChild("key_fields");
	aos_assert_r(keyfields_doc,false);
	AosXmlTagPtr field_doc = keyfields_doc->getFirstChild();
	//arvin 2015-10-23
	//JIMODB-1014,if statmodel have no groupbykey,we wiil give a virtual field "const_statkey",
	//we will check the virtual field in JimoLogicStat.cpp,DataProcGroupBy.cpp and DataProcStatJoin.cpp
	if(!field_doc)
		mKeyFields.push_back(AOS_EMPTY_KEY);
	while(field_doc)
	{
		OmnString field_name = field_doc->getAttrStr("field_name","");
		if( field_name =="")
		{
			OmnAlarm << "miss_keyfield_name" << enderr;
			return false;
		}	
		mKeyFields.push_back(field_name);
		field_doc = keyfields_doc->getNextChild();
	}

	//parse measure
	AosXmlTagPtr meas_doc = conf->getFirstChild("measures");
	aos_assert_r(meas_doc,false);
	AosXmlTagPtr mea_doc = meas_doc->getFirstChild();
	while(mea_doc)
	{
		OmnString agr_type = mea_doc->getAttrStr("agr_type","");
		if( agr_type == "")
		{
			OmnAlarm << "miss_agr_type" << enderr;
			return false;
		}
		
		OmnString parm = mea_doc->getAttrStr("field_name","");
		
		if( parm == "")
		{
			OmnAlarm << "miss_agr_type" << enderr;
			return false;
		}	
		OmnString measure = agr_type;
		if(parm.toInt() == 42)
		{
			char p = '*';
			measure << "(" <<p <<")";
		}
		else
		{
			measure << "("<< parm << ")";
		}
		mMeasureFields.push_back(measure);
		mea_doc = meas_doc->getNextChild();
	}

	//parse timeField
	AosXmlTagPtr timeField_doc = conf->getFirstChild("time_field");
	if (timeField_doc)
	{	
		mStatTime = new AosStatTime();
		bool rslt = mStatTime->config(timeField_doc);
		aos_assert_r(rslt,false);
	}
	//parse cond
	AosXmlTagPtr cond_conf = conf->getFirstChild("cond");
	if (cond_conf)
	{
		mCondStr = cond_conf->getNodeText();
	}

	AosXmlTagPtr shufflField_doc = conf->getFirstChild("shuffle_fields");
	if(shufflField_doc)
	{
		AosXmlTagPtr field_doc = shufflField_doc->getFirstChild("field");
		while(field_doc)
		{
			OmnString fieldName = field_doc->getAttrStr("field_name","");
			mShuffleFields.push_back(fieldName);
			field_doc = shufflField_doc->getNextChild();
		}
	}
	return true;
}

AosStatModelObjPtr 
AosStatModel::createStatModel(AosRundata *rdata, 
		const AosXmlTagPtr &def)
{
	return 0;
}

AosDataFieldObjPtr
AosStatModel::getKey(const OmnString &name)
{
	return 0;
}

AosStatMeasureObjPtr
AosStatModel::getMeasure(const OmnString &name) const
{
	return 0;
}

vector<OmnString>
AosStatModel::getKeyFields()
{
	return mKeyFields;
}


vector<OmnString>
AosStatModel::getMeasures()
{
	return mMeasureFields;
}

	
vector<OmnString> 
AosStatModel::getShuffleFields()
{
	return mShuffleFields;
}

AosStatTimePtr 
AosStatModel::getTime()	
{
	return mStatTime;
}


OmnString 
AosStatModel::getCond()	
{
	return mCondStr;
}

OmnString 
AosStatModel::getDbTableName(
				AosRundata *rdata,
				const OmnString &statModelName)
{
	bool reverse = false;
	OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eStatisticsDoc, statModelName);
	AosXmlTagPtr doc = AosJqlStatement::getDocByObjid(rdata, objid);
	if(!doc)
	{
		OmnString errmsg = "[ERR] :";
		errmsg << " no such statistic model " << statModelName;
		rdata->setJqlMsg(errmsg);
		aos_assert_r(doc, NULL);
		return "";
	}

	OmnString zky_stat_name = doc->getAttrStr("zky_stat_name","");
	OmnString zky_table_name = doc->getAttrStr("zky_table_name","");
	int start_pos = zky_stat_name.findSubString(zky_table_name,0,reverse);
	return OmnString(zky_stat_name.data(),start_pos-1);
}


