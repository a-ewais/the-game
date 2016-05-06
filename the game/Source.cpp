#include<GL/glut.h>
#include<vector>
#include<cmath>
#include<set>
#include<thread>
#include<fstream>
#include<string>
#include<iostream>
#include<mutex>
#include<sstream>
#include<algorithm>
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
class sprite;
set<sprite*> ListOfMovingObjects;
class sprite{
public:
	sprite(vector<point>& m,vector<vector<int>>& s):Vertices(move(m)),faces(move(s)) {
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
//double maxx = -89888, maxy = -857633, minx = 9879835, miny = 45645345, maxz = -45645465, minz = 5464563, aspect;

class world {
private:
	vector<point> V,Vn,Vt;
	vector<vector<vector<int>>> faces;

public:
	void load(string x)
	{
		
		ifstream in(x);
		if (in.is_open())
		{
			vector<thread> threads;
			mutex f;
			while (in.good())
			{
				threads.push_back(thread ([&]() {
					string t;
					f.lock();
					getline(in, t);
					f.unlock();
					stringstream g(t);
					double v[3]; char lala;
					g >> lala;
					if (t[0] == 'v')
					{
						if (t[1] == 'n')
						{
							g >> v[0] >> v[1] >> v[2];
							f.lock();
							Vn.push_back(v);
							f.unlock();
						}
						else if (t[1] == 't')
						{
							g >> v[0] >> v[1] >> v[2];
							v[2] = 0;
							g >> v[2];
							f.lock();
							Vt.push_back(v);
							f.unlock();
						}
						else
						{
							g >> v[0] >> v[1] >> v[2];
							/*maxx = max(maxx, v[0]);
							maxy = max(maxy, v[1]);
							maxz = max(maxz, v[2]);
							minx = min(minx, v[0]);
							miny = min(miny, v[1]);
							minz = min(minz, v[2]);*/
							f.lock();
							V.push_back(v);
							f.unlock();
						}

					}
					else if (t[0] == 'f')
					{
						//cout << t << endl;
						vector<vector<int>> x;
						vector<int>u;
						string e = "";
						for (int i = 2; i < t.size(); i++)
						{
							if (t[i] == '/')
							{
								if (e.size())
									u.push_back(stoi(e));
								else
									u.push_back(-1);
								e.clear();
							}
							else if (t[i] == ' ')
							{
								u.push_back(stoi(e));
								x.push_back(u);
								e.clear();
								u.clear();
							}
							else if (i == t.size() - 1)
							{
								if (t[i] != '/')
								{
									e.push_back(t[i]);
									u.push_back(stoi(e));
									x.push_back(u);
								}
								f.lock();
								faces.push_back(x);
								f.unlock();
							}
							else
								e.push_back(t[i]);
						}
					}
				}));
		
			}
			for (auto &i : threads)
				i.join();
		}

		else
			printf("bad file\n");
	}
	void draw()
	{
		glColor3d(0, 1.0, 1.0);
		for (int j = 0; j < faces.size(); j++)
		{
			if (faces[j].size() == 3)
			{
				glBegin(GL_TRIANGLES);
				for (int i = 0; i < faces[j].size(); i++)
					glVertex3d(V[faces[j][i][0]][0], V[faces[j][i][0]][1], V[faces[j][i][0]][2]);
				glEnd();
			}
			else
			{
				glBegin(GL_POLYGON);
				for (int i = 0; i < faces[j].size(); i++)
					glVertex3d(V[faces[j][i][0]][0], V[faces[j][i][0]][1], V[faces[j][i][0]][2]);
				glEnd();
			}
		}
		glutSwapBuffers();
	}
};

void init1()
{
	glClearColor(0.5, 0.5, 0.5, 0.0);
	glPolygonMode(GL_FRONT, GL_LINE);
	glPolygonMode(GL_BACK, GL_LINE);
	glMatrixMode(GL_PROJECTION);
	glOrtho(- 1000,  1000,  -1000,  1000 , 1000, -1000);
	//aspect = abs(maxx - minx) / abs(maxy - miny);
	/*int w = WinWidth, h = WinHeight;
	if (h*aspect > w)
		h = w / aspect;
	else if (w / aspect > h)
		w = h*aspect;
	glViewport(0, 0, w, h);*/
	glMatrixMode(GL_MODELVIEW);
	//glRotatef(90, 0, 0, 1);
}
void myReshape(int w, int h)
{

	/*if (h*aspect > w)
		h = w / aspect;
	else if (w / aspect > h)
		w = h*aspect;
	glViewport(0, 0, w, h);*/
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1000, 1000, -1000, 1000, 1000, -1000);

	glutPostRedisplay();
}
world g;
void foo()
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	g.draw();
	glutPostRedisplay();
}
int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glEnable(GL_DEPTH_TEST);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(WinWidth, WinHeight);
	glutCreateWindow("The Game");
	init1();	
	g.load("C:\\Users\\mr.3wais\\Desktop\\test.obj");
	glutDisplayFunc(foo);
	glutReshapeFunc(myReshape);
	glutMainLoop();	
	return 0;

}