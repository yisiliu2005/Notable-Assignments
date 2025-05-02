public class Circle extends AbstractElement implements CollisionDetector {
    private Point center;
    private float radius;
    private static int numberOfInstances;

    public Circle(){
        super();
        Circle.numberOfInstances += 1;

        center = new Point();
        radius = 0;
    }
    public Circle(Point c, float r){
        super();
        Circle.numberOfInstances += 1;

        center = c;
        radius = r;
    }

    public Point getCenter(){
        return center;
    }
    public float getRadius(){
        return radius;
    }
    public static int getNumOfInstances(){
        return Circle.numberOfInstances;
    }

    @Override
    public boolean intersect(Point p) {
        return p.intersect(this);
    }
    @Override
    public boolean intersect(LineSeg l) {
        return l.intersect(this);
    }
    @Override
    public boolean intersect(Rectangle r) {
        return r.intersect(this);
    }
    @Override
    public boolean intersect(Circle c) {
        float a = center.getX() - c.getCenter().getX();
        float b = center.getY() - c.getCenter().getX();
        float distance = (float)Math.sqrt(a*a + b*b);

        if ((radius + c.getRadius()) <= distance) {
            return false;
        }
        return true;
    }
}
