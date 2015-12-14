/*
  Copyright (c) 2002, 2014, Oracle and/or its affiliates. All rights reserved.

  The Zykie Connector/J is licensed under the terms of the GPLv2
  <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most Zykie Connectors.
  There are special exceptions to the terms and conditions of the GPLv2 as it is applied to
  this software, see the FLOSS License Exception
  <http://www.zykie.com/about/legal/licensing/foss-exception.html>.

  This program is free software; you can redistribute it and/or modify it under the terms
  of the GNU General Public License as published by the Free Software Foundation; version 2
  of the License.

  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with this
  program; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth
  Floor, Boston, MA 02110-1301  USA

 */

package com.zykie.jdbc;

import java.io.UnsupportedEncodingException;
import java.sql.SQLException;
import java.sql.Types;
import java.util.regex.PatternSyntaxException;

import com.zykie.jdbc.utils.ZykieDefs;
import com.zykie.jdbc.utils.Messages;
import com.zykie.jdbc.utils.SQLError;
import com.zykie.jdbc.utils.SingleByteCharsetConverter;
import com.zykie.jdbc.utils.StringUtils;
import com.zykie.jdbc.utils.CharsetMapping;

/**
 * Field is a class used to describe fields in a ResultSet
 * 
 * @author Mark Matthews
 * @version $Id: Field.java,v 1.2 2015/01/06 08:57:48 andy Exp $
 */
public class Field {

	private static final int AUTO_INCREMENT_FLAG = 512;

	private static final int NO_CHARSET_INFO = -1;

	private byte[] buffer;

	private int charsetIndex = 0;

	private String charsetName = null;

	private int colDecimals;

	private short colFlag;

	private String collationName = null;

	private Connection connection = null;

	private String databaseName = null;

	private int databaseNameLength = -1;

	// database name info
	private int databaseNameStart = -1;

	protected int defaultValueLength = -1;

	// default value info - from COM_LIST_FIELDS execution
	protected int defaultValueStart = -1;

	private String fullName = null;

	private String fullOriginalName = null;

	private boolean isImplicitTempTable = false;

	private long length; // Internal length of the field;

	private int zykieType = -1; // the Zykie type

	private String name; // The Field name

	private int nameLength;

	private int nameStart;

	private String originalColumnName = null;

	private int originalColumnNameLength = -1;

	// column name info (before aliasing)
	private int originalColumnNameStart = -1;

	private String originalTableName = null;

	private int originalTableNameLength = -1;

	// table name info (before aliasing)
	private int originalTableNameStart = -1;

	private int precisionAdjustFactor = 0;

	private int sqlType = -1; // the java.sql.Type

	private String tableName; // The Name of the Table

	private int tableNameLength;

	private int tableNameStart;

	private boolean useOldNameMetadata = false;

	private boolean isSingleBit;

	private int maxBytesPerChar;

	private final boolean valueNeedsQuoting;

