

#include<stdio.h>
#include<stdlib.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>

#include <IL/il.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif


float camX = 00, camY = 30, camZ = 40;
int startX, startY, tracking = 0;

int alpha = 0, beta = 45, r = 50;

unsigned int t;
int image_width, image_height;
unsigned char* data;
GLuint* vertex;
double vertex_count;


void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set the correct perspective
	gluPerspective(45,ratio,1,1000);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

double h(int i, int j) {
	double x = data[i * image_width + j];
	return x / 255 * 30;
}

double hf(double x, double z) {
	double x1 = floor(x);
	double z1 = floor(z);
	double x2 = x1 + 1;
	double z2 = z1 + 1;
	double fx = x - x1;
	double fz = z - z1;
	double h_x1_z = h(x1, z1) * (1 - fz) + h(x1, z2) * fz;
	double h_x2_z = h(x2, z1) * (1 - fz) + h(x2, z2) * fz;
	return h_x1_z * (1 - fx) + h_x2_z * fx;
}


void drawTerrain() {
	for (int strip = 0; strip < image_height - 1; strip++) {
		glBindBuffer(GL_ARRAY_BUFFER, vertex[strip]);
		glVertexPointer(3, GL_DOUBLE, 0, 0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, vertex_count);
	}
}


void renderScene(void) {

	float pos[4] = { -1.0, 1.0, 1.0, 0.0 };

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(camX, camY, camZ,
		0.0, 0.0, 0.0,
		0.0f, 1.0f, 0.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	drawTerrain();

	// End of frame
	glutSwapBuffers();
}


void processMouseButtons(int button, int state, int xx, int yy)
{
	if (state == GLUT_DOWN) {
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
		else
			tracking = 0;
	}
	else if (state == GLUT_UP) {
		if (tracking == 1) {
			alpha += (xx - startX);
			beta += (yy - startY);
		}
		else if (tracking == 2) {

			r -= yy - startY;
			if (r < 3)
				r = 3.0;
		}
		tracking = 0;
	}
}


void processMouseMotion(int xx, int yy)
{
	int deltaX, deltaY;
	int alphaAux, betaAux;
	int rAux;

	if (!tracking)
		return;

	deltaX = xx - startX;
	deltaY = yy - startY;

	if (tracking == 1) {

		alphaAux = alpha + deltaX;
		betaAux = beta + deltaY;

		if (betaAux > 85.0)
			betaAux = 85.0;
		else if (betaAux < -85.0)
			betaAux = -85.0;

		rAux = r;
	}
	else if (tracking == 2) {

		alphaAux = alpha;
		betaAux = beta;
		rAux = r - deltaY;
		if (rAux < 3)
			rAux = 3;
	}
	camX = rAux * sin(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
	camZ = rAux * cos(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
	camY = rAux * sin(betaAux * 3.14 / 180.0);
}


void init() {

	// 	Load the height map "terreno.jpg"
	ilGenImages(1, &t);
	ilBindImage(t);
	ilLoadImage((ILstring)"terreno.jpg");
	ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);
	image_width = ilGetInteger(IL_IMAGE_WIDTH);
	image_height = ilGetInteger(IL_IMAGE_HEIGHT);
	data = ilGetData();

// 	Build the vertex arrays
	vertex = (GLuint*)calloc(image_height - 1, sizeof(GLuint));
	glGenBuffers(image_height - 1, vertex);
	int half_Width = image_width / 2;
	int half_Height = image_height / 2;
	vertex_count = 2 * image_width;

	for (int num_strip = 0; num_strip < image_height - 1; num_strip++) {
		std::vector<double> strip;
		for (int j = 0; j < image_width; j++) {
			strip.push_back(num_strip - half_Height);
			strip.push_back(h(num_strip, j));
			strip.push_back(j - half_Width);

			strip.push_back(num_strip + 1 - half_Height);
			strip.push_back(h(num_strip + 1, j));
			strip.push_back(j - half_Width);
		}
		glBindBuffer(GL_ARRAY_BUFFER, vertex[num_strip]);
		glBufferData(GL_ARRAY_BUFFER, strip.size() * sizeof(double), strip.data(), GL_STATIC_DRAW);
	}

// 	OpenGL settings
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
}


int main(int argc, char** argv) {

	// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(320, 320);
	glutCreateWindow("Gaia");


	// Required callback registry 
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	// Callback registration for keyboard processing
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);

	glewInit();
	glEnableClientState(GL_VERTEX_ARRAY);
	ilInit();
	init();

	// enter GLUT's main cycle
	glutMainLoop();

	return 0;
}
