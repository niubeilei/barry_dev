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

package com.zykie.jdbc.utils;

import java.sql.Types;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

/**
 * ZykieDefs contains many values that are needed for communication with the
 * Zykie server.
 * 
 * @author Mark Matthews
 * @version $Id: ZykieDefs.java,v 1.2 2015/01/06 08:57:48 andy Exp $
 */
public final class ZykieDefs {
	// ~ public static fields/initializers
	// ---------------------------------------------

	public static final int COM_BINLOG_DUMP = 18;
	public static final int COM_CHANGE_USER = 17;
	public static final int COM_CLOSE_STATEMENT = 25;
	public static final int COM_CONNECT_OUT = 20;
	public static final int COM_END = 29;
	public static final int COM_EXECUTE = 23;
	public static final int COM_FETCH = 28;
	public static final int COM_LONG_DATA = 24;
	public static final int COM_PREPARE = 22;
	public static final int COM_REGISTER_SLAVE = 21;
	public static final int COM_RESET_STMT = 26;
	public static final int COM_SET_OPTION = 27;
	public static final int COM_TABLE_DUMP = 19;
	public static final int CONNECT = 11;
	public static final int CREATE_DB = 5;
	public static final int DEBUG = 13;
	public static final int DELAYED_INSERT = 16;
	public static final int DROP_DB = 6;
	public static final int FIELD_LIST = 4;
	
	
	/************************************
	 * field data types
	 ***********************************/
	public static final int FIELD_TYPE_BIT = 1;  //Jimo: int32
	public static final int FIELD_TYPE_BOOLEAN = 2;  //Jimo: int32	
	public static final int FIELD_TYPE_CHAR = 3;  //**Jimo: char
	public static final int FIELD_TYPE_DOUBLE = 4;  //**Jimo: double
	public static final int FIELD_TYPE_INT32 = 5;  //**Jimo: int32 && u32
	public static final int FIELD_TYPE_INT64 = 6; //**Jimo: int64 && u64
	public static final int FIELD_TYPE_U32 = 7;  //**Jimo: int32 && u32
	public static final int FIELD_TYPE_U64 = 8; //**Jimo: int64 && u64
	public static final int FIELD_TYPE_CHARSTRING = 9;  //**Jimo: charstring
	
	//not used data types for now
	public static final int FIELD_TYPE_BLOB = 58;  //Binary large object
	public static final int FIELD_TYPE_DATE = 59;  
	public static final int FIELD_TYPE_DATETIME = 60; 
	public static final int FIELD_TYPE_DECIMAL = 61;
	public static final int FIELD_TYPE_ENUM = 62; 
	public static final int FIELD_TYPE_FLOAT = 63;
	public static final int FIELD_TYPE_GEOMETRY = 64;
	public static final int FIELD_TYPE_INT24 = 65;  //
	public static final int FIELD_TYPE_LONG_BLOB = 66;
	public static final int FIELD_TYPE_MEDIUM_BLOB = 67;
	public static final int FIELD_TYPE_NEW_DECIMAL = 68;
	public static final int FIELD_TYPE_NEWDATE = 69;
	public static final int FIELD_TYPE_NULL = 70;
	public static final int FIELD_TYPE_SET = 71;
	public static final int FIELD_TYPE_SHORT = 72;   //Jimo: future
	public static final int FIELD_TYPE_TIME = 73;   //Jimo: future
	public static final int FIELD_TYPE_TIMESTAMP = 74;
	public static final int FIELD_TYPE_TINY = 75;
	
	//new types for big types
	public static final int FIELD_TYPE_YEAR = 101;
	public static final int INIT_DB = 102;
	public static final long LENGTH_BLOB = 65535;
	public static final long LENGTH_LONGBLOB = 4294967295L;
	public static final long LENGTH_MEDIUMBLOB = 16777215;
	public static final long LENGTH_TINYBLOB = 255;

	// Limitations
	public static final int MAX_ROWS = 50000000; // From the Zykie FAQ

	/**
	 * Used to indicate that the server sent no field-level character set
	 * information, so the driver should use the connection-level character
	 * encoding instead.
	 */
	public static final int NO_CHARSET_INFO = -1;
	public static final byte OPEN_CURSOR_FLAG = 1;
	public static final int PING = 14;
	public static final int PROCESS_INFO = 10;
	public static final int PROCESS_KILL = 12;
	public static final int QUERY = 3;
	public static final int QUIT = 1;
	public static final int RELOAD = 7;
	public static final int SHUTDOWN = 8;
	public static final int SLEEP = 0;
	public static final int STATISTICS = 9;
	public static final int TIME = 15;

