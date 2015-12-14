stmt_create_dict:
	CREATE DICTIONARY W_WORD ON TABLE W_WORD
	KEY '(' expr_list ')'
	opt_where ';'
	{
		AosJqlStmtDictionary *stmt = new AosJqlStmtDictionary;
		stmt->setDictName($3);
		stmt->setTableName($6);
		stmt->setKeys($9);
		if ($11) stmt->setWhereCond($11->getWhereExpr().getPtr());
		stmt->setOp(JQLTypes::eCreate);               
		gAosJQLParser.appendStatement(stmt);   
		$$ = stmt;                             
	};

stmt_drop_dict:
	DROP DICTIONARY W_WORD ';'
	{
		AosJqlStmtDictionary *Dict = new AosJqlStmtDictionary;
		Dict->setDictName($3);
		gAosJQLParser.appendStatement(Dict);   
		$$ = Dict;                             
		$$->setOp(JQLTypes::eDrop);               
	};

stmt_show_dict:
	SHOW DICTIONARY ';'
	{
		AosJqlStmtDictionary *Dict = new AosJqlStmtDictionary;
		gAosJQLParser.appendStatement(Dict);   
		$$ = Dict;                             
		$$->setOp(JQLTypes::eShow);               
	};

stmt_describe_dict:
	DESCRIBE DICTIONARY W_WORD ';'
	{
		AosJqlStmtDictionary *Dict = new AosJqlStmtDictionary;
		Dict->setDictName($3);
		gAosJQLParser.appendStatement(Dict);   
		$$ = Dict;                             
		$$->setOp(JQLTypes::eDescribe);               
	};
