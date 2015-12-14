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
// 04/25/2014 Created by Young
////////////////////////////////////////////////////////////////////////////
#include "AosConf/MapTask.h"
#include <string>
using AosConf::MapTask;


MapTask::MapTask()
{
	init();
}

MapTask::~MapTask()
{
}


void
MapTask::init()
{
}


string	
MapTask::getConfig()
{
	// 	This is "MapTask" format
	//	<map_task>
	//		<actions>
	//			<action>... <action>
	//		</actions>
	//	</map_task>
	
	// 1. MapTask node 
	string config = "<map_task";
	for (map<string, string>::iterator itr=mAttrs.begin(); 
			itr!=mAttrs.end(); itr++)
	{
		config += " " + itr->first + "=\"" + itr->second +  "\"";
	}
	config += ">";

	// 2. actions node
	config += "<actions>";
	if (mDataEngine)
	{
		config += mDataEngine->getConfig();
	}
	config += "</actions>";

	config += "</map_task>";
	return config;
}

void	
MapTask::setDataEngine(const boost::shared_ptr<DataEngine> &dataengine)
{
	mDataEngine = dataengine;
}

