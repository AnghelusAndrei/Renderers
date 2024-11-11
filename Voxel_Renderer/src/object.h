#pragma once

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mutex.h"
#include "SDL2/SDL_ttf.h"


#include <bits/stdc++.h>
#include <strstream>
#include "vector.hpp"
#include "octree.h"
#include "config.h"

#define semn(x)((bool)(x>=0)*2-1)
bool inBounds(cl_float3 v, float n) {
    return 0<=v.x&&v.x<=n &&0<=v.y&&v.y<=n&&0<=v.z&&v.z<n;
}

class Object{
    public:
        Object(Vector position, Vector size, cl_uint3 color_, Octree &octree, cl_uint type);

        Vector Position;
        Vector Size;
        Vector Rotation = Vector(0,0,0);
        cl_uint3 color;
        SDL_Surface* texture = NULL;


        void GeneratePerlin3D(config_ d);
        void GeneratePerlin2D(config_ d);
        bool LoadObj(std::string sFilename, bool bHasTexture, bool bHasNormal);
        void MapNormals(int detail);

        void SetPosition(cl_uint3 p);
        void SetRotation(Vector r);
        void Remove();
    private:
        std::vector<cl_uint3> voxels;
        std::vector<cl_uint3> _data;
        Octree octree_;

        cl_uint type_;

        struct triangle{
            Vector p[3];
            Vector t[3];
            Vector normal[3];
            int vert_i[3];
        };

        float low;
        float high;

        bool CheckNormal(cl_uint3 p);
        void Raster(triangle tr, bool bHasTexture);
        void FillTriangle(triangle t, triangle translation, int view, int step);
        void Bresenham3D(cl_uint3 v1, cl_uint3 v2, triangle t, triangle translation);
        Uint32 getpixel(SDL_Surface *surface, int x, int y);
        float area3d(triangle t);
        void place_on_line(cl_uint3 v1, triangle t, triangle translation);

    private:

        struct mat4x4
        {
            float m[4][4] = { 0 };
        };

        Vector Vector_MultiplyMatrix(Vector &i, mat4x4 &m)
        {
            Vector v;
            v.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + i.w * m.m[3][0];
            v.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + i.w * m.m[3][1];
            v.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + i.w * m.m[3][2];
            v.w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + i.w * m.m[3][3];
            return v;
        }


        mat4x4 Matrix_MultiplyMatrix(mat4x4 &m1, mat4x4 &m2)
        {
            mat4x4 matrix;
            for (int c = 0; c < 4; c++)
                for (int r = 0; r < 4; r++)
                    matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
            return matrix;
        }

        mat4x4 Matrix_RotateX(float ang)
        {
            float fAngleRad = degToRad(ang);
            mat4x4 matrix;
            matrix.m[0][0] = 1;
            matrix.m[1][1] = cosf(fAngleRad);
            matrix.m[1][2] = sinf(fAngleRad);
            matrix.m[2][1] = -sinf(fAngleRad);
            matrix.m[2][2] = cosf(fAngleRad);
            matrix.m[3][3] = 1;
            return matrix;
        }


        mat4x4 Matrix_RotateY(float ang)
        {
            mat4x4 matrix;
            matrix.m[0][0] = cosf(degToRad(ang));
            matrix.m[1][1] = 1;
            matrix.m[0][2] = -sinf(degToRad(ang));
            matrix.m[2][0] = sinf(degToRad(ang));
            matrix.m[2][2] = cosf(degToRad(ang));
            matrix.m[3][3] = 1;
            return matrix;
        }


        mat4x4 Matrix_RotateZ(float ang)
        {
            float fAngleRad = degToRad(ang);
            mat4x4 matrix;
            matrix.m[0][0] = cosf(fAngleRad);
            matrix.m[0][1] = sinf(fAngleRad);
            matrix.m[1][0] = -sinf(fAngleRad);
            matrix.m[1][1] = cosf(fAngleRad);
            matrix.m[2][2] = 1;
            matrix.m[3][3] = 1;
            return matrix;
        }
};

Object::Object(Vector position, Vector size, cl_uint3 color_, Octree &octree, cl_uint type){
    Position = position;
    Size = size;
    octree_ = octree;
    color = color_;
    type_ = type;
}



//public


void Object::SetPosition(cl_uint3 p){
    if(p.x==Position.x && p.y==Position.y && p.z==Position.z)return;
    
    for(int i = 0; i < voxels.size(); i++){
        octree_.Remove({(cl_uint)(voxels[i].x), (cl_uint)(voxels[i].y), (cl_uint)(voxels[i].z)});
        voxels[i] = {(cl_uint)(voxels[i].x-Position.x+p.x), (cl_uint)(voxels[i].y-Position.y+p.y), (cl_uint)(voxels[i].z-Position.z+p.z)};
    }
    for(int i = 0; i < voxels.size(); i++){
        octree_.Insert({(cl_uint)(voxels[i].x-Position.x+p.x), (cl_uint)(voxels[i].y-Position.y+p.y), (cl_uint)(voxels[i].z-Position.z+p.z)}, _data[i], type_);
    }
    
    Position = Vector((float)p.x, (float)p.y, (float)p.z);
}

void Object::Remove(){
    for(int i = 0; i < voxels.size(); i++){
        leaf vox = octree_.Lookup(voxels[i]);
        if(vox.data.x != _data[i].x || vox.data.y != _data[i].y || vox.data.z != _data[i].z)continue;
        octree_.Remove(voxels[i]);
    }
}

