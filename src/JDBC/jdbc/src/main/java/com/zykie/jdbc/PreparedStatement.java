package com.zykie.jdbc;

import java.io.InputStream;
import java.io.Reader;
import java.math.BigDecimal;
import java.net.URL;
import java.sql.Array;
import java.sql.Blob;
import java.sql.Clob;
import java.sql.Date;
import java.sql.NClob;
import java.sql.ParameterMetaData;
import java.sql.Ref;
import java.sql.ResultSet;
import java.sql.ResultSetMetaData;
import java.sql.RowId;
import java.sql.SQLException;
import java.sql.SQLWarning;
import java.sql.SQLXML;
import java.sql.Time;
import java.sql.Timestamp;
import java.sql.Types;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;

import com.zykie.jdbc.utils.SQLError;

/*
 * An object that represents a precompiled SQL statement.
 * A SQL statement is precompiled and stored in a PreparedStatement object. 
 * This object can then be used to efficiently execute this statement multiple times.
 *  
 * Note: The setter methods (setShort, setString, and so on) for setting IN parameter 
 * values must specify types that are compatible with the defined SQL type of the 
 * input parameter. For instance, if the IN parameter has SQL type INTEGER, then 
 * the method setInt should be used.
 * 
 * If arbitrary parameter type conversions are required, the method setObject should 
 * be used with a target SQL type.
 * 
 * In the following example of setting a parameter, con represents an active connection:
 * 
 *   PreparedStatement pstmt = con.prepareStatement("UPDATE EMPLOYEES
 *                                   SET SALARY = ? WHERE ID = ?");
 *   pstmt.setBigDecimal(1, 153833.00)
 *   pstmt.setInt(2, 110592)
 *   
 *  We can regard preparedStatement as statement with "?" parameter(s)
 *  
 *  There are a lot of setXXX() methods in this class used to set different type of values
 *  to the parameters
 *  
 *  Inheritance:
 *   Statement-->PreparedStatement-->CallableStatements
 */
public class PreparedStatement implements java.sql.PreparedStatement {
	protected String charEncoding = null;
	Statement stmt;
	String sql=null;
	/** The number of parameters in this PreparedStatement */
	protected int parameterCount;

//	protected MysqlParameterMetadata parameterMetaData;

//	private InputStream[] parameterStreams = null;

	private byte[][] parameterValues1 = null;
	private String[] paramterValues = null;

	/**
	 * Only used by statement interceptors at the moment to
	 * provide introspection of bound values
	 */
	protected List<String> parameterTypes = null;
	
//	private ParseInfo parseInfo;

	private java.sql.ResultSetMetaData pstmtResultMetaData;

	private byte[][] staticSqlStrings = null;
	private static List<Integer> parameterIndexList = new ArrayList<Integer>();
	
	public static int stringNumbers(String str) {
		int count = 0;
		for(int i = 0; i < str.length(); i++){
		    if('?'==str.charAt(i)) {
		    	count++;
		    	parameterIndexList.add(i);
		    }
		}
		
		return count;
	}  
	//Connection connection=null;
	public PreparedStatement(Connection conn, String database, String sql) throws SQLException {
		//connection=conn;
		stmt = new Statement(conn, database);
		this.sql = sql;
/*		this.charEncoding = conn.getEncoding();
		this.charConverter = conn
		.getCharsetConverter(this.charEncoding);*/
//		parameterCount = sql.con
		parameterTypes = new ArrayList<String>();
		paramterValues = new String[stringNumbers(sql)];
		
	}
	
	@Override
	public ResultSet executeQuery(String sql) throws SQLException {
		return stmt.executeQuery(sql);
	}

	@Override
	public int executeUpdate(String sql) throws SQLException {
		return stmt.executeUpdate(sql);
	}

	@Override
	public void close() throws SQLException {
		parameterTypes =null;
		paramterValues =null;
		stmt.close();

	}

	@Override
	public int getMaxFieldSize() throws SQLException {
		return stmt.getMaxFieldSize();
	}

	@Override
	public void setMaxFieldSize(int max) throws SQLException {
		stmt.setMaxFieldSize(max);

	}

	@Override
	public int getMaxRows() throws SQLException {
		
		return stmt.getMaxRows();
	}

