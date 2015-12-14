package com.zykie.jdbc;

import java.sql.Array;
import java.sql.Blob;
import java.sql.CallableStatement;
import java.sql.Clob;
import java.sql.DatabaseMetaData;
import java.sql.NClob;
import java.sql.PreparedStatement;
import java.sql.SQLClientInfoException;
import java.sql.SQLException;
import java.sql.SQLWarning;
import java.sql.SQLXML;
import java.sql.Savepoint;
import java.sql.Statement;
import java.sql.Struct;
import java.util.Map;
import java.util.HashMap;
import java.util.Properties;
import java.util.concurrent.Executor;
import java.util.logging.LogRecord;
import java.io.BufferedInputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.lang.reflect.Constructor;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.CharBuffer;
import java.nio.charset.CharacterCodingException;
import java.nio.charset.Charset;
import java.nio.charset.CharsetDecoder;
import java.nio.charset.CharsetEncoder;

import org.apache.commons.lang.ArrayUtils;

import java.util.logging.Logger;

import com.zykie.jdbc.utils.*;
import com.zykie.jdbc.exceptions.*;

/****************************************************************************************
 * A connection (session) with a specific database. SQL statements are executed and results 
 * are returned within the context of a connection.
 *
 * A Connection object's database is able to provide information describing its tables, its
 *  supported SQL grammar, its stored procedures, the capabilities of this connection, and 
 *  so on. This information is obtained with the getMetaData method.
 *  
 *  Note: When configuring a Connection, JDBC applications should use the appropritate 
 *  Connection method such as setAutoCommit or setTransactionIsolation. Applications should 
 *  not invoke SQL commands directly to change the connection's configuration when there is
 *   a JDBC method available. By default a Connection object is in auto-commit mode, which
 *    means that it automatically commits changes after executing each statement. 
 *    If auto-commit mode has been disabled, the method commit must be called explicitly
 *     in order to commit changes; otherwise, database changes will not be saved.
 *     
 * A new Connection object created using the JDBC 2.1 core API has an initially empty type
 *  map associated with it. A user may enter a custom mapping for a UDT in this type map. 
 *  When a UDT is retrieved from a data source with the method ResultSet.getObject, the 
 *  getObject method will check the connection's type map to see if there is an entry for 
 *  that UDT. If so, the getObject method will map the UDT to the class indicated. If there 
 *  is no entry, the UDT will be mapped using the standard mapping.
 *  
 * A user may create a new type map, which is a java.util.Map object, make an entry in it, 
 * and pass it to the java.sql methods that can perform custom mapping. In this case, the 
 * method will use the given type map instead of the one associated with the connection.
 * 
 * For example, the following code fragment specifies that the SQL type ATHLETES will be 
 * mapped to the class Athletes in the Java programming language. The code fragment retrieves
 *  the type map for the Connection object con, inserts the entry into it, and then sets the
 *   type map with the new entry as the connection's type map.
 *   
 *    java.util.Map map = con.getTypeMap();
 *    map.put("mySchemaName.ATHLETES", Class.forName("Athletes"));
 *    con.setTypeMap(map);    
 */
public class Connection implements java.sql.Connection {
	private String url = null;
	private String host = null;
	private int port = 0;
	private String user = null;
	private String passwd = null;
	private String database = null;
	private int timeout = 0;
	private Socket socket = null;
	DataOutputStream outstream = null;
	DataInputStream inStream = null;
	private boolean isClosed = true;
	private int bufferSize = 1024;
	private byte[] buffer = new byte[this.bufferSize];
	private String ssid = null;
	private boolean autoCommit = true;
	private boolean readOnly = false;
	private int holdability = ResultSet.HOLD_CURSORS_OVER_COMMIT;
	private java.util.Map<String, Class<?>> typeMap = null;

