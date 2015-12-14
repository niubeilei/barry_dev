/*
  Copyright (c) 2008, 2014, Zykie Inc. All rights reserved.
 */

package com.zykie.jdbc;

import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.lang.ref.PhantomReference;
import java.lang.ref.ReferenceQueue;
import java.net.URLDecoder;
import java.sql.DriverPropertyInfo;
import java.sql.SQLException;
import java.sql.SQLFeatureNotSupportedException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Locale;
import java.util.Properties;
import java.util.StringTokenizer;
import java.util.concurrent.ConcurrentHashMap;
import java.util.logging.Logger;

//import org.slf4j.Logger;
//import org.slf4j.LoggerFactory;
//import org.slf4j.MDC;

import com.zykie.jdbc.utils.SQLError;
import com.zykie.jdbc.utils.StringUtils;
import com.zykie.jdbc.utils.Messages;
import com.zykie.jdbc.utils.ZykieLogger;
import com.zykie.jdbc.utils.ConnectionPropertiesImpl;
import com.zykie.jdbc.utils.ConnectionPropertiesTransform;

/**
 * The Java SQL framework allows for multiple database drivers. Each driver
 * should supply a class that implements the Driver interface
 * 
 * <p>
 * The DriverManager will try to load as many drivers as it can find and then
 * for any given connection request, it will ask each driver in turn to try to
 * connect to the target URL.
 * 
 * <p>
 * It is strongly recommended that each Driver class should be small and
 * standalone so that the Driver class can be loaded and queried without
 * bringing in vast quantities of supporting code.
 * 
 * <p>
 * When a Driver class is loaded, it should create an instance of itself and
 * register it with the DriverManager. This means that a user can load and
 * register a driver by doing Class.forName("foo.bah.Driver")
 */
public class Driver implements java.sql.Driver {

	private static final String ALLOWED_QUOTES = "\"'";
	private static final String REPLICATION_URL_PREFIX = "jdbc:zykie:replication://";
	private static final String URL_PREFIX = "jdbc:zykie://";
	private static final String MXJ_URL_PREFIX = "jdbc:zykie:mxj://";
	public static final String LOADBALANCE_URL_PREFIX = "jdbc:zykie:loadbalance://";

	// protected static final ConcurrentHashMap<ConnectionPhantomReference,
	// ConnectionPhantomReference> connectionPhantomRefs = new
	// ConcurrentHashMap<ConnectionPhantomReference,
	// ConnectionPhantomReference>();

	protected static final ReferenceQueue<Connection> refQueue = new ReferenceQueue<Connection>();

	public static final String OS = getOSName();
	public static final String PLATFORM = getPlatform();
	public static final String LICENSE = "@ZYKIE_CJ_LICENSE_TYPE@";
	public static final String RUNTIME_VENDOR = System
			.getProperty("java.vendor");
	public static final String RUNTIME_VERSION = System
			.getProperty("java.version");
	public static final String VERSION = "@ZYKIE_CJ_VERSION@";
	public static final String NAME = "@ZYKIE_CJ_DISPLAY_PROD_NAME@";

	/**
	 * Key used to retreive the database value from the properties instance
	 * passed to the driver.
	 */
	public static final String DBNAME_PROPERTY_KEY = "DBNAME";

	/** Should the driver generate debugging output? */
	public static final boolean DEBUG = false;

	/** Index for hostname coming out of parseHostPortPair(). */
	public final static int HOST_NAME_INDEX = 0;

	/**
	 * Key used to retreive the hostname value from the properties instance
	 * passed to the driver.
	 */
	public static final String HOST_PROPERTY_KEY = "HOST";
	public static final String NUM_HOSTS_PROPERTY_KEY = "NUM_HOSTS";

	/**
	 * Key used to retrieve the password value from the properties instance
	 * passed to the driver.
	 */
	public static final String PASSWORD_PROPERTY_KEY = "password";

	/** Index for port # coming out of parseHostPortPair(). */
	public final static int PORT_NUMBER_INDEX = 1;

	/**
	 * Key used to retreive the port number value from the properties instance
	 * passed to the driver.
	 */
	public static final String PORT_PROPERTY_KEY = "PORT";
	public static final String PORT_PROPERTY_DEFAULT_VALUE = "9000";
	public static final String PROPERTIES_TRANSFORM_KEY = "propertiesTransform";

	/** Should the driver generate method-call traces? */
	public static final boolean TRACE = false;
	public static final String USE_CONFIG_PROPERTY_KEY = "useConfigs";