	@Override
	public void setMaxRows(int max) throws SQLException {
		stmt.setMaxRows(max);

	}

	@Override
	public void setEscapeProcessing(boolean enable) throws SQLException {
		stmt.setEscapeProcessing(enable);
	}

	@Override
	public int getQueryTimeout() throws SQLException {
		return stmt.getQueryTimeout();
	}

	@Override
	public void setQueryTimeout(int seconds) throws SQLException {
		stmt.setQueryTimeout(seconds);

	}

	@Override
	public void cancel() throws SQLException {
		stmt.cancel();

	}

	@Override
	public SQLWarning getWarnings() throws SQLException {
		return stmt.getWarnings();
	}

	@Override
	public void clearWarnings() throws SQLException {
		stmt.clearWarnings();
	}

	@Override
	public void setCursorName(String name) throws SQLException {
		stmt.setCursorName(name);

	}

	@Override
	public boolean execute(String sql) throws SQLException {
		bindParameter();
		return stmt.execute(sql);
	}

	
	private void bindParameter() {
		System.out.println(paramterValues.length+"mmmmmmmmmmmmmmmmmmmmmmmmmmmmm");
		System.out.println(parameterTypes.size()+"ssssssssssssssssssssssssssssssssss");
		//System.out.println(sql+"mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMm");
//		paramterValues
//		parameterIndexList
		//for(String s:parameterTypes){
			//System.out.println(s+"BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB");
		//}
		for(int i = 0; i < paramterValues.length; i ++) {
			/*if(parameterTypes.get(i) == Types.INTEGER) {
				sql = sql.replaceFirst("[?]", paramterValues[i]);
			} else {
				sql = sql.replaceFirst("[?]", "\""+paramterValues[i]+"\"");
			}*/
			//sql = sql.replaceFirst("[?]", paramterValues[i]);
			//System.out.println(paramterValues[i]+"DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD");
			if(parameterTypes.get(i).equals("int")){
				sql = sql.replaceFirst("[?]", paramterValues[i]);
			}else if(parameterTypes.get(i).equals("string")){
				sql = sql.replaceFirst("[?]", "\""+paramterValues[i]+"\"");
			}
		}
		sql = sql.replaceAll("null","\'"+"NULL"+"\'");
	}
	@Override
	public ResultSet getResultSet() throws SQLException {
		return stmt.getResultSet();
	}

	@Override
	public int getUpdateCount() throws SQLException {
		return stmt.getUpdateCount();
	}

	@Override
	public boolean getMoreResults() throws SQLException {
		return stmt.getMoreResults();
	}

	@Override
	public void setFetchDirection(int direction) throws SQLException {
		stmt.setFetchDirection(direction);

	}

	@Override
	public int getFetchDirection() throws SQLException {
		return stmt.getFetchDirection();
	}

	@Override
	public void setFetchSize(int rows) throws SQLException {
		stmt.setFetchSize(rows);

	}

	@Override
	public int getFetchSize() throws SQLException {
		return stmt.getFetchSize();
	}

	@Override
	public int getResultSetConcurrency() throws SQLException {
		return stmt.getResultSetConcurrency();
	}

	@Override
	public int getResultSetType() throws SQLException {
		return stmt.getResultSetType();
	}

	@Override
	public void addBatch(String sql) throws SQLException {
		stmt.addBatch(sql);
	}

	@Override
	public void clearBatch() throws SQLException {
		stmt.clearBatch();
	}

	@Override
	public int[] executeBatch() throws SQLException {
		return stmt.executeBatch();
	}

	@Override
	public Connection getConnection() throws SQLException {
		return stmt.getConnection();
	}

	@Override
	public boolean getMoreResults(int current) throws SQLException {
		return stmt.getMoreResults();
	}

	@Override
	public ResultSet getGeneratedKeys() throws SQLException {
		return stmt.getGeneratedKeys();
	}

	@Override
	public int executeUpdate(String sql, int autoGeneratedKeys)
			throws SQLException {
		return stmt.executeUpdate(sql, autoGeneratedKeys);
	}

	@Override
	public int executeUpdate(String sql, int[] columnIndexes)
			throws SQLException {
		return stmt.executeUpdate(sql, columnIndexes);
	}