	/**
	 * Maps the given Zykie type to the correct JDBC type.
	 */
	public static int zykieToJavaType(int zykieType) {
		int jdbcType;

		switch (zykieType) {
		case ZykieDefs.FIELD_TYPE_NEW_DECIMAL:
		case ZykieDefs.FIELD_TYPE_DECIMAL:
			jdbcType = Types.DECIMAL;

			break;

		case ZykieDefs.FIELD_TYPE_TINY:
			jdbcType = Types.TINYINT;

			break;

		case ZykieDefs.FIELD_TYPE_SHORT:
			jdbcType = Types.SMALLINT;

			break;

		case ZykieDefs.FIELD_TYPE_FLOAT:
			jdbcType = Types.REAL;

			break;

		case ZykieDefs.FIELD_TYPE_NULL:
			jdbcType = Types.NULL;

			break;

		case ZykieDefs.FIELD_TYPE_TIMESTAMP:
			jdbcType = Types.TIMESTAMP;

			break;
		
		case ZykieDefs.FIELD_TYPE_INT24:
			jdbcType = Types.INTEGER;

			break;

		case ZykieDefs.FIELD_TYPE_DATE:
			jdbcType = Types.DATE;

			break;

		case ZykieDefs.FIELD_TYPE_TIME:
			jdbcType = Types.TIME;

			break;

		case ZykieDefs.FIELD_TYPE_DATETIME:
			jdbcType = Types.TIMESTAMP;

			break;

		case ZykieDefs.FIELD_TYPE_NEWDATE:
			jdbcType = Types.DATE;

			break;

		case ZykieDefs.FIELD_TYPE_ENUM:
			jdbcType = Types.CHAR;

			break;

		case ZykieDefs.FIELD_TYPE_SET:
			jdbcType = Types.CHAR;

			break;

		case ZykieDefs.FIELD_TYPE_MEDIUM_BLOB:
			jdbcType = Types.LONGVARBINARY;

			break;

		case ZykieDefs.FIELD_TYPE_LONG_BLOB:
			jdbcType = Types.LONGVARBINARY;

			break;

		case ZykieDefs.FIELD_TYPE_BLOB:
			jdbcType = Types.LONGVARBINARY;

			break;

		case ZykieDefs.FIELD_TYPE_GEOMETRY:
			jdbcType = Types.BINARY;

			break;

			/**
			 * The following are for real zykie db types
			 */
		case ZykieDefs.FIELD_TYPE_CHAR:
			jdbcType = Types.CHAR;
			break;

		case ZykieDefs.FIELD_TYPE_CHARSTRING:
			jdbcType = Types.CHAR;
			break;

		case ZykieDefs.FIELD_TYPE_DOUBLE:
			jdbcType = Types.DOUBLE;
			break;

		case ZykieDefs.FIELD_TYPE_INT32:
			jdbcType = Types.INTEGER;
			break;

		case ZykieDefs.FIELD_TYPE_INT64:
			jdbcType = Types.BIGINT;
			break;

		case ZykieDefs.FIELD_TYPE_U32:
			jdbcType = Types.BIGINT;
			break;

		case ZykieDefs.FIELD_TYPE_U64:
			jdbcType = Types.BIGINT;
			break;

		case ZykieDefs.FIELD_TYPE_BIT:
		case ZykieDefs.FIELD_TYPE_BOOLEAN:
			jdbcType = Types.BIT;
			break;
			
		default:
			jdbcType = Types.VARCHAR;
		}

		return jdbcType;
	}

