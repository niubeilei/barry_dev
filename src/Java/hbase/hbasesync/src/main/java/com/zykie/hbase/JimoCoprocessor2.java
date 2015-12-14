package com.zykie.hbase;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.NavigableMap;
import org.apache.hadoop.hbase.Cell;
import org.apache.hadoop.hbase.client.Delete;
import org.apache.hadoop.hbase.client.Durability;
import org.apache.hadoop.hbase.client.Put;
import org.apache.hadoop.hbase.coprocessor.BaseRegionObserver;
import org.apache.hadoop.hbase.coprocessor.ObserverContext;
import org.apache.hadoop.hbase.coprocessor.RegionCoprocessorEnvironment;
import org.apache.hadoop.hbase.regionserver.wal.WALEdit;
import com.zykie.bean.ReColumn;

public class JimoCoprocessor2 extends BaseRegionObserver{
	private Map<String, StringBuffer> sqlmap=new HashMap<String, StringBuffer>();
	private File file = null;
	private FileOutputStream out = null;
	private OutputStreamWriter osw = null;
	private BufferedWriter bw = null;
	private String dirname=null;
	private String filename=null;
	
	public void writeSql(String tablename,String sql) throws IOException{
		StringBuffer strbuff=sqlmap.get(tablename);
		if(strbuff==null){
			strbuff=new StringBuffer();
		}
		strbuff.append(sql+"\r\n");
		if(strbuff.length()>100){
			Date d=new Date();
			SimpleDateFormat formatter = new SimpleDateFormat("yyyy-MM-dd"); 
			String datestr = formatter.format(d); 
			dirname="/home/hadoop/lib/syncdata/"+datestr;
			filename=tablename+".txt";
			file = new File(dirname+File.separator+filename);
			File f=new File(dirname);
			if (!f.exists()) {
				   f.mkdir();
			}
			out = new FileOutputStream(file,true);
			osw = new OutputStreamWriter(out, "UTF8");
			bw  = new BufferedWriter(osw);
			bw.write(strbuff.toString());
			bw.flush();
			bw.close();
			osw.close();
			out.close();
			strbuff.setLength(0);
			
		}
		sqlmap.put(tablename, strbuff);
	}
	
	@SuppressWarnings("deprecation")
	@Override
	public void postPut(ObserverContext<RegionCoprocessorEnvironment> e,
			Put put, WALEdit edit, Durability durability) throws IOException {
		super.postPut(e, put, edit, durability);
		String tablename=e.getEnvironment().getRegion().getTableDesc().getNameAsString();
		if(!(tablename.equals("hbase:meta"))){
		String rowkey=new String(put.getRow());
		NavigableMap<byte[],List<Cell>> map=put.getFamilyCellMap();
		Collection<List<Cell>> cl=map.values();
		Iterator<List<Cell>> i=cl.iterator();
		List<Cell> li=null;
		String value="";
		String col="";
		String family="";
		List<ReColumn> list=new ArrayList<ReColumn>();
		ReColumn recolumn=null;
		while(i.hasNext()){
			li=(List<Cell>) i.next();
			for(Cell c:li){
				col=new String(c.getQualifier());
				family=new String(c.getFamily());
				value=new String(c.getValue());
				recolumn=new ReColumn();
				recolumn.setCfamily(family);
				recolumn.setCcol(col);
				recolumn.setCvalue(value);
				list.add(recolumn);
			}
		}
		for(ReColumn c:list){
			 String strput="put '"+tablename+"','"+rowkey+"','"+c.getCfamily()+":"+c.getCcol()+"','"+c.getCvalue()+"'";
			 writeSql(tablename,strput);
		}
		
		}
		
	}

	@SuppressWarnings("deprecation")
	@Override
	public void postDelete(ObserverContext<RegionCoprocessorEnvironment> e,
			Delete delete, WALEdit edit, Durability durability)
			throws IOException {
		super.postDelete(e, delete, edit, durability);
		String tablename=e.getEnvironment().getRegion().getTableDesc().getNameAsString();
		String rowkey=new String(delete.getRow());
		NavigableMap<byte[],List<Cell>> map=delete.getFamilyCellMap();
		Collection<List<Cell>> cl=map.values();
		Iterator<List<Cell>> i=cl.iterator();
		List<Cell> li=null;
		String col="";
		String family="";
		List<ReColumn> list=new ArrayList<ReColumn>();
		boolean isallrow=false;
		ReColumn recolumn=null;
		while(i.hasNext()){
			li=(List<Cell>) i.next();
			if(new String(li.get(0).getQualifier()) == null||new String(li.get(0).getQualifier()).equals("")){
				isallrow=true;
				break;
			}
			for(Cell c:li){
				col=new String(c.getQualifier());
				family=new String(c.getFamily());
				recolumn=new ReColumn();
				recolumn.setCfamily(family);
				recolumn.setCcol(col);
				list.add(recolumn);
			}
			
		}
		try {
			if(isallrow==true){
				//删除
				String strdel="deleteall '"+tablename+"','"+rowkey+"'";
				writeSql(tablename, strdel);
			}else{
				//更新
				//更新数据
				for(ReColumn c:list){
					 String strdelcol="delete '"+tablename+"','"+rowkey+"','"+c.getCfamily()+":"+c.getCcol()+"'";
					 writeSql(tablename,strdelcol);
				}
			}
		} catch (Exception e2) {
			e2.printStackTrace();
		}

		
		
		
	}
	
	
	
	
}
