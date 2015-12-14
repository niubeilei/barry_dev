W_WORD:
	NAME 
	{
		$$ = strdup($1);
	}
	|
	INTNUM
	{
		$$ = strdup(yytext);
	}
	|
	U8VAR
	{
		$$ = strdup(yytext);
	} 
	|
	STRING
	{
		$$ = strdup(yytext);
	}
	|
	UNSIGNED_LONG
	{
		$$ = strdup(yytext);
	}
	|
	UNSIGNED_INT
	{
		$$ = strdup(yytext);
	} 
	|
	DOUBLE_NUM
	{
		$$ = strdup(yytext);
	}
	|
	DOCBEGIN
	{
		$$ = strdup(yytext);
	} 
	|
	DOCEND
	{
		$$ = strdup(yytext);
	} 
	|
	XMLDOC
	{
		$$ = strdup(yytext);
	}
	|
	DOC
	{
		$$ = strdup(yytext);
	} 
	|
	LOGIN
	{
		$$ = strdup(yytext);
	} 
	|
	LOGOUT
	{
		$$ = strdup(yytext);
	}
	|
	RETRIEVE
	{
		$$ = strdup(yytext);
	} 
	|
	START
	{
		$$ = strdup(yytext);
	} 
	|
	STOP
	{
		$$ = strdup(yytext);
	} 
	|
	CHECK
	{
		$$ = strdup(yytext);
	} 
	|
	RESTART
	{
		$$ = strdup(yytext);
	}
	|
	OVERRIDE
	{
		$$ = strdup(yytext);
	} 
	|
	BY
	{
		$$ = strdup(yytext);
	} 
	|
	ANDOP
	{
		$$ = strdup(yytext);
	} 
	|
	IGNORE
	{
		$$ = strdup(yytext);
	} 
	|
	LOW_PRIORITY
	{
		$$ = strdup(yytext);
	} 
	|
	QUICK
	{
		$$ = strdup(yytext);
	} 
	|
	SET
	{
		$$ = strdup(yytext);
	} 
	|
	KEY
	{
		$$ = strdup(yytext);
	} 
	|
	WAIT
	{
		$$ = strdup(yytext);
	}
	|
	DOES
	{
		$$ = strdup(yytext);
	}
	|
	ASSIGNMENT
	{
		$$ = strdup(yytext);
	}
	|
	DELAYED
	{
		$$ = strdup(yytext);
	}
	|
	VALUES
	{
		$$ = strdup(yytext);
	}
	|
	COMPLETE
	{
		$$ = strdup(yytext);
	}	
	|
	LEFTSHIFT 
	{
		$$ = strdup(yytext);
	}
	|
	ROLLUP
	{
		$$ = strdup(yytext);
	}
	|
	RIGHTSHIFT
	{
		$$ = strdup(yytext);
	}
	|
	RECORDS
	{
		$$ = strdup(yytext);
	}
	|
	PICKER
	{
		$$ = strdup(yytext);
	}
	|
	PICKERS
	{
		$$ = strdup(yytext);
	}
	|
	JIMOID
	{
		$$ = strdup(yytext);
	}
	|
	AS
	{
		$$ = strdup(yytext);
	}
	|
	END
	{
		$$ = strdup(yytext);
	}
	|
	PROC
	{
		$$ = strdup(yytext);
	}
	|
	PROCS
	{
		$$ = strdup(yytext);
	}
	|
	DEBUG
	{
		$$ = strdup(yytext);
	}
	|
	DATA
	{
		$$ = strdup(yytext);
	}
	|
	SCANNER
	{
		$$ = strdup(yytext);
	}
	|
	SCANNERS
	{
		$$ = strdup(yytext);
	}
	|
	DATASET
	{
		$$ = strdup(yytext);
	}
	|
	DATASETS
	{
		$$ = strdup(yytext);
	}
	|
	DATABASE
	{
		$$ = strdup(yytext);
	}
	|
	DATABASES
	{
		$$ = strdup(yytext);
	}
	|
	DATE
	{
		$$ = strdup(yytext);
	}
	|
	SCHEMA
	{
		$$ = strdup(yytext);
	}
	|
	SCHEMAS
	{
		$$ = strdup(yytext);
	}
	|
	CONNECTOR
	{
		$$ = strdup(yytext);
	}
	|
	CONNECTORS
	{
		$$ = strdup(yytext);
	}
	|
	CUBE
	{
		$$ = strdup(yytext);
	}
	|
	FILTER
	{
		$$ = strdup(yytext);
	}
	|
	FILTERED
	{
		$$ = strdup(yytext);
	}
	|
	ACCEPTED
	{
		$$ = strdup(yytext);
	}
	|
	MATCHED
	{
		$$ = strdup(yytext);
	}
	|
	MISMATCHED
	{
		$$ = strdup(yytext);
	}
	|
	DROPPED
	{
		$$ = strdup(yytext);
	}
	|
	MULTIPLE
	{
		$$ = strdup(yytext);
	}
	|
	DEFAULT
	{
		$$ = strdup(yytext);
	}
	|
	VALUE
	{
		$$ = strdup(yytext);
	}
	|
	SELECTOR
	{
		$$ = strdup(yytext);
	}
	|
	SELECTORS
	{
		$$ = strdup(yytext);
	}
	|
	INNER
	{
		$$ = strdup(yytext);
	}
	|
	FULL
	{
		$$ = strdup(yytext);
	} 
	|
	OUTER
	{
		$$ = strdup(yytext);
	}
	|
	LEFT
	{
		$$ = strdup(yytext);
	}
	|
	RIGHT
	{
		$$ = strdup(yytext);
	}
	|
	EXCLUSIVE
	{
		$$ = strdup(yytext);
	}
	|
	XO
	{
		$$ = strdup(yytext);
	}
	|
	MOD
	{
		$$ = strdup(yytext);
	}
	|
	MODE
	{
		$$ = strdup(yytext);
	}
	|
	BINARY
	{
		$$ = strdup(yytext);
	}
	|
	BOTH
	{
		$$ = strdup(yytext);
	}
	|
	CASE
	{
		$$ = strdup(yytext);
	}
	|
	COMPARISON
	{
		$$ = strdup(yytext);
	}
	|
	LOGIC
	{
		$$ = strdup(yytext);
	}
	|
	CURRENT_DATE
	{
		$$ = strdup(yytext);
	}
	|
	CURRENT_TIME
	{
		$$ = strdup(yytext);
	}
	|
	CURRENT_TIMESTAMP
	{
		$$ = strdup(yytext);
	}
	|
	DAY_HOUR
	{
		$$ = strdup(yytext);
	}
	|
	DAY_MICROSECOND
	{
		$$ = strdup(yytext);
	}
	|
	DAY_MINUTE
	{
		$$ = strdup(yytext);
	}
	|
	DAY_SECOND
	{
		$$ = strdup(yytext);
	}
	|
	FCOUNT
	{
		$$ = strdup(yytext);
	}
	|
	FDATE_ADD
	{
		$$ = strdup(yytext);
	}
	|
	FDATE_SUB
	{
		$$ = strdup(yytext);
	}
	|
	FSUBSTRING
	{
		$$ = strdup(yytext);
	}
	|
	FTRIM
	{
		$$ = strdup(yytext);
	}
	|
	HOUR_MICROSECOND
	{
		$$ = strdup(yytext);
	}
	|
	HOUR_MINUTE
	{
		$$ = strdup(yytext);
	}
	|
	HOUR_SECOND
	{
		$$ = strdup(yytext);
	}
	|
	IN
	{
		$$ = strdup(yytext);
	}
	|
	INTERVAL
	{
		$$ = strdup(yytext);
	}
	|
	IS
	{
		$$ = strdup(yytext);
	}
	|
	LIKE
	{
		$$ = strdup(yytext);
	}
	|
	NULLX
	{
		$$ = strdup(yytext);
	}
	|
	NUMBER
	{
		$$ = strdup(yytext);
	} 
	|
	REGEXP
	{
		$$ = strdup(yytext);
	}
	|
	SHIFT
	{
		$$ = strdup(yytext);
	}
	|
	SOME
	{
		$$ = strdup(yytext);
	}
	|
	TRIM
	{
		$$ = strdup(yytext);
	}
	|
	TRAILING
	{
		$$ = strdup(yytext);
	}
	|
	YEAR
	{
		$$ = strdup(yytext);
	}
	|
	YEAR_MONTH
	{
		$$ = strdup(yytext);
	}
	|
	LEADING
	{
		$$ = strdup(yytext);
	}
	|
	TABLE
	{
		$$ = strdup(yytext);
	} 
	|
	TABLES
	{
		$$ = strdup(yytext);
	}
	|
	RECORD
	{
		$$ = strdup(yytext);
	}
	|
	INDEX
	{
		$$ = strdup(yytext);
	}
	|
	INDEXES
	{
		$$ = strdup(yytext);
	}
	|
	AUTO_INCREMENT
	{
		$$ = strdup(yytext);
	}
	|
	CHECKSUM
	{
		$$ = strdup(yytext);
	}
	|
	ASC
	{
		$$ = strdup(yytext);
	}
	|
	DESC
	{
		$$ = strdup(yytext);
	}
	|
	COMMENTS
	{
		$$ = strdup(yytext);
	}
	|
	CONFIG
	{
		$$ = strdup(yytext);
	} 
	|
	BLOCKSIZE
	{
		$$ = strdup(yytext);
	}
	|
	UNSIGNED
	{
		$$ = strdup(yytext);
	}
	|
	BIT
	{
		$$ = strdup(yytext);
	}
	|
	INT
	{
		$$ = strdup(yytext);
	} 
	|
	STR
	{
		$$ = strdup(yytext);
	} 
	|
	TEXT
	{
		$$ = strdup(yytext);
	}
	|
	ON
	{
		$$ = strdup(yytext);
	}
	|
	OFF
	{
		$$ = strdup(yytext);
	}
	|
	SUPPRESS
	{
		$$ = strdup(yytext);
	}
	|
	DIR
	{
		$$ = strdup(yytext);
	} 
	|
	VARIABLE
	{
		$$ = strdup(yytext);
	} 
	|
	FIELD
	{
		$$ = strdup(yytext);
	} 
	|
	FIELDS
	{
		$$ = strdup(yytext);
	} 
	|
	DELIMITER
	{
		$$ = strdup(yytext);
	} 
	|
	QUALIFIER
	{
		$$ = strdup(yytext);
	} 
	|
	TASKS
	{
		$$ = strdup(yytext);
	}
	|
	JOB
	{
		$$ = strdup(yytext);
	}
	|
	JOBS
	{
		$$ = strdup(yytext);
	}
	|
	TIMES
	{
		$$ = strdup(yytext);
	}
	|
	TIME
	{
		$$ = strdup(yytext);
	}
	|
	PRIORITY
	{
		$$ = strdup(yytext);
	}
	|
	AT
	{
		$$ = strdup(yytext);
	} 
	|
	ACTOR
	{
		$$ = strdup(yytext);
	}
	|
	JAVA
	{
		$$ = strdup(yytext);
	} 
	|
	ACTION
	{
		$$ = strdup(yytext);
	}
	|
	ENGING
	{
		$$ = strdup(yytext);
	}
	|
	INPUT
	{
		$$ = strdup(yytext);
	}
	|
	MAPPING
	{
		$$ = strdup(yytext);
	}
	|
	MAPTASK
	{
		$$ = strdup(yytext);
	}
	|
	OUTPUT
	{
		$$ = strdup(yytext);
	}
	|
	REDUCETASK
	{
		$$ = strdup(yytext);
	}
	|
	SINGLE
	{
		$$ = strdup(yytext);
	}
	|
	TASK
	{
		$$ = strdup(yytext);
	}
	|
	SCHEDULE
	{
		$$ = strdup(yytext);
	}
	|
	OFFSET
	{
		$$ = strdup(yytext);
	}
	|
	EXPRESSION
	{
		$$ = strdup(yytext);
	}
	|
	FORM
	{
		$$ = strdup(yytext);
	}
	|
	FORMATTER
	{
		$$ = strdup(yytext);
	}
	|
	LENGTH
	{
		$$ = strdup(yytext);
	}
	|
	MAX
	{
		$$ = strdup(yytext);
	}
	|
	POS
	{
		$$ = strdup(yytext);
	}
	|
	TO
	{
		$$ = strdup(yytext);
	} 
	|
	PRINT
	{
		$$ = strdup(yytext);
	}
	|
	CODING
	{
		$$ = strdup(yytext);
	}
	|
	COMBINE
	{
		$$ = strdup(yytext);
	}
	|
	COMBINATIONS
	{
		$$ = strdup(yytext);
	}
	|
	FILENAME
	{
		$$ = strdup(yytext);
	}
	|
	ID
	{
		$$ = strdup(yytext);
	}
	|
	SERVER
	{
		$$ = strdup(yytext);
	}
	|
	SERVICE
	{
		$$ = strdup(yytext);
	}
	|
	SERVICES
	{
		$$ = strdup(yytext);
	}
	|
	SIZE
	{
		$$ = strdup(yytext);
	}
	|
	SPLIT
	{
		$$ = strdup(yytext);
	}
	|
	IILNAME
	{
		$$ = strdup(yytext);
	}
	|
	IIL
	{
		$$ = strdup(yytext);
	}
	|
	OPERATOR
	{
		$$ = strdup(yytext);
	}
	|
	REDUCE
	{
		$$ = strdup(yytext);
	}
	|
	STINCT
	{
		$$ = strdup(yytext);
	}
	|
	DISTINCTROW
	{
		$$ = strdup(yytext);
	}
	|
	HIGH_PRIORITY
	{
		$$ = strdup(yytext);
	}
	|
	SQL_BIG_RESULT
	{
		$$ = strdup(yytext);
	}
	|
	SQL_CALC_FOUND_ROWS
	{
		$$ = strdup(yytext);
	}
	|
	SQL_SMALL_RESULT
	{
		$$ = strdup(yytext);
	}
	|
	STRAIGHT_JOIN
	{
		$$ = strdup(yytext);
	}          
	|
	UNIFORM
	{
		$$ = strdup(yytext);
	} 
	|
	PATH
	{
		$$ = strdup(yytext);
	}
	|
	ENCODING
	{
		$$ = strdup(yytext);
	} 
	|
	STATIC
	{
		$$ = strdup(yytext);
	}
	|
	DYNAMIC
	{
		$$ = strdup(yytext);
	}
	|
	ROW
	{
		$$ = strdup(yytext);
	}
	|
	PICKED
	{
		$$ = strdup(yytext);
	}
	|
	BYTE
	{
		$$ = strdup(yytext);
	}
	|
	BYTES
	{
		$$ = strdup(yytext);
	}
	|
	IDFAMILY
	{
		$$ = strdup(yytext);
	}
	|
	LOAD
	{
		$$ = strdup(yytext);
	}
	|
	LOG
	{
		$$ = strdup(yytext);
	}
	|
	IDENTIFIED
	{
		$$ = strdup(yytext);
	} 
	|
	USER
	{
		$$ = strdup(yytext);
	}
	|
	USERS
	{
		$$ = strdup(yytext);
	}
	|
	SOURCE
	{
		$$ = strdup(yytext);
	}
	|
	DUAL
	{
		$$ = strdup(yytext);
	}
	|
	JIMODB
	{
		$$ = strdup(yytext);
	} 
	|
	DIRECTORY
	{
		$$ = strdup(yytext);
	} 
	|
	STATISTICS
	{
		$$ = strdup(yytext);
	} 
	|
	STAT
	{
		$$ = strdup(yytext);
	}
	|
	UNIT
	{
		$$ = strdup(yytext);
	} 
	|
	OUTFILE
	{
		$$ = strdup(yytext);
	}
	|
	CSV
	{
		$$ = strdup(yytext);
	} 
	|
	FIXED
	{
		$$ = strdup(yytext);
	} 
	|
	DIMENSIONS
	{
		$$ = strdup(yytext);
	} 
	|
	HBASE
	{
		$$ = strdup(yytext);
	}
	|
	RAWKEY
	{
		$$ = strdup(yytext);
	}
	|
	ADDRESS
	{
		$$ = strdup(yytext);
	} 
	|
	PORT
	{
		$$ = strdup(yytext);
	} 
	|
	THREAD
	{
		$$ = strdup(yytext);
	}
	|
	MEASURES
	{
		$$ = strdup(yytext);
	} 
	|
	MODEL
	{
		$$ = strdup(yytext);
	} 
	|
	PARSER
	{
		$$ = strdup(yytext);
	} 
	|
	MAP
	{
		$$ = strdup(yytext);
	} 
	|
	DISTRIBUTIONMAP
	{
		$$ = strdup(yytext);
	} 
	|
	DISTRIBUTIONMAPS
	{
		$$ = strdup(yytext);
	}
	|
	MAPS
	{
		$$ = strdup(yytext);
	} 
	|
	VIRTUAL
	{
		$$ = strdup(yytext);
	} 
	|
	NICK
	{
		$$ = strdup(yytext);
	} 
	|
	CONTAIN
	{
		$$ = strdup(yytext);
	} 
	|
	JIMOLOGIC
	{
		$$ = strdup(yytext);
	} 
	|
	CHECKPOINT
	{
		$$ = strdup(yytext);
	} 
	|
	ENDPOINT
	{
		$$ = strdup(yytext);
	} 
	|
	MATRIX
	{
		$$ = strdup(yytext);
	} 
	|
	RESULTS
	{
		$$ = strdup(yytext);
	} 
	|
	EXPORT
	{
		$$ = strdup(yytext);
	} 
	|
	XPATH
	{
		$$ = strdup(yytext);
	}
	|
	BETWEEN
	{
		$$ = strdup(yytext);
	}
	|
	TERMINATED
	{
		$$ = strdup(yytext);
	} 
	|
	ENCLOSED
	{
		$$ = strdup(yytext);
	} 
	|
	ESCAPED
	{
		$$ = strdup(yytext);
	} 
	|
	LINES
	{
		$$ = strdup(yytext);
	}  
	|
	STARTING
	{
		$$ = strdup(yytext);
	} 
	|
	SKIP
	{
		$$ = strdup(yytext);
	} 
	|
	INVALID
	{
		$$ = strdup(yytext);
	} 
	|
	SUB
	{
		$$ = strdup(yytext);
	} 
	|
	SYSTEM_ARGS
	{
		$$ = strdup(yytext);
	} 
	|
	LEFT_BRACKET
	{
		$$ = strdup(yytext);
	}
	|
	RIGHT_BRACKET
	{
		$$ = strdup(yytext);
	}
	|
	CACHE
	{
		$$ = strdup(yytext);
	} 
	|
	CYCLE
	{
		$$ = strdup(yytext);
	} 
	|
	INCREMENT
	{
		$$ = strdup(yytext);
	} 
	|
	MAXVALUE
	{
		$$ = strdup(yytext);
	} 
	|
	MINVALUE
	{
		$$ = strdup(yytext);
	} 
	|
	NOCACHE
	{
		$$ = strdup(yytext);
	} 
	|
	NOCYCLE
	{
		$$ = strdup(yytext);
	} 
	|
	NOMAXVALUE
	{
		$$ = strdup(yytext);
	} 
	|
	NOMINVALUE
	{
		$$ = strdup(yytext);
	} 
	|
	NOORDER
	{
		$$ = strdup(yytext);
	} 
	|
	SEQUENCE
	{
		$$ = strdup(yytext);
	}
	|
	STATUS
	{
		$$ = strdup(yytext);
	} 
	|
	DICTIONARY	
	{
		$$ = strdup(yytext);
	} 
	;
