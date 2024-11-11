#pragma once
#include <bits/stdc++.h>
#include "vector.hpp"
#include "octree.h"

class model{
    public:
        model(Vector position, float size, Octree octree);

        bool LoadObj(std::string filename);
    private:
        std::vector<int> voxels;
        Octree octree_;

        Vector pos_;
        float size_;

        Vector low;
        Vector high;

        void Raster(Vector triangle[]);

};

model::model(Vector position, float size, Octree octree){
    pos_ = position;
    size_ = size;
    octree_ = octree;
}

bool model::LoadObj(std::string filename){
	std::ifstream f(filename);
	if (!f.is_open())
		return false;

    std::vector<Vector> verts;

    while (!f.eof()){
        char line[128];
		f.getline(line, 128);

		std::strstream s;
		s << line;

		char junk;

		if (line[0] == 'v')
		{
			if (line[1] != 't')
			{
				Vector v;
				s >> junk >> v.x >> v.y >> v.z;
				verts.push_back(v);

                if(v.x < low.x)low.x = v.x;
                if(v.y < low.y)low.y = v.y;
                if(v.z < low.z)low.z = v.z;

                if(v.x < high.x)high.x = v.x;
                if(v.y < high.y)high.y = v.y;
                if(v.z < high.z)high.z = v.z;
			}
		}

		if (line[0] == 'f')
		{
			int f[3];
			s >> junk >> f[0] >> f[1] >> f[2];

            Vector triangle[3];
            triangle[0] = verts[f[0] - 1];
            triangle[1] = verts[f[1] - 1];
            triangle[2] = verts[f[2] - 1];

            
		}
			

        return true;
    }
}


void model::Raster(Vector triangle[]){
    triangle[0] = (triangle[0] - low + pos_) * size_;
    triangle[1] = (triangle[1] - low + pos_) * size_;
    triangle[2] = (triangle[2] - low + pos_) * size_;

    
}