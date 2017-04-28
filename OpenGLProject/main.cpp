/*

Writer: MEHMET MELİH ARICI
Department of Computer Science
S004031

CS 422 - Computer Graphics - Project :  RACE THE SUN (Part 3) -- Texture Mapping


You can adjust velocity by changing the value of VELOCITY. (if it's too slow or fast)


*/

#define _CRT_SECURE_NO_WARNINGS

#include "RGBpixmap.h"
#include <GL/glut.h>
#include <iostream>
#include <list>
#include <math.h>
#include <typeinfo>
#include <sstream>


#define PI 3.14159265
#define VELOCITY 0.3

using namespace std;

GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

RGBpixmap pix[9]; // 8 empty pixmap, one of them for terrain.

class Util {
public:
	static GLfloat** create2Darray(int nrows, int ncols) {

		GLfloat** ary = new GLfloat*[nrows];
		for (int i = 0; i < nrows; ++i)
			ary[i] = new GLfloat[ncols];
		return ary;
	}

	static GLint** create2Dintarray(int nrows, int ncols) {

		GLint** ary = new GLint*[nrows];
		for (int i = 0; i < nrows; ++i)
			ary[i] = new GLint[ncols];
		return ary;
	}
};


class Point {
public:
	GLfloat x, y, z;
	Point(GLfloat a, GLfloat b, GLfloat c)
	{
		x = a; y = b; z = c;
	}
};


class WallRect {
public:

	Point* normal;
	list<Point*> pointsOfRectangle;
	GLfloat x_center, y_center, z_center;
	GLfloat x_total, y_total, z_total = 0.0;
	GLfloat plane_x = 0.0, plane_y = -7.0, plane_z = 5.0;
	GLfloat orientationAngle;


	WallRect(GLfloat angle, Point* norm, list<Point*> list)
	{
		normal = norm;
		pointsOfRectangle = list;
		orientationAngle = angle;
		Point* c = center();
	}

	Point* center()
	{
		for (std::list<Point*>::iterator it = pointsOfRectangle.begin(); it != pointsOfRectangle.end(); it++) {
			Point* p = *it;
			x_total += p->x;
			y_total += p->y;
			z_total += p->z;
		}

		x_center = x_total / 4.0;
		y_center = y_total / 4.0;
		z_center = z_total / 4.0;

		return (new Point(x_center, y_center, z_center));
	}

	double distance(double x, double z)
	{
		double planeX = 0.0;
		double planeZ = 0.0;
		double xSqr = (x - planeX) * (x - planeX);
		double zSqr = (z - planeZ) * (z - planeZ);

		return (sqrt(xSqr + zSqr));
	}


	GLfloat newAngle(double currentAngle)
	{
		GLfloat ang = 0.0;

		if ((orientationAngle + currentAngle) >= 360)
		{
			ang = (orientationAngle + currentAngle) - 360;
		}
		else if ((orientationAngle + currentAngle) < 0)
		{
			ang = (orientationAngle + currentAngle) + 360;
		}
		else {
			ang = (orientationAngle + currentAngle);
		}

		return ang;
	}

	double distFromSphere()
	{
		GLfloat normX = normal->x + x_center;
		GLfloat normY = normal->y + y_center;
		GLfloat normZ = normal->z + z_center;


		double d = (-normX * x_center) + (-normY * y_center) + (-normZ * z_center);

		// plane equation.
		double dist = (normX * plane_x) + (normY * plane_y) + (normZ * plane_z) + d;

		return dist;
	}


	bool collidesWith(double currentAngle)
	{

		bool flag = false;

		if (distFromSphere() < 1.0)
		{

			flag = true;
		}

		return flag;
	}



};


class Model {

protected:
	GLfloat** vertices;
	GLint** faces;
	GLfloat** normals;
	GLfloat position[3];
	GLfloat orientation[3];
	GLfloat color[4];
	list<WallRect*> rectangles;
	double alt = -6.0;
	bool onCollision = false;


public:

	Model(GLfloat x, GLfloat y, GLfloat z, GLfloat yaw,
		GLfloat pitch, GLfloat roll,
		GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {

		position[0] = x; position[1] = y; position[2] = z;
		orientation[0] = yaw; orientation[1] = pitch; orientation[2] = roll;
		color[0] = red; color[1] = green; color[2] = blue; color[3] = alpha;
	}

	GLint** getFaces() { return faces; }
	GLfloat** getNormals() { return normals; }

	virtual void draw() = 0;
	virtual bool collidesWith(double currentAngle) = 0;
	virtual string type() = 0;

	bool isOnCollision()
	{
		return onCollision;
	}

	void setIsOnCollision(bool flag)
	{
		onCollision = flag;
	}

	GLfloat* getPosition() { return position; }

	GLfloat* getOrientation() { return orientation; }

	void addPlaneAltitude(double num)
	{
		alt += num;
	}

	double getPlaneAltitude()
	{
		return alt;
	}


	void addRectangle(WallRect* rect) { rectangles.push_back(rect); }

	list<WallRect*> getRectangles()
	{
		return rectangles;
	}

	void setPosition(GLfloat x, GLfloat y, GLfloat z) { position[0] = x; position[1] = y; position[2] = z; }

	void setZPosition(GLfloat z)
	{
		position[2] = z;
	}

	void setXPosition(GLfloat x)
	{
		position[0] = x;
	}


	void goForward(GLfloat num)
	{
		position[2] -= num;
	}

	void goBack(GLfloat num)
	{
		position[2] += num;
	}

	void goUp(GLfloat num)
	{
		position[1] -= num;
	}

	void goDown(GLfloat num)
	{
		position[1] += num;
	}

	void goRight(GLfloat num)
	{
		position[0] -= num;
	}

	void goLeft(GLfloat num)
	{
		position[0] += num;
	}

	void addXvelocity(double num)
	{
		position[0] += num;
	}

	void addZvelocity(double num)
	{
		position[2] += num;
	}

	void turnRight(GLfloat degree)
	{
		orientation[1] += degree;
	}

	void turnLeft(GLfloat degree)
	{
		orientation[1] -= degree;
	}


	double distanceToCenter()
	{
		double planeX = 0.0;
		double planeY = -6.0;
		double planeZ = 5.0;

		double xSqr = (position[0] - planeX) * (position[0] - planeX);
		double ySqr = (position[1] - planeY) * (position[1] - planeY);
		double zSqr = (position[2] - planeZ) * (position[2] - planeZ);

		return (sqrt(xSqr + ySqr + zSqr));
	}

	double distanceToCenter2(double x, double y, double z)
	{
		double planeX = 0.0;
		double planeY = alt;
		double planeZ = 5.0;

		double xSqr = (x - planeX) * (x - planeX);
		double ySqr = (y - planeY) * (y - planeY);
		double zSqr = (z - planeZ) * (z - planeZ);

		return (sqrt(xSqr + ySqr + zSqr));
	}


};




class Terrain : public Model {
public:
	Terrain(GLfloat x, GLfloat y, GLfloat z, GLfloat yaw,
		GLfloat pitch, GLfloat roll,
		GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) :
		Model(x, y, z, yaw, pitch, roll, red, green, blue, alpha) { // Call the base constructor
		vertices = Util::create2Darray(4, 3);
		normals = Util::create2Darray(1, 3);
		faces = Util::create2Dintarray(1, 4);

		vertices[0][0] = -1000.0; vertices[0][1] = 0.0; vertices[0][2] = -1000.0;
		vertices[1][0] = 1000.0; vertices[1][1] = 0.0; vertices[1][2] = -1000.0;
		vertices[2][0] = 1000.0; vertices[2][1] = 0.0; vertices[2][2] = 1000.0;
		vertices[3][0] = -1000.0; vertices[3][1] = 0.0; vertices[3][2] = 1000.0;

		normals[0][0] = 0.0; normals[0][1] = 1.0; normals[0][2] = 0.0;

		faces[0][0] = 0; faces[0][1] = 1; faces[0][2] = 2; faces[0][3] = 3; // faces[0][0] = 0 // according to rows order.


	};

	void addRectangles()
	{

	}

	bool collidesWith(double currentAngle)
	{
		return false;
	}

	string type()
	{
		return "Terrain";
	}

