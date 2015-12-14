select_stmt:
	SELECT select_opts select_expr_list';'
	{
		AosJqlStmtQuery * select = new AosJqlStmtQuery();
		gAosJQLParser.appendStatement(select);             
		select->setExprs($3);
		$$ = select;   
	}
	|
	jql_select_stmt ';'
	{
		AosJqlStmtQuery * select = new AosJqlStmtQuery($1);
		gAosJQLParser.appendStatement(select);
		$$ = select;
	//	$$->mQueryStr =gAosJQLParser.getCurrJQL();
	}
	;

jql_select_stmt:
	SELECT
        select_opts select_expr_list
    FROM
        table_references opt_where stmt_group_by opt_having stmt_order_by  opt_limit 
	stmt_hack_convert_to stmt_hack_format  stmt_hack_into_file
    {
		$$ =new AosJqlSelect;
		//s1->mType = $2;
		$$->mFieldListPtr = $3;
		$$->mTableReferencesPtr = $5;
		$$->mWherePtr = $6;
		$$->mGroupByPtr = $7;
		$$->mHavingPtr = $8;
		$$->mOrderByPtr = $9;
		$$->mLimitPtr = $10;
		$$->mHackConvertTo = $11;
		$$->mHackFormat = $12;
		$$->mHackIntoFile = $13;
    }
    ;


select_opts:
	/* nil */
	{ 
		$$ = new AosJQLStmtSelectOptionList; 
	}
	| 
	select_opts ALL                 
	{ 
		$$->push_back(eSelectOpt_SelectAll); 
	}
	| 
	select_opts DISTINCT            
	{
		$$->push_back(eSelectOpt_Distinct);
	}
	| 
	select_opts DISTINCTROW         
	{
		$$->push_back(eSelectOpt_DistinctRow);
	}
	| 
	select_opts HIGH_PRIORITY       
	{
		$$->push_back(eSelectOpt_HighPriority);
	}
	| 
	select_opts STRAIGHT_JOIN       
	{
		$$->push_back(eSelectOpt_StraightJoin);
	}
	| 
	select_opts SQL_SMALL_RESULT    
	{
		$$->push_back(eSelectOpt_SmallResult);
	}
	| 
	select_opts SQL_BIG_RESULT      
	{
		$$->push_back(eSelectOpt_BigResult);
	}
	| 
	select_opts SQL_CALC_FOUND_ROWS 
	{
		$$->push_back(eSelectOpt_CallFoundRows);
	}
	;

select_expr_list: 
	select_expr 
	{ 
		$$ = new AosJqlSelectFieldList;
		$$->push_back($1);
	}
	| 
	select_expr_list ',' select_expr 
	{
		$$->push_back($3); 
	}
	;

select_expr: 
	expr opt_as_alias
	{
		AosJqlSelectField *field = new AosJqlSelectField; 
		if ($1->getType() == AosExprType::eGenFunc)
		{
			AosExprGenFuncPtr expr_func;
			expr_func = dynamic_cast<AosExprGenFunc*>($1);
			AosExprList *parmlist = expr_func->getParmList();
			OmnString func_name = expr_func->getFuctName();
			if (func_name.toLower() == "avg" && parmlist->size() == 1)
			{
				AosExprObjPtr lhs = OmnNew AosExprGenFunc("sum", (*parmlist)[0]);
				AosExprObjPtr rhs = OmnNew AosExprGenFunc("count", (*parmlist)[0]);
				AosExprObjPtr expr = new AosExprArith(lhs, AosExprArith::eDivide, rhs);
				field->setField(expr);
				if ($2)
					field->setAliasName($2);
				else
					field->setAliasName(OmnNew AosExprString(expr_func->getStatFieldName().getBuffer()));
			}
			else
			{
				field->setField($1);
				field->setAliasName($2);
			}
		}
		else
		{
			field->setField($1);
			field->setAliasName($2);
		}
		$$ = field;
	}
	|
	'*'
	{
		 AosJqlSelectField *field = new AosJqlSelectField;
		 AosExprObjPtr expr = OmnNew AosExprFieldName("*");
		 field->setField(expr);
		 $$ = field;
	}
	;

table_references:    
	table_reference 
	{ 
		$$ = new AosTableReferences;
		$$->push_back($1);
	}
	| 
	table_references ',' table_reference 
	{ 
		$$->push_back($3); 
	}
	;

table_reference:  
	table_factor
	{
		$$ = new AosJqlTableReference;
		$$->setTableFactor($1);
	}
	|
	join_table
	{
		$$ = new AosJqlTableReference;
		$$->setJqlJoinTable($1);
	}
	;

join_table:
	/* nil */
	{
		$$ = new AosJqlJoinTable;
	}
	;