	/**
	 * Maps the given Zykie type to the correct JDBC type.
	 */
	public static int zykieToJavaType(String zykieType) {
		if (zykieType.equalsIgnoreCase("TINYINT")) { //$NON-NLS-1$
			return zykieToJavaType(FIELD_TYPE_TINY);
		} else if (zykieType.equalsIgnoreCase("SMALLINT")) { //$NON-NLS-1$
			return zykieToJavaType(FIELD_TYPE_SHORT);
		} else if (zykieType.equalsIgnoreCase("MEDIUMINT")) { //$NON-NLS-1$
			return zykieToJavaType(FIELD_TYPE_INT24);
		} else if (zykieType.equalsIgnoreCase("INT24")) { //$NON-NLS-1$
			return zykieToJavaType(FIELD_TYPE_INT24);
		} else if (zykieType.equalsIgnoreCase("REAL")) { //$NON-NLS-1$
			return zykieToJavaType(FIELD_TYPE_DOUBLE);
		} else if (zykieType.equalsIgnoreCase("FLOAT")) { //$NON-NLS-1$
			return zykieToJavaType(FIELD_TYPE_FLOAT);
		} else if (zykieType.equalsIgnoreCase("DECIMAL")) { //$NON-NLS-1$
			return zykieToJavaType(FIELD_TYPE_DECIMAL);
		} else if (zykieType.equalsIgnoreCase("NUMERIC")) { //$NON-NLS-1$
			return zykieToJavaType(FIELD_TYPE_DECIMAL);
		} else if (zykieType.equalsIgnoreCase("DATE")) { //$NON-NLS-1$
			return zykieToJavaType(FIELD_TYPE_DATE);
		} else if (zykieType.equalsIgnoreCase("TIME")) { //$NON-NLS-1$
			return zykieToJavaType(FIELD_TYPE_TIME);
		} else if (zykieType.equalsIgnoreCase("TIMESTAMP")) { //$NON-NLS-1$
			return zykieToJavaType(FIELD_TYPE_TIMESTAMP);
		} else if (zykieType.equalsIgnoreCase("DATETIME")) { //$NON-NLS-1$
			return zykieToJavaType(FIELD_TYPE_DATETIME);
		} else if (zykieType.equalsIgnoreCase("TINYBLOB")) { //$NON-NLS-1$
			return java.sql.Types.BINARY;
		} else if (zykieType.equalsIgnoreCase("BLOB")) { //$NON-NLS-1$
			return java.sql.Types.LONGVARBINARY;
		} else if (zykieType.equalsIgnoreCase("MEDIUMBLOB")) { //$NON-NLS-1$
			return java.sql.Types.LONGVARBINARY;
		} else if (zykieType.equalsIgnoreCase("LONGBLOB")) { //$NON-NLS-1$
			return java.sql.Types.LONGVARBINARY;
		} else if (zykieType.equalsIgnoreCase("TINYTEXT")) { //$NON-NLS-1$
			return java.sql.Types.VARCHAR;
		} else if (zykieType.equalsIgnoreCase("TEXT")) { //$NON-NLS-1$
			return java.sql.Types.LONGVARCHAR;
		} else if (zykieType.equalsIgnoreCase("MEDIUMTEXT")) { //$NON-NLS-1$
			return java.sql.Types.LONGVARCHAR;
		} else if (zykieType.equalsIgnoreCase("LONGTEXT")) { //$NON-NLS-1$
			return java.sql.Types.LONGVARCHAR;
		} else if (zykieType.equalsIgnoreCase("ENUM")) { //$NON-NLS-1$
			return zykieToJavaType(FIELD_TYPE_ENUM);
		} else if (zykieType.equalsIgnoreCase("SET")) { //$NON-NLS-1$
			return zykieToJavaType(FIELD_TYPE_SET);
		} else if (zykieType.equalsIgnoreCase("GEOMETRY")) {
			return zykieToJavaType(FIELD_TYPE_GEOMETRY);
		} else if (zykieType.equalsIgnoreCase("BINARY")) {
			return Types.BINARY; // no concrete type on the wire
		} else if (zykieType.equalsIgnoreCase("VARBINARY")) {
			return Types.VARBINARY; // no concrete type on the wire
		}  
		
		/***the following types are current supported Zykie types*****/
		if (zykieType.equalsIgnoreCase("BIT")) {
			return zykieToJavaType(FIELD_TYPE_BIT);
		} else if (zykieType.equalsIgnoreCase("BOOL")) {
			return zykieToJavaType(FIELD_TYPE_BOOLEAN);
		} else if (zykieType.equalsIgnoreCase("CHAR")) {
			return zykieToJavaType(FIELD_TYPE_CHAR);
		} else if (zykieType.equalsIgnoreCase("CHARSTRING")) {
			return zykieToJavaType(FIELD_TYPE_CHARSTRING);
		} else if (zykieType.equalsIgnoreCase("DOUBLE")) {
			return zykieToJavaType(FIELD_TYPE_DOUBLE);
		} else if (zykieType.equalsIgnoreCase("INT32")) {
			return zykieToJavaType(FIELD_TYPE_INT32);
		} else if (zykieType.equalsIgnoreCase("INT64")) {
			return zykieToJavaType(FIELD_TYPE_INT64);
		} else if (zykieType.equalsIgnoreCase("U32")) {
			return zykieToJavaType(FIELD_TYPE_U32);
		} else if (zykieType.equalsIgnoreCase("U64")) {
			return zykieToJavaType(FIELD_TYPE_U64);
		} 

		// Punt
		return java.sql.Types.OTHER;
	}