	private String connData = "00000000<request>" + "<reqid>login</reqid>"
			+ "<siteid>100</siteid>" + "<transid>1</transid>"  
			+ "<user>${user}</user>" + "<passwd>${passwd}</passwd>"
			+ "<domain>zky_sysuser</domain>" + "</request>";

	private static final Logger log = ZykieLogger
			.getLogger("com.zykie.jdbc.Connection");

	/*
	 * Properties copied from MySQL
	 */
	private ExceptionInterceptor exceptionInterceptor;
	private static final Constructor<?> JDBC_4_CONNECTION_CTOR;
	private static final int DEFAULT_RESULT_SET_TYPE = ResultSet.TYPE_FORWARD_ONLY;
	private static final int DEFAULT_RESULT_SET_CONCURRENCY = ResultSet.CONCUR_READ_ONLY;

	static {
		if (Util.isJdbc4()) {
			log.info("Is jdbc4");
			try {
				JDBC_4_CONNECTION_CTOR = Class.forName(
						"com.zykie.jdbc.JDBC4Connection").getConstructor(
						new Class[] { String.class, Integer.TYPE,
								Properties.class, String.class, String.class });
			} catch (SecurityException e) {
				throw new RuntimeException(e);
			} catch (NoSuchMethodException e) {
				throw new RuntimeException(e);
			} catch (ClassNotFoundException e) {
				throw new RuntimeException(e);
			}
		} else {
			log.info("Not jdbc4");
			JDBC_4_CONNECTION_CTOR = null;
		}
	}

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

	/**
	 * SQL statements without parameters are normally executed using Statement
	 * objects. If the same SQL statement is executed many times, it is more
	 * efficient to use a PreparedStatement
	 * 
	 * @return a new Statement object
	 * @throws SQLException
	 *             passed through from the constructor
	 */
	@Override
	public Statement createStatement() throws SQLException {
		return createStatement(DEFAULT_RESULT_SET_TYPE,
				DEFAULT_RESULT_SET_CONCURRENCY);
	}

	/**
	 * JDBC 2.0 Same as createStatement() above, but allows the default result
	 * set type and result set concurrency type to be overridden.
	 * 
	 * @param resultSetType
	 *            a result set type, see ResultSet.TYPE_XXX. The type is
	 *            normally relevant with scrolling, cursor, etc
	 * @param resultSetConcurrency
	 *            a concurrency type, see ResultSet.CONCUR_XXX
	 * @return a new Statement object
	 * @exception SQLException
	 *                if a database-access error occurs.
	 */
	@Override
	public Statement createStatement(int resultSetType, int resultSetConcurrency)
			throws SQLException {

		com.zykie.jdbc.Statement stmt = new com.zykie.jdbc.Statement(this,
				this.database);
		stmt.setResultSetType(resultSetType);
		stmt.setResultSetConcurrency(resultSetConcurrency);

		return stmt;
	}

	@Override
	public Statement createStatement(int resultSetType,
			int resultSetConcurrency, int resultSetHoldability)
			throws SQLException {

		Statement s = createStatement(resultSetType, resultSetConcurrency);
		((com.zykie.jdbc.Statement) s)
				.setResultSetHoldability(resultSetHoldability);

		return null;
	}

	/*
	 * A statement with parameters which can be parsed for once and used many
	 * times
	 * 
	 * It is a sub-interface from Statement
	 * 
	 * (non-Javadoc)
	 * 
	 * @see java.sql.Connection#prepareStatement(java.lang.String)
	 */
	@Override
	public PreparedStatement prepareStatement(String sql) throws SQLException {
		//throw SQLError.notImplemented();
		com.zykie.jdbc.PreparedStatement p=new com.zykie.jdbc.PreparedStatement(this,database,sql);
		return p;
	}

