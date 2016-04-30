#include<GL/glut.h>
#include<vector>
#include<cmath>
#include<set>
using namespace std;
struct point {
private:
	double x, y, z;
public:
	point(double s[3]) :x(s[0]), y(s[1]), z(s[2])
	{}
	point(double xx=0, double yy=0, double zz=0) :x(xx), y(yy), z(zz)
	{}
	point(point s, point s1) :x(s1[0] - s[0]), y(s1[1] - s[1]), z(s1[2] - s[2])
	{}
	double operator[](int index)
	{
		if (index == 0)
			return x;
		if (index == 1)
			return y;
		if (index == 2)
			return z;
	}
	/*void print() {
		cout << x << " " << y << " " << z << endl;
	}*/
	bool operator == (point& f)
	{
		return x == f.x&&y == f.y&&z == f.z;
	}
	bool operator !=(point&f)
	{
		return !(*this == f);
	}
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
	point place;
	point box[4];
	vector<point> points;
	vector<vector<int>> faces;
};
static set<sprite*> ListOfMovingObjects;
class human :private sprite {
public:
	human(vector<point>& m,vector<vector<int>>& s):sprite(m,s) {

	}

};
class player:private human {

public:
	player(vector<point>& s,vector<vector<int>>& f):human(s,f){
		
	}
};
class enemy :private sprite {
	 
};
class bullet : protected sprite {

};
class app {
	
};
class world {

};
int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("The Game");
	

}