#ifndef _CSURFACE_H_
	#define _CSURFACE_H
#endif

#include <iostream>
#include <glm/glm.hpp>
#include <math.h>
#include <algorithm>
#include <SDL.h>
#include <SDL_image.h>
#include "SDLauxiliary.h"
#include "TestModel.h"

#undef main

using namespace std;
using glm::vec3;
using glm::mat3;

// ----------------------------------------------------------------------------
// GLOBAL VARIABLES

double PI = 3.1415926535;
int oo = 1000000000;

const int SCREEN_WIDTH = 1366;
const int SCREEN_HEIGHT = 768;
int f = 400;
const int imageSize = 400;
SDL_Surface* screen;
int t = 0;
int dt = 0;
int tr = 0;
vec3 velocity = vec3(0.0, 0.0, 0.0);
vector<Planet> planets;

int mode = 0;
const double colorShift = 0.6;
const int PLANET_ICON_MARGIN = 10;
const int PLANET_ICON_SIZE = 50;
const int MOVING_DRAGBAR_WIDTH = 250;
const int MOVING_DRAGBAR_HEIGHT = 50;
double movingSpeed = 0.9;
const double MOVING_SPEED_MIN = 0.1;
const double MOVING_SPEED_MAX = 4;
double playSpeed = 1.0;
const double PLAY_SPEED_MIN = 0.1;
const double PLAY_SPEED_MAX = 4;
SDL_Surface *MOVING_IMAGE = IMG_Load("images/moving.png");

int planetFocus = -1;

float depthBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

vec3 cameraP(0, 15, -5);
mat3 cameraD(vec3(1, 0, 0), vec3(0, 0.707, 0.707), vec3(0, -0.707, 0.707));
// vec3 cameraP(0, 20, 0);
// mat3 cameraD(vec3(1, 0, 0), vec3(0, 0, 1), vec3(0, -1, 0));

int mouseX;
int mouseY;
bool mouseDown;
int mouseXB = 0;
int mouseYB = 0;
bool mouseDownB;

// ----------------------------------------------------------------------------
// FUNCTIONS

void Update();
void Draw();

int main(int argc, char* argv[]) {
	screen = InitializeSDL(SCREEN_WIDTH, SCREEN_HEIGHT);
	t = SDL_GetTicks();	// Set start value for timer.

	LoadTestModel(planets);

	while (NoQuitMessageSDL()) {
		Update();
		Draw();
	}

	SDL_SaveBMP(screen, "screenshot.bmp");
	return 0;
}

void focusOnPoint(vec3 p) {
	vec3 z1 = p;
	double z1Len = sqrt(z1.x * z1.x + z1.y * z1.y + z1.z * z1.z);
	z1 /= z1Len;
	double a = atan2(z1.z, z1.x);
	vec3 y1 = vec3(-cos(a) * z1.y, sqrt(1 - z1.y * z1.y), -sin(a) * z1.y);
	vec3 x1 = -1.0f * glm::cross(z1, y1);
	cameraD = mat3(x1, y1, z1);
	f = 50.0 / 2 * z1Len;
}

void calculatePlanetPos() {
	for (size_t i = 0; i < planets.size(); i++) {
		Planet planet = planets[i];
		double radius = sqrt(planet.p.x * planet.p.x + planet.p.z * planet.p.z);
		double a = -planet.speed / radius / 1000 * dt;
		mat3 change(vec3(cos(a), 0, -sin(a)), vec3(0, 1, 0), vec3(sin(a), 0, cos(a)));
		planets[i].p = change * planets[i].p;
		a = planet.rotate / 1000 * dt;
		change = mat3(vec3(cos(a), 0, -sin(a)), vec3(0, 1, 0), vec3(sin(a), 0, cos(a)));
		planets[i].sdir = change * planets[i].sdir;
	}
}

