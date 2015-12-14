stmt_create_dataset:
	CREATE DATASET W_WORD '(' stmt_expr_name_values ')' ';'
	{
		AosJqlStmtDataset* stmt = new AosJqlStmtDataset;
		stmt->setName($3);
		stmt->setConfParms($5);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE DATASET W_WORD DATA SCANNER W_WORD PARSER W_WORD ';'
	{
		AosJqlStmtDataset* statement = new AosJqlStmtDataset;

		statement->setName($3);
		statement->setScanner($6);
		statement->setSchema($8);
		gAosJQLParser.appendStatement(statement);

		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
		//cout << "Create dataset: " << $3 << endl;
	}
	|
	CREATE DATASET W_WORD DATA SCANNER W_WORD DATA SCHEMA W_WORD ';'
	{
		AosJqlStmtDataset* statement = new AosJqlStmtDataset;

		statement->setName($3);
		statement->setScanner($6);
		statement->setSchema($9);
		
		gAosJQLParser.appendStatement(statement);

		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
		//cout << "Create dataset: " << $3 << endl;
	}
	|
	CREATE DATASET W_WORD ';'
	{
		AosJqlStmtDataset* statement = new AosJqlStmtDataset;
		statement->setName($3); 
		gAosJQLParser.appendStatement(statement);  
		                                           
		$$ = statement;                            
		$$->setOp(JQLTypes::eCreate);              
	};

stmt_show_dataset:
	SHOW DATASETS ';'
	{
		AosJqlStmtDataset* statement = new AosJqlStmtDataset;

		gAosJQLParser.appendStatement(statement);

		$$ = statement;
		$$->setOp(JQLTypes::eShow);
		//cout << "List all the Datasets" << endl;
	};

stmt_describe_dataset:
	DESCRIBE DATASET W_WORD ';'
	{
		AosJqlStmtDataset* statement = new AosJqlStmtDataset;

		gAosJQLParser.appendStatement(statement);

		$$ = statement;
		$$->setName($3);
		$$->setOp(JQLTypes::eDescribe);
		//cout << "Display the Dataset: " << $3 << endl;
	};

stmt_drop_dataset:
	DROP DATASET W_WORD ';'
	{
		AosJqlStmtDataset* statement = new AosJqlStmtDataset;
		statement->setName($3);

		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eDrop);
	}
	;

