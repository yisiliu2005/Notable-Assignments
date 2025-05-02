import static org.junit.jupiter.api.Assertions.*;

import org.junit.jupiter.api.Test;


class A3Test {
	private Point p1 = new Point(0, 0);
	private Point p1copy = new Point(0, 0);
	private Point p2 = new Point(10, 10);
	private Point p55 = new Point(5, 5);

	private LineSeg l1 = new LineSeg(p1, p2);
	private LineSeg l2 = new LineSeg(new Point(0, 5), new Point(5, 0));
	private LineSeg l3 = new LineSeg(new Point(0, 5), new Point(0, 10));

	private Rectangle r1 = new Rectangle(-5, 5, 5, -5);
 	private Rectangle r2 = new Rectangle(15, 20, 20, 15);
	private Rectangle r3 = new Rectangle(0, 10, 10, 0);

	private Circle c1 = new Circle(p1, 5);
	private Circle c1big = new Circle(p1copy, 10);
	private Circle c2 = new Circle(p2, 3);
	private Circle c3 = new Circle(p2, 10);

	@Test
	public void pointIntersectsPoint(){
		assertTrue(p1.intersect(p1copy));
		assertTrue(p1copy.intersect(p1));
	}

	@Test
	public void pointNoIntersectsPoint(){
		assertFalse(p1.intersect(p2));
		assertFalse(p2.intersect(p1));
	}

	@Test
	public void pointIntersectsLine(){
		assertTrue(p55.intersect(l1));
		assertTrue(l1.intersect(p55));
	}

	@Test
	public void pointNoIntersectsLine(){
		assertFalse(p2.intersect(l2));
		assertFalse(l2.intersect(p2));
	}

	@Test
	public void pointIntersectsRectangle(){
		assertTrue(p1.intersect(r1));
		assertTrue(r1.intersect(p1));
	}

	@Test
	public void pointNoIntersectsRectangle(){
		assertFalse(p2.intersect(r2));
		assertFalse(r2.intersect(p2));
	}

	@Test
	public void pointBordersRectangle(){
		assertTrue(p1.intersect(r3));
		assertTrue(r3.intersect(p1));
	}

	@Test
	public void pointIntersectsCircle(){
		assertTrue(p55.intersect(c1big));
		assertTrue(c1big.intersect(p55));
	}

	@Test
	public void pointCentresCircle(){
		assertTrue(p1copy.intersect(c1));
		assertTrue(c1.intersect(p1copy));
	}

	@Test
	public void pointNoIntersectsCircle(){
		assertFalse(p2.intersect(c1));
		assertFalse(c1.intersect(p2));
	}

	@Test
	public void pointBordersCircle(){
		Point p05 = new Point(0, 5);
		assertTrue(p05.intersect(c1));
		assertTrue(c1.intersect(p05));
	}

	@Test
	public void lineIntersectsLine(){
		assertTrue(l1.intersect(l2));
		assertTrue(l2.intersect(l1));
	}

	@Test
	public void lineCoversLine(){
		LineSeg l1half = new LineSeg(p1, p55);
		assertTrue(l1half.intersect(l1));
		assertTrue(l1.intersect(l1half));
	}

	@Test
	public void lineNoIntersectsLine(){
		assertFalse(l1.intersect(l3));
		assertFalse(l3.intersect(l1));
	}

	@Test
	public void lineIntersectsRectangle(){
		assertTrue(l1.intersect(r1));
		assertTrue(r1.intersect(l1));
	}

	@Test
	public void lineNoIntersectsRectangle(){
		assertFalse(l1.intersect(r2));
		assertFalse(r2.intersect(l1));
	}

	@Test
	public void lineBordersRectangle(){
		LineSeg lflat = new LineSeg(p1, new Point(10, 0));
		assertTrue(lflat.intersect(r3));
		assertTrue(r3.intersect(lflat));
	}

	@Test
	public void lineIntersectsCircle(){
		assertTrue(l1.intersect(c1));
		assertTrue(c1.intersect(l1));
	}

	@Test
	public void lineNoIntersectsCircle(){
		assertFalse(l2.intersect(c2));
		assertFalse(c2.intersect(l2));
	}

	@Test
	public void lineTangentCircle(){
		LineSeg tan = new LineSeg(new Point(5, -5), new Point(5, 5));
		assertTrue(c1.intersect(tan));
		assertTrue(tan.intersect(c1));
		
	}

	@Test
	public void rectangleIntersectsRectangle(){
		assertTrue(r1.intersect(r3));
		assertTrue(r3.intersect(r1));
	}

	@Test
	public void rectangleNoIntersectsRectangle(){
		assertFalse(r1.intersect(r2));
		assertFalse(r2.intersect(r1));
	}

	@Test
	public void rectangleBordersRectangle(){
		Rectangle rbottom = new Rectangle(-5, 5, 0, -5);
		assertFalse(rbottom.intersect(r3));
		assertFalse(r3.intersect(rbottom));
	}

	@Test
	public void rectangleIntersectsCircle(){
		assertTrue(r1.intersect(c1));
		assertTrue(c1.intersect(r1));
	}

	@Test
	public void rectangleNoIntersectsCircle(){
		assertFalse(r1.intersect(c2));
		assertFalse(c2.intersect(r1));
	}

	@Test
	public void rectangleBordersCircle(){
		Rectangle rleft = new Rectangle(-5, 0, 5, -5);
		Circle cright = new Circle(new Point(10, 0), 10);
		assertFalse(rleft.intersect(cright));
		assertFalse(cright.intersect(rleft));
	}

	@Test
	public void circleIntersectsCircle(){
		assertTrue(c1.intersect(c3));
		assertTrue(c3.intersect(c1));
	}

	@Test
	public void circleNoIntersectsCircle(){
		assertFalse(c1.intersect(c2));
		assertFalse(c2.intersect(c1));
	}

	@Test
	public void concentricCircles(){
		assertTrue(c1.intersect(c1big));
		assertTrue(c1big.intersect(c1));
	}

	@Test
	public void circleBordersCircle(){
		Circle csr = new Circle(new Point(10, 0), 5);
		assertFalse(csr.intersect(c1));
		assertFalse(c1.intersect(csr));
	}
}
