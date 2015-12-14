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
// 09/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoJob_JimoJob_h
#define Aos_JimoJob_JimoJob_h

#include "API/AosApiG.h"
#include "Jimo/Jimo.h"
#include "JimoJob/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/GenericObj.h"
#include "XmlUtil/XmlTag.h"
#include "JSON/JSON.h"

#include <vector>
using namespace std;

class AosJimoJob : public AosGenericObj
{
	OmnDefineRCObject

	map<OmnString, OmnString>  	mDatasets;

public:
	class AosJimoScheduler
	{
	private:
		OmnString		mId;
		int				mNumSlots;

	public:
		AosJimoScheduler()
		:
		mId("norm"),
		mNumSlots(1)
		{
		}

		OmnString getConfig()
		{
			OmnString conf;
			conf << "<" << AOSTAG_SCHEDULER << " "
				 	<< AOSTAG_JOBSCHEDULER_ID << "=\"" << mId << "\" "
					<< AOSTAG_NUM_SLOTS << "=\"" << mNumSlots << "\" "
				 << "/>";
			return conf;
		}

		OmnString getId() {return mId;}
		int getNumSlots() {return mNumSlots;}

		void setId(const OmnString &id) {mId = id;}
		void setNumSlots(const int &num_slots) {mNumSlots = num_slots;}
	};

public:
	AosJimoJob(const int ver);
	AosJimoJob(const bool regflag);

	// Chen Ding, 2015/05/26
	// AosJimoJob(const AosJimoJob &job);
	AosJimoJob(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosJimoJob();

	bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);

	virtual bool serializeTo(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual AosJimoJobPtr clone();
	virtual AosJimoJobPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &);

	// AosGenericObj interfaces
	virtual bool createByJql(
			AosRundata *rdata,
			const OmnString &obj_name, 
			const OmnString &jsonstr,
			const AosJimoProgObjPtr &prog);

	virtual bool runByJql(
			AosRundata *rdata,
			const OmnString &obj_name, 
			const OmnString &jsonstr);

	virtual bool showByJql(
			AosRundata *rdata,
			const OmnString &obj_name, 
			const OmnString &jsonstr);

	virtual bool dropByJql(
			AosRundata *rdata,
			const OmnString &obj_name, 
			const OmnString &jsonstr);

	AosJimoPtr cloneJimo() const;
	
	//arvin 2015/4/20
		virtual bool parseJQL(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser, 
					AosJimoProgObj *prog,
					bool &parsed, 
					bool dft = false)
	{
		OmnShouldNeverComeHere;  
		return false;
	}
	
	virtual bool setJimoName(const OmnString &name)
	{
		OmnShouldNeverComeHere;  
		return false;	
	}

	virtual OmnString getJimoName() const
	{
		OmnShouldNeverComeHere;  
		return "";
	}

	virtual OmnString getObjType(AosRundata *rdata)
	{
		OmnShouldNeverComeHere;  
		return "";
	}



};

#endif