	@Override
	public int executeUpdate(String sql, String[] columnNames)
			throws SQLException {
		return stmt.executeUpdate(sql, columnNames);
	}

	@Override
	public boolean execute(String sql, int autoGeneratedKeys)
			throws SQLException {
		bindParameter();
		return stmt.execute(sql, autoGeneratedKeys);
	}

	@Override
	public boolean execute(String sql, int[] columnIndexes) throws SQLException {
		bindParameter();
		return stmt.execute(sql, columnIndexes);
	}

	@Override
	public boolean execute(String sql, String[] columnNames)
			throws SQLException {
		bindParameter();
		return stmt.execute(sql, columnNames);
	}

	@Override
	public int getResultSetHoldability() throws SQLException {
		return stmt.getResultSetHoldability();
	}

	@Override
	public boolean isClosed() throws SQLException {
		return stmt.isClosed();
	}

	@Override
	public void setPoolable(boolean poolable) throws SQLException {
		stmt.setPoolable(poolable);
		
	}

	@Override
	public boolean isPoolable() throws SQLException {
		return stmt.isPoolable();
	}

	public void closeOnCompletion() throws SQLException {
		stmt.closeOnCompletion();
	}

	public boolean isCloseOnCompletion() throws SQLException {
		return stmt.isCloseOnCompletion();
	}

	@Override
	public <T> T unwrap(Class<T> iface) throws SQLException {
		return stmt.unwrap(iface);
	}

	@Override
	public boolean isWrapperFor(Class<?> iface) throws SQLException {
		return stmt.isWrapperFor(iface);
	}

	@Override
	public ResultSet executeQuery() throws SQLException {
		bindParameter();
		return stmt.executeQuery(sql);
	}

	@Override
	public int executeUpdate() throws SQLException {
		return stmt.executeUpdate(sql);
	}

	@Override
	public void setNull(int parameterIndex, int sqlType) throws SQLException {
		//throw SQLError.notImplemented();
		setString(parameterIndex, String.valueOf(sqlType));
		
	}
	

	@Override
	public void setBoolean(int parameterIndex, boolean x) throws SQLException {
		//throw SQLError.notImplemented();
		setString(parameterIndex, String.valueOf(x));
	}

	@Override
	public void setByte(int parameterIndex, byte x) throws SQLException {
		// TODO Auto-generated method stub
		//throw SQLError.notImplemented();
		setInt(parameterIndex, x);
	}

	@Override
	public void setShort(int parameterIndex, short x) throws SQLException {
		// TODO Auto-generated method stub
		//throw SQLError.notImplemented();
		setInt(parameterIndex, x);
	}
//	protected void checkClosed() throws SQLException {
//		
//	}
//	protected final void setInternal(int paramIndex, String val)
//	throws SQLException {
//		checkClosed();
//		
//		byte[] parameterAsBytes = null;
//		
//		if (this.charConverter != null) {
//			parameterAsBytes = this.charConverter.toBytes(val);
//		} else {
//			parameterAsBytes = StringUtils.getBytes(val, this.charConverter,
//					this.charEncoding, this.stmt.getConnection().connection
//							.getServerCharacterEncoding(), this.connection
//							.parserKnowsUnicode());
//		}
//		
//		setInternal(paramIndex, parameterAsBytes);
//	}
//	
//	protected final void setInternal(int paramIndex, byte[] val)
//	throws SQLException {
//		/*if (this.isClosed) {
//			throw SQLError.createSQLException(Messages.getString("PreparedStatement.48"), //$NON-NLS-1$
//					SQLError.SQL_STATE_ILLEGAL_ARGUMENT);
//		}*/
//		
//		int parameterIndexOffset = getParameterIndexOffset();
//		
//		checkBounds(paramIndex, parameterIndexOffset);
//		
//		this.isStream[paramIndex - 1 + parameterIndexOffset] = false;
//		this.isNull[paramIndex - 1 + parameterIndexOffset] = false;
//		this.parameterStreams[paramIndex - 1 + parameterIndexOffset] = null;
//		this.parameterValues[paramIndex - 1 + parameterIndexOffset] = val;
//	}
	
