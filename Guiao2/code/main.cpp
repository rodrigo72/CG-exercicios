#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>

float pyramidAngle = 0.0f; // Initial angle for pyramid rotation
float pyramidVelocity = 0.0f; // Velocity of pyramid rotation

float cameraX = 0.0f; // Camera position along X-axis
float cameraY = 5.0f; // Camera position along Y-axis
float cameraZ = 5.0f; // Camera position along Z-axis

float cameraVelocityX = 0.0f; // Velocity of camera movement along X-axis
float cameraVelocityY = 0.0f; // Velocity of camera movement along Y-axis
float cameraVelocityZ = 0.0f; // Velocity of camera movement along Z-axis

const float maxVelocity = 0.1f; // Maximum velocity for smooth movement
const float acceleration = 0.01f; // Acceleration factor
const float damping = 0.98f; // Damping factor for inertia

int lastMouseX = 0; // Last mouse X position
int lastMouseY = 0; // Last mouse Y position

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


void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(cameraX, cameraY, cameraZ,
				0.0, 0.0, 0.0,
				0.0f, 1.0f, 0.0f);

// put axis drawing in here

	glBegin(GL_LINES);
	// X axis in red
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-100.0f, 0.0f, 0.0f);
		glVertex3f(100.0f, 0.0f, 0.0f);
		// Y Axis in Green
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, -100.0f, 0.0f);
		glVertex3f(0.0f, 100.0f, 0.0f);
		// Z Axis in Blue
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, -100.0f);
		glVertex3f(0.0f, 0.0f, 100.0f);
	glEnd();

// put the geometric transformations here

	glPushMatrix();
	glRotatef(pyramidAngle, 0.0f, 1.0f, 0.0f);

// put pyramid drawing instructions here

	glBegin(GL_TRIANGLES);

		// Base
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-1.0f, 0.0f, 1.0f);
		glVertex3f(1.0f, 0.0f, -1.0f);
		glVertex3f(1.0f, 0.0f, 1.0f);

		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-1.0f, 0.0f, 1.0f);
		glVertex3f(-1.0f, 0.0f, -1.0f);
		glVertex3f(1.0f, 0.0f, -1.0f);

		// Front
		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex3f(1.0f, 0.0f, -1.0f);
		glVertex3f(0.0f, 2.0f, 0.0f);
		glVertex3f(1.0f, 0.0f, 1.0f);

		glColor3f(1.0f, 0.0f, 1.0f);
		glVertex3f(1.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 2.0f, 0.0f);
		glVertex3f(-1.0f, 0.0f, 1.0f);

		// Back
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-1.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 2.0f, 0.0f);
		glVertex3f(-1.0f, 0.0f, -1.0f);

		glColor3f(1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, 0.0f, -1.0f);
		glVertex3f(0.0f, 2.0f, 0.0f);
		glVertex3f(1.0f, 0.0f, -1.0f);
	glEnd();

	glPopMatrix();

	pyramidAngle += 0.5f;

	// End of frame
	glutSwapBuffers();
}

void update(int value) {
	// Update pyramid rotation based on velocity
	pyramidAngle += pyramidVelocity;

	// Apply damping to pyramid velocity
	pyramidVelocity *= damping;

	// Update camera position based on velocity
	cameraX += cameraVelocityX;
	cameraY += cameraVelocityY;
	cameraZ += cameraVelocityZ;

	// Apply damping to camera velocity
	cameraVelocityX *= damping;
	cameraVelocityY *= damping;
	cameraVelocityZ *= damping;

	// Request to call update() again in 16 milliseconds
	glutTimerFunc(16, update, 0);

	// Redisplay the scene
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		lastMouseX = x;
		lastMouseY = y;
	}
}

void motion(int x, int y) {
	int deltaX = x - lastMouseX;
	int deltaY = y - lastMouseY;

	// Update camera position based on mouse movement
	cameraX += deltaX * 0.05f;
	cameraY -= deltaY * 0.05f; // Invert Y-axis to match OpenGL coordinate system

	lastMouseX = x;
	lastMouseY = y;

	// Redisplay the scene
	glutPostRedisplay();
}


// write function to process keyboard events

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'a': // Move camera left
		cameraVelocityX -= acceleration;
		break;
	case 'd': // Move camera right
		cameraVelocityX += acceleration;
		break;
	case 'w': // Move camera up
		cameraVelocityY += acceleration;
		break;
	case 's': // Move camera down
		cameraVelocityY -= acceleration;
		break;
	case 'q': // Rotate pyramid counterclockwise
		pyramidVelocity -= acceleration;
		break;
	case 'e': // Rotate pyramid clockwise
		pyramidVelocity += acceleration;
		break;
	}
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
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse); // Register mouse button callback
	glutMotionFunc(motion); // Register mouse motion callback

	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);

	
// put here the registration of the keyboard callbacks



//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glutTimerFunc(0, update, 0);
	
// enter GLUT's main cycle
	glutMainLoop();
	return 1;
}
