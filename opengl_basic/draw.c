#include <stdio.h>
#include <stdlib.h>
#include <GLUT/glut.h>

// 최대 데이터 개수
#define MAXPOINTS         20000

// 패턴
#define PATTERN_POINT     101
#define PATTERN_LINE      102

// 컬러
#define COLOR_RED         201
#define COLOR_WHITE       202
#define COLOR_BLUE        203

// 두께
#define THICKNESS_1       301
#define THICKNESS_2       302
#define THICKNESS_3       303

// 마지막 선을 기록하기 위한 마크
#define MARK_LINE_END     999.9f

// 윈도우 크기
int winw = 800;
int winh = 600;

// Point 데이터 기록을 위한 변수
int count_points = 0;
int color_points[MAXPOINTS];
float x_points[MAXPOINTS];
float y_points[MAXPOINTS];
float thickness_points[MAXPOINTS];

// Line 데이터 기록을 위한 변수
int count_lines = 0;
int color_lines[MAXPOINTS];
float x_lines[MAXPOINTS];
float y_lines[MAXPOINTS];
float thickness_lines[MAXPOINTS];

// 현재 드로잉 모드 저장
int current_draw_pattern = PATTERN_POINT;
int current_draw_color = COLOR_WHITE;
float current_thickness = 1.0f;

// 정수 좌표 값을 -1 ~ 1 값으로 정규화
float x_to_float(int x)
{
	return (2*x-winw)/(float)winw;
}

float y_to_float(int y)
{
	return (winh-2*y)/(float)winh;
}

// 메뉴 선택 시 처리
void menu(int op)
{
	switch(op)
	{
		case 'Q':
		case 'q':
		case  27:   // ESC
			exit(0);

		case PATTERN_POINT:
			current_draw_pattern = PATTERN_POINT;
			break;

		case PATTERN_LINE:
			current_draw_pattern = PATTERN_LINE;
			break;

		case THICKNESS_1:
			current_thickness = 1.0f;
			break;

		case THICKNESS_2:
			current_thickness = 3.0f;
			break;

		case THICKNESS_3:
			current_thickness = 6.0f;
			break;

		case COLOR_RED:
			current_draw_color = COLOR_RED;
			break;

		case COLOR_WHITE:
			current_draw_color = COLOR_WHITE;
			break;

		case COLOR_BLUE:
			current_draw_color = COLOR_BLUE;
			break;
	}
}

void keyboardDown(unsigned char key, int x, int y)
{
	menu(key);
}

/* reshaped window */
void reshape(int w, int h)
{
	winw = w;
	winh = h;
	glViewport(0,0,w,h);
}

// 입력 받은 좌표에 현재 패턴을 그린다.
void save_point_and_draw(int x, int y)
{
	// 저장될 변수 포인터 선언
	int *count;
	int *pColor;
	float *pBuff_x;
	float *pBuff_y;
	float *pThickness;

	// 패턴에 맞는 변수 포인팅
	switch(current_draw_pattern)
	{
		case PATTERN_POINT:
			count = &count_points;
			pColor = color_points;
			pBuff_x = x_points;
			pBuff_y = y_points;
			pThickness = thickness_points;
			break;

		case PATTERN_LINE:
			count = &count_lines;
			pColor = color_lines;
			pBuff_x = x_lines;
			pBuff_y = y_lines;
			pThickness = thickness_lines;
			break;

		default:
			return;
	}

	// 데이터 저장
	pBuff_x[*count] = x_to_float(x);
	pBuff_y[*count] = y_to_float(y);
	pColor[*count] = current_draw_color;
	pThickness[*count] = current_thickness;
	(*count)++;

	// 화면을 다시 그린다.
	glutPostRedisplay();
}

// 마우스 클릭 이벤트 처리
void mouseClick(int button, int state, int x, int y)
{
	if(state == GLUT_DOWN) // pressed
	{
		switch(button)
		{
			case GLUT_LEFT_BUTTON:
				printf("left mbutton pressedd pos = (%d,%d)\n",x,y);

				// 마우스가 pressed 되면 현재 좌표에 패턴을 그린다.
				save_point_and_draw(x, y);
				break;
		}
	}
	else // released
	{
		switch(button)
		{
			case GLUT_LEFT_BUTTON:
				printf("left mbutton released pos = (%d,%d)\n",x,y);
				if(current_draw_pattern == PATTERN_LINE)
				{
					// 라인 패턴일 때 마우스가 release 되면 선 끊어짐 처리를 위해
					// MARK_LINE_END 마크를 저장한다.
					x_lines[count_lines++] = MARK_LINE_END;
				}
				break;
		}
	}
}


