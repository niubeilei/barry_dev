update_stmt: 
	UPDATE 
		expr	
	SET 
		update_asgn_list opt_where ';'
	{
		 AosJqlStmtUpdate *u1 = new AosJqlStmtUpdate;
		 u1->table = $2;
		 u1->update_expr_list = $4;
		 if ($5) u1->opt_where = $5->getWhereExpr().getPtr();

		 AosJqlStmtUpdateItem * update = new AosJqlStmtUpdateItem(u1);
		 gAosJQLParser.appendStatement(update);
		 $$ = update;

		 //cout << "found UPDATA table_references SET update_asgn_list " << endl;
	}
	;

update_asgn_list:
	W_WORD COMPARISON expr 
	{ 
		 if ($2 != AosExprComparison::eEqual) yyerror("bad update assignment to %s", $2);

		 $$ = new AosExprList;

		 AosExprObj *pExpr1 = new AosExprString($1);
		 AosExprObj *pExpr = $3; 
		 AosExpr	*pExpr2 = new AosExprComparison(pExpr1, AosExprComparison::eEqual, pExpr);    
		 pExpr2->setExpr(pExpr1, 1);
		 pExpr2->setExpr(pExpr, 2);

		 free($1);

		 $$->push_back(pExpr2);
	}
	/*
	|
	W_WORD '.' W_WORD COMPARISON expr 
	{ 
		 //cout << "FOUND update_asgn_list  W_WORD '.' W_WORD COMPARISON expr " << endl;
	}
	*/
	|
	update_asgn_list ',' W_WORD COMPARISON expr
	{ 
		 if ($4 != AosExprComparison::eEqual) yyerror("bad update assignment to %s", $4);

		 AosExprObj *pExpr1 = new AosExprString($3);
		 AosExprObj *pExpr = $5; 
		 AosExpr	*pExpr2 = new AosExprComparison(pExpr1, AosExprComparison::eEqual, pExpr);    
		 pExpr2->setExpr(pExpr1, 1);
		 pExpr2->setExpr(pExpr, 2);
		 free($3);
		 $$->push_back(pExpr2);
	}
	/*
	|
	update_asgn_list ',' W_WORD '.' W_WORD COMPARISON expr
	{ 
		 //cout << "FOUND update_asgn_list ',' W_WORD '.' W_WORD COMPARISON expr" << endl;
	}
	*/
	;

