//2D Vector field visualizer by nmz (twitter: @stormoid)

/*
	There is already a shader here on shadertoy for 2d vector field viz, 
	but I found it to be hard to use so I decided to write my own.
*/
#define PI 3.14159265359

// Hash without Sine
// Creative Commons Attribution-ShareAlike 4.0 International Public License
// Created by David Hoskins.

// https://www.shadertoy.com/view/4djSRW
// Trying to find a Hash function that is the same on ALL systens
// and doesn't rely on trigonometry functions that change accuracy
// depending on GPU.
// New one on the left, sine function on the right.
// It appears to be the same speed, but I suppose that depends.

// * Note. It still goes wrong eventually!
// * Try full-screen paused to see details.

#define barWidth 0.002
#define slideValY 0.5
//(cos(iTime)+1.)/2.
#define slideValX 0.5
//(sin(iTime/2.)+1.)/2.
#define ITERATIONS 4


// *** Change these to suit your range of random numbers..

// *** Use this for integer stepped ranges, ie Value-Noise/Perlin noise functions.
#define HASHSCALE1 .1031
#define HASHSCALE3 vec3(.1031, .1030, .0973)
#define HASHSCALE4 vec4(.1031, .1030, .0973, .1099)

// For smaller input rangers like audio tick or 0-1 UVs use these...
//#define HASHSCALE1 443.8975
//#define HASHSCALE3 vec3(443.897, 441.423, 437.195)
//#define HASHSCALE4 vec3(443.897, 441.423, 437.195, 444.129)



//----------------------------------------------------------------------------------------
//  1 out, 1 in...
float hash11(float p)
{
		vec3 p3  = fract(vec3(p) * HASHSCALE1);
	p3 += dot(p3, p3.yzx + 19.19);
	return fract((p3.x + p3.y) * p3.z);
}

//----------------------------------------------------------------------------------------
//  1 out, 2 in...
float hash12(vec2 p)
{
		vec3 p3  = fract(vec3(p.xyx) * HASHSCALE1);
	p3 += dot(p3, p3.yzx + 19.19);
	return fract((p3.x + p3.y) * p3.z);
}

//----------------------------------------------------------------------------------------
//  1 out, 3 in...
float hash13(vec3 p3)
{
		p3  = fract(p3 * HASHSCALE1);
	p3 += dot(p3, p3.yzx + 19.19);
	return fract((p3.x + p3.y) * p3.z);
}

//----------------------------------------------------------------------------------------
//  2 out, 1 in...
vec2 hash21(float p)
{
		vec3 p3 = fract(vec3(p) * HASHSCALE3);
		p3 += dot(p3, p3.yzx + 19.19);
	return fract((p3.xx+p3.yz)*p3.zy);

}

//----------------------------------------------------------------------------------------
///  2 out, 2 in...
vec2 hash22(vec2 p)
{
		vec3 p3 = fract(vec3(p.xyx) * HASHSCALE3);
	p3 += dot(p3, p3.yzx+19.19);
	return fract((p3.xx+p3.yz)*p3.zy);

}

//----------------------------------------------------------------------------------------
///  2 out, 3 in...
vec2 hash23(vec3 p3)
{
		p3 = fract(p3 * HASHSCALE3);
	p3 += dot(p3, p3.yzx+19.19);
	return fract((p3.xx+p3.yz)*p3.zy);
}

//----------------------------------------------------------------------------------------
//  3 out, 1 in...
vec3 hash31(float p)
{
   vec3 p3 = fract(vec3(p) * HASHSCALE3);
   p3 += dot(p3, p3.yzx+19.19);
   return fract((p3.xxy+p3.yzz)*p3.zyx);
}


//----------------------------------------------------------------------------------------
///  3 out, 2 in...
vec3 hash32(vec2 p)
{
		vec3 p3 = fract(vec3(p.xyx) * HASHSCALE3);
	p3 += dot(p3, p3.yxz+19.19);
	return fract((p3.xxy+p3.yzz)*p3.zyx);
}

//----------------------------------------------------------------------------------------
///  3 out, 3 in...
vec3 hash33(vec3 p3)
{
		p3 = fract(p3 * HASHSCALE3);
	p3 += dot(p3, p3.yxz+19.19);
	return fract((p3.xxy + p3.yxx)*p3.zyx);

}

//----------------------------------------------------------------------------------------
// 4 out, 1 in...
vec4 hash41(float p)
{
		vec4 p4 = fract(vec4(p) * HASHSCALE4);
	p4 += dot(p4, p4.wzxy+19.19);
	return fract((p4.xxyz+p4.yzzw)*p4.zywx);
	
}

//----------------------------------------------------------------------------------------
// 4 out, 2 in...
vec4 hash42(vec2 p)
{
		vec4 p4 = fract(vec4(p.xyxy) * HASHSCALE4);
	p4 += dot(p4, p4.wzxy+19.19);
	return fract((p4.xxyz+p4.yzzw)*p4.zywx);

}

//----------------------------------------------------------------------------------------
// 4 out, 3 in...
vec4 hash43(vec3 p)
{
		vec4 p4 = fract(vec4(p.xyzx)  * HASHSCALE4);
	p4 += dot(p4, p4.wzxy+19.19);
	return fract((p4.xxyz+p4.yzzw)*p4.zywx);
}

//----------------------------------------------------------------------------------------
// 4 out, 4 in...
vec4 hash44(vec4 p4)
{
		p4 = fract(p4  * HASHSCALE4);
	p4 += dot(p4, p4.wzxy+19.19);
	return fract((p4.xxyz+p4.yzzw)*p4.zywx);
}

#define time iTime

const float arrow_density = 4.5;
const float arrow_length = .45;

