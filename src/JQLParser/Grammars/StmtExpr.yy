stmt_expr:
	expr ';'
	{	
		AosExprObjPtr expr = $1;
		gAosJQLParser.setExpr(expr);
		$$ = new AosJqlStmtExpr(expr);
		gAosJQLParser.appendStatement($$);
	}
	;

expr: 
	W_WORD
	{
	    //cout << "this is name: " << $1 << endl;
		$$ = new AosExprFieldName($1);
		if ($1) free($1); 
	}
	|
	W_WORD XPATH STRING 
	{
	    //cout << "this is name: " << $1 << endl;
		$$ = new AosExprFieldName($1, $3);
		if ($1) free($1); 
		if ($3) free($3); 
	}
	|
	U8VAR
	{
		//cout << "this is U8VAR: " << $1 << endl;
		$$ = new AosExprString($1);
		if ($1) free($1);
	}
	|
	USERVAR
	{
		OmnString name = $1;
		AosExprUserVar *expr = new AosExprUserVar();
		expr->setName(name);
		$$ = expr;
		if ($1) free($1);
	}
	|
	W_WORD '.' W_WORD 
	{ 
		$$ = new AosExprMemberOpt("", $1, $3);
		if ($1) free($1);
		if ($3) free($3);
	}
    | 
	W_WORD '.' W_WORD '.' W_WORD
	{
		$$ = new AosExprMemberOpt($1, $3, $5);
		if ($1) free($1);
		if ($3) free($3);
		if ($5) free($5);
	}
	|
	STRING	'.' STRING 
	{ 
		$$ = new AosExprMemberOpt("", $1, $3);
		if ($1) free($1);
		if ($3) free($3);
	}
    | 
	STRING '.' STRING '.' STRING 
	{
		$$ = new AosExprMemberOpt($1, $3, $5);
		if ($1) free($1);
		if ($3) free($3);
		if ($5) free($5);
	}
	|

	STRING        
	{ 
	    //cout << "this is string" << endl;
		u32 len = strlen($1);
		if (len > 0)
		{
			char *newch = new char[len+1]; 
			strncpy(newch, $1 , len);
			newch[len] = '\0';
			$$ = new AosExprString(newch);
			delete [] newch;
		}
		else
		{
			$$ = new AosExprString("");
			//cout << "Parser string error!!: " << $1 << endl;
		}
		free($1); 
	}
    | 
	INTNUM	
	{ 
		int64_t e1 = $1;
		//cout << "Found number: " << $1 << endl;
		$$ = new AosExprNumber(e1);
	}
    | 
	DOUBLE_NUM     
	{ 
		$$ = new AosExprDouble($1);
	}
	|
	NULLX
	{
		$$ = new AosExprNull();
	}
	/*
    | 
	BOOL          
	{ 
		int64_t e1 = $1;
        $$ = new AosExprNumber(e1);
	}
	*/
    ;

