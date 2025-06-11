#pragma once
#include <KamataEngine.h>
#include <Novice.h>
using namespace KamataEngine;

class WaterBullet {
public:
	Vector2 pos;
	Vector2 velocity;
	bool isActive;

	WaterBullet(Vector2 startPos, Vector2 vel) : pos(startPos), velocity(vel), isActive(true) {}

	void Update() {
		if (isActive) {
			pos.x += velocity.x;
			pos.y += velocity.y;
		}
	}
};
