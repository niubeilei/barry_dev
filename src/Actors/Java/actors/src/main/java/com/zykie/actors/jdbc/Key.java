package com.zykie.actors.jdbc;

/**
 * @author Administrator
 *
 */
public class Key {

	private String name;
	public Key(){}
	
	public Key(String name){
		setName(name);
	}
		
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}

	public String toString(){
		return name;
	}
}
