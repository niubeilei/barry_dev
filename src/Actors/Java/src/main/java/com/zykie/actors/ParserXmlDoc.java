package com.zykie.actors;

import java.io.IOException;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.jdom2.Attribute;
import org.jdom2.Document;
import org.jdom2.Element;
import org.jdom2.JDOMException;
import org.jdom2.input.SAXBuilder;
import org.xml.sax.InputSource;

import com.zykie.actors.jdbc.Key;

public class ParserXmlDoc {
	
	 public static Map<Key,List<String>> xmlElements(String xmlDoc) {
	        //创建一个新的字符串
	        StringReader read = new StringReader(xmlDoc);
	        //创建新的输入源SAX 解析器将使用 InputSource 对象来确定如何读取 XML 输入
	        InputSource source = new InputSource(read);
	        //创建一个新的SAXBuilder
	        SAXBuilder sb = new SAXBuilder();
	        Map<Key,List<String>> map = new HashMap<Key, List<String>>();
	        List<String> values = null;
	        try {
	            //通过输入源构造一个Document
	            Document doc = sb.build(source);
	            //取的根元素
	            Element root = doc.getRootElement();
	            //得到根元素所有子元素的集合
	            List<Element> jiedian = root.getChildren();
	            
	            Element et = null;
	            for(int i=0;i<jiedian.size();i++){
	            	values = new ArrayList<String>();
	                et = (Element) jiedian.get(i);//循环依次得到子元素
	                if(root.getChildText(et.getName()) != null && !"".equals(root.getChildText(et.getName()))) values.add(root.getChildText(et.getName()));
	                List<Attribute> attrs = et.getAttributes();
	                System.out.println(attrs+"------------------");
	                for(int j = 0;j < attrs.size();j++) {
	                	System.out.println(attrs.get(j).getName()+"***********"+et.getAttributeValue(attrs.get(j).getName()));
	                	values.add(attrs.get(j).getName()+"="+et.getAttributeValue(attrs.get(j).getName()));
	                }
	                map.put(new Key(et.getName()), values);
	            }
	        } catch (JDOMException e) {
	            // TODO 自动生成 catch 块
	            e.printStackTrace();
	        } catch (IOException e) {
	            // TODO 自动生成 catch 块
	            e.printStackTrace();
	        }
	        return map;
	    }
	
    public static void main(String[] args){
    	 ParserXmlDoc doc = new ParserXmlDoc();
         String xml =
        	 "<obj><tablename>student</tablename><operate>create</operate>" +
				"<column name=\"name\" datatype=\"varchar\" size=\"10\"></column>"+
				"<column name=\"age\" datatype=\"varchar\" size=\"10\"></column>"+
				"</obj>";
         System.out.println(doc.xmlElements(xml));
    }
}