	/**
	 * Key used to retreive the username value from the properties instance
	 * passed to the driver.
	 */
	public static final String USER_PROPERTY_KEY = "user";
	public static final String PROTOCOL_PROPERTY_KEY = "PROTOCOL";
	public static final String PATH_PROPERTY_KEY = "PATH";

	/**
	 * logging system
	 */
	// private static final Logger log = LoggerFactory
	// .getLogger(com.zykie.jdbc.Driver.class);
	private static final Logger log = ZykieLogger
			.getLogger("com.zykie.jdbc.Driver");

	// ~ Static fields/initializers
	// ---------------------------------------------

	//
	// Register ourselves with the DriverManager
	//
	static {
		try {
			java.sql.DriverManager.registerDriver(new Driver());
		} catch (SQLException E) {
			throw new RuntimeException("Can't register driver!");
		}
	}

	// ~ Constructors
	// -----------------------------------------------------------

	/**
	 * Construct a new driver and register it with DriverManager
	 * 
	 * @throws SQLException
	 *             if a database error occurs.
	 */
	public Driver() throws SQLException {
		// Required for Class.forName().newInstance()
		// set up logging system
		log.info("Zykie driver loaded.");
	}

	@Override
	public Connection connect(String url, Properties info) throws SQLException {
		Properties props = null;

		log.info("enter zykie connect.");
		try {
			if ((props = parseURL(url, info)) == null) {
				return null;
			}
		} catch (Exception e) {
			e.printStackTrace();
		}

		try {

			log.info("Connect with JimoDB " + " server:" + host(props)
					+ " port:" + port(props) + " database:" + database(props)
					+ " user:" + property("user", props) + " passwd:"
					+ property(PASSWORD_PROPERTY_KEY, props));

			Connection newConn = com.zykie.jdbc.Connection.getInstance(
					host(props), port(props), props, database(props), url);

			return newConn;
		} catch (SQLException sqlEx) {
			// Don't wrap SQLExceptions, throw
			// them un-changed.
			throw sqlEx;
		} catch (Exception ex) {
			SQLException sqlEx = SQLError.createSQLException(
					Messages.getString("NonRegisteringDriver.17") //$NON-NLS-1$
							+ ex.toString()
							+ Messages.getString("NonRegisteringDriver.18"), //$NON-NLS-1$
					SQLError.SQL_STATE_UNABLE_TO_CONNECT_TO_DATASOURCE, null);

			sqlEx.initCause(ex);

			throw sqlEx;
		}
	}

	@Override
	public boolean acceptsURL(String url) throws SQLException {
		// TODO Auto-generated method stub
		return (parseURL(url, null) != null);
	}

	@Override
	public int getMajorVersion() {
		return 1;
	}

	@Override
	public int getMinorVersion() {
		return 0;
	}

	@Override
	public boolean jdbcCompliant() {
		// TODO Auto-generated method stub
		return false;
	}

	public java.util.logging.Logger getParentLogger()
			throws SQLFeatureNotSupportedException {
		// TODO Auto-generated method stub
		return null;
	}

	/***********************************************************************
	 ***************** Help methods ****************************
	 ***********************************************************************/
	/*
	 * Standardizes OS name information to align with other drivers/clients for
	 * zykie connection attributes
	 * 
	 * @return the transformed, standardized OS name
	 */
	public static String getOSName() {
		return System.getProperty("os.name");
	}

	/*
	 * Standardizes platform information to align with other drivers/clients for
	 * zykie connection attributes
	 * 
	 * @return the transformed, standardized platform details
	 */
	public static String getPlatform() {
		return System.getProperty("os.arch");
	}

	/**
	 * Parses hostPortPair in the form of [host][:port] into an array, with the
	 * element of index HOST_NAME_INDEX being the host (or null if not
	 * specified), and the element of index PORT_NUMBER_INDEX being the port (or
	 * null if not specified).
	 * 
	 * @param hostPortPair
	 *            host and port in form of of [host][:port]
	 * 
	 * @return array containing host and port as Strings
	 * 
	 * @throws SQLException
	 *             if a parse error occurs
	 */
	protected static String[] parseHostPortPair(String hostPortPair)
			throws SQLException {

		String[] splitValues = new String[2];

		if (StringUtils.startsWithIgnoreCaseAndWs(hostPortPair, "address")) {
			splitValues[HOST_NAME_INDEX] = hostPortPair.trim();
			splitValues[PORT_NUMBER_INDEX] = null;

			return splitValues;
		}

		int portIndex = hostPortPair.indexOf(":"); //$NON-NLS-1$

		String hostname = null;

		if (portIndex != -1) {
			if ((portIndex + 1) < hostPortPair.length()) {
				String portAsString = hostPortPair.substring(portIndex + 1);
				hostname = hostPortPair.substring(0, portIndex);

				splitValues[HOST_NAME_INDEX] = hostname;

				splitValues[PORT_NUMBER_INDEX] = portAsString;
			} else {
				throw SQLError.createSQLException(
						Messages.getString("NonRegisteringDriver.37"), //$NON-NLS-1$
						SQLError.SQL_STATE_INVALID_CONNECTION_ATTRIBUTE, null);
			}
		} else {
			splitValues[HOST_NAME_INDEX] = hostPortPair;
			splitValues[PORT_NUMBER_INDEX] = null;
		}

		return splitValues;
	}

