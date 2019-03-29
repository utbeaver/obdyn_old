from math import *
from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *
from types import *
import wx
import math
import  wx.py   as  py

def toxml_block(n, tag, text, temp):
    temp.append(n*"  "+"<"+tag+">\n")
    if len(text) == 1:
        temp.append((n+1)*"  "+text[0]+"\n")
    else:
        for tex in text:
            temp.append(tex)
    temp.append(n*"  "+"</"+tag+">\n")
    return temp

#Euler matrix

def Zeros(n):
    m = []
    for i in range(n):
        vec=[]
        for j in range(n):
            vec.append(0.0)
        m.append(vec)
    return m
    
def Identity(n, val=1.0):
    m =Zeros(n)
    for i in range(n):
        m[i][i] = val
    return m
             
def Euler_phi(angles): 
    angles = angles*pi/180.0
    m = Identity(3)
    s_ = sin(angles)
    c_ = cos(angles)
    m[0][0] = c_
    m[1][1] = c_
    m[0][1] = -s_
    m[1][0] = s_
    return m

def Euler_theta(angles): 
    angles = angles*pi/180.0
    m = Identity(3)
    s_ = sin(angles)
    c_ = cos(angles)
    m[1][1] = c_
    m[2][2] = c_
    m[1][2] = -s_
    m[2][1] = s_
    return m

def Euler_psi(angle):
    m=Euler_phi(angle)
    return m

def mat_multiply(m, n, n_):
    p = Identity(n_)
    for i in range(n_):
        for j in range(n_):
            val = 0.0
            for k in range(n_):
                val = val + m[i][k]*n[k][j]
            p[i][j] = val
    return p

def D_H(ang, vec):
    #orientation
    D = Euler_phi(ang[0])
    C = Euler_theta(ang[1])
    B = Euler_psi(ang[2])
    temp_mat = mat_multiply(D, C, 3)
    temp_mat1 = mat_multiply(temp_mat, B, 3)
    p = Identity(4)
    for i in range(3):
        for j in range(3):
            p[i][j] = temp_mat1[i][j]
    #position
    for i in range(3):
        p[i][3] = vec[i] 
    return p

def mat_to_GlM(DH):
    p = []
    temp_d = 0.0
    for i in range(4):
        for j in range(4):
            temp_d = DH[j][i]
            p.append(temp_d)
    return p

def GlM_to_mat(GlM):
    m=Zeros(4)
    for i in range(4):
        for j in range(4):
            m[i][j] = GlM[j*4+i]
    return m

def draw_3axis(x, y, z, r=1.0, name=None):
    # clear color and depth buffers
    r25 = .25*r
    r75 = .75*r
    r11 = 1.1*r
    glPushMatrix()
    glTranslatef(x,y,z)
    glBegin(GL_LINE_STRIP)
    glColor3f(1.0, 0.0, 0.0)
    glVertex3f(0.0, 0.0,0.0)
    glVertex3f(r, 0.0, 0.0)
    glVertex3f(r75, r25, 0.0)
    glVertex3f(r75, -r25, 0.0)
    glVertex3f(r, 0.0, 0.0)
    glVertex3f(r75, 0.0, r25)
    glVertex3f(r75, 0.0, -r25)
    glVertex3f(r, 0.0, 0.0)
    glEnd()
    glColor3f(0.0, 1.0, 0.0)
    glBegin(GL_LINE_STRIP)
    glVertex3f(0.0, 0.0, 0.0)
    glVertex3f(0.0, r, 0.0)
    glVertex3f(0.0, r75, r25)
    glVertex3f(0.0, r75, -r25)
    glVertex3f(0.0, r, 0.0)
    glVertex3f(r25, r75, 0.0)
    glVertex3f(-r25, r75, 0.0)
    glVertex3f(0.0, r, 0.0)
    glEnd()
    glColor3f(0.0, 0.0, 1.0)
    #glColor3f(r, r, r)
    glBegin(GL_LINE_STRIP)
    glVertex3f(0.0, 0.0, 0.0)
    glVertex3f(0.0, 0.0, r)
    glVertex3f(r25, 0.0, r75)
    glVertex3f(-r25, 0.0, r75)
    glVertex3f(0.0, 0.0, r)
    glVertex3f(0.0, r25, r75)
    glVertex3f(0.0, -r25, r75)
    glVertex3f(0.0, 0.0, r)
    glEnd()
    glColor3f(1.0, 1.0, 1.0)

    #glColor3ub(255, 255, 0)
    #glRasterPos3f(r11, 0.0, 0.0)
    #glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ord("x"))
    #glRasterPos3f(0.0, r11, 0.0)
    #glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ord("y"))
    #glRasterPos3f(0.0, 0.0, r11)
    #glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ord("z"))    
    #glRasterPos3f(0.0, 0.0, 0.0)
    #if name != None:
    #    for ch in name:
    #        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ord(ch))    
    glPopMatrix()

