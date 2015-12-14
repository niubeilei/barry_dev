delete_stmt: 
	DELETE 
		delete_opts
	FROM expr opt_where ';'
	{
		AosJqlStmtDelete *d = new AosJqlStmtDelete;
		d->table= $4;
		if ($5) d->opt_where = $5->getWhereExpr().getPtr();

		AosJqlStmtDeleteItem *del = new AosJqlStmtDeleteItem(d);	
		gAosJQLParser.appendStatement(del);
		$$ = del;
	}
	;

delete_opts:
	/* nil */ 
	{
		//cout << "============ " << endl;
	}
	| 
	delete_opts LOW_PRIORITY 
	{
		//cout << "============  delete_opts LOW_PRIORITY  " << endl;
	}
	|
	delete_opts QUICK 
	{
		//cout << "============  delete_opts QUICK " << endl;
	}
	|
	delete_opts IGNORE 
	{
		//cout << "============ delete_opts IGNORE " << endl;
	}
	;
/*
delete_list: 
	expr
	{
		//$$ = new AosExprList;
		//$$->push_back($1);
		//cout << "============   W_WORD opt_dot_star " << endl;
	}
	| 
	delete_list ',' expr 
	{
		//$$->push_back($3);
		//cout << "============  delete_list ',' W_WORD opt_dot_star " <<endl;
	}
	'*'
	{
	    //$ = new AosExprList;                       
		//$$->push_back(new AosExprAtomic("all"));
	}
	;
*/

