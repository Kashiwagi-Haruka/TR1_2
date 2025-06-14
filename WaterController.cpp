#include "WaterController.h"
#include <cmath>
#include <imgui.h>
#include <string>


// 必要に応じてグローバル定数で水色も調整
static unsigned int BaseWaterColor = 0x99CFFFEE; // 水色・半透明

void WaterController::Initialize() {
	handle2_ = Novice::LoadTexture("./Resources/Water2.png");
	splashTex = Novice::LoadTexture("./Resources/Splash.png");
	isCharging_ = false;
	isFired_ = false;

	// 格子点の初期化
	gridPoints_.resize(kRowCount + 1);
	for (int i = 0; i <= kRowCount; ++i) {
		gridPoints_[i].resize(kColCount + 1);
		for (int j = 0; j <= kColCount; ++j) {
			// 初期値はあとでうねり計算で毎フレーム上書きされる
			gridPoints_[i][j] = {640.0f, 360.0f};
		}
	}
}

void WaterController::StartCharge(const Vector2& mousePos, bool justPressed) {
	if (justPressed) {
	}
	mousePos_ = mousePos;
	isCharging_ = true;
	isFired_ = false;
}

void WaterController::Update() {}

void WaterController::Fire() {
	isFired_ = true;
	isCharging_ = false;
}

void WaterController::Draw() {
	float time = static_cast<float>(clock()) / CLOCKS_PER_SEC;
	Vector2 center = mousePos_;
	float ringRadius = 250.0f; // 基本の半径
	float rotSpeed = 0.7f;     // 回転速度
	float angleOffset = time * rotSpeed;

	// 格子点自動生成（うねり＆流線感つき）
	for (int i = 0; i <= kRowCount; ++i) {
		//float bandRate = (float)i / kRowCount; // 0～1
		for (int j = 0; j <= kColCount; ++j) {
			float theta = angleOffset + 2.0f * 3.1415926f * (float)j / kColCount;

			// --- 端だけ丸め補正 ---
			float edgeCurve = 0.0f;
			// 上端 or 下端
			if (i == 0 || i == kRowCount) {
				// 端の「端っこに近い」ほど持ち上げる（jの両端は強く、中央はそのまま）
				float edgeFactor = 1.0f - powf(fabsf((float)j / kColCount - 0.5f) * 2.0f, 2.0f); // 中央0、端1
				edgeCurve = 60.0f * edgeFactor;                                                  // 数字を大きくすると端の丸みが強調される
			}
			// 左端 or 右端
			if (j == 0 || j == kColCount) {
				float edgeFactor = 1.0f - powf(fabsf((float)i / kRowCount - 0.5f) * 2.0f, 2.0f);
				edgeCurve += 60.0f * edgeFactor;
			}

			// 元の半径＋丸み
			float base = ringRadius + (i - kRowCount / 2.0f) * 28.0f + edgeCurve;

			// 残りのwave等はそのまま
			float wave1 = 40.0f * sinf(time * 1.6f + i * 0.5f + j * 0.13f);
			float wave2 = 18.0f * sinf(time * 3.5f + i * 1.3f - j * 0.23f);
			float squeeze = 70.0f * sinf(time * 1.1f + (float)j / kColCount * 3.14f) * (1.0f - powf(fabsf((i - kRowCount / 2.0f) / (kRowCount / 2.0f)), 1.5f));

			float r = base + wave1 + wave2 + squeeze;

			gridPoints_[i][j].x = center.x + r * cosf(theta);
			gridPoints_[i][j].y = center.y + r * sinf(theta);
		}
	}


	// --- 水色や透明感を帯びた色にする ---
	for (int i = 0; i < kRowCount; ++i) {
		int uvRow = 1;
		if (i == 0)
			uvRow = 0;
		else if (i == kRowCount - 1)
			uvRow = 2;

		for (int j = 0; j < kColCount; ++j) {
			int uvCol = 1;
			if (j == 0)
				uvCol = 0;
			else if (j == kColCount - 1)
				uvCol = 2;

			Vector2 v1 = gridPoints_[i][j];
			Vector2 v2 = gridPoints_[i][j + 1];
			Vector2 v3 = gridPoints_[i + 1][j + 1];
			Vector2 v4 = gridPoints_[i + 1][j];

			int srcX = uvCol * pieceW_;
			int srcY = uvRow * pieceH_;

			// “水感”のため色に明るさやα揺らぎをプラスしても良い
			float intensity = 0.8f + 0.15f * sinf(j * 2.0f + i * 0.5f + time * 2.0f);
			// intensityでR,G,B,Aを計算（AABBGGRR）
			unsigned int color = ((int)(220 * intensity) << 24) |     // AA
			                     ((int)(255) << 16) |                 // BB
			                     ((int)(200 + 50 * intensity) << 8) | // GG
			                     ((int)(150 + 80 * intensity));       // RR

			Novice::SetBlendMode(kBlendModeNormal);
			Novice::DrawQuad((int)v1.x, (int)v1.y, (int)v2.x, (int)v2.y, (int)v4.x, (int)v4.y, (int)v3.x, (int)v3.y, srcX, srcY, pieceW_, pieceH_, handle2_, color);
		}
	}
	// 毎フレーム、一定確率で水流格子からランダム発生
	if (splashes.size() < 80) { // 最大個数制限
		for (int tryCount = 0; tryCount < 2; ++tryCount) {
			int i = mt() % (kRowCount + 1);
			int j = mt() % (kColCount + 1);
			Vector2 base = gridPoints_[i][j];
			float ang = (mt() % 360) * 3.14159f / 180.0f;
			float speed = 3.0f + (mt() % 30) / 10.0f; // 3.0～6.0くらい
			SplashParticle sp;
			sp.pos = base;
			sp.vel = {cosf(ang) * speed, sinf(ang) * speed - 2.0f}; // -2.0fで重力感
			sp.lifetime = 0.4f + (mt() % 40) / 100.0f;              // 0.4～0.8秒
			sp.age = 0;
			sp.scale = 0.01f + (mt() % 50) / 1000.0f; // 0.6～1.1
			sp.angle = ang;
			splashes.push_back(sp);
		}
	}
	// 更新と描画
	for (auto it = splashes.begin(); it != splashes.end();) {
		it->age += 1.0f / 60.0f;
		// 位置更新（落下や跳ね返りっぽさ追加したい場合はY方向加速なども）
		it->pos.x += it->vel.x;
		it->pos.y += it->vel.y;
		it->vel.y += 0.4f; // 重力

		// 透明度を徐々に減衰
		float alpha = 1.0f - (it->age / it->lifetime);
		if (alpha < 0.0f)
			alpha = 0.0f;
		unsigned int color = ((int)(220 * alpha) << 24) | (255 << 16) | (255 << 8) | 255;
		Novice::SetBlendMode(kBlendModeAdd);
		// 回転・拡大ありで描画
		Novice::DrawSpriteRect(
		    (int)(it->pos.x - 32 * it->scale), (int)(it->pos.y - 32 * it->scale), 
			0, 0, 512, 512, // 画像全体
			splashTex, 
			it->scale, it->scale, 
			it->angle, 
			color);
		
		// 寿命切れで削除
		if (it->age > it->lifetime) {
			it = splashes.erase(it);
		} else {
			++it;
		}
	}
}
