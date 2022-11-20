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
void DrawPowerUp();
void DrawBullets();
void DrawBossHealthBar();
void DrawPlayerHealthBar();
void Print(int x, int y, char* string);
void DrawCircle(int x, int y, float r);
void DrawRectangle(int x, int y, int w, int h);
void KeyCB(unsigned char key, int x, int y);
bool IsCollision(int x1, int y1, int size1, int x2, int y2, int size2);
void GameLoopCB(int value);
void PlayerShootCB(int value);
void BossShootCB(int value);
void PowerUpDecayCB(int value);
void DisplayCB();

//  Global Variables

int window_x_size = 800;
int window_y_size = 800;
int delta_time = 25;
bool game_over = false;

int player_x = 400;
int player_y = 80;
// radius of player model
int player_size = 30;
int player_fire_rate = 500;
int player_bullet_speed = 50;
int player_speed = 10;
float player_hp = 3;
float player_max_hp = 3;
bool player_invulnerability = false;

int boss_x = 400;
int boss_y = 700;
// radius of boss model
int boss_size = 50;
// true = right;
// false = left;
bool boss_direction = true;
int boss_fire_rate = 750;
int boss_bullet_speed = 10;
int boss_speed = 5;
float boss_hp = 25;
float boss_max_hp = 25;

// list of (x,y) pairs for bullets
vector<pair<int, int> > player_bullets;
vector<pair<int, int> > boss_bullets;
int bullet_size = 5;

int power_up_size = 20;
int power_up_x = rand() % (window_x_size - power_up_size) + power_up_size;
int power_up_y = rand() % (window_y_size - power_up_size) + power_up_size;
// in milliseconds
int power_up_duration = 5000;

void main(int argc, char** argr) {
	glutInit(&argc, argr);

	glutInitWindowSize(window_x_size, window_y_size);
	glutInitWindowPosition(450, 50);

	glutCreateWindow("Chicken Invaders");

	// callbacks
	glutDisplayFunc(DisplayCB);
	glutKeyboardFunc(KeyCB);
	glutTimerFunc(delta_time, GameLoopCB, 0);
	glutTimerFunc(delta_time, PlayerShootCB, 0);
	glutTimerFunc(delta_time, BossShootCB, 0);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	gluOrtho2D(0, window_x_size, 0, window_y_size);

	glutMainLoop();
}

void DisplayCB() {
	glClear(GL_COLOR_BUFFER_BIT);
	if (game_over)
	{
		glColor3f(1, 1, 1);
		char* endgame_text[20];
		if (player_hp > 0)
			sprintf((char*)endgame_text, "You Won !");
		else
			sprintf((char*)endgame_text, "You Lost :(");
		Print(350, 400, (char*)endgame_text);
		glFlush();
		return;
	}
	DrawBoss();
	DrawPlayer();
	DrawBullets();
	DrawBossHealthBar();
	DrawPlayerHealthBar();
	if (!player_invulnerability)
		DrawPowerUp();
	glFlush();
}

// update player position
void KeyCB(unsigned char key, int x, int y) {
	switch (key) {
	case 'a':
		player_x -= player_speed;
		if (player_x - player_size <= 0)
			player_x = player_size;
		break;
	case 'd':
		player_x += player_speed;
		if (player_x + player_size >= window_x_size)
			player_x = window_x_size - player_size;
		break;
	case 'w':
		player_y += player_speed;
		if (player_y + player_size >= window_y_size)
			player_y = window_y_size - player_size;
		break;
	case 's':
		player_y -= player_speed;
		if (player_y - player_size <= 0)
			player_y = player_size;
		break;
	}
}

// draws a circle using OpenGL's gluDisk, given (x,y) of its center and its radius
void DrawCircle(int x, int y, float r) {
	glPushMatrix();
	glTranslatef(x, y, 0);
	GLUquadric* quadObj = gluNewQuadric();
	gluDisk(quadObj, 0, r, 50, 50);
	glPopMatrix();
}

// draws rectangles using the (x,y) of the bottom left vertex, width (w) and height (h)
void DrawRectangle(int x, int y, int w, int h) {
	glBegin(GL_POLYGON);
	glVertex2f(x, y);
	glVertex2f(x + w, y);
	glVertex2f(x + w, y + h);
	glVertex2f(x, y + h);
	glEnd();
}

// draws the player while adding the movement of the player
void DrawPlayer() {
	glPushMatrix();
	glTranslatef(player_x, player_y, 0);
	glColor3f(1, 1, 1);
	if (player_invulnerability)
		glColor3f(1, 0, 1);
	DrawCircle(0, 0, player_size);
	glColor3f(1, 0, 0);
	DrawCircle(10, 10, 5);
	DrawCircle(-10, 10, 5);
	glPopMatrix();
}

