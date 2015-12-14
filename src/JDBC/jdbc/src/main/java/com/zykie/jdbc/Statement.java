package com.zykie.jdbc;

import java.util.List;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Set;
import java.util.logging.Logger;
//import java.sql.Connection;
import java.io.UnsupportedEncodingException;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.SQLWarning;

import com.zykie.jdbc.utils.SingleByteCharsetConverter;
import com.zykie.jdbc.exceptions.ExceptionInterceptor;
import com.zykie.jdbc.utils.SQLError;
import com.zykie.jdbc.utils.StringUtils;
import com.zykie.jdbc.utils.ZykieLogger;

/*
 * The object used for executing a static SQL statement and returning the results it produces.
 * 
 * By default, only one ResultSet object per Statement object can be open at the same time. 
 * Therefore, if the reading of one ResultSet object is interleaved with the reading of 
 * another, each must have been generated by different Statement objects. All execution 
 * methods in the Statement interface implicitly close a statment's current ResultSet object 
 * if an open one exists. 
 * 
 * A statement object may have a batch of SQL statement
 */
public class Statement implements java.sql.Statement {

	private com.zykie.jdbc.Connection conn;
	private String database;
	private int resultSetType = 0;
	private int resultConcurrency;
	private int resultSetHoldability;
	private List<Object> batchedArgs;
	private SingleByteCharsetConverter charConverter = null;
	private String charEncoding = null;
	private boolean isClosed = false;
	private int maxFieldSize = 1024;
	private int maxRows = -1; // -1 means no limitation
	private boolean closeOnCompletion = false;
	private boolean isPoolable = true;
	private boolean escapeProcessingEnabled = false;
	private int fetchSize = 0; // means no hint
	private int fetchDirection = ResultSet.FETCH_FORWARD;

	/** Set of currently-open ResultSets */
	private Set<ResultSet> openResults = new HashSet<ResultSet>();
	private com.zykie.jdbc.ResultSet result = null; // current
																					// result
	private int statementId; // for profiling
	private int timeoutInMillis = 0; // timeout for a query
	private int updateCount = -1; // The update count for this statement
	// private ArrayList<ResultSetRow> batchedGeneratedKeys = null;
	private boolean retrieveGeneratedKeys = false;
	private ExceptionInterceptor exceptionInterceptor;
	private boolean closeOpenResults = true;

	// JimoDB query response
	private QueryResponse resp = null;

	// JimoDB statement xml string
	private String statData = "00000000<request>" + "<reqid>runjql</reqid>"
			+ "<siteid>100</siteid>" + "<transid>1</transid>"
			+ "<contentformat>xml</contentformat>" + "<ssid>${ssid}</ssid>" 
			+ "<content>${jql}</content>" + "</request>";

	private static final Logger log = ZykieLogger
			.getLogger("com.zykie.jdbc.Statement");

	@Override
	public <T> T unwrap(Class<T> iface) throws SQLException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public boolean isWrapperFor(Class<?> iface) throws SQLException {
		// TODO Auto-generated method stub
		return false;
	}

	public Statement(Connection conn, String database) throws SQLException {
		this.conn = (com.zykie.jdbc.Connection) conn;
		this.database = database;
		this.exceptionInterceptor = this.conn.getExceptionInterceptor();
		this.result = new com.zykie.jdbc.ResultSet(conn, this);
	}

	/*
	 * Executes the given SQL statement, which may return multiple results. In
	 * some (uncommon) situations, a single SQL statement may return multiple
	 * result sets and/or update counts. Normally you can ignore this unless you
	 * are (1) executing a stored procedure that you know may return multiple
	 * results or (2) you are dynamically executing an unknown SQL string.
	 * 
	 * The execute method executes an SQL statement and indicates the form of
	 * the first result. You must then use the methods getResultSet or
	 * getUpdateCount to retrieve the result, and getMoreResults to move to any
	 * subsequent result(s).
	 * 
	 * (non-Javadoc)
	 * 
	 * @see java.sql.Statement#execute(java.lang.String)
	 */
	@Override
	public boolean execute(String sql) throws SQLException {
		log.info("execute: " + sql);
		
		String curStatData = statData;
		String recMessage = "";

		//add ";" if not having one
		if (!sql.endsWith(";"))
			//sql = sql + ";";
			sql = "<![CDATA["+sql + ";]]>";
		
		curStatData = curStatData.replace("${ssid}", this.conn.getSsid());
		
		
		
		curStatData = curStatData.replace("${jql}", sql);
		//String curStatData2=curStatData;

		try {
			this.conn.sendMessage(curStatData);
			System.out.println("send message to server : " + curStatData);
			log.info(curStatData);

			recMessage = this.conn.getMessage();
			log.info(recMessage);

	
			this.resp = new QueryResponse(recMessage);
			result.setQueryResponse(resp);
			updateCount = resp.getUpdateCount();

		} catch (Exception e) {
			throw SQLError.createSQLException(
					"Failed to execute the SQL statement: " + sql,
					this.conn.getExceptionInterceptor());
		}
	
		if (!result.hasRecord()) 
            return false;
		else
            return true;
	}

