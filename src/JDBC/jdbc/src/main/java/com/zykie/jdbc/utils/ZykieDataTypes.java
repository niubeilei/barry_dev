package com.zykie.jdbc.utils;

import java.sql.SQLException;

public class ZykieDataTypes {

	/**
	 * Parses and represents common data type information used by various
	 * column/parameter methods.
	 */
	int bufferLength;
	int charOctetLength;
	Integer columnSize;
	short dataType;
	Integer decimalDigits;
	String isNullable;
	int nullability;
	int numPrecRadix = 10;
	String typeName;

	ZykieDataTypes(String typeInfo, String nullabilityInfo) throws SQLException {
		if (typeInfo == null) {
			throw SQLError.createSQLException("NULL typeinfo not supported.",
					SQLError.SQL_STATE_ILLEGAL_ARGUMENT, null);
		}

		boolean isUnsigned = false;

		if (typeInfo.toUpperCase().charAt(0) == 'U')
			isUnsigned = true;

		this.dataType = (short) ZykieDefs.zykieToJavaType(typeInfo);
		this.typeName = typeInfo;

		// Figure Out the Size
		if (StringUtils.startsWithIgnoreCase(typeInfo, "enum")) {
			String temp = typeInfo.substring(typeInfo.indexOf("("),
					typeInfo.lastIndexOf(")"));
			java.util.StringTokenizer tokenizer = new java.util.StringTokenizer(
					temp, ",");
			int maxLength = 0;

			while (tokenizer.hasMoreTokens()) {
				maxLength = Math.max(maxLength,
						(tokenizer.nextToken().length() - 2));
			}

			this.columnSize = Integer.valueOf(maxLength);
			this.decimalDigits = null;
		} else if (StringUtils.startsWithIgnoreCase(typeInfo, "set")) {
			String temp = typeInfo.substring(typeInfo.indexOf("(") + 1,
					typeInfo.lastIndexOf(")"));
			java.util.StringTokenizer tokenizer = new java.util.StringTokenizer(
					temp, ",");
			int maxLength = 0;

			int numElements = tokenizer.countTokens();

			if (numElements > 0) {
				maxLength += (numElements - 1);
			}

			while (tokenizer.hasMoreTokens()) {
				String setMember = tokenizer.nextToken().trim();

				if (setMember.startsWith("'") && setMember.endsWith("'")) {
					maxLength += setMember.length() - 2;
				} else {
					maxLength += setMember.length();
				}
			}

			this.columnSize = Integer.valueOf(maxLength);
			this.decimalDigits = null;
		} else if (typeInfo.indexOf(",") != -1) {
			// Numeric with decimals
			this.columnSize = Integer.valueOf(typeInfo.substring(
					(typeInfo.indexOf("(") + 1), (typeInfo.indexOf(",")))
					.trim());
			this.decimalDigits = Integer.valueOf(typeInfo.substring(
					(typeInfo.indexOf(",") + 1), (typeInfo.indexOf(")")))
					.trim());
		} else {
			this.columnSize = null;
			this.decimalDigits = null;

			/* If the size is specified with the DDL, use that */
			if ((StringUtils.indexOfIgnoreCase(typeInfo, "char") != -1
					|| StringUtils.indexOfIgnoreCase(typeInfo, "charstring") != -1
					|| StringUtils.indexOfIgnoreCase(typeInfo, "text") != -1
					|| StringUtils.indexOfIgnoreCase(typeInfo, "blob") != -1
					|| StringUtils.indexOfIgnoreCase(typeInfo, "binary") != -1 || StringUtils
					.indexOfIgnoreCase(typeInfo, "bit") != -1)
					&& typeInfo.indexOf("(") != -1) {
				int endParenIndex = typeInfo.indexOf(")");

				if (endParenIndex == -1) {
					endParenIndex = typeInfo.length();
				}

				this.columnSize = Integer.valueOf(typeInfo.substring(
						(typeInfo.indexOf("(") + 1), endParenIndex).trim());

			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo,
					"smallint")) {
				this.columnSize = Integer.valueOf(5);
				this.decimalDigits = Integer.valueOf(0);
			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo,
					"mediumint")) {
				this.columnSize = Integer.valueOf(isUnsigned ? 8 : 7);
				this.decimalDigits = Integer.valueOf(0);
			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo, "int")) {
				this.columnSize = Integer.valueOf(10);
				this.decimalDigits = Integer.valueOf(0);
			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo,
					"integer")) {
				this.columnSize = Integer.valueOf(10);
				this.decimalDigits = Integer.valueOf(0);
			} else if (StringUtils
					.startsWithIgnoreCaseAndWs(typeInfo, "bigint")) {
				this.columnSize = Integer.valueOf(isUnsigned ? 20 : 19);
				this.decimalDigits = Integer.valueOf(0);
			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo, "int24")) {
				this.columnSize = Integer.valueOf(19);
				this.decimalDigits = Integer.valueOf(0);
			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo, "real")) {
				this.columnSize = Integer.valueOf(12);
			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo, "float")) {
				this.columnSize = Integer.valueOf(12);
			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo,
					"decimal")) {
				this.columnSize = Integer.valueOf(12);
			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo,
					"numeric")) {
				this.columnSize = Integer.valueOf(12);
			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo,
					"varchar")) {
				this.columnSize = Integer.valueOf(255);
			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo,
					"timestamp")) {
				this.columnSize = Integer.valueOf(19);
			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo,
					"datetime")) {
				this.columnSize = Integer.valueOf(19);
			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo, "date")) {
				this.columnSize = Integer.valueOf(10);
			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo, "time")) {
				this.columnSize = Integer.valueOf(8);
			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo,
					"tinyblob")) {
				this.columnSize = Integer.valueOf(255);
			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo, "blob")) {
				this.columnSize = Integer.valueOf(65535);
			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo,
					"mediumblob")) {
				this.columnSize = Integer.valueOf(16777215);
			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo,
					"longblob")) {
				this.columnSize = Integer.valueOf(Integer.MAX_VALUE);
			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo,
					"tinytext")) {
				this.columnSize = Integer.valueOf(255);
			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo, "text")) {
				this.columnSize = Integer.valueOf(65535);
			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo,
					"mediumtext")) {
				this.columnSize = Integer.valueOf(16777215);
			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo,
					"longtext")) {
				this.columnSize = Integer.valueOf(Integer.MAX_VALUE);
			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo, "enum")) {
				this.columnSize = Integer.valueOf(255);
			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo, "set")) {
				this.columnSize = Integer.valueOf(255);
			}

			// the following is for real zykie types
			if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo, "int32") ||
				StringUtils.startsWithIgnoreCaseAndWs(typeInfo, "u32")) {
				this.columnSize = Integer.valueOf(10);
				this.decimalDigits = Integer.valueOf(0);
			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo, "int64") || 
					   StringUtils.startsWithIgnoreCaseAndWs(typeInfo, "u64")) {
				this.columnSize = Integer.valueOf(20);
				this.decimalDigits = Integer.valueOf(0);
			} else if (StringUtils
					.startsWithIgnoreCaseAndWs(typeInfo, "double")) {
				this.columnSize = Integer.valueOf(22);
			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo, "char")) {
				this.columnSize = Integer.valueOf(1);
			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo, "charstring")) {
				this.columnSize = Integer.valueOf(32);
			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo, "bit")) {
				this.columnSize = Integer.valueOf(1);
			} else if (StringUtils.startsWithIgnoreCaseAndWs(typeInfo, "boolean")) {
				this.columnSize = Integer.valueOf(5);
			} 
		}

		// BUFFER_LENGTH
		this.bufferLength = 8192;

		// NUM_PREC_RADIX (is this right for char?)
		this.numPrecRadix = 10;

		// Nullable?
		if (nullabilityInfo != null) {
			if (nullabilityInfo.equals("YES")) {
				this.nullability = java.sql.DatabaseMetaData.columnNullable;
				this.isNullable = "YES";

			} else if (nullabilityInfo.equals("UNKNOWN")) {
				this.nullability = java.sql.DatabaseMetaData.columnNullableUnknown;
				this.isNullable = "";

				// IS_NULLABLE
			} else {
				this.nullability = java.sql.DatabaseMetaData.columnNoNulls;
				this.isNullable = "NO";
			}
		} else {
			this.nullability = java.sql.DatabaseMetaData.columnNoNulls;
			this.isNullable = "NO";
		}
	}
}
