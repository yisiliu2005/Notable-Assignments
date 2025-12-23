public class Rectangle extends AbstractElement implements CollisionDetector {
    private float left;
    private float right;
    private float top;
    private float bottom;
    private static int numberOfInstances = 0;

    public Rectangle(){
        super();
        Rectangle.numberOfInstances += 1;

        left = 0;
        right = 0;
        top = 0;
        bottom = 0;
    }
    public Rectangle(float l, float r, float t, float b){
        super();
        Rectangle.numberOfInstances += 1;

        left = l;
        right = r;
        top = t;
        bottom = b;
    }

    public float getLeft(){
        return left;
    }
    public float getRight(){
        return right;
    }
    public float getTop(){
        return top;
    }
    public float getBottom(){
        return bottom;
    }
    public static int getNumOfInstances(){
        return Rectangle.numberOfInstances;
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
        if ((this.left < r.getRight()) &&
            (this.right > r.getLeft()) &&
            (this.top > r.getBottom()) &&
            (this.bottom < r.getTop())) {
            return true;
        }
        return false;
    }
    @Override
    public boolean intersect(Circle c) {
        if (c.getCenter().getX() < left) {
            if ((left - c.getCenter().getX()) >= c.getRadius()) {
                return false;
            }
        }
        if (c.getCenter().getX() > right) {
            if ((c.getCenter().getX() - right) >= c.getRadius()) {
                return false;
            }
        }
        if (c.getCenter().getY() > top) {
            if ((c.getCenter().getY() - top) >= c.getRadius()) {
                return false;
            }
        }
        if (c.getCenter().getY() < bottom) {
            if ((bottom - c.getCenter().getY()) >= c.getRadius()) {
                return false;
            }
        }
        return true;
    }
}
