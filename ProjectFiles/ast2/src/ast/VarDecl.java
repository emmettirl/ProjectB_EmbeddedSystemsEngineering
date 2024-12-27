package ast;

public class VarDecl extends Node{
    String name;
    String type;
    int memaddress;
    Variable v;
	public VarDecl(int pos,String name, String type) {
		super(pos);
		this.name = name;
		this.type = type;
		memaddress = 0;
	}
	@Override
	public String toString() {
		return  "VarDecl [name=" + name + ", type=" + type + "]" ;
	}
    public String codeGen(Code c) {
    	System.out.println("VarDecl -codegen");
    	c.addStackVar(name);
    	c.code+= "sub sp,sp,#4\n";
    	return null;
    }
    public Variable eval(Context c) {
    System.out.println("VarDecl -eval"+ name);
    	Object o=null;
    	// if we are going to have floats strings etc
    	switch (type) {
    	case "int":
    		o = new Integer(0);
    	}
    	Variable v = new Variable(name,0);
    	this.v = v;
    	c.addStackVar(name,v);
    	return null;
    }
}
