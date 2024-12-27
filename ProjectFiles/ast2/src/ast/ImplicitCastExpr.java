package ast;

public class ImplicitCastExpr extends Node {

	public ImplicitCastExpr(int p) {
		super(p);
		// TODO Auto-generated constructor stub
	}

	@Override
	public String toString() {
		return "ImplicitCastExpr []";
	}
	public String codeGen(Code c) {
		if (children.size()!=1)
        	System.out.println("need an operands for Implicitcast");
		return children.get(0).codeGen(c);
	}
	public Variable eval(Context c) {
		System.out.println("implicitcast");
		if (children.size()!=1)
        	System.out.println("need an operands for Implicitcast");
		return children.get(0).eval(c);
    }
}