	public Properties parseURL(String url, Properties defaults)
			throws java.sql.SQLException {
		Properties urlProps = (defaults != null) ? new Properties(defaults)
				: new Properties();

		log.info("url is: " + url);
		log.info("properties is: " + defaults);
		if (url == null) {
			return null;
		}

		if (!StringUtils.startsWithIgnoreCase(url, URL_PREFIX)
				&& !StringUtils.startsWithIgnoreCase(url, MXJ_URL_PREFIX)
				&& !StringUtils.startsWithIgnoreCase(url,
						LOADBALANCE_URL_PREFIX)
				&& !StringUtils.startsWithIgnoreCase(url,
						REPLICATION_URL_PREFIX)) { //$NON-NLS-1$

			return null;
		}

		int beginningOfSlashes = url.indexOf("//");

		/*
		 * Parse parameters after the ? in the URL and remove them from the
		 * original URL.
		 */
		int index = url.indexOf("?"); //$NON-NLS-1$

		if (index != -1) {
			String paramString = url.substring(index + 1, url.length());
			url = url.substring(0, index);

			StringTokenizer queryParams = new StringTokenizer(paramString, "&"); //$NON-NLS-1$

			while (queryParams.hasMoreTokens()) {
				String parameterValuePair = queryParams.nextToken();

				int indexOfEquals = StringUtils.indexOfIgnoreCase(0,
						parameterValuePair, "=");

				String parameter = null;
				String value = null;

				if (indexOfEquals != -1) {
					parameter = parameterValuePair.substring(0, indexOfEquals);

					if (indexOfEquals + 1 < parameterValuePair.length()) {
						value = parameterValuePair.substring(indexOfEquals + 1);
					}
				}

				if ((value != null && value.length() > 0)
						&& (parameter != null && parameter.length() > 0)) {
					try {
						urlProps.put(parameter,
								URLDecoder.decode(value, "UTF-8"));
					} catch (UnsupportedEncodingException badEncoding) {
						// punt
						urlProps.put(parameter, URLDecoder.decode(value));
					} catch (NoSuchMethodError nsme) {
						// punt again
						urlProps.put(parameter, URLDecoder.decode(value));
					}
				}
			}
		}

		url = url.substring(beginningOfSlashes + 2);

		String hostStuff = null;

		int slashIndex = StringUtils.indexOfIgnoreCaseRespectMarker(0, url,
				"/", ALLOWED_QUOTES, ALLOWED_QUOTES, true); //$NON-NLS-1$

		if (slashIndex != -1) {
			hostStuff = url.substring(0, slashIndex);

			if ((slashIndex + 1) < url.length()) {
				urlProps.put(DBNAME_PROPERTY_KEY, //$NON-NLS-1$
						url.substring((slashIndex + 1), url.length()));
			}
		} else {
			hostStuff = url;
		}

		int numHosts = 0;

		if ((hostStuff != null) && (hostStuff.trim().length() > 0)) {
			List<String> hosts = StringUtils.split(hostStuff, ",",
					ALLOWED_QUOTES, ALLOWED_QUOTES, false);

			for (String hostAndPort : hosts) {
				numHosts++;

				String[] hostPortPair = parseHostPortPair(hostAndPort);

				if (hostPortPair[HOST_NAME_INDEX] != null
						&& hostPortPair[HOST_NAME_INDEX].trim().length() > 0) {
					urlProps.setProperty(HOST_PROPERTY_KEY + "." + numHosts,
							hostPortPair[HOST_NAME_INDEX]);
				} else {
					urlProps.setProperty(HOST_PROPERTY_KEY + "." + numHosts,
							"localhost");
				}

				if (hostPortPair[PORT_NUMBER_INDEX] != null) {
					urlProps.setProperty(PORT_PROPERTY_KEY + "." + numHosts,
							hostPortPair[PORT_NUMBER_INDEX]);
				} else {
					urlProps.setProperty(PORT_PROPERTY_KEY + "." + numHosts,
							PORT_PROPERTY_DEFAULT_VALUE);
				}
			}
		} else {
			numHosts = 1;
			urlProps.setProperty(HOST_PROPERTY_KEY + ".1", "localhost");
			urlProps.setProperty(PORT_PROPERTY_KEY + ".1", PORT_PROPERTY_DEFAULT_VALUE);
		}

		urlProps.setProperty(NUM_HOSTS_PROPERTY_KEY, String.valueOf(numHosts));
		urlProps.setProperty(HOST_PROPERTY_KEY,
				urlProps.getProperty(HOST_PROPERTY_KEY + ".1"));
		urlProps.setProperty(PORT_PROPERTY_KEY,
				urlProps.getProperty(PORT_PROPERTY_KEY + ".1"));

		String propertiesTransformClassName = urlProps
				.getProperty(PROPERTIES_TRANSFORM_KEY);

		if (propertiesTransformClassName != null) {
			try {
				ConnectionPropertiesTransform propTransformer = (ConnectionPropertiesTransform) Class
						.forName(propertiesTransformClassName).newInstance();

				urlProps = propTransformer.transformProperties(urlProps);
			} catch (InstantiationException e) {
				throw SQLError.createSQLException(
						"Unable to create properties transform instance '"
								+ propertiesTransformClassName
								+ "' due to underlying exception: "
								+ e.toString(),
						SQLError.SQL_STATE_INVALID_CONNECTION_ATTRIBUTE, null);
			} catch (IllegalAccessException e) {
				throw SQLError.createSQLException(
						"Unable to create properties transform instance '"
								+ propertiesTransformClassName
								+ "' due to underlying exception: "
								+ e.toString(),
						SQLError.SQL_STATE_INVALID_CONNECTION_ATTRIBUTE, null);
			} catch (ClassNotFoundException e) {
				throw SQLError.createSQLException(
						"Unable to create properties transform instance '"
								+ propertiesTransformClassName
								+ "' due to underlying exception: "
								+ e.toString(),
						SQLError.SQL_STATE_INVALID_CONNECTION_ATTRIBUTE, null);
			}
		}

		// If we use a config, it actually should get overridden by anything in
		// the URL or passed-in properties

		String configNames = null;

		if (defaults != null) {
			configNames = defaults.getProperty(USE_CONFIG_PROPERTY_KEY);
		}

		if (configNames == null) {
			configNames = urlProps.getProperty(USE_CONFIG_PROPERTY_KEY);
		}

		if (configNames != null) {
			List<String> splitNames = StringUtils.split(configNames, ",", true);

			Properties configProps = new Properties();

			Iterator<String> namesIter = splitNames.iterator();

			while (namesIter.hasNext()) {
				String configName = namesIter.next();

				try {
					InputStream configAsStream = getClass()
							.getResourceAsStream(
									"configs/" + configName + ".properties");

					if (configAsStream == null) {
						throw SQLError
								.createSQLException(
										"Can't find configuration template named '"
												+ configName + "'",
										SQLError.SQL_STATE_INVALID_CONNECTION_ATTRIBUTE,
										null);
					}
					configProps.load(configAsStream);
				} catch (IOException ioEx) {
					SQLException sqlEx = SQLError.createSQLException(
							"Unable to load configuration template '"
									+ configName
									+ "' due to underlying IOException: "
									+ ioEx,
							SQLError.SQL_STATE_INVALID_CONNECTION_ATTRIBUTE,
							null);
					sqlEx.initCause(ioEx);

					throw sqlEx;
				}
			}

			Iterator<Object> propsIter = urlProps.keySet().iterator();

			while (propsIter.hasNext()) {
				String key = propsIter.next().toString();
				String property = urlProps.getProperty(key);
				configProps.setProperty(key, property);
			}

			urlProps = configProps;
		}

		// Properties passed in should override ones in URL

		if (defaults != null) {
			Iterator<Object> propsIter = defaults.keySet().iterator();

			while (propsIter.hasNext()) {
				String key = propsIter.next().toString();
				if (!key.equals(NUM_HOSTS_PROPERTY_KEY)) {
					String property = defaults.getProperty(key);
					urlProps.setProperty(key, property);
				}
			}
		}

		return urlProps;
	}