	/**
	 * @param zykieType
	 * @return
	 */
	public static String typeToName(int zykieType) {
		switch (zykieType) {
		case ZykieDefs.FIELD_TYPE_DECIMAL:
			return "DECIMAL";

		case ZykieDefs.FIELD_TYPE_TINY:
			return "TINYINT";

		case ZykieDefs.FIELD_TYPE_SHORT:
			return "SHORT";

		case ZykieDefs.FIELD_TYPE_FLOAT:
			return "FLOAT";

		case ZykieDefs.FIELD_TYPE_NULL:
			return "NULL";

		case ZykieDefs.FIELD_TYPE_TIMESTAMP:
			return "TIMESTAMP";

		case ZykieDefs.FIELD_TYPE_INT24:
			return "INT24";

		case ZykieDefs.FIELD_TYPE_DATE:
			return "DATE";

		case ZykieDefs.FIELD_TYPE_TIME:
			return "TIME";

		case ZykieDefs.FIELD_TYPE_DATETIME:
			return "DATETIME";

		case ZykieDefs.FIELD_TYPE_NEWDATE:
			return "NEWDATE";

		case ZykieDefs.FIELD_TYPE_ENUM:
			return "ENUM";

		case ZykieDefs.FIELD_TYPE_SET:
			return "SET";

		case ZykieDefs.FIELD_TYPE_MEDIUM_BLOB:
			return "MEDIUM_BLOB";

		case ZykieDefs.FIELD_TYPE_LONG_BLOB:
			return "LONG_BLOB";

		case ZykieDefs.FIELD_TYPE_BLOB:
			return "BLOB";

		case ZykieDefs.FIELD_TYPE_GEOMETRY:
			return "GEOMETRY";

			/*
			 * The following are real zykie types
			 */
		case ZykieDefs.FIELD_TYPE_BIT:
			return "BIT";
			
		case ZykieDefs.FIELD_TYPE_BOOLEAN:
			return "BOOLEAN";
			
		case ZykieDefs.FIELD_TYPE_CHAR:
			return "CHAR";
			
		case ZykieDefs.FIELD_TYPE_CHARSTRING:
			return "CHARSTRING";

		case ZykieDefs.FIELD_TYPE_DOUBLE:
			return "DOUBLE";

		case ZykieDefs.FIELD_TYPE_INT32:
			return "INT32";

		case ZykieDefs.FIELD_TYPE_INT64:
			return "INT64";

		case ZykieDefs.FIELD_TYPE_U32:
			return "U32";

		case ZykieDefs.FIELD_TYPE_U64:
			return "U64";

		default:
			return " Unknown Zykie Type # " + zykieType;
		}
	}

