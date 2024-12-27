package ast;

public class Variable {
		Integer val;
		String name;
		
		public Variable(String n,Integer val) {
			name =n;
			this.val = val;
		}
		
		public void set(Variable v) {
			val = v.val;
		}
		
		
}