expr: 
	expr '+' expr 
	{ 
		AosExprObj* e1 = $1;
		AosExprObj* e2 = $3;
		// AosValuePtr value = OmnNew AosValueInt64(0);

		//cout << "Found expression + " << endl;
        $$ = new AosExprArith(e1, AosExprArith::eAdd, e2);
		// if (!$$->getValue(0, 0, (AosValuePtr &)value)) 
		// {
		// 	//cout << "failed to get add result: " << $1 << " + " << $3 << endl;
		// } else 
		// {
		// 	//cout << value->toString() << endl;
		// }
	}
    | 
	expr '-' expr
	{ 
		AosExprObj* e1 = $1;
		AosExprObj* e2 = $3;
		// AosValuePtr value = OmnNew AosValueInt64(0);

		//cout << "Found expression -"  << endl;
        $$ = new AosExprArith(e1, AosExprArith::eMinus, e2);
		// if (!$$->getValue(0, 0, (AosValuePtr &)value)) 
		// {
		// 	//cout << "failed to get minus result: " << $1 << " + " << $3 << endl;
		// } else 
		// {
		// 	//cout << value->toString() << endl;
		// }
	}
    | 
	expr '*' expr
	{ 
		AosExprObj* e1 = $1;
		AosExprObj* e2 = $3;
		// AosValuePtr value = OmnNew AosValueInt64(0);

		//cout << "Found expression *" << endl;
        $$ = new AosExprArith(e1, AosExprArith::eMultiply, e2);
		// if (!$$->getValue(0, 0, (AosValuePtr &)value)) 
		// {
		// 	//cout << "failed to get multiply result: " << $1 << " + " << $3 << endl;
		// } else 
		// {
		// 	//cout << value->toString() << endl;
		// }
	}
    | 
	expr '/' expr
	{ 
		AosExprObj* e1 = $1;
		AosExprObj* e2 = $3;
		// AosValuePtr value = OmnNew AosValueInt64(0);

		//cout << "Found expression /" << endl;
        $$ = new AosExprArith(e1, AosExprArith::eDivide, e2);
		// if (!$$->getValue(0, 0, (AosValuePtr &)value)) 
		// {
		// 	//cout << "failed to get divide result: " << $1 << " + " << $3 << endl;
		// } else 
		// {
		// 	//cout << value->toString() << endl;
		// }
	}
    | 
	expr '%' expr
	{ 
		AosExprObj* e1 = $1;
		AosExprObj* e2 = $3;
		// AosValuePtr value = OmnNew AosValueU64(0);

		//cout << "Found expression %" << endl;
        $$ = new AosExprArith(e1, AosExprArith::eMod, e2);
		// if (!$$->getValue(0, 0, (AosValuePtr &)value)) 
		// {
		// 	//cout << "failed to get mod result: " << $1 << " % " << $3 << endl;
		// } else 
		// {
		// 	//cout << value->toString() << endl;
		// }
	}
    | 
	'-' expr %prec UMINUS 
	{ 
		//$$ = new AosExprUnaryMinus($2);
	}
    | 
	expr LOGIC expr
	{ 
		AosExprObj *e1 = $1;
		AosExprObj *e2 = $3;
		//$$ = new AosExprLogic(e1, (AosExprLogic::Operator)$2, e2);
       // $$ = new AosExprArith(e1, AosExprArith::eConcat, e2);

		// Young, 2014/11/05
		AosExpr* logicExpr = new AosExprLogic(e1, (AosExprLogic::Operator)$2, e2);
		if (e1->getType() == AosExprType::eComparison || e2->getType() == AosExprType::eComparison)
		{
			logicExpr->setExpr(e1, 1);
			logicExpr->setExpr(e2, 2);
		}
		$$ = logicExpr;
	}
    | 
	expr LIKE expr
	{ 
		AosExprObj *e1 = $1;
		AosExprObj *e2 = $3;
		$$ = new AosExprLike(e1, e2, false);
	}
    | 
	expr NOT LIKE expr
	{ 
		AosExprObj *e1 = $1;
		AosExprObj *e2 = $4;
		$$ = new AosExprLike(e1, e2, true);
	}
    | 
	expr XOR expr
	{ 
        //$$ = new AosExprXor($1, $3);
	}
	|
	expr IN '(' expr_list ')'
	{
		$$ = new AosExprIn($1, $4);
	}
    | 
	expr NOT IN '(' expr_list ')'
	{
		$$ = new AosExprNotIn($1, $5);
	}
	|
	expr COMPARISON expr
	{ 
		AosExprObj *e1 = $1;
		AosExprObj *e2 = $3;
		AosExprType::E expr_type = e1->getType();
		if (expr_type == AosExprType::eComparison)
		{
			AosExprBinary* expr = dynamic_cast<AosExprBinary*>(e1);
			AosExprObjPtr e3 = expr->getRHS();

			AosExpr *rhs= new AosExprComparison(e3, (AosExprComparison::Operator)$2, e2);     
			$$ = new AosExprLogic(e1, AosExprLogic::eAnd, rhs);
		}
		else
		{
			AosExpr *expr= new AosExprComparison(e1, (AosExprComparison::Operator)$2, e2);
			expr->setExpr(e1, 1);
			expr->setExpr(e2, 2);
			$$ = expr;
		}
	}
	|
	expr BETWEEN expr AND expr
	{
		AosExpr *rhs= new AosExprComparison($1, AosExprComparison::eLargerEqual, $3);
		AosExpr *lhs= new AosExprComparison($1, AosExprComparison::eLessEqual, $5);     
		$$ = new AosExprLogic(rhs, AosExprLogic::eAnd, lhs);
	}
	|
	expr '|' expr
	{ 
		AosExprObj* e1 = $1;
		AosExprObj* e2 = $3;
		// AosValuePtr value = OmnNew AosValueU32(0);

		//cout << "Found expression |" << endl;
        $$ = new AosExprBitArith(e1, AosExprBitArith::eBitOr, e2);
		// if (!$$->getValue(0, 0, (AosValuePtr &)value)) 
		// {
		// 	//cout << "failed to get bitor result: " << $1 << " | " << $3 << endl;
		// } else 
		// {
		// 	//cout << value->toString() << endl;
		// }
	}
    | 
	expr '&' expr
	{ 
		AosExprObj* e1 = $1;
		AosExprObj* e2 = $3;
		// AosValuePtr value = OmnNew AosValueU32(0);

		//cout << "Found expression &" << endl;
        $$ = new AosExprBitArith(e1, AosExprBitArith::eBitAnd, e2);
		// if (!$$->getValue(0, 0, (AosValuePtr &)value)) 
		// {
		// 	//cout << "failed to get bitand result: " << $1 << " & " << $3 << endl;
		// } else 
		// {
		// 	//cout << value->toString() << endl;
		// }
	}
    | 
	expr '^' expr 
	{ 
		AosExprObj* e1 = $1;
		AosExprObj* e2 = $3;
		// AosValuePtr value = OmnNew AosValueU32(0);

		//cout << "Found expression ^" << endl;
        $$ = new AosExprBitArith(e1, AosExprBitArith::eBitXor, e2);
		// if (!$$->getValue(0, 0, (AosValuePtr &)value)) 
		// {
		// 	//cout << "failed to get bitand result: " << $1 << " ^ " << $3 << endl;
		// } else 
		// {
		// 	//cout << value->toString() << endl;
		// }
	}
	|
	expr CONTAIN STRING
	{
		$$ = new AosExprContain($1, $3);
	}
    | 
	expr SHIFT expr
	{ 
        // $$ = new AosSQLExpr;
		// $$->setOperType( $2==1 ? eOper_LeftShift : eOper_RightShift);
		// $$->setExpr($1, 1);
		// $$->setExpr($3, 2);
	}
    | 
	NOT expr
	{ 
        // $$ = new AosSQLExpr;
		// $$->setOperType(eOper_Not);
		// $$->setExpr($2, 1);
	}
    | 
	'!' expr
	{ 
        // $$ = new AosSQLExpr;
		// $$->setOperType(eOper_Not);
		// $$->setExpr($2, 1);
	}
    | 
	USERVAR ASSIGN expr
	{ 
        // $$ = new AosSQLExpr;
		// $$->setOperType(eOper_Assign);
		// $$->setValue($1);
		// $$->setExpr($3, 1);
	}
	|
	STRING ':' expr
	{
		$$ = new AosExprNameValue($1, $3);
	}
	|
	LEFT '(' expr_list ')'
	{
		$$ = new AosExprGenFunc("left", $3);
	}
	|
	RIGHT '(' expr_list ')'
	{
		$$ = new AosExprGenFunc("right", $3);
	}
	|
	W_WORD '(' ')'
	{
		AosExprList* expr_list = new AosExprList;
		$$ = new AosExprGenFunc($1, expr_list);
	}
	|
	W_WORD '(' '*' ')'
	{
		AosExprList* expr_list = new AosExprList;
		expr_list->push_back(new AosExprFieldName("*"));
		$$ = new AosExprGenFunc($1, expr_list);
	}
	|
	W_WORD '(' expr_list ')'
	{
		OmnString fname = $1;
		if (fname.toLower() == "avg" && $3->size() == 1)
		{
			AosExprObjPtr lhs = OmnNew AosExprGenFunc("sum", (*$3)[0]);
			AosExprObjPtr rhs = OmnNew AosExprGenFunc("count", (*$3)[0]);
			$$ = new AosExprArith(lhs, AosExprArith::eDivide, rhs);
		}
		else
		{
			$$ = new AosExprGenFunc($1, $3);
		}
	}
	|
	W_WORD '(' DISTINCT expr_list ')'
	{
		OmnString fname = $1;
		AosValueRslt v;

		if (fname.toLower() == "count")
		{
			if ($4->size() == 1)
			{
				AosExprObjPtr expr = (*$4)[0];
				bool rslt = expr->getValue(0, 0, v);
				aos_assert_r(rslt, false);
				OmnString rname = v.getStr();
				if (rname == "*") 
				{
					(*$4)[0] = new AosExprFieldName("_rec_count");
				}
			}
			
			fname = "dist_count";
		}
		$$ = new AosExprGenFunc(fname, $4);
	}
	|
	W_WORD '(' ACCUMULATE expr_list ')'
	{
		OmnString fname = $1;
		OmnString newFname = "accu_";
		AosValueRslt v;

		fname = fname.toLower();
		newFname << fname;
		aos_assert_r(fname == "count" ||
					 fname == "sum" ||
					 fname == "max" ||
					 fname == "min",  false);

		aos_assert_r($4->size() == 1, false);
		$$ = new AosExprGenFunc(newFname, $4);
	}
	|
	W_WORD '(' ACCUMULATE '*' ')'
	{
		OmnString fname = $1;
		OmnString newFname = "accu_";
		AosValueRslt v;

		fname = fname.toLower();
		newFname << fname;
		aos_assert_r(fname == "count",  false);

		AosExprList* exprList = new AosExprList;
		AosExprFieldName* expr = new AosExprFieldName("_rec_count");
		exprList->push_back(expr);

		$$ = new AosExprGenFunc(newFname, exprList);
	}
	|
	MAX '(' expr_list ')'
	{
		//max is an exception since there is 
		//a token "MAX", therefore we need to
		//add one explicit line for it
		$$ = new AosExprGenFunc("max", $3);
	}
	|
	'(' expr ')' 
	{
		//cout << "Found expression" << endl;
		$$ = new AosExprBrackets($2);
	}
	|
	'(' expr_list ')'
	{
		$$ = new AosExprBrackets(*($2));
	}
	|
	LEFT_BRACKET expr_list RIGHT_BRACKET
	{
		$$ = new AosExprBrackets(*($2));
	}
	|
	CASE search_case_expr else_expr END
	{
		//cout << "Found case1............" << endl;
		$$ = new AosExprCase($2, 0, $3);
	}                                          
	|
	CASE simple_case_expr else_expr END
	{
		//cout << "Found case2............" << endl;
		$$ = new AosExprCase(0, $2, $3);
	}
	|
	expr IS NOT NULLX
	{
		//cout << "Found is not null ..." << endl;
		$$ = new AosExprIsNotNull($1);
	}
	|
	expr IS NULLX
	{
		//cout << "Found is not null ..." << endl;
		$$ = new AosExprIsNull($1);
	}
	;    

