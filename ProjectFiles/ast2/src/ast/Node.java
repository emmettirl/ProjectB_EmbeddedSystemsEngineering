package ast;

import java.util.ArrayList;

public class Node {
      ArrayList<Node> children;
      int pos; // position in line in outputed ast
      
      Node (int p){
    	  pos =p;
    	  children = new ArrayList<Node>();
      }
      void add(Node n) {
    	  children.add(n);
      }

	public String toString1() {
		return "Node [children=" + children + ", pos=" + pos + "]";
	}
	public String codeGen(Code c) {
	//	System.out.println("Code Gen: "+this.getClass().getName());
		return null;
	}
	public Variable eval(Context c) {
	//	System.out.println("Eval: "+this.getClass().getName());
		return null;
	}
}