void Object::GeneratePerlin3D(config_ d){
    const siv::PerlinNoise perlin{ d.seed };
    if(d.density<0)d.density=0;
    if(d.density>255)d.density=255;
    
    int q = 0;

    for(int k=0;k<Size.z;k++){
        if((int)(((float)k/(float)Size.z)*100)>q){q=(int)(((float)k/(float)Size.z)*100);std::cout<<q<<'%'<<std::endl;}

        for(int i=0;i<Size.y;i++){
            for(int j=0;j<Size.x;j++){
                int x=j,y=i,z=k;
                int noiseData = (int)(perlin.octave3D_01((x*0.01f), (y*0.01f), (z*0.01f), d.octaves)*255);
                if(noiseData<d.density){

                    cl_uint3 col = {
                        255 - (float)z / (float)Size.z * color.x,
                        255 - (float)z / (float)Size.z * color.y,
                        255 - (float)z / (float)Size.z * color.z
                    };
                    
                    voxels.push_back((cl_uint3){x + Position.x,y + Position.y,z + Position.z});
                    octree_.Insert((cl_uint3){x + Position.x,y + Position.y,z + Position.z}, col, type_);
                }
            }
        }
    }
    
}

void Object::GeneratePerlin2D(config_ d){
    const siv::PerlinNoise perlin{ d.seed };
    
    int q = 0;

    for(int k=0;k<Size.y;k++){
        if((int)(((float)k/(float)Size.y)*100)>q){q=(int)(((float)k/(float)Size.y)*100);std::cout<<q<<'%'<<std::endl;}

        for(int i=0;i<Size.x;i++){
            int x=k,y=i;
            int z = (int)(perlin.octave2D_01((x*0.01f), (y*0.01f), d.octaves)*Size.z);

            cl_uint3 col = {
                255 - (float)z / (float)Size.z * color.x,
                255 - (float)z / (float)Size.z * color.y,
                255 - (float)z / (float)Size.z * color.z
            };
                    
            for(int j = z; j > z-5 && j > 1; j--){
                voxels.push_back((cl_uint3){x + Position.x,y + Position.y,j + Position.z});
                octree_.Insert((cl_uint3){x + Position.x,y + Position.y,j + Position.z}, col, type_);
            }
        }
    }
    
}

bool Object::LoadObj(std::string sFilename, bool bHasTexture, bool bHasNormal){

    std::ifstream inFile(sFilename); 
    int length = std::count(std::istreambuf_iterator<char>(inFile), std::istreambuf_iterator<char>(), '\n');
    int k = 0,q = 0;

	std::ifstream f(sFilename);
	if (!f.is_open())
		return false;



	std::vector<Vector> verts;
	std::vector<Vector> texs;
    std::vector<Vector> normals;
    std::vector<triangle> tris;

    std::cout<<"[";


	while (!f.eof())
	{

        if((int)(((float)k/(float)length)*100)>q){int p=(int)(((float)k/(float)length)*100);for(int i = q; i < p; i++)std::cout<<".";q=p;}
        k++;

		char line[128];
		f.getline(line, 128);

		std::strstream s;
		s << line;

		char junk;

		if (line[0] == 'v')
		{
			if (line[1] == 't')
			{
				Vector v;
				s >> junk >> junk >> v.x >> v.y;
				texs.push_back(v);
			}
            else if(line[1] == 'n'){
				Vector v;
				s >> junk >> junk >> v.x >> v.y;
				//normals.push_back(v);
            }
			else
			{
				Vector v;
				s >> junk >> v.x >> v.y >> v.z;
				verts.push_back(v);

                //if(!bHasNormal){
                    normals.push_back(Vector(0,0,0));
                //}

                if(v.x > high)high = v.x;
                if(v.y > high)high = v.y;
                if(v.z > high)high = v.z;

                if(v.x < low)low = v.x;
                if(v.y < low)low = v.y;
                if(v.z < low)low = v.z;
			}
		}
		

		if (!bHasTexture)
		{
			if (line[0] == 'f')
			{
                triangle tr;

                if(!bHasNormal){
                    int f[3];
                    s >> junk >> f[0] >> f[1] >> f[2];
                    tr = { verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] };
                    tr.vert_i[0] = f[0] - 1;
                    tr.vert_i[1] = f[1] - 1;
                    tr.vert_i[2] = f[2] - 1;
                }else{
                    s >> junk;

                    std::string tokens[6];
                    int nTokenCount = -1;


                    while (!s.eof())
                    {
                        char c = s.get();
                        if (c == ' ' || c == '/')
                            nTokenCount++;
                        else
                            tokens[nTokenCount].append(1, c);
                    }

                    tokens[nTokenCount].pop_back();


                    tr.p[0] = verts[stoi(tokens[0]) - 1];
                    tr.p[1] = verts[stoi(tokens[2]) - 1];
                    tr.p[2] = verts[stoi(tokens[4]) - 1];
                    
                    //tr.normal[0] = normals[stoi(tokens[1]) - 1];
                    //tr.normal[1] = normals[stoi(tokens[3]) - 1];
                    //tr.normal[2] = normals[stoi(tokens[5]) - 1];

                    tr.vert_i[0] = stoi(tokens[0]) - 1;
                    tr.vert_i[1] = stoi(tokens[2]) - 1;
                    tr.vert_i[2] = stoi(tokens[4]) - 1;
                }

                //if(!bHasNormal){
                    Vector line1 = tr.p[1] - tr.p[0];
                    Vector line2 = tr.p[2] - tr.p[0];
                    Vector line3 = tr.p[2] - tr.p[1];

                    Vector normal = (line1.cross_product(line2)).normalization();

                    normals[tr.vert_i[0]] +=  normal;
                    normals[tr.vert_i[1]] +=  normal;
                    normals[tr.vert_i[2]] +=  normal;
                //}

                tris.push_back(tr);
			}
        }
		else
		{
			if (line[0] == 'f')
			{
                triangle tr;

                if(!bHasNormal){
                    s >> junk;

                    std::string tokens[6];
                    int nTokenCount = -1;


                    while (!s.eof())
                    {
                        char c = s.get();
                        if (c == ' ' || c == '/')
                            nTokenCount++;
                        else
                            tokens[nTokenCount].append(1, c);
                    }

                    tokens[nTokenCount].pop_back();


                    tr = { verts[stoi(tokens[0]) - 1], verts[stoi(tokens[2]) - 1], verts[stoi(tokens[4]) - 1], texs[stoi(tokens[1]) - 1], texs[stoi(tokens[3]) - 1], texs[stoi(tokens[5]) - 1] };

                    tr.vert_i[0] = stoi(tokens[0]) - 1;
                    tr.vert_i[1] = stoi(tokens[2]) - 1;
                    tr.vert_i[2] = stoi(tokens[4]) - 1;
                }else{
                    s >> junk;

                    std::string tokens[20];
                    int nTokenCount = -1;


                    while (!s.eof())
                    {
                        char c = s.get();
                        if (c == ' ' || c == '/')
                            nTokenCount++;
                        else
                            tokens[nTokenCount].append(1, c);
                    }

                    tokens[nTokenCount].pop_back();


                    tr.p[0] = verts[stoi(tokens[0]) - 1];
                    tr.p[1] = verts[stoi(tokens[3]) - 1];
                    tr.p[2] = verts[stoi(tokens[6]) - 1];

                    tr.t[0] = texs[stoi(tokens[1]) - 1];
                    tr.t[1] = texs[stoi(tokens[4]) - 1];
                    tr.t[2] = texs[stoi(tokens[7]) - 1];
                    
                    //tr.normal[0] = normals[stoi(tokens[2]) - 1];
                    //tr.normal[1] = normals[stoi(tokens[5]) - 1];
                    //tr.normal[2] = normals[stoi(tokens[8]) - 1];

                    tr.vert_i[0] = stoi(tokens[0]) - 1;
                    tr.vert_i[1] = stoi(tokens[3]) - 1;
                    tr.vert_i[2] = stoi(tokens[6]) - 1;
                }

                //if(!bHasNormal){
                    Vector line1 = tr.p[1] - tr.p[0];
                    Vector line2 = tr.p[2] - tr.p[0];
                    Vector line3 = tr.p[2] - tr.p[1];

                    Vector normal = (line1.cross_product(line2)).normalization();

                    normals[tr.vert_i[0]] +=  normal;
                    normals[tr.vert_i[1]] +=  normal;
                    normals[tr.vert_i[2]] +=  normal;
                //}


                tris.push_back(tr);
			}
		}
	}

    for(int i = 0; i < tris.size(); i++){
        for(int j = 0; j < 3; j++){
            //if(!bHasNormal){
                if(normals[tris[i].vert_i[j]].magnitude() != 0){
                    tris[i].normal[j] = normals[tris[i].vert_i[j]].normalization();
                }
            //}else{
            //    tris[i].normal[j] = tris[i].normal[j].normalization();
            //}
        }

        Raster(tris[i], bHasTexture);
    }

    std::cout<<"]"<<std::endl;

	return true;
}