	private static Map<String, Integer> zykieToJdbcTypesMap = new HashMap<String, Integer>();
	static {
		zykieToJdbcTypesMap.put("TINYINT", Integer.valueOf(
				zykieToJavaType(FIELD_TYPE_TINY)));
		zykieToJdbcTypesMap.put("SMALLINT", Integer.valueOf(
				zykieToJavaType(FIELD_TYPE_SHORT)));
		zykieToJdbcTypesMap.put("MEDIUMINT", Integer.valueOf(
				zykieToJavaType(FIELD_TYPE_INT24)));
		zykieToJdbcTypesMap.put("INT", Integer.valueOf(
				zykieToJavaType(FIELD_TYPE_INT32)));
		zykieToJdbcTypesMap.put("INT24", Integer.valueOf(
				zykieToJavaType(FIELD_TYPE_INT24)));
		zykieToJdbcTypesMap.put("REAL", Integer.valueOf(
				zykieToJavaType(FIELD_TYPE_DOUBLE)));
		zykieToJdbcTypesMap.put("FLOAT", Integer.valueOf(
				zykieToJavaType(FIELD_TYPE_FLOAT)));
		zykieToJdbcTypesMap.put("DECIMAL", Integer.valueOf(
				zykieToJavaType(FIELD_TYPE_DECIMAL)));
		zykieToJdbcTypesMap.put("NUMERIC", Integer.valueOf(
				zykieToJavaType(FIELD_TYPE_DECIMAL)));
		zykieToJdbcTypesMap.put("DATE", Integer.valueOf(
				zykieToJavaType(FIELD_TYPE_DATE)));
		zykieToJdbcTypesMap.put("TIME", Integer.valueOf(
				zykieToJavaType(FIELD_TYPE_TIME)));
		zykieToJdbcTypesMap.put("TIMESTAMP", Integer.valueOf(
				zykieToJavaType(FIELD_TYPE_TIMESTAMP)));
		zykieToJdbcTypesMap.put("DATETIME", Integer.valueOf(
				zykieToJavaType(FIELD_TYPE_DATETIME)));
		zykieToJdbcTypesMap.put("TINYBLOB", Integer.valueOf(java.sql.Types.BINARY));
		zykieToJdbcTypesMap.put("BLOB", Integer.valueOf(
				java.sql.Types.LONGVARBINARY));
		zykieToJdbcTypesMap.put("MEDIUMBLOB", Integer.valueOf(
				java.sql.Types.LONGVARBINARY));
		zykieToJdbcTypesMap.put("LONGBLOB", Integer.valueOf(
				java.sql.Types.LONGVARBINARY));
		zykieToJdbcTypesMap
				.put("TINYTEXT", Integer.valueOf(java.sql.Types.VARCHAR));
		zykieToJdbcTypesMap
				.put("TEXT", Integer.valueOf(java.sql.Types.LONGVARCHAR));
		zykieToJdbcTypesMap.put("MEDIUMTEXT", Integer.valueOf(
				java.sql.Types.LONGVARCHAR));
		zykieToJdbcTypesMap.put("LONGTEXT", Integer.valueOf(
				java.sql.Types.LONGVARCHAR));
		zykieToJdbcTypesMap.put("ENUM", Integer.valueOf(
				zykieToJavaType(FIELD_TYPE_ENUM)));
		zykieToJdbcTypesMap.put("SET", Integer.valueOf(
				zykieToJavaType(FIELD_TYPE_SET)));
		zykieToJdbcTypesMap.put("GEOMETRY", Integer.valueOf(
				zykieToJavaType(FIELD_TYPE_GEOMETRY)));

		/*********************************
		 *   Zykie types into Map
		 *********************************/
		zykieToJdbcTypesMap.put("BIT", Integer.valueOf(
				zykieToJavaType(FIELD_TYPE_BIT)));
		zykieToJdbcTypesMap.put("BOOLEAN", Integer.valueOf(
				zykieToJavaType(FIELD_TYPE_BOOLEAN)));
		zykieToJdbcTypesMap.put("DOUBLE", Integer.valueOf(
				zykieToJavaType(FIELD_TYPE_DOUBLE)));
		zykieToJdbcTypesMap.put("CHAR", Integer.valueOf(
				zykieToJavaType(FIELD_TYPE_CHAR)));
		zykieToJdbcTypesMap.put("CHARSTRING", Integer.valueOf(
				zykieToJavaType(FIELD_TYPE_CHARSTRING)));
		zykieToJdbcTypesMap.put("INT32", Integer.valueOf(
				zykieToJavaType(FIELD_TYPE_INT32)));
		zykieToJdbcTypesMap.put("INT64", Integer.valueOf(
				zykieToJavaType(FIELD_TYPE_INT64)));
		zykieToJdbcTypesMap.put("U32", Integer.valueOf(
				zykieToJavaType(FIELD_TYPE_U32)));
		zykieToJdbcTypesMap.put("U64", Integer.valueOf(
				zykieToJavaType(FIELD_TYPE_U64)));
	}

	public static final void appendJdbcTypeMappingQuery(StringBuffer buf, String zykieTypeColumnName) {

		buf.append("CASE ");
		Map<String, Integer> typesMap = new HashMap<String, Integer>();
		typesMap.putAll(zykieToJdbcTypesMap);
		typesMap.put("BINARY", Integer.valueOf(Types.BINARY));
		typesMap.put("VARBINARY", Integer.valueOf(Types.VARBINARY));
		
		Iterator<String> zykieTypes = typesMap.keySet().iterator();
		
		while (zykieTypes.hasNext()) {
			String zykieTypeName = zykieTypes.next();
			buf.append(" WHEN ");
			buf.append(zykieTypeColumnName);
			buf.append("='");
			buf.append(zykieTypeName);
			buf.append("' THEN ");
			buf.append(typesMap.get(zykieTypeName));
			
			if (zykieTypeName.equalsIgnoreCase("DOUBLE") ||
					zykieTypeName.equalsIgnoreCase("FLOAT") ||
					zykieTypeName.equalsIgnoreCase("DECIMAL") ||
					zykieTypeName.equalsIgnoreCase("NUMERIC")) {
				buf.append(" WHEN ");
				buf.append(zykieTypeColumnName);
				buf.append("='");
				buf.append(zykieTypeName);
				buf.append(" unsigned' THEN ");
				buf.append(typesMap.get(zykieTypeName));	
			}	
		}
		
		buf.append(" ELSE ");
		buf.append(Types.OTHER);
		buf.append(" END ");
		
	}
}