	/**
	 * Constructor used when communicating with 4.1 and newer servers
	 */
	Field(Connection conn, byte[] buffer, int databaseNameStart,
			int databaseNameLength, int tableNameStart, int tableNameLength,
			int originalTableNameStart, int originalTableNameLength,
			int nameStart, int nameLength, int originalColumnNameStart,
			int originalColumnNameLength, long length, int zykieType,
			short colFlag, int colDecimals, int defaultValueStart,
			int defaultValueLength, int charsetIndex) throws SQLException {
		this.connection = conn;
		this.buffer = buffer;
		this.nameStart = nameStart;
		this.nameLength = nameLength;
		this.tableNameStart = tableNameStart;
		this.tableNameLength = tableNameLength;
		this.length = length;
		this.colFlag = colFlag;
		this.colDecimals = colDecimals;
		this.zykieType = zykieType;

		// 4.1 field info...
		this.databaseNameStart = databaseNameStart;
		this.databaseNameLength = databaseNameLength;

		this.originalTableNameStart = originalTableNameStart;
		this.originalTableNameLength = originalTableNameLength;

		this.originalColumnNameStart = originalColumnNameStart;
		this.originalColumnNameLength = originalColumnNameLength;

		this.defaultValueStart = defaultValueStart;
		this.defaultValueLength = defaultValueLength;

		// If we're not running 4.1 or newer, use the connection's
		// charset
		this.charsetIndex = charsetIndex;

		// Map ZykieTypes to java.sql Types
		this.sqlType = ZykieDefs.zykieToJavaType(this.zykieType);

		checkForImplicitTemporaryTable();
		// Re-map to 'real' blob type, if we're a BLOB
		boolean isFromFunction = this.originalTableNameLength == 0;

		if (this.zykieType == ZykieDefs.FIELD_TYPE_BLOB) {
			// *TEXT masquerading as blob
			this.zykieType = ZykieDefs.FIELD_TYPE_CHARSTRING;
			this.sqlType = Types.LONGVARCHAR;
		}

		if (this.sqlType == Types.TINYINT && this.length == 1)
			this.sqlType = Types.BIT;

		if (!isNativeNumericType() && !isNativeDateTimeType()) {
			// ucs2, utf16, and utf32 cannot be used as a client character set,
			// but if it was received from server under some circumstances
			// we can parse them as utf16
			this.charsetName = "UTF-16";

			// Handle VARBINARY/BINARY (server doesn't have a different type
			// for this
			boolean isBinary = isBinary();

			if (this.zykieType == ZykieDefs.FIELD_TYPE_CHARSTRING && isBinary
					&& this.charsetIndex == 63) {
				//
				// Okay, this is a hack, but there's currently no way
				// to easily distinguish something like DATE_FORMAT( ..)
				// from the "BINARY" column type, other than looking
				// at the original column name.
				//

				if (isOpaqueBinary())
					this.sqlType = Types.BINARY;
			}

			if (this.zykieType == ZykieDefs.FIELD_TYPE_BIT) {
				this.isSingleBit = (this.length == 0);

				if (this.length == 1)
					this.isSingleBit = true;

				if (this.isSingleBit) {
					this.sqlType = Types.BIT;
				} else {
					this.sqlType = Types.VARBINARY;
					this.colFlag |= 128; // we need to pretend this is a full
					this.colFlag |= 16; // binary blob
					isBinary = true;
				}
			}

			//
			// Handle TEXT type (special case), Fix proposed by Peter McKeown
			//
			if ((this.sqlType == java.sql.Types.LONGVARBINARY) && !isBinary) {
				this.sqlType = java.sql.Types.LONGVARCHAR;
			} else if ((this.sqlType == java.sql.Types.VARBINARY) && !isBinary) {
				this.sqlType = java.sql.Types.VARCHAR;
			}
		} else {
			this.charsetName = "US-ASCII";
		}

		//
		// Handle odd values for 'M' for floating point/decimal numbers
		//
		if (!isUnsigned()) {
			switch (this.zykieType) {
			case ZykieDefs.FIELD_TYPE_DECIMAL:
			case ZykieDefs.FIELD_TYPE_NEW_DECIMAL:
				this.precisionAdjustFactor = -1;

				break;
			case ZykieDefs.FIELD_TYPE_DOUBLE:
			case ZykieDefs.FIELD_TYPE_FLOAT:
				this.precisionAdjustFactor = 1;

				break;
			}
		} else {
			switch (this.zykieType) {
			case ZykieDefs.FIELD_TYPE_DOUBLE:
			case ZykieDefs.FIELD_TYPE_FLOAT:
				this.precisionAdjustFactor = 1;

				break;
			}
		}
		this.valueNeedsQuoting = determineNeedsQuoting();
	}

