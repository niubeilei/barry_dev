insert_stmt: 
	INSERT insert_opts opt_into table_name opt_col_names VALUES insert_vals_list ';'
	{
		AosJqlStmtInsert* insert = new AosJqlStmtInsert;
		insert->table_name = $4;
		insert->opt_col_names = $5;
		insert->insert_vals_list = $7;
		$$ = new  AosJqlStmtInsertItem(insert);
		gAosJQLParser.appendStatement($$);
	}
	;

insert_opts: 
	/* nil */ 
	{  
	}
	| 
	insert_opts LOW_PRIORITY 
	{
	}
	| 
	insert_opts DELAYED 
	{ 
	}
	|
	insert_opts HIGH_PRIORITY 
	{ 
	}
	|
	insert_opts IGNORE 
	{ 
	}
	;

opt_into: 
	INTO 
	{
	}
	| 
	/* nil */
	{
	}
	;

opt_col_names: 
	/* nil */ 
	{
		$$ = new AosExprList;
	}
	| 
	'(' column_list ')' 
	{ 
		$$ = $2; 
	}
	;

column_list: 
	expr	
	{
		$$ = new AosExprList;
		$$->push_back($1);
	}
	|
	column_list ',' expr 
	{ 
		$$->push_back($3);
	}
	;

insert_vals_list: 
	'(' insert_vals ')' 
	{ 
		$$ = $2;
	}
	;

insert_vals:
	expr 
	{ 
		$$ = new AosExprList;
		$$->push_back($1); 
	}
	| 
	DEFAULT 
	{ 
	}
	| 
	insert_vals ',' expr 
	{ 
		$$->push_back($3); 
	}
	| 
	insert_vals ',' DEFAULT 
	{ 
	}
	;
