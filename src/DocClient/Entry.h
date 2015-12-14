////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 04/15/2013 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocClient_Entry_h
#define AOS_DocClient_Entry_h

#include "Rundata/Rundata.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/XmlDoc.h"


struct AosEntry : public OmnRCObject  
{                                     
private:                              
	OmnDefineRCObject;                

	struct Msg
	{
		bool		send_data;
		bool 		finished;
		int			num_data_msgs;
		int			total_num_data_msgs;
		
		Msg()
		:
		send_data(false),
		finished(false),
		num_data_msgs(0),
		total_num_data_msgs(0)
		{
		}
	};


	u64 					timestamp;
	AosDocClientCallerPtr 	caller;
	vector<Msg>		 		msg;

public:                               
	AosEntry()
	:
	timestamp(0)
	{
	}

	AosEntry(const AosDocClientCallerPtr &caller, const int numPhysicals)
	:
	timestamp(OmnGetTimestamp()),
	caller(caller)
	{
		for (int serverid = 0; serverid < numPhysicals; serverid++)
		{
			Msg m;
			msg.push_back(m);
		}
	}

	void setSendDataFlag(const int serverid)
	{
		aos_assert(serverid < (int)msg.size());
		msg[serverid].send_data = true;
	}

	void updateFinished(const int serverid, const int total_num_data_msgs)
	{
		aos_assert(serverid < (int)msg.size());
		msg[serverid].finished = true;
		msg[serverid].total_num_data_msgs = total_num_data_msgs;
	}

	void updateNumDataMsgs(const int serverid)
	{
		aos_assert(serverid < (int)msg.size());
		msg[serverid].num_data_msgs ++;
	}

	bool checkFinished()
	{
		for(int serverid = 0; serverid < (int)msg.size(); serverid++)
		{
			if (!msg[serverid].send_data) continue;	
			if (!msg[serverid].finished) return false;
			if (msg[serverid].total_num_data_msgs != msg[serverid].num_data_msgs)
			{
OmnScreen << "====================== total: " << msg[serverid].total_num_data_msgs << " , num_data_msgs: " << msg[serverid].num_data_msgs << endl;
				if (msg[serverid].total_num_data_msgs < msg[serverid].num_data_msgs)
				{
					OmnAlarm << "total_num_data_msgs(" << msg[serverid].total_num_data_msgs 
						<< ") <" << "num_data_msgs(" << msg[serverid].num_data_msgs <<")" << enderr; 
				}
				return false;
			}
		}
		return true;
	}

	~AosEntry(){}
};                                    


#endif

