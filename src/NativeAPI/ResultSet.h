/*
 * ResultSet.h
 *
 *  Created on: Nov 28, 2014
 *      Author: root
 */

#ifndef Aos_NativeAPI_ResultSet_h
#define Aos_NativeAPI_ResultSet_h


typedef struct st_zykie_field {
  char *name;                 /* Name of column */
  char *org_name;             /* Original column name, if an alias */
  char *table;                /* Table of column if column was a field */
  char *org_table;            /* Org table name, if table was an alias */
  char *db;                   /* Database for table */
  char *catalog;	      /* Catalog for table */
  char *def;                  /* Default value (set by mysql_list_fields) */
  unsigned long length;       /* Width of column (create length) */
  unsigned long max_length;   /* Max width for selected set */
  unsigned int name_length;
  unsigned int org_name_length;
  unsigned int table_length;
  unsigned int org_table_length;
  unsigned int db_length;
  unsigned int catalog_length;
  unsigned int def_length;
  unsigned int flags;         /* Div flags */
  unsigned int decimals;      /* Number of decimals in field */
  unsigned int charsetnr;     /* Character set */
  //enum enum_field_types type; /* Type of field. See mysql_com.h for types */
  void *extension;
} ZYKIE_FIELD;

class ResultSet {
  my_ulonglong  row_count;
  ZYKIE_FIELD	*fields;
  //ZYKIE_DATA	*data;
  //ZYKIE_ROWS	*data_cursor;
  unsigned long *lengths;		/* column lengths of current row */
  ZYKIE		*handle;		/* for unbuffered reads */
  //const struct st_zykie_methods *methods;
  ZYKIE_ROW** row;			/* If unbuffered read */
  ZYKIE_ROW	current_row;		/* buffer to current row */
  //MEM_ROOT	field_alloc;
  unsigned int	field_count, current_field;
  my_bool	eof;			/* Used by ZYKIE_fetch_row */
  /* ZYKIE_stmt_close() had to cancel this result */
  my_bool       unbuffered_fetch_cancelled;
  void *extension;


};


#endif /* RESULTSET_H_ */