void Object::MapNormals(int detail){
    int q = 0;

    std::cout<<"Generating Normals..."<<std::endl;

    for(int i = 0; i < voxels.size(); i++){
        if((int)(((float)i/(float)voxels.size())*100)>q){q=(int)(((float)i/(float)voxels.size())*100);std::cout<<q<<'%'<<std::endl;}

        leaf v = octree_.Lookup((cl_uint3){voxels[i].x, voxels[i].y, voxels[i].z});
        int index = v.index;

        if(v.type == type_){

            Vector normal;


            for(int x = -detail; x <= detail; x++){
                for(int y = -detail; y <= detail; y++){
                    for(int z = -detail; z <= detail; z++){
                        leaf q = octree_.Lookup((cl_uint3){(int)voxels[i].x + x,(int)voxels[i].y + y,(int)voxels[i].z + z});
                        if(q.type <= 1 || !inBounds((cl_float3){(float)voxels[i].x + x,(float)voxels[i].y + y,(float)voxels[i].z + z}, octree_.n-1)){
                            normal.x += x;
                            normal.y += y;
                            normal.z += z;
                        }
                    }
                }
            }

            if(normal.x != 0 || normal.y != 0 || normal.z != 0)
            {
                normal = normal.normalization();
            

                octree_.octree[index].x = octree_.octree[index].x + (int)((normal.x + 1) * 100) * 1000;
                octree_.octree[index].y = octree_.octree[index].y + (int)((normal.y + 1) * 100) * 1000;
                octree_.octree[index].z = octree_.octree[index].z + (int)((normal.z + 1) * 100) * 1000;
            }
        }
    }
}

//private

