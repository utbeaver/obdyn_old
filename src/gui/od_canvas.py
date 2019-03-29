from OpenGL.GL import *
import wx
from wx import glcanvas
from od_geom import *
from od_model import *
from math import *
from od_utils import *


class od_canvas(glcanvas.GLCanvas):
    def __init__(self, parent):
        wx.glcanvas.GLCanvas.__init__(self, parent, -1)
        self.dynamic_rotation = 0
        self.dynamic_translation = 0
        self.dynamic_zoom = 0
        self.zoom = 0
        self.x = self.lastx = 0
        self.y = self.lasty = 0
        self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)
        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_LEFT_DOWN, self.OnMouseLeftDown)
        self.Bind(wx.EVT_RIGHT_DOWN, self.OnRightClick)
        self.Bind(wx.EVT_LEFT_UP, self.OnMouseUp)
        self.Bind(wx.EVT_MOTION, self.OnMouseMotion)
        self.model = None
        self.first_motion = 1
        # set viewing projection
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        self.bx0=0.0
        self.by0=0.0
        self.bx=0.0
        self.by=0.0
        self.left=-1.0
        self.right=1.0
        self.bottom=-1.
        self.top=1.0
        self.near=-300.0
        self.far=300.0
        self._left=-1.0
        self._right=1.0
        self._bottom=-1.0
        self._top=1.0
        self._near=-300.0
        self._far=300.0
        self.counter = 0
        self.ratio = 1.0
        self.oldH=None
        glOrtho(self.left, self.right, self.bottom, self.top, self.near, self.far)

        # position viewer
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()
        glTranslatef(0.0, 0.0, 0.0)

        # position object
        glRotatef(self.y, 1.0, 0.0, 0.0)
        glRotatef(self.x, 0.0, 1.0, 0.0)

        glEnable(GL_DEPTH_TEST)
