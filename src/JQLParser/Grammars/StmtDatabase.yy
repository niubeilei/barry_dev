stmt_create_database:
	CREATE DATABASE W_WORD ';'
	{
		AosJqlStmtDatabase* statement = new AosJqlStmtDatabase;
		
		statement->setName($3);

		gAosJQLParser.appendStatement(statement);

		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
		//cout << "Create database:" << " name " << $3 << endl;
	};

stmt_drop_database:
	DROP DATABASE W_WORD ';'
	{
		AosJqlStmtDatabase* statement = new AosJqlStmtDatabase;
		statement->setName($3);
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eDrop); 
	}
	;

stmt_show_databases:
	SHOW DATABASES ';'
	{
		AosJqlStmtDatabase* statement = new AosJqlStmtDatabase;

		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eShow); 
	}
	;

stmt_use_database:
	USE W_WORD ';'
	{
		AosJqlStmtDatabase* statement = new AosJqlStmtDatabase;

		gAosJQLParser.appendStatement(statement);
		statement->setName($2);
		$$ = statement;
		$$->setOp(JQLTypes::eUse); 
	}
	;
