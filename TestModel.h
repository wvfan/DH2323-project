#ifndef TEST_MODEL_CORNEL_BOX_H
#define TEST_MODEL_CORNEL_BOX_H

using namespace std;

#include <glm/glm.hpp>
#include <vector>
#include <string.h>
#include <math.h>

// Used to describe a triangular surface:
class Planet {
	public:
	glm::vec3 p;
	glm::vec3 sp;
	glm::vec3 gp;
	glm::mat3 dir;
	glm::mat3 gdir;
	glm::mat3 sdir;
	double r;
	double speed;
	double rotate;
	SDL_Surface *image;
	SDL_Surface *image1;
	int parent;

	Planet() {

	}

	Planet(glm::vec3 p, glm::mat3 dir, double r, double speed, double rotate, string img, string img1, int parent) {
		this->p = p;
		this->sp = p;
		this->gp = p;
		this->dir = dir;
		this->gdir = dir;
		this->sdir = glm::mat3(glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
		this->r = r;
		this->speed = speed;
		this->rotate = rotate;
		string imagePath = "images/" + img + ".png";
		string imagePath1 = "images/" + img1 + ".png";
		this->image = IMG_Load(imagePath.c_str());
		this->image1 = IMG_Load(imagePath1.c_str());
		this->parent = parent;
	}
};

void LoadTestModel( std::vector<Planet>& planets ) {
	using glm::vec3;
	double angle;
	double pos;
	double dis;
	double PI = 3.1415926535;

	planets.clear();
	planets.reserve(30);

	planets.push_back(Planet(vec3(0, 0, 10), glm::mat3(vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1)), 2.318, 0, 0.13, "sun", "sun1", -1));
	angle = 12.0 / 180 * PI;
	pos = 0.0 / 180 * PI;
	dis = 6;
	planets.push_back(Planet(vec3(cos(pos) * dis, -0.8, -sin(pos) * dis), glm::mat3(vec3(cos(angle), -sin(angle), 0), vec3(sin(angle), cos(angle), 0), vec3(0, 0, 1)), 0.6, 0.5, 0.2, "mars", "mars", 0));
	angle = 18.0 / 180 * PI;
	pos = 50.0 / 180 * PI;
	dis = 8;
	planets.push_back(Planet(vec3(cos(pos) * dis, -1.0, -sin(pos) * dis), glm::mat3(vec3(cos(angle), -sin(angle), 0), vec3(sin(angle), cos(angle), 0), vec3(0, 0, 1)), 0.3, 0.5, 0.2, "mercury", "mercury", 0));
	angle = 15.0 / 180 * PI;
	pos = 100.0 / 180 * PI;
	dis = 10;
	planets.push_back(Planet(vec3(cos(pos) * dis, -1.2, -sin(pos) * dis), glm::mat3(vec3(cos(angle), -sin(angle), 0), vec3(sin(angle), cos(angle), 0), vec3(0, 0, 1)), 0.8, 0.5, 0.2, "jupiter", "jupiter", 0));
	angle = 25.0 / 180 * PI;
	pos = 150.0 / 180 * PI;
	dis = 12;
	planets.push_back(Planet(vec3(cos(pos) * dis, -1.6, -sin(pos) * dis), glm::mat3(vec3(cos(angle), -sin(angle), 0), vec3(sin(angle), cos(angle), 0), vec3(0, 0, 1)), 0.5, 0.5, 0.2, "uranus", "uranus", 0));
	angle = 30.0 / 180 * PI;
	pos = 200.0 / 180 * PI;
	dis = 14;
	planets.push_back(Planet(vec3(cos(pos) * dis, -2.0, -sin(pos) * dis), glm::mat3(vec3(cos(angle), -sin(angle), 0), vec3(sin(angle), cos(angle), 0), vec3(0, 0, 1)), 0.4, 0.5, 0.5, "neptune", "neptune", 0));
}

#endif
