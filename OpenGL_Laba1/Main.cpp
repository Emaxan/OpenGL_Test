#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glut32.lib")
#include <Gl/glut.h>
#include <wchar.h>
#include <math.h>

#define SCREEN_H 700
#define SCREEN_W 800

HWND hwnd;
int maximized = 0;
//дл€ стебл€
float stem[][2]{ {4.15f, 3.0f}, {3.0f, 0.0f}, { 0.0f, 0.0f } };
//дл€ листьев
float leafX = -0.05f, leafY = -stem[0][1], h1 = 0.0f, h2 = 0.0f, delta = 0.0f;
//дл€ цветка
float scale = 0.0f, translate = -3.0f;
float tx = 0.0f, ty1 = 0.0f, ty2 = 0.0f, th1 = 0.0f, th2 = 0.0f;
//покачивание
float rotateX = 0.0f, angle = 0.0f, maxAngle = 10.0f, maxX = -0.72f, maxY = -0.1f, translateX = 0.0f, translateY = 0.0f, sign = 1.0f;
//машина состо€ний
int state = 0;

void Initialize()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
}

void changeSize(int w, int h) {
	// предотвращение делени€ на ноль
	if (h == 0)
		h = 1;
	float ratio = w * 1.0f / h;
	// используем матрицу проекции
	glMatrixMode(GL_PROJECTION);
	// обнул€ем матрицу
	glLoadIdentity();
	// установить параметры вьюпорта
	glViewport(0, 0, w, h);
	// установить корректную перспективу
	gluPerspective(45.0f, ratio, 0.1f, 100.0f);
	// вернутьс€ к матрице проекции
	glMatrixMode(GL_MODELVIEW);
}

void CurveBezier(int n,float point[][3]) {
	//glMap1f - ‘ункци€ оценки, генерирующа€ координаты.
	//ѕараметры: 
	//  GL_MAP1_VERTEX_3 -	тип генерируемых данных, 
	//  0.0f - нижн€€ граница параметра u (перва€ точка), 
	//  100.0f - верхн€€ граница(последн€€ точка), 
	//  3 - рассто€ние между точками данных, 
	//  n - число точек, 
	// &P[0][0] - массив контрольных точек
	glMap1f(GL_MAP1_VERTEX_3, 0.0f, 100.0f, 3, n, &point[0][0]);
	glEnable(GL_MAP1_VERTEX_3);
	glBegin(GL_LINE_STRIP); //точки соедин€ютс€ ломанной линией
	for (int i = 0; i <= 100; i++)
		//ќценка кривой в точке. ‘ункци€ принимает параметрическое значение, и вычисл€ет точку
		glEvalCoord1f(float(i));
	glEnd();
}

void DrawArc(float x0, float y0, float x1, float y1, float h, float width, int direction)
{
	glLineWidth(width);
	float x, y;
	float midX = (x0 + x1) / 2;
	float midY = (y0 + y1) / 2;
	if (y0 != y1) {
		int absX = midX < 0 ? -1 : 1;
		int absY = midY < 0 ? -1 : 1;

		float q = acosf(midX / sqrtf(absX*midX + absY*midY));
		x = midX - direction*h * 2 * sinf(q);
		y = midY + direction*h * 2 * cosf(q);
	}
	else {
		x = midX;
		y = midY + direction*h;
	}
	float point[3][3]{ { x0, y0, 0.0f }, { x, y, 0.0f }, { x1, y1, 0.0f } };
	CurveBezier(3, point);
}

void line(float x1, float y1, float x2, float y2, float width) {
	glBegin(GL_LINES);
	glVertex3f(x1 - width / 200, y1, 0);
	glVertex3f(x2 - width / 200, y2, 0);
	glVertex3f(x1 + width / 200, y1, 0);
	glVertex3f(x2 + width / 200, y2, 0);
	glEnd();
}

void renderScene(void) {
	//инициализаци€
	{
		// очистка буфера цвета и глубины
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// обнуление трансформации
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);

		// установка камеры
		gluLookAt(0.0f, 0.0f, 10.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f);
	}
	
	int width = 5;
	//покачивание
	glTranslatef(translateX, translateY, 0);
	glRotatef(angle, 0, 0, 1);
	//лист1	
	{
		glColor3f(0.07f, 0.97f, 0.16f);
		DrawArc(-0.05f, -stem[0][1], leafX, leafY, h1, width, -1);
		DrawArc(-0.05f, -stem[0][1], leafX, leafY, h2, width, -1);
	}

	//лист2
	{
		DrawArc(0.05f, -stem[0][1] - delta, -leafX, +leafY - delta, h1, width, 1);
		DrawArc(0.05f, -stem[0][1] - delta, -leafX, +leafY - delta, h2, width, 1);
	}

	//стебель
	{
		glColor3f(0.56f, 0.96f, 0.0f);
		glLineWidth(width);
		line(0, -4.15f, 0, -stem[0][0], 10);
		line(0, -stem[0][1], 0, -stem[1][0], 10);
	}

	//цветок
	{
		glTranslatef(0, translate, 0);
		glScalef(scale, scale, scale);
		glColor3f(1, 0.24f, 0.59f);

		DrawArc(-0.5f, 0.25f - ty1, -tx, 2.25f - ty2, 1 - th1, width, 1);
		DrawArc(-0.5f, 0.25f - ty1, -tx, 2.25f - ty2, 0.5f - th2, width, 1);
		DrawArc(-0.5f, 0.25f - ty1, 0, 2.25f - ty2, 0.5f, width, 1);
		DrawArc(0.5f, 0.25f - ty1, 0, 2.25f - ty2, 0.5f, width, -1);
		DrawArc(0.5f, 0.25f - ty1, tx, 2.25f - ty2, 0.5f - th2, width, -1);
		DrawArc(0.5f, 0.25f - ty1, tx, 2.25f - ty2, 1 - th1, width, -1);
		DrawArc(-0.5f, 0.25f - ty1, 0.5f, 0.25f - ty1, 0.5f - th2, width, -1);
	}

	switch (state) {
		//рост стебл€
		case 0: {
			stem[0][0] -= 0.001f;
			if (stem[0][0] - stem[0][1] < 0.001) {
				state++;
				stem[0][0] = stem[0][1]; 
				delta = 1;
			}
			break;
		}
		//рост стебл€, листьев и цветка
		case 1: {
			scale += 0.0003f;
			translate += 0.001f;
			stem[1][0] -= 0.001f;
			leafX -= 0.0006f;
			leafY += 0.001f;
			h1 += 0.0002f;
			h2 += 0.00007f;
			if (stem[1][0] - stem[1][1] < 0.001) {
				state++;
				stem[1][0] = stem[1][1];
			}
			break;
		}
		//раскрытие бутона
		case 2: {
			tx += 0.001f;
			ty1 += 0.0001f;
			ty2 += 0.0007f;
			th1 += 0.0003f;
			th2 += 0.0002f;
			if (1.5f - tx < 0.001) state++;
			break;
		}
		//покачивание
		case 3: {
			angle += sign*0.01f;
			translateX -= sign*0.00072f;
			translateY -= sign*0.0001f;
			if (maxAngle - angle < 0.001)sign = -1;
			if (angle < 0) sign = 1;
			break;
		}
	}
	if (maximized == 0)
	{
		SendMessage(hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
		maximized = 1;
	}
	glutSwapBuffers();
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(SCREEN_W, SCREEN_H);
	glutCreateWindow("OpenGL");
	hwnd = FindWindow(NULL, "OpenGL");
	Initialize();

	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);

	glutMainLoop();	

	return 0;
}