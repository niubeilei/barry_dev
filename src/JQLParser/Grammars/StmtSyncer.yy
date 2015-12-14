stmt_create_syncer:
	CREATE SYNCER expr SERVICE
	'(' expr_list ')' ';'
	{
		AosJqlStmtSyncer * statement = new AosJqlStmtSyncer;
		statement->setName($3);
		statement->setParms($6);
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	;


stmt_drop_syncer:
	DROP SYNCER expr ';'
	{
		AosJqlStmtSyncer * statement = new AosJqlStmtSyncer;
		statement->setName($3);
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eDrop);
	}
	;

