#include <GL/glut.h>
#include <stdio.h>
#include <time.h>

#define MAX_PARTICLES 100

typedef struct {
	float x, y; // particle position
	float vx, vy; // particle velocity 
	float life; // particle life
} Particle;


Particle particles[MAX_PARTICLES];
int numParticles = MAX_PARTICLES;

// Initial values for the particles
void initializeParticles() {
	srand(time(NULL));

	for (int i = 0; i < numParticles; i++) {
		particles[i].x = 0.0f;
		particles[i].y = 0.0f;
		particles[i].vx = (float)((rand() % 200 - 100) / 100); // -1.0 - 0.99 velocity counter
		particles[i].vy = (float)((rand() % 200 - 100) / 100); // same for the y velocity
		particles[i].life = (float)((rand % 100 - 100) / 100);

	}

}

// reset and incremeant the properties
void updateParticles() {
	for (int i = 0; i < numParticles; i++) {
		particles[i].x += particles[i].vx * 0.01f;
		particles[i].y = particles[i].vy * 0.01f;
		particles[i].life -= 0.005f;

		// if the particle is dead... 
		if (particles[i].life <= 0.0f) {
			// reset the particle
			particles[i].x = 0.0f;
			particles[i].y = 0.0f;
			particles[i].vx = (float)((rand() % 200 - 100) / 100);
			particles[i].vy = (float)((rand() % 200 - 100) / 100);
			particles[i].life = 1.0f;
		}
	}

}

// display the generated particles
void display() {
	glClearColor(0.2f, 0.4f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	updateParticles();

	glBegin(GL_POINTS);
	for (int i = 0; i < numParticles; i++) {
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex2f(particles[i].x, particles[i].y);
	}
	glEnd();

	glutSwapBuffers();
}

// main loop function
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Particle System");

	glutInitDisplayFunc(display);

	initializeParticles();

	glutMainLoop();
	return 0;
}