void Object::Raster(triangle tr, bool bHasTexture){
    triangle proccesed;

	for(int i = 0; i < 3; i++){
        tr.p[i].x = (int)(((tr.p[i].x - low) / (high - low)) * Size.x);
        tr.p[i].y = (int)(((tr.p[i].y - low) / (high - low)) * Size.y);
        tr.p[i].z = (int)(((tr.p[i].z - low) / (high - low)) * Size.z);

        proccesed.p[i].x = (int)(Position.x); 
        proccesed.p[i].y = (int)(Position.y); 
        proccesed.p[i].z = (int)(Position.z); 
    }


    int az = fabs(
        tr.p[0].x * (tr.p[1].y - tr.p[2].y) + 
        tr.p[1].x * (tr.p[2].y - tr.p[0].y) + 
        tr.p[2].x * (tr.p[0].y - tr.p[1].y)) / 2;

    int ay = fabs(
        tr.p[0].x * (tr.p[1].z - tr.p[2].z) + 
        tr.p[1].x * (tr.p[2].z - tr.p[0].z) + 
        tr.p[2].x * (tr.p[0].z - tr.p[1].z)) / 2;

    int ax = fabs(
        tr.p[0].y * (tr.p[1].z - tr.p[2].z) + 
        tr.p[1].y * (tr.p[2].z - tr.p[0].z) + 
        tr.p[2].y * (tr.p[0].z - tr.p[1].z)) / 2;
    
      
    if(ax >= ay && ax >= az){
        triangle t;
        t.p[0] = Vector(tr.p[0].y, tr.p[0].z, tr.p[0].x);
        t.p[1] = Vector(tr.p[1].y, tr.p[1].z, tr.p[1].x);
        t.p[2] = Vector(tr.p[2].y, tr.p[2].z, tr.p[2].x);
        t.t[0] = tr.t[0];
        t.t[1] = tr.t[1];
        t.normal[0] = tr.normal[0];
        t.normal[1] = tr.normal[1];
        t.normal[2] = tr.normal[2];
        FillTriangle(t, proccesed, 1, 1);

        if(ay > ax/2 && ay > az){
            t.p[0] = Vector(tr.p[0].x, tr.p[0].z, tr.p[0].y);
            t.p[1] = Vector(tr.p[1].x, tr.p[1].z, tr.p[1].y);
            t.p[2] = Vector(tr.p[2].x, tr.p[2].z, tr.p[2].y);
            t.t[0] = tr.t[0];
            t.t[1] = tr.t[1];
            t.normal[0] = tr.normal[0];
            t.normal[1] = tr.normal[1];
            t.normal[2] = tr.normal[2];
            FillTriangle(t, proccesed, 2, 2);
        }

        if(az > ax/2 && az > ay){
            t.p[0] = Vector(tr.p[0].x, tr.p[0].y, tr.p[0].z);
            t.p[1] = Vector(tr.p[1].x, tr.p[1].y, tr.p[1].z);
            t.p[2] = Vector(tr.p[2].x, tr.p[2].y, tr.p[2].z);
            t.t[0] = tr.t[0];
            t.t[1] = tr.t[1];
            t.normal[0] = tr.normal[0];
            t.normal[1] = tr.normal[1];
            t.normal[2] = tr.normal[2];
            FillTriangle(t, proccesed, 3, 2);
        }

    }else if(ay >= ax && ay >= az){
        triangle t;
        t.p[0] = Vector(tr.p[0].x, tr.p[0].z, tr.p[0].y);
        t.p[1] = Vector(tr.p[1].x, tr.p[1].z, tr.p[1].y);
        t.p[2] = Vector(tr.p[2].x, tr.p[2].z, tr.p[2].y);
        t.t[0] = tr.t[0];
        t.t[1] = tr.t[1];
        t.normal[0] = tr.normal[0];
        t.normal[1] = tr.normal[1];
        t.normal[2] = tr.normal[2];
        FillTriangle(t, proccesed, 2, 1);

        if(ax > ay/2 && ax > az){
            t.p[0] = Vector(tr.p[0].y, tr.p[0].z, tr.p[0].x);
            t.p[1] = Vector(tr.p[1].y, tr.p[1].z, tr.p[1].x);
            t.p[2] = Vector(tr.p[2].y, tr.p[2].z, tr.p[2].x);
            t.t[0] = tr.t[0];
            t.t[1] = tr.t[1];
            t.normal[0] = tr.normal[0];
            t.normal[1] = tr.normal[1];
            t.normal[2] = tr.normal[2];
            FillTriangle(t, proccesed, 1, 2);
        }

        if(az > ay/2 && az > ax){
            t.p[0] = Vector(tr.p[0].x, tr.p[0].y, tr.p[0].z);
            t.p[1] = Vector(tr.p[1].x, tr.p[1].y, tr.p[1].z);
            t.p[2] = Vector(tr.p[2].x, tr.p[2].y, tr.p[2].z);
            t.t[0] = tr.t[0];
            t.t[1] = tr.t[1];
            t.normal[0] = tr.normal[0];
            t.normal[1] = tr.normal[1];
            t.normal[2] = tr.normal[2];
            FillTriangle(t, proccesed, 3, 2);
        }
    }else{
        triangle t;
        t.p[0] = Vector(tr.p[0].x, tr.p[0].y, tr.p[0].z);
        t.p[1] = Vector(tr.p[1].x, tr.p[1].y, tr.p[1].z);
        t.p[2] = Vector(tr.p[2].x, tr.p[2].y, tr.p[2].z);
        t.t[0] = tr.t[0];
        t.t[1] = tr.t[1];
        t.normal[0] = tr.normal[0];
        t.normal[1] = tr.normal[1];
        t.normal[2] = tr.normal[2];
        FillTriangle(t, proccesed, 3, 1);  

        if(ax > az/2 && ax > ay){
            t.p[0] = Vector(tr.p[0].y, tr.p[0].z, tr.p[0].x);
            t.p[1] = Vector(tr.p[1].y, tr.p[1].z, tr.p[1].x);
            t.p[2] = Vector(tr.p[2].y, tr.p[2].z, tr.p[2].x);
            t.t[0] = tr.t[0];
            t.t[1] = tr.t[1];
            t.normal[0] = tr.normal[0];
            t.normal[1] = tr.normal[1];
            t.normal[2] = tr.normal[2];
            FillTriangle(t, proccesed, 1, 2);
        }

        if(ay > az/2 && ay > ax){
            t.p[0] = Vector(tr.p[0].x, tr.p[0].z, tr.p[0].y);
            t.p[1] = Vector(tr.p[1].x, tr.p[1].z, tr.p[1].y);
            t.p[2] = Vector(tr.p[2].x, tr.p[2].z, tr.p[2].y);
            t.t[0] = tr.t[0];
            t.t[1] = tr.t[1];
            t.normal[0] = tr.normal[0];
            t.normal[1] = tr.normal[1];
            t.normal[2] = tr.normal[2];
            FillTriangle(t, proccesed, 2, 2);
        }
    }

    cl_uint3 tr_u1 = (cl_uint3){round(tr.p[0].x), round(tr.p[0].y), round(tr.p[0].z)};
    cl_uint3 tr_u2 = (cl_uint3){round(tr.p[1].x), round(tr.p[1].y), round(tr.p[1].z)};
    cl_uint3 tr_u3 = (cl_uint3){round(tr.p[2].x), round(tr.p[2].y), round(tr.p[2].z)};

    Bresenham3D(tr_u1, tr_u2, tr, proccesed);
    Bresenham3D(tr_u2, tr_u3, tr, proccesed);
    Bresenham3D(tr_u1, tr_u3, tr, proccesed);
}