simple_case_expr:
	 expr WHEN expr THEN expr when_case_list
	 {
		AosExprSimpleCase::AosJqlWhenCaseExpr *aa = new AosExprSimpleCase::AosJqlWhenCaseExpr($3, $5);
		$6->appendWhenCase(aa);
	 	//$6->appendPair($3, $5);
		$6->setCaseExpr($1);
		$$ = $6;
	 };

when_case_list:
	 {
	 	$$ = new AosExprSimpleCase();
	 }
	 |
	 when_case_list WHEN expr THEN expr
	 {
	 	//$1->appendPair($3, $5);
		AosExprSimpleCase::AosJqlWhenCaseExpr *aa = new AosExprSimpleCase::AosJqlWhenCaseExpr($3, $5);
		$1->appendWhenCase(aa);
	 };

search_case_expr:
	 when_case_list
	 {
		 $$ = new AosExprSearchCase($1);
	 };

else_expr:
	 /* nill */
	 {
		 $$ = 0;
	 }
	 |
	 ELSE expr
	 {
		 $$ = $2;
	 };

expr_list:
	expr
	{
		$$ = new AosExprList;
		$$->push_back($1);
	}
	|
	expr_list ',' expr
	{
		$$ = $1;
		$$->push_back($3);
	}
	;

