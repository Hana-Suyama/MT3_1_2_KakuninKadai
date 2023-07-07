#include <Novice.h>
#include <MyMath.h>
#define _USE_MATH_DEFINES
#include <math.h>

const char kWindowTitle[] = "LE2A_08_スヤマハナ";

float kWindowWidth = 1280;
float kWindowHeight = 720;

//関数
float DEGtoRAD(float degree) {
	float result;
	result = degree * ((float)M_PI / 180);
	return result;
}

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f;//Gridの半分の幅
	const uint32_t kSubdivision = 10;//分割数
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision);//1つ分の長さ
	Vector3 StartPos{0, 0, 0};

	//奥から手前への線を順々に引いていく
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		Vector3 worldStart{};
		Vector3 worldEnd{};
		worldStart.z = StartPos.z + kGridHalfWidth;
		worldEnd.z = worldStart.z - kGridHalfWidth * 2.0f;
		worldStart.x = kGridHalfWidth - (xIndex * kGridEvery);
		worldEnd.x = worldStart.x;

		Vector3 StartVertex{};
		Vector3 EndVertex{};

		Vector3 StartndcVertex = Transform(worldStart, viewProjectionMatrix);
		StartVertex = Transform(StartndcVertex, viewportMatrix);
		Vector3 EndndcVertex = Transform(worldEnd, viewProjectionMatrix);
		EndVertex = Transform(EndndcVertex, viewportMatrix);

		if (xIndex == 5) {
			Novice::DrawLine((int)StartVertex.x, (int)StartVertex.y, (int)EndVertex.x, (int)EndVertex.y, BLACK);
		} else {
			Novice::DrawLine((int)StartVertex.x, (int)StartVertex.y, (int)EndVertex.x, (int)EndVertex.y, 0xAAAAAAFF);
		}
	}

	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		Vector3 worldStart{};
		Vector3 worldEnd{};
		worldStart.x = kGridHalfWidth;
		worldEnd.x = worldStart.x - kGridHalfWidth * 2.0f;
		worldStart.z = kGridHalfWidth - (zIndex * kGridEvery);
		worldEnd.z = worldStart.z;

		Vector3 StartVertex{};
		Vector3 EndVertex{};

		Vector3 StartndcVertex = Transform(worldStart, viewProjectionMatrix);
		StartVertex = Transform(StartndcVertex, viewportMatrix);
		Vector3 EndndcVertex = Transform(worldEnd, viewProjectionMatrix);
		EndVertex = Transform(EndndcVertex, viewportMatrix);

		if (zIndex == 5) {
			Novice::DrawLine((int)StartVertex.x, (int)StartVertex.y, (int)EndVertex.x, (int)EndVertex.y, BLACK);
		} else {
			Novice::DrawLine((int)StartVertex.x, (int)StartVertex.y, (int)EndVertex.x, (int)EndVertex.y, 0xAAAAAAFF);
		}
	}
}

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const uint32_t kSubdivision = 26;//分割数
	const float kLonEvery = DEGtoRAD(360 / kSubdivision);//経度分割1つ分の角度
	const float kLatEvery = DEGtoRAD(360 / kSubdivision);//緯度分割1つ分の角度
	//緯度の方向に分割 0 ~ 2π
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat =  - (float)M_PI / 2.0f + kLatEvery * latIndex;//現在の緯度
		//経度の方向に分割 0 ~ 2π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;//現在の経度

			//world座標系でのa, b, cを求める
			Vector3 a{}, b{}, c{};
			a.x = sphere.center.x + (cosf(lat) * cosf(lon) * sphere.radius);
			a.y = sphere.center.y + (sinf(lat) * sphere.radius);
			a.z = sphere.center.z + (cosf(lat) * sinf(lon) * sphere.radius);

			b.x = sphere.center.x + (cosf(lat + kLatEvery) * cosf(lon) * sphere.radius);
			b.y = sphere.center.y + (sinf(lat + kLatEvery) * sphere.radius);
			b.z = sphere.center.z + (cosf(lat + kLatEvery) * sinf(lon) * sphere.radius);

			c.x = sphere.center.x + (cosf(lat) * cosf(lon + kLonEvery) * sphere.radius);
			c.y = sphere.center.y + (sinf(lat) * sphere.radius);
			c.z = sphere.center.z + (cosf(lat) * sinf(lon + kLonEvery) * sphere.radius);

			// a,b,cをScreen座標系まで変換
			Vector3 VertexA{};
			Vector3 ndcVertexA = Transform(a, viewProjectionMatrix);
			VertexA = Transform(ndcVertexA, viewportMatrix);

			Vector3 VertexB{};
			Vector3 ndcVertexB = Transform(b, viewProjectionMatrix);
			VertexB = Transform(ndcVertexB, viewportMatrix);

			Vector3 VertexC{};
			Vector3 ndcVertexC = Transform(c, viewProjectionMatrix);
			VertexC = Transform(ndcVertexC, viewportMatrix);

			//ab,bcで線を引く
			Novice::DrawLine((int)VertexA.x, (int)VertexA.y, (int)VertexB.x, (int)VertexB.y, color);
			Novice::DrawLine((int)VertexA.x, (int)VertexA.y, (int)VertexC.x, (int)VertexC.y, color);
		}
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	Vector3 cameraTranslate{ 0.0f, 1.9f, -6.49f };
	Vector3 cameraRotate{ 0.26f, 0.0f, 0.0f };

	Vector3 rotate{};
	Vector3 translate{};

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///
		//各種行列の計算
		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, rotate, translate);
		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		DrawGrid(worldViewProjectionMatrix, viewportMatrix);
		DrawSphere({ {0,0,0},1 }, worldViewProjectionMatrix, viewportMatrix, BLACK);
		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
