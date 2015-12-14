package util;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.dom4j.Attribute;
import org.dom4j.Document;
import org.dom4j.DocumentException;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import bean.RValue;

/**
 * 
 * 解析XML
 *
 */
public class ParserXml {
	private XmlParser p=null;
	public ParserXml(String xml){
		p=new XmlParser(xml);
	}
	/**
	 * 获得身份认证号
	 * @return
	 */
	public String getIdenCode(){
		//获得父节点
		Element headNode=p.GetNode("Head");
		Element idenNode=p.GetNode(headNode, "IdenCode");
		String idenCode=idenNode.getTextTrim();
		return idenCode;
	}
	/**
	 * 获得关键字
	 * @return
	 */
	public String getKeyWords(){
		//获得父节点
		Element headNode=p.GetNode("Head");
		Element keyNode=p.GetNode(headNode,"KeyWord");
		String keyWord=keyNode.getTextTrim();
		return keyWord;
	}
	
	/**
	 * 获得返回值的列表
	 * @return
	 */
	public List<RValue> getValues(){
		List<RValue> list=new ArrayList<RValue>();
		Element parentNode=p.GetNode("Values");
		List<Element> elements=p.GetNodes(parentNode,"Value");
		for (Element element : elements) {
			List<RValue> l=new ArrayList<RValue>();
			System.out.println("属性个数---"+element.attributeCount());
			for(int i=0;i<element.attributeCount();i++){
				RValue value=null;
				value=new RValue(element.attribute(i).getName(), element.attribute(i).getValue());
				l.add(value);
			}
		}
		return list;
	}
	
	public Map<String,String> getResult(){
		Element resNode=p.GetNode("Result");
		Map<String,String> map=new HashMap<String, String>();
		map.put("code", resNode.attributeValue("Code"));
		map.put("message", resNode.attributeValue("Message"));
		return map;
	}
	
	public static void main(String[] args) throws DocumentException {
		String xml="<embedobj zky_role=\"学校老师\" zky_class=\"中心老师\" zky_name=\"钟丽佳\" zky_uname=\"zhonglijia\" zky_access_chakan=\"查看申请\" zky_access_add=\"新增申请\" ><zky_passwd__n><![CDATA[111111]]></zky_passwd__n></embedobj>";
		 Document document = DocumentHelper.parseText(xml);  
		 Element root = document.getRootElement();  
		 System.out.println(root);
		 List<Attribute> attrList = root.attributes(); 
		 for (int i = 0; i < attrList.size(); i++) {    
			 //属性的取得    
			 Attribute item = attrList.get(i);    
			 System.out.println(item.getName() + "=" + item.getValue());
		}
		 




	}
}
