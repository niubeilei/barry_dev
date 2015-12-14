stmt_create_actor:
	CREATE ACTOR JAVA STRING STRING STRING STRING STRING ';'
    {
		AosJqlStmtActor *actor= new AosJqlStmtActor;
	           actor->setName($4);
	           actor->setClassName($5);
	           actor->setPath($6);
	           actor->setType_language($7);
	           actor->setDescription($8);
			   gAosJQLParser.appendStatement(actor);
		       $$ = actor;
			   $$->setOp(JQLTypes::eCreate);
    }
    ;

stmt_run_actor:
	RUN ACTOR STRING par_key_names VALUES run_vals_list ';'
	{
		AosJqlStmtActor *actor = new AosJqlStmtActor;

		actor->setName($3);
		actor->setKeys($4);
		actor->setValues($6);
		gAosJQLParser.appendStatement(actor);

		$$ = actor;
		$$->setOp(JQLTypes::eRun);
	}
	;

par_key_names:
		opt_col_names
		{
			$$ = $1;
		}
		;

run_vals_list:
		insert_vals_list
		{
			$$ = $1;
		}
		;
