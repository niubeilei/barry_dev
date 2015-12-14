stmt_create_schema:
	CREATE STATIC SCHEMA W_WORD
	TYPE W_WORD 
	RECORD W_WORD ';'
	{
		AosJqlStmtSchema* statement = new AosJqlStmtSchema;
		statement->setName($4);
		statement->setType($6);
		statement->setRecord($8);
		statement->setSchemaType("static");
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE PARSER W_WORD 
	TYPE W_WORD
	SCHEMA W_WORD ';'
	{
		AosJqlStmtSchema* statement = new AosJqlStmtSchema;
		OmnString type = "parser";
		statement->setRecordType(type);
		statement->setName($3);
		statement->setType($5);
		statement->setRecord($7);
		statement->setSchemaType("static");
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE PARSER W_WORD 
	TYPE W_WORD 
	record_delimiter
	RECORD PICKER BY BYTES INTNUM ',' INTNUM 
	'(' stmt_record_pick_list ')' ';'
	{
		AosJqlStmtSchema* statement = new AosJqlStmtSchema;
		statement->setName($3);
		statement->setType("fixedlength");
		statement->setRecordLen(2000);
		statement->setRecordDelimiter($6);
		statement->setSchemaPos($11);
		statement->setSchemaLen($13);
		statement->setPickers($15);
		statement->setSchemaType("static");
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	;


stmt_record_pick_list:
	SCHEMA W_WORD KEY W_WORD LENGTH INTNUM 
	{
	 	$$ = new vector<AosJqlRecordPickerPtr>;
		AosJqlRecordPickerPtr record_pick = OmnNew AosJqlRecordPicker;
		record_pick->record = $2;
		record_pick->matchStr = $4;
		record_pick->offset = $6;
		free($2);
		free($4);
		$$->push_back(record_pick);
	}
	|
	stmt_record_pick_list ',' SCHEMA W_WORD KEY W_WORD LENGTH INTNUM
	{
		$$ = $1;
		AosJqlRecordPickerPtr record_pick = OmnNew AosJqlRecordPicker;
		record_pick->record = $4;
		record_pick->matchStr = $6;
		record_pick->offset = $8;
		free($4);
		free($6);
		$$->push_back(record_pick);
	}
	;

stmt_show_schema:
	SHOW PARSER ';'
	{
		AosJqlStmtSchema* statement = new AosJqlStmtSchema;

		gAosJQLParser.appendStatement(statement);

		$$ = statement;
		$$->setOp(JQLTypes::eShow);
		//cout << "List all the Schema " << endl;
	};

stmt_describe_schema:
	DESCRIBE PARSER W_WORD ';'
	{
		AosJqlStmtSchema* statement = new AosJqlStmtSchema;
		gAosJQLParser.appendStatement(statement);

		$$ = statement;
		$$->setName($3);
		$$->setOp(JQLTypes::eDescribe);
	};
  
stmt_drop_schema:
	DROP PARSER W_WORD ';'
	{
		AosJqlStmtSchema* statement = new AosJqlStmtSchema;
		statement->setName($3);

		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eDrop);
	}
	;
