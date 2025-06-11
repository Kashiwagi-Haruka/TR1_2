#pragma once
#include <KamataEngine.h>
#include <Novice.h>
using namespace KamataEngine;

struct WaterPiece {
	int gridX, gridY; // 元画像グリッド上の座標
	Vector2 pos;      // 画面上での描画座標
	float t;          // 曲線進行度[0,1]
	bool reached;     // 吸着済みフラグ

	WaterPiece(int gx, int gy, Vector2 p) : gridX(gx), gridY(gy), pos(p), t(0), reached(false) {}
};
