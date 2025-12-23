public class LineSeg extends AbstractElement implements CollisionDetector {
    private Point begin;
    private Point end;
    private static int numberOfInstances = 0;

    public LineSeg(){
        super();
        LineSeg.numberOfInstances += 1;

        begin = new Point();
        end = new Point();
    }
    public LineSeg(Point b, Point e){
        super();
        LineSeg.numberOfInstances += 1;

        begin = b;
        end = e;
    }

    public Point getBegin(){
        return begin;
    }
    public Point getEnd(){
        return end;
    }

    public static int getNumOfInstances(){
        return LineSeg.numberOfInstances;
    }

    public static float slope(Point start, Point finish){
        float run = start.getX() - finish.getX();
        float rise = start.getY() - finish.getY();
        return rise/run;
    }
    public float slope(){
        return LineSeg.slope(begin, end);
    }

    @Override
    public boolean intersect(Point p) {
        return p.intersect(this);
    }
    @Override
    public boolean intersect(LineSeg l) {
        float thisSlope = this.slope();
        float otherSlope = l.slope();
        //y = mx + b rearranged from point-slope form using begin
        float thisB = begin.getY() - thisSlope*begin.getX();
        float otherB = l.getBegin().getY() - otherSlope*l.getBegin().getX();

        if (thisSlope == otherSlope) {
            if (thisB == otherB) {
                if (begin.intersect(l) || end.intersect(l)) {
                    return true; // inf and touching
                }
            }
            return false; //no touch
        }

        //finding the intersection point since it has one solution
        float a = (otherB - thisB)/(thisSlope - otherSlope); //system of equation with y = mx + b
        float b = thisSlope*a + thisB; //subbing back in for y
        Point intersection = new Point(a, b);
        if (intersection.intersect(l) && intersection.intersect(this)) {
            return true;
        }
        return false;
    }
    @Override
    public boolean intersect(Rectangle r) {
        if (begin.intersect(r) || end.intersect(r)) {
            return true;
        } //in case entire line is inside shape, also shortcut

        LineSeg top = new LineSeg(new Point(r.getLeft(), r.getTop()), new Point(r.getRight(), r.getTop()));
        LineSeg bottom = new LineSeg(new Point(r.getLeft(), r.getBottom()), new Point(r.getRight(), r.getBottom()));
        LineSeg left = new LineSeg(new Point(r.getLeft(), r.getTop()), new Point(r.getLeft(), r.getBottom()));
        LineSeg right = new LineSeg(new Point(r.getRight(), r.getTop()), new Point(r.getRight(), r.getBottom()));

        if (this.intersect(top) || this.intersect(bottom) || this.intersect(left) || this.intersect(right)) {
            return true;
        }
        return false;
    }
    @Override
    public boolean intersect(Circle c) {
        if (begin.intersect(c) || end.intersect(c)) {
            return true;
        } //in case entire line is inside shape, also shortcut

        // (x - h)^2 + (mx + b - k)^2 = r^2 rearranged Ax^2 + Bx + C = 0
        float m = this.slope();
        float b = begin.getY() - m*begin.getX();
        float h = c.getCenter().getX();
        float k = c.getCenter().getY();
        float A = 1 + m*m;
        float B = 2*(m*b - h - m*k);
        float C = h*h + k*k + b*b - 2*b*k - c.getRadius()*c.getRadius();

        // quadratic equation
        float discriminant = B*B - 4*A*C;

        if (discriminant < 0) {
            return false;
        }
        if (discriminant == 0) {
            float intX = (-B)/(2*A);
            float intY = m*intX + b; //subbing back in for y
            Point intersection = new Point(intX, intY);
            if (intersection.intersect(this)) {
                return true;
            }
            return false;
        }
        float x1 = (-B + (float)Math.sqrt(discriminant))/(2*A);
        float y1 = m*x1 + b;
        Point intersection1 = new Point(x1, y1);
        float x2 = (-B - (float)Math.sqrt(discriminant))/(2*A);
        float y2 = m*x2 + b;
        Point intersection2 = new Point(x2, y2);
        if (intersection1.intersect(this) || intersection2.intersect(this)) {
            return true;
        }
        return false;
    }

}
