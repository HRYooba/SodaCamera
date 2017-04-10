#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ///　width height border UpDownの設定
    width       = 640;
    height      = 480;
    border      = height/2-150;
    UpDown      = 0;
    
    /// imageの設定
    imgBubble.loadImage("bubble.png");
    
    ///　フレームレートと背景色の設定
    ofSetFrameRate(60);
    ofBackground(0);
    
    /// 水面の模様の画像の初期設定
    myShader.mySetup();
    camShader.camSetup();
    
    ///　ビデオの初期設定
    camera.setDeviceID(0);
    camera.initGrabber(width, height);
    
    ///  box2d
    box2d.init();
    box2d.setFPS(60.0);
    
    ///  water
    water.init();
    water.setFPS(60.0);
    water.setGravity(0, 0);
    
    ///  World
    world.init();
    world.setFPS(60.0);
    world.setGravity(0, 10);
    world.createBounds(0, 0, width, height);
    for (int i=0; i<40; i++) {
        shared_ptr<ofxBox2dCircle> rect = shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle);
        rect.get()->setPhysics(1, 0.5, 1);
        float size = ofRandom(10, 15);
        rect.get()->setup(world.getWorld(), ofRandom(0, width), border, size);
        ball.push_back(rect);
        ofSpherePrimitive sp;
        sp.set(10, 7);
        sphere.push_back(sp);
    }
    
    
    /// ベクトル場の初期化
    VF.setupField(64, 48, width, height);
    
    /// 水面の初期設定
    for(int i=0; i<width; i++) {
        groundLine.addVertex(i, border);
    }
    // 水面の基準となる境界面に無数にある円
    for (int i=0; i<60; i++) {
        shared_ptr<ofxBox2dCircle> circle = shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle);
        circle.get()->setPhysics(1, 1, 1);
        circle.get()->setup(water.getWorld(), i*21-width/2, border, 2);
        borderCircles.push_back(circle);
    }
    
}

