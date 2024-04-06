#include <stdio.h>
#include <vector>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

float alfa = 0.0f, beta = 0.0f, radius = 5.0f;
float camX, camY, camZ;

std::vector<float>  vertexB, pontos;
GLuint buffers[2], n_vertices, n_vertices_2;

double timebase;
double frames;

void spherical2Cartesian() {
    camX = radius * cos(beta) * sin(alfa);
    camY = radius * sin(beta);
    camZ = radius * cos(beta) * cos(alfa);
}


void changeSize(int w, int h) {
    // Prevent a divide by zero, when window is too short
    if (h == 0)
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
    gluPerspective(45.0f, ratio, 1.0f, 1000.0f);
    // return to the model view matrix mode
    glMatrixMode(GL_MODELVIEW);
}


void framerate() {
    frames++;
    int time = glutGet(GLUT_ELAPSED_TIME);
    if (time - timebase > 1000) {
        printf("FPS:%4.2f\n",
            frames * 1000.0 / (time - timebase));
        timebase = time;
        frames = 0;
    }
}

void cylinderVBO(float radius, float height, int sides) {
    int pos;

    float step = (2 * M_PI) / sides;

    // top
    for (int i = 0; i < sides; i++) {
        vertexB.push_back(0);
        vertexB.push_back(height * 0.5);
        vertexB.push_back(0);
        vertexB.push_back(cos(i * step) * radius);
        vertexB.push_back(height * 0.5);
        vertexB.push_back(-sin(i * step) * radius);
        vertexB.push_back(cos((i + 1) * step) * radius);
        vertexB.push_back(height * 0.5);
        vertexB.push_back(-sin((i + 1) * step) * radius);
    }

    // bottom
    for (int i = 0; i < sides; i++) {
        vertexB.push_back(0);
        vertexB.push_back(-height * 0.5);
        vertexB.push_back(0);
        vertexB.push_back(cos((i + 1) * step) * radius);
        vertexB.push_back(-height * 0.5);
        vertexB.push_back(-sin((i + 1) * step) * radius);
        vertexB.push_back(cos(i * step) * radius);
        vertexB.push_back(-height * 0.5);
        vertexB.push_back(-sin(i * step) * radius);
    }

    // body
    for (int i = 0; i <= sides; i++) {

        vertexB.push_back(cos(i * step) * radius);
        vertexB.push_back(height * 0.5);
        vertexB.push_back(-sin(i * step) * radius);
        vertexB.push_back(cos(i * step) * radius);
        vertexB.push_back(-height * 0.5);
        vertexB.push_back(-sin(i * step) * radius);
        vertexB.push_back(cos((i + 1) * step) * radius);
        vertexB.push_back(height * 0.5);
        vertexB.push_back(-sin((i + 1) * step) * radius);

        vertexB.push_back(cos(i * step) * radius);
        vertexB.push_back(-height * 0.5);
        vertexB.push_back(-sin(i * step) * radius);
        vertexB.push_back(cos((i + 1) * step) * radius);
        vertexB.push_back(-height * 0.5);
        vertexB.push_back(-sin((i + 1) * step) * radius);
        vertexB.push_back(cos((i + 1) * step) * radius);
        vertexB.push_back(height * 0.5);
        vertexB.push_back(-sin((i + 1) * step) * radius);
    }

    n_vertices = vertexB.size();

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexB.data()) * n_vertices, &vertexB[0], GL_STATIC_DRAW);
}

struct Ponto {
    float x;
    float y;
    float z;
};

