//
//  ShadersSwell.h
//  waterEffect
//
//  Created by 大場洋哉 on 2015/07/06.
//
//

#ifndef __waterEffect__ShadersSwell__
#define __waterEffect__ShadersSwell__

#include "ofMain.h"

class ShadersSwell{
public:
    
    ShadersSwell(){
        targetBuffer = NULL;
        shader.load("Shaders/swell");
    }
    
    void setup(ofFbo* buffer_);
    void setFbo(ofFbo* buffer_);
    void setFx(bool enabled);
    void toggleFx();
    bool getFx();
    void generateFx();
    void mySetup();
    void myUpdate();
    void myDraw(float x, float y);
    void camSetup();
    void camUpdate(float x, float y, ofVideoGrabber camera);
    void camDraw(float x, float y);
    
protected:
    bool		bShading;
    ofShader	shader;
    ofFbo*		targetBuffer;
    ofFbo		ShadingBuffer;
    ofPoint		buffer_size;
    float ShadeVal[4];
    
    ofImage			img;
    ofPixels        imgPixel;
    ofFbo			myFbo;
    ofFbo           camFbo;
};


#endif /* defined(__waterEffect__ShadersSwell__) */
