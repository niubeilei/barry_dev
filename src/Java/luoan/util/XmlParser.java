package util;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.DocumentException;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

public class XmlParser {
	private Document doc=null;
	private Element rootEle=null;
	
	public XmlParser(String xml){
		try {
			//将字符串转化成xml
			doc=DocumentHelper.parseText(xml);
			//获取根节点
			rootEle=doc.getRootElement();	
		} catch (DocumentException e) {
			e.printStackTrace();
		} 
	}	
	
	/**** * 
	 * @param nodeName
	 * @param attriName
	 * @param isAttri
	 * @return
	 */
	private String GetValue(String nodeName,String attriName,boolean isAttri)
	{
		if(rootEle == null) return "";
		
		Element node = rootEle.element(nodeName);
		if(node==null) return "";
		
		if(isAttri)
		{
			if(attriName.length()==0) return "";
			return node.attributeValue(attriName);
		}
		else
			return node.getTextTrim();
	}
	
	private String GetValue(String nodeName)
	{
		return GetValue(nodeName,"",false);
	}
	
	private String GetValue(String nodeName,String attriName)
	{
		return GetValue(nodeName,attriName,true);
	}
	
	@SuppressWarnings("unchecked")
	public List<Element> GetNodes(Element parent, String nodeName)
	{
		if(parent ==null) return null;
		
		return parent.elements(nodeName);
	}
	
	public List<Element> GetNodes(String nodeName)
	{		
		return GetNodes(rootEle,nodeName);
	}
	
	
	public Element GetNode(Element parent, String nodeName)
	{
		if(parent ==null) return null;
		
		return parent.element(nodeName);
	}
	
	public Element GetNode(String nodeName)
	{
		return GetNode(rootEle,nodeName);
	}
	
	public String GetNodeText(String nodeName)
	{
		return GetValue(nodeName);
	}
	
	public String GetNodeAttrValue(String nodeName,String attriName)
	{
		return GetValue(nodeName,attriName);
	} 
}
