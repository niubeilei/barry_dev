stmt_union:
	query_union ';'
	{
		$$ = $1;	
		gAosJQLParser.appendStatement($1);             
	}
	;

query_union:
	jql_select_stmt stmt_union_opt jql_select_stmt
	{
		AosJqlStmtQueryUnion *stmt_union = new AosJqlStmtQueryUnion();
		AosJqlStmtQuery *query1 = new AosJqlStmtQuery($1);
		AosJqlStmtQuery *query2 = new AosJqlStmtQuery($3);

		stmt_union->setIsUnionAll($2);
		stmt_union->addQuery(query1);
		stmt_union->addQuery(query2);

		$$ = stmt_union;   
	}
	|
	query_union stmt_union_opt jql_select_stmt 
	{
		AosJqlStmtQueryUnion *stmt_union = $1;
		AosJqlStmtQuery *query = new AosJqlStmtQuery($3);

		stmt_union->addQuery(query);
		$$ = stmt_union;   
	}
	;

stmt_union_opt:
	UNION
	{
		$$ = false;
	}
	|
	UNION ALL
	{
		$$ = true;
	}
	;