void generateSphere(int r, int sector_count, int stack_count) {

    float sector_step = (2 * M_PI) / sector_count;
    float theta, theta_next; // horizontal

    float stack_step = M_PI / stack_count;
    float phi, phi_next; // vertical

    float xy, y, xy_next, y_next;

    for (int i = 0; i < stack_count; i++) {

        phi = (M_PI / 2) - (i * stack_step); // pi/2 até -pi/2

        /* os pontos numa linha horizontal de um setor têm todos a mesma:
            - altura;
            - distancia da origem até à projeção do ponto no plano XY (componete de cálculo) */
        xy = r * cos(phi);
        y = r * sin(phi);

        // proxima linha
        phi_next = (M_PI / 2) - ((i + 1) * stack_step);
        xy_next = r * cos(phi_next);
        y_next = r * sin(phi_next);

        /* x, e z irão variar de acordo com o theta
        (x e z serão o valor de x e z do ponto projetado
        mas neste nesta parte do código ainda não se sabe qual é esse ponto, pois não se conhece o theta;
        será introduzido no loop seguinte) */

        for (int j = 0; j < sector_count; j++) {

            // 0 até 2pi
            theta = j * sector_step;
            theta_next = (j + 1) * sector_step;

            //  p1--p4
            //  |    |
            //  p2--p3

            Ponto p1 = { xy * sin(theta),           y,      xy * cos(theta) }; // ((r * cos(phi)) * cos(theta), r * sin(phi), (r * cos(phi)) * sin(theta))
            Ponto p2 = { xy_next * sin(theta),      y_next, xy_next * cos(theta) }; // phi move
            Ponto p3 = { xy_next * sin(theta_next), y_next, xy_next * cos(theta_next) }; // phi e theta movem
            Ponto p4 = { xy * sin(theta_next),      y,      xy * cos(theta_next) }; // theta move

            // criar uma "fila" de triangulos com a forma de um paralelogramo:

            // primeiro triangulo
            if (i != 0) {
                pontos.push_back(p1.x); pontos.push_back(p1.y); pontos.push_back(p1.z);
                pontos.push_back(p2.x); pontos.push_back(p2.y); pontos.push_back(p2.z);
                pontos.push_back(p4.x); pontos.push_back(p4.y); pontos.push_back(p4.z);
            }

            // segundo triangulo
            if (i != stack_count - 1) {
                pontos.push_back(p2.x); pontos.push_back(p2.y); pontos.push_back(p2.z);
                pontos.push_back(p3.x); pontos.push_back(p3.y); pontos.push_back(p3.z);
                pontos.push_back(p4.x); pontos.push_back(p4.y); pontos.push_back(p4.z);
            }

        }
    }

    n_vertices_2 = pontos.size();

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pontos.data()) * n_vertices_2, &pontos[0], GL_STATIC_DRAW);
    
}

void renderScene(void) {

    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set the camera
    glLoadIdentity();
    gluLookAt(camX, camY, camZ,
        0.0, 0.0, 0.0,
        0.0f, 1.0f, 0.0f);

    //glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    //glVertexPointer(3, GL_FLOAT, 0, 0);
    //glDrawArrays(GL_TRIANGLES, 0, vertexB.size());

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, pontos.size());

    // End of frame
    glutSwapBuffers();
}


void processKeys(unsigned char c, int xx, int yy) {

    // put code to process regular keys in here

}


void processSpecialKeys(int key, int xx, int yy) {

    switch (key) {

    case GLUT_KEY_RIGHT:
        alfa -= 0.1; break;

    case GLUT_KEY_LEFT:
        alfa += 0.1; break;

    case GLUT_KEY_UP:
        beta += 0.1f;
        if (beta > 1.5f)
            beta = 1.5f;
        break;

    case GLUT_KEY_DOWN:
        beta -= 0.1f;
        if (beta < -1.5f)
            beta = -1.5f;
        break;

    case GLUT_KEY_PAGE_DOWN: radius -= 0.1f;
        if (radius < 0.1f)
            radius = 0.1f;
        break;

    case GLUT_KEY_PAGE_UP: radius += 0.1f; break;
    }
    spherical2Cartesian();
    glutPostRedisplay();

}


void printInfo() {

    printf("Vendor: %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version: %s\n", glGetString(GL_VERSION));

    printf("\nUse Arrows to move the camera up/down and left/right\n");
    printf("Home and End control the distance from the camera to the origin");
}


int main(int argc, char** argv) {

    // init GLUT and the window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 800);
    glutCreateWindow("CG@DI-UM");

    // Required callback registry 
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);

    // Callback registration for keyboard processing
    glutKeyboardFunc(processKeys);
    glutSpecialFunc(processSpecialKeys);

#ifndef __APPLE__
    glewInit();
#endif

    //  OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_VERTEX_ARRAY);
    glPolygonMode(GL_FRONT, GL_LINE);

    spherical2Cartesian();

    printInfo();

    glGenBuffers(1, buffers);
    generateSphere(1, 10, 10);
    //cylinderVBO(1, 2, 262144);


    timebase = glutGet(GLUT_ELAPSED_TIME);
    glutIdleFunc(framerate);
    // enter GLUT's main cycle
    glutMainLoop();

    return 1;
}
