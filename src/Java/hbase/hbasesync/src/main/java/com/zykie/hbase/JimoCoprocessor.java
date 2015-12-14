package com.zykie.hbase;

import java.io.IOException;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.NavigableMap;



import org.apache.hadoop.hbase.Cell;
import org.apache.hadoop.hbase.client.Delete;
import org.apache.hadoop.hbase.client.Durability;
import org.apache.hadoop.hbase.client.Put;
import org.apache.hadoop.hbase.coprocessor.BaseRegionObserver;
import org.apache.hadoop.hbase.coprocessor.ObserverContext;
import org.apache.hadoop.hbase.coprocessor.RegionCoprocessorEnvironment;
import org.apache.hadoop.hbase.regionserver.wal.WALEdit;


import com.zykie.bean.Column;
import com.zykie.util.JDBCFactory;

public class JimoCoprocessor extends BaseRegionObserver{
	//判断该条数据是否已经存在
	public static boolean isExists(String tablename,String rowkey) throws SQLException{
		Connection conn=JDBCFactory.getConnection();
		Statement stmt=conn.createStatement();
		String sql="select * from "+tablename+" where rowkey=\""+rowkey+"\"";
		ResultSet rs = stmt.executeQuery(sql);
		boolean isexists=false;
		while(rs.next()){
			isexists=true;
		}
		JDBCFactory.close(rs, stmt, conn);
		return isexists;
	}
	
	public static boolean common(String sql) throws SQLException{
		Connection conn=JDBCFactory.getConnection();
		Statement stmt=conn.createStatement();
		stmt.execute(sql);
		com.zykie.jdbc.ResultSet result=(com.zykie.jdbc.ResultSet) stmt.getResultSet();
		//System.out.println(result.getQueryResponse().getError()+"=============");
		boolean issuccessful=!(result.getQueryResponse().getError());
		JDBCFactory.close(null, stmt, conn);
		return issuccessful;
	}

	@SuppressWarnings("deprecation")
	@Override
	public void postPut(ObserverContext<RegionCoprocessorEnvironment> e,
			Put put, WALEdit edit, Durability durability) throws IOException {
		super.postPut(e, put, edit, durability);
		String tablename=e.getEnvironment().getRegion().getTableDesc().getNameAsString();
		/*File file = null;
		FileOutputStream out = null;
		OutputStreamWriter osw = null;
		BufferedWriter bw = null;
		file = new File("/home/hadoop/lib/mm.txt");
		out = new FileOutputStream(file,true);
		osw = new OutputStreamWriter(out, "UTF8");
		bw = new BufferedWriter(osw);*/
		if(!(tablename.equals("hbase:meta"))){
		String rowkey=new String(put.getRow());
		NavigableMap<byte[],List<Cell>> map=put.getFamilyCellMap();
		Collection<List<Cell>> cl=map.values();
		Iterator<List<Cell>> i=cl.iterator();
		List<Cell> li=null;
		String value="";
		String col="";
		String family="";
		List<Column> list=new ArrayList<Column>();
		Column column=null;
		while(i.hasNext()){
			li=(List<Cell>) i.next();
			for(Cell c:li){
				col=new String(c.getQualifier());
				family=new String(c.getFamily());
				value=new String(c.getValue());
				column=new Column();
				column.setCname(family+"_"+col);
				column.setCvalue(value);
				//bw.write("fmclo="+family+"_"+col+"\n");
				list.add(column);
			}
			/*for(Column c:list){
				bw.write("cname="+c.getCname()+"\n");
				bw.write("cvalue="+c.getCvalue()+"\n");
			}*/
		}
		try {
			boolean isexists=isExists(tablename, rowkey);
			//bw.write("isexists="+isexists+"\n");
			if(isexists){
				//更新数据
				String sql="update "+tablename+" set ";
				String psql="";
				for(int j=0;j<list.size();j++){
					if(j<list.size()-1){
						psql=psql+list.get(j).getCname()+"=\""+list.get(j).getCvalue()+"\",";
					}else if(j==list.size()-1){
						psql=psql+list.get(j).getCname()+"=\""+list.get(j).getCvalue()+"\"";
					}
				}
				sql=sql+psql+" where rowkey=\""+rowkey+"\"";
				boolean issuccessful=common(sql);
				if(issuccessful){
					System.out.println("更新成功");
				}else{
					System.out.println("更新失败");
				}
				//bw.write("sql="+sql+"\n");
			}else{
				//插入数据
				String columns="";
				String values="";
				for(int j=0;j<list.size();j++){
					if(j<list.size()-1){
						columns=columns+list.get(j).getCname()+",";
						values=values+"\""+list.get(j).getCvalue()+"\",";
					}else if(j==list.size()-1){
						columns=columns+list.get(j).getCname();
						values=values+"\""+list.get(j).getCvalue()+"\"";
					}
				}
				String sql ="insert into "+tablename+"(rowkey,"+columns+") values (\""+rowkey+"\","+values+")";
				boolean issuccessful=common(sql);
				if(issuccessful){
					System.out.println("插入成功");
				}else{
					System.out.println("插入失败");
				} 
				//bw.write("sql2="+sql+"\n");
			}
		} catch (Exception e1) {
			e1.printStackTrace();
		}
		/*bw.flush();
		bw.close();
		osw.close();
		out.close();*/
		}
		
	}

