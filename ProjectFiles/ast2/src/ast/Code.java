package ast;

import java.util.HashMap;
import java.util.Stack;

public class Code {

	String data;
	String code;
	Stack allocatedRegs;
	String[] registers = {"r0","r1","r2","r3","r4","r5","r6","r7","r8"};
	int noregs;
	int sp;
	
	
HashMap<String,String> stackvars;
	
	
	
	
	void addStackVar(String name) {
		String loc = "[fp,#-"+sp+"]";
		System.out.println("addstackvar "+name +" "+loc);
		sp+=4;
		stackvars.put(name,loc);
	}
    String getStackVar(String name) {
		
		return stackvars.get(name);
	}
	Code(){
		data="\n";
		code="\n";
		allocatedRegs=new Stack();
		sp=4;
		stackvars = new HashMap<String,String>();
		
	}
	void resetRegisters() {
		noregs=0;
		allocatedRegs.clear();
	}
	
	String getRegister() {
		return registers[noregs++];
	}
	void Print(){
		System.out.println("data "+data);
		System.out.println("code "+code);
	}
}
