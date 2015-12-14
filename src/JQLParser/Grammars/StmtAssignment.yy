stmt_assignment:
	USERVAR  data_field_data_type ';' 
	{
		OmnString name = $1;
		AosJqlStmtAssignment *stmt = new AosJqlStmtAssignment();
		stmt->setName(name);
		stmt->setType($2);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
		gAosJQLParser.appendStatement(stmt);
	}
	|
	USERVAR  data_field_data_type  ASSIGNMENT expr ';' 
	{
		OmnString name = $1;
		AosJqlStmtAssignment *stmt = new AosJqlStmtAssignment();
		stmt->setName(name);
		stmt->setType($2);
		stmt->setValue($4);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
		gAosJQLParser.appendStatement(stmt);
	}
	|
	USERVAR  ASSIGNMENT expr ';' 
	{
		OmnString name = $1;
		AosJqlStmtAssignment *stmt = new AosJqlStmtAssignment();
		stmt->setName(name);
		stmt->setValue($3);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
		gAosJQLParser.appendStatement(stmt);
	}
	|
	USERVAR  ASSIGNMENT SYSTEM_ARGS '(' INTNUM ')' ';'
	{
		OmnString name = $1;
		AosJqlStmtAssignment *stmt = new AosJqlStmtAssignment();
		stmt->setName(name);
		stmt->setIsParms(true);
		stmt->setParms($5);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
		gAosJQLParser.appendStatement(stmt);
	}
	|
	USERVAR  data_field_data_type ASSIGNMENT SYSTEM_ARGS '(' INTNUM ')' ';'
	{
		OmnString name = $1;
		AosJqlStmtAssignment *stmt = new AosJqlStmtAssignment();
		stmt->setName(name);
		stmt->setType($2);
		stmt->setIsParms(true);
		stmt->setParms($6);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
		gAosJQLParser.appendStatement(stmt);
	}

	;



		
