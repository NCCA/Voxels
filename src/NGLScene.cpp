#include <QGuiApplication>
#include <QMouseEvent>

#include "NGLScene.h"
#include "ScopedBind.h"
#include <cstdlib>
#include <iostream>
#include <ngl/NGLInit.h>
#include <ngl/ShaderLib.h>
#include <ngl/Texture.h>
#include <ngl/Types.h>
#include <ngl/Util.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/Vec3.h>
NGLScene::NGLScene()
{
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  setTitle("Voxel Rendering in NGL");
  m_timer.start();
}

NGLScene::~NGLScene()
{
  std::cout << "Shutting down NGL, removing VAO's and Shaders\n";
}

void NGLScene::resizeGL(int _w, int _h)
{
  m_win.width = static_cast< int >(_w * devicePixelRatio());
  m_win.height = static_cast< int >(_h * devicePixelRatio());
  m_cam.setProjection(45.0f, static_cast< float >(_w) / _h, 0.05f, 350.0f);
}

void NGLScene::createFramebuffer()
{
  FrameBufferObject::setDefaultFBO(defaultFramebufferObject());
  m_renderFBO = FrameBufferObject::create(width() * devicePixelRatio(), height() * devicePixelRatio());
  m_renderFBO->bind();
  m_renderFBO->addColourAttachment("colour", GLAttatchment::_0, GLTextureFormat::RGBA, GLTextureInternalFormat::RGBA16F, GLTextureDataType::FLOAT, GLTextureMinFilter::NEAREST, GLTextureMagFilter::NEAREST, GLTextureWrap::CLAMP_TO_EDGE, GLTextureWrap::CLAMP_TO_EDGE, true);
  m_renderFBO->addColourAttachment("id", GLAttatchment::_1, GLTextureFormat::RGBA, GLTextureInternalFormat::RGBA16F, GLTextureDataType::FLOAT, GLTextureMinFilter::NEAREST, GLTextureMagFilter::NEAREST, GLTextureWrap::CLAMP_TO_EDGE, GLTextureWrap::CLAMP_TO_EDGE, true);

  m_renderFBO->addDepthBuffer(GLTextureDepthFormats::DEPTH_COMPONENT24, GLTextureMinFilter::NEAREST, GLTextureMagFilter::NEAREST, GLTextureWrap::CLAMP_TO_EDGE, GLTextureWrap::CLAMP_TO_EDGE, true);
  // setup draw buffers whilst still bound
  std::array< GLuint, 2 > attachments = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
  glDrawBuffers(attachments.size(), &attachments[0]);

  if(!m_renderFBO->isComplete())
  {
    ngl::NGLMessage::addWarning("FrameBuffer incomplete");
    m_renderFBO->print();
  }
  m_renderFBO->unbind();
}
void NGLScene::initializeGL()
{
  // we must call that first before any other GL commands to load and link the
  // gl commands from the lib, if that is not done program will crash
  ngl::NGLInit::initialize();
  glClearColor(0.7f, 0.7f, 0.7f, 1.0f); // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);
  glGenVertexArrays(1, &m_vaoID);
  ngl::ShaderLib::loadShader("VoxelShader", "shaders/VoxelVertex.glsl", "shaders/VoxelFragment.glsl", "shaders/VoxelGeometry.glsl");
  ngl::ShaderLib::use("VoxelShader");
  ngl::ShaderLib::setUniform("textureAtlasDims", 16, 16);
  ngl::Texture texture("textures/minecrafttextures.jpg");
  m_textureID = texture.setTextureGL();
  ngl::ShaderLib::setUniform("textureSampler", 0);
  ngl::ShaderLib::setUniform("posSampler", 1);
  ngl::ShaderLib::setUniform("texIndexSampler", 2);
  ngl::ShaderLib::setUniform("isActiveSampler", 3);

  m_terrain = std::make_unique< Terrain >(200, 20, 200, 16 * 16);
  // m_terrain = std::make_unique< Terrain >(10, 10, 10, 16 * 16);
  m_terrain->genTextureBuffer();

  ngl::Vec3 from(0, 10, 60);
  ngl::Vec3 to(0, 0, 0);
  ngl::Vec3 up(0, 1, 0);
  // now load to our new camera
  m_cam.set(from, to, up);
  m_cam.setProjection(45.0f, 1024.0f / 720.0f, 0.05f, 350.0f);
  createFramebuffer();
}

