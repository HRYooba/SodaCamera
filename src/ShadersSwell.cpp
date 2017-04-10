//
//  ShadersSwell.cpp
//  waterEffect
//
//  Created by 大場洋哉 on 2015/07/06.
//
//

#include "ShadersSwell.h"

void ShadersSwell::setup(ofFbo *buffer_){
    targetBuffer = buffer_;
    buffer_size.set(buffer_->getWidth(), buffer_->getHeight());
    ShadingBuffer.allocate(buffer_size.x,buffer_size.y);
}

void ShadersSwell::setFbo(ofFbo *buffer_){
    targetBuffer = buffer_;
    buffer_size.set(buffer_->getWidth(), buffer_->getHeight());
    ShadingBuffer.allocate(buffer_size.x,buffer_size.y);
}

void ShadersSwell::setFx(bool enabled){
    bShading = enabled;
}

void ShadersSwell::toggleFx(){
    bShading ^= true;
}

bool ShadersSwell::getFx(){
    return bShading;
}

void ShadersSwell::generateFx(){
    if (targetBuffer == NULL){
        ofLog(OF_LOG_WARNING, "ofxFboFX --- Fbo is not allocated.");
        return;
    }
    
    static int step = ofRandom(4,15);
    float v[2];
    v[0] = ofRandom(3);v[1] = ofRandom(3);
    if (ofGetFrameNum() % step == 0){
        step = ofRandom(10,30);
        ShadeVal[0] = ofRandom(100);
        ShadeVal[2] = ofRandom(100);
        ShadeVal[3] = ofRandom(100);
    }
    
    ofSetColor(255);
    glClearColor(0, 0, 0, 0.0);
    
    if (bShading){
        shader.begin();
        shader.setUniformTexture	("image"		,*targetBuffer,0);
        shader.setUniform1i		("trueWidth"	,buffer_size.x);
        shader.setUniform1i		("trueHeight"	,buffer_size.y);
        shader.setUniform1f		("rand"			,ofRandom(1));
        shader.setUniform1f		("mouseX"		,ofGetMouseX());
        shader.setUniform1f		("mouseY"		,ofGetMouseY());
        shader.setUniform1f		("val1"			,ShadeVal[0]);
        shader.setUniform1f		("val2"			,ShadeVal[1]);
        shader.setUniform1f		("val3"			,ShadeVal[2]);
        shader.setUniform1f		("val4"			,ShadeVal[3]);
        shader.setUniform1f		("timer"		,ofGetElapsedTimef());
        shader.setUniform1f     ("width"        ,ofGetWidth());
        shader.setUniform1f     ("height"       ,ofGetHeight());
        shader.setUniform2fv		("blur_vec"		,v);
        
        ShadingBuffer.begin();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ofRect(0, 0, buffer_size.x, buffer_size.y);
        ShadingBuffer.end();
        shader.end();
        
        targetBuffer->begin();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ShadingBuffer.draw(0, 0,buffer_size.x,buffer_size.y);
        targetBuffer->end();
    }
}

void ShadersSwell::mySetup() {
    int width = ofGetWidth();
    int height = ofGetHeight();
    
    img.allocate(width, height*2, OF_IMAGE_GRAYSCALE);
    imgPixel = img.getPixels();
    for (int y=0; y<height*2; y++) {
        for (int x=0; x<width; x++) {
            float scaleX = width / 130.0;
            float scaleY = height*2 / 130.0;
            float noiseX = ofMap(x, 0, width, 0, scaleX);
            float noiseY = ofMap(y, 0, height*2, 0, scaleY);
            int noiseVal = ofNoise(noiseX, noiseY) * 60;
            imgPixel[y * width + x] = noiseVal;
        }
    }
    img.update();
    
    myFbo.allocate(640, 480*2);
    setup(&myFbo);
    setFx(true);
}

void ShadersSwell::myUpdate() {
    myFbo.begin();
    ofClear(0, 0, 0, 255);
    ofSetColor(255);
    img.draw(0, 0);
    myFbo.end();
}

void ShadersSwell::myDraw(float x, float y) {
    ofPushMatrix();
    generateFx();
    ofTranslate(x, y);
    ofSetColor(255);
    myFbo.draw(0, 0);
    ofPopMatrix();
}

void ShadersSwell::camSetup() {
    camFbo.allocate(640, 480);
    setup(&camFbo);
    setFx(true);
}

void ShadersSwell::camUpdate(float x, float y, ofVideoGrabber camera) {
    camFbo.allocate(640, 480-y);
    camFbo.begin();
    ofClear(0, 0, 0, 255);
    ofSetColor(255);
    camera.draw(0, -y);
    camFbo.end();
}

void ShadersSwell::camDraw(float x, float y) {
    ofPushMatrix();
    generateFx();
    ofTranslate(x, y);
    ofSetColor(255);
    camFbo.draw(0, 0);
    ofPopMatrix();
}