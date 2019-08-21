import java.util.ArrayList;
import java.util.HashMap;

class Oblig1 {
 public static void main(String[] args) {
 TestCode testCode = new TestCode();

 switch(args.length > 0 ? args[0] : "" ) {
 case "1": testCode.runProgram1(); return;
 case "2": testCode.runProgram2(); return;
 case "3": testCode.runProgram3(); return;
 case "4": testCode.runProgram4(); return;
 case "all": testCode.runAll(); return;
 default: System.out.println("USAGE: java Oblig1 1|2|3|4|all"); return;
 }
 }

}
class TestCode {
  ArrayList<Statement> statements = new ArrayList<Statement>();
  HashMap<String,IdentifierExp> variables = new HashMap<String,IdentifierExp>();
  ArrayList<Statement> whileStatements = new ArrayList<Statement>();
  //lag en liste med variablier
 void runProgram1() {
 // Create the AST based on testing code 1
 // This code is just to help you understand how to create an AST
 Grid grid = new Grid(new NumberExp(64),new NumberExp(64));
 Start start = new Start(new NumberExp(23),new NumberExp(30));
 statements.add(new Move(Direction.west, new NumberExp(15)));
 statements.add(new Move(Direction.south, new NumberExp(15)));
 //er usikker paa hvordan initialisere Arithmetic_exp riktig
 statements.add(new Move(Direction.east, new ArithmeticExp(ArithmeticOp.pluss, new NumberExp(2), new NumberExp(3))));
 statements.add(new Move(Direction.north, new ArithmeticExp(ArithmeticOp.pluss, new NumberExp(10), new NumberExp(27))));
 statements.add(new Stop(start));
 Robot robot = new Robot(start, statements, variables, grid);
 Program prog = new Program(grid, robot);
 // Fill in rest of the code

 // Run the interpreter
 prog.interpret(null);
 }

 void runProgram2() {
   statements.clear();
   variables.clear();
 //same as runProgram1 but with the AST based on testing code 2
 Grid grid = new Grid(new NumberExp(64), new NumberExp(64));
 variables.put("i", new IdentifierExp("i", 5));
 variables.put("j", new IdentifierExp("j", 3));
 Start start = new Start(new NumberExp(23),new NumberExp(6));
 statements.add(new Move(Direction.north, new ArithmeticExp(ArithmeticOp.gange, new NumberExp(3), variables.get("i"))));
 statements.add(new Move(Direction.east, new NumberExp(15)));
 statements.add(new Move(Direction.south, new NumberExp(4)));
 statements.add(new Move(Direction.west, new ArithmeticExp(ArithmeticOp.pluss, new ArithmeticExp(ArithmeticOp.gange, new NumberExp(2), variables.get("i")), new ArithmeticExp(ArithmeticOp.gange, new NumberExp(3), variables.get("j")), new NumberExp(5))));
 statements.add(new Stop(start));
 Robot robot = new Robot(start, statements, variables, grid);
 Program prog = new Program(grid, robot);

 prog.interpret(null);
 }

 void runProgram3() {
   statements.clear();
   variables.clear();
   whileStatements.clear();

   Grid grid = new Grid(new NumberExp(64), new NumberExp(64));
   variables.put("i", new IdentifierExp("i", 5));
   variables.put("j", new IdentifierExp("j", 3));
   Start start = new Start(new NumberExp(23),new NumberExp(6));
   statements.add(new Move(Direction.north, new ArithmeticExp(ArithmeticOp.gange, new NumberExp(3), variables.get("i"))));
   statements.add(new Move(Direction.west, new NumberExp(15)));
   statements.add(new Move(Direction.east, new NumberExp(4)));
   whileStatements.add(new Move(Direction.south, variables.get("j")));
   whileStatements.add(new Assignment(variables.get("j"), ArithmeticOp.minus));
   statements.add(new While(new BoolExp(Bool.bigger, variables.get("j"), new NumberExp(0)), whileStatements));
   statements.add(new Stop(start));

   Robot robot = new Robot(start, statements, variables, grid);
   Program prog = new Program(grid, robot);

   prog.interpret(null);
 }

 void runProgram4() {
   statements.clear();
   variables.clear();
   whileStatements.clear();

   Grid grid = new Grid(new NumberExp(64), new NumberExp(64));
   variables.put("j", new IdentifierExp("j", 3));
   Start start = new Start(new NumberExp(1), new NumberExp(1));
   whileStatements.add(new Move(Direction.north, variables.get("j")));
   statements.add(new While(new BoolExp(Bool.bigger, variables.get("j"), new NumberExp(0)), whileStatements));
   statements.add(new Stop(start));

   Robot robot = new Robot(start, statements, variables, grid);
   Program prog = new Program(grid, robot);

   prog.interpret(null);

 }

 void runAll() {
 runProgram1();
 runProgram2();
 runProgram3();
 runProgram4();
 }
}
interface Robol {
  //sender med robot som en parameter i funksjonen interpret.
  //Dette gjør at statements har faar en referanse til robot.
 void interpret(Robot robot);
}
//klassen til program
class Program implements Robol {
 Grid grid;
 Robot robot;
 public Program(Grid grid, Robot robot) {
 this.grid = grid;
 this.robot = robot;
 }
//test om dette fungerer. Kan den ta imot robot som parameter men
 public void interpret(Robot robot) {
 this.robot.interpret(this.robot);
 }
}