	/**
	 * Constructor used when communicating with pre 4.1 servers
	 */
	Field(Connection conn, byte[] buffer, int nameStart, int nameLength,
			int tableNameStart, int tableNameLength, int length, int zykieType,
			short colFlag, int colDecimals) throws SQLException {
		this(conn, buffer, -1, -1, tableNameStart, tableNameLength, -1, -1,
				nameStart, nameLength, -1, -1, length, zykieType, colFlag,
				colDecimals, -1, -1, NO_CHARSET_INFO);
	}

	/**
	 * Constructor used by DatabaseMetaData methods.
	 */
	Field(String tableName, String columnName, int jdbcType, int length) {
		this.tableName = tableName;
		this.name = columnName;
		this.length = length;
		this.sqlType = jdbcType;
		this.colFlag = 0;
		this.colDecimals = 0;
		this.valueNeedsQuoting = determineNeedsQuoting();
	}

	/**
	 * Used by prepared statements to re-use result set data conversion methods
	 * when generating bound parmeter retrieval instance for statement
	 * interceptors.
	 * 
	 * @param tableName
	 *            not used
	 * @param columnName
	 *            not used
	 * @param charsetIndex
	 *            the Zykie collation/character set index
	 * @param jdbcType
	 *            from java.sql.Types
	 * @param length
	 *            length in characters or bytes (for BINARY data).
	 */
	Field(String tableName, String columnName, int charsetIndex, int jdbcType,
			int length) {
		this.tableName = tableName;
		this.name = columnName;
		this.length = length;
		this.sqlType = jdbcType;
		this.colFlag = 0;
		this.colDecimals = 0;
		this.charsetIndex = charsetIndex;
		this.valueNeedsQuoting = determineNeedsQuoting();

		switch (this.sqlType) {
		case Types.BINARY:
		case Types.VARBINARY:
			this.colFlag |= 128;
			this.colFlag |= 16;
			break;
		}
	}

	private void checkForImplicitTemporaryTable() {
		this.isImplicitTempTable = this.tableNameLength > 5
				&& this.buffer[tableNameStart] == (byte) '#'
				&& this.buffer[tableNameStart + 1] == (byte) 's'
				&& this.buffer[tableNameStart + 2] == (byte) 'q'
				&& this.buffer[tableNameStart + 3] == (byte) 'l'
				&& this.buffer[tableNameStart + 4] == (byte) '_';
	}

	/**
	 * Returns the character set (if known) for this field.
	 * 
	 * @return the character set
	 */
	public String getCharacterSet() throws SQLException {
		return this.charsetName;
	}

	public void setCharacterSet(String javaEncodingName) throws SQLException {
		this.charsetName = javaEncodingName;
		try {
			this.charsetIndex = CharsetMapping
					.getCharsetIndexForZykieEncodingName(javaEncodingName);
		} catch (RuntimeException ex) {
			SQLException sqlEx = SQLError.createSQLException(ex.toString(),
					SQLError.SQL_STATE_ILLEGAL_ARGUMENT, null);
			sqlEx.initCause(ex);
			throw sqlEx;
		}
	}

