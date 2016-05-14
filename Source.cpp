#include<GL/glut.h>
#include<vector>
#include<cmath>
#include<set>
#include <iostream>
#include "SOIL.h"
#include<thread>
#include<fstream>
#include<string>
#include<mutex>
#include<sstream>
#include<algorithm>
#include "point.h"
#include<map>
#include<exception>
using namespace std;
class world;
class loader;
class sprite;
class Human;
class enemy;
class bullet;
set<sprite*> ListOfMovingObjects;
class sprite {
public:
	sprite(vector<point>& Vertices, vector<vector<int>>& faces) :Vertices(move(Vertices)), faces(move(faces)) {
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
	Human(vector<point>& Vertices, vector<vector<int>>& Faces) :sprite(Vertices, Faces) {}
};

class player :private Human {
public:
	player(vector<point>& Vertices, vector<vector<int>>& Faces) :Human(Vertices, Faces) {}
};

class enemy :private sprite {

};

class bullet :private sprite {

};

class app {

};
template <typename T>
class Loader
{
public:
	Loader() {};
	void LoadData(std::string image, T* DP)
	{
		path = image;
		DataPtr = DP;
		LoadDataPriv();
	}
	std::string path;
	map<string, GLuint> MtlToTex;
	map<int,GLuint> TexRange;
	T* DataPtr;
	void LoadDataPriv();
	
