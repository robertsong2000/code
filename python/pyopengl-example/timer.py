# OpenGL Timer sample in Python

import sys
from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *

top = -0.9
isUp = 1
def disp():
  glClear(GL_COLOR_BUFFER_BIT)
  glBegin(GL_POLYGON)
  glVertex2f(-0.9 , -0.9)
  glVertex2f(0 , top)
  glVertex2f(0.9 , -0.9)
  glEnd()
  glFlush()

def timer(value):
  global top, isUp
  if top > 0.9:
    isUp = 0
  elif top <= -0.9:
    isUp = 1
  if isUp:
    topadd = 0.1
  else:
    topadd = -0.1
  top = top + topadd
  glutPostRedisplay()
  glutTimerFunc(1000 , timer , 0)

glutInit(sys.argv)
glutInitWindowPosition(100 , 50)
glutInitWindowSize(400 , 300)
glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA)
glutCreateWindow("Kitty on your lap")
glutDisplayFunc(disp)
glutTimerFunc(100 , timer , 0)
glutMainLoop()
