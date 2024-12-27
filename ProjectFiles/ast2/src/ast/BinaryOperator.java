package ast;

public class BinaryOperator extends Node{
    String op;

	public BinaryOperator(int pos,String op) {
		super(pos);
		this.op = op;
	}

	@Override
	public String toString() {
		return "BinaryOperator [op=" + op + "]";
	}
	public String codeGen(Code c) {
		
        if (children.size()!=2)
        	System.out.println("need to operands for "+op);
        String left=children.get(0).codeGen(c);
        String right =children.get(1).codeGen(c);
        System.out.println("Binaryoperator -codegen" +op + " "+ left +" "+right);
        String reg;
        switch (op) {
        case "=":
	    	c.code += "str "+right+",["+left+"]"+"\n";
	    	return null;
        case "+":
        	reg = c.getRegister();
        	c.code += "add "+reg+","+left+","+right+"\n";
	    	return reg;
        case "-":
        	reg = c.getRegister();
        	c.code += "sub "+reg+","+left+","+right+"\n";
	    	return reg;
        case "*":
        	reg = c.getRegister();
        	c.code += "mul "+reg+","+left+","+right+"\n";
	    	return reg;
        
        	
        
        }
        
      return null;     	
		
	}
	
	/*public Variable eval(Context c) {
		//	System.out.println("Binaryoperator -eval");
	        if (children.size()!=2)
	        	System.out.println("need to operands for "+op);
	        while (true) {
	        Variable condition=children.get(0).eval(c);
	        if (condition.val==0)
	        	break;
	        Variable right =children.get(1).eval(c);
	        }
	        
	        }*/
	public Variable eval(Context c) {
		System.out.println("Binaryoperator -eval "+op);
        if (children.size()!=2)
        	System.out.println("need two operands for "+op);
       
		for (int ii=0;ii<children.size();ii++) {
			System.out.println(children.get(ii).getClass().getName());

		}
        Variable left=children.get(0).eval(c);
        Variable right =children.get(1).eval(c);
        System.out.println("Binaryoperator -eval "+op+
        		" left "+left.val+" right "+right.val);
        Variable v;
        switch (op) {
        case "=":
	    	left.set(right);
	    	System.out.println("assign "+left.name+" = "+left.val);
	    	return null;
        case "+":
        	v = new Variable("",left.val+right.val);
        	
	    	return v;
        case "-":
        	v = new Variable("",left.val-right.val);
        	
	    	return v;
        case "*":
        	v = new Variable("",left.val*right.val);
        	
	    	return v;
        case "==":
        	v = new Variable("",left.val==right.val?1:0);
        	
	    	return v;
        case ">":
        	v = new Variable("",left.val>right.val?1:0);
        	
	    	return v;
        
        	
        
        }
        
      return null;     	
		
	}
}
