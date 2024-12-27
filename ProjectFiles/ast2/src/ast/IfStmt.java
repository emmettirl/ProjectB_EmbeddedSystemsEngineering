package ast;

public class IfStmt extends Node{
   

	public IfStmt(int pos) {
		super(pos);
		
	}

	@Override
	public String toString() {
		return "IfStmt ";
	}
	public String codeGen(Code c) {
		System.out.println("ifstmt -codegen");
        if (children.size()!=2)
        	System.out.println("need to a condition and a statment for if");
            	
		return null;
	}
	public Variable eval(Context c) {
			System.out.println("if  -eval");
			for (int ii=0;ii<children.size();ii++) {
				System.out.println(children.get(ii).getClass().getName());
			}
	        if (children.size()<2)
	        	System.out.println("need to a condition and a statment for if");
	        Variable condition=children.get(0).eval(c);
	        if (condition.val!=0) {
	        	Variable statment =children.get(1).eval(c);
	        } 
	        return null;
	}
	
}
