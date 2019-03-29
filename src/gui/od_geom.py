from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *
from OpenGL.GLE import *
from math import *
from od_utils import *


class od_shape:
    def __init__(self, name=None):
        self.id = glGenLists(1)
        self._name = name

    def draw(self):
        if glIsList(self.id):
            glCallList(self.id)

    def __del__(self):
        if glIsList(self.id):
            glDeleteLists(self.id, 1)

    def name(self):
        return self._name;
    
class od_cylinder(od_shape):
    def __init__(self, rad, top, height, name= "cylinder", slices=10, stacks=2):
        od_shape.__init__(self, name)
        x=[]
        y=[]
        self.rad = rad
        self.top = top
        self.height = height
        half_h = height/2.0
        glNewList(self.id, GL_COMPILE)
        for i in range(slices):
            theta = 2.0*pi/slices*i
            c = rad*cos(theta)
            s = rad*sin(theta)
            x.append(c)
            y.append(s)
        glBegin(GL_LINE_LOOP)
        for i in range(slices):
            glVertex3f(x[i], y[i], half_h)
        glEnd()
        glBegin(GL_LINE_LOOP)
        for i in range(slices):
            glVertex3f(x[i], y[i], -half_h)
        glEnd()
        glBegin(GL_LINES)
        for i in range(slices):
            glVertex3f(x[i], y[i], -half_h)
            glVertex3f(x[i], y[i], half_h)
        glEnd()
        glEndList()

    def save(self, n, temp):
        att=[]
        att = toxml_block(n+1, "type", [self.name()], att)
        att = toxml_block(n+1, "dims", [str(self.rad)+' '+str(self.top)+' '+str(self.height)], att)
        temp = toxml_block(n, "shape", att, temp)
        return temp
    
class od_box(od_shape):
    def __init__(self, l, w, h, name="box"):
        od_shape.__init__(self, name)
        self.l = l
        self.w = w
        self.h = h
        glNewList(self.id, GL_COMPILE)
        glTranslatef(-l/2.0, -w/2.0, -h/2.0) 
        glBegin(GL_LINE_LOOP)
        glVertex3f(0.0, 0.0, 0.0)
        glVertex3f(l, 0.0, 0.0)
        glVertex3f(l, w, 0.0)
        glVertex3f(0.0, w, 0.0)
        glEnd()
        glBegin(GL_LINE_LOOP)
        glVertex3f(0.0, 0.0, h)
        glVertex3f(l,   0.0, h)
        glVertex3f(l,   w,   h)
        glVertex3f(0.0, w,   h)
        glEnd()
        glBegin(GL_LINE_LOOP)
        glVertex3f(0.0, 0.0, 0.0)
        glVertex3f(0.0, 0.0, h)
        glVertex3f(l, 0.0, 0.0)
        glVertex3f(l,   0.0, h)
        glEnd()
        glBegin(GL_LINE_LOOP)
        glVertex3f(l, w, 0.0)
        glVertex3f(l,   w,   h)
        glVertex3f(0.0, w, 0.0)
        glVertex3f(0.0, w,   h)
        glEnd()
        glEndList()

    def save(self, n, temp):
        att=[]
        att = toxml_block(n+1, "type", [self.name()], att)
        att = toxml_block(n+1, "dims", [str(self.l)+' '+str(self.w)+' '+str(self.h)], att)
        temp = toxml_block(n, "shape", att, temp)
        return temp

class od_link(od_shape):
    def __init__(self, l, w, h, name="link"):
        od_shape.__init__(self, name)
        self.l = l
        self.w = w
        self.h = h
        h_l = l/2.0
        h_w = w/2.0
        h_h = h/2.0
        arc_pnts_f=[]
        arc_pnts_b=[]
        low_pnts=[[0.0, 0.0, 0.0], [l, 0.0, 0.0], [l, w, 0.0], [0.0, w, 0.0]]
        up_pnts=[[0.0, 0.0, h], [l, 0.0, h], [l, w, h], [0.0, w, h]]
        for i in range(11):
            angle=pi*float(i)/10.0
            #print cos(angle)
            c = cos(angle)*h_l
            s = sin(angle)*h_l
            arc_pnts_b.append([c+h_l, 0, s])
            arc_pnts_f.append([c+h_l, w, s])
        glNewList(self.id, GL_COMPILE)
        glTranslatef(-l/2.0, -w/2.0, -h/2.0) 
        #draw 4 lines
        glBegin(GL_LINES)
        for i in range(4):
            glVertex3f(low_pnts[i][0], low_pnts[i][1], low_pnts[i][2])
            glVertex3f(up_pnts[i][0], up_pnts[i][1], up_pnts[i][2])
        glEnd()
        #####################################
        glBegin(GL_LINE_STRIP)
        for i in range(11):
            glVertex3f(arc_pnts_b[i][0], arc_pnts_b[i][1], -arc_pnts_b[i][2])
        glEnd()
        glBegin(GL_LINE_STRIP)
        for i in range(11):
            glVertex3f(arc_pnts_f[i][0], arc_pnts_f[i][1], -arc_pnts_f[i][2])
        glEnd()
        glBegin(GL_LINES)
        for i in range(11):
            glVertex3f(arc_pnts_b[i][0], arc_pnts_b[i][1], -arc_pnts_b[i][2])
            glVertex3f(arc_pnts_f[i][0], arc_pnts_f[i][1], -arc_pnts_f[i][2])
        glEnd()
        ########################################
        glBegin(GL_LINE_STRIP)
        for i in range(11):
            glVertex3f(arc_pnts_b[i][0], arc_pnts_b[i][1], h+arc_pnts_b[i][2])
        glEnd()
        glBegin(GL_LINE_STRIP)
        for i in range(11):
            glVertex3f(arc_pnts_f[i][0], arc_pnts_f[i][1], h+arc_pnts_f[i][2])
        glEnd()
        glBegin(GL_LINES)
        for i in range(11):
            glVertex3f(arc_pnts_b[i][0], arc_pnts_b[i][1], h+arc_pnts_b[i][2])
            glVertex3f(arc_pnts_f[i][0], arc_pnts_f[i][1], h+arc_pnts_f[i][2])
        glEnd()
        glEndList()

    def save(self, n, temp):
        att=[]
        att = toxml_block(n+1, "type", [self.name()], att)
        att = toxml_block(n+1, "dims", [str(self.l)+' '+str(self.w)+' '+str(self.h)], att)
        temp = toxml_block(n, "shape", att, temp)
        return temp