const int iterationTime1 = 20;
const int iterationTime2 = 20;
const int vector_field_mode = 0;
const float scale = 6.;

const float velocity_x = 0.1;
const float velocity_y = 0.2;

const float mode_2_speed = 0.01;
const float mode_1_detail = 200.;
const float mode_1_twist = 10.;

const bool isArraw = false;

const vec3 luma = vec3(0.2126, 0.7152, 0.0722);


float f(in vec2 p)
{
    return sin(p.x+sin(p.y+time*velocity_x)) * sin(p.y*p.x*0.1+time*velocity_y);
}


struct Field {
    vec2 vel;
    vec2 pos;
};

//---------------Field to visualize defined here-----------------

Field field(in vec2 p,in int mode)
{
    float _time = time * 0.001;
    float timeDecrease = 100.0;
    Field field;
    if(mode == 0){
    	vec2 ep = vec2(0.05,0.);
        vec2 rz= vec2(0);
        //# centered grid sampling
        for( int i=0; i<iterationTime1; i++ )
        {
            float t0 = f(p);
            float t1 = f(p + ep.xy);
            float t2 = f(p + ep.yx);
            vec2 g = vec2((t1-t0), (t2-t0))/ep.xx;
            vec2 t = vec2(-g.y,g.x);
            
            //# need update 'p' for next iteration,but give it some change.
            p += (mode_1_twist*0.01)*t + g*(1./mode_1_detail);
            p.x = p.x + sin(_time*mode_2_speed/10.)/timeDecrease;
            p.y = p.y + cos(_time*mode_2_speed/10.)/timeDecrease;
            rz= g; 
        }
        field.vel = rz;
        return field;
    }
    
    if(mode == 1){
        vec2 ep = vec2(0.05,0.);
        vec2 rz= vec2(0);
        //# centered grid sampling
        for( int i=0; i<iterationTime1; i++ )
        {
            float t0 = f(p);
            float t1 = f(p + ep.xy);
            float t2 = f(p + ep.yx);
            vec2 g = vec2((t1-t0), (t2-t0))/ep.xx;
            vec2 t = vec2(-g.y,g.x);

            //# need update 'p' for next iteration,but give it some change.
            p += (mode_1_twist*0.01)*t + g*(1./mode_1_detail);
            p.x = p.x + sin(_time*mode_2_speed/10.)/timeDecrease;
            p.y = p.y + cos(_time*mode_2_speed/10.)/timeDecrease;
            rz= g;
        }
        
        field.vel = rz;
        // add curved effect into curved mesh
        for(int i=1; i<iterationTime2; i++){
            //# try comment these 2 lines,will give more edge effect
            p.x+=0.3/float(i)*sin(float(i)*3.*p.y+_time*mode_2_speed) + 0.5;
            p.y+=0.3/float(i)*cos(float(i)*3.*p.x + _time*mode_2_speed) + 0.5;
        }
        field.pos = p;
        return field;
    }
    
    return field;
}
//---------------------------------------------------------------

float segm(in vec2 p, in vec2 a, in vec2 b) //from iq
{
	vec2 pa = p - a;
	vec2 ba = b - a;
	float h = clamp(dot(pa,ba)/dot(ba,ba), 0., 1.);
	return length(pa - ba*h)*20.*arrow_density;
}

float fieldviz(in vec2 p,in int mode)
{
    vec2 ip = floor(p*arrow_density)/arrow_density + .5/arrow_density;   
    vec2 t = field(ip,mode).vel;
    float m = min(0.1,pow(length(t),0.5)*(arrow_length/arrow_density));
    vec2 b = normalize(t)*m;
    float rz = segm(p, ip, ip+b);
    vec2 prp = (vec2(-b.y,b.x));
    rz = min(rz,segm(p, ip+b, ip+b*0.65+prp*0.3));
    return clamp(min(rz,segm(p, ip+b, ip+b*0.65-prp*0.3)),0.,1.);
}


vec3 getRGB(in Field fld,in int mode){

    if(mode == 0){
        vec2 p = fld.vel;
        vec3 origCol = vec3(p * 0.5 + 0.5, 1.5);
        return origCol;
    }
    
    if(mode == 1){
        vec2 p = fld.pos;
        float r=cos(p.x+p.y+1.)*.5+.5;
        float g=sin(p.x+p.y+1.)*.5+.5;
        float b=(sin(p.x+p.y)+cos(p.x+p.y))*.3+.5;
        vec3 col = sin(vec3(-.3,0.1,0.5)+p.x-p.y)*0.65+0.35;
        // gray scale 
        col = vec3(r*0.3+g*0.59+b*0.11);
        // apply blue
        col.b += 0.2;
        return col;
    }

}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 p = fragCoord.xy / iResolution.xy-0.5 ;
	p.x *= iResolution.x/iResolution.y;
    p *= scale;
    
    vec2 uv = fragCoord.xy / iResolution.xy;
    vec3 col;
    float fviz;
    
    int vector_mode = 1;
    if(uv.x > slideValX){
        vector_mode = 1;
    }

    //vec2 fld = field(p,vector_mode).vel;
    //col = sin(vec3(-.3,0.1,0.5)+fld.x-fld.y)*0.65+0.35;
    Field fld = field(p,vector_mode);
    col = getRGB(fld,vector_mode) * 0.85;
    fviz = fieldviz(p,vector_mode);
    
    if(isArraw == true){
        #if 1
        col = max(col, 1.-fviz*vec3(1.));
        #else
        if (dot(luma,col) < 0.5)
            col = max(col, 1.-fviz*vec3(1.));
        else
            col = min(col, fviz*vec3(1.));
        #endif
    }
    
	fragColor = vec4(col,1.0);
}