// draws player & boss bullets
void DrawBullets() {
	for (auto& bullet : player_bullets) // access by reference to avoid copying
	{
		glColor3f(0, 0, 1);
		DrawCircle(bullet.first, bullet.second, bullet_size);
	}

	for (auto& bullet : boss_bullets) // access by reference to avoid copying
	{
		glColor3f(1, 0, 0);
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

// draws the boss health bar
void DrawBossHealthBar() {
	glColor3f(1, 1, 1);
	DrawRectangle(195, window_y_size - 30, 410, 25);
	glColor3f(1, 0.3, 0.3);
	DrawRectangle(200, window_y_size - 25, 400 * (boss_hp / boss_max_hp), 15);
}

// draws the player health bar
void DrawPlayerHealthBar() {
	glColor3f(1, 1, 1);
	DrawRectangle(245, 5, 310, 25);
	glColor3f(1, 0.3, 0.3);
	DrawRectangle(250, 10, 300 * (player_hp / player_max_hp), 15);
}

// draws the power up while adding the movement of the power up
void DrawPowerUp() {
	glPushMatrix();
	glTranslatef(power_up_x, power_up_y, 0);
	glColor3f(1, 0, 1);
	DrawCircle(0, 0, power_up_size);
	glPopMatrix();
}

// main loop called every delta_time to update state
void GameLoopCB(int value) {
	// check if game over
	if (player_hp == 0 || boss_hp == 0)
	{
		game_over = true;
	}

	// update boss location
	if (boss_x + boss_size >= 800)
		boss_direction = false;
	else if (boss_x - boss_size <= 0)
		boss_direction = true;

	if (boss_direction)
		boss_x += boss_speed;
	else
		boss_x -= boss_speed;

	// update player bullets' locations & detect hitting boss
	for (auto bullet = player_bullets.begin(); bullet < player_bullets.end(); )
	{
		// collision with boss
		if (IsCollision(bullet->first, bullet->second, bullet_size, boss_x, boss_y, boss_size)) {
			bullet = player_bullets.erase(bullet);
			boss_hp--;
			continue;
		}

		// update location
		bullet->second += player_bullet_speed;
		if (bullet->second > window_y_size)
			bullet = player_bullets.erase(bullet);
		else
			bullet++;
	}

	// update boss bullets' locations & detect hitting player
	for (auto bullet = boss_bullets.begin(); bullet < boss_bullets.end(); )
	{
		// collision with player
		if (!player_invulnerability && IsCollision(bullet->first, bullet->second, bullet_size, player_x, player_y, player_size)) {
			bullet = boss_bullets.erase(bullet);
			player_hp--;
			continue;
		}

		// update location
		bullet->second -= boss_bullet_speed;
		if (bullet->second < 0)
			bullet = boss_bullets.erase(bullet);
		else
			bullet++;
	}

	// detect power up collision
	if (IsCollision(power_up_x, power_up_y, power_up_size, player_x, player_y, player_size)) {
		player_invulnerability = true;
		power_up_x = rand() % (window_x_size - power_up_size) + power_up_size;
		power_up_y = rand() % (window_y_size - power_up_size - 200) + power_up_size + 100;
		glutTimerFunc(power_up_duration, PowerUpDecayCB, 0);
	}

	glutTimerFunc(delta_time, GameLoopCB, 0);
	glutPostRedisplay();
}

// removes power ups
void PowerUpDecayCB(int value) {
	player_invulnerability = false;
}

// creates bullets depending on player_fire_rate
void PlayerShootCB(int value) {
	// make player shoot
	player_bullets.push_back(make_pair(player_x, player_y + player_size + bullet_size));

	glutTimerFunc(player_fire_rate, PlayerShootCB, 0);
}

// creates bullets depending on boss_fire_rate
void BossShootCB(int value) {
	// make boss shoot
	boss_bullets.push_back(make_pair(boss_x, boss_y - boss_size - bullet_size));

	glutTimerFunc(boss_fire_rate, BossShootCB, 0);
}

// detects collision using rectanges for hitboxes
// x1,y1 = center of object 1
// size1 = size of object 1
// x2,y2 = center of object 2
// size2 = size of object 2
bool IsCollision(int x1, int y1, int size1, int x2, int y2, int size2) {
	pair<int, int> top_right1 = make_pair(x1 + size1, y1 + size1);
	pair<int, int> bottom_left1 = make_pair(x1 - size1, y1 - size1);

	pair<int, int> top_right2 = make_pair(x2 + size2, y2 + size2);
	pair<int, int> bottom_left2 = make_pair(x2 - size2, y2 - size2);

	// If one rectangle is on left side of other
	if (top_right1.first < bottom_left2.first || top_right2.first < bottom_left1.first)
		return false;

	// If one rectangle is above other
	if (top_right1.second < bottom_left2.second || top_right2.second < bottom_left1.second)
		return false;

	return true;
}

//this is the method used to print text in OpenGL
//there are three parameters,
//the first two are the coordinates where the text is display,
//the third coordinate is the string containing the text to display
void Print(int x, int y, char* string)
{
	int len, i;

	//set the position of the text in the window using the x and y coordinates
	glRasterPos2f(x, y);

	//get the length of the string to display
	len = (int)strlen(string);

	//loop to display character by character
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
}