	/**
	 * The getPropertyInfo method is intended to allow a generic GUI tool to
	 * discover what properties it should prompt a human for in order to get
	 * enough information to connect to a database.
	 * 
	 * <p>
	 * Note that depending on the values the human has supplied so far,
	 * additional values may become necessary, so it may be necessary to iterate
	 * through several calls to getPropertyInfo
	 * </p>
	 * 
	 * @param url
	 *            the Url of the database to connect to
	 * @param info
	 *            a proposed list of tag/value pairs that will be sent on
	 *            connect open.
	 * 
	 * @return An array of DriverPropertyInfo objects describing possible
	 *         properties. This array may be an empty array if no properties are
	 *         required
	 * 
	 * @exception SQLException
	 *                if a database-access error occurs
	 * 
	 * @see java.sql.Driver#getPropertyInfo
	 */
	public DriverPropertyInfo[] getPropertyInfo(String url, Properties info)
			throws SQLException {
		if (info == null) {
			info = new Properties();
		}

		if ((url != null) && url.startsWith(URL_PREFIX)) { //$NON-NLS-1$
			info = parseURL(url, info);
		}

		DriverPropertyInfo hostProp = new DriverPropertyInfo(HOST_PROPERTY_KEY, //$NON-NLS-1$
				info.getProperty(HOST_PROPERTY_KEY)); //$NON-NLS-1$
		hostProp.required = true;
		hostProp.description = Messages.getString("NonRegisteringDriver.3"); //$NON-NLS-1$

		DriverPropertyInfo portProp = new DriverPropertyInfo(PORT_PROPERTY_KEY, //$NON-NLS-1$
				info.getProperty(PORT_PROPERTY_KEY, PORT_PROPERTY_DEFAULT_VALUE)); //$NON-NLS-1$ //$NON-NLS-2$
		portProp.required = false;
		portProp.description = Messages.getString("NonRegisteringDriver.7"); //$NON-NLS-1$

		DriverPropertyInfo dbProp = new DriverPropertyInfo(DBNAME_PROPERTY_KEY, //$NON-NLS-1$
				info.getProperty(DBNAME_PROPERTY_KEY)); //$NON-NLS-1$
		dbProp.required = false;
		dbProp.description = "Database name"; //$NON-NLS-1$

		DriverPropertyInfo userProp = new DriverPropertyInfo(USER_PROPERTY_KEY, //$NON-NLS-1$
				info.getProperty(USER_PROPERTY_KEY)); //$NON-NLS-1$
		userProp.required = true;
		userProp.description = Messages.getString("NonRegisteringDriver.13"); //$NON-NLS-1$

		DriverPropertyInfo passwordProp = new DriverPropertyInfo(
				PASSWORD_PROPERTY_KEY, //$NON-NLS-1$
				info.getProperty(PASSWORD_PROPERTY_KEY)); //$NON-NLS-1$
		passwordProp.required = true;
		passwordProp.description = Messages
				.getString("NonRegisteringDriver.16"); //$NON-NLS-1$

		DriverPropertyInfo[] dpi = ConnectionPropertiesImpl
				.exposeAsDriverPropertyInfo(info, 5);

		dpi[0] = hostProp;
		dpi[1] = portProp;
		dpi[2] = dbProp;
		dpi[3] = userProp;
		dpi[4] = passwordProp;

		return dpi;
	}

