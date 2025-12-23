public interface CollisionDetector {
    public boolean intersect(Point p);

    public boolean intersect(LineSeg l);

    public boolean intersect(Rectangle r);

    public boolean intersect(Circle c);
}
