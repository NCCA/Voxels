#include "Terrain.h"
#include <ngl/Random.h>
Terrain::Terrain(size_t _width, size_t _height, size_t _depth,int _numTextures)
    : m_width(_width), m_height(_height),m_depth(_depth), m_numTextures(_numTextures)
{
    m_voxels.resize(m_width * m_height * m_depth);
    std::uniform_int_distribution<int> randTexture(0,_numTextures);
    ngl::Random::addIntGenerator("randTexture", randTexture );

    // we assume for now that each voxel is 1 unit so we can center our world at 0,0,0 and do half extents for our voxels and set them
    int startX = -(_width/2);
    int startY = -(_height/2);
    int startZ = -(_depth/2);
    int step=1;

    int xpos=startX;
    int ypos=startY;
    int zpos=startZ;

    for (int y=0; y<m_height; ++y)
    {
        for(int x=0; x<m_width; ++x)
        {
            for(int z=0; z<m_depth; ++z)
            {
                bool active = ngl::Random::getIntFromGeneratorName("randTexture") >_numTextures/2 ? true  : false;
                setVoxel(x,y,z,ngl::Vec3(xpos,ypos,zpos),ngl::Random::getIntFromGeneratorName("randTexture"),active);
                zpos+=step;
            }
            zpos=startZ;
            xpos+=step;
        }
        xpos=startX;
        ypos+=step;
    }

}

std::vector<ngl::Vec4> Terrain::packData() const
{
    std::vector<ngl::Vec4> data;
    for(auto v : m_voxels)
    {
        if(v.isActive)
            data.emplace_back(ngl::Vec4(v.position.m_x,v.position.m_y,v.position.m_z,v.textureIndex));
    }
    return data;
}

Terrain::Voxel Terrain::getVoxel(size_t _x, size_t _y,size_t _z) const
{

    if (_x >= m_width || _y >= m_height || _z >= m_depth)
        return Voxel();
    return m_voxels[_x + _y * m_width + _z * m_width * m_height];
}

void Terrain::setVoxel(size_t _x, size_t _y,size_t _z,ngl::Vec3 _pos, int _tex, bool _active)
{
    if (_x >= m_width || _y >= m_height || _z >= m_depth)
        return ;
    auto index=_x + _y * m_width + _z * m_width * m_height;
    m_voxels[index].position=_pos;
    m_voxels[index].textureIndex=_tex;
    m_voxels[index].isActive=_active;

}


Terrain::~Terrain()
{
}
