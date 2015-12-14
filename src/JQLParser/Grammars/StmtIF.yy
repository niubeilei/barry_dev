stmt_execution_if:
	IF if_not expr
	if_then_statements
	if_else_statements
	END IF ';'
	{
		AosJqlStmtIF *stmt = new AosJqlStmtIF;
		stmt->setNot($2);
		stmt->setCond($3);
		stmt->setThen($4);
		stmt->setElse($5);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eRun);
	}
    ;

if_not:
	{
		$$ = false;
	}
	|
	NOT
	{
		$$ = true;
	}

if_then_statements:
	THEN statement 
	{
		typedef vector<AosJqlStatementPtr> AosJqlStatementList;
		$$ = new AosJqlStatementList; 
		$$->push_back($2);
		gAosJQLParser.finishParse();
	}
	|
	if_then_statements statement
	{
		$$ = $1;
		$$->push_back($2);
		gAosJQLParser.finishParse();
	}
	;

if_else_statements:
	{
		$$ = NULL;
	}
	|
	ELSE  statement
	{
		typedef vector<AosJqlStatementPtr> AosJqlStatementList;
		$$ = new AosJqlStatementList; 
		$$->push_back($2);
		gAosJQLParser.finishParse();
	}
	|
	if_else_statements statement
	{
		$$ = $1;
		$$->push_back($2);
		gAosJQLParser.finishParse();
	}
	;

