package ast;

public class CompoundStmt extends Node {

	public CompoundStmt(int p) {
		super(p);
		// TODO Auto-generated constructor stub
	}
	// not tested
	public String codeGen(Code c) {
		System.out.println("Compound -codegen");
		int sz = children.size();
		Node n;
		for (int i=0;i<sz;i++) {
			c.resetRegisters();
		    n = children.get(i);
			System.out.println(" codegen "+n);
			n.codeGen(c);
		}
		return null;
       
	    
	}
	public Variable eval(Context c) {
		System.out.println("compound  -eval");
		for (int ii=0;ii<children.size();ii++) {
			System.out.println("compound "+children.get(ii).getClass().getName());
			Variable statment =children.get(ii).eval(c);
		}
        
        		
        return null;
}
}
