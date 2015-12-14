package com.zykie.serverutil;

import java.io.IOException;
import java.io.StringReader;
import java.util.HashMap;
import java.util.List;
import org.jdom2.Document;
import org.jdom2.Element;
import org.jdom2.JDOMException;
import org.jdom2.Namespace;
import org.jdom2.input.SAXBuilder;
import org.xml.sax.InputSource;
import java.util.Map;


public class ParserXmlDoc {
	public static Document getDoc(String xml) throws JDOMException, IOException{
		 //创建一个新的字符串
        StringReader read = new StringReader(xml);
        //创建新的输入源SAX 解析器将使用 InputSource 对象来确定如何读取 XML 输入
        InputSource source = new InputSource(read);
        //创建一个新的SAXBuilder
        SAXBuilder sb = new SAXBuilder();
        Document doc = sb.build(source);
        return doc;
	}
    public Map<String,String> xmlElements(String xmlDoc) {
    	Map<String,String> map = new HashMap<String, String>();
        //创建一个新的字符串
        StringReader read = new StringReader(xmlDoc);
        //创建新的输入源SAX 解析器将使用 InputSource 对象来确定如何读取 XML 输入
        InputSource source = new InputSource(read);
        //创建一个新的SAXBuilder
        SAXBuilder sb = new SAXBuilder();
        try {
            //通过输入源构造一个Document
            Document doc = sb.build(source);
            //取的根元素
            Element root = doc.getRootElement();
            System.out.println("=========root:"+root.getName());//输出根元素的名称（测试）
            //得到根元素所有子元素的集合
            List<Element> jiedian = root.getChildren();
            //获得XML中的命名空间（XML中未定义可不写）
            //Namespace ns = root.getNamespace();
            //System.out.println(ns);
            Element et = null;
            for(int i=0;i<jiedian.size();i++){
                et = (Element) jiedian.get(i);//循环依次得到子元素
                System.out.println(et);
                map.put(et.getName(), root.getChildText(et.getName()));
                System.out.println("-------------map:"+map);
            }
        } catch (JDOMException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return map;
    }
    
    public static String getactname(String xml) throws IOException, JDOMException{
    	Document doc=getDoc(xml);
    	Element root = doc.getRootElement();
    	String actname=root.getChildText("actname");
    	//System.out.println(actname);
    	return actname;
    }
    public static void main(String[] args) throws JDOMException, IOException{
        String xml = "<request>" +
        				"<actname>descTable</actname>" +
        				"<actparams>" +
        					"<tablename>t1</tablename>" +
        				"</actparams>" +
        			 "</request>";
       // doc.xmlElements(xml);
        System.out.println(getactname(xml));
    }
}
