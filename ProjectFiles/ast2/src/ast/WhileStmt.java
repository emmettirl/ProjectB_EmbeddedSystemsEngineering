package ast;

public class WhileStmt extends Node{
   

	public WhileStmt(int pos) {
		super(pos);
		
	}

	@Override
	public String toString() {
		return "WhileStmt ";
	}
	public String codeGen(Code c) {
		System.out.println("While stmt -codegen");
        if (children.size()!=2)
        	System.out.println("need to a condition and a statment for if");
            	
		return null;
	}
	public Variable eval(Context c) {
			System.out.println("while  -eval");
			for (int ii=0;ii<children.size();ii++) {
				System.out.println(children.get(ii).getClass().getName());
			}
	        if (children.size()<2)
	        	System.out.println("need to a condition and a statment for if");
	       
	        
	        return null;
	}
	
}
