enum Direction {
  north, south, east, west
}

class EnumTest {
  Direction direction;

  public EnumTest(Direction direction) {
    this.direction = direction;
  }

  public void hvilkenRetning() {
    switch(direction) {
      case north:
        System.out.println("Retning nord");
        break;

      case south:
        System.out.println("Retning sor");
        break;

      case east:
        System.out.println("Retning ost");
        break;

      case west:
        System.out.println("Retning vest");
        break;
    }
  }
}

public class MainEnumTest {
  public static void main(String[] args) {

    EnumTest enumTest = new EnumTest(Direction.north);
    enumTest.hvilkenRetning();
  }
}
