package com.zykie.actors.thrift;

import com.zykie.actors.IActor;

public class ActHelloWorld implements IActor{

	@Override
	public boolean run(String xml,StringBuffer response) {

		System.out.println("ActHelloWorld request:"+xml);
		response.append("helloworld");
		return true;

	}
	
	public static void main(String[] args) {
		StringBuffer s = new StringBuffer();
		System.out.println(new ActHelloWorld().run("", s));
		System.out.println(s);
	}

}
