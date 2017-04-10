uniform sampler2DRect image;
uniform int imgWidth,imgHeight;
uniform float rand;
uniform int flags;
uniform vec2 blur_vec;
uniform float timer;
uniform float width;
uniform float height;
int flgs;
float pix_w,pix_h;
varying vec3 pos;

void main (void)
{
    pix_w = 1.0;
    pix_h = 1.0;
    flgs = flags;
    
    float swell = 0.0;
    for (float i=0.0; i<10.0; i++) {
        swell += sin(pos.y*sin(i)*0.07+timer*(i+10.0))*(1.2);
    }
    
    float a = ( 1.0 - exp( -pos.y * 0.025 ) );
    swell *= a;

    vec2 texCoord = vec2(pos.x,pos.y);
    vec4 col = texture2DRect(image,texCoord + vec2(swell,0));
    vec4 colPre = texture2DRect(image, texCoord);
	
    gl_FragColor.rgba = vec4((col.rgb+colPre.rgb)/2.0, a );
}