def update_time_on_display(value, (x,y)):
    glMatrixMode(GL_PROJECTION)
    glPushMatrix()
    glLoadIdentity()
    glRasterPos2i(x,y)
    for char in value:
        if char == "\n":
            glRasterPos2i(x,y)
        else:
            glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ord(char))    
    glPopMatrix()
    glMatrixMode(GL_MODELVIEW)


def Unproject(x, y, z):
    mat_mv = glGetDoublev(GL_MODELVIEW_MATRIX)
    mat_pr = glGetDoublev(GL_PROJECTION_MATRIX)
    vport = glGetIntegerv(GL_VIEWPORT)
    xyz= gluUnProject(x, y, z, mat_mv, mat_pr, vport)
    return xyz

def Project(x, y, z):
    mat_mv = glGetDoublev(GL_MODELVIEW_MATRIX)
    mat_pr = glGetDoublev(GL_PROJECTION_MATRIX)
    vport = glGetIntegerv(GL_VIEWPORT)
    xyz= gluProject(x, y, z, mat_mv, mat_pr, vport)
    return xyz

def radius(rad=50.0):
    glPushMatrix()
    glLoadIdentity()
    xyz1 = Unproject(0.0, 0.0, .5)
    xyz2 = Unproject(rad, 0.0, .5)
    glPopMatrix()
    rad = 0.0
    temp = 0.0
    for i in range(3):
        temp = xyz2[i]-xyz1[i]
        rad = rad + temp*temp 
    rad = sqrt(rad)
    return rad


class od_pyshell(wx.Frame):
    def __init__(self, parent=None, id=-1, title="Command Window",
                 size=[1000, 500], pos=wx.DefaultPosition 
                 ):
        wx.Frame.__init__(self, parent, id, title, pos, size)
        win = py.shell.Shell(self, -1, introText="")
        btn = wx.Button(self, wx.ID_OK, " CLOSE ")
        box = wx.BoxSizer(wx.VERTICAL)
        box.Add(win, 1, wx.ALIGN_CENTRE|wx.ALL|wx.EXPAND, 5)
        box.Add(btn, 0, wx.ALIGN_CENTRE|wx.FIXED_MINSIZE, 5)
        self.Bind(wx.EVT_BUTTON, self.OnCloseMe, btn)
        self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)
        self.SetSizer(box)
        self.SetAutoLayout(True)
        

    def OnCloseMe(self, evt):
        self.Close(True)

    def OnCloseWindow(self, event):
        self.Destroy()

def print_svg_header():
    """header for SVG file"""
    head_str=[]
    head_str.append('<?xml version="1.0" standalone="no"?>\n')
    head_str.append('<!DOCTYPE svg SYSTEM "svg-19990812.dtd">\n')
    head_str.append('<!--\n')
    head_str.append('<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG August 1999//EN"\n')
    head_str.append('  "http://www.w3.org/Graphics/SVG/svg-19990812.dtd">\n')
    head_str.append('-->\n')
    return head_str

def is_free_joint(index, free_js):
    """determine if free joint"""
    for i in free_js:
        if (index-1) == i:
            return 1
    return 0

def create_branches(x, free_js, broken_bs, fname):
    """create branches"""
    branches = []
    temp = []
    for i in x:
        if i != -2:
            temp.append(i+1)
        else:
            branches.append(temp)
            temp = []
    tip_body=[]
    num = len(branches)
    for i in range(num-1):
        branch = branches[i]
        length = len(branch)
        branch_n = branches[i+1]
        length_n = len(branch_n)
        tip_body.append(branch[length-1])
        length = min(length, length_n)
        for j in range(length):
            if abs(branch[j]) == abs(branch_n[j]):
                branch_n[j] = -branch_n[j]
##     for i in range(num):
##         for j in branches[i]:
##             print "%2d"%j,
##         print '\n'
    max_len = 0
    for i in range(num):
        length = len(branches[i])
        max_len =max(length, max_len)

    max_len = max_len/2
    branches_info=[]
    for ii in range(len(branches)):
        branch_info = []
        br = branches[ii]
        length = len(br)/2
        for i in range(length):
            if br[2*i] > 0:
                temp=[(ii+1)*50, (i+1)*50, (br[2*i]), (br[2*i+1])]
            else:
                temp_br = branches_info[ii-1]
                temp = temp_br[i]
            branch_info.append(temp)
        branches_info.append(branch_info)
                
