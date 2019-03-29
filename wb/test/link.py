import sys
from math import *
from qt import *
from qtgl import *
from OpenGL.GL import *
from odsystem import *

pi=3.1415926
zeros = [0.0, 0.0, 0.0]
half = sqrt(2.0)/2

b=[]
c=[]
for i in range(4):
    b.append(OdBody(i))
for i in range(4):
    c.append(OdRevoluteJoint(i))
cm1 = OdMarker(1)
cm1.set_position([0.0, 0.5, 0.0])
cm1.set_angles([0.0, 0.0, 0.0])
b[1].add_cm_marker(cm1)
cm2 = OdMarker(2, [0.5, 1.5, 0.0], [pi/4, 0.0, 0.0])
b[2].add_cm_marker(cm2)
cm3 = OdMarker(3, [1.0, 1.0, 0.0], [0.0, 0.0, 0.0])
b[3].add_cm_marker(cm3)

omar1 = OdMarker(4, zeros, zeros)
b[0].add_marker(omar1)
omar2 = OdMarker(5, [0.0, -0.5, 0.0], zeros)
b[1].add_marker(omar2)
c[0].set_imarker(omar2)
c[0].set_jmarker(omar1)
omar3 = OdMarker(6, [0.0, 0.5, 0.0], zeros)
b[1].add_marker(omar3)
omar4 = OdMarker(7, [-half, 0.0, 0.0], zeros)
b[2].add_marker(omar4)
c[1].set_imarker(omar4)
c[1].set_jmarker(omar3)
omar5 = OdMarker(8, [half, 0.0, 0.0], zeros)
b[2].add_marker(omar5)
omar6 = OdMarker(9, [0.0, 1.0, 0.0], zeros)
b[3].add_marker(omar6)
c[2].set_imarker(omar6)
c[2].set_jmarker(omar5)
omar7 = OdMarker(10, [0.0, -1.0, 0.0], zeros)
b[3].add_marker(omar7)
omar8 = OdMarker(11, [1.0, 0.0, 0.0], zeros)
b[0].add_marker(omar8)
c[3].set_imarker(omar7)
c[3].set_jmarker(omar8)

#c[0].set_expression("90/180.0*pi")
c[0].set_expression("0*pi/180.0+(time+.0)*pi")
#c[0].set_expression("1*time*pi")

sys_ = OdSystem("4bar")
for i in range(4):
    sys_.add_body(b[i])
for i in range(4):
    sys_.add_constraint(c[i])
sys_.displacement_ic()
sys_.velocity_ic()
sys_.kinematic_analysis(.25, 1)
pos1=omar4.position()
pos2=omar5.position()
pos3=cm3.position()
print pos3

del sys_

def link(x1, y1, x2, y2, x3, y3, x4, y4):

  glShadeModel(GL_FLAT)
  w=2
  h=2
  glMatrixMode(GL_MODELVIEW)
  glLoadIdentity()
  glOrtho(-1.0, 3, -2.5, 2.5, -1, 1)
  glLineWidth(3.0)
  glBegin(GL_LINE_STRIP)
  glVertex3f(0.0, 0.0, 0.0)
  glVertex3f(x1, y1, 0.0)
  glVertex3f(x2, y2, 0.0)
  glVertex3f(x3, y3, 0.0)
  glVertex3f(x4, y4, 0.0)
  glEnd()

class GearWidget(QGLWidget):
  def __init__(self,parent=None,name=None):
    QGLWidget.__init__(self,parent,name)

  def paintGL(self):
      link(pos1[0], pos1[1], pos2[0], pos2[1], pos3[0], pos3[1], 1.0, 0.0)
  def initializeGL(self):
      link(pos1[0], pos1[1], pos2[0], pos2[1], pos3[0], pos3[1], 1.0, 0.0)
  def resizeGL(self,width,height):
      glViewport( 0, 0, width, height )
      link(pos1[0], pos1[1], pos2[0], pos2[1], pos3[0], pos3[1], 1.0, 0.0)
if __name__=='__main__':
  QApplication.setColorSpec(QApplication.CustomColor)
  app=QApplication(sys.argv)

  if not QGLFormat.hasOpenGL():
    raise 'No Qt OpenGL support.'

  widget=GearWidget()
  app.setMainWidget(widget)
  widget.show()
  app.exec_loop()
