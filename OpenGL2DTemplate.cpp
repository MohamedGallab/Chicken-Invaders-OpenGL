//  Includes

#include <stdio.h>
#include <math.h>
#include <random>
#include <glut.h>
#include <iostream>

using namespace std;

//  Methods Signatures

void DrawBoss();
void DrawPlayer();
void DrawBullets();
void DrawCircle(int x, int y, float r);
void KeyCB(unsigned char key, int x, int y);
//void drawRect(int x, int y, int w, int h);
//void KeyUp(unsigned char key, int x, int y);
//void Mouse(int button, int state, int x, int y);
void MainLoopCB(int value);
void PlayerShootCB(int value);
void DisplayCB();

//  Global Variables

int window_x_size = 800;
int window_y_size = 800;
int delta_time = 25;

int player_x = 400;
int player_y = 30;
// radius of player model
int player_size = 30;
int player_fire_rate = 500;

int boss_x = 400;
int boss_y = 700;
// radius of boss model
int boss_size = 50;
// true = right;
// false = left;
bool boss_direction = true;

// list of (x,y) pairs for bullets
vector<pair<int, int> > bullets;
int bullet_size = 10;

void main(int argc, char** argr) {
	glutInit(&argc, argr);

	glutInitWindowSize(window_x_size, window_y_size);
	glutInitWindowPosition(450, 50);

	glutCreateWindow("Chicken Invaders");

	// callbacks
	glutDisplayFunc(DisplayCB);
	glutKeyboardFunc(KeyCB);
	glutTimerFunc(delta_time, MainLoopCB, 0);
	//glutTimerFunc(delta_time, PlayerShootCB, 0);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	gluOrtho2D(0, window_x_size, 0, window_y_size);

	glutMainLoop();
}

void DisplayCB() {
	glClear(GL_COLOR_BUFFER_BIT);
	DrawBoss();
	DrawPlayer();
	DrawBullets();
	glFlush();
}

// update player position
void KeyCB(unsigned char key, int x, int y) {
	switch (key) {
	case 'a':
		player_x -= 10;
		if (player_x - player_size <= 0)
			player_x = player_size;
		break;
	case 'd':
		player_x += 10;
		if (player_x + player_size >= window_x_size)
			player_x = window_x_size - player_size;
		break;
	case 'w':
		player_y += 10;
		if (player_y + player_size >= window_y_size)
			player_y = window_y_size - player_size;
		break;
	case 's':
		player_y -= 10;
		if (player_y - player_size <= 0)
			player_y = player_size;
		break;
	}
	glutPostRedisplay();
}

// draws a circle using OpenGL's gluDisk, given (x,y) of its center and its radius
void DrawCircle(int x, int y, float r) {
	glPushMatrix();
	glTranslatef(x, y, 0);
	GLUquadric* quadObj = gluNewQuadric();
	gluDisk(quadObj, 0, r, 50, 50);
	glPopMatrix();
}

// draws the player while adding the movement of the player
void DrawPlayer() {
	glPushMatrix();
	glTranslatef(player_x, player_y, 0);
	glColor3f(1, 1, 1);
	DrawCircle(0, 0, player_size);
	glColor3f(1, 0, 0);
	DrawCircle(10, 10, 5);
	DrawCircle(-10, 10, 5);
	glPopMatrix();
}

// draws the bullets
void DrawBullets() {
	for (auto& bullet : bullets) // access by reference to avoid copying
	{
		glColor3f(0, 0, 1);
		DrawCircle(bullet.first, bullet.second, bullet_size);
	}
}

// draws the boss while adding the movement of the boss
void DrawBoss() {
	glPushMatrix();
	glTranslatef(boss_x, boss_y, 0);
	glColor3f(1, 1, 1);
	DrawCircle(0, 0, boss_size);
	glPopMatrix();
}

// main loop called every delta_time to update state
void MainLoopCB(int value) {
	// update boss location
	if (boss_x + boss_size >= 800)
		boss_direction = false;
	else if (boss_x - boss_size <= 0)
		boss_direction = true;

	if (boss_direction)
		boss_x += 5;
	else
		boss_x -= 5;

	// update bullets' locations
	for (auto bullet = bullets.begin(); bullet < bullets.end(); )
	{
		bullet->second += 50;
		if (bullet->second > window_y_size)
			bullet = bullets.erase(bullet);
		else
			bullet++;
	}

	glutTimerFunc(delta_time, MainLoopCB, 0);
	glutPostRedisplay();
}

// creates bullets depending on player_fire_rate
void PlayerShootCB(int value) {
	// make player shoot
	bullets.push_back(make_pair(player_x, player_y + player_size + bullet_size));

	glutTimerFunc(player_fire_rate, PlayerShootCB, 0);
	glutPostRedisplay();
}