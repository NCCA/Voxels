#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include <ngl/Vec3.h>

class Terrain
{
    public :
    Terrain(size_t _width, size_t _height, size_t _depth, int numTextures);
    ~Terrain();
    std::vector<ngl::Vec4> packData() const;
    private :

        struct Voxel
        {
            Voxel()=default;
            ngl::Vec3 position={0,0,0};
            int textureIndex=0;
            bool isActive=false;
        };

        Voxel getVoxel(size_t _x, size_t _y,size_t _z) const;
        void setVoxel(size_t _x, size_t _y,size_t _z,ngl::Vec3 _pos, int _tex, bool _active);

        std::vector<Voxel> m_voxels;
        size_t m_width;
        size_t m_height;
        size_t m_depth;

        int m_numTextures;
};


#endif
