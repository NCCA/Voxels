#include "Terrain.h"
#include <OpenImageIO/imageio.h>
#include <ngl/Random.h>
#include <cstddef>
#include <random>

Terrain::Terrain(size_t _width, size_t _height, size_t _depth, int _numTextures)
  : m_width(_width), m_height(_height), m_depth(_depth), m_numTextures(_numTextures)
{
  m_voxels.resize(m_width * m_height * m_depth);
  std::uniform_int_distribution< int > randTexture(0, _numTextures);
  ngl::Random::addIntGenerator("randTexture", randTexture);

  // we assume for now that each voxel is 1 unit so we can center our world at 0,0,0 and do half extents for our voxels and set them
  int startX = -(_width / 2);
  int startY = -(_height / 2);
  int startZ = -(_depth / 2);
  int step = 1;

  int xpos = startX;
  int ypos = startY;
  int zpos = startZ;

  for(int y = 0; y < m_height; ++y)
  {
    for(int x = 0; x < m_width; ++x)
    {
      for(int z = 0; z < m_depth; ++z)
      {
        bool active = true; //ngl::Random::getIntFromGeneratorName("randTexture") > _numTextures / 3 ? true : false;
        setVoxel(x, y, z, ngl::Vec3(xpos, ypos, zpos), ngl::Random::getIntFromGeneratorName("randTexture"), active);
        zpos += step;
      }
      zpos = startZ;
      xpos += step;
    }
    xpos = startX;
    ypos += step;
  }
}

size_t Terrain::getNumVoxels() const
{
  return m_numActiveVoxels;
}

std::vector< ngl::Vec4 > Terrain::packData()
{
  m_numActiveVoxels = 0;
  std::vector< ngl::Vec4 > data;
  for(auto v : m_voxels)
  {
    if(v.isActive)
    {
      data.emplace_back(ngl::Vec4(v.position.m_x, v.position.m_y, v.position.m_z, v.textureIndex));
      ++m_numActiveVoxels;
    }
  }
  return data;
}

Terrain::Voxel Terrain::getVoxel(size_t _x, size_t _y, size_t _z) const
{
  if(_x >= m_width || _y >= m_height || _z >= m_depth)
    return Voxel();
  return m_voxels[_x + _y * m_width + _z * m_width * m_height];
}

void Terrain::setVoxel(size_t _x, size_t _y, size_t _z, ngl::Vec3 _pos, int _tex, bool _active)
{
  if(_x >= m_width || _y >= m_height || _z >= m_depth)
    return;
  auto index = _x + _y * m_width + _z * m_width * m_height;
  m_voxels[index].position = _pos;
  m_voxels[index].textureIndex = _tex;
  m_voxels[index].isActive = _active;
}

Terrain::~Terrain()
{
  glDeleteTextures(1, &m_textureID);
}

void Terrain::genTextureBuffer()
{
  glGenTextures(1, &m_textureID);
  glBindBuffer(GL_TEXTURE_BUFFER, m_textureID);
  auto buffer = packData();
  glBufferData(GL_TEXTURE_BUFFER, buffer.size() * sizeof(ngl::Vec4), &buffer[0].m_x, GL_STATIC_DRAW);
}

void Terrain::activateTextureBuffer(GLenum textureUnit)
{
  glActiveTexture(textureUnit);
  glBindTexture(GL_TEXTURE_BUFFER, m_textureID);
  glBindBuffer(GL_TEXTURE_BUFFER, m_textureID);
  glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, m_textureID);
}
