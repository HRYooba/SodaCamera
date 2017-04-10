#pragma once

#include "ofMain.h"

class VectorField {
	
public:
	
	//---------- プロパティー  -------------------------
	
	//ベクトル場のグリッドの数を指定 (例：60x40)
	int fieldWidth;
	int fieldHeight;
	//ベクトルの数 (= fieldWidth x fieldHeight)
	int fieldSize;
	//ベクトル場が描画される画面の大きさ
	int externalWidth;
	int externalHeight;
	//ベクトルの配列
	vector <ofVec2f> field;
	

	//---------- メソッド  -----------------------------
	
	//コンストラクタとデストラクタ
	VectorField();
	virtual ~VectorField();
	//ベクトル場の初期化
	void setupField(int innerW, int innerH, int outerW, int outerH);
	//消去
	void clear();
	//徐々にベクトル場に係る力が弱まるようにする
	void fadeField(float fadeAmount);
	//全てのベクトルをランダムに生成
	void randomizeField(float scale);
	//ベクトル場の様子を描画
	void draw();
	//座標を指定して、その地点でのベクトル場から受ける力を算出
	ofVec2f	getForceFromPos(float xpos, float ypos);

	//外向きの力を加える (噴出)
	void addOutwardCircle(float x, float y, float radius, float strength);	
	
protected:
private:
};