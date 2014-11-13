#include <iostream>
#include <GLUT/glut.h>

using namespace std;

int Action = 0;

// for blending
GLfloat Alpha = 0.5f;
GLenum Src = GL_SRC_ALPHA;
GLenum Dest = GL_ONE_MINUS_SRC_ALPHA;

// for animation
bool animation_on = false;
const GLfloat size = 0.2;
GLfloat x;
GLfloat dx = 0.02;

void DoTimer(int value)
{
	x += dx;
	if(x + size > 1 || x - size < -1)
	{
		dx *= -1;
	}
	glutPostRedisplay();

	if(animation_on)
	{
		glutTimerFunc(1, DoTimer, 1);
	}
}

void displayText(float x, float y, const char *string)
{
	int j = strlen(string);

	glRasterPos2f(x, y);
	for(int i = 0; i < j; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
}

void keyboardDown(unsigned char key, int x, int y)
{
	printf("key pressed : %d\n", (int) key);

	switch(key)
	{
	case 'q':
	case 227: // 한글 q
		exit(0);
		break;
	}
}

void specialKeyDown(int key, int x, int y)
{
	printf("special key pressed : %d\n", key);

	switch(key)
	{
	case GLUT_KEY_UP:
		Alpha += 0.1;
		break;
	case GLUT_KEY_DOWN:
		Alpha -= 0.1;
		break;
	}

	glutPostRedisplay();
}

void DoMenu(int value)
{
	Action = value;

//	glClearColor(0.8, 0.8, 0.8, 0.8);
//	glColor3f(1.0, 1.0, 1.0);

	animation_on = false;

	if(value == 9)
	{
		animation_on = true;
		glutTimerFunc(30, DoTimer, 1);
	}

	glutPostRedisplay();
}

void DoDisplay()
{
	if(Action == 0)
	{
		// 색상버퍼를 지워 화면을 Clear 한다.
		glClear(GL_COLOR_BUFFER_BIT);
		glutSwapBuffers();
	}
	else if(Action == 1)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glColor3f(1.0, 0.0, 0.0); // 정점의 색 설정
		glBegin(GL_TRIANGLES);
		glVertex2f(0.0, 0.5);
		glVertex2f(-0.5, -0.5);
		glVertex2f(0.5, -0.5);
		glEnd();
		glutSwapBuffers();
	}

	else if(Action == 2)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glBegin(GL_POLYGON);
		//glColor3f(1.0, 1.0, 1.0);
		glVertex2f(0.0, 0.6);
		glColor3f(1.0, 0.0, 0.0);
		glVertex2f(-0.6, 0.0);
		//glColor3f(1.0, 0.0, 0.0);
		glVertex2f(-0.4, -0.6);
		glColor3f(0.0, 1.0, 0.0);
		glVertex2f(0.4, -0.6);
		glColor3f(0.0, 0.0, 1.0);
		glVertex2f(0.6, 0.0);
		glEnd();
		glutSwapBuffers();
	}
	else if(Action == 3)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glPointSize(100.0);
		glBegin(GL_POINTS);
		glVertex2f(0.0, 0.5);
		glVertex2f(-0.5, -0.5);
		glVertex2f(0.5, -0.5);
		glEnd();
		glutSwapBuffers();
	}
	else if(Action == 4)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		GLfloat y;
		GLfloat w = 1;
		for(y = 0.8; y > -0.8; y -= 0.2)
		{
			glLineWidth(w++);
			glBegin(GL_LINES);
			glVertex2f(-0.8, y);
			glVertex2f(0.8, y);
			glEnd();
		}
		glutSwapBuffers();
	}
	else if(Action == 5)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glColor3f(0.73, 0.89, 0.40); // 정점의 색 설정
		glRectf(-0.8, 0.8, 0.8, -0.8);
		glutSwapBuffers();
	}
	else if(Action == 6)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glBegin(GL_QUADS);
		glVertex2f(0.0, 0.5);
		glVertex2f(-0.5, 0.0);
		glVertex2f(0.0, -0.5);
		glVertex2f(0.5, 0.0);
		glEnd();
		glutSwapBuffers();
	}
	else if(Action == 7)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glBegin(GL_POLYGON);
		glVertex2f(0.0, 0.5);
		glVertex2f(-0.5, 0.0);
		glVertex2f(0.5, 0.0);
		glVertex2f(0.0, -0.5);
		glEnd();
		glutSwapBuffers();
	}
	else if(Action == 8)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glBlendFunc(Src, Dest);

		glColor3f(0, 0, 1);
		glRectf(-0.5, 0.8, 0.5, 0.0);

		glColor4f(1, 0, 0, Alpha);
		glBegin(GL_TRIANGLES);
		glVertex2f(0.0, 0.5);
		glVertex2f(-0.5, -0.5);
		glVertex2f(0.5, -0.5);
		glEnd();
		glutSwapBuffers();
	}
	else if(Action == 9)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_POLYGON);
		glVertex2f(x, size);
		glVertex2f(x - size, -size);
		glVertex2f(x + size, -size);
		glEnd();
		glutSwapBuffers();
	}
	else if(Action == 10)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		displayText(0, 0, "text print");

		glutSwapBuffers();
	}
}

void setMenu()
{
	glutCreateMenu(DoMenu);
	glutAddMenuEntry("Clear", 0);
	glutAddMenuEntry("빨간색 삼각형", 1);
	glutAddMenuEntry("오각형 그리기", 2);
	glutAddMenuEntry("점 찍기", 3);
	glutAddMenuEntry("선 그리기", 4);
	glutAddMenuEntry("사각형 그리기", 5);
	glutAddMenuEntry("마름모 그리기", 6);
	glutAddMenuEntry("다각형 그리기", 7);
	glutAddMenuEntry("블랜딩", 8);
	glutAddMenuEntry("애니메이션", 9);
	glutAddMenuEntry("텍스트 그리기", 10);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
void init(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(600, 600);
	glutCreateWindow("OpenGL");

	glutKeyboardFunc(keyboardDown);
	glutSpecialFunc(specialKeyDown);
	glutDisplayFunc(DoDisplay);

	glEnable(GL_BLEND);
	glEnable(GL_POINT_SMOOTH); // 점을 둥글게 그리기 위해 안티엘리어싱 on
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);

	glClearColor(0.8, 0.8, 0.8, 0.8);

	setMenu();
}

void printGLInfo()
{
	GLfloat range[2], granu;
	glGetFloatv(GL_POINT_SIZE_RANGE, range);
	glGetFloatv(GL_POINT_SIZE_GRANULARITY, &granu);

	printf("GL_POINT_SIZE_RANGE       : %f ~ %f\n", range[0], range[1]);
	printf("GL_POINT_SIZE_GRANULARITY : %f\n\n", granu);

	glGetFloatv(GL_LINE_WIDTH_RANGE, range);
	glGetFloatv(GL_LINE_WIDTH_GRANULARITY, &granu);

	printf("GL_LINE_WIDTH_RANGE       : %f ~ %f\n", range[0], range[1]);
	printf("GL_LINE_WIDTH_GRANULARITY : %f\n\n", granu);
}

int main(int argc, char** argv)
{
	init(argc, argv);

	printGLInfo();

	glutMainLoop();
	return 0;
}

