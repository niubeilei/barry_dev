package com.zykie.jdbc.exceptions;

import java.sql.SQLException;
import java.sql.Connection;
import java.util.Properties;

public interface Extension {

	/**
	 * Called once per connection that wants to use the extension
	 * 
	 * The properties are the same ones passed in in the URL or arguments to
	 * Driver.connect() or DriverManager.getConnection().
	 * 
	 * @param conn
	 *            the connection for which this extension is being created
	 * @param props
	 *            configuration values as passed to the connection. Note that in
	 *            order to support javax.sql.DataSources, configuration
	 *            properties specific to an interceptor <strong>must</strong> be
	 *            passed via setURL() on the DataSource. Extension properties
	 *            are not exposed via accessor/mutator methods on DataSources.
	 * 
	 * @throws SQLException
	 *             should be thrown if the the Extension can not initialize
	 *             itself.
	 */

	public abstract void init(Connection conn, Properties props)
			throws SQLException;

	/**
	 * Called by the driver when this extension should release any resources it
	 * is holding and cleanup internally before the connection is closed.
	 */
	public abstract void destroy();
}