	//
	// return the value of any property this driver knows about
	//

	/**
	 * Returns the hostname property
	 * 
	 * @param props
	 *            the java.util.Properties instance to retrieve the hostname
	 *            from.
	 * 
	 * @return the hostname
	 */
	public String host(Properties props) {
		return props.getProperty(HOST_PROPERTY_KEY, "localhost"); //$NON-NLS-1$ //$NON-NLS-2$
	}

	/**
	 * Returns the port number property
	 * 
	 * @param props
	 *            the properties to get the port number from
	 * 
	 * @return the port number
	 */
	public int port(Properties props) {
		return Integer.parseInt(props.getProperty(PORT_PROPERTY_KEY, PORT_PROPERTY_DEFAULT_VALUE)); //$NON-NLS-1$ //$NON-NLS-2$
	}

	/**
	 * Returns the database property from <code>props</code>
	 * 
	 * @param props
	 *            the Properties to look for the database property.
	 * 
	 * @return the database name.
	 */
	public String database(Properties props) {
		return props.getProperty(DBNAME_PROPERTY_KEY); //$NON-NLS-1$
	}

	/**
	 * Returns the given property from <code>props</code>
	 * 
	 * @param name
	 *            the property name
	 * @param props
	 *            the property instance to look in
	 * 
	 * @return the property value, or null if not found.
	 */
	public String property(String name, Properties props) {
		return props.getProperty(name);
	}

}
