package com.zykie.jdbc.utils;

import java.sql.SQLException;
import java.util.Properties;

/**
 * Implement this interface, and pass the class name as the
 * 'propertiesTransform' property in your JDBC URL, and the driver will pass the
 * properties it has parsed to your transform implementation so that you can
 * modify/substitute/add any that you desire.
 * 
 * @author Mark Matthews
 * 
 * @version $Id: ConnectionPropertiesTransform.java,v 1.1.2.1 2005/05/13
 *          18:58:37 mmatthews Exp $
 */
public interface ConnectionPropertiesTransform {
	/**
	 * The JDBC driver will call this method if the user has loaded your
	 * implementation of this interface by specifying the 'propertiesTransform'
	 * property in their JDBC URL.
	 * 
	 * @param props
	 *            the properties as passed by the driver (never null)
	 * 
	 * @return the same properties with any transformations that your
	 *         implementation has made
	 * 
	 * @throws SQLException
	 *             if a transform can not be made for any reason.
	 */
	public Properties transformProperties(Properties props) throws SQLException;
}