table_factor:
    expr opt_as_alias index_hint	
	{
		$$ = new AosJqlTableFactor;
		$$->setType(eTRef_TableFactorByTable);
		$$->setAliasName($2);
		$$->setTable($1);
		$$->setIndexHint($3);
	}
	|
	table_subquery AS W_WORD
	{
		$$ = new AosJqlTableFactor;
		/*$$->setVirtualTable($1);*/
		$$->setQuery($1);
		$$->setTable(new AosExprFieldName($3));
		$$->setAliasName(new AosExprFieldName($3));

	}
	|
	STRING '.' STRING opt_as_alias index_hint
	{
		$$ = new AosJqlTableFactor;
		$$->setTable(new AosExprMemberOpt("", $1, $3));
		$$->setAliasName($4);
		$$->setIndexHint($5);
	}
	;

table_subquery:
	'(' jql_select_stmt ')'
	{
		/*
		$$ = new AosJqlSubQueryTable;
		$$->setVirtualTable($2);
		$$->setName($5);
		*/
		AosJqlStmtQuery *select = new AosJqlStmtQuery($2);
		$$ = select;
	}
	;

index_hint:
	{
		$$ = NULL;
	}
	|
	USE INDEX '(' expr_list ')' 
	{
		/* index hint */			
		$$ = $4;
	}
	;


opt_limit:                                  
	 /* nil */ 
	{
		$$ = 0;
	//  $$ = new AosExprList;
	}
	| 
	LIMIT INTNUM 
	{
	  $$ = new AosJqlLimit;
	  $$->setOffset(0);
	  $$->setRowCount($2);
	}
	|
	LIMIT INTNUM ',' INTNUM
	{
	  $$ = new AosJqlLimit;
	  if ($2 < 0) $2 = 0;
	  $$->setOffset($2);
	  $$->setRowCount($4);
	}
	;


stmt_group_by:
	{
		$$ = 0;
	}
	|
	GROUP BY select_expr_list 
	{
		$$ = new AosJqlGroupBy;
		$$->setGroupFieldList($3);
	}
	|
	GROUP BY ROLLUP '(' rollup_lists ')'
	{
		$$ = new AosJqlGroupBy;
		$$->setRollupLists($5);
	}
	|
	GROUP BY CUBE '(' select_expr_list ')'
	{
		$$ = new AosJqlGroupBy;
		$$->setCubeLists($5);
	}
	|
	GROUP BY select_expr_list ',' CUBE '(' select_expr_list ')'
	{
		$$ = new AosJqlGroupBy;
		$$->setGroupFieldList($3);
		$$->setCubeLists($7);
	}
	;

rollup_lists:
	'(' rollup_list ')'
	{
		vector<AosJqlSelectFieldList*> *rollupLists;

		rollupLists = new vector<AosJqlSelectFieldList*>;
		rollupLists->push_back($2);
		$$ = rollupLists;
	}
	|
	rollup_lists ',' '(' rollup_list ')'
	{
		$$ = $1;
		$$->push_back($4);
	}
	;

rollup_list:
	{
		/*nil, init a expression list*/
		AosJqlSelectFieldList *rollupList = new AosJqlSelectFieldList;
		$$ = rollupList;
	}
	|
	select_expr_list
	{
		$$ = $1;
	}
	;
	
stmt_order_by:
	{
		$$ = 0;
	}
	|
	ORDER BY stmt_order_field_list
	{
		$$ = new AosJqlOrderBy;	
		$$->setOrderFieldList($3);
	}
	;

stmt_order_field:
	expr stmt_order_opt
	{
		$$ =  new AosJqlOrderByField;
		AosJqlSelectField *field = new AosJqlSelectField;
		field->setField($1);
		$$->mField = field;
		$$->mIsAsc = $2;
	}
	;

stmt_order_field_list:
	stmt_order_field
	{
		$$ = new AosJqlOrderByFieldList;
		$$->push_back($1);
	}
	|
	stmt_order_field_list ',' stmt_order_field
	{
		$$->push_back($3);
	}
	;

stmt_order_opt:
	{
		$$ = true;
	}
	|
	DESC
	{
		$$ = false;
	}
	|
	ASC
	{
		$$ = true;
	}
	;

opt_having:
	{
		$$ = 0;
	}
	|
	HAVING expr 
	{
		$$ = new AosJqlHaving();
		$$->setHavingExpr($2);
	}
	;

stmt_hack_format:
	{
		$$ = NULL;
	}
	|
	FORMAT expr
	{
		$$ = $2;
	};

stmt_hack_into_file:
	{
		$$ = NULL;
	}
	|
	INTO expr
	{
		$$ = $2;
	};

stmt_hack_convert_to:
	{
		$$ = NULL;
	}
	|
	CONVERT TO expr
	{
		$$ = $3;
	}
	;