	public synchronized String getCollation() throws SQLException {
		if (this.collationName == null) {
			if (this.connection != null) {
				java.sql.DatabaseMetaData dbmd = this.connection.getMetaData();

				String quotedIdStr = dbmd.getIdentifierQuoteString();

				if (" ".equals(quotedIdStr)) { //$NON-NLS-1$
					quotedIdStr = ""; //$NON-NLS-1$
				}

				String csCatalogName = getDatabaseName();
				String csTableName = getOriginalTableName();
				String csColumnName = getOriginalName();

				if (csCatalogName != null && csCatalogName.length() != 0
						&& csTableName != null && csTableName.length() != 0
						&& csColumnName != null && csColumnName.length() != 0) {
					StringBuffer queryBuf = new StringBuffer(
							csCatalogName.length() + csTableName.length() + 28);
					queryBuf.append("SHOW FULL COLUMNS FROM "); //$NON-NLS-1$
					queryBuf.append(quotedIdStr);
					queryBuf.append(csCatalogName);
					queryBuf.append(quotedIdStr);
					queryBuf.append("."); //$NON-NLS-1$
					queryBuf.append(quotedIdStr);
					queryBuf.append(csTableName);
					queryBuf.append(quotedIdStr);

					java.sql.Statement collationStmt = null;
					java.sql.ResultSet collationRs = null;

					try {
						collationStmt = this.connection.createStatement();

						collationRs = collationStmt.executeQuery(queryBuf
								.toString());

						while (collationRs.next()) {
							if (csColumnName.equals(collationRs
									.getString("Field"))) { //$NON-NLS-1$
								this.collationName = collationRs
										.getString("Collation"); //$NON-NLS-1$

								break;
							}
						}
					} finally {
						if (collationRs != null) {
							collationRs.close();
							collationRs = null;
						}

						if (collationStmt != null) {
							collationStmt.close();
							collationStmt = null;
						}
					}
				}
			} else {
				try {
					this.collationName = CharsetMapping.INDEX_TO_COLLATION[charsetIndex];
				} catch (RuntimeException ex) {
					SQLException sqlEx = SQLError.createSQLException(
							ex.toString(), SQLError.SQL_STATE_ILLEGAL_ARGUMENT,
							null);
					sqlEx.initCause(ex);
					throw sqlEx;
				}
			}
		}

		return this.collationName;
	}

	public String getColumnLabel() throws SQLException {
		return getName(); // column name if not aliased, alias if used
	}

	/**
	 * DOCUMENT ME!
	 * 
	 * @return DOCUMENT ME!
	 */
	public String getDatabaseName() throws SQLException {
		if ((this.databaseName == null) && (this.databaseNameStart != -1)
				&& (this.databaseNameLength != -1)) {
			this.databaseName = getStringFromBytes(this.databaseNameStart,
					this.databaseNameLength);
		}

		return this.databaseName;
	}

	int getDecimals() {
		return this.colDecimals;
	}

	/**
	 * DOCUMENT ME!
	 * 
	 * @return DOCUMENT ME!
	 */
	public String getFullName() throws SQLException {
		if (this.fullName == null) {
			StringBuffer fullNameBuf = new StringBuffer(getTableName().length()
					+ 1 + getName().length());
			fullNameBuf.append(this.tableName);

			// much faster to append a char than a String
			fullNameBuf.append('.');
			fullNameBuf.append(this.name);
			this.fullName = fullNameBuf.toString();
			fullNameBuf = null;
		}

		return this.fullName;
	}

	/**
	 * DOCUMENT ME!
	 * 
	 * @return DOCUMENT ME!
	 */
	public String getFullOriginalName() throws SQLException {
		getOriginalName();

		if (this.originalColumnName == null) {
			return null; // we don't have this information
		}

		if (this.fullName == null) {
			StringBuffer fullOriginalNameBuf = new StringBuffer(
					getOriginalTableName().length() + 1
							+ getOriginalName().length());
			fullOriginalNameBuf.append(this.originalTableName);

			// much faster to append a char than a String
			fullOriginalNameBuf.append('.');
			fullOriginalNameBuf.append(this.originalColumnName);
			this.fullOriginalName = fullOriginalNameBuf.toString();
			fullOriginalNameBuf = null;
		}

		return this.fullOriginalName;
	}

	/**
	 * DOCUMENT ME!
	 * 
	 * @return DOCUMENT ME!
	 */
	public long getLength() {
		return this.length;
	}

	/**
	 * DOCUMENT ME!
	 * 
	 * @return DOCUMENT ME!
	 */
	public int getZykieType() {
		return this.zykieType;
	}

	/**
	 * DOCUMENT ME!
	 * 
	 * @return DOCUMENT ME!
	 */
	public String getName() throws SQLException {
		if (this.name == null) {
			this.name = getStringFromBytes(this.nameStart, this.nameLength);
		}

		return this.name;
	}