	void loadmtl(string file) {
		ifstream in(file);
		if (in.is_open())
		{
			string t, object, photo;
			GLuint y;
			while (!in.eof())
			{
				getline(in, t);
				if (t.find("newmtl") != string::npos)
					object = t.substr(7);
				else if (t.find("map_Ka") != string::npos|| t.find("map_Kd") != string::npos)
				{
					if (t[7] == ' ')
						photo = t.substr(8);
					else
						photo = t.substr(7);
					char*u = new char[photo.length() + 1];
					strcpy(u, photo.c_str());
					y = LoadTexture(u);
					delete[] u;
					MtlToTex[object] = y;
				}
			}
			in.close();
		}
		else
			cout << file<<" is a bad file\n";
	}
	GLuint LoadTexture(char* ImageName)
	{
		cout << ImageName << endl;
		GLuint texture;
		texture = SOIL_load_OGL_texture
			(
				ImageName,
				SOIL_LOAD_AUTO,
				SOIL_CREATE_NEW_ID,
				SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
		
		if (!texture)
			cerr<< "TEXTURE FILE COULD NOT BE LOADED!\n";

		////glBindTexture(GL_TEXTURE_2D, texture);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		return texture;
	}
};
template<typename T>
void Loader<T>::LoadDataPriv() {
	{
		std::ifstream in(path);

		if (in.is_open())
		{
			vector<thread> par;
			while (in.good())
			{
				std::string Line;
				double vectors[3]; char input;
				//Extract First Line
				getline(in, Line);
				std::stringstream DataStream(Line);
				DataStream >> input;
				if (Line.find("mtllib") != std::string::npos)
					//par.push_back(thread([&]() { loadmtl(Line.substr(7)); }));
					loadmtl(Line.substr(7));
				else if (Line.find("usemtl") != std::string::npos)
					TexRange[DataPtr->faces.size()] = MtlToTex[Line.substr(7)];
				
				if (Line[0] == 'v')
				{
				//	cout << " v\n";
					if (Line[1] == 'n')
					{
						DataStream >> input;
						DataStream >> vectors[0] >> vectors[1] >> vectors[2];
						DataPtr->Vn.push_back(vectors);
					}
					else if (Line[1] == 't')
					{
						DataStream >> input;
						DataStream >> vectors[0] >> vectors[1] >> vectors[2];
						vectors[2] = 0;
						DataStream >> vectors[2];
						DataPtr->Vt.push_back(vectors);
					}
					else
					{
						DataStream >> vectors[0] >> vectors[1] >> vectors[2];
						DataPtr->V.push_back(vectors);
						DataPtr->maxx = max(DataPtr->maxx, vectors[0]);
						DataPtr->maxy = max(DataPtr->maxy, vectors[1]);
						DataPtr->maxz = max(DataPtr->maxz, vectors[2]);
						DataPtr->minx = min(DataPtr->minx, vectors[0]);
						DataPtr->miny = min(DataPtr->miny, vectors[1]);
						DataPtr->minz = min(DataPtr->minz, vectors[2]);
					}
				}
				else if (Line[0] == 'f')
				{
					std::vector<std::vector<int>> Face;
					std::vector<int> Point;
					std::string Data = "";

					for (int i = 2; i < Line.size(); i++)
						if (Line[i] == '/')
						{
							if (Data.size())
								Point.push_back(stoi(Data) - 1);
							else
								Point.push_back(-1);

							Data.clear();
						}
						else if (Line[i] == ' ')
						{
							Point.push_back(stoi(Data) - 1);
							if (Point.size() == 1)
								Point.push_back(-1);
							if (Point.size() == 2)
								Point.push_back(-1);
							Face.push_back(Point);
							Data.clear();
							Point.clear();
						}
						else if (i == Line.size() - 1)
						{
							if (Line[i] != '/')
							{
								Data.push_back(Line[i]);
								Point.push_back(stoi(Data) - 1);
								if (Point.size() == 1)
									Point.push_back(-1);
								if (Point.size() == 2)
									Point.push_back(-1);
								Face.push_back(Point);
							}
							DataPtr->faces.push_back(Face);
						}
						else
							Data.push_back(Line[i]);
				}
			}
			DataPtr->aspect = abs(DataPtr->maxx - DataPtr->minx) / abs(DataPtr->maxy - DataPtr->miny);
			DataPtr->center = { DataPtr->maxx - DataPtr->minx,DataPtr->maxy - DataPtr->miny,DataPtr->maxz - DataPtr->minz };
		}
		else
			printf("Bad file!\n");
	}
}

class world {
private:
	std::vector<point> V, Vn, Vt;
	std::vector<std::vector<std::vector<int>>> faces;
	template <class T>
	friend class Loader;
	Loader<world> MapLoader;
public:
	point center;
	double maxx = -89888, maxy = -857633, minx = 9879835, miny = 45645345, maxz = -45645465, minz = 5464563, aspect;
	world();
	void Load(std::string path);
	void Draw();
};
world::world()
{
}
void world::Load(std::string path)
{
	MapLoader.LoadData(path, this);
}

void world::Draw()
{
	glColor3d(0.0, 0.0, 0.0);

	for (int j = 0; j < faces.size(); j++)
	{
		if (MapLoader.TexRange.find(j) != MapLoader.TexRange.end())
		{
			glBindTexture(GL_TEXTURE_2D, MapLoader.TexRange[j]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		if (faces[j].size() == 3)
		{
			glBegin(GL_TRIANGLES);
			for (int i = 0; i < faces[j].size(); i++)
			{
				/*glNormal3d(Vn[faces[j][i][2]][0], Vn[faces[j][i][2]][1], Vn[faces[j][i][2]][2]);
				glTexCoord2d(Vt[faces[j][i][1]][0], Vt[faces[j][i][1]][1]);*/
				glVertex3d(V[faces[j][i][0]][0], V[faces[j][i][0]][1], V[faces[j][i][0]][2]);
			}
			glEnd();
		}
		else
		{
			glBegin(GL_POLYGON);
			for (int i = 0; i < faces[j].size(); i++)
			{
				/*glNormal3d(Vn[faces[j][i][2]][0], Vn[faces[j][i][2]][1], Vn[faces[j][i][2]][2]);
				glTexCoord2d(Vt[faces[j][i][1]][0], Vt[faces[j][i][1]][1]);*/
				glVertex3d(V[faces[j][i][0]][0], V[faces[j][i][0]][1], V[faces[j][i][0]][2]);
			}
			glEnd();
		}
	}

	glutSwapBuffers();
}
world MapA;
int WinWidth = 1280;
int WinHeight = 720;
// actual vector representing the camera's direction
float lx, lz , ly;
// XZ position of the camera
float EyeX = 300, EyeY = 300, EyeZ = 300;
// angle for rotating triangle
float angle = 0.0f;


void SpecialKeys(int key, int x, int y)
{
	float fraction = 2.0f;

	switch (key) {
	case GLUT_KEY_LEFT:
		angle -= 0.05f;
		lx = sin(angle);
		lz = -cos(angle);
		break;
	case GLUT_KEY_RIGHT:
		angle += 0.05f;
		lx = sin(angle);
		lz = -cos(angle);
		break;
	case GLUT_KEY_UP:
		EyeX += lx * fraction;
		EyeY += ly * fraction;
		EyeZ += lz * fraction;
		break;
	case GLUT_KEY_DOWN:
		EyeX -= lx * fraction;
		EyeY -= ly * fraction;
		EyeZ -= lz * fraction;
		break;
	}

}
//void SpecialKeys(int key, int x, int y)
//{
//	glMatrixMode(GL_MODELVIEW);
//	switch (key)
//	{
//	case GLUT_KEY_UP:
//		//glTranslatef(0.0, 5, 0.0);
//		glRotatef(5.0, 1.0, 0.0, 0.0);
//
//		break;
//	case GLUT_KEY_DOWN:
//		glRotatef(5.0, 1.0, 0.0, 0.0);
//		//glTranslatef(0.0, -5.0, 0.0);
//		//keystate[1] = true;
//		break;
//	case GLUT_KEY_LEFT:
//		glRotatef(5.0, 0.0, 1.0, 0.0);
//		//keystate[2] = true;
//		break;
//	case GLUT_KEY_RIGHT:
//		glRotatef(-5.0, 0.0, 1.0, 0.0);
//
//		//keystate[3] = true;
//		break;
//	}
//
//	glutPostRedisplay();
//}
void myKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case 'w':
		angle += 0.05f;
		ly = sin(angle);
		lz = -cos(angle);
		break;
	case 's':
		angle -= 0.05f;
		ly = sin(angle);
		lz = -cos(angle);
		break;
	}
}

void setLight()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	float ambientLight0[] = { 0.0f, 0.0f, 0.0f, 1.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight0);