	@Override
	public ResultSet executeQuery(String sql) throws SQLException {
		log.info("executeQuery: " + sql);
		execute(sql);
		log.info("The result is: " + this.result);
		return this.result;
	}

	@Override
	public int executeUpdate(String sql) throws SQLException {
		log.info("executeUpdate: " + sql);
		execute(sql);
		return this.updateCount;
		//return 1;
	}

	/*
	 * Releases this Statement object's database and JDBC resources immediately
	 * instead of waiting for this to happen when it is automatically closed. It
	 * is generally good practice to release resources as soon as you are
	 * finished with them to avoid tying up database resources. (non-Javadoc)
	 * 
	 * @see java.sql.Statement#close()
	 */
	@Override
	public void close() throws SQLException {
		if (this.conn == null)
			return; // already closed

		synchronized (this) {

			// additional check in case Statement was closed
			// while current thread was waiting for lock
			if (this.isClosed)
				return;

			if (closeOpenResults) {
				if (this.result != null) {

					try {
						this.result.close();
					} catch (Exception ex) {
						;
					}
				}
			}

			this.isClosed = true;

			this.result = null;
			this.conn = null;
			this.openResults = null;
		}
	}

	@Override
	public int getMaxFieldSize() throws SQLException {
		return this.maxFieldSize;
	}

	/*
	 * Sets the limit for the maximum number of bytes in a ResultSet column
	 * storing character or binary values to the given number of bytes.
	 * 
	 * (non-Javadoc)
	 * 
	 * @see java.sql.Statement#setMaxFieldSize(int)
	 */
	@Override
	public void setMaxFieldSize(int max) throws SQLException {
		this.maxFieldSize = max;
	}

	@Override
	public int getMaxRows() throws SQLException {
		return this.maxRows;
	}

	/*
	 * Sets the limit for the maximum number of rows that any ResultSet object
	 * can contain to the given number. If the limit is exceeded, the excess
	 * rows are silently dropped.
	 * 
	 * (non-Javadoc)
	 * 
	 * @see java.sql.Statement#setMaxRows(int)
	 */
	@Override
	public void setMaxRows(int max) throws SQLException {
		this.maxRows = max;
	}

	/*
	 * Sets escape processing on or off. If escape scanning is on (the default),
	 * the driver will do escape substitution before sending the SQL statement
	 * to the database. Note: Since prepared statements have usually been parsed
	 * prior to making this call, disabling escape processing for
	 * PreparedStatements objects will have no effect.
	 * 
	 * We might need to translate the SQL for sometime
	 * 
	 * (non-Javadoc)
	 * 
	 * @see java.sql.Statement#setEscapeProcessing(boolean)
	 */
	@Override
	public void setEscapeProcessing(boolean enable) throws SQLException {
		this.escapeProcessingEnabled = enable;
	}

	@Override
	public int getQueryTimeout() throws SQLException {
		return this.timeoutInMillis / 1000;
	}

	@Override
	public void setQueryTimeout(int seconds) throws SQLException {
		this.timeoutInMillis = seconds * 1000;
	}

	@Override
	public void cancel() throws SQLException {
		// TODO Auto-generated method stub

	}

	@Override
	public SQLWarning getWarnings() throws SQLException {
		return null;
	}

	@Override
	public void clearWarnings() throws SQLException {
		return;
	}

	/*
	 * Sets the SQL cursor name to the given String, which will be used by
	 * subsequent Statement object execute methods. This name can then be used
	 * in SQL positioned update or delete statements to identify the current row
	 * in the ResultSet object generated by this statement. If the database does
	 * not support positioned update/delete, this method is a noop. To insure
	 * that a cursor has the proper isolation level to support updates, the
	 * cursor's SELECT statement should have the form SELECT FOR UPDATE. If FOR
	 * UPDATE is not present, positioned updates may fail.
	 * 
	 * Note: By definition, the execution of positioned updates and deletes must
	 * be done by a different Statement object than the one that generated the
	 * ResultSet object being used for positioning. Also, cursor names must be
	 * unique within a connection.
	 * 
	 * (non-Javadoc)
	 * 
	 * @see java.sql.Statement#setCursorName(java.lang.String)
	 */
	@Override
	public void setCursorName(String name) throws SQLException {
		// No-Op for now
	}

	@Override
	public ResultSet getResultSet() throws SQLException {
		return this.result;
	}

