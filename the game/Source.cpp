#include<GL/glut.h>
#include<vector>
#include<cmath>
#include<set>

using namespace std;

int WinWidth = 1280;
int WinHeight = 720;

struct point {

private:
	double x;
	double y;
	double z;
public:
	point(double Points[3]):x(Points[0]), y(Points[1]), z(Points[2]){} //Construct From Array
	point(double PointX = 0, double PointY = 0, double PointZ = 0) :x(PointX), y(PointY), z(PointZ){} //Constructor with 3 arguments
	point(point PointA, point PointB) :x(PointB[0] - PointA[0]), y(PointB[1] - PointA[1]), z(PointB[2] - PointA[2]){} //Merge two points into one

	double operator[](int index)
	{
		if (index == 0)
			return x;

		if (index == 1)
			return y;

		if (index == 2)
			return z;
	}

	point& point::operator=(const point& a)
	{
		x = a.x;
		y = a.y;
		z = a.z;

		return *this;
	}

	bool operator==(const point& a)
	{
		if (x != a.x)
			return false;

		if (y != a.y)
			return false;

		if (z != a.z)
			return false;

		return true;
	}

	bool operator!=(const point&a)
	{
		return !(*this == a);
	}

	double* point::data()
	{
		double a[] = { x,y,z };
		return a;
	}

	/*void print() {
	cout << x << " " << y << " " << z << endl;
	}*/
};

static set<sprite*> ListOfMovingObjects;

class sprite{
public:
	sprite(vector<point>& m,vector<vector<int>>& s):points(move(m)),faces(move(s)) {
		ListOfMovingObjects.insert(this);
	}
	virtual void update() = 0;
	~sprite()
	{
		ListOfMovingObjects.erase(this);
	}
private:
	point Center;
	point Box[4];
	vector<point> Vertices;
	vector<vector<int>> faces;
};

static set<sprite*> ListOfMovingObjects;

class Human :private sprite {
public:
	Human(vector<point>& Vertices, vector<vector<int>>& Faces):sprite(Vertices,Faces) {}
};

class player:private Human {
public:
	player(vector<point>& Vertices, vector<vector<int>>& Faces):Human(Vertices, Faces){}
};

class enemy :private sprite {
	 
};

class bullet :private sprite {

};

class app {
	
};

class world {

};

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(WinWidth, WinHeight);
	glutCreateWindow("The Game");
	
}