	/*
	 * A statement with procedure call functions
	 * 
	 * It is a sub-interface from PreparedStatement
	 * 
	 * (non-Javadoc)
	 * 
	 * @see java.sql.Connection#CallableStatement(java.lang.String)
	 */
	@Override
	public CallableStatement prepareCall(String sql) throws SQLException {
		throw SQLError.notImplemented();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.sql.Connection#nativeSQL(java.lang.String)
	 * 
	 * Converts the given SQL statement into the system's native SQL grammar. A
	 * driver may convert the JDBC SQL grammar into its system's native SQL
	 * grammar prior to sending it. This method returns the native form of the
	 * statement that the driver would have sent.
	 * 
	 * 1. Convert ? to parameter value if any. 2. Convert to Zykie's xml format
	 */
	@Override
	public String nativeSQL(String sql) throws SQLException {
		return sql;
	}

	/**
	 * If a connection is in auto-commit mode, than all its SQL statements will
	 * be executed and committed as individual transactions. Otherwise, its SQL
	 * statements are grouped into transactions that are terminated by either
	 * commit() or rollback(). By default, new connections are in auto- commit
	 * mode. The commit occurs when the statement completes or the next execute
	 * occurs, whichever comes first. In the case of statements returning a
	 * ResultSet, the statement completes when the last row of the ResultSet has
	 * been retrieved or the ResultSet has been closed. In advanced cases, a
	 * single statement may return multiple results as well as output parameter
	 * values. Here the commit occurs when all results and output param values
	 * have been retrieved.
	 * <p>
	 * <b>Note:</b> Zykie does not support transactions, so this method is a
	 * no-op.
	 * </p>
	 * 
	 * @param autoCommitFlag
	 *            - true enables auto-commit; false disables it
	 * @exception SQLException
	 *                if a database access error occurs
	 */
	@Override
	public void setAutoCommit(boolean autoCommit) throws SQLException {
		this.autoCommit = autoCommit;

	}

	@Override
	public boolean getAutoCommit() throws SQLException {
		// TODO Auto-generated method stub
		return this.autoCommit;
	}

	public String getUrl() throws SQLException {
		// TODO Auto-generated method stub
		return this.url;
	}

	/**
	 * The method commit() makes all changes made since the previous
	 * commit/rollback permanent and releases any database locks currently held
	 * by the Connection. This method should only be used when auto-commit has
	 * been disabled.
	 * <p>
	 * <b>Note:</b> Zykie does not support transactions, so this method is a
	 * no-op.
	 * </p>
	 * 
	 * @exception SQLException
	 *                if a database access error occurs
	 * @see setAutoCommit
	 */
	@Override
	public void commit() throws SQLException {
		// do nothing for now
	}

	/**
	 * The method rollback() drops all changes made since the previous
	 * commit/rollback and releases any database locks currently held by the
	 * Connection.
	 * 
	 * @exception SQLException
	 *                if a database access error occurs
	 * @see commit
	 */
	@Override
	public void rollback() throws SQLException {
		// do nothing for now
	}

	@Override
	public void close() throws SQLException {
		// close connection to a jimo AccessServer
		try {
			if (this.inStream != null)
				this.inStream.close();
			if (this.outstream != null)
				this.outstream.close();
			if (this.socket != null)
				this.socket.close();
		} catch (Exception e) {
			System.err.println(e.getMessage());
		}

		this.isClosed = true;
	}

	@Override
	public boolean isClosed() throws SQLException {
		// TODO Auto-generated method stub
		return this.isClosed;
	}

	/*
	 * Retrieves a DatabaseMetaData object that contains metadata about the
	 * database to which this Connection object represents a connection. The
	 * metadata includes information about the database's tables, its supported
	 * SQL grammar, its stored procedures, the capabilities of this connection,
	 * and so on.
	 * 
	 * (non-Javadoc)
	 * 
	 * @see java.sql.Connection#getMetaData()
	 */
	@Override
	public DatabaseMetaData getMetaData() throws SQLException {
		return new com.zykie.jdbc.DatabaseMetaData(this, this.database);
	}

	/**
	 * You can put a connection in read-only mode as a hint to enable database
	 * optimizations <B>Note:</B> setReadOnly cannot be called while in the
	 * middle of a transaction
	 * 
	 * @param readOnlyFlag
	 *            - true enables read-only mode; false disables it
	 * @exception SQLException
	 *                if a database access error occurs
	 */
	@Override
	public void setReadOnly(boolean readOnly) throws SQLException {
		// TODO Auto-generated method stub
		this.readOnly = readOnly;

	}

	@Override
	public boolean isReadOnly() throws SQLException {
		// TODO Auto-generated method stub
		return this.readOnly;
	}

	/**
	 * A sub-space of this Connection's database may be selected by setting a
	 * catalog name. If the driver does not support catalogs, it will silently
	 * ignore this request
	 * <p>
	 * <b>Note:</b> Zykie's notion of catalogs are individual databases.
	 * </p>
	 * 
	 * @param catalog
	 *            the database for this connection to use
	 * @throws SQLException
	 *             if a database access error occurs
	 */
	@Override
	public void setCatalog(String catalog) throws SQLException {
		synchronized (getConnectionMutex()) {
			if (catalog == null) {
				throw SQLError.createSQLException("Catalog can not be null",
						SQLError.SQL_STATE_ILLEGAL_ARGUMENT,
						getExceptionInterceptor());
			}

			this.database = catalog;
		}
	}

	@Override
	public String getCatalog() throws SQLException {
		// TODO Auto-generated method stub
		System.out.println("调用了getCataloghhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh");
		return this.database;
	}

	@Override
	public void setTransactionIsolation(int level) throws SQLException {
//		synchronized (getConnectionMutex()) {
//
//			throw SQLError.createSQLException(
//					"Transaction Isolation Levels are "
//							+ "not supported on Zykie DB now.",
//					SQLError.SQL_STATE_DRIVER_NOT_CAPABLE,
//					getExceptionInterceptor());
//		}
	}

	@Override
	public int getTransactionIsolation() throws SQLException {
		//throw SQLError.notImplemented();
		return 0;
	}

	/**
	 * The first warning reported by calls on this Connection is returned.
	 * <B>Note:</B> Subsequent warnings will be changed to this
	 * java.sql.SQLWarning
	 * 
	 * @return the first java.sql.SQLWarning or null
	 * @exception SQLException
	 *                if a database access error occurs
	 */
	@Override
	public SQLWarning getWarnings() throws SQLException {
		// TODO Auto-generated method stub
		return null;
	}

	/**
	 * After this call, getWarnings returns null until a new warning is reported
	 * for this connection.
	 * 
	 * @exception SQLException
	 *                if a database access error occurs
	 */
	@Override
	public void clearWarnings() throws SQLException {
		// TODO Auto-generated method stub
	}

	@Override
	public PreparedStatement prepareStatement(String sql, int resultSetType,
			int resultSetConcurrency) throws SQLException {
		// TODO Auto-generated method stub
		return new com.zykie.jdbc.PreparedStatement(this,database,sql);
	}

	@Override
	public CallableStatement prepareCall(String sql, int resultSetType,
			int resultSetConcurrency) throws SQLException {
		throw SQLError.notImplemented();
	}

	/*
	 * The TypeMap is empty at the beginning. If not adding later, empty map
	 * will be returned.
	 * 
	 * We may add XML class in the typeMap???
	 * 
	 * (non-Javadoc)
	 * 
	 * @see java.sql.Connection#getTypeMap()
	 */
	@Override
	public Map<String, Class<?>> getTypeMap() throws SQLException {
		synchronized (getConnectionMutex()) {
			if (this.typeMap == null) {
				this.typeMap = new HashMap<String, Class<?>>();
			}

			return this.typeMap;
		}
	}

	/*
	 * A new Connection object created using the JDBC 2.1 core API has an
	 * initially empty type map associated with it. A user may enter a custom
	 * mapping for a UDT in this type map. When a UDT is retrieved from a data
	 * source with the method ResultSet.getObject, the getObject method will
	 * check the connection's type map to see if there is an entry for that UDT.
	 * If so, the getObject method will map the UDT to the class indicated. If
	 * there is no entry, the UDT will be mapped using the standard mapping.
	 * 
	 * A user may create a new type map, which is a java.util.Map object, make
	 * an entry in it, and pass it to the java.sql methods that can perform
	 * custom mapping. In this case, the method will use the given type map
	 * instead of the one associated with the connection.
	 * 
	 * For example, the following code fragment specifies that the SQL type
	 * ATHLETES will be mapped to the class Athletes in the Java programming
	 * language. The code fragment retrieves the type map for the Connection
	 * object con, inserts the entry into it, and then sets the type map with
	 * the new entry as the connection's type map.
	 * 
	 * java.util.Map map = con.getTypeMap(); map.put("mySchemaName.ATHLETES",
	 * Class.forName("Athletes")); con.setTypeMap(map);
	 * 
	 * (non-Javadoc)
	 * 
	 * @see java.sql.Connection#setTypeMap(java.util.Map)
	 */
	@Override
	public void setTypeMap(Map<String, Class<?>> map) throws SQLException {
		// TODO Auto-generated method stub

	}

	/*
	 * Holdability means whether or not a resultset cursor can be held when
	 * committing
	 * 
	 * (non-Javadoc)
	 * 
	 * @see java.sql.Connection#setHoldability(int)
	 */
	@Override
	public void setHoldability(int holdability) throws SQLException {
		// TODO Auto-generated method stub
		this.holdability = holdability;
	}

	@Override
	public int getHoldability() throws SQLException {
		return this.holdability;
	}

	/*
	 * Savepoint is used in transaction mode to tell which place in in a
	 * transaction to rollback, commit or resume, etc
	 * 
	 * Not supported for now (non-Javadoc)
	 * 
	 * @see java.sql.Connection#setSavepoint()
	 */
	@Override
	public Savepoint setSavepoint() throws SQLException {
		return null;
	}

	@Override
	public Savepoint setSavepoint(String name) throws SQLException {
		return null;
	}

	/*
	 * Rollback to a specified savepoint
	 * 
	 * Not supported for now
	 * 
	 * (non-Javadoc)
	 * 
	 * @see java.sql.Connection#rollback(java.sql.Savepoint)
	 */
	@Override
	public void rollback(Savepoint savepoint) throws SQLException {
		// Do nothing for now
	}

	/*
	 * Remove the current savepoint set previously
	 * 
	 * Not supported for now. In the future, we will use a list of savepoints to
	 * control where to rollback to
	 * 
	 * (non-Javadoc)
	 * 
	 * @see java.sql.Connection#releaseSavepoint(java.sql.Savepoint)
	 */
	@Override
	public void releaseSavepoint(Savepoint savepoint) throws SQLException {
		// do nothing for now
	}

	/*
	 * This is for statements has parameters to fed into
	 * 
	 * Not supported for now
	 * 
	 * (non-Javadoc)
	 * 
	 * @see java.sql.Connection#prepareStatement(java.lang.String, int, int,
	 * int)
	 */
	@Override
	public PreparedStatement prepareStatement(String sql, int resultSetType,
			int resultSetConcurrency, int resultSetHoldability)
			throws SQLException {
		// TODO Auto-generated method stub
		return new com.zykie.jdbc.PreparedStatement(this,database,sql);
	}

	@Override
	public CallableStatement prepareCall(String sql, int resultSetType,
			int resultSetConcurrency, int resultSetHoldability)
			throws SQLException {
		throw SQLError.notImplemented();
	}

	@Override
	public PreparedStatement prepareStatement(String sql, int autoGeneratedKeys)
			throws SQLException {
		//throw SQLError.notImplemented();
		return new com.zykie.jdbc.PreparedStatement(this,database,sql);
	}

	@Override
	public PreparedStatement prepareStatement(String sql, int[] columnIndexes)
			throws SQLException {
		//throw SQLError.notImplemented();
		return new com.zykie.jdbc.PreparedStatement(this,database,sql);
	}

	@Override
	public PreparedStatement prepareStatement(String sql, String[] columnNames)
			throws SQLException {
		//throw SQLError.notImplemented();
		return new com.zykie.jdbc.PreparedStatement(this,database,sql);
	}

	@Override
	public Clob createClob() throws SQLException {
		throw SQLError.notImplemented();
	}

	@Override
	public Blob createBlob() throws SQLException {
		throw SQLError.notImplemented();
	}

	@Override
	public NClob createNClob() throws SQLException {
		throw SQLError.notImplemented();
	}

	@Override
	public SQLXML createSQLXML() throws SQLException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public boolean isValid(int timeout) throws SQLException {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public void setClientInfo(String name, String value)
			throws SQLClientInfoException {
		// TODO Auto-generated method stub

	}

	@Override
	public void setClientInfo(Properties properties)
			throws SQLClientInfoException {
		// TODO Auto-generated method stub

	}

	@Override
	public String getClientInfo(String name) throws SQLException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Properties getClientInfo() throws SQLException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Array createArrayOf(String typeName, Object[] elements)
			throws SQLException {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Struct createStruct(String typeName, Object[] attributes)
			throws SQLException {
		// TODO Auto-generated method stub
		return null;
	}

	public void setSchema(String schema) throws SQLException {
		// TODO Auto-generated method stub

	}

	public String getSchema() throws SQLException {
		// TODO Auto-generated method stub
		return null;
	}

	public void abort(Executor executor) throws SQLException {
		// TODO Auto-generated method stub

	}

	public void setNetworkTimeout(Executor executor, int milliseconds)
			throws SQLException {
		// TODO Auto-generated method stub

	}

	public int getNetworkTimeout() throws SQLException {
		// TODO Auto-generated method stub
		return 0;
	}

	public boolean getUseSqlStateCodes() {
		// TODO Auto-generated method stub
		return false;
	}

	public String getServerVariable(String string) {
		// TODO Auto-generated method stub
		return null;
	}

	public boolean getInteractiveClient() {
		// TODO Auto-generated method stub
		return false;
	}

	public String getLocalSocketAddress() {
		// TODO Auto-generated method stub
		return null;
	}

	public boolean getMaintainTimeStats() {
		// TODO Auto-generated method stub
		return false;
	}

	public boolean getParanoid() {
		// TODO Auto-generated method stub
		return false;
	}

	public SingleByteCharsetConverter getCharsetConverter(String encoding) {
		// TODO Auto-generated method stub
		return null;
	}

	public String getEncoding() {
		// TODO Auto-generated method stub
		return null;
	}

	public boolean getUseUnicode() {
		// TODO Auto-generated method stub
		return false;
	}

	/**
	 * Creates a connection to a Zykie Server.
	 * 
	 * @param hostToConnectTo
	 *            the hostname of the database server
	 * @param portToConnectTo
	 *            the port number the server is listening on
	 * @param info
	 *            a Properties[] list holding the user and password
	 * @param databaseToConnectTo
	 *            the database to connect to
	 * @param url
	 *            the URL of the connection
	 * @param d
	 *            the Driver instantation of the connection
	 * @exception SQLException
	 *                if a database access error occurs
	 */
	public Connection(String host, int port, Properties info, String database,
			String url) throws Exception {

		this.url = url;
		try {
			this.host = host;
			this.port = port;
			// this.timeout = timeout;
			this.socket = new Socket(host, port);
			this.socket.setSoTimeout(timeout);
			this.socket.setKeepAlive(true);
			this.socket.setOOBInline(true);
			this.isClosed = this.socket.isClosed();
			this.inStream = new DataInputStream(new BufferedInputStream(
					this.socket.getInputStream()));
			this.outstream = new DataOutputStream(this.socket.getOutputStream());
			this.user = info.getProperty(Driver.USER_PROPERTY_KEY);
			this.passwd = info.getProperty(Driver.PASSWORD_PROPERTY_KEY);
			this.database = info.getProperty(Driver.DBNAME_PROPERTY_KEY);

			connData = connData.replace("${user}", this.user);
			connData = connData.replace("${passwd}", this.passwd);
			this.sendMessage(connData);
			log.info(connData);
			String recMessage = this.getMessage();

			log.info(recMessage);
			this.ssid = recMessage.substring(recMessage.indexOf("<session_id>")
					+ "<session_id>".length(),
					recMessage.indexOf("</session_id>"));
			
			//change the working database to user specified database
			String sql = "";
			Statement s;
			
			if (this.database != null) {
				sql = "use " + this.database + ";";
				s = new com.zykie.jdbc.Statement(this, this.database);
				s.executeQuery(sql);
			}

		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public static Connection getInstance(String host, int port,
			Properties props, String database, String url) throws Exception {
		if (!Util.isJdbc4()) {
			return new Connection(host, port, props, database, url);
		}

		return (Connection) Util.handleNewInstance(JDBC_4_CONNECTION_CTOR,
				new Object[] { host, Integer.valueOf(port), props, database,
						url }, null);
	}

	/************************************************************
	 ************ Methods copied from MySQL driver
	 ************************************************************/
	public Object getConnectionMutex() {
		return this;
	}

	public ExceptionInterceptor getExceptionInterceptor() {
		return this.exceptionInterceptor;
	}

	/************************************************************
	 ************ JimoDB specific methods
	 ************************************************************/
	public synchronized String getSsid() {
		return ssid;
	}

	public synchronized boolean sendMessage(String message) throws Exception {
		if (message == null)
			return false;

		if ((this.outstream != null) && (this.socket != null)) {
			log.info("message encoding : " + StringUtils.getEncoding(message));
			Charset charset = Charset.forName("UTF-8");
			CharsetDecoder decoder = charset.newDecoder();
			CharsetEncoder encoder = charset.newEncoder();
			 ByteBuffer bbuf =null;
			try {
			    
			    bbuf = encoder.encode(CharBuffer.wrap(message));
			    System.out.println(bbuf.array().length);
			    CharBuffer cbuf = decoder.decode(bbuf);
			    String s = cbuf.toString();
			    System.out.println(s+"==================890809780");
			} catch (CharacterCodingException e) {
			}
			this.outstream.writeInt(bbuf.array().length);
			this.outstream.write(bbuf.array());
			this.outstream.flush();

		} else {
			this.isClosed = true;
			close();
			return false;
		}

		return true;
	}

	public synchronized String getMessage() throws Exception {

		if ((this.inStream != null) && (this.socket != null)) {
			byte[] recData = new byte[0];
			int crtLength = 0;
			crtLength = this.inStream.readInt();
			int offset = 0;
			int len = 0;

			while ((crtLength > 0) && (offset < crtLength) && (len != -1)) {
				try {
					len = this.inStream.read(this.buffer, 0, this.bufferSize);

				} catch (Exception e) {
					System.out.println("Read timeout");
					e.printStackTrace();
					close();

					break;
				}

				if (len != -1) {
					recData = ArrayUtils.addAll(recData,
							ArrayUtils.subarray(this.buffer, 0, len));
					offset += len;
				}
			}
			try {
				return new String(recData, "UTF-8");

			} catch (Exception e) {
				e.printStackTrace();
			}
		} else {
			this.isClosed = true;
			close();
		}
		return null;
	}

}
