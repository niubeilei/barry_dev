package com.zykie.jdbc.exceptions;

import com.zykie.jdbc.utils.Messages;
import com.zykie.jdbc.utils.SQLError;

/**
 * Thrown from methods not required to be implemented.
 * 
 * @author Mark Matthews
 */
public class NotImplemented extends java.sql.SQLException {

	static final long serialVersionUID = 7768433826547599990L;

	// ~ Constructors
	// -----------------------------------------------------------

	/**
	 * Creates a new NotImplemented object.
	 */
	public NotImplemented() {
		super(
				Messages.getString("NotImplemented.0"), SQLError.SQL_STATE_DRIVER_NOT_CAPABLE); //$NON-NLS-1$
		Thread.dumpStack();
	}
}
