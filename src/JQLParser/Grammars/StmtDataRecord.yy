stmt_create_data_record:
	CREATE DATA RECORD W_WORD '(' stmt_expr_name_values ')' ';'
	{
		AosJqlStmtDataRecord *stmt = new AosJqlStmtDataRecord;
		stmt->setName($4);
		stmt->setConfParms($6);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE data_record_type_opts DATA RECORD W_WORD
	record_length
	FIELDS '(' data_field_list ')'
	record_delimiter
	field_delimiter
	text_qualidier ';'
	{
		AosJqlStmtDataRecord *data_record = new AosJqlStmtDataRecord;
		OmnString name = $5;
		data_record->setName(name);
		data_record->setType($2);
		data_record->setRecordLength($6);
		data_record->setFieldNames($9);
		data_record->setRecordDelimiter($11);
		data_record->setFieldDelimiter($12);
		data_record->setTextQualidier($13);
		gAosJQLParser.appendStatement(data_record);
		$$ = data_record;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE SCHEMA W_WORD 
	SOURCE TYPE data_record_type_opts 
	record_length
	trim_condition
	schema_picker
	FIELDS '(' data_field_list ')'
	record_delimiter
	field_delimiter
	text_qualidier ';'
	{
		AosJqlStmtDataRecord *data_record = new AosJqlStmtDataRecord;
		OmnString type = "schema";
		OmnString name = $3;
		data_record->setName(name);
		data_record->setRecordType(type);
		data_record->setType($6);
		data_record->setRecordLength($7);
		data_record->setTrimCondition($8);
		data_record->setScheamPicker($9);
		data_record->setFieldNames($12);
		data_record->setRecordDelimiter($14);
		data_record->setFieldDelimiter($15);
		data_record->setTextQualidier($16);
		gAosJQLParser.appendStatement(data_record);
		$$ = data_record;
		$$->setOp(JQLTypes::eCreate);
	};

data_record_type_opts:
	CSV
	{
		$$ = OmnString("csv").getBuffer();
	}
	|
	FIXED LENGTH
	{
		$$ = OmnString("fixbin").getBuffer();
	}
	|
	MULTIPLE RECORD
	{
		$$ = OmnString("multi").getBuffer();
	};

trim_condition:
	{
		$$ = NULL;
	}
	|
	TRIM STRING
	{
		$$ = $2;	
	}

data_field_list:
	data_field
	{
		$$ = new AosExprList; 
		$$->push_back($1);
	}
	|
	data_field_list ',' data_field
	{
		 $$->push_back($3);
	};

data_field:
	expr 
	{
		$$ = $1;
	}
	|
	expr stmt_infofield TYPE data_field_data_type
	field_formater
	data_idfamily
	data_field_max_len
	data_field_offset
	data_field_default_value
	field_mapping
	{
		AosJqlStmtDataFieldPtr data_field = OmnNew AosJqlStmtDataField; 
		//data_field->setName($1->getValue(0).toLower());
		data_field->setName($1->getValue(0));
		data_field->setIsInfoField($2);
		data_field->setType($4);
		data_field->setFormater($5);
		data_field->setIdfamily($6);
		data_field->setMaxLen($7);
		data_field->setOffset($8); 
		data_field->setDefValue($9);
		data_field->setMapping($10);
		gAosJQLParser.appendStatement(data_field); 
		data_field->setOp(JQLTypes::eCreate); 
		//$$ = new AosExprString(($1->getValue(0).toLower()).data());
		$$ = new AosExprString(($1->getValue(0)).data());
	}
	|
	expr stmt_infofield TYPE data_field_data_type
	stmt_data_type
	field_formater
	VALUE expr 
	data_idfamily
	data_field_max_len
	data_field_offset
	data_field_default_value
	{
		AosJqlStmtDataField *data_field = new AosJqlStmtDataField; 
		//data_field->setName($1->getValue(0).toLower());
		data_field->setName($1->getValue(0));
		data_field->setIsInfoField($2);
		data_field->setType($4);                                      
        data_field->setDataType($5);
		data_field->setFormater($6);
		if ($8)
		{
			data_field->setValue($8->dump());
		}
		data_field->setIdfamily($9);
		data_field->setMaxLen($10);
		data_field->setOffset($11); 
		data_field->setDefValue($12);
		gAosJQLParser.appendStatement(data_field); 
		data_field->setOp(JQLTypes::eCreate); 
		//$$ = new AosExprString(($1->getValue(0).toLower()).data());
		$$ = new AosExprString(($1->getValue(0)).data());
	}
	|
	expr stmt_infofield TYPE data_field_data_type
	field_formater
	data_idfamily
	data_field_max_len
	data_field_offset
	data_field_default_value
	NOT NULLX
	{
		AosJqlStmtDataField *data_field = new AosJqlStmtDataField; 
		//data_field->setName($1->getValue(0).toLower());
		data_field->setName($1->getValue(0));
		data_field->setIsInfoField($2);
		data_field->setType($4); 
		data_field->setFormater($5);
		data_field->setIdfamily($6);
		data_field->setMaxLen($7);
		data_field->setOffset($8); 
		data_field->setDefValue($9);
		data_field->setNOTNULL("true");
		gAosJQLParser.appendStatement(data_field); 
		data_field->setOp(JQLTypes::eCreate); 
		//$$ = new AosExprString(($1->getValue(0).toLower()).data());
		$$ = new AosExprString(($1->getValue(0)).data());
	};

field_formater:
	{
		$$ = "";
	}
	|
	FORMAT STRING
	{
		$$ = $2;
	};

data_field_data_type:
	W_WORD
	{
		$$ = new AosJQLDataFieldTypeInfo();
		$$->setType($1);
	}
	|
	DOUBLE
	{
		$$ = new AosJQLDataFieldTypeInfo();
		$$->setType("double");
	}
	|
	DATETIME	
	{
		$$ = new AosJQLDataFieldTypeInfo();
		$$->setType("datetime");
	}
	|
	VARCHAR '(' INTNUM ')'
	{
		$$ = new AosJQLDataFieldTypeInfo();
		$$->setType("varchar");
		$$->setValue($3, 0);
	}
	|
	VARCHAR2 '(' INTNUM ')'
	{
		$$ = new AosJQLDataFieldTypeInfo();
		$$->setType("varchar");
		$$->setValue($3, 0);
	}
	|
	CHAR '(' INTNUM ')'
	{
		$$ = new AosJQLDataFieldTypeInfo();
		$$->setType("varchar");
		$$->setValue($3, 0);
	}
	|
	TEXT
	{
		$$ = new AosJQLDataFieldTypeInfo();
		$$->setType("text");
	}
	|
	NUMBER '(' INTNUM ',' INTNUM ')'
	{
		$$ = new AosJQLDataFieldTypeInfo();
		$$->setType("number");
		$$->setValue($3, $5);
	}
	|
	NUMBER '(' INTNUM ')'
	{
		$$ = new AosJQLDataFieldTypeInfo();
		$$->setType("number");
		$$->setValue($3, 0);
	}
	|
	DECIMAL '(' INTNUM ',' INTNUM ')'
	{
		$$ = new AosJQLDataFieldTypeInfo();
		$$->setType("decimal");
		$$->setValue($3, $5);
	}
	|
	BIGINT '(' INTNUM ')'
	{
		$$ = new AosJQLDataFieldTypeInfo();
		$$->setType("bigint");
		$$->setValue($3, 0);
	} ;

record_length:
	{
		$$ = 0;
	}
	|
	RECORD LENGTH INTNUM
	{
		$$ = $3;
	}
	|
	SCHEMA LENGTH INTNUM
	{
		$$ = $3;
	};

schema_picker:
	{
		$$ = 0;
	}
	|
	PICKER expr
	{
		$$ = $2;
	};

record_delimiter:
	{
		$$ = NULL;
	}
	|
	RECORD DELIMITER STRING 
	{
		$$ = $3;
	};
	|
	SCHEMA DELIMITER STRING 
	{
		$$ = $3;
	};

field_mapping:
	{
		$$ = NULL;
	}
	|
	MAPPING '(' expr_list ')'
	{
		$$ = $3;	
	};

field_delimiter:
	{
		$$ = NULL;
	}
	|
	FIELD DELIMITER STRING 
	{
		/*$$ = $3->getValue(0).getBuffer();*/
		$$ = $3;
	};

text_qualidier:
	{
		$$ = NULL;
	}
	|
/*	TEXT QUALIFIER expr 
	{
		$$ = $3->getValue(0).getBuffer();
	};*/
	TEXT QUALIFIER STRING 
	{
		/*$$ = $3->getValue(0).getBuffer();*/
		$$ = $3;
	};

stmt_drop_data_record:
	DROP DATA RECORD W_WORD ';'
	{
		AosJqlStmtDataRecord *data_record = new AosJqlStmtDataRecord;
		data_record->setName($4);
		gAosJQLParser.appendStatement(data_record);
		$$ = data_record;
		$$->setOp(JQLTypes::eDrop);
	}
	|
	DROP SCHEMA W_WORD ';'
	{
		AosJqlStmtDataRecord *data_record = new AosJqlStmtDataRecord;
		gAosJQLParser.appendStatement(data_record);
		OmnString name = $3;
		data_record->setName(name);
		$$ = data_record;
		$$->setOp(JQLTypes::eDrop);
	};

stmt_show_data_record:
	SHOW DATA RECORDS ';'
	{
		AosJqlStmtDataRecord *data_record = new AosJqlStmtDataRecord;
		gAosJQLParser.appendStatement(data_record);
		$$ = data_record;
		$$->setOp(JQLTypes::eShow);
	}
	|
	SHOW SCHEMAS ';'
	{
		AosJqlStmtDataRecord *stmt = new AosJqlStmtDataRecord;
	 	gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eShow);
	};

stmt_describe_data_record:
	DESCRIBE DATA RECORD W_WORD ';'
	{
		AosJqlStmtDataRecord *data_record = new AosJqlStmtDataRecord;
		data_record->setName($4);
		gAosJQLParser.appendStatement(data_record);
		$$ = data_record;
		$$->setOp(JQLTypes::eDescribe);
	};
