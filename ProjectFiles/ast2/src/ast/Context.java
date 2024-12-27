package ast;

import java.util.HashMap;
import java.util.Stack;

public class Context {
	
	HashMap<String,Variable> stackvars;
	
	
	
	Context(){
		
	stackvars = new HashMap<String,Variable>();
		
	}
	
	void addStackVar(String name,Variable o) {
		System.out.println("addstackvar "+name +" "+o.val);
		stackvars.put(name,o);
	}
    Variable getStackVar(String name) {
		
		return stackvars.get(name);
	}
	
	void Print(){
		
	}
}