	@Override
	public void setInt(int parameterIndex, int x) throws SQLException {
		// TODO Auto-generated method stub
		//throw SQLError.notImplemented();
//		setInternal(parameterIndex, String.valueOf(x));
		
		//this.parameterTypes[parameterIndex - 1 + getParameterIndexOffset()] = Types.INTEGER;
		//System.out.println("sql= "+sql+"/22222222222222222222222222222222222222222222222222");
		//System.out.println("x= "+x+"666666666666666666666666666666666666666666666666666666");
		this.paramterValues[parameterIndex-1] = String.valueOf(x);
		//parameterTypes.set(parameterIndex-1, Types.INTEGER);
		parameterTypes.add("int");
		
	}

	@Override
	public void setLong(int parameterIndex, long x) throws SQLException {
		// TODO Auto-generated method stub
		//throw SQLError.notImplemented();
		setInt(parameterIndex, (int)x);
	}

	@Override
	public void setFloat(int parameterIndex, float x) throws SQLException {
		// TODO Auto-generated method stub
		//throw SQLError.notImplemented();
		setString(parameterIndex, String.valueOf(x));
	}

	@Override
	public void setDouble(int parameterIndex, double x) throws SQLException {
		// TODO Auto-generated method stub
		//throw SQLError.notImplemented();
		setString(parameterIndex, String.valueOf(x));
	}

	@Override
	public void setBigDecimal(int parameterIndex, BigDecimal x)
			throws SQLException {
		// TODO Auto-generated method stub
		//throw SQLError.notImplemented();
		setString(parameterIndex, String.valueOf(x));
	}

	@Override
	public void setString(int parameterIndex, String x) throws SQLException {
		// TODO Auto-generated method stub
		System.out.println("x="+x+"3333333333333333333333333333333333333333333333");
		System.out.println("parameterIndex="+parameterIndex+"555555555555555555555555555555555");
		
		parameterTypes.add("string");
		this.paramterValues[parameterIndex-1] = x;
	}

	@Override
	public void setBytes(int parameterIndex, byte[] x) throws SQLException {
		// TODO Auto-generated method stub
		//throw SQLError.notImplemented();
		setString(parameterIndex, String.valueOf(x));
	}

	@Override
	public void setDate(int parameterIndex, Date x) throws SQLException {
		// TODO Auto-generated method stub
		//throw SQLError.notImplemented();
		setString(parameterIndex, String.valueOf(x));
	}

	@Override
	public void setTime(int parameterIndex, Time x) throws SQLException {
		// TODO Auto-generated method stub
		//throw SQLError.notImplemented();
		setString(parameterIndex, String.valueOf(x));
	}

	@Override
	public void setTimestamp(int parameterIndex, Timestamp x)
			throws SQLException {
		// TODO Auto-generated method stub
		//throw SQLError.notImplemented();
		//setString(parameterIndex, String.valueOf(x));
		 SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		 java.util.Date dateTime = new java.util.Date();
         Timestamp ts = new Timestamp(dateTime.getTime());
         //System.out.println(sdf.format(ts));
		 setString(parameterIndex,sdf.format(ts));
	}

	@Override
	public void setAsciiStream(int parameterIndex, InputStream x, int length)
			throws SQLException {
		// TODO Auto-generated method stub
		//throw SQLError.notImplemented();
		setString(parameterIndex, String.valueOf(x));
	}

	@Override
	public void setUnicodeStream(int parameterIndex, InputStream x, int length)
			throws SQLException {
		// TODO Auto-generated method stub
		//throw SQLError.notImplemented();
		setString(parameterIndex, String.valueOf(x));
	}

	@Override
	public void setBinaryStream(int parameterIndex, InputStream x, int length)
			throws SQLException {
		//throw SQLError.notImplemented();
		setString(parameterIndex, String.valueOf(x));
	}

	@Override
	public void clearParameters() throws SQLException {
		throw SQLError.notImplemented();
	}

	@Override
	public void setObject(int parameterIndex, Object x, int targetSqlType)
			throws SQLException {
		//throw SQLError.notImplemented();
		setString(parameterIndex, String.valueOf(x));
	
	}

