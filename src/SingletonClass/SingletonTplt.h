////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SingletonTplt.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_SingletonClass_SingletonTplt_h
#define Omn_SingletonClass_SingletonTplt_h

#include "SingletonClass/SingletonObj.h"
#include "SingletonClass/SingletonObjId.h"
#include "XmlParser/Ptrs.h"


#define OmnDefineSingletonClass(classname, rname, rself, enumName, strName) \
class rname;																\
extern rname * rself;														\
class classname : public OmnSingletonObj									\
{																			\
public:																		\
	classname() : OmnSingletonObj(enumName) {}								\
	~classname() {}															\
																			\
	virtual bool		createSingleton();									\
	static	bool		startSingleton1(const bool needToLock);				\
	virtual bool        stopSingleton();									\
	virtual OmnString   getSysObjName() const {return strName;}				\
	virtual bool		config(const AosXmlTagPtr &def);					\
	virtual OmnSingletonObjId::E	getSysObjId() const {return enumName;}	\
};																			\

#endif
