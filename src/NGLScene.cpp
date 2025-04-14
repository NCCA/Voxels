#include <OpenGL/gl.h>
#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include <ngl/Types.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <ngl/Util.h>
#include <iostream>
#include <cstdlib>
#include <array>
#include <ngl/Texture.h>
NGLScene::NGLScene()
{
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  setTitle("Voxel Rendering in NGL");
  m_timer.start();

}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";

}



void NGLScene::resizeGL(int _w , int _h)
{
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
  m_cam.setProjection(45.0f, static_cast<float>(_w) / _h, 0.05f, 350.0f);

}


void NGLScene::initializeGL()
{
  // we must call that first before any other GL commands to load and link the
  // gl commands from the lib, if that is not done program will crash
  ngl::NGLInit::initialize();
  glClearColor(0.7f, 0.7f, 0.7f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);
  glGenVertexArrays(1, &m_vaoID);
  ngl::ShaderLib::loadShader("VoxelShader","shaders/VoxelVertex.glsl","shaders/VoxelFragment.glsl","shaders/VoxelGeometry.glsl");
  ngl::ShaderLib::use("VoxelShader");
  ngl::ShaderLib::setUniform("textureAtlasDims",16,16);
  ngl::Texture texture("textures/minecrafttextures.jpg");
  m_textureID = texture.setTextureGL();

  m_terrain=std::make_unique<Terrain>(100,50,100,16*16);
  // Now we will create a basic Camera from the graphics library
  // This is a static camera so it only needs to be set once
  // First create Values for the camera position
  ngl::Vec3 from(0, 20, 150);
  ngl::Vec3 to(0, 0, 0);
  ngl::Vec3 up(0, 1, 0);
  // now load to our new camera
  m_cam.set(from, to, up);
  m_cam.setProjection(45.0f, 720.0f / 576.0f, 0.05f, 350.0f);


}



void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_win.width,m_win.height);
  float currentFrame = m_timer.elapsed() * 0.001f;
  std::cout << "Current Frame " << currentFrame << '\n';
  m_deltaTime = currentFrame - m_lastFrame;
  m_lastFrame = currentFrame;

  /// first we reset the movement values
  float xDirection = 0.0;
  float yDirection = 0.0;
  constexpr float inc=2.5f;
  // now we loop for each of the pressed keys in the the set
  // and see which ones have been pressed. If they have been pressed
  // we set the movement value to be an incremental value
  foreach (Qt::Key key, m_keysPressed)
  {
    switch (key)
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
  if (m_keysPressed.size() != 0)
  {
    m_cam.move(xDirection, yDirection, m_deltaTime);
  }



  ngl::ShaderLib::setUniform("MVP",m_cam.getVP());
  glBindBuffer(GL_ARRAY_BUFFER,m_vaoID);
  glBindTexture(GL_TEXTURE_2D,m_textureID);

  glBindVertexArray(m_vaoID);
  std::array<GLfloat,4> point={0.0f,0.0f,0.0f,85};
  GLuint vboID;
  glGenBuffers(1, &vboID);
  // now bind this to the VBO buffer
  glBindBuffer(GL_ARRAY_BUFFER, vboID);
  // allocate the buffer data
  auto buffer=m_terrain->packData();
  glBufferData(GL_ARRAY_BUFFER, buffer.size()*sizeof(ngl::Vec4), &buffer[0].m_x, GL_STATIC_DRAW);
  // now fix this to the attribute buffer 0
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);
  glDrawArrays(GL_POINTS,0,buffer.size());
  glDeleteBuffers(1,&vboID);
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
    m_keysPressed += static_cast<Qt::Key>(_event->key());

  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
  case Qt::Key_Space :
      m_win.spinXFace=0;
      m_win.spinYFace=0;
      m_modelPos.set(ngl::Vec3::zero());

  break;
  default : break;
  }
  // finally update the GLWindow and re-draw
    update();
}


void NGLScene::keyReleaseEvent(QKeyEvent *_event)
{
  // remove from our key set any keys that have been released
  m_keysPressed -= static_cast<Qt::Key>(_event->key());
}