	/**
	 * getUpdateCount returns the current result as an update count, if the
	 * result is a ResultSet or there are no more results, -1 is returned. It
	 * should only be called once per result.
	 * 
	 * @return the current result as an update count.
	 * 
	 * @exception SQLException
	 *                if a database access error occurs
	 */
	@Override
	public int getUpdateCount() throws SQLException {
		log.info("getUpdateCount :" + this.updateCount);
		Exception exc = new Exception("this is a log");
		exc.printStackTrace();
		return this.updateCount;
	//	return -1;
	}

	@Override
	public boolean getMoreResults() throws SQLException {
		return getMoreResults(CLOSE_CURRENT_RESULT);
	}

	/*
	 * Retrieves the direction for fetching rows from database tables that is
	 * the default for result sets generated from this Statement object. If this
	 * Statement object has not set a fetch direction by calling the method
	 * setFetchDirection, the return value is implementation-specific.
	 * 
	 * (non-Javadoc)
	 * 
	 * @see java.sql.Statement#setFetchDirection(int)
	 */
	@Override
	public void setFetchDirection(int direction) throws SQLException {
		this.fetchDirection = direction;
	}

	@Override
	public int getFetchDirection() throws SQLException {
		return this.fetchDirection;
	}

	/*
	 * Gives the JDBC driver a hint as to the number of rows that should be
	 * fetched from the database when more rows are needed. The number of rows
	 * specified affects only result sets created using this statement. If the
	 * value specified is zero, then the hint is ignored. The default value is
	 * zero.
	 * 
	 * (non-Javadoc)
	 * 
	 * @see java.sql.Statement#setFetchSize(int)
	 */
	@Override
	public void setFetchSize(int rows) throws SQLException {
		this.fetchSize = rows;
	}

	@Override
	public int getFetchSize() throws SQLException {
		return this.fetchSize;
	}

	@Override
	public int getResultSetConcurrency() throws SQLException {
		return this.resultConcurrency;
	}

	@Override
	public int getResultSetType() throws SQLException {
		return this.resultSetType;
	}

	/*
	 * A statement may have multiple SQLs (non-Javadoc)
	 * 
	 * @see java.sql.Statement#addBatch(java.lang.String)
	 */
	@Override
	public void addBatch(String sql) throws SQLException {
		synchronized (this.conn.getConnectionMutex()) {
			if (this.batchedArgs == null) {
				this.batchedArgs = new ArrayList<Object>();
			}

			if (sql != null) {
				this.batchedArgs.add(sql);
			}
		}

	}

	@Override
	public void clearBatch() throws SQLException {
		synchronized (this.conn.getConnectionMutex()) {
			if (this.batchedArgs != null) {
				this.batchedArgs.clear();
			}
		}
	}

	/*
	 * Submits a batch of commands to the database for execution and if all
	 * commands execute successfully, returns an array of update counts.
	 * (non-Javadoc)
	 * 
	 * @see java.sql.Statement#executeBatch()
	 */
	@Override
	public int[] executeBatch() throws SQLException {
		throw SQLError.notImplemented();
	}

	@Override
	public Connection getConnection() throws SQLException {
		return this.conn;
	}

	/*
	 * Moves to this Statement object's next result, returns true if it is a
	 * ResultSet object, and implicitly closes any current ResultSet object(s)
	 * obtained with the method getResultSet.
	 * 
	 * (non-Javadoc)
	 * 
	 * @see java.sql.Statement#getMoreResults(int)
	 */
	@Override
	public boolean getMoreResults(int current) throws SQLException {
		return false;
	}

	@Override
	public ResultSet getGeneratedKeys() throws SQLException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public int executeUpdate(String sql, int autoGeneratedKeys)
			throws SQLException {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public int executeUpdate(String sql, int[] columnIndexes)
			throws SQLException {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public int executeUpdate(String sql, String[] columnNames)
			throws SQLException {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public boolean execute(String sql, int autoGeneratedKeys)
			throws SQLException {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean execute(String sql, int[] columnIndexes) throws SQLException {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean execute(String sql, String[] columnNames)
			throws SQLException {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public int getResultSetHoldability() throws SQLException {
		return this.resultSetHoldability;
	}

	@Override
	public boolean isClosed() throws SQLException {
		return isClosed;
	}

	@Override
	public void setPoolable(boolean poolable) throws SQLException {
		this.isPoolable = poolable;
	}

	@Override
	public boolean isPoolable() throws SQLException {
		// TODO Auto-generated method stub
		return isPoolable;
	}

	public void closeOnCompletion() throws SQLException {
		closeOnCompletion = true;
	}

	public boolean isCloseOnCompletion() throws SQLException {
		return this.closeOnCompletion;
	}

	public void setResultSetType(int resultSetType) {
		this.resultSetType = resultSetType;
	}

	public void setResultSetConcurrency(int resultSetConcurrency) {
		this.resultConcurrency = resultSetConcurrency;
	}

	public void setResultSetHoldability(int resultSetHoldability) {
		this.resultSetHoldability = resultSetHoldability;
	}
}
