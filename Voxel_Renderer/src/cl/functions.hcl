#pragma once


#define semn(x)((x>=0) ? 1 : -1)



void swap(float a, float b){
    float x = b;
    b=a;
    a=x;}

float vec3Distance(float3 vec1, float3 vec2) {
  float d[] = {fabs(vec1.x - vec2.x), 
              fabs(vec1.y - vec2.y),
              fabs(vec1.z - vec2.z)};

  if (d[0] < d[1])
    swap(d[0], d[1]);
  if (d[0] < d[2])
    swap(d[0], d[2]);
    
  return d[0] * sqrt(1.0 + d[1] / d[0] + d[2] / d[0]);
}


bool inBounds(float3 v, float n) {
  return 0<=v.x&&v.x<=n &&0<=v.y&&v.y<=n&&0<=v.z&&v.z<n;
}

bool inBoundsD(float3 v, float n, float d) {
  return d<=v.x&&v.x<=n &&d<=v.y&&v.y<=n&&d<=v.z&&v.z<n;
}

uchar3 Gradient(uchar3 c1, uchar3 c2, int q){
  return (uchar3){
    min(c2.x,c1.x)+((c2.x-c1.x)/255)*q,
    min(c2.y,c1.y)+((c2.y-c1.y)/255)*q,
    min(c2.z,c1.z)+((c2.z-c1.z)/255)*q
  };
}


uchar3 Color(float3 u){
  uchar3 v;
  if(u.x<0)v.x = 0;
  else if(u.x>255)v.x = 255;
  else v.x = (uchar)u.x;

  if(u.y<0)v.y = 0;
  else if(u.y>255)v.y = 255;
  else v.y = (uchar)u.y;

  if(u.z<0)v.z = 0;
  else if(u.z>255)v.z = 255;
  else v.z = (uchar)u.z;

  return v;
}

float dot_clamped(float3 v1, float3 v2){
  return dot(v1, v2) * (int)(dot(v1, v2)>=0);
}


float3 multiply_vec(float3 v1, float3 v2){
  return (float3)(v1.x*v2.x, v1.y*v2.y, v1.z*v2.z);
}


float3 Clampf3(float3 v){
    return (float3)(v.x * (float)(v.x>0), v.y * (float)(v.y>0), v.z * (float)(v.z>0));
}