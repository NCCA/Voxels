#ifndef TERRAIN_H
#define TERRAIN_H
#include <array>
#include <cstddef>
#include <ngl/Vec3.h>
#include <vector>
class Terrain
{
    public:
  Terrain(size_t _width, size_t _height, size_t _depth, int numTextures);
  ~Terrain();
  std::vector< ngl::Vec4 > packData();
  void genTextureBuffer();
  void activateTextureBuffer(GLenum _bufferLocation, GLenum _textureLocation, GLenum _activeLocation);
  size_t getNumVoxels() const;

    private:
  // struct Voxel
  // {

  //   Voxel()
  //   {
  //     idx = current_idx++;
  //   };
  //   ngl::Vec3 position = {0, 0, 0};
  //   int textureIndex = 0;
  //   bool isActive = false;
  //   size_t idx;
  //   inline static size_t current_idx = 0;
  // };
  // we can use the index as the main identifier and send all to the GPU
  std::vector< ngl::Vec3 > m_voxelPosition;
  std::vector< GLuint > m_isActive;
  std::vector< GLuint > m_textureIndex;

  //  Voxel getVoxel(size_t _x, size_t _y, size_t _z) const;
  void setVoxel(size_t _x, size_t _y, size_t _z, ngl::Vec3 _pos, int _tex, bool _active);

  // std::vector< Voxel > m_voxels;
  size_t m_width;
  size_t m_height;
  size_t m_depth;
  // we will have two texture buffers 1 for position as a vec3 of type float
  // the 2nd will be an ivec2 with the bufferID and TextureID
  std::array< GLuint, 3 > m_textureID;
  int m_numTextures;
  // size_t m_numActiveVoxels;
};

#endif