void updateGUI() {
	if (mouseDown && !mouseDownB) {
		for (size_t o = 0; o < planets.size(); o++) {
			int x = PLANET_ICON_MARGIN;
			int y = PLANET_ICON_MARGIN + (PLANET_ICON_MARGIN + PLANET_ICON_SIZE) * o;
			if (mouseX < x || mouseY < y || mouseX > x + PLANET_ICON_SIZE || mouseY > y + PLANET_ICON_SIZE) continue;
			if (planetFocus == o) {
				planetFocus = -1;
			} else {
				planetFocus = o;
			}
		}
		for (int o = 0; o < 3; o++) {
			int x = SCREEN_WIDTH - 10 - 30;
			int y = 10 + o * (5 + 30);
			double r = sqrt((mouseX - x - 15) * (mouseX - x - 15) + (mouseY - y - 15) * (mouseY - y - 15));
			if (r < 15) {
				mode = o;
			}
		}
	}
	if (mouseDown) {
		int x = 10;
		int y = SCREEN_HEIGHT - 10 - MOVING_DRAGBAR_HEIGHT;
		if (mouseX > x && mouseX < x + MOVING_DRAGBAR_WIDTH - 5 && mouseY > y && mouseY < y + MOVING_DRAGBAR_HEIGHT) {
			movingSpeed = double(mouseX - x) / (MOVING_DRAGBAR_WIDTH - 5) * (MOVING_SPEED_MAX - MOVING_SPEED_MIN) + MOVING_SPEED_MIN;
		}
		x = SCREEN_WIDTH - 10 - MOVING_DRAGBAR_WIDTH;
		y = SCREEN_HEIGHT - 10 - MOVING_DRAGBAR_HEIGHT;
		if (mouseX > x && mouseX < x + MOVING_DRAGBAR_WIDTH - 5 && mouseY > y && mouseY < y + MOVING_DRAGBAR_HEIGHT) {
			playSpeed = double(mouseX - x) / (MOVING_DRAGBAR_WIDTH - 5) * (PLAY_SPEED_MAX - PLAY_SPEED_MIN) + PLAY_SPEED_MIN;
		}
	}
}

void Update() {
	// Compute frame time:
	int t2 = SDL_GetTicks();
	dt = t2 - t;
	cout << "Render time: " << dt << " ms." << endl;
	
	t = t2;
	dt *= playSpeed;
	tr += dt;

	SDL_PumpEvents();
	mouseDown = SDL_GetMouseState(&mouseX, &mouseY);
	int dx = mouseX - mouseXB;
	int dy = mouseY - mouseYB;

	if (mode == 0 && planetFocus != -1) {
		focusOnPoint(planets[planetFocus].gp - cameraP);
	} else if (planetFocus == -1) {
		if (mouseDown && mouseX > 60 && mouseX < SCREEN_WIDTH - 60 && mouseY < SCREEN_HEIGHT - 60) {
			double a = double(dx) / 300;
			mat3 change(vec3(cos(a), 0, -sin(a)), vec3(0, 1, 0), vec3(sin(a), 0, cos(a)));
			cameraD = cameraD * change;
			a = -double(dy) / 300;
			change = mat3(vec3(1, 0, 0), vec3(0, cos(a), -sin(a)), vec3(0, sin(a), cos(a)));
			cameraD = cameraD * change;
			a = -(atan2(cameraD[1][1], cameraD[0][1]) / PI * 180 - 90) / 180 * PI;
			change = mat3(vec3(cos(a), -sin(a), 0), vec3(sin(a), cos(a), 0), vec3(0, 0, 1));
			cameraD = cameraD * change;
		}
	}

	updateGUI();

	mouseXB = mouseX;
	mouseYB = mouseY;
	mouseDownB = mouseDown;

	Uint8* keystate = SDL_GetKeyState(0);
	double speed = movingSpeed;
	velocity = vec3(0, 0, 0);
	if (keystate[SDLK_w]) {
		velocity += vec3(0, 0, speed);
	}
	if (keystate[SDLK_s]) {
		velocity += vec3(0, 0, -speed);
	}
	if (keystate[SDLK_a]) {
		velocity += vec3(-speed, 0, 0);
	}
	if (keystate[SDLK_d]) {
		velocity += vec3(speed, 0, 0);
	}
	if (keystate[SDLK_q]) {
		velocity += vec3(0, -speed, 0);
	}
	if (keystate[SDLK_e]) {
		velocity += vec3(0, speed, 0);
	}
	velocity = cameraD * velocity;
	cameraP += (float(dt) / 1000) * velocity;

	calculatePlanetPos();
}