// 마우스 모션 이벤트 처리
void mouseMotion(int x, int y)
{
	printf("mouse motion pos = (%d,%d)\n",x,y);
	save_point_and_draw(x, y);
}

// 드로잉 컬러 설정
void set_color(int color)
{
	if(color == COLOR_RED)
	{
		glColor3f(1.0f, 0.0f, 0.0f);
	}
	else if(color == COLOR_WHITE)
	{
		glColor3f(1.0f, 1.0f, 1.0f);
	}
	else if(color == COLOR_BLUE)
	{
		glColor3f(0.0f, 0.0f, 1.0f);
	}
}

// Point 데이터 드로잉
void draw_points()
{
	int drawed_point = 0;
	while(drawed_point < count_points)
	{
		// Point 두께 설정
		float thickness = thickness_points[drawed_point];
		glPointSize(thickness);

glBegin(GL_POINTS);
		for(; drawed_point < count_points; drawed_point++)
		{
			if(thickness != thickness_points[drawed_point])
			{
				// 달라진 두께를 적용하기 위해 break
				break;
			}

			// 컬러 설정
			set_color(color_points[drawed_point]);

			// Point 드로잉
			glVertex2f(x_points[drawed_point], y_points[drawed_point]);
		}
glEnd();
	}
}

// Line 데이터 드로잉
void draw_lines()
{
	int drawed_point = 0;
	while(drawed_point < count_lines)
	{
		// 라인 두께 설정
		glLineWidth(thickness_lines[drawed_point]);

glBegin(GL_LINE_STRIP);
		for(; drawed_point < count_lines; drawed_point++)
		{
			if(x_lines[drawed_point] == MARK_LINE_END)
			{
				// MARK_LINE_END 일 경우 선 끊어짐 처리를 위해 break
				// glBegin(GL_LINE_STRIP) ~ glEnd() 에서 그려진
				// 선은 모두 연결되어 그려진다.
				// 선 끊어짐 처리를 위해선 glBegin(GL_LINE_STRIP) ~ glEnd()
				// 을 다시 시작해야 한다.
				drawed_point++;
				break;
			}

			// 컬러 설정
			set_color(color_lines[drawed_point]);

			// 라인 드로잉
			glVertex2f(x_lines[drawed_point], y_lines[drawed_point]);
		}
glEnd();

	}
}

// 드로잉 이벤트 처리
void draw()
{
	// clear the background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	draw_points();
	draw_lines();

	glFlush();
	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(winw, winh);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Perspective's GLUT Template");

	// register glut call backs
	glutDisplayFunc(draw);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMotion);
	glutKeyboardFunc(keyboardDown);

	// create a sub menus
	int submenu_color = glutCreateMenu(menu);
	glutAddMenuEntry("red",   COLOR_RED);
	glutAddMenuEntry("white", COLOR_WHITE);
	glutAddMenuEntry("blue",  COLOR_BLUE);

	int submenu_thickness = glutCreateMenu(menu);
	glutAddMenuEntry("1", THICKNESS_1);
	glutAddMenuEntry("2", THICKNESS_2);
	glutAddMenuEntry("3", THICKNESS_3);

	int submenu_drawpattern = glutCreateMenu(menu);
	glutAddMenuEntry("points", PATTERN_POINT);
	glutAddMenuEntry("lines",  PATTERN_LINE);
	glutAddSubMenu("thickness", submenu_thickness);
	glutAddSubMenu("color", submenu_color);

	int submenu_quit = glutCreateMenu(menu);
	glutAddMenuEntry("yes", 'q');
	glutAddMenuEntry("no", 0);

	// create main "right click" menu
	glutCreateMenu(menu);
	glutAddSubMenu("drawpattern", submenu_drawpattern);
	glutAddSubMenu("Quit", submenu_quit);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();
	return 0;
}


