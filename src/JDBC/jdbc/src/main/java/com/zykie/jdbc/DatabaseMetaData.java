package com.zykie.jdbc;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.RowIdLifetime;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.List;
import java.util.logging.Logger;
import java.sql.Types;

import com.zykie.jdbc.utils.SQLError;
import com.zykie.jdbc.utils.ZykieLogger;
import com.zykie.jdbc.exceptions.ExceptionInterceptor;
import com.zykie.jdbc.utils.ZykieDefs;

/*
 *This interface is implemented by driver vendors to let users know the capabilities of 
 *a Database Management System (DBMS) in combination with the driver based on JDBCTM 
 *technology ("JDBC driver") that is used with it. Different relational DBMSs often 
 *support different features, implement features in different ways, and use different 
 *data types. In addition, a driver may implement a feature on top of what the DBMS offers. 
 *Information returned by methods in this interface applies to the capabilities of a 
 *particular driver and a particular DBMS working together. Note that as used in this 
 *documentation, the term "database" is used generically to refer to both the driver and DBMS.
 *
 *A user for this interface is commonly a tool that needs to discover how to deal with 
 *the underlying DBMS. This is especially true for applications that are intended to be 
 *used with more than one DBMS. For example, a tool might use the method getTypeInfo to 
 *find out what data types can be used in a CREATE TABLE statement. Or a user might call 
 *the method supportsCorrelatedSubqueries to see if it is possible to use a correlated
 * subquery or supportsBatchUpdates to see if it is possible to use batch updates.
 *
 *Some DatabaseMetaData methods return lists of information in the form of ResultSet objects.
 * Regular ResultSet methods, such as getString and getInt, can be used to retrieve the data 
 * from these ResultSet objects. If a given form of metadata is not available, an empty 
 * ResultSet will be returned. Additional columns beyond the columns defined to be returned 
 * by the ResultSet object for a given method can be defined by the JDBC driver vendor and 
 * must be accessed by their column label.
 *
 *Some DatabaseMetaData methods take arguments that are String patterns. These arguments 
 *all have names such as fooPattern. Within a pattern String, "%" means match any substring
 * of 0 or more characters, and "_" means match any one character. Only metadata entries 
 * matching the search pattern are returned. If a search pattern argument is set to null, 
 * that argument's criterion will be dropped from the search. 
 */
public class DatabaseMetaData implements java.sql.DatabaseMetaData {

	private com.zykie.jdbc.Connection conn;
	private String database;
	private boolean hasParametersView = false;
	private static final Logger log = ZykieLogger
			.getLogger("com.zykie.jdbc.Connection");

	@Override
	public <T> T unwrap(Class<T> iface) throws SQLException {
		throw SQLError.notImplemented();
	}

	@Override
	public boolean isWrapperFor(Class<?> iface) throws SQLException {
		throw SQLError.notImplemented();
	}