##     for br in branches_info:   
##         for b in br:
##             print b,
##         print '\n'
                                   
    width = (len(branches)+1)*50
    if width < 200:
        width = 200
    height = (max_len+3)*50
    body=[]
    header = print_svg_header()
    for line in header:
        body.append(line)
    body.append('<svg width=\"%d\" height=\"%d\">\n'%(width, height))
    string_ = "<title> System Topology </title>\n"
    body.append(string_)

    last_x = width/2
    last_y = height-10
    string_ = '<circle cx=\"%d\" cy=\"%d\" r=\"%d\" style=\"fill:red; stroke:navy; stroke-width:2\" />\n'%(last_x, last_y, 8)
    body.append(string_)
    string_ = '<text style=\"font-size: 14; fill: black; stroke-width: 1\" x=\"%d\" y=\"%d\">%s</text>\n'%(last_x+10, height-5, 'ground')
    body.append(string_)
    for br in branches_info:
        last_x = width/2
        last_y = height-10
        for b in br:
            #drawline
            if is_free_joint(b[2], free_js):
                string_ = '<line x1=\"%d\" x2=\"%d\" y1=\"%d\" y2=\"%d\" style=\"stroke: yellow; stroke-width: 2\" />\n'%(last_x, b[0], last_y, height-b[1])
            else:
                string_ = '<line x1=\"%d\" x2=\"%d\" y1=\"%d\" y2=\"%d\" style=\"stroke: blue; stroke-width: 2\" />\n'%(last_x, b[0], last_y, height-b[1])
            body.append(string_)
            string_ = '<circle cx=\"%d\" cy=\"%d\" r=\"%d\" style=\"fill:red; stroke:navy; stroke-width:2\" />\n'%(b[0], height-b[1], 5)
            body.append(string_)
            string_ = '<text style=\"fontfrom odsystem import *-size: 14; fill: black; stroke-width: 1\" x=\"%d\" y=\"%d\">%s</text>\n'%((last_x+b[0])/2+3, (last_y+height-b[1])/2, 'j'+repr(b[2]))
            body.append(string_)
            string_ = '<text style=\"font-size: 14; fill: black; stroke-width: 1\" x=\"%d\" y=\"%d\">%s</text>\n'%(b[0]+5, height-b[1], 'b'+repr(b[3]))
            body.append(string_)
            last_x = b[0]
            last_y = height-b[1]
    #print len(broken_bs)
    for i in range(len(broken_bs)/2):
        b1 = broken_bs[2*i]+1
        b2 = broken_bs[2*i+1]+1
        string_ = '<text style=\"font-size: 14; fill: black; stroke-width: 1\" x=\"%d\" y=\"%d\">b%d is connected to b%d</text>\n'%(0, (i+1)*20, b1, b2  )
#        print string_
        body.append(string_)
    str="</svg>\n"
    body.append(str)
    fid = open(fname, "w")
    fid.writelines(body)
    fid.close()
        

def print_permu(perm):
    print "permutation Vector"
    for x in perm:
      print '%2d'%(x),
    print '\n'
def print_mats(n_b, n_j, inc, rel, perm_b, perm_j, topb):
    print "permutation vector"+"\n"
    for x in perm_b:
        print '%2d'%(x+1),
    print '-->',
    for x in perm_j:
        print '%2d'%(x+1),
    print '\n'
    for i in range(n_j):
        for j in range(n_b):
            x = inc[i*n_b+j]
            print '%2d'%(x),
        print '-->',
        temp_int = n_j
        for j in range(temp_int):
            x = rel[i*temp_int+j]
            print '%2d'%(x),
        print '\n'
    for x in topb:
        if (x+1) == -1:
            print  "\n" 
        else:
            print '%2d'%(x+1),
    print '\n'
    
def DX(I, J, K=0):
    return 1.0
  
def DY(I, J, K=0):
    return 1.0
  
def DZ(I, J, K=0):
    return 1.0
  
def DM(i, J, K=0):
     return 1.0

def JOINTD(i, j=0):
     return 1.0

def JOINTV(i, j=0):
     return 1.0

def JOINTA(i, j=0):
     return 1.0


def cross_product(x, y):
      z=[]
      z.append( x[1]*y[2]-x[2]*y[1])
      z.append(x[2]*y[0]-x[0]*y[2])
      z.append( x[0]*y[1]-x[1]*y[0])
      return z

def dot_product(x, y):
     return sqrt(x[0]*y[0]+x[1]*y[1]+x[2]*y[2])
  