	public String getNameNoAliases() throws SQLException {
		if (this.useOldNameMetadata) {
			return getName();
		}

		if (this.connection != null) {
			return getOriginalName();
		}

		return getName();
	}

	/**
	 * DOCUMENT ME!
	 * 
	 * @return DOCUMENT ME!
	 */
	public String getOriginalName() throws SQLException {
		if ((this.originalColumnName == null)
				&& (this.originalColumnNameStart != -1)
				&& (this.originalColumnNameLength != -1)) {
			this.originalColumnName = getStringFromBytes(
					this.originalColumnNameStart, this.originalColumnNameLength);
		}

		return this.originalColumnName;
	}

	/**
	 * DOCUMENT ME!
	 * 
	 * @return DOCUMENT ME!
	 */
	public String getOriginalTableName() throws SQLException {
		if ((this.originalTableName == null)
				&& (this.originalTableNameStart != -1)
				&& (this.originalTableNameLength != -1)) {
			this.originalTableName = getStringFromBytes(
					this.originalTableNameStart, this.originalTableNameLength);
		}

		return this.originalTableName;
	}

	/**
	 * Returns amount of correction that should be applied to the precision
	 * value.
	 * 
	 * Different versions of Zykie report different precision values.
	 * 
	 * @return the amount to adjust precision value by.
	 */
	public int getPrecisionAdjustFactor() {
		return this.precisionAdjustFactor;
	}

	/**
	 * DOCUMENT ME!
	 * 
	 * @return DOCUMENT ME!
	 */
	public int getSQLType() {
		return this.sqlType;
	}

	/**
	 * Create a string with the correct charset encoding from the byte-buffer
	 * that contains the data for this field
	 */
	private String getStringFromBytes(int stringStart, int stringLength)
			throws SQLException {
		if ((stringStart == -1) || (stringLength == -1)) {
			return null;
		}

		String stringVal = null;

		if (this.connection != null) {
			if (this.connection.getUseUnicode()) {
				String encoding = null;
				if (encoding == null) {
					encoding = connection.getEncoding();
				}

				if (encoding != null) {
					SingleByteCharsetConverter converter = null;

					if (this.connection != null) {
						converter = this.connection
								.getCharsetConverter(encoding);
					}

					if (converter != null) { // we have a converter
						stringVal = converter.toString(this.buffer,
								stringStart, stringLength);
					} else {
						// we have no converter, use JVM converter
						try {
							stringVal = StringUtils.toString(buffer,
									stringStart, stringLength, encoding);
						} catch (UnsupportedEncodingException ue) {
							throw new RuntimeException(
									Messages.getString("Field.12") + encoding //$NON-NLS-1$
											+ Messages.getString("Field.13")); //$NON-NLS-1$
						}
					}
				} else {
					// we have no encoding, use JVM standard charset
					stringVal = StringUtils.toAsciiString(this.buffer,
							stringStart, stringLength);
				}
			} else {
				// we are not using unicode, so use JVM standard charset
				stringVal = StringUtils.toAsciiString(this.buffer, stringStart,
						stringLength);
			}
		} else {
			// we don't have a connection, so punt
			stringVal = StringUtils.toAsciiString(this.buffer, stringStart,
					stringLength);
		}

		return stringVal;
	}

	/**
	 * DOCUMENT ME!
	 * 
	 * @return DOCUMENT ME!
	 */
	public String getTable() throws SQLException {
		return getTableName();
	}

	/**
	 * DOCUMENT ME!
	 * 
	 * @return DOCUMENT ME!
	 */
	public String getTableName() throws SQLException {
		if (this.tableName == null) {
			this.tableName = getStringFromBytes(this.tableNameStart,
					this.tableNameLength);
		}

		return this.tableName;
	}

	public String getTableNameNoAliases() throws SQLException {
		return getOriginalTableName();
	}