void Object::FillTriangle(triangle t, triangle translation, int view, int step)
{

    triangle o = t;

	if (t.p[1].y < t.p[0].y)
	{
		std::swap(t.p[0].y, t.p[1].y);
		std::swap(t.p[0].x, t.p[1].x);
		std::swap(t.t[0].x, t.t[1].x);
		std::swap(t.t[0].y, t.t[1].y);
		std::swap(t.p[0].z, t.p[1].z);
	}

	if (t.p[2].y < t.p[0].y)
	{
		std::swap(t.p[0].y, t.p[2].y);
		std::swap(t.p[0].x, t.p[2].x);
		std::swap(t.t[0].x, t.t[2].x);
		std::swap(t.t[0].y, t.t[2].y);
		std::swap(t.p[0].z, t.p[2].z);
	}

	if (t.p[2].y < t.p[1].y)
	{
		std::swap(t.p[1].y, t.p[2].y);
		std::swap(t.p[1].x, t.p[2].x);
		std::swap(t.t[1].x, t.t[2].x);
		std::swap(t.t[1].y, t.t[2].y);
		std::swap(t.p[1].z, t.p[2].z);
	}

	int dy1 = t.p[1].y - t.p[0].y;
	int dx1 = t.p[1].x - t.p[0].x;
	float dv1 = t.t[1].y - t.t[0].y;
	float du1 = t.t[1].x - t.t[0].x;
	float dw1 = t.p[1].z - t.p[0].z;

	int dy2 = t.p[2].y - t.p[0].y;
	int dx2 = t.p[2].x - t.p[0].x;
	float dv2 = t.t[2].y - t.t[0].y;
	float du2 = t.t[2].x - t.t[0].x;
	float dw2 = t.p[2].z - t.p[0].z;

	float tex_u, tex_v, tex_w;

	float dax_step = 0, dbx_step = 0,
		du1_step = 0, dv1_step = 0,
		du2_step = 0, dv2_step = 0,
		dw1_step=0, dw2_step=0;

	if (dy1) dax_step = dx1 / (float)abs(dy1);
	if (dy2) dbx_step = dx2 / (float)abs(dy2);

	if (dy1) du1_step = du1 / (float)abs(dy1);
	if (dy1) dv1_step = dv1 / (float)abs(dy1);
	if (dy1) dw1_step = dw1 / (float)abs(dy1);

	if (dy2) du2_step = du2 / (float)abs(dy2);
	if (dy2) dv2_step = dv2 / (float)abs(dy2);
	if (dy2) dw2_step = dw2 / (float)abs(dy2);

	if (dy1)
	{
		for (int i = t.p[0].y; i <= t.p[1].y; i++)
		{
			int ax = t.p[0].x + (float)(i - t.p[0].y) * dax_step;
			int bx = t.p[0].x + (float)(i - t.p[0].y) * dbx_step;

			float tex_su = t.t[0].x + (float)(i - t.p[0].y) * du1_step;
			float tex_sv = t.t[0].y + (float)(i - t.p[0].y) * dv1_step;
			float tex_sw = t.p[0].z + (float)(i - t.p[0].y) * dw1_step;

			float tex_eu = t.t[0].x + (float)(i - t.p[0].y) * du2_step;
			float tex_ev = t.t[0].y + (float)(i - t.p[0].y) * dv2_step;
			float tex_ew = t.p[0].z + (float)(i - t.p[0].y) * dw2_step;

			if (ax > bx)
			{
				std::swap(ax, bx);
				std::swap(tex_su, tex_eu);
				std::swap(tex_sv, tex_ev);
				std::swap(tex_sw, tex_ew);
			}

			tex_u = tex_su;
			tex_v = tex_sv;
			tex_w = tex_sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float q = 0.0f;

			for (int j = ax; j <= bx; j++)
			{
				tex_u = (1.0f - q) * tex_su + q * tex_eu;
				tex_v = (1.0f - q) * tex_sv + q * tex_ev;
				tex_w = (1.0f - q) * tex_sw + q * tex_ew;

                cl_uint3 v1 = (cl_uint3){j,i,tex_w};

                triangle t1 = {o.p[1], o.p[2], Vector(v1.x,v1.y,v1.z)};
                triangle t2 = {o.p[0], o.p[2], Vector(v1.x,v1.y,v1.z)};
                triangle t3 = {o.p[0], o.p[1], Vector(v1.x,v1.y,v1.z)};

                float w1 = area3d(t1)/area3d(o);
                float w2 = area3d(t2)/area3d(o);
                float w3 = 1 - w1 - w2;

                Vector normal = Vector(o.normal[0] * w1 + o.normal[1] * w2 + o.normal[2] * w3);

                if(view == 1){v1 = (cl_uint3){tex_w,j,i};}
                else if(view == 2){v1 = (cl_uint3){j,tex_w,i};}
                else{v1 = (cl_uint3){j,i,tex_w};}

                v1.x+=translation.p[0].x;
                v1.y+=translation.p[0].y;
                v1.z+=translation.p[0].z;

                if(step == 1){
                    if(texture != NULL){
                        Uint32 data = getpixel(texture, (int)((float)(tex_u/tex_w)*(float)(texture->w)), (int)((float)(tex_v/tex_w)*(float)(texture->h)));
                        SDL_Color col;
                        SDL_GetRGB(data, texture->format, &col.r, &col.g, &col.b);

                        cl_uint3 v_data = {(color.x+col.r-255) < 0 ? 0 : (color.x+col.r-255), (color.y+col.g-255) < 0 ? 0 : (color.y+col.g-255), (color.z+col.b-255)  < 0 ? 0 : (color.z+col.b-255)};

                        if(normal.x != 0 || normal.y != 0 || normal.z != 0){
                            normal.normalization();

                            v_data.x = v_data.x + (int)((normal.x + 1) * 100) * 1000;
                            v_data.y = v_data.y + (int)((normal.y + 1) * 100) * 1000;
                            v_data.z = v_data.z + (int)((normal.z + 1) * 100) * 1000;
                        }else{

                            v_data.x = v_data.x + 1000;
                            v_data.y = v_data.y + 1000;
                            v_data.z = v_data.z + 1000;
                        }
                        voxels.push_back(v1);
                        _data.push_back(v_data);
                        octree_.Insert(v1,v_data,type_);
                    }else{
                        cl_uint3 v_data;

                        if(normal.x != 0 || normal.y != 0 || normal.z != 0){
                            normal.normalization();

                            v_data.x = color.x + (int)((normal.x + 1) * 100) * 1000;
                            v_data.y = color.y + (int)((normal.y + 1) * 100) * 1000;
                            v_data.z = color.z + (int)((normal.z + 1) * 100) * 1000;
                        }else{

                            v_data.x = color.x + 1000;
                            v_data.y = color.y + 1000;
                            v_data.z = color.z + 1000;
                        }
                        voxels.push_back(v1);
                        _data.push_back(v_data);
                        octree_.Insert(v1,v_data,type_);
                    }
                }else{
                    if(octree_.Lookup(v1).type <= 1){
                        if(texture != NULL){
                            Uint32 data = getpixel(texture, (int)((float)(tex_u/tex_w)*(float)(texture->w)), (int)((float)(tex_v/tex_w)*(float)(texture->h)));
                            SDL_Color col;
                            SDL_GetRGB(data, texture->format, &col.r, &col.g, &col.b);

                            cl_uint3 v_data = {(color.x+col.r-255) < 0 ? 0 : (color.x+col.r-255), (color.y+col.g-255) < 0 ? 0 : (color.y+col.g-255), (color.z+col.b-255)  < 0 ? 0 : (color.z+col.b-255)};

                            if(normal.x != 0 || normal.y != 0 || normal.z != 0){
                                normal.normalization();

                                v_data.x = v_data.x + (int)((normal.x + 1) * 100) * 1000;
                                v_data.y = v_data.y + (int)((normal.y + 1) * 100) * 1000;
                                v_data.z = v_data.z + (int)((normal.z + 1) * 100) * 1000;
                            }else{

                                v_data.x = v_data.x + (int)((normal.x + 1) * 100) * 1000;
                                v_data.y = v_data.y + (int)((normal.y + 1) * 100) * 1000;
                                v_data.z = v_data.z + (int)((normal.z + 1) * 100) * 1000;
                            }
                            voxels.push_back(v1);
                            _data.push_back(v_data);
                            octree_.Insert(v1,v_data,type_);
                        }else{
                            cl_uint3 v_data;

                            if(normal.x != 0 || normal.y != 0 || normal.z != 0){
                                normal.normalization();

                                v_data.x = color.x + (int)((normal.x + 1) * 100) * 1000;
                                v_data.y = color.y + (int)((normal.y + 1) * 100) * 1000;
                                v_data.z = color.z + (int)((normal.z + 1) * 100) * 1000;
                            }else{

                                v_data.x = color.x + (int)((normal.x + 1) * 100) * 1000;
                                v_data.y = color.y + (int)((normal.y + 1) * 100) * 1000;
                                v_data.z = color.z + (int)((normal.z + 1) * 100) * 1000;
                            }
                            voxels.push_back(v1);
                            _data.push_back(v_data);
                            octree_.Insert(v1,v_data,type_);
                        }
                    }
                }

				q += tstep;
			}
		}
	}

	dy1 = t.p[2].y - t.p[1].y;
	dx1 = t.p[2].x - t.p[1].x;
	dv1 = t.t[2].y - t.t[1].y;
	du1 = t.t[2].x - t.t[1].x;
	dw1 = t.p[2].z - t.p[1].z;

	if (dy1) dax_step = dx1 / (float)abs(dy1);
	if (dy2) dbx_step = dx2 / (float)abs(dy2);

	du1_step = 0, dv1_step = 0;
	if (dy1) du1_step = du1 / (float)abs(dy1);
	if (dy1) dv1_step = dv1 / (float)abs(dy1);
	if (dy1) dw1_step = dw1 / (float)abs(dy1);

	if (dy1)
	{
		for (int i = t.p[1].y; i <= t.p[2].y; i++)
		{
			int ax = t.p[1].x + (float)(i - t.p[1].y) * dax_step;
			int bx = t.p[0].x + (float)(i - t.p[0].y) * dbx_step;

			float tex_su = t.t[1].x + (float)(i - t.p[1].y) * du1_step;
			float tex_sv = t.t[1].y + (float)(i - t.p[1].y) * dv1_step;
			float tex_sw = t.p[1].z + (float)(i - t.p[1].y) * dw1_step;

			float tex_eu = t.t[0].x + (float)(i - t.p[0].y) * du2_step;
			float tex_ev = t.t[0].y + (float)(i - t.p[0].y) * dv2_step;
			float tex_ew = t.p[0].z + (float)(i - t.p[0].y) * dw2_step;

			if (ax > bx)
			{
				std::swap(ax, bx);
				std::swap(tex_su, tex_eu);
				std::swap(tex_sv, tex_ev);
				std::swap(tex_sw, tex_ew);
			}

			tex_u = tex_su;
			tex_v = tex_sv;
			tex_w = tex_sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float q = 0.0f;

			for (int j = ax; j <= bx; j++)
			{
				tex_u = (1.0f - q) * tex_su + q * tex_eu;
				tex_v = (1.0f - q) * tex_sv + q * tex_ev;
				tex_w = (1.0f - q) * tex_sw + q * tex_ew;

                cl_uint3 v1 = (cl_uint3){j,i,tex_w};

                triangle t1 = {o.p[1], o.p[2], Vector(v1.x,v1.y,v1.z)};
                triangle t2 = {o.p[0], o.p[2], Vector(v1.x,v1.y,v1.z)};
                triangle t3 = {o.p[0], o.p[1], Vector(v1.x,v1.y,v1.z)};

                float w1 = area3d(t1)/area3d(o);
                float w2 = area3d(t2)/area3d(o);
                float w3 = 1 - w1 - w2;

                Vector normal = Vector(o.normal[0] * w1 + o.normal[1] * w2 + o.normal[2] * w3);

                if(view == 1){v1 = (cl_uint3){tex_w,j,i};}
                else if(view == 2){v1 = (cl_uint3){j,tex_w,i};}
                else{v1 = (cl_uint3){j,i,tex_w};}

                v1.x+=translation.p[0].x;
                v1.y+=translation.p[0].y;
                v1.z+=translation.p[0].z;

                if(step == 1){
                    if(texture != NULL){
                        Uint32 data = getpixel(texture, (int)((float)(tex_u/tex_w)*(float)(texture->w)), (int)((float)(tex_v/tex_w)*(float)(texture->h)));
                        SDL_Color col;
                        SDL_GetRGB(data, texture->format, &col.r, &col.g, &col.b);

                        cl_uint3 v_data = {(color.x+col.r-255) < 0 ? 0 : (color.x+col.r-255), (color.y+col.g-255) < 0 ? 0 : (color.y+col.g-255), (color.z+col.b-255)  < 0 ? 0 : (color.z+col.b-255)};

                        if(normal.x != 0 || normal.y != 0 || normal.z != 0){
                            normal.normalization();

                            v_data.x = v_data.x + (int)((normal.x + 1) * 100) * 1000;
                            v_data.y = v_data.y + (int)((normal.y + 1) * 100) * 1000;
                            v_data.z = v_data.z + (int)((normal.z + 1) * 100) * 1000;
                        }else{

                            v_data.x = v_data.x + (int)((normal.x + 1) * 100) * 1000;
                            v_data.y = v_data.y + (int)((normal.y + 1) * 100) * 1000;
                            v_data.z = v_data.z + (int)((normal.z + 1) * 100) * 1000;
                        }
                        voxels.push_back(v1);
                        _data.push_back(v_data);
                        octree_.Insert(v1,v_data,type_);
                    }else{
                        cl_uint3 v_data;

                        if(normal.x != 0 || normal.y != 0 || normal.z != 0){
                            normal.normalization();

                            v_data.x = color.x + (int)((normal.x + 1) * 100) * 1000;
                            v_data.y = color.y + (int)((normal.y + 1) * 100) * 1000;
                            v_data.z = color.z + (int)((normal.z + 1) * 100) * 1000;
                        }else{

                            v_data.x = color.x + (int)((normal.x + 1) * 100) * 1000;
                            v_data.y = color.y + (int)((normal.y + 1) * 100) * 1000;
                            v_data.z = color.z + (int)((normal.z + 1) * 100) * 1000;
                        }
                        voxels.push_back(v1);
                        _data.push_back(v_data);
                        octree_.Insert(v1,v_data,type_);
                    }
                }else{
                    if(octree_.Lookup(v1).type <= 1){
                        if(texture != NULL){
                            Uint32 data = getpixel(texture, (int)((float)(tex_u/tex_w)*(float)(texture->w)), (int)((float)(tex_v/tex_w)*(float)(texture->h)));
                            SDL_Color col;
                            SDL_GetRGB(data, texture->format, &col.r, &col.g, &col.b);

                            cl_uint3 v_data = {(color.x+col.r-255) < 0 ? 0 : (color.x+col.r-255), (color.y+col.g-255) < 0 ? 0 : (color.y+col.g-255), (color.z+col.b-255)  < 0 ? 0 : (color.z+col.b-255)};

                            if(normal.x != 0 || normal.y != 0 || normal.z != 0){
                                normal.normalization();

                                v_data.x = v_data.x + (int)((normal.x + 1) * 100) * 1000;
                                v_data.y = v_data.y + (int)((normal.y + 1) * 100) * 1000;
                                v_data.z = v_data.z + (int)((normal.z + 1) * 100) * 1000;
                            }else{

                                v_data.x = v_data.x + (int)((normal.x + 1) * 100) * 1000;
                                v_data.y = v_data.y + (int)((normal.y + 1) * 100) * 1000;
                                v_data.z = v_data.z + (int)((normal.z + 1) * 100) * 1000;
                            }
                            voxels.push_back(v1);
                            _data.push_back(v_data);
                            octree_.Insert(v1,v_data,type_);
                        }else{
                            cl_uint3 v_data;

                            if(normal.x != 0 || normal.y != 0 || normal.z != 0){
                                normal.normalization();

                                v_data.x = color.x + (int)((normal.x + 1) * 100) * 1000;
                                v_data.y = color.y + (int)((normal.y + 1) * 100) * 1000;
                                v_data.z = color.z + (int)((normal.z + 1) * 100) * 1000;
                            }else{

                                v_data.x = color.x + (int)((normal.x + 1) * 100) * 1000;
                                v_data.y = color.y + (int)((normal.y + 1) * 100) * 1000;
                                v_data.z = color.z + (int)((normal.z + 1) * 100) * 1000;
                            }
                            voxels.push_back(v1);
                            _data.push_back(v_data);
                            octree_.Insert(v1,v_data,type_);
                        }
                    }
                }
				
				q += tstep;
			}
		}	
	}		
}