void drawPlanet(Planet planet) {
	vec3 p = glm::inverse(cameraD) * (planet.gp - cameraP);
	int x = round(p.x / p.z * f) + SCREEN_WIDTH / 2;
	int y = -round(p.y / p.z * f) + SCREEN_HEIGHT / 2;
	int r = round(planet.r / p.z * f);
	mat3 dir = glm::inverse(cameraD) * planet.gdir * glm::transpose(planet.sdir);
	float imageRatio = float(abs(t % 2000 - 1000)) / 1000;
	for (int i = max(0, x - r); i < min(SCREEN_WIDTH, x + r); i++) {
		for (int j = max(0, y - r); j < min(SCREEN_HEIGHT, y + r); j++) {
			if (sqrt((i - x) * (i - x) + (j - y) * (j - y)) > r) continue;
			double x1 = double(i - x) / r;
			double y1 = -double(j - y) / r;
			double z1 = -sqrt(1 - x1 * x1 - y1 * y1);
			vec3 p1 = vec3(x1, y1, z1);
			p1 = glm::inverse(dir) * p1;
			double zinv = 1.0 / (p1.z * planet.r + p.z);
			if (zinv > depthBuffer[j][i]) {
				depthBuffer[j][i] = zinv;
				vec3 pixel = GetPixelSDL(planet.image, round((p1.x + 1) / 2 * imageSize * 0.98), round((-p1.y + 1) / 2 * imageSize * 0.98));
				vec3 pixel1 = GetPixelSDL(planet.image1, round((p1.x + 1) / 2 * imageSize * 0.98), round((-p1.y + 1) / 2 * imageSize * 0.98));
				PutPixelSDL(screen, i, j, imageRatio * pixel + float(1.0 - imageRatio) * pixel1);
			}
		}
	}
}