	/**
	 * DOCUMENT ME!
	 * 
	 * @return DOCUMENT ME!
	 */
	public boolean isAutoIncrement() {
		return ((this.colFlag & AUTO_INCREMENT_FLAG) > 0);
	}

	/**
	 * DOCUMENT ME!
	 * 
	 * @return DOCUMENT ME!
	 */
	public boolean isBinary() {
		return ((this.colFlag & 128) > 0);
	}

	/**
	 * DOCUMENT ME!
	 * 
	 * @return DOCUMENT ME!
	 */
	public boolean isBlob() {
		return ((this.colFlag & 16) > 0);
	}

	/**
	 * Is this field owned by a server-created temporary table?
	 * 
	 * @return
	 */
	private boolean isImplicitTemporaryTable() {
		return this.isImplicitTempTable;
	}

	/**
	 * DOCUMENT ME!
	 * 
	 * @return DOCUMENT ME!
	 */
	public boolean isMultipleKey() {
		return ((this.colFlag & 8) > 0);
	}

	boolean isNotNull() {
		return ((this.colFlag & 1) > 0);
	}

	boolean isOpaqueBinary() throws SQLException {

		//
		// Detect CHAR(n) CHARACTER SET BINARY which is a synonym for
		// fixed-length binary types
		//

		if (this.charsetIndex == 63
				&& isBinary()
				&& (this.getZykieType() == ZykieDefs.FIELD_TYPE_CHARSTRING) ) {

			if (this.originalTableNameLength == 0 && this.connection != null) {
				return false; // Probably from function
			}

			// Okay, queries resolved by temp tables also have this 'signature',
			// check for that

			return !isImplicitTemporaryTable();
		}

		return ("binary".equalsIgnoreCase(getCharacterSet()));

	}

	/**
	 * DOCUMENT ME!
	 * 
	 * @return DOCUMENT ME!
	 */
	public boolean isPrimaryKey() {
		return ((this.colFlag & 2) > 0);
	}

	/**
	 * Is this field _definitely_ not writable?
	 * 
	 * @return true if this field can not be written to in an INSERT/UPDATE
	 *         statement.
	 */
	boolean isReadOnly() throws SQLException {
		String orgColumnName = getOriginalName();
		String orgTableName = getOriginalTableName();

		return !(orgColumnName != null && orgColumnName.length() > 0
				&& orgTableName != null && orgTableName.length() > 0);
	}

	/**
	 * DOCUMENT ME!
	 * 
	 * @return DOCUMENT ME!
	 */
	public boolean isUniqueKey() {
		return ((this.colFlag & 4) > 0);
	}

	/**
	 * DOCUMENT ME!
	 * 
	 * @return DOCUMENT ME!
	 */
	public boolean isUnsigned() {
		return ((this.colFlag & 32) > 0);
	}

	public void setUnsigned() {
		this.colFlag |= 32;
	}

	/**
	 * DOCUMENT ME!
	 * 
	 * @return DOCUMENT ME!
	 */
	public boolean isZeroFill() {
		return ((this.colFlag & 64) > 0);
	}

	//
	// Zykie only has one protocol-level BLOB type that it exposes
	// which is FIELD_TYPE_BLOB, although we can divine what the
	// actual type is by the length reported ...
	//
	private void setBlobTypeBasedOnLength() {
		if (this.length == ZykieDefs.LENGTH_BLOB) {
			this.zykieType = ZykieDefs.FIELD_TYPE_BLOB;
		} else if (this.length == ZykieDefs.LENGTH_MEDIUMBLOB) {
			this.zykieType = ZykieDefs.FIELD_TYPE_MEDIUM_BLOB;
		} else if (this.length == ZykieDefs.LENGTH_LONGBLOB) {
			this.zykieType = ZykieDefs.FIELD_TYPE_LONG_BLOB;
		}
	}