def Frame(z):
     xyz=[[1.0, 0.0,0.0],[0.0, 1.0, 0.0],[0.0, 0.0,1.0]]
     i = 0
     for ii in range(3):
      if abs(dot_product(xyz[ii], z)) < 1/sqrt(3):
         i =  ii;
     x = cross_product(xyz[i],  z)
     mag = dot_product(x,x)
     for i in range(3):
       x[i] = x[i]/mag
     y = cross_product(z,x)
     xyz=[x,y,z]
     return xyz

class Mat33:
  def __init__(self, a1_=0.0, a2=0.0, a3=0.0, radian=None):
     if radian is None: _pi = pi
     else: _pi = 180.0
     if type(a1_) is ListType:
       a1 = a1_[0]*_pi/180.0
       a2 = a1_[1]*_pi/180.0
       a3 = a1_[2]*_pi/180.0
     else:
       a1 = a1_* _pi/180.0
       a2 = a2*_pi/180.0
       a3 = a3*_pi/180.0
     c1 = cos(a1)
     s1 = sin(a1)
     d=[[c1, -s1, 0.0], [s1, c1, 0.0], [0.0, 0.0, 1.0]]
     c2 = cos(a2)
     s2 = sin(a2)
     c=[[1, 0.0, 0.0], [0.0, c2, -s2], [0.0, s2, c2]]
     c1 = cos(a3)
     s1 = sin(a3)
     b=[[c1, -s1, 0.0], [s1, c1, 0.0], [0.0, 0.0, 1.0]]
     dc = mat_multiply(d, c,3)
     dcb = mat_multiply(dc,b,3)
     self.data=dcb
     self.smallValue = 1.0e-7

  def  __mul__ (self, another): return self.multiply(another)
  def __setitem__(self,key,value): self.data[key] = value
  def __getitem__(self,key):       return self.data[key]
  def __repr__(self):
    _str=''
    idx=range(3)
    for i in idx:
      for j in idx:
        _str = _str+str(self.data[i][j])+"  "
      _str = _str+"\n"
    return _str

  def _print(self):
    print self.__str__()
        

  def assignColumns(self, mat):
     for i in [0,1,2]:
       for j in [0,1,2]: 
         self.data[i][j]=mat[j][i]

  def assign(self, v9):
     for i in [0,1,2]:
       self.data[i][0] = v9[i*3]
       self.data[i][1] = v9[i*3+1]
       self.data[i][2] = v9[i*3+2]

  def invert(self):
     mat33Temp = Mat33()
     _range=range(3)
     for i in _range:
       for j in _range:
         mat33Temp.data[i][j] = self.data[j][i]
     return mat33Temp
  
  def multiply(self, another):
     _res  = Mat33()
     res = mat_multiply(self.data, another.data, 3)
     _res[0] = res[0]
     _res[1] = res[1]
     _res[2] = res[2]
     return _res
  
  def wrt(self, another):
     if another != None:
       mat33Temp = another.invert()*self
       return mat33Temp
     else:  
       return self
  
  def transform(self,  vec):
     _range = range(3)
     _vec=[0.0,0.0,0.0]
     for i in _range:
       for j in _range:
         _vec[i] = _vec[i]+self.data[i][j]*vec[j]
     return _vec
  
  def euler_angles(self):
     a = [0.0, 0.0, 0.0]
     sin2 = sqrt(1-self.data[2][2]*self.data[2][2])
     if sin2 < self.smallValue:
        a[1]=a[2]=0.0
        a[0]= atan2(self.data[1][0], self.data[0][0])
     else:
        a[0] = atan2(self.data[0][2], -self.data[1][2])
        a[2] = atan2(self.data[2][0],  self.data[2][1])
        if abs(sin(a[2])) > abs(cos(a[2])):
          sin_theta = self.data[2][0]/sin(a[2])
        else:
          sin_theta = self.data[2][1]/cos(a[2])
        a[1] = atan2(sin_theta, self.data[2][2])
     return [180.0/pi*x for x in a]
  

def fromMattoAngles(mat, _wrt=None):
   _m33 = mat.wrt(_wrt)
   angles= _m33.euler_angles()
   return angles

def fromZtoAngles(z, _wrt=None):   
   mat = Frame(z)
   m33=Mat33()
   m33.assignColumns(mat)
   return fromMattoAngles(m33, _wrt)

def fromAnglestoAngles(z, _wrt=None):   
   mat = Mat33(z) 
   return fromMattoAngles(mat, _wrt)

def Mag(vec):
   temp = vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]     
   return sqrt(temp)
