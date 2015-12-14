package com.zykie.jdbc.exceptions.jdbc4;

import java.net.BindException;

import java.sql.SQLRecoverableException;

import com.zykie.jdbc.Connection;
import com.zykie.jdbc.utils.SQLError;
import com.zykie.jdbc.exceptions.StreamingNotifiable;

/**
 * An exception to represent communications errors with the database.
 * 
 * Attempts to provide 'friendler' error messages to end-users, including last
 * time a packet was sent to the database, what the client-timeout is set to,
 * and whether the idle time has been exceeded.
 * 
 * @author Mark Matthews
 * 
 * @version $Id: CommunicationsException.java,v 1.1.2.1 2005/05/13 18:58:37
 *          mmatthews Exp $
 */
public class CommunicationsException extends SQLRecoverableException implements StreamingNotifiable {

	private String exceptionMessage;

	private boolean streamingResultSetInPlay = false;

	public CommunicationsException(Connection conn, long lastPacketSentTimeMs,
			long lastPacketReceivedTimeMs,
			Exception underlyingException) {

		this.exceptionMessage = SQLError.createLinkFailureMessageBasedOnHeuristics(conn,
				lastPacketSentTimeMs, lastPacketReceivedTimeMs, underlyingException, this.streamingResultSetInPlay);
		
		if (underlyingException != null) {
			initCause(underlyingException);
		}
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Throwable#getMessage()
	 */
	public String getMessage() {
		return this.exceptionMessage;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.sql.SQLException#getSQLState()
	 */
	public String getSQLState() {
		return SQLError.SQL_STATE_COMMUNICATION_LINK_FAILURE;
	}

	public void setWasStreamingResults() {
		this.streamingResultSetInPlay = true;
	}

}
