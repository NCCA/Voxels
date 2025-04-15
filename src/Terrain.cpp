#include "Terrain.h"
#include <OpenImageIO/imageio.h>
#include <algorithm>
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

void Terrain::removeIndex(size_t _index)
{
  if(_index > m_isActive.size())
    return;
  m_isActive[_index] = false;
  glBindBuffer(GL_TEXTURE_BUFFER, m_textureID[2]);
  glBufferData(GL_TEXTURE_BUFFER, m_isActive.size() * sizeof(GLuint), &m_isActive[0], GL_STATIC_DRAW);
}

void Terrain::changeTextureID(size_t _index, int value)
{
  if(_index > m_textureIndex.size())
    return;

  m_textureIndex[_index] += value;
  m_textureIndex[_index] = std::clamp(m_textureIndex[_index], GLuint{0}, GLuint{m_numTextures});
  glBindBuffer(GL_TEXTURE_BUFFER, m_textureID[1]);
  glBufferData(GL_TEXTURE_BUFFER, m_textureIndex.size() * sizeof(GLuint), &m_textureIndex[0], GL_STATIC_DRAW);
}

void Terrain::setVoxel(size_t _x, size_t _y, size_t _z, ngl::Vec3 _pos, int _tex, bool _active)
{
  if(_x >= m_width || _y >= m_height || _z >= m_depth)
    return;
  auto index = _x + _y * m_width + _z * m_width * m_height;

  m_voxelPosition[index] = _pos;
  m_textureIndex[index] = _tex;
  m_isActive[index] = _active;
}

Terrain::~Terrain()
{
  glDeleteTextures(m_textureID.size(), &m_textureID[0]);
}

void Terrain::genTextureBuffer()
{
  glGenTextures(m_textureID.size(), &m_textureID[0]);
  // voxel position id 0
  glBindBuffer(GL_TEXTURE_BUFFER, m_textureID[0]);
  glBufferData(GL_TEXTURE_BUFFER, m_voxelPosition.size() * sizeof(ngl::Vec3), &m_voxelPosition[0].m_x, GL_STATIC_DRAW);
  // what texture to use id 1
  glBindBuffer(GL_TEXTURE_BUFFER, m_textureID[1]);
  glBufferData(GL_TEXTURE_BUFFER, m_textureIndex.size() * sizeof(GLuint), &m_textureIndex[0], GL_STATIC_DRAW);
  // visibility flage id 2
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
