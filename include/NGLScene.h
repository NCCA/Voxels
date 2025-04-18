#ifndef NGLSCENE_H_
#define NGLSCENE_H_
#include "FirstPersonCamera.h"
#include "FrameBufferObject.h"
#include "WindowParams.h"
#include <ngl/Mat4.h>
#include <ngl/Vec3.h>

// this must be included after NGL includes else we get a clash with gl libs
#include <QElapsedTimer>
#include <QOpenGLWindow>
#include <QSet>

#include "Terrain.h"
#include <memory>
//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene.h
/// @brief this class inherits from the Qt OpenGLWindow and allows us to use NGL to draw OpenGL
/// @author Jonathan Macey
/// @version 1.0
/// @date 10/9/13
/// Revision History :
/// This is an initial version used for the new NGL6 / Qt 5 demos
/// @class NGLScene
/// @brief our main glwindow widget for NGL applications all drawing elements are
/// put in this file
//----------------------------------------------------------------------------------------------------------------------

class NGLScene : public QOpenGLWindow
{
    public:
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief ctor for our NGL drawing class
  /// @param [in] parent the parent window to the class
  //----------------------------------------------------------------------------------------------------------------------
  NGLScene();
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief dtor must close down ngl and release OpenGL resources
  //----------------------------------------------------------------------------------------------------------------------
  ~NGLScene() override;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the initialize class is called once when the window is created and we have a valid GL context
  /// use this to setup any default GL stuff
  //----------------------------------------------------------------------------------------------------------------------
  void initializeGL() override;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief this is called everytime we want to draw the scene
  //----------------------------------------------------------------------------------------------------------------------
  void paintGL() override;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief this is called everytime we resize the window
  //----------------------------------------------------------------------------------------------------------------------
  void resizeGL(int _w, int _h) override;

    private:
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief Qt Event called when a key is pressed
  /// @param [in] _event the Qt event to query for size etc
  //----------------------------------------------------------------------------------------------------------------------
  void keyPressEvent(QKeyEvent *_event) override;
  void keyReleaseEvent(QKeyEvent *_event) override;
  void updateVoxelIndex();
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief this method is called every time a mouse is moved
  /// @param _event the Qt Event structure
  //----------------------------------------------------------------------------------------------------------------------
  void mouseMoveEvent(QMouseEvent *_event) override;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief this method is called everytime the mouse button is pressed
  /// inherited from QObject and overridden here.
  /// @param _event the Qt Event structure
  //----------------------------------------------------------------------------------------------------------------------
  void mousePressEvent(QMouseEvent *_event) override;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief this method is called everytime the mouse button is released
  /// inherited from QObject and overridden here.
  /// @param _event the Qt Event structure
  //----------------------------------------------------------------------------------------------------------------------
  void mouseReleaseEvent(QMouseEvent *_event) override;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief this method is called everytime the mouse wheel is moved
  /// inherited from QObject and overridden here.
  /// @param _event the Qt Event structure
  //----------------------------------------------------------------------------------------------------------------------
  void wheelEvent(QWheelEvent *_event) override;

  void createFramebuffer();
  /// @brief windows parameters for mouse control etc.
  WinParams m_win;
  /// position for our model
  ngl::Vec3 m_modelPos;
  GLuint m_vaoID;
  ngl::Mat4 m_view;
  ngl::Mat4 m_project;
  ngl::Mat4 m_mouseGlobalTX;
  GLuint m_textureID;
  std::unique_ptr< Terrain > m_terrain;
  FirstPersonCamera m_cam;
  std::unique_ptr< FrameBufferObject > m_renderFBO;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief timing for camera update
  //----------------------------------------------------------------------------------------------------------------------
  float m_deltaTime = 0.0f;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief frame time for camera update
  //----------------------------------------------------------------------------------------------------------------------
  float m_lastFrame = 0.0f;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief timer for measurement
  /// -----------------------------------------------------------------------------
  QElapsedTimer m_timer;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the keys being pressed
  //----------------------------------------------------------------------------------------------------------------------
  QSet< Qt::Key > m_keysPressed;
  bool m_debug = false;
  ngl::Vec2 m_screenClick;
  size_t m_voxelIndex;
  float m_currentDepth;
};

#endif