bool Object::CheckNormal(cl_uint3 p){
    leaf q;
    q = octree_.Lookup((cl_uint3){p.x - 1,p.y,p.z});
    if(q.type != type_ || !inBounds((cl_float3){(float)p.x - 1,(float)p.y,(float)p.z}, octree_.n-1)) return 1;

    q = octree_.Lookup((cl_uint3){p.x + 1,p.y,p.z});
    if(q.type != type_ || !inBounds((cl_float3){(float)p.x + 1,(float)p.y,(float)p.z}, octree_.n-1)) return 1;

    q = octree_.Lookup((cl_uint3){p.x,p.y - 1,p.z});
    if(q.type != type_ || !inBounds((cl_float3){(float)p.x,(float)p.y - 1,(float)p.z}, octree_.n-1)) return 1;

    q = octree_.Lookup((cl_uint3){p.x,p.y + 1,p.z});
    if(q.type != type_ || !inBounds((cl_float3){(float)p.x,(float)p.y + 1,(float)p.z}, octree_.n-1)) return 1;

    q = octree_.Lookup((cl_uint3){p.x,p.y,p.z - 1});
    if(q.type != type_ || !inBounds((cl_float3){(float)p.x,(float)p.y,(float)p.z - 1}, octree_.n-1)) return 1;

    q = octree_.Lookup((cl_uint3){p.x,p.y,p.z + 1});
    if(q.type != type_ || !inBounds((cl_float3){(float)p.x,(float)p.y,(float)p.z + 1}, octree_.n-1)) return 1;
}

