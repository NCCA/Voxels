#include <OpenGL/gl.h>
#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include "ngl/Shader.h"
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
}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}



void NGLScene::resizeGL(int _w , int _h)
{
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
  m_project = ngl::perspective(45.0f,float(_w)/float(_h),0.01f, 1000.0f);
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
  glBindVertexArray(m_vaoID);
  std::array<GLfloat,4> point={0.0f,0.0f,0.0f,85};
  GLuint vboID;
  glGenBuffers(1, &vboID);
  // now bind this to the VBO buffer
  glBindBuffer(GL_ARRAY_BUFFER, vboID);
  // allocate the buffer data
  glBufferData(GL_ARRAY_BUFFER, point.size()*sizeof(GLfloat), &point[0], GL_STATIC_DRAW);
  // now fix this to the attribute buffer 0
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);
  ngl::ShaderLib::loadShader("VoxelShader","shaders/VoxelVertex.glsl","shaders/VoxelFragment.glsl","shaders/VoxelGeometry.glsl");
  ngl::ShaderLib::use("VoxelShader");
  ngl::ShaderLib::setUniform("textureAtlasDims",16,16);
  ngl::Texture texture("textures/minecrafttextures.jpg");
  m_textureID = texture.setTextureGL();


}



void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_win.width,m_win.height);

  // Rotation based on the mouse position for our global transform
  auto rotX = ngl::Mat4::rotateX(m_win.spinXFace);
  auto rotY = ngl::Mat4::rotateY(m_win.spinYFace);

  // multiply the rotations
  m_mouseGlobalTX = rotX * rotY;
  // add the translations
  m_mouseGlobalTX.m_m[3][0] = m_modelPos.m_x;
  m_mouseGlobalTX.m_m[3][1] = m_modelPos.m_y;
  m_mouseGlobalTX.m_m[3][2] = m_modelPos.m_z;

  m_view=ngl::lookAt({0.0f,2.0f,2.0f},{0,0,0},{0,1,0});
  ngl::ShaderLib::setUniform("MVP",m_project*m_view*m_mouseGlobalTX);
  glBindBuffer(GL_ARRAY_BUFFER,m_vaoID);
  glBindTexture(GL_TEXTURE_2D,m_textureID);
  glDrawArrays(GL_POINTS,0,1);
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
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
