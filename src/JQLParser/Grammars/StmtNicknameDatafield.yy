stmt_create_nick_field:
	CREATE NICK FIELD W_WORD '(' stmt_expr_name_values ')' ';'
	{
		AosJqlStmtNickField *stmt = new AosJqlStmtNickField;
		stmt->setName($4);
		stmt->setConfParms($6);
		gAosJQLParser.appendStatement(stmt);
		$$ = stmt;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE NICK FIELD STRING
		ON W_WORD '(' STRING ')' 
	    data_field_max_len ';'
	{
		AosJqlStmtNickField *field = new AosJqlStmtNickField;
		field->setName($4);
		field->setTable($6);
		field->setOrigFieldName($8);
		field->setMaxLen($10);
		gAosJQLParser.appendStatement(field);
		$$ = field;
		$$->setOp(JQLTypes::eCreate);
	}
    ;

