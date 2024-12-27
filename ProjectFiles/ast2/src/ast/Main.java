package ast;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;

public class Main {
	public static void main(String[] args) throws IOException{
HashMap<String,VarDecl> symboltable = new HashMap<String,VarDecl>();
HashMap<String,Node> parent = new HashMap<String,Node>();
BufferedReader reader;
Node head,n;
int oldpos=0;
n=null;
Code code = new Code();
Context context = new Context();
try {
	reader = new BufferedReader(new FileReader(
			"out.txt"));
	String line = reader.readLine();

	head=null;
	String name,type;
	while ((line = reader.readLine()) != null) {
	//	System.out.println(line);
		// read next line
		n=null;
		int pos = line.indexOf('-');
		String str1 = line.substring(pos, line.length());
		if (str1.contains("<<<NULL>>>"))
			continue;
		int pos1 = str1.indexOf(' ');
		String ast = str1.substring(1, pos1);
		//System.out.println("ast "+ast);
		String[] strs =str1.split(" ");
	//	for (int i=0;i<strs.length;i++)
		//	System.out.println("split["+i+"] ="+strs[i]+"=");
	
		switch (ast) {
		case "DeclRefExpr":
		   name = strs[strs.length-2].substring(1, strs[strs.length-2].length()-1);
		   type = strs[strs.length-1].substring(1, strs[strs.length-1].length()-1);
	       n = new DeclRefExpr(pos,name,type);
	 //      System.out.println(n);
	       break;
		case "BinaryOperator":
			  // type = strs[strs.length-2].substring(1, strs[strs.length-2].length()-1);
			   String op = strs[strs.length-1].substring(1, strs[strs.length-1].length()-1);
		       n = new BinaryOperator(pos,op);
		//       System.out.println(n);
		       break;
		case "IntegerLiteral":
			  // type = strs[strs.length-2].substring(1, strs[strs.length-2].length()-1);
			   String theint = strs[strs.length-1];
		       n = new IntegerLiteral(pos,Integer.parseInt(theint));
		   //    System.out.println(n);
		       break;
		case "CompoundStmt":
			  // work with compound statments
		       n = new CompoundStmt(pos);
		       if (head == null)
		    	   head = n;
		 //      System.out.println(n);
		       break;
		case "ImplicitCastExpr":
			  
		       n = new ImplicitCastExpr(pos);
		   //    System.out.println(n);
		       break;
		case "ParenExpr":
			  
		       n = new ParenExpr(pos);
		   //    System.out.println(n);
		       break;
		case "DeclStmt":
			  
		       n = new DeclStmt(pos);
		  //     System.out.println(n);
		       break;
		case "ReturnStmt":
			  
		       n = new ReturnStmt(pos);
		   //    System.out.println(n);
		       break;
		case "IfStmt":
			  
		       n = new IfStmt(pos);
		   //    System.out.println(n);
		       break;
		case "WhileStmt":
			  
		       n = new WhileStmt(pos);
		   //    System.out.println(n);
		       break;
		case "VarDecl":
			   name = strs[strs.length-2];
			   type = strs[strs.length-1].substring(1, strs[strs.length-1].length()-1);
			//   System.out.println("var decl name="+name);
			//   System.out.println("vardecl type "+type);
		       n = new VarDecl(pos,name,type);
		       symboltable.put(name, (VarDecl)n);
		 //      System.out.println(n);
		       break;
		}
		Node parent1 = parent.get(""+(pos-2));
		if (parent1 != null) {
			
			parent1.add(n);
		//	System.out.println("parent "+parent1);
		}
		if (n!=null)
		parent.put(""+pos, n);
			
	}
	int sz = head.children.size();
	for (int i=0;i<sz;i++) {
		code.resetRegisters();
	    n = head.children.get(i);
		System.out.println(" codegen "+n);
		n.codeGen(code);
	}
	code.Print();
	//head.eval(context);
	reader.close();
	for (String key : context.stackvars.keySet()) {
		Variable v = context.stackvars.get(key);
		System.out.println(key+" = "+v.val);
		
	}
} catch (IOException e) {
	e.printStackTrace();
}
	}
}
