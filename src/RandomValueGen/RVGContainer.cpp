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
// A ParmContainer contains one or more Parms. These parms can be 
// defined through an XML document. Parms are identified by indices.
// It provides the member functions to retrieve the next value of 
// any parms in the container. 
//
// Examples:
// In this example, we want to construct a statement to add a user:
//     bool addUser(const u32 userId, 
//     				const OmnString &username,
//     				const OmnString &addr, 
//     				const OmnString &email,
//     				const u16 year,
//     				const u16 month,
//     				const u16 day);
// We need to create a ParmContainer with seven parms in it, each for
// a parm for the above function. 
//
// int main()
// {
// 		OmnString filename = get the configure file name;
//     	AosRvgContainer pc(filename);
//     	AosGenericTable table;
//
//     	for (int i=0; i<10000; i++)
//     	{
//     		bool ret1 = pc.nextValue(table);
//     		bool ret2 = addUser(pc.getU32(0), 
//     					 		pc.getStr(1),
//     					 		pc.getStr(2), 
//     					 		pc.getStr(3),
//     					 		pc.getU16(4), 
//     					 		pc.getU16(5),
//     					 		pc.getU16(6));
//     		OmnTC(OmnExpected<bool>(ret1), OmnActual<bool>(ret2)) << endtc;
//     	}
//
//     	return 0;
//  }
//
// Modification History:
// 4/8/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#include "Parms/RVGContainer.h"

#include "Debug/Except.h"
#include "Parms/Ptrs.h"
#include "Parms/RVG.h"
#include "Util/GenTable.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlParser.h"



AosRvgContainer::AosRvgContainer()
{
}


AosRvgContainer::~AosRvgContainer()
{
}


bool
AosRvgContainer::parse(const OmnString &filename)
{
	// 
	// Open the file, get the item, and then call 
	// parse(xml);
	//
	return false;
}


bool
AosRvgContainer::parse(const OmnXmlItemPtr &def)
{
	// 
	// This function parses the configure file "filename"
	// and creates this object. The configure file should
	// be in the format:
	//     <Parms>
	//     		<Parm>
	//     		...
	//     		</Parm>
	//     		<Parm>
	//     		...
	//     		</Parm>
	//     		...
	//     </Parms>
	//
	return false;
}


bool
AosRvgContainer::nextValue(const AosGenTablePtr &table)
{
	// 
	// This function calls every member parm's 'nextValue(...)'
	// member function to generate the next value. If needed, 
	// it will update the data 'table'. If all the parms
	// generate correct values, it returns true. Otherwise, 
	// it returns false. 
	//
	return false;
}


bool
AosRvgContainer::getU16(const u16 index, u16 &value)
{
	// 
	// The function retrieves the 'index'th parm current
	// value as a u16 and returns it. If the index goes
	// out of bound, it throws an exception. 
	//
	if (index >= mParms.entries())
	{
		OmnThrowException("Index out of bound");
	}

	return mParms[index]->getCrtValue(value);
}


bool
AosRvgContainer::getU32(const u16 index, u32 &value)
{
	// 
	// The function retrieves the 'index'th parm current
	// value as a u32 and returns it. If the index goes
	// out of bound, it throws an exception. 
	//
	if (index >= mParms.entries())
	{
		OmnThrowException("Index out of bound");
	}

	return mParms[index]->getCrtValue(value);
}


bool
AosRvgContainer::getStr(const u16 index, OmnString &value)
{
	// 
	// The function retrieves the 'index'th parm current
	// value as an OmnString and returns it. If the index goes
	// out of bound, it throws an exception. 
	//
	if (index >= mParms.entries())
	{
		OmnThrowException("Index out of bound");
	}

	return mParms[index]->getCrtValue(value);
}


