package com.zykie.jdbc.utils;

import java.io.IOException;
import java.util.logging.LogRecord;
import java.util.logging.Logger;
import java.util.logging.Level;
import java.util.logging.ConsoleHandler;
import java.util.logging.FileHandler;

import java.util.logging.Formatter;
import java.util.logging.SimpleFormatter;
import java.util.logging.XMLFormatter;

/**
 * Use java.util.Logger but add some default settings
 */
public class ZykieLogger {
	public static String logFile = "f:/dingsheng/jdbc/jimo/ws/zykie.log";

	public static Logger getLogger(String className) {
		Logger log = null;

		try {
			log = Logger.getLogger(className);
			ConsoleHandler consoleHandler = new ConsoleHandler();
			FileHandler fileHandler = new FileHandler(logFile);

			log.setLevel(Level.INFO);
			consoleHandler.setLevel(Level.ALL);
			log.addHandler(consoleHandler);

			fileHandler.setLevel(Level.INFO);
			fileHandler.setFormatter(new ZykieLogHandler());
			log.addHandler(fileHandler);
		} catch (Exception e) {
			// do nothing for now
		}

		return log;
	}
}

class ZykieLogHandler extends Formatter {
	@Override
	public String format(LogRecord record) {
		return record.getLoggerName() + ":" + record.getLevel() + ":"
				+ record.getMessage() + "\n";
	}
}