	@Override
	public void setObject(int parameterIndex, Object parameterObj) throws SQLException {
		// TODO Auto-generated method stub
		//throw SQLError.notImplemented();
		if (parameterObj == null) {
			setNull(parameterIndex, java.sql.Types.OTHER);
		} else {
			if (parameterObj instanceof Byte) {
				setInt(parameterIndex, ((Byte) parameterObj).intValue());
			} else if (parameterObj instanceof String) {
				setString(parameterIndex, (String) parameterObj);
			} else if (parameterObj instanceof BigDecimal) {
				setBigDecimal(parameterIndex, (BigDecimal) parameterObj);
			} else if (parameterObj instanceof Short) {
				setShort(parameterIndex, ((Short) parameterObj).shortValue());
			} else if (parameterObj instanceof Integer) {
				setInt(parameterIndex, ((Integer) parameterObj).intValue());
			} else if (parameterObj instanceof Long) {
				setLong(parameterIndex, ((Long) parameterObj).longValue());
			} else if (parameterObj instanceof Float) {
				setFloat(parameterIndex, ((Float) parameterObj).floatValue());
			} else if (parameterObj instanceof Double) {
				setDouble(parameterIndex, ((Double) parameterObj).doubleValue());
			} else if (parameterObj instanceof byte[]) {
				setBytes(parameterIndex, (byte[]) parameterObj);
			} else if (parameterObj instanceof java.sql.Date) {
				setDate(parameterIndex, (java.sql.Date) parameterObj);
			} else if (parameterObj instanceof Time) {
				setTime(parameterIndex, (Time) parameterObj);
			} else if (parameterObj instanceof Timestamp) {
				setTimestamp(parameterIndex, (Timestamp) parameterObj);
			} else if (parameterObj instanceof Boolean) {
				setBoolean(parameterIndex, ((Boolean) parameterObj)
						.booleanValue());
			} else if (parameterObj instanceof InputStream) {
				setBinaryStream(parameterIndex, (InputStream) parameterObj, -1);
			} else if (parameterObj instanceof java.sql.Blob) {
				setBlob(parameterIndex, (java.sql.Blob) parameterObj);
			} else if (parameterObj instanceof java.sql.Clob) {
				setClob(parameterIndex, (java.sql.Clob) parameterObj);
			}
		}
	}

	@Override
	public boolean execute() throws SQLException {
		///////
		bindParameter();
		return stmt.execute(sql);
	}

	@Override
	public void addBatch() throws SQLException {
		stmt.addBatch(sql);

	}

	@Override
	public void setCharacterStream(int parameterIndex, Reader reader, int length)
			throws SQLException {
		throw SQLError.notImplemented();
		//setString(parameterIndex, String.valueOf(x));
	}

	@Override
	public void setRef(int parameterIndex, Ref x) throws SQLException {
		//throw SQLError.notImplemented();
		setString(parameterIndex, String.valueOf(x));

	}

	@Override
	public void setBlob(int parameterIndex, Blob x) throws SQLException {
		//throw SQLError.notImplemented();
		setString(parameterIndex, String.valueOf(x));
	}

	@Override
	public void setClob(int parameterIndex, Clob x) throws SQLException {
		//throw SQLError.notImplemented();
		setString(parameterIndex, String.valueOf(x));
	}

	@Override
	public void setArray(int parameterIndex, Array x) throws SQLException {
		//throw SQLError.notImplemented();
		setString(parameterIndex, String.valueOf(x));
	}

	@Override
	public ResultSetMetaData getMetaData() throws SQLException {
		return null;
	}

	@Override
	public void setDate(int parameterIndex, Date x, Calendar cal)
			throws SQLException {
		//throw SQLError.notImplemented();
		setString(parameterIndex, String.valueOf(x));
	}

	@Override
	public void setTime(int parameterIndex, Time x, Calendar cal)
			throws SQLException {
		//throw SQLError.notImplemented();
		setString(parameterIndex, String.valueOf(x));
	}
	
	/*
	 *modified by lina
	 */

	@Override
	public void setTimestamp(int parameterIndex, Timestamp x, Calendar cal)
			throws SQLException {
		throw SQLError.notImplemented();
		//setString(parameterIndex, String.valueOf(x));
		
	}

	@Override
	public void setNull(int parameterIndex, int sqlType, String typeName)
			throws SQLException {
		//throw SQLError.notImplemented();
		setString(parameterIndex, typeName);
	}

