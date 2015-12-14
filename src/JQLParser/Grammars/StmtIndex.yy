stmt_create_index:
	CREATE INDEX W_WORD ON TABLE W_WORD 
	'(' expr_list ')'
	opt_where ';'
	{
		AosJqlStmtIndex* statement = new AosJqlStmtIndex;
		statement->setIndexName($3);
		statement->setTableName($6);
		//statement->setFieldIndexColumns($8);
		statement->setWhereConds($10);
		// statement->setFieldIndexColumns($9);
		gAosJQLParser.appendStatement(statement);

		AosJqlStmtIndex::AosFieldIdxCols *field_list = new AosJqlStmtIndex::AosFieldIdxCols; 
		field_list->mFieldList = $8;
		field_list->mIndexName= "";
		field_list->mType = "";

		statement->setFieldList(field_list);

		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE INDEX W_WORD ON TABLE W_WORD 
	WITH MULTIPLE RECORDS 
	'{' stmt_index_records	'}' ';'
	{
		AosJqlStmtIndex* statement = new AosJqlStmtIndex;
		statement->setIndexName($3);
		statement->setTableName($6);
		statement->setIndexRecords($11);
		gAosJQLParser.appendStatement(statement);

		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE INDEX W_WORD ON W_WORD opt_where ';'
	{
		AosJqlStmtIndex* stmt = new AosJqlStmtIndex;
		stmt->setIndexName($3);
		stmt->setTableName($5);
		stmt->setWhereConds($6);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
	};

stmt_describe_index:
	DESCRIBE INDEX W_WORD ';'
	{
		AosJqlStmtIndex* statement = new AosJqlStmtIndex;
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setIndexName($3);
		$$->setOp(JQLTypes::eDescribe);

		cout << "Describe index: " << $3 << endl;
	}
	;

stmt_list_index:
	LIST INDEX expr stmt_list_key_list opt_where opt_limit';'
	{
		AosJqlStmtIndex* statement = new AosJqlStmtIndex;
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setIndexName($3->getValue(0));
		$$->setKeysExpr($4);
		$$->setWhereConds($5);
		$$->setLimit($6);
		$$->setOp(JQLTypes::eList);
	}
	;

stmt_list_key_list:
	{
		$$ = NULL;
	}
	|
	'(' expr_list ')'
	{
		$$ = $2;
	}
	;

show_iil_stmt:
   	SHOW INDEXES ';'
    {
		AosJqlStmtIndex *show_iil = new AosJqlStmtIndex();	
		gAosJQLParser.appendStatement(show_iil);
		$$ = show_iil;
		$$->setOp(JQLTypes::eShow); 
    }
    ;

drop_iil_stmt:
	DROP INDEX W_WORD ';'
	{
		AosJqlStmtIndex *drop_iil = new AosJqlStmtIndex();
		drop_iil->setIndexName($3);
		gAosJQLParser.appendStatement(drop_iil);
		$$ = drop_iil;
		$$->setOp(JQLTypes::eDrop); 
		free($3);
	}
	;

stmt_index_records:
    '(' expr_list ')'	
	{
		$$ = new vector<AosExprList *>;
		$$->push_back($2);
	}
	|
	stmt_index_records ',' '(' expr_list ')'
	{
		$$ = $1;
		$$->push_back($4);
	}
	;

