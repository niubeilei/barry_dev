stmt_create_user:
	CREATE USER STRING IDENTIFIED BY STRING ';'
	{
		AosJqlStmtUserMgr *user_mgr = new AosJqlStmtUserMgr;
		user_mgr->setUserName($3);
		user_mgr->setPwd($6);
		gAosJQLParser.appendStatement(user_mgr);
		$$ = user_mgr;
		$$->setOp(JQLTypes::eCreate);
	}
    ;


stmt_drop_user:
	DROP USER W_WORD ';'
	{
		AosJqlStmtUserMgr *user_mgr = new AosJqlStmtUserMgr;
		user_mgr->setUserName($3);
		gAosJQLParser.appendStatement(user_mgr);
		$$ = user_mgr;
		$$->setOp(JQLTypes::eDrop);
	}
	;

stmt_show_user:
	SHOW USERS ';'
	{
		AosJqlStmtUserMgr *user_mgr = new AosJqlStmtUserMgr;
		gAosJQLParser.appendStatement(user_mgr);
		$$ = user_mgr;
		$$->setOp(JQLTypes::eShow);
	}
	;

stmt_describe_user:
	DESCRIBE USER W_WORD ';'
	{
		AosJqlStmtUserMgr *user_mgr = new AosJqlStmtUserMgr;
		user_mgr->setUserName($3);
		gAosJQLParser.appendStatement(user_mgr);
		$$ = user_mgr;
		$$->setOp(JQLTypes::eDescribe);
	}
	;

stmt_alter_user:
	ALTER USER STRING IDENTIFIED BY STRING ';'
	{
		AosJqlStmtUserMgr *user_mgr = new AosJqlStmtUserMgr;
		user_mgr->setUserName($3);
		user_mgr->setPwd($6);
		gAosJQLParser.appendStatement(user_mgr);
		$$ = user_mgr;
		$$->setOp(JQLTypes::eAlter);	
	}
	;