	@Override
	public void setURL(int parameterIndex, URL x) throws SQLException {
		//throw SQLError.notImplemented();
		setString(parameterIndex, String.valueOf(x));
	}

	@Override
	public ParameterMetaData getParameterMetaData() throws SQLException {
		return null;
	}

	@Override
	public void setRowId(int parameterIndex, RowId x) throws SQLException {
		throw SQLError.notImplemented();
	}

	@Override
	public void setNString(int parameterIndex, String value)
			throws SQLException {
		//throw SQLError.notImplemented();
		setString(parameterIndex,value);
	}

	@Override
	public void setNCharacterStream(int parameterIndex, Reader value,
			long length) throws SQLException {
		throw SQLError.notImplemented();
	}

	@Override
	public void setNClob(int parameterIndex, NClob value) throws SQLException {
		//throw SQLError.notImplemented();
		setString(parameterIndex, value.toString());
	}

	@Override
	public void setClob(int parameterIndex, Reader reader, long length)
			throws SQLException {
		throw SQLError.notImplemented();
	}

	@Override
	public void setBlob(int parameterIndex, InputStream inputStream, long length)
			throws SQLException {
		throw SQLError.notImplemented();
	}

	@Override
	public void setNClob(int parameterIndex, Reader reader, long length)
			throws SQLException {
		throw SQLError.notImplemented();
	}

	@Override
	public void setSQLXML(int parameterIndex, SQLXML xmlObject)
			throws SQLException {
		throw SQLError.notImplemented();
	}

	@Override
	public void setObject(int parameterIndex, Object parameterObj, int targetSqlType,
			int scaleOrLength) throws SQLException {
		//throw SQLError.notImplemented();
		setString(parameterIndex, String.valueOf(parameterObj));
		
	}
	
	/*private final void setSerializableObject(int parameterIndex,
			Object parameterObj) throws SQLException {
		try {
			ByteArrayOutputStream bytesOut = new ByteArrayOutputStream();
			ObjectOutputStream objectOut = new ObjectOutputStream(bytesOut);
			objectOut.writeObject(parameterObj);
			objectOut.flush();
			objectOut.close();
			bytesOut.flush();
			bytesOut.close();

			byte[] buf = bytesOut.toByteArray();
			ByteArrayInputStream bytesIn = new ByteArrayInputStream(buf);
			setBinaryStream(parameterIndex, bytesIn, buf.length);
			this.parameterTypes[parameterIndex - 1 + getParameterIndexOffset()] = Types.JAVA_OBJECT;
		} catch (Exception ex) {
			throw SQLError.createSQLException(Messages.getString("PreparedStatement.54") //$NON-NLS-1$
					+ ex.getClass().getName(),
					SQLError.SQL_STATE_ILLEGAL_ARGUMENT);
		}
	}*/

	@Override
	public void setAsciiStream(int parameterIndex, InputStream x, long length)
			throws SQLException {
		throw SQLError.notImplemented();
	}

	@Override
	public void setBinaryStream(int parameterIndex, InputStream x, long length)
			throws SQLException {
		throw SQLError.notImplemented();
	}

	@Override
	public void setCharacterStream(int parameterIndex, Reader reader,
			long length) throws SQLException {
		throw SQLError.notImplemented();
	}

	@Override
	public void setAsciiStream(int parameterIndex, InputStream x)
			throws SQLException {
		throw SQLError.notImplemented();
	}

	@Override
	public void setBinaryStream(int parameterIndex, InputStream x)
			throws SQLException {
		throw SQLError.notImplemented();
	}

	@Override
	public void setCharacterStream(int parameterIndex, Reader reader)
			throws SQLException {
		throw SQLError.notImplemented();
	}

	@Override
	public void setNCharacterStream(int parameterIndex, Reader value)
			throws SQLException {
		throw SQLError.notImplemented();
	}

	@Override
	public void setClob(int parameterIndex, Reader reader) throws SQLException {
		throw SQLError.notImplemented();
	}

	@Override
	public void setBlob(int parameterIndex, InputStream inputStream)
			throws SQLException {
		throw SQLError.notImplemented();
	}

	@Override
	public void setNClob(int parameterIndex, Reader reader) throws SQLException {
		throw SQLError.notImplemented();
	}

}