	void draw() {
		int i;

	
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glPushMatrix();
		glLightfv(GL_LIGHT0, GL_DIFFUSE, color);

		
		glTranslatef(position[0], position[1], position[2]);

		

		glBindTexture(GL_TEXTURE_2D,2007);
		glBegin(GL_QUADS);
		glNormal3fv(&normals[0][0]);
		glTexCoord2f(-50.0, -50.0);  glVertex3fv(&vertices[faces[0][0]][0]);
		glTexCoord2f(50.0, -50.0);   glVertex3fv(&vertices[faces[0][1]][0]);
		glTexCoord2f(50.0, 50.0);    glVertex3fv(&vertices[faces[0][2]][0]);
		glTexCoord2f(-50.0, 50.0);   glVertex3fv(&vertices[faces[0][3]][0]);
		glEnd();

		glPopMatrix();
		

	}


};

class Plane : public Model {
private:
	GLfloat altitude = 0.0;

public:
	Plane(GLfloat x, GLfloat y, GLfloat z, GLfloat yaw, GLfloat pitch, GLfloat roll, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
		: Model(x, y, z, yaw, pitch, roll, red, green, blue, alpha)
	{
		altitude = y;
	}

	void addAltitude(GLfloat num)
	{
		altitude += num;
	}

	GLfloat getAltitude()
	{
		return altitude;
	}

	bool collidesWith(double currentAngle)
	{
		return false;
	}

	string type()
	{
		return "Plane";
	}

	void draw()
	{
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glPushMatrix();
		glLightfv(GL_LIGHT0, GL_DIFFUSE, color);


		GLfloat ambientColor[3];
		ambientColor[0] = color[0];
		ambientColor[1] = color[1];
		ambientColor[2] = color[2];

		glLightfv(GL_LIGHT1, GL_AMBIENT, color);

		glTranslatef(position[0], position[1], position[2]);

		glBindTexture(GL_TEXTURE_2D, 2008);

		glTexCoord2f(0.0 , 0.0);



		// Initialization
		GLUquadricObj *quadric;
		quadric=gluNewQuadric();
		gluQuadricNormals(quadric, GLU_SMOOTH);
		gluQuadricTexture(quadric, GL_TRUE);

		// Render
		glPushMatrix();
		glTranslated(0,0,-3);
		gluSphere(quadric,0.75,50,50);
		glPopMatrix();

		//glutSolidSphere(0.75, 50, 50);   // our plane

		glPopMatrix();

	}



};


class Bonus : public Model {
public:
	Bonus(GLfloat x, GLfloat y, GLfloat z, GLfloat yaw, GLfloat pitch, GLfloat roll, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
		: Model(x, y, z, yaw, pitch, roll, red, green, blue, alpha)
	{

	}

	bool collidesWith(double currentAngle)
	{
		bool flag = false;
		if (distanceToCenter() < (1.6))
		{
			flag = true;
		}
		return flag;
	}

	string type()
	{
		return "Bonus";
	}

	void draw()
	{
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glPushMatrix();
		glLightfv(GL_LIGHT0, GL_DIFFUSE, color);

		GLfloat ambientColor[3];
		ambientColor[0] = color[0];
		ambientColor[1] = color[1];
		ambientColor[2] = color[2];

		glLightfv(GL_LIGHT1, GL_AMBIENT, color);

		glTranslatef(position[0], position[1], position[2]);

		glBindTexture(GL_TEXTURE_2D, 2009);

		glTexCoord2f(0.0 , 0.0);


		// Initialization
		GLUquadricObj *quadric;
		quadric=gluNewQuadric();
		gluQuadricNormals(quadric, GLU_SMOOTH);
		gluQuadricTexture(quadric, GL_TRUE);

		// Render
		glPushMatrix();
		glTranslated(0,0,-3);
		gluSphere(quadric,0.55,50,50);  // 0.55
		glPopMatrix();


		glPopMatrix();
	}
};


class Wall : public Model {
public:
	GLfloat orientationAngle;
	list<Point*> innerSphereCenters;
	double x_total, y_total, z_total = 0.0;

	int h;

	Wall(GLfloat x, GLfloat y, GLfloat z, GLfloat yaw, GLfloat pitch, GLfloat roll, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha, int height, int width, int diameter)
		: Model(x, y, z, yaw, pitch, roll, red, green, blue, alpha)
	{

		h = height;

		vertices = Util::create2Darray(8, 3);
		normals = Util::create2Darray(6, 3);
		faces = Util::create2Dintarray(6, 4);

		int minusHeight = 0 - height;
		int minusWidth = 0 - width;
		int minusDiameter = 0 - diameter;

		vertices[0][0] = minusDiameter; vertices[0][1] = minusHeight; vertices[0][2] = width;
		vertices[1][0] = minusDiameter; vertices[1][1] = minusHeight; vertices[1][2] = minusWidth;
		vertices[2][0] = minusDiameter; vertices[2][1] = height; vertices[2][2] = minusWidth;
		vertices[3][0] = minusDiameter; vertices[3][1] = height; vertices[3][2] = width;
		vertices[4][0] = diameter; vertices[4][1] = minusHeight; vertices[4][2] = width;
		vertices[5][0] = diameter; vertices[5][1] = minusHeight; vertices[5][2] = minusWidth;
		vertices[6][0] = diameter; vertices[6][1] = height; vertices[6][2] = minusWidth;
		vertices[7][0] = diameter; vertices[7][1] = height; vertices[7][2] = width;

		normals[0][0] = -1.0; normals[0][1] = 0.0; normals[0][2] = 0.0;
		normals[1][0] = 0.0; normals[1][1] = 1.0; normals[1][2] = 0.0;
		normals[2][0] = 1.0; normals[2][1] = 0.0; normals[2][2] = 0.0;
		normals[3][0] = 0.0; normals[3][1] = -1.0; normals[3][2] = 0.0;
		normals[4][0] = 0.0; normals[4][1] = 0.0; normals[4][2] = 1.0;
		normals[5][0] = 0.0; normals[5][1] = 0.0; normals[5][2] = -1.0;

		faces[0][0] = 0; faces[0][1] = 1; faces[0][2] = 2; faces[0][3] = 3;
		faces[1][0] = 3; faces[1][1] = 2; faces[1][2] = 6; faces[1][3] = 7;
		faces[2][0] = 7; faces[2][1] = 6; faces[2][2] = 5; faces[2][3] = 4;
		faces[3][0] = 4; faces[3][1] = 5; faces[3][2] = 1; faces[3][3] = 0;
		faces[4][0] = 5; faces[4][1] = 6; faces[4][2] = 2; faces[4][3] = 1;
		faces[5][0] = 7; faces[5][1] = 4; faces[5][2] = 0; faces[5][3] = 3;

		orientationAngle = orientation[1];


	};