Uint32 Object::getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp)
	{
		case 1:
			return *p;
			break;

		case 2:
			return *(Uint16 *)p;
			break;

		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
				return p[0] << 16 | p[1] << 8 | p[2];
			else
				return p[0] | p[1] << 8 | p[2] << 16;
				break;

			case 4:
				return *(Uint32 *)p;
				break;

			default:
				return 0;
		}
}

void Object::place_on_line(cl_uint3 v1, triangle t, triangle translation){
    triangle t1 = {t.p[1], t.p[2], Vector(v1.x,v1.y,v1.z)};
    triangle t2 = {t.p[0], t.p[2], Vector(v1.x,v1.y,v1.z)};
    triangle t3 = {t.p[0], t.p[1], Vector(v1.x,v1.y,v1.z)};

    float w1 = area3d(t1)/area3d(t);
    float w2 = area3d(t2)/area3d(t);
    float w3 = 1 - w1 - w2;

    Vector normal = Vector(t.normal[0] * w1 + t.normal[1] * w2 + t.normal[2] * w3);

    v1.x+=translation.p[0].x;
    v1.y+=translation.p[0].y;
    v1.z+=translation.p[0].z;

    if(octree_.Lookup(v1).type <= 1){
        cl_uint3 v_data;

        if(normal.x != 0 || normal.y != 0 || normal.z != 0){
            normal.normalization();

            v_data.x = color.x + (int)((normal.x + 1) * 100) * 1000;
            v_data.y = color.y + (int)((normal.y + 1) * 100) * 1000;
            v_data.z = color.z + (int)((normal.z + 1) * 100) * 1000;
        }else{

            v_data.x = color.x + (int)((normal.x + 1) * 100) * 1000;
            v_data.y = color.y + (int)((normal.y + 1) * 100) * 1000;
            v_data.z = color.z + (int)((normal.z + 1) * 100) * 1000;
        }
        voxels.push_back(v1);
        _data.push_back(v_data);
        octree_.Insert(v1,v_data,type_);
    }
}

