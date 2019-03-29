from math import *
from qt import *
from qtgl import *
from OpenGL.GL import *

def draw_cylinder(height, rad):
    den = 20
    x = []
    y = []
    glColor3f(1.0, 1.0, 1.0)
    half_h = height/2.0;
    for i in range(den+1):
        angle = float(i)/float(den)*pi*2.0
        x.append(rad*cos(angle))
        y.append(rad*sin(angle))
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
    #draw top cover
    glBegin(GL_TRIANGLE_FAN)
    glVertex3f(0.0, 0.0, half_h)
    for i in range(den+1):
        glVertex3f(x[i], y[i], half_h)
    glEnd()
    #draw bottom cover
    glBegin(GL_TRIANGLE_FAN)
    glVertex3f(0.0, 0.0, -half_h)
    for i in range(den+1):
        j = den-i-1
        glVertex3f(x[j], y[j], -half_h)
    glEnd()
    #draw cylinder
    glBegin(GL_QUAD_STRIP)
    for i in range(den+1):
        glVertex3f(x[i], y[i], half_h)
        glVertex3f(x[i], y[i], -half_h)
    glEnd()

def draw_link(l, w, h):
    den = 5
    half_l = l/2.0
    half_w = w/2.0
    half_h = h/2.0
    #draw_box
    glBegin(GL_QUAD_STRIP)
    glVertex3f(-half_l, -half_w, half_h)
    glVertex3f(-half_l, -half_w, -half_h)
    #
    glVertex3f(half_l, -half_w, half_h)
    glVertex3f(half_l, -half_w, -half_h)
    #
    glVertex3f(half_l, half_w, half_h)
    glVertex3f(half_l, half_w, -half_h)
    #
    glVertex3f(-half_l, half_w, half_h)
    glVertex3f(-half_l, half_w, -half_h)
    #
    glVertex3f(-half_l, -half_w, half_h)
    glVertex3f(-half_l, -half_w, -half_h)
    glEnd()
    #discs
    x=[]
    z=[]
    for i in range(den+1):
        angle = float(i)/float(den)*pi
        x.append(half_l*cos(angle))
        z.append(half_l*sin(angle))
    #draw front face -- top
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
    glBegin(GL_TRIANGLE_FAN)
    glVertex3f(0.0, half_w, half_h)
    for i in range(den+1):
        glVertex3f(x[i], half_w, z[i]+half_h)
    glEnd()
    #draw front face -- bottom
    glPolygonMode(GL_BACK, GL_LINE)
    glBegin(GL_TRIANGLE_FAN)
    glVertex3f(0.0, half_w, -half_h)
    for i in range(den+1):
        glVertex3f(x[i], half_w, -z[i]-half_h)
    glEnd()
    #draw back face -- top
    glPolygonMode(GL_BACK, GL_LINE)
    glBegin(GL_TRIANGLE_FAN)
    glVertex3f(0.0, -half_w, half_h)
    for i in range(den+1):
        glVertex3f(x[i], -half_w, z[i]+half_h)
    glEnd()
    #draw back face -- bottom
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
    glBegin(GL_TRIANGLE_FAN)
    glVertex3f(0.0, -half_w, -half_h)
    for i in range(den+1):
        glVertex3f(x[i], -half_w, -z[i]-half_h)
    glEnd()
    #draw disk side face -- top
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
    glBegin(GL_QUAD_STRIP)
    for i in range(den+1):
        glVertex3f(x[i], half_w, z[i]+half_h)
        glVertex3f(x[i], -half_w, z[i]+half_h)
    glEnd()
    #draw disk side face -- bottom
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
    glBegin(GL_QUAD_STRIP)
    for i in range(den+1):
        glVertex3f(x[i], half_w, -z[i]-half_h)
        glVertex3f(x[i], -half_w, -z[i]-half_h)
    glEnd()
    
        
        
    