	void addInnerCenter(Point* center) { innerSphereCenters.push_back(center); }

	list<Point*> getInnerSphereCenters()
	{
		return innerSphereCenters;
	}

	void addInnerCenters()
	{
		innerSphereCenters.clear();
		int numOfCenter = h / 2;

		double x_center = position[0];
		double y_center = position[1];
		double z_center = position[2];

		double radius = 2.0;

		for (int y = (h - radius); y > -6; y -= 2 * radius)
		{
			Point* p = new Point(x_center, y, z_center);
			addInnerCenter(p);
		}
	}

	void addRectangles()
	{
		for (int i = 0; i < 6; i++)
		{
			list<Point*> pointsOfRectangle;
			GLfloat normX = normals[i][0];
			GLfloat normY = normals[i][1];
			GLfloat normZ = normals[i][2];

			Point* norm = new Point(normX, normY, normZ);

			for (int j = 0; j < 4; j++)
			{
				GLfloat p_x = position[0] + (&vertices[faces[i][j]][0])[0];
				GLfloat p_y = position[1] + (&vertices[faces[i][j]][0])[1];
				GLfloat p_z = position[2] + (&vertices[faces[i][j]][0])[2];

				Point* p = new Point(p_x, p_y, p_z);

				pointsOfRectangle.push_back(p);
			}

			WallRect* rect = new WallRect(orientationAngle, norm, pointsOfRectangle);

			x_total += rect->center()->x;
			y_total += rect->center()->y;
			z_total += rect->center()->z;


		}
	}

	bool collidesWith(double currentAngle)
	{
		addRectangles();
		addInnerCenters();
		bool flag = false;
		for (std::list<Point*>::iterator it = innerSphereCenters.begin(); it != innerSphereCenters.end(); ++it) {
			Point* center = *it;
			double distanceToPlane = distanceToCenter2(center->x, center->y, center->z);
			if (distanceToPlane < (5.5))
			{
				flag = true;
			}
		}
		return flag;
	}

	string type()
	{
		return "Wall";
	}

	void draw() {

		int i;

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glPushMatrix();
		glLightfv(GL_LIGHT0, GL_DIFFUSE, color);

		GLfloat ambientColor[3];
		ambientColor[0] = color[0];
		ambientColor[1] = color[1];
		ambientColor[2] = color[2];

		glLightfv(GL_LIGHT1, GL_AMBIENT, color);

		glTranslatef(position[0], position[1], position[2]);
		glRotatef(orientation[0], 1.0, 0.0, 0.0);
		glRotatef(orientation[1], 0.0, 1.0, 0.0);
		glRotatef(orientation[2], 0.0, 0.0, 1.0);

		int num = 2001;

	//	double x_val = 2.0;
	//	double y_val = 2.0;

		for (i = 0; i < 6; i++) {

			glBindTexture(GL_TEXTURE_2D, (num + i));
			glBegin(GL_QUADS);
			glNormal3fv(&normals[i][0]);
			glTexCoord2f(0.0 , 0.0);  glVertex3fv(&vertices[faces[i][0]][0]);
			glTexCoord2f(0.0 , 2.0);   glVertex3fv(&vertices[faces[i][1]][0]);
			glTexCoord2f(2.0 , 2.0);    glVertex3fv(&vertices[faces[i][2]][0]);
			glTexCoord2f(2.0 , 0.0);   glVertex3fv(&vertices[faces[i][3]][0]);
			glEnd();

	//		x_val = 2.0;
	//		y_val = 2.0;  // reset them.

		//	/* DENEME AMAÇLIDIR */
		//
		//	cout << "i = " << i << endl;
		//
		//	double x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4;
		//
		//	x1 = (&vertices[faces[i][0]][0])[0];
		//	y1 = (&vertices[faces[i][0]][0])[1];
		//	z1 = (&vertices[faces[i][0]][0])[2];
		//
		//	x2 = (&vertices[faces[i][0]][0])[0];
		//	y2 = (&vertices[faces[i][0]][0])[1];
		//	z2 = (&vertices[faces[i][0]][0])[2];
		//
		//	x3 = (&vertices[faces[i][0]][0])[0];
		//	y3 = (&vertices[faces[i][0]][0])[1];
		//	z3 = (&vertices[faces[i][0]][0])[2];
		//
		//	x4 = (&vertices[faces[i][0]][0])[0];
		//	y4 = (&vertices[faces[i][0]][0])[1];
		//	z4 = (&vertices[faces[i][0]][0])[2];
		//
		//	cout << "v1 = (" << x1 << " , " << y1 << " , " << z1 << " ) " << endl;
		//	cout << "v2 = (" << x2 << " , " << y2 << " , " << z2 << " ) " << endl;
		//	cout << "v3 = (" << x3 << " , " << y3 << " , " << z3 << " ) " << endl;
		//	cout << "v4 = (" << x4 << " , " << y4 << " , " << z4 << " ) " << endl;
		//
		//	cout << "---------------------" << endl;
		//
		//	/* -----------------------------------  */
		}

		glPopMatrix();
	}

};


class Scene {
private:
	std::list<Model*> scene_graph;


public:

	GLfloat cameraX = 0.0;
	double currentAngle = 90.0;
	GLfloat constantFactor = VELOCITY;

	double x_velocity;
	double z_velocity;

	void setXvelocity(double vel)
	{
		x_velocity = vel;
	}

	void setZvelocity(double vel)
	{
		z_velocity = vel;
	}

	void addCameraX(double num)
	{
		cameraX += num;
	}

	void addCurrentAngle(double add)
	{
		if ((currentAngle + add) >= 360.0)
		{
			currentAngle = (currentAngle + add) - 360.0;
		}
		else if ((currentAngle + add) < 0.0)
		{
			currentAngle = (currentAngle + add) + 360.0;
		}
		else
		{
			currentAngle = currentAngle + add;
		}

	}

	double getAngle()
	{
		return currentAngle;
	}

	void  changeCurrentAngle(double val)
	{
		currentAngle = val;
	}

	void draw() {
		Model* prevmodel;


		x_velocity = constantFactor * cos(currentAngle*PI / 180);
		z_velocity = (constantFactor * sin(currentAngle*PI / 180));


		for (std::list<Model*>::iterator it = scene_graph.begin(); it != scene_graph.end(); it++) {
			Model* curmodel = *it;

			if (currentAngle <= 135 && currentAngle > 45) {    // CASE 1

				if (curmodel->getPosition()[2] >= 10) {

					curmodel->setIsOnCollision(false);

					int mean1 = -220 * sin(currentAngle*PI / 180);
					curmodel->setZPosition(rand() % ((mean1)-(mean1 - 150) + 1) + (mean1 - 150));

					int mean2 = 140 * cos(currentAngle*PI / 180);
					curmodel->setXPosition(rand() % ((mean2 + 100) - (mean2 - 100) + 1) + (mean2 - 100));

				}
			}
			else if (currentAngle > 135 && currentAngle <= 225) // CASE 2. 
			{
				if (curmodel->getPosition()[0] >= 10) {

					curmodel->setIsOnCollision(false);

					int mean1 = -220 * sin(currentAngle*PI / 180);
					curmodel->setZPosition(rand() % ((mean1 + 100) - (mean1 - 100) + 1) + (mean1 - 100));

					int mean2 = 140 * cos(currentAngle*PI / 180);
					curmodel->setXPosition(rand() % ((mean2)-(mean2 - 150) + 1) + (mean2 - 150));
				}
			}
			else if (currentAngle > 225 && currentAngle <= 315)  // CASE 3. 
			{
				if (curmodel->getPosition()[2] <= -10) {

					curmodel->setIsOnCollision(false);

					int mean1 = -220 * sin(currentAngle*PI / 180);
					curmodel->setZPosition(rand() % ((mean1 + 150) - (mean1)+1) + (mean1));

					int mean2 = 140 * cos(currentAngle*PI / 180);
					curmodel->setXPosition(rand() % ((mean2 + 100) - (mean2 - 100) + 1) + (mean2 - 100));

				}
			}
			else if ((currentAngle > 315 && currentAngle < 360) || (currentAngle >= 0 && currentAngle <= 45)) // CASE 4 
			{
				if (curmodel->getPosition()[0] <= -10) {

					curmodel->setIsOnCollision(false);

					int mean1 = -220 * sin(currentAngle*PI / 180);
					curmodel->setZPosition(rand() % ((mean1 + 100) - (mean1 - 100) + 1) + (mean1 - 100)); // 50 ler

					int mean2 = 140 * cos(currentAngle*PI / 180);
					curmodel->setXPosition(rand() % ((mean2 + 150) - (mean2)+1) + (mean2));

				}
			}
			else {
				cout << "Error angle : " << currentAngle << endl;
			}

			curmodel->addZvelocity(z_velocity);
			curmodel->addXvelocity(-x_velocity);
			curmodel->draw();
		}
	}

