#include <GL/glut.h>

void display() {
	glClearColor(0.2f, 0.4f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glutSwapBuffers();
}

int main(int argc, char** argv) {

	glutInit(&argc, arv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Hello, OpenGL!");

	glutInitDisplayFunc(display);
	glutMainLoop();

	return 0;
}