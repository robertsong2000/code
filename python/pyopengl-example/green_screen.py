# --- clr.py
from OpenGL.GL import *
from OpenGL.GLUT import *
#from OpenGL.GLU import *
import sys

ESCAPE = '\033'

def initialize():
    glClearColor(0.5, 0.9, 0.3, 1.0)

def display():
    glClear( GL_COLOR_BUFFER_BIT )
    glFlush()
    
def keyboard(*args):
    if args[0] == ESCAPE:
        sys.exit()

def reshape(w, h):
    pass

def main():
    glutInit(sys.argv)

    glutInitDisplayMode( GLUT_RGB )
    glutInitWindowPosition( 100, 100 )
    glutInitWindowSize( 500, 400 )
    glutCreateWindow( sys.argv[0] )

    glutDisplayFunc( display )
    glutKeyboardFunc( keyboard )
    glutReshapeFunc( reshape )

    initialize()

    glutMainLoop()

print "Hit ESC key to quit."
main()