	list<Model*> getGraph()
	{
		return scene_graph;
	}

	void addModel(Model* newModel) { scene_graph.push_back(newModel); }
};

Scene* scene;
Plane* plane;
GLfloat altitude = -6.0;
int total_point = 0;
bool firstFlag = true;

GLfloat getAltitude()
{
	return altitude;
}

bool isRegionContainsObject(int x1, int x2, int z1, int z2)
{
	list<Model*> scene_graph = scene->getGraph();
	bool flag = false;
	for (std::list<Model*>::iterator it = scene_graph.begin(); it != scene_graph.end(); it++)
	{
		Model* curmodel = *it;
		int x = curmodel->getPosition()[0];
		int z = curmodel->getPosition()[2];
		if (x > x1 && x < x2 && z > z1 && z < z2)
		{
			flag = true;
		}
	}
	return flag;
}

void checkNearObjects()
{

	list<Model*> scene_graph = scene->getGraph();
	for (std::list<Model*>::iterator it = scene_graph.begin(); it != scene_graph.end(); it++)
	{
		Model* curmodel = *it;
		int x = curmodel->getPosition()[0];
		int z = curmodel->getPosition()[2];
		if (x > -10 && x < 10 && z > -10 && z < 10)
		{
			if (curmodel->collidesWith(scene->currentAngle) && !curmodel->isOnCollision())
			{
				if (curmodel->type().compare("Wall") == 0)
				{

					Wall* w = (Wall*)curmodel;

					if (!(curmodel->getPlaneAltitude() + 6.0 > (w->h) + 0.75))
					{
						total_point -= 10;

						double centerXwall = curmodel->getPosition()[0]; // x coordinate of collision wall

						if (centerXwall == 0)  // head to head
						{

							for (std::list<Model*>::iterator it = scene_graph.begin(); it != scene_graph.end(); it++)
							{
								Model* curmodel = *it;
								curmodel->goForward(1000 * (scene->z_velocity));
							}
						}
						else if (centerXwall < 0)  // wall coming from left
						{

							if (scene->currentAngle <= 135 && scene->currentAngle > 45) {    // CASE 1

								for (std::list<Model*>::iterator it = scene_graph.begin(); it != scene_graph.end(); it++)
								{
									Model* curmodel = *it;
									curmodel->goRight(5);
								}

							}
							else if (scene->currentAngle > 135 && scene->currentAngle <= 225) // CASE 2. 
							{
								// x aynı , z tam tersine
								for (std::list<Model*>::iterator it = scene_graph.begin(); it != scene_graph.end(); it++)
								{
									Model* curmodel = *it;
									curmodel->goBack(5);
								}

							}
							else if (scene->currentAngle > 225 && scene->currentAngle <= 315)  // CASE 3. 
							{
								// z aynı , x tam tersine
								for (std::list<Model*>::iterator it = scene_graph.begin(); it != scene_graph.end(); it++)
								{
									Model* curmodel = *it;
									curmodel->goRight(5);
								}

							}
							else if ((scene->currentAngle > 315 && scene->currentAngle < 360) || (scene->currentAngle >= 0 && scene->currentAngle <= 45)) // CASE 4 
							{
								// x aynı , z tam tersine
								for (std::list<Model*>::iterator it = scene_graph.begin(); it != scene_graph.end(); it++)
								{
									Model* curmodel = *it;
									curmodel->goForward(10);
								}
							}
							else {
								// nothing.
							}

						}
						else if (centerXwall > 0) // wall coming from right
						{
							if (scene->currentAngle <= 135 && scene->currentAngle > 45) {    // CASE 1

								// z aynı , x tam tersine
								for (std::list<Model*>::iterator it = scene_graph.begin(); it != scene_graph.end(); it++)
								{
									Model* curmodel = *it;
									curmodel->goLeft(5);
								}

							}
							else if (scene->currentAngle > 135 && scene->currentAngle <= 225) // CASE 2. 
							{
								// x aynı , z tam tersine
								for (std::list<Model*>::iterator it = scene_graph.begin(); it != scene_graph.end(); it++)
								{
									Model* curmodel = *it;
									curmodel->goForward(10);
								}
							}
							else if (scene->currentAngle > 225 && scene->currentAngle <= 315)  // CASE 3. 
							{
								// z aynı , x tam tersine
								for (std::list<Model*>::iterator it = scene_graph.begin(); it != scene_graph.end(); it++)
								{
									Model* curmodel = *it;
									curmodel->goLeft(5);
								}

							}
							else if ((scene->currentAngle > 315 && scene->currentAngle < 360) || (scene->currentAngle >= 0 && scene->currentAngle <= 45)) // CASE 4 
							{
								// x aynı , z tam tersine
								for (std::list<Model*>::iterator it = scene_graph.begin(); it != scene_graph.end(); it++)
								{
									Model* curmodel = *it;
									curmodel->goBack(5);
								}


							}
							else {
								// nothing.
							}

						}
					}

				}
				else if (curmodel->type().compare("Bonus") == 0)
				{
					total_point += 10;

					if (firstFlag)
					{
						total_point -= 110;
						firstFlag = false;
					}
				}

				curmodel->setIsOnCollision(true);
			}
		}
	}

}

void printtext(int x, int y, string String)
{
	//(x,y) is from the bottom left of the window
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 800, 0, 800, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);
	glRasterPos2i(x, y);
	//	glColor3f(1,1,0);