void Object::Bresenham3D(cl_uint3 v1, cl_uint3 v2, triangle t, triangle translation){
        
    place_on_line(v1, t, translation);

    int dx = abs((int)v2.x - (int)v1.x);
    int dy = abs((int)v2.y - (int)v1.y);
    int dz = abs((int)v2.z - (int)v1.z);

    int xs,ys,zs,p1,p2;

    if (v2.x > v1.x)
        xs = 1;
    else
        xs = -1;
    if (v2.y > v1.y)
        ys = 1;
    else
        ys = -1;
    if (v2.z > v1.z)
        zs = 1;
    else
        zs = -1;
  
    // Driving axis is X-axis
    if (dx >= dy && dx >= dz){        
        p1 = 2 * dy - dx;
        p2 = 2 * dz - dx;
        while (v1.x != v2.x){
            v1.x += xs;
            if (p1 >= 0){
                v1.y += ys;
                p1 -= 2 * dx;
            }
            if (p2 >= 0){
                v1.z += zs;
                p2 -= 2 * dx;
            }
            p1 += 2 * dy;
            p2 += 2 * dz;

            place_on_line(v1, t, translation);
        }
    }
    // Driving axis is Y-axis
    else if (dy >= dx && dy >= dz){       
        p1 = 2 * dx - dy;
        p2 = 2 * dz - dy;
        while (v1.y != v2.y){
            v1.y += ys;
            if (p1 >= 0){
                v1.x += xs;
                p1 -= 2 * dy;
            }
            if (p2 >= 0){
                v1.z += zs;
                p2 -= 2 * dy;
            }
            p1 += 2 * dx;
            p2 += 2 * dz;

            place_on_line(v1, t, translation);
        }
    }
    // Driving axis is Z-axi
    else{     
        p1 = 2 * dy - dz;
        p2 = 2 * dx - dz;
        while (v1.z != v2.z){
            v1.z += zs;
            if (p1 >= 0){
                v1.y += ys;
                p1 -= 2 * dz;
            }
            if (p2 >= 0){
                v1.x += xs;
                p2 -= 2 * dz;
            }
            p1 += 2 * dy;
            p2 += 2 * dx;

            place_on_line(v1, t, translation);
        }
    }
}

float Object::area3d(triangle t){
    Vector AB = t.p[0] - t.p[1];
    Vector AC = t.p[0] - t.p[2];
    Vector n = AB.cross_product(AC)/2;
    return n.magnitude()/2;
}