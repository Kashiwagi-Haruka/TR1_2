#include "WaterController.h"
#include <cmath>
#include <imgui.h>
#include <string>

// 初期化
void WaterController::Initialize() {
	handle2_ = Novice::LoadTexture("./Resources/Water2.png");
	isCharging_ = false;
	isFired_ = false;

	// 格子点の初期化
	gridPoints_.resize(kRowCount + 1);
	for (int i = 0; i <= kRowCount; ++i) {
		gridPoints_[i].resize(kColCount + 1);
		for (int j = 0; j <= kColCount; ++j) {
			float tx = (float)j / kColCount;
			float ty = (float)i / kRowCount;
			gridPoints_[i][j] = {100 + tx * 1080.0f, 300 + 200 * ty + 30 * sinf(ty * 3.14f) + 30 * sinf(tx * 6.28f)};
		}
	}
}

void WaterController::StartCharge(const Vector2& mousePos, bool justPressed) {
	if (justPressed) {
	}
	mousePos_ = mousePos;
	isCharging_ = true;
	isFired_ = false;

	// 右端(j=kColCount)をマウス位置に寄せる例
	for (int i = 0; i <= kRowCount; ++i) {
		float ty = (float)i / kRowCount;
		gridPoints_[i][kColCount].x = mousePos.x;
		gridPoints_[i][kColCount].y = mousePos.y - 60 + (120.0f * ty);
	}
}

void WaterController::Update() {}

void WaterController::Fire() {
	isFired_ = true;
	isCharging_ = false;
}

void WaterController::Draw() {
	// --- ImGuiで格子点編集 ---
	ImGui::Begin("Water Grid Editor");
	for (int i = 0; i <= kRowCount; ++i) {
		for (int j = 0; j <= kColCount; ++j) {
			std::string label = "P[" + std::to_string(i) + "][" + std::to_string(j) + "]";
			ImGui::SliderFloat2(label.c_str(), &gridPoints_[i][j].x, 0.0f, 1280.0f);
		}
	}
	ImGui::End();

	// --- 描画 ---
	for (int i = 0; i < kRowCount; ++i) {
		// 画像の縦グリッド選択
		int uvRow = 1;
		if (i == 0)
			uvRow = 0;
		else if (i == kRowCount - 1)
			uvRow = 2;

		for (int j = 0; j < kColCount; ++j) {
			// 横グリッド選択
			int uvCol = 1;
			if (j == 0)
				uvCol = 0;
			else if (j == kColCount - 1)
				uvCol = 2;

			// 4頂点
			Vector2 v1 = gridPoints_[i][j];         // 左上
			Vector2 v2 = gridPoints_[i][j + 1];     // 右上
			Vector2 v3 = gridPoints_[i + 1][j + 1]; // 右下
			Vector2 v4 = gridPoints_[i + 1][j];     // 左下

			int srcX = uvCol * pieceW_;
			int srcY = uvRow * pieceH_;

			Novice::DrawQuad((int)v1.x, (int)v1.y, (int)v2.x, (int)v2.y, (int)v4.x, (int)v4.y, (int)v3.x, (int)v3.y, srcX, srcY, pieceW_, pieceH_, handle2_, 0xFFFFFFFF);
		}
	}
}
