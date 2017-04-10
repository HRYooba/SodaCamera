#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"
#include "vectorField.h"
#include "ShadersSwell.h"

#define CAMERA_WIDTH 1920
#define CAMERA_HEIGHT 1080

class ofApp : public ofBaseApp {
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    int                                     width; //カメラのwidth
    int                                     height; //カメラのheight
    float                                   border; //水面基準となるY座標
    float                                   UpDown; //水面の UP DOWN
    
    unsigned char                           pixelBuffer[CAMERA_WIDTH * CAMERA_HEIGHT * 3]; //1フレーム前のpixel情報を保存するところ
    
    ofxBox2d                                box2d; //泡の物理Box2dの世界
    ofxBox2d                                water; //水面のためのBox2dの世界
    ofxBox2d                                world; //タピオカ
    
    vector <ofPtr<ofxBox2dCircle> >         ball;
    vector <ofPtr<ofxBox2dCircle> >         circles; //全ての泡のオブジェクトを統括しいるベクトル
    vector <ofPtr<ofxBox2dCircle> >         borderCircles; //全ての水面の円を統括してるベクトル
    vector <ofSpherePrimitive> sphere;
    
    ofImage                                 imgBubble; //泡の画
    ofPolyline                              groundLine; //水面の線
    ofVideoGrabber                          camera; //カメラ映像
    
    ShadersSwell                            myShader; //shaderの適応(水中のゆらゆら再現)
    ShadersSwell                            camShader; //shaderの適応(水中のゆらゆら再現)
    VectorField                             VF; //ベクトル場
};
