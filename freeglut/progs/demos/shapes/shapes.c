#include <GL/freeglut.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static int slices = 16;
static int stacks = 16;

static void 
reshape(int width, int height)
{
	float ar;
	
	glViewport( 0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ar = (float) width / (float) height;
	glFrustum ( -ar, ar, -1.0, 1.0, 2.0, 100.0);
	
	glMatrixMode(GL_MODELVIEW) ;
	glLoadIdentity() ;
}

static void 
display(void)
{
	float t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	float a = t*90.0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
		glTranslatef(-2.4,1.2,-6);
		glRotatef(60,1,0,0);
		glRotatef(a,0,0,1);
		glColor3f(1,0,0);
		glutSolidSphere(1,slices,stacks);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0,1.2,-6);
		glRotatef(60,1,0,0);
		glRotatef(a,0,0,1);
		glColor3f(1,0,0);
		glutSolidCone(1,1,slices,stacks);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(2.4,1.2,-6);
		glRotatef(60,1,0,0);
		glRotatef(a,0,0,1);
		glColor3f(1,0,0);
		glutSolidTorus(0.2,0.8,slices,stacks);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(-2.4,-1.2,-6);
		glRotatef(60,1,0,0);
		glRotatef(a,0,0,1);
		glColor3f(1,0,0);
		glutWireSphere(1,slices,stacks);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0,-1.2,-6);
		glRotatef(60,1,0,0);
		glRotatef(a,0,0,1);
		glColor3f(1,0,0);
		glutWireCone(1,1,slices,stacks);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(2.4,-1.2,-6);
		glRotatef(60,1,0,0);
		glRotatef(a,0,0,1);
		glColor3f(1,0,0);
		glutWireTorus(0.2,0.8,slices,stacks);
	glPopMatrix();

	glutSwapBuffers();
}


static void 
key(unsigned char key, int x, int y)
{
	switch (key) {
		case 27 : 
		case 'q':
		exit(0);
		break;

 	 case '+':
 	 	slices++;
 	 	stacks++;
 	 	break;
 	 	
 	 case '-':
 	 	slices--;
 	 	stacks--;
 	 	break;
	}
	
	glutPostRedisplay();
}

void 
idle()
{
	glutPostRedisplay();
}

GLfloat light_ambient[]  = { 0.0, 0.0, 0.0, 1.0 };
GLfloat light_diffuse[]  = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_position[] = { 2.0, 5.0, 5.0, 0.0 };

GLfloat mat_ambient[]    = { 0.7, 0.7, 0.7, 1.0 };
GLfloat mat_diffuse[]    = { 0.8, 0.8, 0.8, 1.0 };
GLfloat mat_specular[]   = { 1.0, 1.0, 1.0, 1.0 };
GLfloat high_shininess[] = { 100.0 };

int 
main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitWindowSize(640,480);
	glutInitWindowPosition (40,40);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	glutCreateWindow("FreeGLUT Shapes");

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(key);
	glutIdleFunc(idle);

	glClearColor(1,1,1,1);
	glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

	glutMainLoop();

	return EXIT_SUCCESS;
}
