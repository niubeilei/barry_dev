stmt_join_syncher:
	CREATE JOIN '(' expr_list ')' 
	ON '(' expr ')' 
	INDEX '(' expr_list ')' ';'
	{
		AosJqlStmtJoinSyncher *stmt = new AosJqlStmtJoinSyncher;
		stmt->setJoinType("inner_join");
		stmt->setJoinTables($4);
		stmt->setCondition($8);
		stmt->setJoinIndexs($12);
		stmt->setOp(JQLTypes::eCreate);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
	};