void NGLScene::paintGL()
{

  {
    ScopedBind< FrameBufferObject > scope(m_renderFBO.get());
    m_renderFBO->setViewport();
    // clear the screen and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, m_win.width, m_win.height);
    float currentFrame = m_timer.elapsed() * 0.001f;
    m_deltaTime = currentFrame - m_lastFrame;
    m_lastFrame = currentFrame;

    /// first we reset the movement values
    float xDirection = 0.0;
    float yDirection = 0.0;
    constexpr float inc = 1.0f;
    // now we loop for each of the pressed keys in the the set
    // and see which ones have been pressed. If they have been pressed
    // we set the movement value to be an incremental value
    foreach(Qt::Key key, m_keysPressed)
    {
      switch(key)
      {
        case Qt::Key_Left:
        {
          yDirection = -inc;
          break;
        }
        case Qt::Key_Right:
        {
          yDirection = inc;
          break;
        }
        case Qt::Key_Up:
        {
          xDirection = inc;
          break;
        }
        case Qt::Key_Down:
        {
          xDirection = -inc;
          break;
        }
        default:
          break;
      }
    }
    // if the set is non zero size we can update the ship movement
    // then tell openGL to re-draw
    if(m_keysPressed.size() != 0)
    {
      m_cam.move(xDirection, yDirection, m_deltaTime);
    }
    ngl::ShaderLib::setUniform("MVP", m_cam.getVP());
    glBindBuffer(GL_ARRAY_BUFFER, m_vaoID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    m_terrain->activateTextureBuffer(GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3);
    glBindVertexArray(m_vaoID);
    auto numVoxels = m_terrain->getNumVoxels();
    glDrawArrays(GL_POINTS, 0, numVoxels);
  } // end scoped bind

  updateVoxelIndex();
  if(m_keysPressed.contains(Qt::Key_S))
  {
    m_terrain->removeIndex(m_voxelIndex);
  }
  if(m_keysPressed.contains(Qt::Key_Z))
  {
    m_terrain->changeTextureID(m_voxelIndex, -1);
  }

  if(m_keysPressed.contains(Qt::Key_X))
  {
    m_terrain->changeTextureID(m_voxelIndex, 1);
  }

  // now blit
  auto w = m_renderFBO->width();
  auto h = m_renderFBO->height();
  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_renderFBO->getID());
  if(!m_debug)
    glReadBuffer(GL_COLOR_ATTACHMENT0);
  else
    glReadBuffer(GL_COLOR_ATTACHMENT1);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, defaultFramebufferObject()); // default framebuffer
  glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
  glReadBuffer(GL_COLOR_ATTACHMENT0);
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  m_keysPressed += static_cast< Qt::Key >(_event->key());

  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch(_event->key())
  {
    // escape key to quite
    case Qt::Key_Escape:
      QGuiApplication::exit(EXIT_SUCCESS);
      break;
    case Qt::Key_Space:
      m_win.spinXFace = 0;
      m_win.spinYFace = 0;
      m_modelPos.set(ngl::Vec3::zero());
      break;
    case Qt::Key_D:
      m_debug ^= true;
      break;

    default:
      break;
  }
  // finally update the GLWindow and re-draw
  update();
}

void NGLScene::updateVoxelIndex()
{
  std::array< GLubyte, 4 > data;
  glBindFramebuffer(GL_READ_FRAMEBUFFER, m_renderFBO->getID());
  // ScopedBind<FrameBufferObject> renderFBO(m_renderFBO.get());
  glReadBuffer(GL_COLOR_ATTACHMENT1);

  std::array< GLint, 4 > viewport;
  glGetIntegerv(GL_VIEWPORT, &viewport[0]);
  // read the pixels (1,1 at present but could do wider area)
  glReadPixels(m_screenClick.m_x * devicePixelRatio(), viewport[3] - m_screenClick.m_y * devicePixelRatio(), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

  // glReadPixels(m_screenClick.m_x, m_screenClick.m_y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
  glReadBuffer(GL_COLOR_ATTACHMENT0);
  m_voxelIndex = ((data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); // back to default
}

void NGLScene::keyReleaseEvent(QKeyEvent *_event)
{
  // remove from our key set any keys that have been released
  m_keysPressed -= static_cast< Qt::Key >(_event->key());
  update();
}