void drawPlanet1(Planet planet, bool focus) {
	int tmin = tr - 100000000;
	int tmax = tr;
	vec3 p;
	vec3 v;
	if (mode == 0) {
		tmin = tr - 1;
		tmax = tr + 1;
	}
	for (; tmax - tmin > 1;) {
		int tmid = (tmin + tmax) / 2;
		double dis = double(tr - tmid) / 1000;
		p = vec3(0, 0, 0);
		v = vec3(0, 0, 0);
		for (Planet c = planet;; c = planets[c.parent]) {
			double radius = sqrt(c.p.x * c.p.x + c.p.z * c.p.z);
			double a = -c.speed / radius / 1000 * tmid;
			mat3 change(vec3(cos(a), 0, -sin(a)), vec3(0, 1, 0), vec3(sin(a), 0, cos(a)));
			p = change * c.sp + c.gdir * p;
			v = change * vec3(-c.sp.z / radius * c.speed, 0, c.sp.x / radius * c.speed) + c.gdir * v;
			if (c.parent == -1) break;
		}
		p -= cameraP;
		double dis2 = sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
		if (dis > dis2) {
			tmin = tmid;
		} else {
			tmax = tmid;
		}
	}
	double velocityX = velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z;

	if (focus) {
		cout << p.x << " " << p.y << " " << p.z << endl;
		focusOnPoint(p);
	}

	p = glm::inverse(cameraD) * p;
	int x = round(p.x / p.z * f) + SCREEN_WIDTH / 2;
	int y = -round(p.y / p.z * f) + SCREEN_HEIGHT / 2;
	int r = round(planet.r / p.z * f);
	double a = planet.rotate / 1000 * tmax;
	mat3 sdir(vec3(cos(a), 0, -sin(a)), vec3(0, 1, 0), vec3(sin(a), 0, cos(a)));
	mat3 dir = glm::inverse(cameraD) * planet.gdir * glm::transpose(sdir);
	for (int i = max(0, x - r); i < min(SCREEN_WIDTH, x + r); i++) {
		for (int j = max(0, y - r); j < min(SCREEN_HEIGHT, y + r); j++) {
			if (sqrt((i - x) * (i - x) + (j - y) * (j - y)) > r) continue;
			double x1 = double(i - x) / r;
			double y1 = -double(j - y) / r;
			double z1 = -sqrt(1 - x1 * x1 - y1 * y1);
			double rr = sqrt(x1 * x1 + z1 * z1) * planet.r;
			vec3 v1(-z1, 0, x1);
			v1 = float(PI * 1.0 / sqrt(z1 * z1 + x1 * x1) * planet.r * planet.rotate) * v1;
			v1 += v;
			double vX = v1.x * v1.x + v1.y * v1.y + v1.z * v1.z;
			double vvX = velocity.x * v1.x + velocity.y * v1.y + velocity.z * v1.z;
			double yA = 1.0 / sqrt(1 - velocityX);
			vec3 vr;
			if (velocityX > 0.0001 && velocityX < 1.0) {
				vr = float(1.0 / yA / (1 - vvX)) * (v1 - velocity + float((yA - 1) * (vvX / velocityX - 1)) * velocity);
			} else {
				vr = v1;
			}

			vec3 p1 = vec3(x1, y1, z1);
			p1 = glm::inverse(dir) * p1;
			double zinv = 1.0 / (p1.z * planet.r + p.z);
			if (zinv > depthBuffer[j][i]) {
				depthBuffer[j][i] = zinv;
				vec3 pixel = GetPixelSDL(planet.image, round((p1.x + 1) / 2 * imageSize * 0.98), round((-p1.y + 1) / 2 * imageSize * 0.98));
				vec3 pixel1 = GetPixelSDL(planet.image1, round((p1.x + 1) / 2 * imageSize * 0.98), round((-p1.y + 1) / 2 * imageSize * 0.98));
				int t1 = double(tmax) - z1 * planet.r * 1000;
				if (mode == 0) t1 = tmax;
				float imageRatio = float(abs(abs(t1 % 2000) - 1000)) / 1000;
				vec3 pixel2 = imageRatio * pixel + float(1.0 - imageRatio) * pixel1;
				if (mode == 2 && velocityX < 1.0) {
					if (vr.z < 0) {
						pixel2.x *= 1 + vr.z * colorShift;
						pixel2.y *= 1 + vr.z * colorShift;
						pixel2.z += (1 - pixel2.z) * -vr.z * colorShift;
					} else {
						pixel2.x += (1 - pixel2.z) * vr.z * colorShift;
						pixel2.y *= 1 - vr.z * colorShift;
						pixel2.z *= 1 - vr.z * colorShift;
					}
				}
				PutPixelSDL(screen, i, j, pixel2);
			}
		}
	}
}

