package ast;

public class ParenExpr extends Node {

	public ParenExpr(int p) {
		super(p);
		// TODO Auto-generated constructor stub
	}

	@Override
	public String toString() {
		return "ParenExpr []";
	}
	public String codeGen(Code c) {
		if (children.size()!=1)
        	System.out.println("need an operands for ParenExpr");
	    String r = children.get(0).codeGen(c);
	    System.out.println("ParenExpr - "+r);
	    return r;
	}
	public Variable eval(Context c) {
		System.out.println("ParenExpr");
		if (children.size()!=1)
        	System.out.println("need an operands for ParenExpr");
		return children.get(0).eval(c);
    }
}
