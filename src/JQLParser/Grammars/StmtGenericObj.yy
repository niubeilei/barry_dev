stmt_create_genericobj:
	CREATE expr expr 
	'(' stmt_genericobj_value_list ')' ';'
	{
		AosJqlStmtGenericobj * statement = new AosJqlStmtGenericobj;
		statement->setType($2);
		statement->setName($3);
//		statement->setGenericobjValues($5);
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	;

stmt_genericobj_value_list:
	{
		$$ = NULL;
	}
	|
	stmt_genericobj_value
	{
		$$ = new vector<AosGenericValueObjPtr>();
		$$->push_back($1);
	}
	|
	stmt_genericobj_value_list ',' stmt_genericobj_value
	{
		$$ = $1;
		$$->push_back($3);
	}
	;

stmt_genericobj_value:
	{
		$$ = NULL;
	}
	|
	expr
	{
		$$ = new AosGenericValueObj();
		$$->mValue = $1;
	}
	|
	expr ':' '[' expr_list ']'
	{
		$$ = new AosGenericValueObj();
		$$->mName = $1;
		$$->mArrayValues = *$4;
	}
	;


