stmt_create_data_field:
	CREATE DATA FIELD W_WORD TYPE STRING
	data_idfamily
	data_field_max_len 
	data_field_offset
	data_field_default_value ';'
	{
		AosJqlStmtDataField *data_field = new AosJqlStmtDataField;
		data_field->setName($4);
		data_field->setType($6);
		data_field->setIdfamily($7);
		data_field->setMaxLen($8);
		data_field->setOffset($9);
		data_field->setDefValue($10);
		gAosJQLParser.appendStatement(data_field);
		$$ = data_field;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE DATA FIELD W_WORD TYPE STRING
	VALUE FROM STRING
	INDEX W_WORD STRING
	iilmap_mode ASC 
	data_field_max_len ';'
	{
		AosJqlStmtDataField *data_field = new AosJqlStmtDataField;
		OmnString mode = $13;

		data_field->setName($4);
		data_field->setType($6);
		data_field->setFromFieldName($9);
		data_field->setIILName($12);
		if (mode == "combine")
			data_field->setNeedSplit(true);
		else
			data_field->setNeedSplit(false);

		if (mode == "single")
			data_field->setUseKeyAsValue(true);
		else
			data_field->setUseKeyAsValue(false);

		data_field->setNeedSwap(false);

		gAosJQLParser.appendStatement(data_field);
		$$ = data_field;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE DATA FIELD W_WORD TYPE STRING FORMATTER
	VALUE FROM STRING 
	FROM STRING TO STRING ';' 
	{
		AosJqlStmtDataField *data_field = new AosJqlStmtDataField;
		data_field->setName($4);
		data_field->setType($6);
		data_field->setFromFieldName($10);
		data_field->setFromName($12);
		data_field->setToName($14);
		gAosJQLParser.appendStatement(data_field);
		$$ = data_field;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE DATA FIELD W_WORD '(' stmt_expr_name_values ')' ';'
	{
		AosJqlStmtDataField* stmt = new AosJqlStmtDataField;
		stmt->setName($4);
		stmt->setConfParms($6);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
	}
    ;

data_field_max_len:
	{	
		$$ = 0;
	}
	|
	MAX LENGTH INTNUM
	{
		$$ = $3;
	}
	;

data_field_default_value:
	{
		$$ = NULL;
	}
	|
	DEFAULT VALUE STRING 
	{
		$$ = $3; 
	}	
	;

data_idfamily:
	{
		$$ = NULL;
	}
	|
	IDFAMILY W_WORD 
	{
		$$ = $2;
	}
data_field_offset:
	{
		$$ = -1;
	}
	|
	OFFSET INTNUM
	{
		$$ = $2;
	}
	;

stmt_drop_data_field:
	DROP DATA FIELD W_WORD ';'
	{
		AosJqlStmtDataField *data_field = new AosJqlStmtDataField;
		data_field->setName($4);
		gAosJQLParser.appendStatement(data_field);
		$$ = data_field;
		$$->setOp(JQLTypes::eDrop);
	}
	;

stmt_show_data_field:
	SHOW DATA FIELDS ';'
	{
		AosJqlStmtDataField *data_field = new AosJqlStmtDataField;
		gAosJQLParser.appendStatement(data_field);
		$$ = data_field;
		$$->setOp(JQLTypes::eShow);
	}
	;

stmt_describe_data_field:
	DESCRIBE DATA FIELD W_WORD ';'
	{
		AosJqlStmtDataField *data_field = new AosJqlStmtDataField;
		data_field->setName($4);
		gAosJQLParser.appendStatement(data_field);
		$$ = data_field;
		$$->setOp(JQLTypes::eDescribe);
	}
	;

iilmap_mode:
	MODE COMBINE
	{
		$$ = OmnString("combine").getBuffer();
	}
	|
	MODE SINGLE
	{
		$$ = OmnString("single").getBuffer();
	}
	;