//klassen til robot
class Robot implements Robol {
  Start start;
  ArrayList<Statement> statements;
  Grid grid;
  HashMap<String,IdentifierExp> variables;
  public Robot(Start start, ArrayList<Statement> statements, HashMap<String,IdentifierExp> variables, Grid grid) {
    this.start = start;
    this.statements = statements;
    this.grid = grid;
    this.variables = variables;
  }
 public void interpret(Robot robot) {
   // write interpreter code for the robot. Skal gå gjennom alle statementene.
   //robot trenger også en referanse til grid. Kaller interpret pa statements.
   //System.out.println("Starter roboten. Gaar gjennom " + statements.size() + " statements");
   for (Statement s : statements) {
     //System.out.println("Gaar gjennom en statement");
     s.interpret(robot);
     //System.out.println("Ferdig med en statement");
   }
 }

 public void moveRobot(int x, int y) {
   //System.out.println("Roboten staar paa plass " + start.x + ", " + start.y);

   start.x += x;
   start.y += y;

   if(start.x < 0 || start.y < 0 || start.x > grid.x || start.y > grid.y) {
     System.out.println("Roboten beveget seg av bordet");
     System.exit(0);
   }
   //System.out.println("Roboten byttet plass til " + start.x + ", " + start.y);
   //System.out.println("|");
 }
}

//her folger statements klassene
abstract class Statement implements Robol {
 public abstract void interpret(Robot robot);
}
class Assignment extends Statement {
  IdentifierExp identExp;
  ArithmeticOp arithmeticOp;
  public Assignment (IdentifierExp identExp, ArithmeticOp arithmeticOp) {
    this.identExp = identExp;
    this.arithmeticOp = arithmeticOp;
  }
 public void interpret(Robot robot) {
   switch(arithmeticOp) {

     case pluss:
      identExp.val++;
      break;

     case minus:
      identExp.val--;
      break;
   }
 }
}
class While extends Statement {
 BoolExp condition;
 ArrayList<Statement> statements;

 public While(BoolExp condition, ArrayList<Statement> statements) {
   this.condition = condition;
   this.statements = statements;
 }

 public void interpret(Robot robot) {
   while(condition.get() == 1) {
     for(Statement s: statements) {
       s.interpret(robot);
     }
   }
 }
}

//Klassen til move
class Move extends Statement{
  Direction direction;
  Expression expression;
  public Move(Direction direction, Expression expression) {
    this.direction = direction;
    this.expression = expression;
  }

  public void interpret(Robot robot) {
    //bevegelse sjekker hva slags bevegelse det var den skulle gjøre og gjor det
    //sender dette inn til roboten.
    switch(direction) {

      case north:
        //System.out.println("Beveger seg nord");
        robot.moveRobot(0, expression.get());
        break;
      case south:
        //System.out.println("Beveger seg soor");
        robot.moveRobot(0, (expression.get()*(-1)));
        break;
      case east:
        //System.out.println("Beveger seg ost");
        robot.moveRobot(expression.get(), 0);
        break;
      case west:
        //System.out.println("Beveger seg vest");
        robot.moveRobot((expression.get()*(-1)), 0);
        break;
    }
  }
}

class Stop extends Statement {
  Start start;
  public Stop(Start start) {
    this.start = start;
  }

  public void interpret(Robot robot) {
    System.out.println("The result is (" + start.x + ", " + start.y + ")");
  }
}

//Alle expressions maa extende Expression slik at klasser kan ta imot forskjellige expressions.
abstract class Expression {
  public abstract int get();
}
class BoolExp extends Expression {
 protected Expression left;
 protected Expression right;
 Bool bool;

 public BoolExp(Bool bool, Expression left, Expression right) {
   this.bool = bool;
   this.left = left;
   this.right = right;
 }

 public int get() {
   switch(bool) {

     case bigger:
      if(left.get()>right.get()) {
        return 1;
      } else {
        return -1;
      }
     case smaller:
      if(left.get()<right.get()) {
        return 1;
      } else {
        return -1;
      }
     case equal:
      if(left.get() == right.get()) {
        return 1;
      } else {
        return -1;
      }
   }
   return 0;
 }
}

class IdentifierExp extends Expression {
  String var;
  int val;

  public IdentifierExp(String var, int val) {
    this.var = var;
    this.val = val;
  }

  public int get() {
    return val;
  }

  public String getString() {
    return var;
  }

  public void pluss(int i) {
    val++;
  }
}

class NumberExp extends Expression {
  int i;
  public NumberExp(int i) {
    this.i = i;
  }

  public int get() {
    return i;
  }

}

class ArithmeticExp extends Expression{
  ArithmeticOp arithmeticOp;
  Expression[] a;
  public ArithmeticExp(ArithmeticOp arithmeticOp, Expression ...a) {
    this.arithmeticOp = arithmeticOp;
    this.a = a;
  }

  public int get() {
    int value = 0;
    switch(arithmeticOp) {

      case pluss:
        for(Expression x: a) {
          value += x.get();
        }
        return value;

      case minus:
        value = a[0].get();
        for(int i = 1; i<a.length; i++) {
          value -= a[i].get();
        }
        return value;

      case gange:
        value = a[0].get();
        for(int i = 1; i<a.length; i++) {
          value = value * a[i].get();
        }
        return value;
    }
    return 0;
  }
}

//Klasse start
class Start {
  public int x,y;

  public Start(NumberExp x, NumberExp y) {
    this.x = x.get();
    this.y = y.get();
  }
}

//Klassen til grid
class Grid {
  public int x,y;
  public Grid(NumberExp x, NumberExp y) {
    this.x = x.get();
    this.y = y.get();
  }
}

enum Direction {
  north, south, east, west
}

enum ArithmeticOp {
  pluss, minus, gange
}

enum Bool {
  bigger, smaller, equal
}