	GLfloat color[4];
	color[0] = 0.196078;
	color[1] = 0.6;
	color[2] = 0.8;
	color[3] = 1.0;

	glLightfv(GL_LIGHT0, GL_DIFFUSE, color);

	GLfloat ambientColor[3];
	ambientColor[0] = color[0];
	ambientColor[1] = color[1];
	ambientColor[2] = color[2];

	glLightfv(GL_LIGHT1, GL_AMBIENT, color);

	for (int i = 0; i < String.size(); i++)
	{
		//GLUT_BITMAP_9_BY_15
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, String[i]);
	}
	glPopAttrib();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}






//display(int)
void
run()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	int filenameIndex = total_point;
	stringstream temp_str;
	temp_str << (filenameIndex);
	std::string str = temp_str.str();
	const char* cstr2 = str.c_str();

	char string[64];
	sprintf(string, cstr2);
	printtext(10, 700, string);





	plane = new Plane(0.0, -6.0, 5.0,  // position
		0.0, 0.0, 0.0, // rotation
		1.0, 0.0, 0.0, 1.0 // color
	);


	plane->draw();

	scene->draw();



	checkNearObjects();

	glutSwapBuffers();
	glFlush();
}


Wall* generateRandomWall(int x, int z)
{
	int randomHeight = 4 * (rand() % (3 - 1 + 1) + 1);
	int randomWidth = (rand() % (4 - 1 + 1) + 1);
	int randomDiameter = (rand() % (3 - 1 + 1) + 1);

	return new Wall(x, -6.0, z,    // position
		0.0, 0.0, 0.0,   // rotation
		2.0, 1.0, 2.0, 2.0, // color
		randomHeight, // height
		randomWidth,  // width
		randomDiameter // diameter
	);
}


void keyboard_n(int key, int x, int y)
{
	std::list<Model*> scene_graph = scene->getGraph();

	if (key == GLUT_KEY_UP)
	{
		if (altitude < 6.0) {
			for (std::list<Model*>::iterator it = scene_graph.begin(); it != scene_graph.end(); it++)
			{
				Model* curmodel = *it;
				curmodel->goUp(2.0);
				curmodel->addPlaneAltitude(2.0);

			}
			altitude += 2.0;
		}
	}
	else if (key == GLUT_KEY_DOWN)
	{
		if (altitude > -6) {
			for (std::list<Model*>::iterator it = scene_graph.begin(); it != scene_graph.end(); it++)
			{
				Model* curmodel = *it;
				curmodel->goDown(2.0);
				curmodel->addPlaneAltitude(-2.0);
			}

			altitude -= 2.0;
		}
	}
	else if (key == GLUT_KEY_RIGHT)
	{
		glRotatef(2.0, 0, 1, 0); // rotate camera, 1 degree around y-axis

		for (std::list<Model*>::iterator it = scene_graph.begin(); it != scene_graph.end(); it++)
		{
			Model* curmodel = *it;
			curmodel->turnLeft(2.0);
		}
		scene->addCurrentAngle(-2.0);



	}
	else if (key == GLUT_KEY_LEFT)
	{
		glRotatef(-2.0, 0, 1, 0); // rotate camera, -1 degree around y-axis



		for (std::list<Model*>::iterator it = scene_graph.begin(); it != scene_graph.end(); it++)
		{
			Model* curmodel = *it;
			//	curmodel->goRight(3.0);
			curmodel->turnRight(2.0);
		}
		scene->addCurrentAngle(2.0);



	}
	else {
		cout << "This key is not supported !!!" << endl;

	}
}