stmt_expr_name_values:
	{
		typedef vector<AosExprNameValuePtr> AosExprNameValues;
		$$ = new AosExprNameValues;
	}
	|
	STRING ':' expr
	{
		typedef vector<AosExprNameValuePtr> AosExprNameValues;
		AosExprNameValue *expr = new AosExprNameValue($1, $3);
		$$ = new AosExprNameValues;
		$$->push_back(expr);
	}
	|
	stmt_expr_name_values ','  STRING ':' expr 
	{
		$$ = $1;
		AosExprNameValue *expr = new AosExprNameValue($3, $5);
		$$->push_back(expr);
	}
	;

opt_as_alias: 
  	AS W_WORD 
  	{ 
		$$ = new AosExprFieldName($2);  
		if ($2) free($2);            
  	}
	|
	AS STRING
	{
		$$ = new AosExprString($2);  
		if ($2) free($2);            
	}
	|
	STRING
	{
		$$ = new AosExprString($1);  
		if ($1) free($1);            
	}
	|
	W_WORD              
  	{ 
		$$ = new AosExprFieldName($1);  
		if ($1) free($1);            
  	}
	| 
	/* nil */
  	{
		$$ = NULL;
  	}
	;

opt_where:         
    /* nil */      
    {              
        $$ = 0;    
    }              
    |              
    WHERE expr     
    {              
		$$ = new AosJqlWhere();
		$$->setWhereExpr($2);
    }
	;