//--------------------------------------------------------------
void ofApp::update(){
    /// 水面の更新
    //UpDown = 150*sin(ofGetFrameNum()/180.0*PI);
    for (int i=1; i<borderCircles.size()-1; i++) {
        ofVec2f pos = borderCircles[i].get()->getPosition();
        ofVec2f frc = VF.getForceFromPos(pos.x, pos.y);
        float dist = (border - pos.y)+UpDown;
        //波をだんだん弱くしていく
        borderCircles[i].get()->setVelocity(borderCircles[i].get()->getVelocity()*0.99);
        
        //ベクトル場に力があれば水面を揺らす
        if (frc.length() > 0) {
            borderCircles[i].get()->addForce(ofVec2f(0, frc.y), 15);
        }
        //水面の反発力
        borderCircles[i].get()->addForce(ofVec2f(0, dist), 0.1);
        //隣の水面に力を加える
        ofVec2f pos1 = borderCircles[i-1].get()->getPosition();
        float force1 = pos.y - pos1.y;
        borderCircles[i-1].get()->addForce(ofVec2f(0, force1), 0.3);
        //隣の水面に力を加える
        ofVec2f pos2 = borderCircles[i+1].get()->getPosition();
        float force2 = pos.y - pos2.y;
        borderCircles[i+1].get()->addForce(ofVec2f(0, force2), 0.3);
    }
    //端の固定
    borderCircles[0].get()->setPosition(-width/2, border+UpDown);
    borderCircles[borderCircles.size()-1].get()->setPosition(width+width/2, border+UpDown);
    
    /// 泡の処理
    // 現在のカメラのpixelデータを取得
    ofPixels pixels = camera.getPixels();
    if (ofGetFrameNum() > 30) {
        for (int y=0; y<height; y++) {
            for (int x=0; x<width; x++) {
                // pixelのindexの設定
                int index = (y*width + x) * 3;
                // 前フレームとのRGBの差分を計算
                int R = int(pixels[index]) - int(pixelBuffer[index]);
                int G = int(pixels[index+1]) - int(pixelBuffer[index+1]);
                int B = int(pixels[index+2]) - int(pixelBuffer[index+2]);
                // 前フレームとの差分が一定値を超えたらベクトル場に力を発生
                if (R*R + G*G + B*B >= 70*70) {
                    VF.addOutwardCircle(x, y, 20, 0.08);
                    // 泡の作成
                    if(x%30 == 0 && y%30 == 0 && circles.size() < 300) {
                        float size = ofRandom(1, 5);
                        ofPtr<ofxBox2dCircle> circle = ofPtr<ofxBox2dCircle>(new ofxBox2dCircle);
                        circle.get()->setPhysics(1, 0.5, 0.1);
                        circle.get()->setup(box2d.getWorld(), x+ofRandom(-30, 30), y+ofRandom(-30, 30), size);
                        circles.push_back(circle);
                    }
                }
            }
        }
    }
    // 最後に今のフレームのpixelをbufferに保存
    for (int i=0; i<height*width*3; i++) {
        pixelBuffer[i] = pixels[i];
    }
    
    //　下から泡から出現
    if (ofGetFrameNum()%10 == 0) {
        ofPtr<ofxBox2dCircle> circle = ofPtr<ofxBox2dCircle>(new ofxBox2dCircle);
        circle.get()->setPhysics(1, 0.5, 0.1);
        circle.get()->setup(box2d.getWorld(), ofRandom(0, width), height-10 , ofRandom(1, 5));
        circles.push_back(circle);
    }
    
    // 泡の処理(人間と泡の判定, 浮力, 泡の削除)
    for (int i=0; i<circles.size(); i++) {
        
        //　浮力
        float size = circles[i].get()->getRadius();
        float frequency = (7.0/size)*sin(ofGetFrameNum()/(float)size*PI);
        ofVec2f buoyancy = ofVec2f(circles[i].get()->getVelocity().x+frequency, -size*size);
        circles[i].get()->setVelocity(buoyancy);
        
        // 力を加える
        ofVec2f posCircle = circles[i].get()->getPosition();
        ofVec2f frc = VF.getForceFromPos(posCircle.x, posCircle.y);
        //ベクトル場に力があればボールに力を当た得る
        if (frc.length() > 0) {
            circles[i].get()->addForce(ofVec2f(frc.x, frc.y), 1);
        }
        
        // 泡の削除のため画面外に泡を飛ばす
        ofVec2f pos = circles[i].get()->getPosition();
        for (int j=0; j<borderCircles.size(); j++) {
            ofVec2f posBorder = borderCircles[j].get()->getPosition();
            if (pos.y < posBorder.y+size*2 && ofDist(posBorder.x, 0, pos.x, 0) <= 21) {
                circles[i].get()->setPosition(pos.x, -100);
            }
        }
    }
    ofRemove(circles, ofxBox2dBaseShape::shouldRemoveOffScreen);
    
    /*for (int i=0; i<40; i++) {
        ofVec2f frc = VF.getForceFromPos(ball[i].get()->getPosition().x, ball[i].get()->getPosition().y);
        if (frc.length() > 0) {
            ball[i].get()->addForce(ofVec2f(frc.x, frc.y), 100);
        }
    }*/
    
    /// ベクトル場の力を消していく
    VF.fadeField(0.8f);
    /// 物理エンジンの世界の更新
    box2d.update();
    water.update();
    world.update();
    /// カメラの更新
    camera.update();
    /// shaderの更新
    myShader.myUpdate();
    camShader.camUpdate(0, border+UpDown, camera);
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofPushMatrix();
    
    ///　反転処理

    ofScale(-1*ofGetWidth()/640.0, 1*ofGetWidth()/480.0);
    ofTranslate(-camera.getWidth(), 0);
    
    /*
    // デバック用
    ofScale(-1, 1);
    ofTranslate(-ofGetWidth()+(ofGetWidth()-camera.width)/2, 100);
*/
     
    ///　カメラの描画
    ofSetColor(255);
    camera.draw(0, 0);
    camShader.camDraw(0, border+UpDown);
    
    /// 透過処理開始（後ろの色と合体させるモード）
    ofEnableBlendMode(OF_BLENDMODE_ADD);
    
    /// 水面と水中の描写
    ofSetLineWidth(0);
    ofSetColor(0, 100, 155, 200);
    ofBeginShape();
    ofVertex(width, border+UpDown);
    ofVertex(width, height+UpDown+1000);
    ofVertex(0, height+UpDown+1000);
    ofVertex(0, border+UpDown);
    for (int i=0; i<borderCircles.size(); i++) {
        ofVec2f pos = borderCircles[i].get()->getPosition();
        ofCurveVertex(pos.x, pos.y);
    }
    ofEndShape();
    
    ofSetColor(255);
    myShader.myDraw(0, border+UpDown);
    
    ///　泡の描写
    for (int i=0; i<circles.size(); i++) {
        ofVec2f pos = circles[i].get()->getPosition();
        float size = circles[i].get()->getRadius();
        imgBubble.draw(pos, size*2, size*2);
    }
    
    ///　ベクトル場のデバック用の描画
     /*ofSetLineWidth(2);
     ofSetColor(0, 200, 200, 200);
     VF.draw();
     */
    
    
    ///　水面のデバック用の描画
//    for(int i=0; i<borderCircles.size(); i++) {
//        ofFill();
//        ofSetHexColor(0x01b1f2);
//        borderCircles[i].get()->draw();
//    }
   /*
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    for (int i=0; i<40; i++) {
        ofSetColor(50, 150);
        ofVec2f pos = ball[i].get()->getPosition();
//        ofCircle(pos, 10);
        sphere[i].setPosition(pos.x, pos.y, 0);
        sphere[i].drawWireframe();
//        sp®here[i].draw();
    }
    ofDisableBlendMode();*/
    
    ofDisableBlendMode();
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}

