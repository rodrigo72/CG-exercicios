#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>

float size = 1.0f;
float angle = 0;
float tx = 0;
float ty = 0;
float tz = 0;

void changeSize(int w, int h)
{
	// prevent a divide by zero, when window is too short
	// (you can not make a window with zero width).
	if (h == 0)
		h = 1;
	// compute window's aspect ratio
	float ratio = w * 1.0f / h;

	// set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load the identity matrix
	glLoadIdentity();
	// set the perspective
	gluPerspective(45.0f, ratio, 1.0f, 1000.0f);
	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);

	// et the viewport to be the entire window
	glViewport(0, 0, w, h);
}


void renderScene(void)
{
	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// set camera
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 5.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 1.0f, 0.0f);
		
	// put drawing instructions here
	glColor3f(1.0, 0, 0);
	glTranslated(tx, ty, tz);
	glRotated(angle, 0, 1, 0);
	glutSolidTeapot(size);
	
	// End of frame
	glutSwapBuffers();
}

void reactKey(unsigned char key, int x, int y) {
	switch (key) {
	case '+':
	case '-':
		size += (key == '+') ? 0.1 : -0.1;
		break;
	case 'h':
	case 'l':
		angle += (key == 'h') ? 1 : -1;
		break;
	case 'w':
	case 's':
		tz += (key == 'w') ? 0.1 : -0.1;
		break;
	case 'a':
	case 'd':
		tx += (key == 'a') ? 0.1 : -0.1;
		break;
	case 'k':
	case 'j':
		ty += (key == 'k') ? 0.1 : -0.1;
		break;
	}
}


void printInfo() {

	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));
}


int main(int argc, char** argv)
{
	// put GLUT init here
	glutInit(&argc, argv); // pointer to the unmodified argc from main
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("CG@DI");
    
	// put callback registry here
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	
	glutKeyboardFunc(reactKey);

	// some OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	// enter GLUTs main cycle
	glutMainLoop();
	
	return 1;
}