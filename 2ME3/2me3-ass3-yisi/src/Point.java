public class Point extends AbstractElement implements CollisionDetector{
    private float x = 0;
    private float y = 0;
    private static int numberOfInstances = 0;

    public Point(){
        super();
        Point.numberOfInstances += 1;
    }
    public Point(float x, float y){
        super();
        Point.numberOfInstances += 1;

        this.x = x;
        this.y = y;
    }

    public float getX(){
        return x;
    }
    public float getY(){
        return y;
    }

    public static int getNumOfInstances(){
        return Point.numberOfInstances;
    }

    @Override
    public boolean intersect(Point p) {
        float otherX = p.getX();
        float otherY = p.getY();

        if ((x != otherX) || (y != otherY)) {
            return false;
        }

        return true;
    }
    @Override
    public boolean intersect(LineSeg l) {
        if ((this.intersect(l.getBegin())) || (this.intersect(l.getEnd()))) {
            return true;
        } //getting an easy case out of the way

        if ((this.x < Math.min(l.getBegin().getX(), l.getEnd().getX())) || 
            (this.x > Math.max(l.getBegin().getX(), l.getEnd().getX())) || 
            (this.y < Math.min(l.getBegin().getY(), l.getEnd().getY())) || 
            (this.y > Math.max(l.getBegin().getY(), l.getEnd().getY()))) {
            return false;
        } //the code below checks if a point is on a line, so before then I need to check if it's within the segment
        
        if (l.slope() == LineSeg.slope(l.getBegin(), this)) {
            return true;
        }
        return false;
    }
    @Override
    public boolean intersect(Rectangle r) {
        if ((x >= r.getLeft()) &&
            (x <= r.getRight()) &&
            (y >= r.getBottom()) &&
            (y <= r.getTop())) {
            return true;
        }
        return false;
    }
    @Override
    public boolean intersect(Circle c) {
        float a = c.getCenter().getX() - this.x;
        float b = c.getCenter().getY() - this.y;
        float distance = (float)Math.sqrt(a*a + b*b);

        if (distance <= c.getRadius()) {
            return true;
        }
        return false;
    }
    
}