void drawGUI() {
	// Planet icon
	for (size_t o = 0; o < planets.size(); o++) {
		int x = PLANET_ICON_MARGIN;
		int y = PLANET_ICON_MARGIN + (PLANET_ICON_MARGIN + PLANET_ICON_SIZE) * o;
		for (int i = 0; i < PLANET_ICON_SIZE; i++) {
			for (int j = 0; j < PLANET_ICON_SIZE; j++) {
				double r = sqrt((i - PLANET_ICON_SIZE / 2) * (i - PLANET_ICON_SIZE / 2) + (j - PLANET_ICON_SIZE / 2) * (j - PLANET_ICON_SIZE / 2));
				if (r > PLANET_ICON_SIZE / 2) continue;
				vec3 pixel = GetPixelSDL(planets[o].image, round(double(i) / PLANET_ICON_SIZE * imageSize), round(double(j) / PLANET_ICON_SIZE * imageSize));
				PutPixelSDL(screen, i + x, j + y, pixel);
			}
		}
		if (planetFocus == o) {
			for (int i = -5; i < PLANET_ICON_SIZE + 5; i++) {
				for (int j = -5; j < PLANET_ICON_SIZE + 5; j++) {
					if (i < 0 || j < 0 || i > PLANET_ICON_SIZE || j > PLANET_ICON_SIZE) {
						PutPixelSDL(screen, i + x, j + y, vec3(1, 0, 0));
					}
				}
			}
		}
	}

	// Moving speed
	int x = 10;
	int y = SCREEN_HEIGHT - 10 - MOVING_DRAGBAR_HEIGHT;
	for (int i = 0; i < MOVING_DRAGBAR_WIDTH; i++) {
		for (int j = 0; j < MOVING_DRAGBAR_HEIGHT; j++) {
			PutPixelSDL(screen, i + x, j + y, GetPixelSDL(MOVING_IMAGE, i * 0.75, j));
		}
	}
	int x1 = round((movingSpeed - MOVING_SPEED_MIN) / (MOVING_SPEED_MAX - MOVING_SPEED_MIN) * (MOVING_DRAGBAR_WIDTH - 5));
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < MOVING_DRAGBAR_HEIGHT - 15; j++) {
			PutPixelSDL(screen, i + x + x1, j + y, vec3(1, 0, 0));
		}
	}

	// Play speed
	x = SCREEN_WIDTH - 10 - MOVING_DRAGBAR_WIDTH;
	y = SCREEN_HEIGHT - 10 - MOVING_DRAGBAR_HEIGHT;
	for (int i = 0; i < MOVING_DRAGBAR_WIDTH; i++) {
		for (int j = 0; j < MOVING_DRAGBAR_HEIGHT; j++) {
			PutPixelSDL(screen, i + x, j + y, GetPixelSDL(MOVING_IMAGE, i * 0.75, j));
		}
	}
	x1 = round((playSpeed - PLAY_SPEED_MIN) / (PLAY_SPEED_MAX - PLAY_SPEED_MIN) * (MOVING_DRAGBAR_WIDTH - 5));
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < MOVING_DRAGBAR_HEIGHT - 15; j++) {
			PutPixelSDL(screen, i + x + x1, j + y, vec3(1, 0, 0));
		}
	}

	// Mode
	for (int o = 0; o < 3; o++) {
		x = SCREEN_WIDTH - 10 - 30;
		y = 10 + o * (5 + 30);
		for (int i = 0; i < 30; i++) {
			for (int j = 0; j < 30; j++) {
				double r = sqrt((i - 15) * (i - 15) + (j - 15) * (j - 15));
				if (r > 15) continue;
				if (r > 12 || mode == o) {
					PutPixelSDL(screen, i + x, j + y, vec3(1, 0, 0));
				}
			}
		}
	}
}

void Draw() {
	SDL_FillRect(screen, 0, 0);
	for (int i = 0; i < SCREEN_HEIGHT; i++) {
		for (int j = 0; j < SCREEN_WIDTH; j++) {
			depthBuffer[i][j] = 0;
		}
	}

	if (SDL_MUSTLOCK(screen)) {
		SDL_LockSurface(screen);
	}

	for (size_t i = 0; i < planets.size(); i++) {
		if (planets[i].parent != -1) {
			planets[i].gp = planets[planets[i].parent].gdir * planets[i].p + planets[planets[i].parent].gp;
			planets[i].gdir = planets[planets[i].parent].gdir * planets[i].gdir;
		}
		drawPlanet1(planets[i], i == planetFocus);
	}

	drawGUI();

	if (SDL_MUSTLOCK(screen)) {
		SDL_UnlockSurface(screen);
	}
	SDL_UpdateRect(screen, 0, 0, 0, 0);
}