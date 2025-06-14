#pragma once
#include <KamataEngine.h>
#include <Novice.h>
#include <array>
#include <vector>
#include <random>
using namespace KamataEngine;

// 分割数
static const int kRowCount = 12; // 縦分割（行数）
static const int kColCount = 50; // 横分割（列数）



class WaterController {
public:
	void Initialize();
	void Update();
	void Draw();
	void StartCharge(const Vector2& mousePos, bool justPressed);
	void Fire();

private:
	int handle2_; // Water2.png（水流画像）

	Vector2 mousePos_;
	bool isCharging_ = false;
	bool isFired_ = false;

	// Water2.pngの分割
	static const int gridNum_ = 3;
	static const int texSize_ = 512;
	static const int pieceW_ = texSize_ / gridNum_;
	static const int pieceH_ = texSize_ / gridNum_;

	// 格子点配列: [行][列]
	std::vector<std::vector<Vector2>> gridPoints_;

	struct SplashParticle {
		Vector2 pos;
		Vector2 vel;
		float lifetime;
		float age;
		float scale;
		float angle;
	};

	std::vector<SplashParticle> splashes;
	int splashTex = -1; // 水しぶき画像テクスチャ
	std::mt19937 mt{std::random_device{}()};
};
