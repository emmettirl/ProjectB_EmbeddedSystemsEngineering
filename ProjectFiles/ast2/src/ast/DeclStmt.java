package ast;

public class DeclStmt extends Node {

	public DeclStmt(int p) {
		super(p);
		// TODO Auto-generated constructor stub
	}

	@Override
	public String toString() {
		return "DeclStmt []";
	}
	public String codeGen(Code c) {
		int sz = children.size();
		System.out.println("DeclStmt - codegensz "+sz);
		Node n;
		for (int i=0;i<sz;i++) {
			n = children.get(i);
			System.out.println(" codegen "+n);
			n.codeGen(c);
		}
		return null;
	}
	public Variable eval(Context c) {
		int sz = children.size();
	//	System.out.println("DeclStmt - eval sz "+sz);
		Node n;
		for (int i=0;i<sz;i++) {
			n = children.get(i);
			n.eval(c);
		}
		return null;
	}
}