	private boolean isNativeNumericType() {
		return ((this.zykieType >= ZykieDefs.FIELD_TYPE_TINY && this.zykieType <= ZykieDefs.FIELD_TYPE_DOUBLE)
				|| this.zykieType == ZykieDefs.FIELD_TYPE_U64 || this.zykieType == ZykieDefs.FIELD_TYPE_YEAR);
	}

	private boolean isNativeDateTimeType() {
		return (this.zykieType == ZykieDefs.FIELD_TYPE_DATE
				|| this.zykieType == ZykieDefs.FIELD_TYPE_NEWDATE
				|| this.zykieType == ZykieDefs.FIELD_TYPE_DATETIME
				|| this.zykieType == ZykieDefs.FIELD_TYPE_TIME || this.zykieType == ZykieDefs.FIELD_TYPE_TIMESTAMP);
	}

	/**
	 * DOCUMENT ME!
	 * 
	 * @param conn
	 *            DOCUMENT ME!
	 */
	public void setConnection(Connection conn) {
		this.connection = conn;

		if (this.charsetName == null || this.charsetIndex == 0) {
			this.charsetName = this.connection.getEncoding();
		}
	}

	void setZykieType(int type) {
		this.zykieType = type;
		this.sqlType = ZykieDefs.zykieToJavaType(this.zykieType);
	}

	protected void setUseOldNameMetadata(boolean useOldNameMetadata) {
		this.useOldNameMetadata = useOldNameMetadata;
	}

	public String toString() {
		try {
			StringBuffer asString = new StringBuffer();
			asString.append(super.toString());
			asString.append("[");
			asString.append("catalog=");
			asString.append(this.getDatabaseName());
			asString.append(",tableName=");
			asString.append(this.getTableName());
			asString.append(",originalTableName=");
			asString.append(this.getOriginalTableName());
			asString.append(",columnName=");
			asString.append(this.getName());
			asString.append(",originalColumnName=");
			asString.append(this.getOriginalName());
			asString.append(",zykieType=");
			asString.append(getZykieType());
			asString.append("(");
			asString.append(ZykieDefs.typeToName(getZykieType()));
			asString.append(")");
			asString.append(",flags=");

			if (isAutoIncrement()) {
				asString.append(" AUTO_INCREMENT");
			}

			if (isPrimaryKey()) {
				asString.append(" PRIMARY_KEY");
			}

			if (isUniqueKey()) {
				asString.append(" UNIQUE_KEY");
			}

			if (isBinary()) {
				asString.append(" BINARY");
			}

			if (isBlob()) {
				asString.append(" BLOB");
			}

			if (isMultipleKey()) {
				asString.append(" MULTI_KEY");
			}

			if (isUnsigned()) {
				asString.append(" UNSIGNED");
			}

			if (isZeroFill()) {
				asString.append(" ZEROFILL");
			}

			asString.append(", charsetIndex=");
			asString.append(this.charsetIndex);
			asString.append(", charsetName=");
			asString.append(this.charsetName);

			// if (this.buffer != null) {
			// asString.append("\n\nData as received from server:\n\n");
			// asString.append(StringUtils.dumpAsHex(this.buffer,
			// this.buffer.length));
			// }

			asString.append("]");

			return asString.toString();
		} catch (Throwable t) {
			return super.toString();
		}
	}

	protected boolean isSingleBit() {
		return this.isSingleBit;
	}

	protected boolean getvalueNeedsQuoting() {
		return this.valueNeedsQuoting;
	}

	private boolean determineNeedsQuoting() {
		boolean retVal = false;

		switch (this.sqlType) {
		case Types.BIGINT:
		case Types.BIT:
		case Types.DECIMAL:
		case Types.DOUBLE:
		case Types.FLOAT:
		case Types.INTEGER:
		case Types.NUMERIC:
		case Types.REAL:
		case Types.SMALLINT:
		case Types.TINYINT:
			retVal = false;
			break;
		default:
			retVal = true;
		}
		return retVal;

	}
}
