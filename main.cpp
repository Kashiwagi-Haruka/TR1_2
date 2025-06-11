#include "WaterController.h"
#include <Novice.h>

// ウィンドウタイトル
const char kWindowTitle[] = "水流ストリップ魔法サンプル";

// エントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	Novice::Initialize(kWindowTitle, 1280, 720);

	char keys[256] = {0};
	char preKeys[256] = {0};
	int MousePosX = 0, MousePosY = 0;
	int PrePressMouse = 0;

	WaterController water;
	water.Initialize();

	while (Novice::ProcessMessage() == 0) {
		Novice::BeginFrame();
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		// マウス座標取得
		Novice::GetMousePosition(&MousePosX, &MousePosY);

		// 押し始め判定
		bool justPressed = (Novice::IsPressMouse(0) && PrePressMouse == 0);

		// チャージ処理
		if (Novice::IsPressMouse(0)) {
			water.StartCharge({(float)MousePosX, (float)MousePosY}, justPressed);
		}
		// 発射処理
		if (!Novice::IsPressMouse(0) && PrePressMouse == 1) {
			water.Fire();
		}

		water.Update();
		water.Draw();

		PrePressMouse = Novice::IsPressMouse(0);

		Novice::EndFrame();

		// ESCで終了
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}
	Novice::Finalize();
	return 0;
}
