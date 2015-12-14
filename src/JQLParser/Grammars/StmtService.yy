stmt_create_service:
	CREATE SERVICE W_WORD USING JOB W_WORD ';' 
	{
		AosJqlStmtService* statement = new AosJqlStmtService;

		statement->setServiceName($3);
		statement->setJobName($6);
		gAosJQLParser.appendStatement(statement);

		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	;

stmt_start_service:
   	START SERVICE W_WORD ';'
    {
		AosJqlStmtService* statement = new AosJqlStmtService();	
		gAosJQLParser.appendStatement(statement);

		statement->setServiceName($3);
		$$ = statement;
		$$->setOp(JQLTypes::eStart); 
    }
	|
	START SERVICE W_WORD '{' stmt_expr_name_values '}' ';'
	{
		AosJqlStmtService *stmt = new AosJqlStmtService;
		stmt->setServiceName($3);
		stmt->setConfParms($5);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eStart);
	}
    ;

stmt_stop_service:
   	STOP SERVICES W_WORD ';'
    {
		AosJqlStmtService* statement = new AosJqlStmtService();	
		gAosJQLParser.appendStatement(statement);

		statement->setServiceName($3);
		$$ = statement;
		$$->setOp(JQLTypes::eStop); 
    }
    ;

stmt_show_service:
   	SHOW SERVICES ';'
    {
		AosJqlStmtService* statement = new AosJqlStmtService();	
		gAosJQLParser.appendStatement(statement);

		$$ = statement;
		$$->setOp(JQLTypes::eShow); 
    }
    ;

stmt_describe_service:
   	DESCRIBE SERVICE W_WORD ';'
    {
		AosJqlStmtService* statement = new AosJqlStmtService();	
		gAosJQLParser.appendStatement(statement);

		statement->setServiceName($3);
		$$ = statement;
		$$->setOp(JQLTypes::eDescribe); 
    }
    ;
stmt_drop_service:
   	DROP SERVICE W_WORD ';'
    {
		AosJqlStmtService* statement = new AosJqlStmtService();	
		gAosJQLParser.appendStatement(statement);

		statement->setServiceName($3);
		$$ = statement;
		$$->setOp(JQLTypes::eDrop); 
    }
    ;
