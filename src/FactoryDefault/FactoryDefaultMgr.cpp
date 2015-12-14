////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FactoryDefaultMgr.cpp
// Description:
//   
//
// Modification History:
// 12/07/2006: Created by jzz
// 
////////////////////////////////////////////////////////////////////////////


#include "FactoryDefault/FactoryDefaultMgr.h"
#include "FactoryDefault/FactoryDefault.h"

AosFactoryDefaultMgr::AosFactoryDefaultMgr()
{
}


AosFactoryDefaultMgr::~AosFactoryDefaultMgr()
{
}

bool
AosFactoryDefaultMgr::setToDefault(OmnString &rslt)
{    
	mModules.reset();
	for (int i=0; i<mModules.entries(); i++)
	{
		AosFactoryDefaultPtr c = mModules.crtValue();
		c->setToDefault(rslt);
		mModules.next();
	}
	return true;
}


bool
AosFactoryDefaultMgr::showDefault(OmnString &rslt)
{
}


bool
AosFactoryDefaultMgr::moduleRegister(const OmnString &name, 
									const OmnString &port, 
												OmnString &rslt)
{
	mModules.reset();
	for (int i=0; i<mModules.entries(); i++)
	{
		AosFactoryDefaultPtr c = mModules.crtValue();
		if ((c->name() == name))
		{
			//
			//to be decide if a module has registered a port,whether we will 
			//give it a new port and return true or denied the request and 
			//return false
			//
			c->set(name,port);
			rslt << name << "'s port has been modified.";
			return true;
		}
		mModules.next();
	}
	AosFactoryDefaultPtr module ;
	module->set(name,port);
	rslt << name << " has been registered." ; 
	return true;
}

bool
AosFactoryDefaultMgr::moduleUnregister(const OmnString &name, 
												OmnString &rslt)
{
	mModules.reset();
	for (int i=0; i<mModules.entries(); i++)
	{
		AosFactoryDefaultPtr c = mModules.crtValue();
		if ((c->name() == name))
		{
			mModules.eraseCrt();
			rslt << name << "has been unregistered.";
			return true;
		}
		mModules.next();
	}
	return false;
}

bool
AosFactoryDefaultMgr::showRegistration(OmnString &rslt)
{
	mModules.reset();
	for (int i=0; i<mModules.entries(); i++)
	{
		AosFactoryDefaultPtr c = mModules.crtValue();
		rslt << "Module Name: " << c->name() 
				<< "Module Port" << c->port() 
				<< "\n";
		mModules.next();
	}
	return true;
}

bool
AosFactoryDefaultMgr::resetRegistration(OmnString &rslt)
{
	mModules.clear();
	rslt << "Registration has been reset.";
	return true;
}

AosFactoryDefaultPtr
AosFactoryDefaultMgr::getModulePtr(const OmnString &name)
{
	mModules.reset();
	for (int i=0; i<mModules.entries(); i++)
	{
		AosFactoryDefaultPtr c = mModules.crtValue();
		if ((c->name() == name))
			return c;
		mModules.next();
	}
	return 0;
}
