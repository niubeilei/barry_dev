package com.zykie.jdbc.utils;

import java.sql.DataTruncation;

/**
 * Zykie wrapper for DataTruncation until the server can support sending all
 * needed information.
 * 
 * @author Mark Matthews
 * 
 * @version $Id: ZykieDataTruncation.java,v 1.1.2.1 2005/05/13 18:58:38
 *          mmatthews Exp $
 */
public class ZykieDataTruncation extends DataTruncation {

	static final long serialVersionUID = 3263928195256986226L;

	private String message;

	private int vendorErrorCode;

	/**
	 * Creates a new ZykieDataTruncation exception/warning.
	 * 
	 * @param message
	 *            the message from the server
	 * @param index
	 *            of column or parameter
	 * @param parameter
	 *            was a parameter?
	 * @param read
	 *            was truncated on read?
	 * @param dataSize
	 *            size requested
	 * @param transferSize
	 *            size actually used
	 */
	public ZykieDataTruncation(String message, int index, boolean parameter,
			boolean read, int dataSize, int transferSize, int vendorErrorCode) {
		super(index, parameter, read, dataSize, transferSize);

		this.message = message;
		this.vendorErrorCode = vendorErrorCode;
	}

	public int getErrorCode() {
		return this.vendorErrorCode;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Throwable#getMessage()
	 */
	public String getMessage() {
		return super.getMessage() + ": " + this.message; //$NON-NLS-1$
	}
}
