#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

class Polar {
	public:
		float radius;
		float alpha;
		float beta;

		Polar(float r, float a, float b) : radius(r), alpha(a), beta(b) {}

		float getX() const {
			return radius * cos(beta) * sin(alpha);
		}

		float getY() const {
			return radius * sin(beta);
		}

		float getZ() const {
			return radius * cos(beta) * cos(alpha);
		}
};

Polar camPos(sqrt(75), M_PI_4, M_PI_4);


void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}


void drawCylinder(float radius, float height, int slices) {

	double h = height / 2;
	double step = M_PI * 2 / slices;

	Polar base(h, 0, -M_PI / 2);
	Polar top(h, 0, M_PI / 2);

	double beta = atan(h / radius); // b = tan^(-1)(h/r)
	double radiusC = sqrt(pow(h, 2) + pow(radius, 2)); // hip = sqrt(h^2 + r^2) -- raio a partir do centro do cilindro

	// BASE
	// first vector defined is the center
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(base.getX(), base.getY(), base.getZ());

	for (int i = slices; i >= 0; i--) {
		Polar p(radiusC, step * i, -beta);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(p.getX(), p.getY(), p.getZ());
	}
	glEnd();

	// TOP
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(top.getX(), top.getY(), top.getZ());
	for (int i = 0; i <= slices; i++) {
		Polar p(radiusC, step * i, beta);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(p.getX(), p.getY(), p.getZ());
	}
	glEnd();

	// SIDE
	glBegin(GL_TRIANGLE_STRIP); // Each triangle in the strip shares an edge with the previous triangle, forming a continuous strip of triangles.
	for (int i = 0; i <= slices; i++) {
		Polar pb(radiusC, step * i, beta);
		Polar pt(radiusC, step * i, -beta);
		glColor3f(1.0f, 0.0f, 1.0f);
		glVertex3f(pb.getX(), pb.getY(), pb.getZ());
		glVertex3f(pt.getX(), pt.getY(), pt.getZ());
	}
	glEnd();

}


void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(camPos.getX(), camPos.getY(), camPos.getZ(),
		      0.0,0.0,0.0,
			  0.0f,1.0f,0.0f);

	drawCylinder(1,2,10);

	// End of frame
	glutSwapBuffers();
}


void processKeys(unsigned char c, int xx, int yy) {

	int n = 16;
	switch (c) {
		case 'a':
			camPos.alpha -= M_PI / n;
			break;
		case 'd':
			camPos.alpha += M_PI / n;
			break;
		case 's':
			camPos.beta -= M_PI / n;
			break;
		case 'w':
			camPos.beta += M_PI / n;
			break;
		}

		if (camPos.alpha < 0) 
			camPos.alpha += M_PI * 2;
		else if (camPos.alpha > M_PI * 2) 
			camPos.alpha -= M_PI * 2;
		if (camPos.beta < -M_PI) 
			camPos.beta += M_PI * 2;
		else if (camPos.beta > M_PI) 
			camPos.beta -= M_PI * 2;

		glutPostRedisplay();

}


void processSpecialKeys(int key, int xx, int yy) {

// put code to process special keys in here

}


int main(int argc, char **argv) {

// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(800,800);
	glutCreateWindow("CG@DI-UM");
		
// Required callback registry 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	
// Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);

//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}
