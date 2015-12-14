package servlet;

import java.text.SimpleDateFormat;
import java.util.Date;

public class Test {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");//设置日期格式
//		System.out.println(format.format(new Date()));// new Date()为获取当前系统时间
		
		System.out.println("update \"luoan_申请单\" set zky_iscomment=\"已评论\",zky_comment_marks=\"\"," +
				"zky_comment_assessment=\"\"," +
				"zky_schoolLaoShi_status=\"已评论\"," +"zky_schoolLaoShi_comment_time=\""+
				format.format(new Date())+"\" " +"where contact_id=\"\" and zky_status=\"已完成\"");
	}

}