#        glEnable(GL_LIGHTING)
        glEnable(GL_LIGHT0)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

    def SetModel(self, _model):
        self.model = _model

    def OnSize(self, event):
        size = self.GetClientSize()
        self.height=size.height

        self.width=size.width
        if self.height == 0 or self.width == 0:
            return
        if self.GetContext():
            aspect = float(self.width)/float(self.height)
            ratio_l = self.left/(self.right-self.left)
            ratio_r = self.right/(self.right-self.left)
            len_ = (self.top-self.bottom)*aspect
            self.left = len_*ratio_l
            self.right = len_*ratio_r
            self.SetCurrent()
            glViewport(0, 0, size.width, size.height)
            glMatrixMode(GL_PROJECTION)
            glLoadIdentity()
            glOrtho(self.left, self.right, self.bottom, self.top, self.near, self.far)
            glMatrixMode(GL_MODELVIEW)
        event.Skip()

    def draw_all(self, replay=None):
        self.SetCurrent()
        glMatrixMode(GL_MODELVIEW)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        if self.model != None:
            self.model.draw(replay)
        rad = radius()
        self.OnDraw(0.0, 0.0, 0.0, rad)
        xyz = Unproject(50.0, 50.0, 0.5)
        self.OnDraw(xyz[0], xyz[1], xyz[2], rad)
        self.SwapBuffers()      

    def OnPaint(self, event=None):
        dc =wx.PaintDC(self)
        self.draw_all()

    def OnEraseBackground(self, event):
        pass # Do nothing, to avoid flashing on MSW.
        
    def OnMouseLeftDown(self, evt):
        #self.CaptureMouse()
        if self.zoom == 1:
            glEnable(GL_COLOR_LOGIC_OP)
            glLogicOp(GL_XOR)
            self.x, self.y = evt.GetPosition()
            #from mouse system to window system
            _x = self.x
            _y=self.height-self.y

            wx = (self.right-self.left)/self.width*_x
            wy = (self.top-self.bottom)/self.height*_y

            self.bx0 = wx+self.left
            self.by0 =wy+self.bottom
        
    def OnMouseRightDown(self, evt):
        pass #self.CaptureMouse()

    def OnMouseUp(self, evt):
        #self.ReleaseMouse()
        if self.dynamic_rotation == 1: 
             self.dynamic_rotation = 0
        elif self.dynamic_translation == 1:
             self.left = self.left_
             self.right = self.right_
             self.bottom = self.bottom_
             self.top = self.top_
             self.dynamic_translation = 0
             
        elif self.zoom == 1:
             self.zoom = 0
             glDisable(GL_COLOR_LOGIC_OP)
             lowx = min(self.bx0, self.bx)
             upx = max(self.bx0, self.bx)
             lowy = min(self.by0, self.by)
             upy = max(self.by0, self.by)
             rat_v = float(self.height)/float(self.width)
             rat_w = (upy-lowy)/(upx-lowx)
             if (rat_v > rat_w):  #window is fat so increase its height
                    wheight = (upx-lowx)*rat_v
                    delta = wheight-(upy-lowy)
                    upy = upy+delta/2.0
                    lowy=lowy-delta/2.0
             else:                        #window is high so increate is width
                    wwidth = (upy-lowy)/rat_v
                    delta=wwidth-(upx-lowx)
                    upx = upx+delta/2.0
                    lowx=lowx-delta/2.0
             glMatrixMode(GL_PROJECTION)
             glLoadIdentity()
             glOrtho(lowx, upx, lowy, upy, self._near, self._far)
             self.left = lowx
             self.right = upx
             self.bottom = lowy
             self.top = upy
             glMatrixMode(GL_MODELVIEW)
                    
        self.first_motion = 1
        self.Refresh(True)


    def OnMouseMotion(self, evt):
        temp = self.dynamic_rotation or self.dynamic_translation or self.zoom or self.dynamic_zoom
        if temp == 0:
            return
        if evt.Dragging():
            self.x, self.y = evt.GetPosition()
            _x = self.x
            _y = self.y
            self.y = self.height/2.0 -self.y
            self.x = -self.width/2.0 +self.x
            if evt.LeftIsDown() and not evt.RightIsDown():
                if self.first_motion == 1:
                    self.lastx, self.lasty = self.x, self.y
                    self.first_motion = 0
                else:
                    deltax = self.x - self.lastx
                    deltay = self.y - self.lasty
                    length = deltax*deltax + deltay*deltay
                    if length == 0:
                        return
                    length = sqrt(length)
                    #moton direction
                    deltax = float(deltax)/float(length)
                    deltay = float(deltay)/float(length)
                    if self.dynamic_rotation == 1:
                        #rotation axis
                        mat_mv = glGetDoublev(GL_MODELVIEW_MATRIX)
                        glLoadIdentity()
                        glRotatef(self.mod_360(length*.3), -deltay, deltax, 0.0)
                        glMultMatrixd(mat_mv)
                    elif self.dynamic_translation == 1:
                        dx = float(self.x)/float(self.width)*(self.right-self.left)
                        dy = float(self.y)/float(self.height)*(self.top-self.bottom)
                        self.left_ = self.left-dx
                        self.right_ = self.right-dx
                        self.bottom_ = self.bottom-dy
                        self.top_ = self.top-dy
                        glMatrixMode(GL_PROJECTION)
                        glLoadIdentity()
                        glOrtho(self.left_, self.right_, self.bottom_,
                                self.top_, self._near, self._far)
                        glMatrixMode(GL_MODELVIEW)
                    elif self.zoom == 1:
                        self.counter = self.counter +1
                        if self.counter == 2:
                            _y=self.height-_y
                            wx = (self.right-self.left)/self.width*_x
                            wy = (self.top-self.bottom)/self.height*_y
                            self.bx = wx+self.left
                            self.by =wy+self.bottom
                            self. DrawRubberBox()
                            self.SwapBuffers()
                            self. DrawRubberBox()
                            self.counter = 0
                    elif self.dynamic_zoom == 1:
                         dx = self.y-self.lasty   
                         if abs(dx) > 1:
                           if dx > 0:
                              self.left = self.left*1.1 
                              self.right = self.right*1.1 
                              self.top = self.top*1.1 
                              self.bottom = self.bottom*1.1 
                           else:   
                              self.left = self.left*0.9 
                              self.right = self.right*0.9 
                              self.top = self.top*0.9 
                              self.bottom = self.bottom*0.9 
                           glMatrixMode(GL_PROJECTION)
                           glLoadIdentity()
                           glOrtho(self.left, self.right, self.bottom, self.top, self.near, self.far)
                           glMatrixMode(GL_MODELVIEW)

                    self.lastx, self.lasty = self.x, self.y
            elif evt.LeftIsDown() and evt.RightIsDown():
                if self.dynamic_rotation == 1:
                    ratioy = 2.0*float(self.y)/float(self.height)
                    theta = 180.0 *ratioy
                    glMatrixMode(GL_MODELVIEW)
                    mat_mv = glGetDoublev(GL_MODELVIEW_MATRIX)
                    glLoadIdentity()
                    glRotatef(theta*.1, 0.0, 0.0, 1.0)
                    glMultMatrixd(mat_mv)
                self.lastx, self.lasty = self.x, self.y
        self.Refresh(True)

    def OnRightClick(self, evt):
        if not hasattr(self, "DynamicRotation"):
            self.DynamicRotation =wx.NewId()
            self.Bind(wx.EVT_MENU, self.SetRotation, id=self.DynamicRotation)
        if not hasattr(self, "DynamicTranslation"):
            self.DynamicTranslation =wx.NewId()
            self.Bind(wx.EVT_MENU, self.SetTranslation, id=self.DynamicTranslation)
        if not hasattr(self, "DynamicZoom"):
            self.DynamicZoom =wx.NewId()
            self.Bind(wx.EVT_MENU, self.SetDynamicZoom, id=self.DynamicZoom)
        if not hasattr(self, "Zoom"):
            self.Zoom =wx.NewId()
            self.Bind(wx.EVT_MENU, self.SetZoom, id=self.Zoom)
        if not hasattr(self, "Reset"):
            self.Reset =wx.NewId()
            self.Bind(wx.EVT_MENU, self.ReSet, id=self.Reset)
        menu=wx.Menu()
        item = wx.MenuItem(menu, self.DynamicRotation, "Rotation")
        menu.AppendItem(item)
        item = wx.MenuItem(menu, self.DynamicTranslation, "Translation")
        menu.AppendItem(item)
        item = wx.MenuItem(menu, self.Zoom, "Zoom")
        menu.AppendItem(item)
        item = wx.MenuItem(menu, self.DynamicZoom, "DynamicZoom")
        menu.AppendItem(item)
        item = wx.MenuItem(menu, self.Reset, "Reset")
        menu.AppendItem(item)
        self.PopupMenu(menu, evt.GetPosition())
        menu.Destroy()

         
    def SetRotation(self, evt):
        self.dynamic_rotation = 1
        self.dynamic_translation = 0
        self.zoom = 0
        self.dynamic_zoom = 0
         
    def SetTranslation(self, evt):
        self.dynamic_rotation = 0
        self.dynamic_translation = 1
        self.zoom = 0
        self.dynamic_zoom = 0
        
    def SetZoom(self, evt):
        self.dynamic_rotation = 0
        self.dynamic_translation = 0
        self.zoom = 1
        self.dynamic_zoom = 0

    def SetDynamicZoom(self, evt):
        self.dynamic_rotation = 0
        self.dynamic_translation = 0
        self.zoom = 0
        self.dynamic_zoom = 1

    def ReSet(self, evt):
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        glOrtho(self._left, self._right, self._bottom,self._top,self._near,self._far)
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()
        self.Refresh(True)

    def mod_360(self, value):
        if value > 360.0:
            value = value - 360.0
        return value
    
    def OnDraw(self, x=0.0, y=0.0, z=0.0, r=1.0):
        draw_3axis(x,y,z,r)





    def DrawRubberBox(self):
        glMatrixMode(GL_MODELVIEW)
        glPushMatrix()
        glLoadIdentity()
        glColor3d(1.0,1.0,0.0)
        glEnable(GL_LINE_STIPPLE)
        glLineStipple(2, 0x1c47)	
        glBegin(GL_LINE_LOOP)			
        glVertex3f(self.bx0, self.by0, 0.0)
        glVertex3f(self.bx,  self.by0, 0.0)
        glVertex3f(self.bx,  self.by,   0.0)
        glVertex3f(self.bx0, self.by,  0.0)	
        glEnd()				
        glDisable(GL_LINE_STIPPLE)
        glPopMatrix()


        