	public static boolean isAllEmpty(String tablename,String rowkey) throws SQLException{
		Connection conn=JDBCFactory.getConnection();
		Statement stmt=conn.createStatement();
		String sql="select * from "+tablename+" where rowkey=\""+rowkey+"\"";
		ResultSet rs = stmt.executeQuery(sql);
		int columncount=rs.getMetaData().getColumnCount();
		boolean isallempty=true;
		while(rs.next()){
			for(int i=2;i<=columncount;i++){
				if(rs.getString(i) != null){
					isallempty=false;
					break;
				}
			}
			
		}
		return isallempty;
	}
	@SuppressWarnings("deprecation")
	@Override
	public void postDelete(ObserverContext<RegionCoprocessorEnvironment> e,
			Delete delete, WALEdit edit, Durability durability)
			throws IOException {
		super.postDelete(e, delete, edit, durability);
		/*File file = null;
		FileOutputStream out = null;
		OutputStreamWriter osw = null;
		BufferedWriter bw = null;
		file = new File("/home/hadoop/lib/mmm.txt");
		out = new FileOutputStream(file,true);
		osw = new OutputStreamWriter(out, "UTF8");
		bw = new BufferedWriter(osw);*/
		String tablename=e.getEnvironment().getRegion().getTableDesc().getNameAsString();
		//bw.write("tablename="+tablename+"\n");
		String rowkey=new String(delete.getRow());
		//bw.write("rowkey="+rowkey+"\n");
		//int numfam=delete.numFamilies();
		//bw.write("numfam="+numfam+"\n");
		NavigableMap<byte[],List<Cell>> map=delete.getFamilyCellMap();
		Collection<List<Cell>> cl=map.values();
		Iterator<List<Cell>> i=cl.iterator();
		List<Cell> li=null;
		String col="";
		String family="";
		String jimoclo="";
		List<String> list=new ArrayList<String>();
		boolean isallrow=false;
		while(i.hasNext()){
			li=(List<Cell>) i.next();
			if(new String(li.get(0).getQualifier()) == null||new String(li.get(0).getQualifier()).equals("")){
				isallrow=true;
				//bw.write("删除整行");
				break;
			}
			for(Cell c:li){
				col=new String(c.getQualifier());
				family=new String(c.getFamily());
				jimoclo=family+"_"+col;
				list.add(jimoclo);
			}
			/*for(String s:list){
				bw.write("cname="+s+"\n");
			}*/
		}
		try {
			if(isallrow==true){
				//删除
				String sql="delete from "+tablename+" where rowkey=\""+rowkey+"\"";
				//bw.write("sql="+sql);
				boolean issuccessful=common(sql);
				if(issuccessful){
					System.out.println("删除行成功");
				}else{
					System.out.println("删除行失败");
				}
			}else{
				//更新
				//更新数据
				String sql="update "+tablename+" set ";
				String psql="";
				for(int j=0;j<list.size();j++){
					if(j<list.size()-1){
						psql=psql+list.get(j)+"=\" \",";
					}else if(j==list.size()-1){
						psql=psql+list.get(j)+"=\" \"";
					}
				}
				sql=sql+psql+" where rowkey=\""+rowkey+"\"";
				//bw.write("sql="+sql);
				boolean issuccessful=common(sql);
				if(issuccessful){
					System.out.println("删除列成功");
					//判断列是否都为空，如都为空，则删除该行
					boolean isallempty=isAllEmpty(tablename,rowkey);
					if(isallempty){
						String delsql="delete from "+tablename+" where rowkey=\""+rowkey+"\"";
						boolean isdelsuccessful=common(delsql);
						if(isdelsuccessful){
							System.out.println("删除空行成功");
						}else{
							System.out.println("删除空行失败");
						}
					}
				}else{
					System.out.println("删除列失败");
				}
			}
		} catch (Exception e2) {
			e2.printStackTrace();
		}
//		bw.flush();
//		bw.close();
//		osw.close();
//		out.close();
		
		
		
	}
	
	
	
	
}
