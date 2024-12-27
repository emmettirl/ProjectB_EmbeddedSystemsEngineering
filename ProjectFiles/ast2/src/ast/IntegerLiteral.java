package ast;

public class IntegerLiteral extends Node {
     int val;

	public IntegerLiteral(int pos,int val) {
		super(pos);
		this.val = val;
	}

	@Override
	public String toString() {
		return "IntegerLiteral [val=" + val + "]";
	}
	public String codeGen(Code c) {
		String reg = c.getRegister();
		c.code += "mov "+reg+", #"+val+"\n";
		return reg;
	}
	public Variable eval(Context c) {
    //	System.out.println("VarDecl -eval");
    	
    	Variable v = new Variable("",val);
    	return v;
    }
     
}
