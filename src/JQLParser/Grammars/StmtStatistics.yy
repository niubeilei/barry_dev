stmt_create_statistics:
	CREATE STATISTICS W_WORD ON TABLE W_WORD ';'
	{
		AosJqlStmtStatistics* statement = new AosJqlStmtStatistics;
		statement->setStatName($3);
		statement->setTableName($6);
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE STATISTICS MODEL W_WORD ON TABLE expr_list 
	opt_where GROUP BY '(' stmt_dimensions_list ')'
	MEASURES '(' stmt_stat_measure_list ')' 
	opt_shuffle_field  ';'
	{
		AosJqlStmtStatistics* statement = new AosJqlStmtStatistics;
		statement->setStatName($4);
		statement->setTableNames($7);
		if ($8 != 0)
			statement->setWhereCond($8->getWhereExpr().getPtr());
		statement->setGroupByFields($12);
		statement->setMeasures($16);
		statement->setShuffleField($18);
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE STATISTICS MODEL W_WORD ON TABLE expr_list 
	opt_where MEASURES '(' stmt_stat_measure_list ')'
	opt_shuffle_field  ';'
	{
		AosJqlStmtStatistics* statement = new AosJqlStmtStatistics;
		statement->setStatName($4);
		statement->setTableNames($7);
		if ($8 != 0)
			statement->setWhereCond($8->getWhereExpr().getPtr());
		statement->setMeasures($11);
		statement->setShuffleField($13);
		
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE STATISTICS MODEL W_WORD ON TABLE expr_list 
	opt_where MEASURES '(' stmt_stat_measure_list ')'
	TIME FIELD expr UNIT STRING 
	opt_shuffle_field  ';'
	{
		AosJqlStmtStatistics* statement = new AosJqlStmtStatistics;
		statement->setStatName($4);
		statement->setTableNames($7);
		if ($8 != 0)
			statement->setWhereCond($8->getWhereExpr().getPtr());
		statement->setMeasures($11);
		statement->setTimeFieldName($15);
		statement->setTimeUnit($17);
		statement->setShuffleField($18);

		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE STATISTICS MODEL W_WORD ON TABLE expr_list 
	opt_where MEASURES '(' stmt_stat_measure_list ')'
	TIME FIELD expr FORMAT STRING UNIT STRING
	opt_shuffle_field  ';'
	{
		AosJqlStmtStatistics* statement = new AosJqlStmtStatistics;
		statement->setStatName($4);
		statement->setTableNames($7);
		if ($8 != 0)
			statement->setWhereCond($8->getWhereExpr().getPtr());
		statement->setMeasures($11);
		statement->setTimeFieldName($15);
		statement->setTimeOriginalFormat($17);
		statement->setTimeUnit($19);
		statement->setShuffleField($20);

		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE STATISTICS MODEL W_WORD ON TABLE expr_list 
	opt_where GROUP BY '(' stmt_dimensions_list ')'
	MEASURES '(' stmt_stat_measure_list ')'
	TIME FIELD expr UNIT STRING 
	opt_shuffle_field  ';'
	{
		AosJqlStmtStatistics* statement = new AosJqlStmtStatistics;
		statement->setStatName($4);
		statement->setTableNames($7);
		if ($8 != 0)
			statement->setWhereCond($8->getWhereExpr().getPtr());
		statement->setGroupByFields($12);
		statement->setMeasures($16);
		statement->setTimeFieldName($20);
		statement->setTimeUnit($22);
		statement->setShuffleField($23);

		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	CREATE STATISTICS MODEL W_WORD ON TABLE expr_list 
	opt_where GROUP BY '(' stmt_dimensions_list ')'
	MEASURES '(' stmt_stat_measure_list ')'
	TIME FIELD expr FORMAT STRING UNIT STRING
	opt_shuffle_field ';'
	{
		AosJqlStmtStatistics* statement = new AosJqlStmtStatistics;
		statement->setStatName($4);
		statement->setTableNames($7);
		if ($8 != 0)
			statement->setWhereCond($8->getWhereExpr().getPtr());
		statement->setGroupByFields($12);
		statement->setMeasures($16);
		statement->setTimeFieldName($20);
		statement->setTimeOriginalFormat($22);
		statement->setTimeUnit($24);
		statement->setShuffleField($25);

		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eCreate);
	}
	|
	UPDATE STATISTICS MODEL W_WORD ON TABLE W_WORD 
	GROUP BY '(' stmt_dimensions_list ')' ';'
	{
		AosJqlStmtStatistics* statement = new AosJqlStmtStatistics;
		statement->setStatName($4);
		statement->setTableName($7);
		statement->setDimensions($11);

		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eUpdate);
	}
	|
	DROP STATISTICS W_WORD ';'
	{
		AosJqlStmtStatistics* statement = new AosJqlStmtStatistics;
		statement->setStatName($3);
		gAosJQLParser.appendStatement(statement);
		$$ = statement;
		$$->setOp(JQLTypes::eDrop);
	}
	;

opt_shuffle_field:
	 {
	 	$$ = NULL;
	 }
	 |
	 SHUFFLE FIELD '('expr_list')'
	 {
	 	$$ = new AosExprList;
		$$ = $4;
	 }
	 ;
stmt_dimensions_list:
	stmt_dimension_list
	{
		$$ = new vector<AosExprList*>;
		$$->push_back($1);
	}
	|
	stmt_dimensions_list ',' stmt_dimension_list
	{
		$$->push_back($3);
	}
	;

stmt_dimension_list:
	column_list 
	{
		$$ = $1;
	}
	|
	DISTINCT W_WORD ON column_list
	{
		OmnString distName = "distinct:";
		AosExprObj *expr;
		
		$$ = $4;
		distName << $2;
		expr = new AosExprFieldName(distName.getBuffer());
		$$->push_back(expr);
	}
	;

stmt_stat_measure_list:
		stmt_stat_measure
		{
			$$ = new vector<AosJqlStmtStatistics::AosMeasure*>;
			$$->push_back($1);
		}
		|
		stmt_stat_measure_list ',' stmt_stat_measure  
		{
			$$->push_back($3);
		}
		;

stmt_stat_measure:
		expr
		{
			$$ = new AosJqlStmtStatistics::AosMeasure;
			$$->mExpr = $1;
		}
		|
		expr ON column_list
		{
			$$ = new AosJqlStmtStatistics::AosMeasure;
			$$->mExpr = $1;
			$$->mExprList = $3;
		}
		;


stmt_show_statistics:
	SHOW STATISTICS ';'
	{
	    AosJqlStmtStatistics *statistics = new AosJqlStmtStatistics;
	  	gAosJQLParser.appendStatement(statistics);   
	    $$ = statistics;                             
		$$->setOp(JQLTypes::eShow);
	}
	;


stmt_describe_statistics:
	DESCRIBE STATISTICS W_WORD ';'
	{
		AosJqlStmtStatistics *statistics = new AosJqlStmtStatistics;
		statistics->setStatName($3);
		gAosJQLParser.appendStatement(statistics);   
		$$ = statistics;                             
		$$->setOp(JQLTypes::eDescribe);        
	}
	;
