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
//
// Modification History:
// 12/22/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataStore_GenericDbObj_h
#define AOS_DataStore_GenericDbObj_h

#include "DataStore/GenericField.h"
#include "Obj/ObjDb.h"
#include "Util/String.h"
#include "XmlInterface/XmlRc.h"

class TiXmlElement;

class AosGenericDbObj : virtual public OmnDbObj
{
	OmnDefineRCObject;

	enum
	{
		eMaxNumFields = 256
	};

private:
	int					mNumFields;
	OmnString			mTablename;
	AosGenericFieldPtr	mFields[eMaxNumFields];

public:
	AosGenericDbObj();
	AosGenericDbObj(const OmnString &tablename);
	virtual ~AosGenericDbObj();

	//
	// OmnDbObj interface
	//
	virtual OmnRslt     serializeFromRecord(const OmnDbRecordPtr &record);
	virtual OmnString   insertStmt() const;
	virtual OmnString   updateStmt() const;
	virtual OmnString   removeStmt() const;
	virtual OmnString   retrieveStmt() const;
	virtual OmnString   removeAllStmt() const;
	virtual OmnString   existStmt() const;
	virtual int			objSize() const {return sizeof(*this);}

	bool	parseDef(const OmnString &tabledef, 
				AosXmlRc &errcode, 
				OmnString &errmsg);
	bool	setObjAttrs(TiXmlElement *def, 
				AosXmlRc &errcode, 
				OmnString &errmsg);
	bool	addRecordToDb(TiXmlElement *def, 
				AosXmlRc &errcode, 
				OmnString &errmsg);
	AosGenericFieldPtr	getField(const OmnString &field);
	bool	setFieldValue(const OmnString &name, const OmnString &value);

	void	resetValues();
	void	setTablename(const OmnString &tname) {mTablename = tname;}
	bool	retrieveFromDb(const TiXmlElement *query, 
				bool &retrieved, 
				int &num_matched,
				AosXmlRc &errcode, 
				OmnString &errmsg);

	bool	addField(const OmnString &name, 
				const AosGenericField::DataType type,
				const OmnString &value, 
				const OmnString &dft_value);
};
#endif
