////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KernelConfigLoader.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

/*#include "KernelInterface/KernelConfig.h"

#include "Alarm/Alarm.h"
#include "Config/ConfigReader.h"
#include "Debug/Debug.h"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"

const OmnString sgConfigFileName = "/aos/data/config/CrtAosConfig.txt";



aosKernelConfig::aosKernelConfig()
{
	OmnConfigReader reader(sgConfigFileName);

	mParser = reader.getParser();
	if (!mParser)
	{
		OmnAlarm << "Failed to open the default configuration file: " 
			<< sgConfigFileName << enderr;
		return;
	}
}


bool
OmnKernelConfig::loadConfig()
{
	loadCoreConfig();
	return true
}


bool
aosKernelConfig::saveConfig()
{
	saveCoreConfig();
	return true;
}
*/

/*
bool
aosKernelConfig::loadCoreConfig()
{
	// 
	// 	<CoreConfig>
	//		<BridgeStatus>
	//		<Bridges>
	//			<BridgeDef>
	//				<BridgeId>
	//				<DevicesLeft>
	//					<Device>
	//						<Name>
	//						<Weight>
	//					</Device>
	//					...
	//				</DevicesLeft>
	//				<DevicesRight>
	//					<Device>
	//						<Name>
	//						<Weight>
	//					</Device>
	//					...
	//				</DevicesRight>
	//			</BridgeDef>
	//			...
	//		</Bridges>
	//	
	OmnXmlItemPtr def = mParser->getItem("CoreConfig");
	if (!def)
	{
		OmnAlarm << "Missing Core Config: " << enderr;
		return false;
	}

	aosKernelApi_ConfigCore data;
	data.mOprId = eAosKid_LoadConfig;
	data.mConfigId = eAosLoadConfig_core;
	data.mBridgeStatus = def->getInt("BridgeStatus", 0);
	data.mNumBridges = 0;
	
	int index = 0;
	OmnXmlItemPtr bridges = def->getItem("Bridges");
	if (bridges)
	{
		bridges->reset();
		while (bridges->hasMore() && index < eAosMaxDev)
		{
			OmnXmlItemPtr bdef = bridges->next();
			data.mBridgeDef[index].mBridgeId = bdef->getInt("BridgeId", -1);

			OmnXmlItemPtr devs = bdef->getItem("DevicesLeft");
			devs->reset();
			int numDevs = 0;
			while (devs->hasMore() && numDevs < eAosMaxDev)
			{
				OmnXmlItemPtr devdef = devs->next();
				OmnString name = devdef->getStr("Name");
				int weight = devdef->getInt("Weight", -1);
				if (weight < 0)
				{
					OmnAlarm << "Device weight negative: " 
						<< devdef->toString() << enderr;
					continue;
				}

				if (name.length() >= eAosMaxDevNameLen)
				{
					OmnAlarm << "Device name too long: " 
						<< devdef->toString() << enderr;
					continue;
				}

				strcpy(data.mBridgeDef[index].mNames1[numDevs], name.toData());
				data.mBridgeDef[index].mWeight1[numDevs] = weight;
				numDevs++;
			}

			if (numDevs >= eAosMaxDev)
			{
				OmnAlarm << "Too many devices: " 
					<< devs->toString() << enderr;
			}

			data.mBridgeDef[index].mNumDev1 = numDevs;

			// 
			// Load the right side devices
			//
			devs = bdef->getItem("DevicesRight");
			devs->reset();
			numDevs = 0;
			while (devs->hasMore() && numDevs < eAosMaxDev)
			{
				OmnXmlItemPtr devdef = devs->next();
				OmnString name = devdef->getStr("Name");
				int weight = devdef->getInt("Weight", -1);
				if (weight < 0)
				{
					OmnAlarm << "Device weight negative: " 
						<< devdef->toString() << enderr;
					continue;
				}

				if (name.length() >= eAosMaxDevNameLen)
				{
					OmnAlarm << "Device name too long: " 
						<< devdef->toString() << enderr;
					continue;
				}

				strcpy(data.mBridgeDef[index].mNames2[numDevs], name.toData());
				data.mBridgeDef[index].mWeight2[numDevs] = weight;
				numDevs++;
			}

			if (numDevs >= eAosMaxDev)
			{
				OmnAlarm << "Too many devices: " 
					<< devs->toString() << enderr;
			}

			data.mBridgeDef[index].mNumDev2 = numDevs;

			index++;
		}

		if (index >= eAosMaxDev)
		{
			OmnAlarm << "Too many bridges: " << eAosMaxDev << enderr;
		}
		data.mNumBridges = index;
	}

	if (OmnKernelAPI::sendToKernel(&data, sizeof(data))
	{
		OmnAlarm << "Failed to configure core" << enderr;
		return false;
	}

	return true;
}


bool
aosKernelConfig::saveCoreConfig()
{
	// 
	// 	<CoreConfig>
	//		<BridgeStatus>
	//		<Bridges>
	//			<BridgeDef>
	//				<BridgeId>
	//				<DevicesLeft>
	//					<Device>
	//						<Name>
	//						<Weight>
	//					</Device>
	//					...
	//				</DevicesLeft>
	//				<DevicesRight>
	//					<Device>
	//						<Name>
	//						<Weight>
	//					</Device>
	//					...
	//				</DevicesRight>
	//			</BridgeDef>
	//			...
	//		</Bridges>
	//	</CoreConfig>
	//	
	aosKernelApi_ConfigCore data;
	data.mOprId = eAosKid_SaveConfig;
	data.mConfigId = eAosSaveConfig_core;
	
	if (OmnKernelAPI::sendToKernel(&data, sizeof(data))
	{
		OmnAlarm << "Failed to retrieve the data from kernel" << enderr;
		return false;
	}


	try
	{
		OmnXmlItemPtr def = mParser->addItem("CoreConfig");

		def->set("BridgeStatus", data.mBridgeStatus, true);
		OmnXmlItemPtr bridges = def->addItem("Bridges");
		bridges->clear();
		
		for (int i=0; i<data.mNumBridges; i++)
		{
			OmnXmlItemPtr bdef = bridges->append("BridgeDef");
			bdef->set("BridgeId", data.mBridgeDef[i].mBridgeId;

			//
			// The left devices
			//
			OmnXmlItemPtr left = bdef->add("DevicesLeft");
			for (int j=0; j<data.mBridgeDef[i].mNumDev1; j++)
			{
				OmnXmlItemPtr devdef = left->append("Device");
				devdef->set("Name", data.mBridgeDef[i].mNames1[j]);
				devdef->set("Weight", data.mBridgeDef[i].mWeight1[j]);
			}

			//
			// The right devices
			//
			OmnXmlItemPtr right = bdef->add("DevicesRight");
			for (int j=0; j<data.mBridgeDef[i].mNumDev2; j++)
			{
				OmnXmlItemPtr devdef = left->append("Device");
				devdef->set("Name", data.mBridgeDef[i].mNames1[j]);
				devdef->set("Weight", data.mBridgeDef[i].mWeight1[j]);
			}
		}
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Memory error: " << e.toString() << enderr;
		return false;
	}

	return true;
}
*/


