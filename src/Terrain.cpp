#include "Terrain.h"
#include <OpenImageIO/imageio.h>
#include <cstddef>
#include <ngl/Random.h>
#include <random>

Terrain::Terrain(size_t _width, size_t _height, size_t _depth, int _numTextures)
  : m_width(_width), m_height(_height), m_depth(_depth), m_numTextures(_numTextures)
{

  m_voxelPosition.resize(m_width * m_height * m_depth);
  m_isActive.resize(m_width * m_height * m_depth);
  m_textureIndex.resize(m_width * m_height * m_depth);

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
        bool active = ngl::Random::getIntFromGeneratorName("randTexture") > _numTextures / 2 ? true : false;
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
  return m_voxelPosition.size();
}

// std::vector< ngl::Vec4 > Terrain::packData()
// {
//   m_numActiveVoxels = 0;
//   std::vector< ngl::Vec4 > data;
//   for(auto v : m_voxels)
//   {
//     if(v.isActive)
//     {
//       data.emplace_back(ngl::Vec4(v.position.m_x, v.position.m_y, v.position.m_z, v.textureIndex));
//       ++m_numActiveVoxels;
//     }
//   }
//   return data;
// }

// Terrain::Voxel Terrain::getVoxel(size_t _x, size_t _y, size_t _z) const
// {
//   if(_x >= m_width || _y >= m_height || _z >= m_depth)
//     return Voxel();
//   return m_voxels[_x + _y * m_width + _z * m_width * m_height];
// }

void Terrain::setVoxel(size_t _x, size_t _y, size_t _z, ngl::Vec3 _pos, int _tex, bool _active)
{
  if(_x >= m_width || _y >= m_height || _z >= m_depth)
    return;
  auto index = _x + _y * m_width + _z * m_width * m_height;

  m_voxelPosition[index] = _pos;
  m_textureIndex[index] = _tex;
  m_isActive[index] = _active;
  // m_voxels[index].position = _pos;
  // m_voxels[index].textureIndex = _tex;
  // m_voxels[index].isActive = _active;
}

Terrain::~Terrain()
{
  glDeleteTextures(2, &m_textureID[0]);
}

void Terrain::genTextureBuffer()
{
  glGenTextures(3, &m_textureID[0]);
  glBindBuffer(GL_TEXTURE_BUFFER, m_textureID[0]);
  glBufferData(GL_TEXTURE_BUFFER, m_voxelPosition.size() * sizeof(ngl::Vec3), &m_voxelPosition[0].m_x, GL_STATIC_DRAW);

  glBindBuffer(GL_TEXTURE_BUFFER, m_textureID[1]);
  glBufferData(GL_TEXTURE_BUFFER, m_textureIndex.size() * sizeof(GLuint), &m_textureIndex[0], GL_STATIC_DRAW);

  glBindBuffer(GL_TEXTURE_BUFFER, m_textureID[2]);
  glBufferData(GL_TEXTURE_BUFFER, m_isActive.size() * sizeof(GLuint), &m_isActive[0], GL_STATIC_DRAW);
}

void Terrain::activateTextureBuffer(GLenum _bufferLocation, GLenum _textureLocation, GLenum _activeLocation)
{
  glActiveTexture(_bufferLocation);
  glBindTexture(GL_TEXTURE_BUFFER, m_textureID[0]);
  glBindBuffer(GL_TEXTURE_BUFFER, m_textureID[0]);
  glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, m_textureID[0]);

  glActiveTexture(_textureLocation);
  glBindTexture(GL_TEXTURE_BUFFER, m_textureID[1]);
  glBindBuffer(GL_TEXTURE_BUFFER, m_textureID[1]);
  glTexBuffer(GL_TEXTURE_BUFFER, GL_R32I, m_textureID[1]);

  glActiveTexture(_activeLocation);
  glBindTexture(GL_TEXTURE_BUFFER, m_textureID[2]);
  glBindBuffer(GL_TEXTURE_BUFFER, m_textureID[2]);
  glTexBuffer(GL_TEXTURE_BUFFER, GL_R32I, m_textureID[2]);
}