	float diffuseLight0[] = { 1.0f, 1.0f, 1.0f, 1.0 };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight0);

	float specularLight0[] = { 0.0f, 0.0f, 0.0f, 1.0 };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight0);

	float position0[] = { 20.0f, 40.0f, -50.0f, 0.5 };
	glLightfv(GL_LIGHT0, GL_POSITION, position0);

	float mcolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mcolor);

}

void init()
{
	glClearColor(1.0, 1.0 , 1.0, 0.0);
	glPolygonMode(GL_FRONT, GL_LINES);
	glPolygonMode(GL_BACK, GL_LINES);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH);
	setLight();
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//cout << 2 * MapA.minx << " " << 2 * MapA.maxx << " " <<2 * MapA.miny << " " << 2 * MapA.maxy << " " << -2 * MapA.minz << " " << -2 * MapA.maxz << endl;
	//glOrtho(2 * MapA.minx, 2 * MapA.maxx, 2 * MapA.miny, 2 * MapA.maxy, -2 * MapA.minz, -2 * MapA.maxz);
	//double aspect = MapA.aspect;
	//int w = WinWidth, h = WinHeight;
	//if (h*aspect > w)
	//h = w / aspect;
	//else if (w / aspect > h)
	//w = h*aspect;
	//glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();
	//glOrtho(2 * MapA.minx, 2 * MapA.maxx, 2 * MapA.miny, 2 * MapA.maxy, -2 * MapA.minz, -2 * MapA.maxz);
	// Set the viewport to be the entire window
	//glViewport(0, 0, w, h);

	// Set the correct perspective.
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glRotatef(-90, 1, 0, 0);
}

void myReshape(int w, int h)
{

	if (h == 0)
		h = 1;

	float ratio = w * 1.0 / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();
	//glOrtho(2 * MapA.minx, 2 * MapA.maxx, 2 * MapA.miny, 2 * MapA.maxy, -2 * MapA.minz, -2 * MapA.maxz);
	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45.0f, 1.0, 0.1f, -1000.0f);

	gluLookAt(EyeX, EyeY, EyeZ,
		EyeX + lx, EyeY+ly, EyeZ + lz,
		0.0f, 1.0f, 0.0f);
	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Reset transformations
	glLoadIdentity();
	// Set the camera
	gluLookAt(EyeX, EyeY, EyeZ,
		EyeX + lx, EyeY+ly, EyeZ + lz,
		0.0f, 1.0f, 0.0f);
	MapA.Draw();
}
int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WinWidth, WinHeight);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("The Game");
	glClearColor(0, 0, 0, 0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	MapA.Load("plasma tv.obj");
	lx = MapA.center[0], lz = MapA.center[2], ly = MapA.center[1];
	// XZ position of the camera
	EyeX = 2*MapA.center[0], EyeY = 2*MapA.center[1], EyeZ = 2*MapA.center[2];
	init();
	glutDisplayFunc(&display);
	glutKeyboardFunc(&myKeyboard);
	glutIdleFunc(&display);
	glutSpecialFunc(&SpecialKeys);
	glutReshapeFunc(&myReshape);
	glutMainLoop();
	return 0;
}