	/*
	 * Normally, database metadata is saved in a system level database. And
	 * relative information is saved in xxxParameters files for some databases
	 */
	public DatabaseMetaData(Connection conn, String database)
			throws SQLException {
		this.conn = (com.zykie.jdbc.Connection) conn;
		this.database = database;
		ResultSet rs = null;

		try {
			if (database != null) {
				log.info("get metadata");
				rs = getTables(database, null, "PARAMETERS",
						new String[0]);
				this.hasParametersView = rs.next();
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			if (rs != null) {
				rs.close();
			}
		}
	}

	@Override
	public boolean allProceduresAreCallable() throws SQLException {
		throw SQLError.notImplemented();
	}

	@Override
	public boolean allTablesAreSelectable() throws SQLException {
		throw SQLError.notImplemented();
	}

	@Override
	public String getURL() throws SQLException {
		System.out.println("conn.getUrl()="+conn.getUrl());
		return conn.getUrl();
	}

	@Override
	public String getUserName() throws SQLException {
		throw SQLError.notImplemented();
	}

	@Override
	public boolean isReadOnly() throws SQLException {
		throw SQLError.notImplemented();
	}

	@Override
	public boolean nullsAreSortedHigh() throws SQLException {
		throw SQLError.notImplemented();
	}

	@Override
	public boolean nullsAreSortedLow() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean nullsAreSortedAtStart() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean nullsAreSortedAtEnd() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public String getDatabaseProductName() throws SQLException {
		return "Zykie";
	}

	@Override
	public String getDatabaseProductVersion() throws SQLException {
		return "1.0";
	}

	@Override
	public String getDriverName() throws SQLException {
		return "Zykie Driver";
	}

	@Override
	public String getDriverVersion() throws SQLException {
		return "1.0";
	}

	@Override
	public int getDriverMajorVersion() {
		return 1;
	}

	@Override
	public int getDriverMinorVersion() {
		return 0;
	}

	@Override
	public boolean usesLocalFiles() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean usesLocalFilePerTable() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsMixedCaseIdentifiers() throws SQLException {
		//throw SQLError.notImplemented();
		return false;
	}

	@Override
	public boolean storesUpperCaseIdentifiers() throws SQLException {
		//throw SQLError.notImplemented();
		return false;
	}

	@Override
	public boolean storesLowerCaseIdentifiers() throws SQLException {
		//throw SQLError.notImplemented();
		return false;
	}

	@Override
	public boolean storesMixedCaseIdentifiers() throws SQLException {
		//throw SQLError.notImplemented();
		return false;
	}

	@Override
	public boolean supportsMixedCaseQuotedIdentifiers() throws SQLException {
		//throw SQLError.notImplemented();
		return false;
	}

	@Override
	public boolean storesUpperCaseQuotedIdentifiers() throws SQLException {
		//throw SQLError.notImplemented();
		return false;
	}

	@Override
	public boolean storesLowerCaseQuotedIdentifiers() throws SQLException {
		//throw SQLError.notImplemented();
		return false;
	}

	@Override
	public boolean storesMixedCaseQuotedIdentifiers() throws SQLException {
		//throw SQLError.notImplemented();
		return false;
	}

	@Override
	public String getIdentifierQuoteString() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public String getSQLKeywords() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public String getNumericFunctions() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public String getStringFunctions() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public String getSystemFunctions() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public String getTimeDateFunctions() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public String getSearchStringEscape() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public String getExtraNameCharacters() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsAlterTableWithAddColumn() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsAlterTableWithDropColumn() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsColumnAliasing() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean nullPlusNonNullIsNull() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsConvert() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsConvert(int fromType, int toType)
			throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsTableCorrelationNames() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsDifferentTableCorrelationNames() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsExpressionsInOrderBy() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsOrderByUnrelated() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsGroupBy() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsGroupByUnrelated() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsGroupByBeyondSelect() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsLikeEscapeClause() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsMultipleResultSets() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsMultipleTransactions() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsNonNullableColumns() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsMinimumSQLGrammar() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsCoreSQLGrammar() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsExtendedSQLGrammar() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsANSI92EntryLevelSQL() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsANSI92IntermediateSQL() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsANSI92FullSQL() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsIntegrityEnhancementFacility() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsOuterJoins() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsFullOuterJoins() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsLimitedOuterJoins() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public String getSchemaTerm() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public String getProcedureTerm() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public String getCatalogTerm() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean isCatalogAtStart() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	/*
	 * Retrieves the String that this database uses as the separator between a
	 * catalog and table name. (non-Javadoc)
	 * 
	 * @see java.sql.DatabaseMetaData#getCatalogSeparator()
	 */
	@Override
	public String getCatalogSeparator() throws SQLException {
		return ".";
	}

	@Override
	public boolean supportsSchemasInDataManipulation() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsSchemasInProcedureCalls() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsSchemasInTableDefinitions() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsSchemasInIndexDefinitions() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsSchemasInPrivilegeDefinitions() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsCatalogsInDataManipulation() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsCatalogsInProcedureCalls() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsCatalogsInTableDefinitions() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsCatalogsInIndexDefinitions() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsCatalogsInPrivilegeDefinitions() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsPositionedDelete() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsPositionedUpdate() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsSelectForUpdate() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsStoredProcedures() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsSubqueriesInComparisons() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsSubqueriesInExists() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsSubqueriesInIns() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsSubqueriesInQuantifieds() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsCorrelatedSubqueries() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsUnion() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsUnionAll() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsOpenCursorsAcrossCommit() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsOpenCursorsAcrossRollback() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsOpenStatementsAcrossCommit() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsOpenStatementsAcrossRollback() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public int getMaxBinaryLiteralLength() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public int getMaxCharLiteralLength() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public int getMaxColumnNameLength() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public int getMaxColumnsInGroupBy() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public int getMaxColumnsInIndex() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public int getMaxColumnsInOrderBy() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public int getMaxColumnsInSelect() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public int getMaxColumnsInTable() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	/**
	 * How many active connections can we have at a time to this database?
	 */
	@Override
	public int getMaxConnections() throws SQLException {
		return 0;
	}

	@Override
	public int getMaxCursorNameLength() throws SQLException {
		return 64;
	}

	@Override
	public int getMaxIndexLength() throws SQLException {
		return 256;
	}

	@Override
	public int getMaxSchemaNameLength() throws SQLException {
		return 0;
	}

	@Override
	public int getMaxProcedureNameLength() throws SQLException {
		return 0;
	}

	@Override
	public int getMaxCatalogNameLength() throws SQLException {
		return 64;
	}

	@Override
	public int getMaxRowSize() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean doesMaxRowSizeIncludeBlobs() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public int getMaxStatementLength() throws SQLException {
		return 1024;
	}

	/**
	 * How many active statements can we have open at one time to this database?
	 * 
	 * (non-Javadoc)
	 * 
	 * @see java.sql.DatabaseMetaData#getMaxStatements()
	 */
	@Override
	public int getMaxStatements() throws SQLException {
		return 0;
	}

	@Override
	public int getMaxTableNameLength() throws SQLException {
		return 64;
	}

	/*
	 * What's the maximum number of tables in a SELECT?
	 * 
	 * (non-Javadoc)
	 * 
	 * @see java.sql.DatabaseMetaData#getMaxTablesInSelect()
	 */
	@Override
	public int getMaxTablesInSelect() throws SQLException {
		return 256;
	}

	@Override
	public int getMaxUserNameLength() throws SQLException {
		return 16;
	}

	@Override
	public int getDefaultTransactionIsolation() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsTransactions() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsTransactionIsolationLevel(int level)
			throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsDataDefinitionAndDataManipulationTransactions()
			throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsDataManipulationTransactionsOnly()
			throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean dataDefinitionCausesTransactionCommit() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean dataDefinitionIgnoredInTransactions() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public ResultSet getProcedures(String catalog, String schemaPattern,
			String procedureNamePattern) throws SQLException {
		java.sql.Statement stmt = null;
		com.zykie.jdbc.ResultSet rs = null;
		log.info("get procedures");
		try {
			rs = new com.zykie.jdbc.ResultSet(this.conn, null);
			rs.setMetaData(new com.zykie.jdbc.ResultSetMetaData(
					createProcedureFields(), false, false,
					getExceptionInterceptor()));

		} catch (Exception e) {
			e.printStackTrace();
		}

		return rs;
	}

	@Override
	public ResultSet getProcedureColumns(String catalog, String schemaPattern,
			String procedureNamePattern, String columnNamePattern)
			throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	/**
	 * Get a description of tables available in a catalog. Table, view, system
	 * tables, etc all belong to table
	 * <P>
	 * Only table descriptions matching the catalog, schema, table name and type
	 * criteria are returned. They are ordered by TABLE_TYPE, TABLE_SCHEM and
	 * TABLE_NAME.
	 * </p>
	 * <P>
	 * Each table description has the following columns:
	 * <OL>
	 * <li><B>TABLE_CAT</B> String => table catalog (may be null)</li>
	 * <li><B>TABLE_SCHEM</B> String => table schema (may be null)</li>
	 * <li><B>TABLE_NAME</B> String => table name</li>
	 * <li><B>TABLE_TYPE</B> String => table type. Typical types are "TABLE",
	 * "VIEW", "SYSTEM TABLE", "GLOBAL TEMPORARY", "LOCAL TEMPORARY", "ALIAS",
	 * "SYNONYM".</li>
	 * <li><B>REMARKS</B> String => explanatory comment on the table</li>
	 * </ol>
	 * </p>
	 * <P>
	 * <B>Note:</B> Some databases may not return information for all tables.
	 * </p>
	 * 
	 * @param catalog
	 *            a catalog name; "" retrieves those without a catalog
	 * @param schemaPattern
	 *            a schema name pattern; "" retrieves those without a schema
	 * @param tableNamePattern
	 *            a table name pattern
	 * @param types
	 *            a list of table types to include; null returns all types
	 * @return ResultSet each row is a table description
	 * @throws SQLException
	 *             DOCUMENT ME!
	 * @see #getSearchStringEscape
	 */
	public ResultSet getTables(String catalog, String schemaPattern,
			String tableNamePattern, String[] types) throws SQLException {
		System.out.println("调用了ffffffffffffffffffffffffffffffffffffff");
		System.out.println("catalog="+catalog+"\nschemaPattern="+schemaPattern+
				"\ntableNamePattern="+tableNamePattern+"\ntypes="+types+"gggggggggggggggggggggggggg");
		String sql = "";
		Statement s;
		String[] row;
		ResultSet rs = null;
		ResultSet rs1 = null;

		if (catalog == null) 
			return null;

		if (tableNamePattern == null) {
			throw SQLError.createSQLException(
					"Table name pattern can not be NULL or empty.",
					SQLError.SQL_STATE_ILLEGAL_ARGUMENT,
					getExceptionInterceptor());
		}

		sql = "use " + catalog + ";";
		s = new Statement(this.conn, this.database);
		rs1 = s.executeQuery(sql);
		
		sql = "show tables;";
		s = new Statement(this.conn, this.database);
		rs1 = s.executeQuery(sql);

		/**
		 * Build rs from rs1 with expanding fields
		 */
		rs = new com.zykie.jdbc.ResultSet(this.conn, null);

		// build fields
		Field[] fields = createTableFields();
		com.zykie.jdbc.ResultSetMetaData mData = new ResultSetMetaData(fields,
				false, false, getExceptionInterceptor());
		((com.zykie.jdbc.ResultSet) rs).setMetaData(mData);

		// build records to follow SQL table description fields
		while (rs1.next()) {
			//System.out.println("有值-------------------------------------------------------");
			row = new String[10];
			row[0] = catalog;
			row[1] = null;
			row[2] = rs1.getString(1);
			row[3] = "TABLE"; // need to change if more types supported
			row[4] = null;
			row[5] = null;
			row[6] = null;
			row[7] = null;
			row[8] = null;
			row[9] = null;
log.info("get metadata info : " + catalog + ", name:"+rs1.getString(1));
			((com.zykie.jdbc.ResultSet) rs).addRecord(row);
		}

		return rs;
	}

	private ExceptionInterceptor getExceptionInterceptor() {
		return this.conn.getExceptionInterceptor();
	}

	@Override
	public ResultSet getSchemas() throws SQLException {
		return getSchemas("TABLE_CATALOG", "TABLE_SCHEM");
	}

	/*
	 * Retrieves the catalog names available in this database. The results are
	 * ordered by catalog name.
	 * 
	 * (non-Javadoc)
	 * 
	 * @see java.sql.DatabaseMetaData#getCatalogs()
	 */
	@Override
	public ResultSet getCatalogs() throws SQLException {
		String sql = "";
		Statement s;
		String[] row;
		ResultSet rs = null;
		ResultSet rs1 = null;

		sql = "show databases;";
		s = new Statement(this.conn, this.database);
		rs1 = s.executeQuery(sql);

		/**
		 * Build rs from rs1 with expanding fields
		 */
		rs = new com.zykie.jdbc.ResultSet(this.conn, null);

		// build fields
		Field[] fields = new Field[1];
		fields[0] = new Field("", "TABLE_CAT", Types.VARCHAR, 32);
		com.zykie.jdbc.ResultSetMetaData mData = new ResultSetMetaData(fields,
				false, false, getExceptionInterceptor());
		((com.zykie.jdbc.ResultSet) rs).setMetaData(mData);

		// build records to follow SQL table description fields
		while (rs1.next()) {
			row = new String[1];
			row[0] = rs1.getString("zky_database_name");
			log.info("db name : " + row[0]);
			((com.zykie.jdbc.ResultSet) rs).addRecord(row);
		}

		return rs;
	}

	/*
	 * Retrieves the table types available in this database. The results are
	 * ordered by table type.
	 * 
	 * The table type is:
	 * 
	 * TABLE_TYPE String => table type. Typical types are "TABLE", "VIEW",
	 * "SYSTEM TABLE", "GLOBAL TEMPORARY", "LOCAL TEMPORARY", "ALIAS",
	 * "SYNONYM".
	 * 
	 * Returns: a ResultSet object in which each row has a single String column
	 * that is a table type
	 * 
	 * (non-Javadoc)
	 * 
	 * @see java.sql.DatabaseMetaData#getTableTypes()
	 */
	@Override
	public ResultSet getTableTypes() throws SQLException {
		
		Exception exc = new Exception("this is getTableTypes");
		exc.printStackTrace();
		
		com.zykie.jdbc.ResultSet rs = new com.zykie.jdbc.ResultSet(this.conn,
				null);
		Field[] fields;
		String[][] rows;
		
		// setup the fields information
		fields = new Field[] { new Field("", "TABLE_TYPE", Types.VARCHAR, 256) };
		rs.setMetaData(new com.zykie.jdbc.ResultSetMetaData(fields, false,
				false, getExceptionInterceptor()));

		rows = new String[1][1];
		rows[0][0] = "TABLE";
//		 rows[1][0] = "LOCAL TEMPORARY";
		// rows[2][0] = "SYSTEM TABLE";
		// rows[3][0] = "SYSTEM VIEW";
		// rows[4][0] = "VIEW";
		log.info("getTableTypes :  " + rows.length);
		for (int i = 0; i < rows.length; i++)
			rs.addRecord(rows[i]);

		return rs;
	}

	/*
	 * 
	 * Retrieves a description of table columns available in the specified
	 * catalog.
	 * 
	 * Only column descriptions matching the catalog, schema, table and column
	 * name criteria are returned. They are ordered by TABLE_CAT,TABLE_SCHEM,
	 * TABLE_NAME, and ORDINAL_POSITION.
	 * 
	 * Each column description has the following columns:
	 * 
	 * TABLE_CAT String => table catalog (may be null) TABLE_SCHEM String =>
	 * table schema (may be null) TABLE_NAME String => table name COLUMN_NAME
	 * String => column name DATA_TYPE int => SQL type from java.sql.Types
	 * TYPE_NAME String => Data source dependent type name, for a UDT the type
	 * name is fully qualified COLUMN_SIZE int => column size. BUFFER_LENGTH is
	 * not used. DECIMAL_DIGITS int => the number of fractional digits. Null is
	 * returned for data types where DECIMAL_DIGITS is not applicable.
	 * NUM_PREC_RADIX int => Radix (typically either 10 or 2) NULLABLE int => is
	 * NULL allowed. columnNoNulls - might not allow NULL values columnNullable
	 * - definitely allows NULL values columnNullableUnknown - nullability
	 * unknown REMARKS String => comment describing column (may be null)
	 * COLUMN_DEF String => default value for the column, which should be
	 * interpreted as a string when the value is enclosed in single quotes (may
	 * be null) SQL_DATA_TYPE int => unused SQL_DATETIME_SUB int => unused
	 * CHAR_OCTET_LENGTH int => for char types the maximum number of bytes in
	 * the column ORDINAL_POSITION int => index of column in table (starting at
	 * 1) IS_NULLABLE String => ISO rules are used to determine the nullability
	 * for a column. YES --- if the parameter can include NULLs NO --- if the
	 * parameter cannot include NULLs empty string --- if the nullability for
	 * the parameter is unknown SCOPE_CATLOG String => catalog of table that is
	 * the scope of a reference attribute (null if DATA_TYPE isn't REF)
	 * SCOPE_SCHEMA String => schema of table that is the scope of a reference
	 * attribute (null if the DATA_TYPE isn't REF) SCOPE_TABLE String => table
	 * name that this the scope of a reference attribure (null if the DATA_TYPE
	 * isn't REF) SOURCE_DATA_TYPE short => source type of a distinct type or
	 * user-generated Ref type, SQL type from java.sql.Types (null if DATA_TYPE
	 * isn't DISTINCT or user-generated REF) IS_AUTOINCREMENT String =>
	 * Indicates whether this column is auto incremented YES --- if the column
	 * is auto incremented NO --- if the column is not auto incremented empty
	 * string --- if it cannot be determined whether the column is auto
	 * incremented parameter is unknown
	 * 
	 * The COLUMN_SIZE column the specified column size for the given column.
	 * For numeric data, this is the maximum precision. For character data, this
	 * is the length in characters. For datetime datatypes, this is the length
	 * in characters of the String representation (assuming the maximum allowed
	 * precision of the fractional seconds component). For binary data, this is
	 * the length in bytes. For the ROWID datatype, this is the length in bytes.
	 * Null is returned for data types where the column size is not applicable.
	 * 
	 * (non-Javadoc)
	 * 
	 * @see java.sql.DatabaseMetaData#getColumns(java.lang.String,
	 * java.lang.String, java.lang.String, java.lang.String)
	 */
	@Override
	public ResultSet getColumns(String catalog, String schemaPattern,
			String tableNamePattern, String columnNamePattern)
			throws SQLException {

		String sql = "";
		Statement s;
		String[] row;
		ResultSet rs = null;
		ResultSet rs1 = null;

		if (catalog == null) {
			catalog = this.database;
		}

		// use the catalog/database firstly
		sql = "use " + catalog + ";";
		s = new Statement(this.conn, catalog);
		rs1 = s.executeQuery(sql);
		
		// assume that tableNamePattern is a table name
		sql = "describe table " + tableNamePattern + ";";
		s = new Statement(this.conn, catalog);
		rs1 = s.executeQuery(sql);

		/**
		 * Build rs from rs1 with expanding fields
		 */
		rs = new com.zykie.jdbc.ResultSet(this.conn, null);

		// build fields
		Field[] fields = createColumnFields();
		com.zykie.jdbc.ResultSetMetaData mData = new ResultSetMetaData(fields,
				false, false, getExceptionInterceptor());
		((com.zykie.jdbc.ResultSet) rs).setMetaData(mData);

		int i = 0;
		while (rs1.next()) {
			i++;
			row = new String[24];

			row[0] = s2b(catalog); // TABLE_CAT
			row[1] = null; // TABLE_SCHEM (No schemas in Zykie)

			row[2] = s2b(tableNamePattern); // TABLE_NAME
			row[3] = rs1.getString(1);

			row[4] = String
					.valueOf(ZykieDefs.zykieToJavaType(rs1.getString(2)));

			// DATA_TYPE (jdbc)
			row[5] = rs1.getString(2); // TYPE_NAME
			row[6] = rs1.getString(3);
			row[7] = Integer.toString(0);
			row[8] = null;
			row[9] = Integer.toString(2);
			row[10] = Integer.toString(2);
			row[11] = null;
			row[12] = null;
			row[13] = Integer.toString(0);
			row[14] = Integer.toString(0);
			row[15] = row[6]; // CHAR_OCTET_LENGTH
			row[16] = Integer.toString(i);
			row[17] = "";

			// We don't support REF or DISTINCT types
			row[18] = null;
			row[19] = null;
			row[20] = null;
			row[21] = null;
			row[22] = "";

			((com.zykie.jdbc.ResultSet) rs).addRecord(row);
		}

		return rs;

	}

	@Override
	public ResultSet getColumnPrivileges(String catalog, String schema,
			String table, String columnNamePattern) throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public ResultSet getTablePrivileges(String catalog, String schemaPattern,
			String tableNamePattern) throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public ResultSet getBestRowIdentifier(String catalog, String schema,
			String table, int scope, boolean nullable) throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public ResultSet getVersionColumns(String catalog, String schema,
			String table) throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	/*
	 * Retrieves a description of the given table's primary key columns. They
	 * are ordered by COLUMN_NAME.
	 * 
	 * Each primary key column description has the following columns:
	 * 
	 * TABLE_CAT String => table catalog (may be null) TABLE_SCHEM String =>
	 * table schema (may be null) TABLE_NAME String => table name COLUMN_NAME
	 * String => column name KEY_SEQ short => sequence number within primary
	 * key( a value of 1 represents the first column of the primary key, a value
	 * of 2 would represent the second column within the primary key). PK_NAME
	 * String => primary key name (may be null)
	 * 
	 * (non-Javadoc)
	 * 
	 * @see java.sql.DatabaseMetaData#getPrimaryKeys(java.lang.String,
	 * java.lang.String, java.lang.String)
	 */
	@Override
	public ResultSet getPrimaryKeys(String catalog, String schema, String table)
			throws SQLException {

		String sql = "";
		Statement s;
		String[] row;
		ResultSet rs = null;
		ResultSet rs1 = null;

		if (catalog == null) {
			catalog = this.database;
		}

		if (table == null) {
			throw SQLError.createSQLException(
					"Table name can not be NULL or empty.",
					SQLError.SQL_STATE_ILLEGAL_ARGUMENT,
					getExceptionInterceptor());
		}

		// we need to add "show keys from table" in the future
		// right now use "describe table" as a workaround
		// sql = "SHOW KEYS FROM " + table + ";";
		sql = "describe table " + table + ";";
		s = new Statement(this.conn, this.database);
		rs1 = s.executeQuery(sql);

		/**
		 * Build rs from rs1 with expanding fields
		 */
		rs = new com.zykie.jdbc.ResultSet(this.conn, null);

		// build fields
		Field[] fields = createPrimaryKeyFields();
		com.zykie.jdbc.ResultSetMetaData mData = new ResultSetMetaData(fields,
				false, false, getExceptionInterceptor());
		((com.zykie.jdbc.ResultSet) rs).setMetaData(mData);

		int i = 0;
		while (rs.next()) {
			i++;
			row = new String[6];
			row[0] = this.database;
			row[1] = null;
			row[2] = table;

			// String columnName = rs1.getString("Column_name");
			row[3] = rs1.getString(i); // use all the columns as key for now
			row[4] = String.valueOf(i);
			row[5] = rs1.getString(1); // use the first column as the primary
										// key name for now
			

			((com.zykie.jdbc.ResultSet) rs).addRecord(row);
		}

		return rs;
	}

	@Override
	public ResultSet getImportedKeys(String catalog, String schema, String table)
			throws SQLException {
		// TODO Auto-generated method stub
		//throw SQLError.notImplemented();
		return null;
	}

	@Override
	public ResultSet getExportedKeys(String catalog, String schema, String table)
			throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public ResultSet getCrossReference(String parentCatalog,
			String parentSchema, String parentTable, String foreignCatalog,
			String foreignSchema, String foreignTable) throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public ResultSet getTypeInfo() throws SQLException {
		
		
		Field[] fields = new Field[18];
		String[] row = null;
		com.zykie.jdbc.ResultSet rs = new com.zykie.jdbc.ResultSet(this.conn,
				null);

		fields[0] = new Field("", "TYPE_NAME", Types.CHAR, 32);
		fields[1] = new Field("", "DATA_TYPE", Types.INTEGER, 5);
		fields[2] = new Field("", "PRECISION", Types.INTEGER, 10);
		fields[3] = new Field("", "LITERAL_PREFIX", Types.CHAR, 4);
		fields[4] = new Field("", "LITERAL_SUFFIX", Types.CHAR, 4);
		fields[5] = new Field("", "CREATE_PARAMS", Types.CHAR, 32);
		fields[6] = new Field("", "NULLABLE", Types.SMALLINT, 5);
		fields[7] = new Field("", "CASE_SENSITIVE", Types.BOOLEAN, 3);
		fields[8] = new Field("", "SEARCHABLE", Types.SMALLINT, 3);
		fields[9] = new Field("", "UNSIGNED_ATTRIBUTE", Types.BOOLEAN, 3);
		fields[10] = new Field("", "FIXED_PREC_SCALE", Types.BOOLEAN, 3);
		fields[11] = new Field("", "AUTO_INCREMENT", Types.BOOLEAN, 3);
		fields[12] = new Field("", "LOCAL_TYPE_NAME", Types.CHAR, 32);
		fields[13] = new Field("", "MINIMUM_SCALE", Types.SMALLINT, 5);
		fields[14] = new Field("", "MAXIMUM_SCALE", Types.SMALLINT, 5);
		fields[15] = new Field("", "SQL_DATA_TYPE", Types.INTEGER, 10);
		fields[16] = new Field("", "SQL_DATETIME_SUB", Types.INTEGER, 10);
		fields[17] = new Field("", "NUM_PREC_RADIX", Types.INTEGER, 10);

		// create resultSetMetaData based on fields
		com.zykie.jdbc.ResultSetMetaData mData = new ResultSetMetaData(fields,
				false, false, getExceptionInterceptor());
		rs.setMetaData(mData);

		/*
		 * Type: BIT (silently converted to TINYINT(1)) JDBC Type: BIT
		 */
		row = new String[18];
		row[0] = s2b("BIT");
		row[1] = Integer.toString(java.sql.Types.BIT);

		// JDBC Data type
		row[2] = s2b("1"); // Precision
		row[3] = s2b(""); // Literal Prefix
		row[4] = s2b(""); // Literal Suffix
		row[5] = s2b(""); // Create Params
		row[6] = Integer.toString(java.sql.DatabaseMetaData.typeNullable);

		// Nullable
		row[7] = s2b("true"); // Case Sensitive
		row[8] = Integer.toString(java.sql.DatabaseMetaData.typeSearchable);

		// Searchable
		row[9] = s2b("false"); // Unsignable
		row[10] = s2b("false"); // Fixed Prec Scale
		row[11] = s2b("false"); // Auto Increment
		row[12] = s2b("BIT"); // Locale Type Name
		row[13] = s2b("0"); // Minimum Scale
		row[14] = s2b("0"); // Maximum Scale
		row[15] = s2b("0"); // SQL Data Type (not used)
		row[16] = s2b("0"); // SQL DATETIME SUB (not used)
		row[17] = s2b("10"); // NUM_PREC_RADIX (2 or 10)

		rs.addRecord(row);

		/*
		 * Zykie Type: CHAR JDBC Type: CHAR
		 */
		row = new String[18];
		row[0] = s2b("CHAR");
		row[1] = Integer.toString(java.sql.Types.CHAR);

		// JDBC Data type
		row[2] = s2b("255"); // Precision
		row[3] = s2b("'"); // Literal Prefix
		row[4] = s2b("'"); // Literal Suffix
		row[5] = s2b("(M)"); // Create Params
		row[6] = Integer.toString(java.sql.DatabaseMetaData.typeNullable);

		// Nullable
		row[7] = s2b("false"); // Case Sensitive
		row[8] = Integer.toString(java.sql.DatabaseMetaData.typeSearchable);

		// Searchable
		row[9] = s2b("false"); // Unsignable
		row[10] = s2b("false"); // Fixed Prec Scale
		row[11] = s2b("false"); // Auto Increment
		row[12] = s2b("CHAR"); // Locale Type Name
		row[13] = s2b("0"); // Minimum Scale
		row[14] = s2b("0"); // Maximum Scale
		row[15] = s2b("0"); // SQL Data Type (not used)
		row[16] = s2b("0"); // SQL DATETIME SUB (not used)
		row[17] = s2b("10"); // NUM_PREC_RADIX (2 or 10)

		rs.addRecord(row);
		
		/*
		 * Zykie Type: CHAR JDBC Type: INT
		 */
		row = new String[18];
		row[0] = s2b("INT");
		row[1] = Integer.toString(java.sql.Types.INTEGER);

		// JDBC Data type
		row[2] = s2b("2147483647"); // Precision
		row[3] = s2b(""); // Literal Prefix
		row[4] = s2b(""); // Literal Suffix
		row[5] = s2b(""); // Create Params
		row[6] = Integer.toString(java.sql.DatabaseMetaData.typeNullable);

		// Nullable
		row[7] = s2b("true"); // Case Sensitive
		row[8] = Integer.toString(java.sql.DatabaseMetaData.typeSearchable);

		// Searchable
		row[9] = s2b("false"); // Unsignable
		row[10] = s2b("false"); // Fixed Prec Scale
		row[11] = s2b("false"); // Auto Increment
		row[12] = s2b("INT"); // Locale Type Name
		row[13] = s2b("0"); // Minimum Scale
		row[14] = s2b("0"); // Maximum Scale
		row[15] = s2b("0"); // SQL Data Type (not used)
		row[16] = s2b("0"); // SQL DATETIME SUB (not used)
		row[17] = s2b("10"); // NUM_PREC_RADIX (2 or 10)

		rs.addRecord(row);
		
		/*
		 * Zykie Type: CHAR JDBC Type: VARCHAR
		 */
		row = new String[18];
		row[0] = s2b("VARCHAR");
		row[1] = Integer.toString(java.sql.Types.VARCHAR);

		// JDBC Data type
		row[2] = s2b("0"); // Precision
		row[3] = s2b("'"); // Literal Prefix
		row[4] = s2b("'"); // Literal Suffix
		row[5] = s2b("(M)"); // Create Params
		row[6] = Integer.toString(java.sql.DatabaseMetaData.typeNullable);

		// Nullable
		row[7] = s2b("false"); // Case Sensitive
		row[8] = Integer.toString(java.sql.DatabaseMetaData.typeSearchable);

		// Searchable
		row[9] = s2b("false"); // Unsignable
		row[10] = s2b("false"); // Fixed Prec Scale
		row[11] = s2b("false"); // Auto Increment
		row[12] = s2b("VARCHAR"); // Locale Type Name
		row[13] = s2b("0"); // Minimum Scale
		row[14] = s2b("0"); // Maximum Scale
		row[15] = s2b("0"); // SQL Data Type (not used)
		row[16] = s2b("0"); // SQL DATETIME SUB (not used)
		row[17] = s2b("10"); // NUM_PREC_RADIX (2 or 10)

		rs.addRecord(row);
		
		return rs;
	}

	@Override
	public ResultSet getIndexInfo(String catalog, String schema, String table,
			boolean unique, boolean approximate) throws SQLException {
		/*
		 * modified by lina
		 */
		//throw SQLError.notImplemented();
		return null;
	}

	@Override
	public boolean supportsResultSetType(int type) throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsResultSetConcurrency(int type, int concurrency)
			throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean ownUpdatesAreVisible(int type) throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean ownDeletesAreVisible(int type) throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean ownInsertsAreVisible(int type) throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean othersUpdatesAreVisible(int type) throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean othersDeletesAreVisible(int type) throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean othersInsertsAreVisible(int type) throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean updatesAreDetected(int type) throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean deletesAreDetected(int type) throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean insertsAreDetected(int type) throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsBatchUpdates() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public ResultSet getUDTs(String catalog, String schemaPattern,
			String typeNamePattern, int[] types) throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public Connection getConnection() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsSavepoints() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsNamedParameters() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsMultipleOpenResults() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsGetGeneratedKeys() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public ResultSet getSuperTypes(String catalog, String schemaPattern,
			String typeNamePattern) throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public ResultSet getSuperTables(String catalog, String schemaPattern,
			String tableNamePattern) throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public ResultSet getAttributes(String catalog, String schemaPattern,
			String typeNamePattern, String attributeNamePattern)
			throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsResultSetHoldability(int holdability)
			throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public int getResultSetHoldability() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public int getDatabaseMajorVersion() throws SQLException {
		return 1;
	}

	@Override
	public int getDatabaseMinorVersion() throws SQLException {
		return 0;
	}

	@Override
	public int getJDBCMajorVersion() throws SQLException {
		return 1;
	}

	@Override
	public int getJDBCMinorVersion() throws SQLException {
		return 0;
	}

	@Override
	public int getSQLStateType() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean locatorsUpdateCopy() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean supportsStatementPooling() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public RowIdLifetime getRowIdLifetime() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	/*
	 * In a database, associated with each database user is a schema.[5] A
	 * schema comprises a collection of schema objects. Examples of schema
	 * objects include: tables views sequences synonyms indexes clusters
	 * database links snapshots procedures functions packages
	 * 
	 * 
	 * Retrieves the schema names available in this database. The results are
	 * ordered by TABLE_CATALOG and TABLE_SCHEM.
	 * 
	 * The schema columns are:
	 * 
	 * TABLE_SCHEM String => schema name TABLE_CATALOG String => catalog name
	 * (may be null)
	 * 
	 * Parameters:
	 * 
	 * catalog - a catalog name; must match the catalog name as it is stored in
	 * the database;"" retrieves those without a catalog; null means catalog
	 * name should not be used to narrow down the search.
	 * 
	 * schemaPattern - a schema name; must match the schema name as it is stored
	 * in the database; null means schema name should not be used to narrow down
	 * the search. Retrieves the schema names available in this database. The
	 * results are ordered by TABLE_CATALOG and TABLE_SCHEM.
	 * 
	 * (non-Javadoc)
	 * 
	 * @see java.sql.DatabaseMetaData#getSchemas(java.lang.String,
	 * java.lang.String)
	 */
	@Override
	public ResultSet getSchemas(String catalog, String schemaPattern)
			throws SQLException {
		java.sql.Statement stmt = null;
		com.zykie.jdbc.ResultSet rs = null;

		try {
			rs = new com.zykie.jdbc.ResultSet(this.conn, null);

			Field[] fields = new Field[2];
//			fields[0] = new Field("", "TABLE_SCHEM", java.sql.Types.CHAR, 0);
//			fields[1] = new Field("", "TABLE_CATALOG", java.sql.Types.CHAR, 0);
			rs.setMetaData(new com.zykie.jdbc.ResultSetMetaData(fields, false,
					false, getExceptionInterceptor()));

		} catch (Exception e) {
			e.printStackTrace();
		}

		return rs;
	}

	@Override
	public boolean supportsStoredFunctionsUsingCallSyntax() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public boolean autoCommitFailureClosesAllResultSets() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public ResultSet getClientInfoProperties() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public ResultSet getFunctions(String catalog, String schemaPattern,
			String functionNamePattern) throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	@Override
	public ResultSet getFunctionColumns(String catalog, String schemaPattern,
			String functionNamePattern, String columnNamePattern)
			throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	public ResultSet getPseudoColumns(String catalog, String schemaPattern,
			String tableNamePattern, String columnNamePattern)
			throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	public boolean generatedKeyAlwaysReturned() throws SQLException {
		// TODO Auto-generated method stub
		throw SQLError.notImplemented();
	}

	/**
	 * Help methods
	 */
	/**
	 * Converts the given string to bytes, using the connection's character
	 * encoding, or if not available, the JVM default encoding.
	 * 
	 * @param s
	 *            DOCUMENT ME!
	 * @return DOCUMENT ME!
	 */
	protected String s2b(String s) throws SQLException {
		if (s == null) {
			return null;
		}

		return s;
	}

	/**
	 * Helper methods to create fields for different purposes
	 */
	protected Field[] createProcedureFields() {
		Field[] fields = new Field[9];
		fields[0] = new Field("", "PROCEDURE_CAT", Types.CHAR, 255);
		fields[1] = new Field("", "PROCEDURE_SCHEM", Types.CHAR, 255);
		fields[2] = new Field("", "PROCEDURE_NAME", Types.CHAR, 255);
		fields[3] = new Field("", "reserved1", Types.CHAR, 0);
		fields[4] = new Field("", "reserved2", Types.CHAR, 0);
		fields[5] = new Field("", "reserved3", Types.CHAR, 0);
		fields[6] = new Field("", "REMARKS", Types.CHAR, 255);
		fields[7] = new Field("", "PROCEDURE_TYPE", Types.SMALLINT, 6);
		fields[8] = new Field("", "SPECIFIC_NAME", Types.CHAR, 255);

		return fields;
	}

	protected Field[] createTableFields() {
		Field[] fields = new Field[10];
		fields[0] = new Field("", "TABLE_CAT", java.sql.Types.VARCHAR, 255);
		fields[1] = new Field("", "TABLE_SCHEM", java.sql.Types.VARCHAR, 0);
		fields[2] = new Field("", "TABLE_NAME", java.sql.Types.VARCHAR, 255);
		fields[3] = new Field("", "TABLE_TYPE", java.sql.Types.VARCHAR, 5);
		fields[4] = new Field("", "REMARKS", java.sql.Types.VARCHAR, 0);
		fields[5] = new Field("", "TYPE_CAT", java.sql.Types.VARCHAR, 0);
		fields[6] = new Field("", "TYPE_SCHEM", java.sql.Types.VARCHAR, 0);
		fields[7] = new Field("", "TYPE_NAME", java.sql.Types.VARCHAR, 0);
		fields[8] = new Field("", "SELF_REFERENCING_COL_NAME",
				java.sql.Types.VARCHAR, 0);
		fields[9] = new Field("", "REF_GENERATION", java.sql.Types.VARCHAR, 0);
		return fields;
	}

	protected Field[] createColumnFields() {
		Field[] fields = new Field[24];
		fields[0] = new Field("", "TABLE_CAT", Types.CHAR, 255);
		fields[1] = new Field("", "TABLE_SCHEM", Types.CHAR, 0);
		fields[2] = new Field("", "TABLE_NAME", Types.CHAR, 255);
		fields[3] = new Field("", "COLUMN_NAME", Types.CHAR, 32);
		fields[4] = new Field("", "DATA_TYPE", Types.INTEGER, 5);
		fields[5] = new Field("", "TYPE_NAME", Types.CHAR, 16);
		fields[6] = new Field("", "COLUMN_SIZE", Types.INTEGER, Integer
				.toString(Integer.MAX_VALUE).length());
		fields[7] = new Field("", "BUFFER_LENGTH", Types.INTEGER, 10);
		fields[8] = new Field("", "DECIMAL_DIGITS", Types.INTEGER, 10);
		fields[9] = new Field("", "NUM_PREC_RADIX", Types.INTEGER, 10);
		fields[10] = new Field("", "NULLABLE", Types.INTEGER, 10);
		fields[11] = new Field("", "REMARKS", Types.CHAR, 0);
		fields[12] = new Field("", "COLUMN_DEF", Types.CHAR, 0);
		fields[13] = new Field("", "SQL_DATA_TYPE", Types.INTEGER, 10);
		fields[14] = new Field("", "SQL_DATETIME_SUB", Types.INTEGER, 10);
		fields[15] = new Field("", "CHAR_OCTET_LENGTH", Types.INTEGER, Integer
				.toString(Integer.MAX_VALUE).length());
		fields[16] = new Field("", "ORDINAL_POSITION", Types.INTEGER, 10);
		fields[17] = new Field("", "IS_NULLABLE", Types.CHAR, 3);
		fields[18] = new Field("", "SCOPE_CATALOG", Types.CHAR, 255);
		fields[19] = new Field("", "SCOPE_SCHEMA", Types.CHAR, 255);
		fields[20] = new Field("", "SCOPE_TABLE", Types.CHAR, 255);
		fields[21] = new Field("", "SOURCE_DATA_TYPE", Types.SMALLINT, 10);
		fields[22] = new Field("", "IS_AUTOINCREMENT", Types.CHAR, 3); // JDBC 4
		fields[23] = new Field("", "IS_GENERATEDCOLUMN", Types.CHAR, 3); // JDBC
																			// 4.1
		return fields;
	}

	protected Field[] createPrimaryKeyFields() {
		Field[] fields = new Field[6];

		fields[0] = new Field("", "TABLE_CAT", Types.CHAR, 255);
		fields[1] = new Field("", "TABLE_SCHEM", Types.CHAR, 0);
		fields[2] = new Field("", "TABLE_NAME", Types.CHAR, 255);
		fields[3] = new Field("", "COLUMN_NAME", Types.CHAR, 32);
		fields[4] = new Field("", "KEY_SEQ", Types.SMALLINT, 5);
		fields[5] = new Field("", "PK_NAME", Types.CHAR, 32);

		return fields;
	}

}
