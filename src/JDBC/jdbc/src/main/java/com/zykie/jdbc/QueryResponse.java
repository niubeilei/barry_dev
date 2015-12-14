package com.zykie.jdbc;

import java.sql.Types;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.logging.Logger;

import org.dom4j.Document;
import org.dom4j.Attribute;
import org.dom4j.DocumentException;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;
import org.dom4j.Node;
import org.dom4j.io.DOMReader;
import org.dom4j.io.SAXReader;

import com.zykie.jdbc.utils.ZykieLogger;

//
//parse the recMessage xml
//1. if <status error="true">, means error happened. There
//   will be error message inside <status />
//2. If successful, the result will be in:
//   <contents> xxxx </contents>   <-- for select query
//   <updatecount> 5 </updatecount>  <-- for affected rows
//3. For create/drop methods, updatecount should be 0
//
public class QueryResponse {

	private int transId = 0;
	private int returnCode = 200;
	private int updateCount = 0;
	private String procTime = ""; // in milliseconds
	private boolean isError = false;
	private String errMsg = "";
	private String msg = "";
	private List<List<Object>> records = null;
	private Field[] fields = null;

	private String[] field;

	private static final Logger log = ZykieLogger
			.getLogger("com.zykie.jdbc.QueryResponse");

	public QueryResponse(String text) throws Exception {
		try {
			// SAXReader saxReader = new SAXReader();
			// Document document = saxReader.read(wrapXML(text));
			System.out.println(text+"==============================");
			String xml = wrapXML(text);
//			System.out.println("xml======================"+xml+"====================================");
			Document doc = DocumentHelper.parseText(removeIllegalXmlCharacters(xml));
			Element root = (Element) doc.selectSingleNode("/response");
			// Element root = doc.getRootElement();
			Element node = null;
			int num;
			String str;

			node = root.element("transid");
			if (node != null) {
				str = node.getText();
				if (str == null || str.isEmpty()) {
					str = node.attributeValue("value");
				}
				transId = Integer.parseInt(str);
			}

			node = root.element("returncode");
			if (node != null) {
				str = node.getText();
				if (str == null || str.isEmpty()) {
					str = node.attributeValue("value");
				}
				returnCode = Integer.parseInt(str);
			}

			node = root.element("updatecount");
			if (node != null) {
				str = node.getText();
				if (str == null || str.isEmpty()) {
					str = node.attributeValue("value");
				}
				updateCount = Integer.parseInt(str);
			}

			node = root.element("proctime");
			if (node != null) {
				procTime = node.getText();
				if (procTime == null || procTime.isEmpty()) {
					procTime = node.attributeValue("value");
				}
			}

			node = root.element("error");
			if (node != null) {
				str = node.getText();
				if (str == null || str.isEmpty()) {
					str = node.attributeValue("value");
				}

				isError = Boolean.valueOf(str);
			}

			node = root.element("message");
			if (node != null) {
				msg = node.getText();
				if (msg == null || msg.isEmpty()) {
					msg = node.attributeValue("value");
				}
			}

			// get root node for records if any
			node = root.element("content");
			// ugly workaround for non-formal output

			if (node == null)
				node = root.element("Contents");

			buildFieldsAndRecords(node);

		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	public void buildFieldsAndRecords(Element node) throws Exception {
		if (node == null)
			return;

		Element e = null;
		List<Element> rows = null;

		// get record if any
		// rows = node.elements("record");
		rows = node.elements();

		// ugly workaround for non-formal output
		// if (rows.isEmpty())
		// rows = node.elements("database");

		if (!rows.isEmpty()) {
			Iterator<?> it = rows.iterator();
			records = new ArrayList<List<Object>>();

			/**
			 * Get fields from the first record
			 */

			/**
			 * old record is in the following format : <content total="10">
			 * <----- 90% <fieldnames>name,age</fieldnames><record name="Tom"
			 * age="11"> <record name="Jerry" age="12"> </content> new record is
			 * in the following format : <content total="1" start_idx="0"
			 * crt_idx="0" queryid="1" num="1" time="8.319000ms">
			 * <output_datarecord_t1> <age><![CDATA[20]]></age>
			 * <name><![CDATA[zhansan]]></name> </output_datarecord_t1>
			 * </content>
			 */

			Element fieldnode = (Element) node.selectSingleNode("fieldnames");
			if (fieldnode != null) {
				String text = fieldnode.getText();
				String[] names = text.split(",");
				fields = new Field[names.length];
				for (int i = 0; i < names.length; i++) {
					fields[i] = new Field("", names[i], Types.CHAR, 32);
				}
				while (it.hasNext()) {
					Element r = (Element) it.next();
					List<Object> record = new ArrayList<Object>();
					for (int i = 0; i < names.length; i++) {
						String value = r.attributeValue(names[i]);
						// System.out.println(names[i] + ":" + value);
						record.add(value);

						log.info(names[i] + ":" + value);
					}
					records.add(record);

				}
			} else {
				// System.out.println("===========testtetstsete======================");
				//System.out.println(node.attributeValue("fieldnames")
				//		+ "=========================");
				/*
				 * List<Attribute> attrList=node.attributes(); 
				 * for (int i = 0; i
				 * < attrList.size(); i++) { 
				 * //灞炴�鐨勫彇寰�Attribute item =
				 * attrList.get(i); if(item.getName().equals("fieldnames")){
				 * String strfileds=item.getValue(); field=strfileds.split(",");
				 * fields = new Field[field.length]; for(int
				 * j=0;j<field.length;j++){System.out.println(field[j]+
				 * "====================**********==================");
				 * fields[j] = new Field("", field[j], Types.CHAR, 32);
				 * //System.out.println(field[j]+
				 * "====================**********=================="); }
				 * 
				 * } System.out.println(item.getName() + "=" + item.getValue());
				 * }
				 */

				String strfileds = node.attributeValue("fieldnames");
				if (strfileds != null && !(strfileds.equals(""))) {
					field = strfileds.split(",");
					fields = new Field[field.length];
					for (int j = 0; j < field.length; j++) {
					//	System.out
					//			.println(field[j]
					//					+ "====================**********==================");
						fields[j] = new Field("", field[j], Types.CHAR, 32);
						// System.out.println(field[j]+"====================**********==================");
					}

					while (it.hasNext()) {
						Element r = (Element) it.next();
						List<Object> record = new ArrayList<Object>();
						for (int k = 0; k < field.length; k++) {
							if (r.selectSingleNode(field[k]) != null) {
					//			System.out.println("record 鏈夊�=============");
					//			System.out.println(r.selectSingleNode(field[k])
					//					.getText()
					//					+ "***==***==******");
								record.add(r.selectSingleNode(field[k])
										.getText());
							} else {
					//			System.out.println("record 娌″�===============");
								record.add(null);
							}
						}

						records.add(record);
					}
				}
				else
				{
					int i = 0;			
					while (it.hasNext()) {
						Element r = (Element) it.next();
						List<Object> record = new ArrayList<Object>();
						
						
						//List<Element> attrElements = r.elements();
						List<Attribute> attrElements =r.attributes();
					//	System.out.println("-----------"+r.getText()+"***********"+r.asXML());
						
						if (i == 0) fields = new Field[attrElements.size()];
					//	System.out.println(attrElements.size()+"%%%%%%%%%%%%%%%%%%%%%");
						for(int index=0; index<attrElements.size(); index++) {
							//Element attr = attrElements.get(index);
							Attribute attr = attrElements.get(index);
							//ugly workaround to get rid of jimo system fields
							//if (attr.getName().startsWith("zky_"))
							//	continue;
							
							record.add(attr.getText());
							if (i == 0) fields[index] = new Field("", attr.getName(), Types.CHAR, 32);
							log.info(attr.getName() + ":" + attr.getText());
						}
						
						records.add(record);
						i++;						
					}
				}
			}

		}
	}

	public int getTransId() {
		return this.transId;
	}

	public int getUpdateCount() {
		return this.updateCount;
	}

	public int getReturnCode() {
		return this.returnCode;
	}

	public String getMsg() {
		return this.msg;
	}

	public String getprocTime() {
		return this.procTime;
	}

	public boolean getError() {
		return this.isError;
	}

	public List<List<Object>> getRecords() {
		return this.records;
	}

	public Field[] getFields() {
		return this.fields;
	}

	/**
	 * Sometimes, the xml is not format, we need to fix some formatting issues
	 * before reading it
	 */
	public static String wrapXML(String text) {
		text = text.replace("\"\"/>", "\"/>");
		return "<response>" + text + "</response>";
	}

	/*
	 * This code is using StAX event handling for better performance, we may use
	 * it later
	 */
	/*
	 * public static void listAllByXMLEventReader() { String xmlFile =
	 * ListUsers.class.getResource("/").getFile() + "users.xml"; XMLInputFactory
	 * factory = XMLInputFactory.newInstance(); try { XMLEventReader reader =
	 * factory .createXMLEventReader(new FileReader(xmlFile)); while
	 * (reader.hasNext()) { XMLEvent event = reader.nextEvent();
	 * 
	 * if (event.isStartElement()) { StartElement start =
	 * event.asStartElement(); System.out.print(start.getName().getLocalPart());
	 * Iterator attrs = start.getAttributes();
	 * 
	 * while (attrs.hasNext()) { Attribute attr = (Attribute) attrs.next();
	 * System.out.print(":" + attr.getName().getLocalPart() + "=" +
	 * attr.getValue()); } System.out.println(); } } reader.close(); } catch
	 * (FileNotFoundException e) { e.printStackTrace(); } catch
	 * (XMLStreamException e) { e.printStackTrace(); } } }
	 */
	/**
	 * 去掉xml字符串中的非法字符
	 * 
	 * @param xml
	 * @return
	 */
	public static String removeIllegalXmlCharacters(String xml) {
		if (xml == null || xml.length() <= 0) {
			return "";
		}
		int len = xml.length();
		StringBuilder sb = new StringBuilder(len);
		for (int i = 0; i < len; i++) {
			char ch = xml.charAt(i);
			if (isLegalXMLCharacter(ch)) {
				sb.append(ch);
			}
		}
		return sb.toString();
	}
	
	/**
	 * 检查字符是否为合法的xml字符
	 * XML规范中规定了允许的字符范围(http://www.w3.org/TR/REC-xml#dt-character): 
	 * Char ::= #x9| #xA | #xD | [#x20-#xD7FF] | [#xE000-#xFFFD] | [#x10000-#x10FFFF]
	 * 
	 * @param ch
	 * @return
	 */
	public static boolean isLegalXMLCharacter(int ch) {
		if (ch <= 0xD7FF) {
			if (ch >= 0x20) {
				return true;
			} else {
				return ch == '\n' || ch == '\r' || ch == '\t';
			}
		}else{
			return (ch >= 0xE000 && ch <= 0xFFFD) || (ch >= 0x10000 && ch <= 0x10FFFF);
		}
	}
}