void generateMap()
{

	scene = new Scene();

	scene->addModel(new Terrain(0.0, -10.0, 0.0,        // position
		0.0, 0.0, 0.0,    // rotation
		220.0, 220.0, 220.0, 1.0 // color
	));

	int randomX = 0;
	int spaceBetween = 0;
	int randomHeight = 0;
	int randomWidth = 2;
	int randomDiameter = 2;
	int randomDegree = 0;

	for (int i = 0; i < 13; i++)
	{
		int optimalZ = (i * 20);  // wihout random,  it goes like  20, 40, 60, ...  (for optimal z-axis)

		spaceBetween = 0 - (optimalZ + (10 * (0 - (rand() % (3 - 1 + 1) + 1))));
		randomX = (rand() % (200 - 0 + 1) + 0) - 100;
		scene->addModel(new Bonus(0.0, -6.0, -50.0,  // position
			0.0, 0.0, 0.0, // rotation
			0.137255, 0.419608, 0.556863, 1.0 // color
		));




		for (int j = 0; j < 3; j++)    // at each layer (from 0 to 25) there will be 2 obstacles. so in total there will be 50 obstacles.
		{

			randomDegree = rand() % (180 - 0 + 1) + 0;  // random degree between 0-90
			spaceBetween = 0 - (optimalZ + (10 * (0 - (rand() % (3 - 1 + 1) + 1))));   // with random and taking minus of it,
			randomX = (rand() % (400 - 0 + 1) + 0) - 200;   // it gives a random value between  (-200, +200) (for x - axis)
			randomHeight = 4 * (rand() % (3 - 1 + 1) + 1); // height will be; from 4,8,12 --> 8,16 or 24 pixels. (because its x2 in Wall method.)
		//	randomWidth = (rand() % (4 - 1 + 1) + 1);  // width will be either 2,4,6 or 8 pixels.
		//	randomDiameter = (rand() % (3 - 1 + 1) + 1); // diameter will be either 1,2 or 3

			scene->addModel(new Wall(randomX, -6.0, spaceBetween,    // position
				0.0, 0.0, 0.0,   // rotation  // 	0.0, randomDegree, 0.0,   // rotation
				0.678431, 0.917647, 0.917647, 0.1, // color // 1.0 0.5 0.0
				randomHeight, // height
				randomWidth,  // width
				randomDiameter // diameter
			));
		}
	}
}

void generateTexture() 
{
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	glClearColor(0.196078, 0.6, 0.8, 1.0f);  // background is white


	// Image Path
	std::string iPath = "./res/";
	std::string brickImg = iPath + "brick.bmp";
	std::string rockImg = iPath + "rock.bmp";
	std::string ufoImg = iPath + "Ufo1.bmp";
	std::string bonusImg = iPath + "bonus.bmp";

	
	pix[0].readBMPFile(brickImg); 
	pix[0].setTexture(2001);
	pix[1].readBMPFile(brickImg);
	pix[1].setTexture(2002);
	pix[2].readBMPFile(brickImg);
	pix[2].setTexture(2003);
	pix[3].readBMPFile(brickImg);
	pix[3].setTexture(2004);
	pix[4].readBMPFile(brickImg);
	pix[4].setTexture(2005);
	pix[5].readBMPFile(brickImg);
	pix[5].setTexture(2006);

	pix[6].readBMPFile(rockImg);
	pix[6].setTexture(2007);  // for terrain

	pix[7].readBMPFile(ufoImg);
	pix[7].setTexture(2008); // for plane

	pix[8].readBMPFile(bonusImg);
	pix[8].setTexture(2009); // for bonus


	//glDisable(GL_TEXTURE_2D);
}


void
init(void)
{

	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_TEXTURE_2D);

	generateMap();
//	generateTexture();

	/* Enable a single OpenGL light. */
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_position);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHTING);

	/*--------------------------------------------------*/
	GLfloat qaSpecularLight[] = { 1.0, 1.0, 1.0, 1.0 };   // RGB
	GLfloat qaLightPosition[] = { 5.0, 0, 13.5, 0.5 };    // Position of the light source
	GLfloat qaWhite[] = { 1.0, 1.0, 1.0, 1.0 }; //White Color
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_SPECULAR, qaSpecularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, qaLightPosition);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, qaWhite);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20);
	/*--------------------------------------------------*/

	/* Use depth buffering for hidden surface elimination. */
	glEnable(GL_DEPTH_TEST);

	/* Setup the view of the cube. */
	glMatrixMode(GL_PROJECTION);
	gluPerspective( /* field of view in degree */ 50.0,
		/* aspect ratio */ 1.0,
		/* Z near */ 1.0, /* Z far */ 1000.0);
	glMatrixMode(GL_MODELVIEW);

	gluLookAt(scene->cameraX, 1.0, 25.0, /* eye is at (0,0,30) */
		0.0, 0.0, 5.0,          /* center is at (0,0,0) */
		0.0, 1.0, 0.0);         /* up is in positive Y direction */


}

int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Race the Sun");
	glutDisplayFunc(run);
	glutSpecialFunc(keyboard_n);
	generateTexture();
	glutIdleFunc(run);
	init();
	glutMainLoop();

	return 0;
}
