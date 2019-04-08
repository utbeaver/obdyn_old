#This file defines thel classes for body, constraint, marker and model.
from math import *
from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *
from OpenGL.GLE import *
from od_utils import *
from string import *
from types import *
import sys
import wx
import time
from od_utils import *
import wx.lib.rcsizer as rcs
from random import *
import xml.dom.minidom
from xml.dom.minidom import Node
from sets import *
sys.path.append(os.getcwd())
from odsystem import *

def printTree(_tree, n=0):
    s = n*"   "    
    print s,  _tree[0]    
    print s,  "Joint"    
    for j in _tree[1]:
     print s+"   ",  j.name()    
    print s,  "Body"    
    for b in _tree[2]:
     print s+"   ",  b.name()    
    for t in _tree[3]: 
     printTree(t, n+2)       
              

zeros = [0.0, 0.0, 0.0]
class od_TextCtrl(wx.TextCtrl):
    def __init__(self,  parent, id, value, t_ype):
        wx.TextCtrl.__init__(self, parent, id, value, size=(450, -1))
        self.func_type = t_ype
        
    def get_type(self):
        return self.func_type
            
class od_dlg(wx.Dialog):
    def __init__(self, entity, attrs, odsystem, title, parent=None, id=-1, 
                 size=wx.DefaultSize, posi=wx.DefaultPosition, 
                 style=wx.DEFAULT_DIALOG_STYLE):
        wx.Dialog.__init__(self, parent, id, title, posi, size, style)
        sub_lables={
                    "Gravity":  ["X","Y","Z"],
                    "Position": ["X","Y","Z"],
                    "Omega":    ["X","Y","Z"],
                    "Velocity": ["X","Y","Z"],
                    "Orientation": ["Phi","Theta","Psi"],
                    "Kt": ["Translational Stiffness X", 
                           "Translational Stiffness Y", 
                           "Translational Stiffness Z"],   
                    "Ct": ["Translational Damping X", 
                           "Translational Damping Y", 
                           "Translational Damping Z"],   
                    "Kr": ["Rotational Stiffness X", 
                           "Rotational Stiffness Y", 
                           "Rotational Stiffness Z"],   
                    "Cr": ["Rotational Damping X", 
                           "Rotational Damping Y", 
                           "Rotational Damping Z"],   
                    "F": ["Preloaded Force X", 
                          "Preloaded Force Y", 
                          "Preloaded Force Z"],   
                    "T": ["Preloaded Torque X", 
                          "Preloaded Torque Y", 
                          "Preloaded Torque Z"]   
                    }
        self.create_ids={}
        self.system = odsystem
        self.entity=entity
        self.attrs=attrs
        self.field_bank={}
        sizer = wx.FlexGridSizer(0,2,0,0)
        for key in self.attrs:
            label = None
            vals, type = self.entity.get_data(key)
            if type is "str" or type is "float" or type is "od_function" or type is "Integer":
                label = wx.StaticText(self, -1, key)
                if type is "float" or type is "Integer":
                    vals = str(vals)
                if vals is None:
                    vals = ""
                if type is "od_function":
                   val_field = od_TextCtrl(self, -1, vals, type)
                else:
                   val_field = wx.TextCtrl(self, -1, vals)
                content = val_field
                self.field_bank[key] = val_field
                if type is "od_function":
                   self.field_bank[key] .Bind(wx.EVT_RIGHT_UP,  self.AddFunc)
            elif type is "vec3":
                label = wx.StaticText(self, -1, key)
                rcsizer = rcs.RowColSizer()
                content = rcsizer
                sub_title=sub_lables[key]
                for i in range(3):
                    rcsizer.Add(wx.StaticText(self, -1, sub_title[i]+"  "), row=1, col=i+1, flag=wx.ALIGN_LEFT|wx.LEFT)
                    if vals[i] is None:
                      tempStr = ""
                    else:  
                      tempStr = str(vals[i]) 
                    val_field = wx.TextCtrl(self, -1, tempStr)
                    self.field_bank[key+str(i)] = val_field
                    rcsizer.Add(val_field,  row=2, col=i+1,flag=wx.ALIGN_LEFT|wx.LEFT)
            elif type is "mat3":
                label = wx.StaticText(self, -1, key)
                rcsizer = rcs.RowColSizer()
                content = rcsizer
                xyz=['x', 'y', 'z']
                for i in range(3):
                    rcsizer.Add(wx.StaticText(self, -1, xyz[i]), row=1, col=i+2, flag=wx.ALIGN_LEFT|wx.LEFT)
                    rcsizer.Add(wx.StaticText(self, -1, xyz[i]), row=i+2, col=1, flag=wx.ALIGN_LEFT|wx.LEFT)
                    for j in range(0, i+1):
                        if vals[i*3+j] is None:
                          tempStr = ""
                        else:  
                          tempStr = str(vals[i*3+j]) 
                        val_field = wx.TextCtrl(self, -1, tempStr)
                        self.field_bank[key+str(i)+str(j)] = val_field
                        rcsizer.Add(val_field,  row=i+2, col=j+2, flag=wx.ALIGN_LEFT|wx.LEFT)
            elif type is 'marker_ref':
                mar_pair = self.system.get_marker_pair()
                key_list= mar_pair.keys()
                key_list.sort()
                key_list.insert(0, "Select a marker from the list.  0 is for marker of origin")
                label = wx.StaticText(self, -1, "Select "+key)
                lb1 = wx.Choice(self, -1, (100, 50), choices=key_list)
                if vals is None: lb1.SetSelection(0)
                else: lb1.SetStringSelection(vals.composite_name())
                content = lb1
                self.field_bank[key] = lb1
            elif type is 'joint_ref':
                j_pair = self.system.get_joint_pair()
                key_list= j_pair.keys()
                key_list.sort()
                key_list.insert(0, "Select a Joint from the list.")
                #print key_list;
                label = wx.StaticText(self, -1, "Select "+key)
                lb1 = wx.Choice(self, -1, (100, 50), choices=key_list)
                _name_str= vals.name()
                if vals is None: lb1.SetSelection(0)
                else: 
                  lb1.SetStringSelection(_name_str)
                content = lb1
                self.field_bank[key] = lb1
            if label is not None:
                sizer.Add(label, 0, wx.ALIGN_LEFT|wx.LEFT|wx.EXPAND, 5 )
                sizer.Add(content, 0, wx.ALIGN_LEFT|wx.LEFT|wx.EXPAND, 5 )

        btn = wx.Button(self, wx.ID_OK, " OK ")
        btn.SetDefault()
        btn.SetHelpText("The OK button completes the dialog")
        sizer.Add(btn, 0, wx.ALIGN_LEFT|wx.LEFT|wx.EXPAND, 5 )
        btn = wx.Button(self, wx.ID_CANCEL, " Cancel ")
        btn.SetHelpText("The Cancel button cnacels the dialog. (Cool, huh?)")
        sizer.Add(btn, 0, wx.ALIGN_CENTER, 5 )

        self.SetSizer(sizer)
        self.SetAutoLayout(True)
        sizer.Fit(self)

    def OnOk(self):
        for key in self.attrs:
          vals, type = self.entity.get_data(key)
          if type is "str" or type is "float" or type is "od_function" or type is "Integer":
                temp = self.field_bank[key].GetValue()
                if type is "float":
                    temp = eval(temp)
                elif type is "Integer":
                    temp = int(temp)
                elif  type is "od_function" and temp == "":
                    temp=None
                self.entity.set(key, temp)
          elif type is "vec3":
                list_vals=[]
                for i in range(3):
                    fie=self.field_bank[key+str(i)]
                    temp = fie.GetValue()
                    if temp is "": temp=None
                    else: temp = eval(temp)
                    list_vals.append(temp)
                self.entity.set(key, list_vals)
          elif type is "mat3":
                list_vals=[]
                for i in range(9):
                    list_vals.append(0.0)
                for i in range(3):
                    for j in range(3):
                        if j <= i:
                            temp = self.field_bank[key+str(i)+str(j)]
                            temp = temp.GetValue()
                            temp = eval(temp)
                            list_vals[i*3+j]=temp
                for i in range(3):
                    for j in range(3):
                        if j > i:
                            list_vals[i*3+j]=list_vals[j*3+i]
                self.entity.set(key, list_vals)
          elif type is "marker_ref":
                temp = self.field_bank[key]
                temp = temp.GetStringSelection()
                mar_pair = self.system.get_marker_pair()
                temp = mar_pair[temp]
                self.entity.set(key, temp)
          elif type is "joint_ref":
                temp = self.field_bank[key]
                temp = temp.GetStringSelection()
                mar_pair = self.system.get_joint_pair()
                temp = mar_pair[temp]
                self.entity.set(key, temp)
        if hasattr(self.entity, "initialize"):
           self.entity.initialize(self.system); 

    def AddFunc(self, event):
          self.od_text_field = event.GetEventObject()
          func_type = self.od_text_field.get_type()
          function = func_type+"()"
          obj = eval(function)
          self.sub_types = obj.get_subtype()
          obj=None
          #popup menu
          menu = wx.Menu()
          for sub_type in self.sub_types:
            if self.create_ids.has_key(sub_type) == False:
              self.create_ids[sub_type] = wx.NewId()
            self.Bind(wx.EVT_MENU, self.OnExpr, id=self.create_ids[sub_type])
            menu.Append(self.create_ids[sub_type],  sub_type)
          self.PopupMenu(menu, event.GetPosition())
          menu.Destroy()         
        
    def OnExpr(self, event):
          id = event.GetId()
          for sub_type in self.sub_types:
                if self.create_ids[sub_type] == id:
                    od_type = sub_type
          #creation 
          obj=od_type+"()"
          obj = eval(obj)
          dlg = od_dlg(obj, obj.get_editable(), self.system, "Creating "+obj.get_type())
          if dlg.ShowModal() == wx.ID_OK:
                dlg.OnOk()
          else:
                obj = None
          if obj is not None:
               __str = str(obj)
               self.od_text_field.AppendText(__str)
              


def typeMap(key):
   typeMap_ = {
     'Omega':         'vec3',
     'Velocity':      'vec3',
     'Position':      'vec3',
     'Orientation':   'vec3',
     'WrtBCS':        'Integer',
     'last':          None
   }
   if typeMap_.has_key(key):
     return typeMap_[key]
   else:
     return None

def vec3(vals):

    if type(vals) is ListType:
       temp_str = ""
       for val in vals: temp_str = temp_str + " "+str(val)
    else:
       vals = string.split(vals)
       temp_str=[]
       for val in vals: temp_str.append(float(val))
    return temp_str

class od_entity:
    def __init__(self, _name="model"):
        self.datas={}
        self.sub_type=[]
        self.sub_types={}
        self.displayable=[]
        self.editable=[]
        self.plotable=[]
        self.sequence=[]
        self.save_need = True
        self.editable.append('Name')
        self.sequence.append('Name')
        self.datas['type'] = [None, 'str']; 
        self.datas['Name'] = [_name, 'str']; 
        self.datas['Dady'] = [None, 'obj'];
        self.tagged = None

    def isTagged(self):
        return self.tagged

    def tag(self):
        self.tagged = (1==1)    

    def untag(self):
        self.tagged = (0==1)    

    def get_subtype(self, key=None):
        if key is None:
            return self.sub_type
        else:
            return self.sub_types[key]
    
    def get_plotable(self):
        return self.plotable;
    
    def get_displayable(self):
        return self.displayable;
    
    def get_editable(self):
        return self.editable;
    
    def composite_name(self):
        name_ = self.get("Dady").name()+"."+self.name()
        return name_
    
    def has_key(self, key):
        return self.datas.has_key(key)
    
    def get_data(self, key):
        if self.datas.has_key(key):
          return self.datas[key]
        else:
          #print self.__class__.__name__, " has no key ", key;
          return None
    
    def get(self, name_str):
        temp = self.get_data(name_str)
        if temp != None:
          return temp[0]
        else:  
          return None
        
    def clear(self, name_str):
        self.datas[name_str]=[[], 'List']

    def set(self, name_str, val):
        self.save_need = True
        _type_ = None
        if self.datas.has_key(name_str):
          temp_name = self.datas[name_str]
          _type_ = temp_name[1]
        else:
          _type_ = typeMap(name_str)
        self.datas[name_str] = [val, _type_]
        

    def add(self, name_str, val):
        self.save_need = True
        temp = self.get_data(name_str)
        temp[0].append(val)

    def name(self):
        return self.get('Name')
        
    def set_name(self, _name):
        self.set('Name', _name)

    def modify(self, dict):
        for key in dict.keys():
            self.set(key, dict[key])

    def get_type(self):
        return self.get("type")
        
    def set_type(self, _type):
        self.set("type",  _type)

    def xml_item(self, n, tag, val):
        temp_str = n*" "+"<%s>%s</%s>\n"%(tag, val, tag)
        return temp_str

    def to_xml(self, fid, n=0):
        n1 = n+1
        fid.write(n*" "+"<%s>\n"%self.__class__.__name__)
        keys = self.sequence 
        for key in keys:
           obj, type = self.datas[key]
           if obj is None: continue
           if type is "vec3" or type is "mat3" or type is "marker_ref" or type is "joint_ref":
              temp_str=self.xml_item(n1, key, str(obj))
              fid.write(temp_str)
           elif type is "List":
              for _obj in obj:
                 _obj.to_xml(fid, n1)
           else:
              temp_str=self.xml_item(n1, key, str(obj))
              fid.write(temp_str)
        fid.write(n*" "+"</%s>\n"%self.__class__.__name__)


class od_function(od_entity):
    def __init__(self, _name="function"):
        od_entity.__init__(self, _name)
        self.set_type("function")
        self.sub_type.append("od_DX")
        self.sub_type.append("od_DY")
        self.sub_type.append("od_DZ")
        self.sub_type.append("od_STEP")


    def measure_str(self):
        temp_str =  self.get("function_type") + "(" + self.get("I_marker").composite_name()+", " + self.get("J_marker").composite_name()
        if self.get("K_marker") is not None:
           temp_str = temp_str + ", " +   self.get("I_marker").composite_name()
        temp_str = temp_str + ")"
        return temp_str

class od_STEP(od_function):
   def __init__(self, _name="STEP"):
        od_function.__init__(self, _name)
        self.datas["X"]=["TIME", "od_function"]
        self.datas["X0"]=[0.0, "float"]        
        self.datas["H0"]=[0.0, "float"]        
        self.datas["X1"]=[0.0, "float"]        
        self.datas["H1"]=[0.0, "float"]
        self.datas["function_type"] = ["STEP",  'str']

   def __str__(self):
        temp_str =  self.measure_str()
        return temp_str

class od_object(od_entity):
    def __init__(self, _name="joint"):
        od_entity.__init__(self, _name)
        self.id = 0
        self.DisplayListId = -1
        self.Core = None
        self.shape =None
        self.rad = 20
        self.selected = 0


    def __del__(self):
        self.Core=None

    def select(self):
        self.selected = 1

    def unselect(self):
        self.selected = 0

    def set_id(self, id_):
        if self.id < 0: return    
        self.id = id_
        
    def Id(self):
        return self.id

    def core(self):
        return self.Core

    def set_core(self, _core):
        self.Core = _core

    def draw_shape(self):
        geoms = self.geometry()
        for g in geoms:
            g.draw()

    def geometry(self):
        return self.get("Geometry")

    def add_geometry(self, _od_shape): 
        self.add("Geometry", _od_shape)

    def reset_icon(self, rad1):
        self.rad = rad1

class od_shape(od_entity):
    def __init__(self, _name="shape"):
        od_entity.__init__(self, _name)
        self.flag = 0
        self.id = glGenLists(1)
        self.sub_type.append("od_cylinder")
        self.sub_type.append("od_link")
        self.sub_type.append("od_box")
        self.set_type("shape")


    def draw(self):
        if glIsList(self.id):
          glCallList(self.id)

    def __del__(self):
        if glIsList(self.id):
            glDeleteLists(self.id, 1)

    def set(self, name_str, val):
        od_entity.set(self, name_str, val)
        if hasattr(self, "init") and self.flag == 1:
            self.init()
    
class od_cylinder(od_shape):
    def __init__(self, rad=1.0, top=1.0, height=1.0, name= "cylinder", slices=10, stacks=2):
        od_shape.__init__(self, name)
        attributes={"Radius":[rad, "float"], 
                    "Height":[height, "float"], 
                    "Slices":[slices, "Integer"], 
                    "Stacks":[stacks, "Integer"]}
        for k, v in attributes.iteritems():
          self.displayable.append(k)
          self.editable.append(k) 
          self.sequence.append(k) 
          self.datas[k]=v
        #self.displayable.append("Radius")
        #self.displayable.append("Height")
        #self.displayable.append("Slices") 
        #self.displayable.append("Stacks") 
        #self.editable.append("Radius") 
        #self.editable.append("Height") 
        #self.editable.append("Slices") 
        #self.editable.append("Stacks") 
        #self.sequence.append("Radius") 
        #self.sequence.append("Height") 
        #self.sequence.append("Slices") 
        #self.sequence.append("Stacks") 
        #self.datas['Radius'] = [rad, "float"]
        #self.datas['Height'] = [height, "float"]
        #self.datas['Slices'] = [slices, "Integer"]
        #self.datas['Stacks'] = [stacks, "Integer"]
        self.top = top
        self.flag = 1
        self.init()

    def init(self):
        if glIsList(self.id):
            glDeleteLists(self.id, 1)
        x=[]
        y=[]
        half_h = self.get('Height')/2.0
        glNewList(self.id, GL_COMPILE)
        for i in range(self.get('Slices')):
            theta = 2.0*pi/self.get('Slices')*i
            c = self.get('Radius')*cos(theta)
            s = self.get('Radius')*sin(theta)
            x.append(c)
            y.append(s)
        glBegin(GL_LINE_LOOP)
        for i in range(self.get('Slices')):
            glVertex3f(x[i], y[i], half_h)
        glEnd()
        glBegin(GL_LINE_LOOP)
        for i in range(self.get('Slices')):
            glVertex3f(x[i], y[i], -half_h)
        glEnd()
        glBegin(GL_LINES)
        for i in range(self.get('Slices')):
            glVertex3f(x[i], y[i], -half_h)
            glVertex3f(x[i], y[i], half_h)
        glEnd()
        glEndList()

    #def save(self, n, temp):
    #    att=[]
    #    att = toxml_block(n+1, "type", [self.name()], att)
    #    att = toxml_block(n+1, "dims", [str(self.get("Radius"))+' '+str(self.top) +' '+str(self.get("Height"))], att)
    #    temp = toxml_block(n, "shape", att, temp)
    #    return temp
    
class od_box(od_shape):
    def __init__(self, l=1, w=1, h=1, name="box"):
        od_shape.__init__(self, name)
        attributes={
                    "Length":[l, "float"], 
                    "Width": [w, "float"], 
                    "Height":[h, "float"]
                    }
        for k, v in attributes.iteritems():
          self.displayable.append(k)
          self.editable.append(k) 
          self.sequence.append(k) 
          self.datas[k]=v
        #self.displayable.append("Length")
        #self.displayable.append("Width")
        #self.displayable.append("Height")
        #self.editable.append("Length") 
        #self.editable.append("Width") 
        #self.editable.append("Height") 
        #self.sequence.append("Length") 
        #self.sequence.append("Width") 
        #self.sequence.append("Height") 
        #self.datas['Length'] = [l, "float"]
        #self.datas['Width'] = [w, "float"]
        #self.datas['Height'] = [h, "float"]
        self.flag = 1
        self.init()

    def init(self):
        if glIsList(self.id):
            glDeleteLists(self.id, 1)
        l = self.get('Length')
        w = self.get('Width')
        h = self.get('Height')
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


class od_link(od_shape):
    def __init__(self, l=1, w=1, h=1, name="link"):
        od_shape.__init__(self, name)
        attributes={
                    "Length":[l, "float"], 
                    "Width": [w, "float"], 
                    "Height":[h, "float"]}
        for k, v in attributes.iteritems():
          self.displayable.append(k)
          self.editable.append(k) 
          self.sequence.append(k) 
          self.datas[k]=v
        #self.displayable.append("Length")
        #self.displayable.append("Width")
        #self.displayable.append("Height")
        #self.editable.append("Length") 
        #self.editable.append("Width") 
        #self.editable.append("Height") 
        #self.sequence.append("Length") 
        #self.sequence.append("Width") 
        #self.sequence.append("Height") 
        #self.datas['Length'] = [l, "float"]
        #self.datas['Width'] = [w, "float"]
        #self.datas['Height'] = [h, "float"]
        self.flag = 1
        self.init()

    def init(self):
        if glIsList(self.id):
            glDeleteLists(self.id, 1)
        h_l = self.get('Length')/2.0
        h_w = self.get('Width')/2.0
        h_h = self.get('Height')/2.0
        l = h_l*2.0
        w = h_w*2.0
        h = h_h*2.0
        arc_pnts_f=[]
        arc_pnts_b=[]
        low_pnts=[[0.0, 0.0, 0.0], [l, 0.0, 0.0], [l, w, 0.0], [0.0, w, 0.0]]
        up_pnts=[[0.0, 0.0, h], [l, 0.0, h], [l, w, h], [0.0, w, h]]
        for i in range(11):
            angle=pi*float(i)/10.0
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

    #def save(self, n, temp):
    #    att=[]
    #    att = toxml_block(n+1, "type", [self.name()], att)
    #    att = toxml_block(n+1, "dims", [str(self.get("Length"))+' '+str(self.get("Width")) +' '+str(self.get("Height"))], att)
    #    temp = toxml_block(n, "shape", att, temp)
    #    return temp
    
class od_marker(od_object):
    def __init__(self, _name=None, pos=[0.0,0.0,0.0], ori=[0.0,0.0,0.0]):
        if _name == None:
            _name = "marker_"+str(randint(1, 10000))
        od_object.__init__(self, _name)
        self.set_type("marker")
        self.GlM = D_H(ori, pos)            
        attributes={'Position':[pos, 'vec3'], 'Orientation':[ori, 'vec3'], 'WrtBCS':[1, 'Integer']}
        for k, v in attributes.iteritems():
          self.displayable.append(k)
          self.editable.append(k) 
          self.sequence.append(k) 
          self.datas[k]=v
        #self.datas['Position'] = [pos, 'vec3']
        #self.datas['Orientation'] = [ori, 'vec3']
        #self.datas['WrtBCS'] = [1, 'Integer' ]
        #self.displayable.append('WrtBCS')
        #self.displayable.append('Position')
        #self.displayable.append('Orientation')
        #self.editable.append('WrtBCS')
        #self.editable.append('Position')
        #self.editable.append('Orientation')
        #self.sequence.append('WrtBCS')
        #self.sequence.append('Position')
        #self.sequence.append('Orientation')
        self.sub_type.append("od_marker")
        self.absolute_position=None
        self.absolute_orientation=None
        self.body=None
        if self.name() is "cm_marker":  
          self.ics()

    def absPosition(self):
        abs_pos = self.get("Position") 
        if self.get("WrtBCS") == 1: 
           cm=self.body.get_cm_marker()
           ref_pos = cm.absPosition()
           for i in (0,1,2):
             abs_pos[i] = abs_pos[i]+ref_pos[i]      
        return abs_pos

    def setBody(self, b):
        self.body=b
        self.set("Dady", b)

    def getBody(self):
        return self.body

    def toAdm(self, admFile, qg, euler, part_id):
        mp = self.get("Position")
        pos=[0.0, 0.0, 0.0]
        eu=[0.0, 0.0, 0.0]
        if self.name() != "cm_marker":  
          for i in [0,1,2]:
            pos[i]=mp[i]
          #lprf=Mat33(euler)
          thisMat=Mat33(self.get("Orientation"))
          #wrt=thisMat.wrt(lprf)
          eu=thisMat.euler_angles()
        admFile.write("Marker/%d, Part=%d\n"%(self.Id(), part_id))
        admFile.write(", QP=%f, %f, %f\n"%(pos[0], pos[1], pos[2]))
        admFile.write(", Reuler=%fD, %fD, %fD\n"%(eu[0], eu[1], eu[2]))

    def ics(self):    
        self.datas['WrtBCS'] = [0, 'Integer' ]
        self.set('Omega', [None, None, None])
        self.set('Velocity', [None, None, None])
        self.displayable.append('Omega')
        self.editable.append('Omega')
        self.sequence.append('Omega')
        self.displayable.append('Velocity')
        self.editable.append('Velocity')
        self.sequence.append('Velocity')
          
    def abs_position(self, val):
        self.absolute_position=val
    
    def abs_orientation(self, val):
        self.absolute_orientation=val
        
    def from_absolute_to_relative(self, cm_mar):
        if self == cm_mar: return
        if self.get("WrtBCS") == 1: return 
        if self.absolute_position is not None:
           cm_pos = cm_mar.position()
           p=[0.0, 0.0, 0.0]
           for i in [0, 1,2]: p[i] = self.absolute_position[i]-cm_pos[i]
           self.set_position(p)
        if self.orientation is not None:
           cm_ori = cm_mar.orientation()
           cmMat = Mat33(cm_ori[0], cm_ori[1], cm_ori[2])
           abMat = Mat33(self.absolute_orientation)
           relMat = abMat.wrt(cmMat)
           angles = relMat.euler_angles()
           self.set_orientation(angles)

    def set_position(self, val):
        _temp_str = "Position"
        if self.name() is not "cm_marker" and self.get("WrtBCS") != 1:
          cm_mar = self.get("Dady").get_cm_marker()
          pos = cm_marker.position()
          for i in [0,1,2]: val[i] = val[i]-pos[i]
        self.set(_temp_str, val)
        self.GlM = D_H(self.orientation(), self.position())            

    def set_orientation(self, val):
        _temp_str = "Orientation"
        angles = val
        if self.name() is not "cm_marker" and self.get("WrtBCS") != 1:
          cm_mar = self.get("Dady").get_cm_marker()
          cm_ori = cm_mar.orientation()
          cmMat = Mat33(cm_ori[0], cm_ori[1], cm_ori[2])
          abMat = Mat33(val)
          angles = relMat.euler_angles()
        self.set(_temp_str, angles)
        self.GlM = D_H(self.orientation(), self.position())

        
    def __del__(self):
        self.GlM=[]
        
    def __str__(self):
       return self.composite_name()

    def __repr__(self):
       return self.composite_name()
        
    def position(self, pva=None):
        if pva == None:
          _temp_str = "Position"
          return self.get(_temp_str)
        else:  
          if self.core() != None:
            _temp_str = self.core().position(pva) 
            return _temp_str
          return None  
         
    def orientation(self, pva=None):
        if pva == None:
          _temp_str = "Orientation"
          return self.get(_temp_str)
        else:
          if self.core() != None:
            _temp_str = self.core().orientation(pva) 
            return _temp_str
          return None  

       
    def GLMatrix(self, DH):
        self.GlM = D_H(self.orientation(), self.position())    
        if self.core() != None:
            glmatrix = self.core().position_orientation()
            return glmatrix
        else:
            if DH == None:
                return mat_to_GlM(self.GlM)
            else:
                temp_mat = mat_multiply(DH, self.GlM, 4)
                return mat_to_GlM(temp_mat)

    def corization(self):
        if self.core() != None:
            self.set_core(None)
        temp_ang=[]
        for ang in self.orientation():
            temp_ang.append(ang*pi/180.0)
        _Core = OdMarker(self.Id(), self.position(), temp_ang, self.name())
        self.set_core(_Core)
        if "Omega" in self.datas.keys():
          _switch = [0, 0, 0]
          omega=[0.0, 0.0, 0.0]
          vals=self.get("Omega")
          for i in [0,1,2]:
            if vals[i] is not None:
              _switch[i] = 1
              omega[i]=vals[i]
              _Core.set_omega(omega, _switch)
        if "Velocity" in self.datas.keys():
          _switch = [0, 0, 0]
          vel=[0.0, 0.0, 0.0]
          vals=self.get("Velocity")
          for i in [0,1,2]:
            if vals[i] is not None:
              _switch[i] = 1
              vel[i]=vals[i]
              _Core.set_velocity(vel, _switch)

    def draw(self, cm):
        glPushMatrix()
        pos_ori = self.GLMatrix(cm)
        glMultMatrixf(pos_ori)
        rad1=radius(self.rad)
        draw_3axis(0.0, 0.0, 0.0, rad1, self.name())
        draw_3axis(0.0, 0.0, 0.0, rad1, None)
        glPopMatrix()
        
class od_body(od_object):
    def __init__(self, _name=None, dummy=False):
        if _name ==  None:
            _name = "body_"+str(randint(1, 10000))
        od_object.__init__(self, _name)
        cm_marker = None
        self.markers = []
        self.graphics_marker=None
        I = [1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0]
        I1 = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
        self.displayable.append('Mass');
        self.displayable.append('Inertia');
        self.displayable.append('Markers');
        self.displayable.append('Geometry');
        self.ploted=[ "Displacement_X", 
                      "Displacement_Y",
                      "Displacement_Z", 
                      "Velocity_X", 
                      "Velocity_Y", 
                      "Velocity_Z", 
                      "Acceleration_X", 
                      "Acceleration_Y", 
                      "Acceleration_Z",
                      "Angle1", 
                      "Angle2", 
                      "Angle3", 
                      "Omega1", 
                      "Omega2", 
                      "Omega3", 
                      "Omega_dot1", 
                      "Omega_dot2", 
                      "Omega_dot3" 
                      ]

        for key in self.ploted:
          self.plotable.append(key)
          self.datas[key]=[]
        self.editable.append('Position');
        self.editable.append('Orientation');
        self.editable.append('Mass');
        self.editable.append('Inertia');
        self.sequence.append('Position');
        self.sequence.append('Orientation');
        self.sequence.append('Mass');
        self.sequence.append('Inertia');
        self.sequence.append('Markers');
        self.sequence.append('Geometry');
        self.datas['Mass'] = [1.0, 'float']
        self.datas['Inertia'] = [I, 'mat3']
        if dummy is True:
          self.datas['Mass'] = [0.0, 'float']
          self.datas['Inertia'] = [I1, 'mat3']
        self.datas['Markers'] = [[], 'List']
        self.datas['Geometry'] = [[], 'List']
        self.set_type("body")
        self.sub_type.append("od_body")
        self.datas['Position'] = [[0,0,0], "vec3"]
        self.datas['Orientation'] = [[0,0,0], "vec3"]
        self.cm_mar = od_marker("cm_marker")
        self.add('Markers', self.cm_mar)
        self.cm_mar.setBody(self)
        self.cm_mar.set('Dady', self)
        self.graphics_marker = od_marker("ref")
        self.add('Markers', self.graphics_marker)
        self.graphics_marker.set('Dady', self)
        temp= od_marker()
        self.sub_types['Markers'] = temp.get_subtype()
        temp= od_shape()
        self.sub_types['Geometry'] = temp.get_subtype()
        self.cmglmat=[]
        self.joints=[]

    def consolidate(self):
        self.joints=[]
        for mar in self.get_markers():
          mar.setBody(self)      
            
    def getJoints(self):
        return self.joints

    def addJoint(self, j):
        if j.get("joint_type") == "fixed" and j.Id() < 0: return    
        self.joints.append(j)

    def requestEnergy(self, grav):
      id = self.cm_mar.Id()
      #g=self.get("Gravity")
      energy = ", +0.5*VM(%d)*VM(%d)+.5*WM(%d)*WM(%d)-%f*DY(%d)\n"%(id, id, id, id, grav, id)
      return energy

    def toAdm(self, admFile=None):
      part_id = self.Id()
      qg = self.get("Position")
      _re=self.get("Orientation")
      if part_id == 0: 
        part_id=100000
        admFile.write("Part/%d, ground\n"%(part_id))
      else:
        admFile.write("Part/%d, Mass=%f, Cm=%d\n"%(part_id, self.get("Mass"), self.cm_mar.Id()))
        vals=self.get("Inertia")
        admFile.write(", IP= %f, %f, %f, %f, %f, %f\n"%(vals[0], vals[4], vals[8], vals[1], vals[2], vals[5]))
        admFile.write(", Qg=%f, %f, %f\n, Reuler=%fD, %fD, %fD\n"%(qg[0], qg[1], qg[2], _re[0], _re[1], _re[2]))
      markerList=self.get_data("Markers")
      for mar in markerList[0] :
       mar.toAdm(admFile, qg, _re, part_id)

    def __del__(self):
        for i in self.get('Markers'):
            i=None;

        
    def corization(self):
        if self.core() != None:
            self.set_core(None)
        _Core = OdBody(self.Id(), self.name())
        self.set_core(_Core)
        self.get_cm_marker().corization()
        self.core().add_cm_marker(self.get_cm_marker().core())
        mass = self.get("Mass")
        self.core().setMass(mass)
        II = self.get("Inertia")
        self.core().setI(II)
        count = 0 
        for mar in self.get_markers():
            if count > 0:
              mar.corization()
              self.core().add_marker(mar.core())
              count = count + 1
            else: count = 1


    def set(self, name_str, val):
        if name_str[:3] != 'Ref':
            od_entity.set(self, name_str, val)
        if name_str == 'Position':
             self.cm_mar.set_position(val)
        elif name_str == 'Orientation':
             self.cm_mar.set_orientation(val)
        elif name_str == 'Ref_Position':
            self.graphics_marker.set_position(val)
        elif name_str == 'Ref_Orientation':
            self.graphics_marker.set_orientation(val)        

    def get_cm_marker(self):
        return self.cm_mar

    def get_markers(self):
        return self.get("Markers")

    def get_marker(self, i):
        return self.get_markers()[i]

    def get_num_markers(self):
        return len(self.get_markers())

    def adding(self, mar):    
        if mar.get_type() == "marker":
          mar.setBody(self)      
          if mar.name() == "cm_marker": 
             self.cm_mar.set_position(mar.position())
             self.cm_mar.set_orientation(mar.orientation())
             if mar.has_key('Omega'): self.cm_mar.set('Omega', mar.get('Omega'))
             if mar.has_key('Velocity'): self.cm_mar.set('Velocity', mar.get('Velocity'))
          elif mar.name() == "ref":
             self.graphics_marker.set_position(mar.position())
             self.graphics_marker.set_orientation(mar.orientation())
          else:
             self.add("Markers", mar)
             mar.set("Dady", self)
        elif mar.get_type() == "shape":
          self.add_geometry(mar)
          mar.set("Dady", self)

    def reSet(self):
        self.cmglmat=[]
        for key in self.ploted:
          self.datas[key]=[]

    def draw(self, replay=None):
        if replay is None:
          if len(self.geometry()) > 0:
            glPushMatrix()
            pos_ori = self.graphics_marker.GLMatrix(self.get_cm_marker().GlM)
            self.cmglmat.append(pos_ori)
            glMultMatrixf(pos_ori)
            self.draw_shape()
            glPopMatrix()
          if self.selected == 1:
            for mar in self.get_markers():
                mar.draw(self.get_cm_marker().GlM)
          pos = self.get_cm_marker().position(0)
          if pos != None:
            for i in [0,1,2]: self.datas[self.ploted[i]].append(pos[i])
          pos = self.get_cm_marker().position(1)
          if pos != None:
            for i in [0,1,2]: self.datas[self.ploted[i+3]].append(pos[i])
          pos = self.get_cm_marker().position(2)
          if pos != None:
            for i in [0,1,2]: self.datas[self.ploted[i+6]].append(pos[i])
          pos = self.get_cm_marker().orientation(0)
          if pos != None:
            tempOri = Mat33()
            tempOri.assign(pos)
            euler = tempOri.euler_angles()
            tempOri=None
            for i in [0,1,2]: self.datas[self.ploted[i+9]].append(euler[i])
          pos = self.get_cm_marker().orientation(1)
          if pos != None:
            for i in [0,1,2]: self.datas[self.ploted[i+12]].append(pos[i])
          pos = self.get_cm_marker().orientation(2)
          if pos != None:
            for i in [0,1,2]: self.datas[self.ploted[i+15]].append(pos[i])
        else:    
          if len(self.geometry()) > 0:
            glPushMatrix()
            glMultMatrixf(self.cmglmat[replay])
            self.draw_shape()
            glPopMatrix()

    def __repr__(self):
        temp_str = "body\n"
        for mar in self.get_markers():
            temp_str = temp_str + mar.__repr__() + "\n"
        return temp_str + " id " + str(self.Id())
        
class od_force(od_object):        
    def __init__(self, _name=None):
        if _name == None:
            _name = "force"+str(randint(1, 10000))
        od_object.__init__(self, _name)
        self.expr = None 
        self.force_type = None
        self.displayable.append("force_type")
        self.sub_type.append("od_sforce")
        self.sub_type.append("od_storque")
        self.sub_type.append("od_spdpr") 
        self.sub_type.append("od_spdpt")
        self.sub_type.append("od_beam")
        self.sub_type.append("od_bushing")
        self.sub_type.append("od_joint_force")
        self.datas["Expression"] = [None, 'od_function']
        self.set_type("force")

    def set_expression(self, str):
        self.set("Expression", str)

    def expression(self):
        return self.get("Expression")

class od_ij_force(od_force):
    def __init__(self, _name=None):
        if _name == None:
            _name = "force"+str(randint(1, 10000))
        od_force.__init__(self, _name)
        self._i_body = None
        self._j_body = None
        self.datas["force_type"] = ["sforce", 'str']
        self.datas["I_marker"] = [None, 'marker_ref']
        self.datas["J_marker"] = [None, 'marker_ref']
        self.displayable.append("I_marker")
        self.displayable.append("J_marker")
        self.editable.append("I_marker")
        self.editable.append("J_marker")
        self.sequence.append("I_marker")
        self.sequence.append("J_marker")
        self.f_t=[]
        
    def __repr__(self):
        temp_str = "force\n"
        temp_str = "imarker "+self.get_imarker().composite_name() + " "
        temp_str = temp_str+"jmarker "+self.get_jmarker().composite_name()
        temp_str = "force " + self.name() + " " + temp_str
        return temp_str + " id " + str(self.Id())

    def set_imarker(self, mar, body=None):
        self.set("I_marker", mar)
        self._i_body = body

    def set_jmarker(self, mar, body=None):
        self.set("J_marker", mar)
        self._j_body = body
            
    def get_imarker(self):
        return self.get("I_marker")

    def get_jmarker(self):
        return self.get("J_marker")

    def reSet(self):
        self.f_t=[]

    def draw(self, replay=None):
       if replay is None:
         f = self.get_jmarker().position(0)
         t = self.get_imarker().position(0)
         if f is None:
           return
         glBegin(GL_LINES)
         glVertex3f(f[0], f[1], f[2]) 
         glVertex3f(t[0], t[1], t[2])
         glEnd()
         self.f_t.append((f, t))
       else:
         ft = self.f_t[replay]
         glBegin(GL_LINES)
         glVertex3f(ft[0][0], ft[0][1], ft[0][2]) 
         glVertex3f(ft[1][0], ft[1][1], ft[1][2])
         glEnd()

    def corization(self):
        self.set_core(None)
        _Core = OdForce(self.Id(), self.name())
        self.set_core(_Core)
        if self.get_imarker() != None:
          self.core().set_imarker(self.get_imarker().core())
        if self.get_jmarker() != None:
          self.core().set_jmarker(self.get_jmarker().core())
        if self.expression() != None:
          self.core().set_expression(self.expression())

class od_joint_force(od_force):
    def __init__(self, _name="Joint_force"):
        od_force.__init__(self, _name)
        self.displayable.append("Joint")
        self.editable.append("Joint")
        self.sequence.append("Joint")
        self.displayable.append("Expression")
        self.editable.append("Expression")
        self.sequence.append("Expression")
        self.set("force_type", "od_joint_force")
        self.datas["Joint"] = [None, 'joint_ref']

    def get_joint(self):
        return self.get("Joint")

    def set_joint(self, j):
        self.set("Joint", j)

    def corization(self):
        self.set_core(None)
        _Core = OdJointForce(self.Id(), self.name())
        self.set_core(_Core)
        self.core().set_joint(self.get_joint().core())
        self.core().set_expression(self.expression())

    #def save(self, n, temp):
    #    att=[]
    #    att = toxml_block(n+1, "name", [self.name()], att)
    #    att = toxml_block(n+1, "type", [self.get("force_type")], att)
    #    if self.get_joint() != None:
    #      att = toxml_block(n+1, "joint", [self.get_joint().name()], att)
    #    if self.expression()  != None:
    #      att = toxml_block(n+1, "function", [self.expression()], att)
    #    temp = toxml_block(n, "force", att, temp)
    #    return temp

    def draw(self, replay=None):
        pass

    def reSet(self):
        pass

    def toAdm(self, admFile):
        joint = self.get("Joint")
        expression = self.get("expression")
        _str__ = "Sforce/%d, JOINT=%d, func=%s\n"%(self.Id(), joint.Id(), expression)
        admFile.write(_str__)

    def requestEnergy(self):
        return ""

class od_sforce(od_ij_force):
    def __init__(self, _name = "Sforce"):
        od_ij_force.__init__(self, _name)
        self.displayable.append("Expression")
        self.editable.append("Expression")
        self.sequence.append("Expression")
        self.set("force_type", "od_sforce")
        
    def corization(self):
      od_ij_force.corization(self)
      self.core().sforce()
        
class od_storque(od_ij_force):
    def __init__(self, _name="Storque"):
        od_ij_force.__init__(self, _name)
        self.displayable.append("Expression")
        self.editable.append("Expression")
        self.sequence.append("Expression")
        self.set("force_type", "od_storque")

    def corization(self):
        od_ij_force.corization(self)
        self.core().storque()

class od_spdpt(od_ij_force):
    def __init__(self, _name="Translational Spring Damper"):
        od_ij_force.__init__(self, _name)
        self.displayable.append("Stiffness")
        self.displayable.append("Damping")
        self.displayable.append("Distance")
        self.displayable.append("Force")
        self.editable.append("Stiffness")
        self.editable.append("Damping")
        self.editable.append("Distance")
        self.editable.append("Force")
        self.sequence.append("Stiffness")
        self.sequence.append("Damping")
        self.sequence.append("Distance")
        self.sequence.append("Force")
        self.datas["Stiffness"]=[1.0, "float"]
        self.datas["Damping"]=[1.0, "float"]
        self.datas["Distance"]=[0.0, "float"]
        self.datas["Force"]=[0.0, "float"]
        self.set("force_type", "od_spdpt")

    def requestEnergy(self):
        iid = self.get_imarker().Id()
        jid = self.get_jmarker().Id()
        K=self.get("Stiffness")
        L=self.get("Distance")
        energy=", +0.5*%f*(DM(%d, %d)-%f)*(DM(%d, %d)-%f)\n"%(K, iid, jid, L, iid, jid, L)
        return energy

    def toAdm(self, admFile):    
        iid= self.get_imarker().Id()
        jid= self.get_jmarker().Id()
        admFile.write("SpringDamper/%d, Translation, I=%d, J=%d\n"%(self.Id(), iid, jid)) 
        admFile.write(", K=%f, C=%f, F=%f, L=%f\n\n"%(self.get("Stiffness"), self.get("Damping"), self.get("Force"), self.get("Distance")))
        
    def corization(self):
        od_ij_force.corization(self)
        self.core().set_stiffness(self.get("Stiffness"))
        self.core().set_damping(self.get("Damping"))
        self.core().set_distance(self.get("Distance"))
        self.core().set_force(self.get("Force"))
        self.core().spdpt()

class od_bushing(od_ij_force):        
    def __init__(self, _name="Bushing"):
        od_ij_force.__init__(self, _name)
        self.datas["Kt"]=[[1.0e6, 1.0e6, 1.0e6], "vec3"]
        self.datas["Ct"]=[[1.0, 1.0, 1.0], "vec3"]
        self.datas["Kr"]=[[1.0e6, 1.0e6, 1.0e6], "vec3"]
        self.datas["Cr"]=[[1.0, 1.0, 1.0], "vec3"]
        self.datas["F"]=[[0.0, 0.0, 0.0], "vec3"]
        self.datas["T"]=[[0.0, 0.0, 0.0], "vec3"]
        self.displayable.append("Kt")
        self.editable.append("Kt")
        self.sequence.append("Kt")
        self.displayable.append("Kr")
        self.editable.append("Kr")
        self.sequence.append("Kr")
        self.displayable.append("Ct")
        self.editable.append("Ct")
        self.sequence.append("Ct")
        self.displayable.append("Cr")
        self.editable.append("Cr")
        self.sequence.append("Cr")
        self.displayable.append("F")
        self.editable.append("F")
        self.sequence.append("F")
        self.displayable.append("T")
        self.editable.append("T")
        self.sequence.append("T")
        self.set("force_type", "od_bushing")

    def corization(self):
        od_ij_force.corization(self)
        self.core().set_Kt(self.get("Kt"))
        self.core().set_Kr(self.get("Kr"))
        self.core().set_Ct(self.get("Ct"))
        self.core().set_Cr(self.get("Cr"))
        self.core().set_F(self.get("F"))
        self.core().set_T(self.get("T"))
        self.core().bushing()

    def toAdm(self, admFile):    
        iid= self.get_imarker().Id()
        jid= self.get_jmarker().Id()
        admFile.write("Bushing/%d, I=%d, J=%d\n"%(self.Id(), iid, jid)) 
        temp = self.get("Kt")
        admFile.write(", K=%f, %f, %f\n"%(temp[0], temp[1], temp[2]))
        temp = self.get("Ct")
        admFile.write(", C=%f, %f, %f\n"%(temp[0], temp[1], temp[2]))
        temp = self.get("Kr")
        admFile.write(", KT=%f, %f, %f\n"%(temp[0], temp[1], temp[2]))
        temp = self.get("Cr")
        admFile.write(", CT=%f, %f, %f\n"%(temp[0], temp[1], temp[2]))
        temp = self.get("F")
        admFile.write(", FORCE=%f, %f, %f\n"%(temp[0], temp[1], temp[2]))
        temp = self.get("T")
        admFile.write(", TORQUE=%f, %f, %f\n"%(temp[0], temp[1], temp[2]))

    def requestEnergy(self):
        return ""

class od_beam(od_ij_force):        
    def __init__(self, _name="Beam"):
        od_ij_force.__init__(self, _name)
        attributes={
                        "Emodulus":[1.0, "float"],
                        "Gmodulus":[1.0, "float"],
                        "Area":[1.0, "float"],
                        "Length":[1.0, "float"],
                        "Ixx":[1.0, "float"],
                        "Iyy":[1.0, "float"],
                        "Izz":[1.0, "float"],
                        "Asy":[0.0, "float"],
                        "Asz":[0.0, "float"],
                        "Ratio":[0.0, "float"],
                   }
        for k, v in attributes.iteritems():
          self.displayable.append(k)
          self.editable.append(k) 
          self.sequence.append(k) 
          self.datas[k]=v
        self.set("force_type", "od_beam")
     
    def corization(self):
        od_ij_force.corization(self)
        self.core().set_gmodulus(self.get("Gmodulus"))
        self.core().set_emodulus(self.get("Emodulus"))
        self.core().set_ratio(self.get("Ratio"))
        self.core().set_asz(self.get("Asz"))
        self.core().set_asy(self.get("Asy"))
        self.core().set_area(self.get("Area"))
        self.core().set_ixx(self.get("Ixx"))
        self.core().set_iyy(self.get("Iyy"))
        self.core().set_izz(self.get("Izz"))
        self.core().set_distance(self.get("Length"))
        self.core().beam()
        
    def toAdm(self, admFile):    
        iid= self.get_imarker().Id()
        jid= self.get_jmarker().Id()
        admFile.write("Beam/%d, I=%d, J=%d, L=%f\n"%(self.Id(), iid, jid, self.get("Length"))) 
        admFile.write(", Area=%f, Asy=%f, Asz=%f\n"%(self.get("Area"), self.get("Asy"), self.get("Asz")))
        admFile.write(", Ixx=%f, Iyy=%f, Izz=%f\n"%(self.get("Ixx"), self.get("Iyy"), self.get("Izz")))
        admFile.write(", E=%f, G=%f, CR=%f\n\n"%(self.get("Emodulus"), self.get("Gmodulus"), self.get("Ratio")))
                        
    def requestEnergy(self):
        return ""


class od_joint(od_object):
    def __init__(self, _name=None):
        if _name == None:
            _name = "joint_"+str(randint(1, 10000))
        od_object.__init__(self, _name)
        self._i_body = None
        self._j_body = None
        self.expr = None 
        self.joint_type = None 
        self.set_type("joint")
        self.datas["displacement"]=[]
        self.datas["velocity"]=[]
        self.datas["acceleration"]=[]
        self.plotable.append("displacement")
        self.plotable.append("velocity")
        self.plotable.append("acceleration")
        self.displayable.append("joint_type")
        self.displayable.append("I_marker")
        self.displayable.append("J_marker")
        self.displayable.append("Expression")
        self.editable.append("I_marker")
        self.editable.append("J_marker")
        self.editable.append("Expression")
        self.sequence.append("I_marker")
        self.sequence.append("J_marker")
        self.sequence.append("Expression")
        self.datas["joint_type"] = ["revolute", 'str']
        self.datas["I_marker"] = [None, 'marker_ref']
        self.datas["J_marker"] = [None, 'marker_ref']
        self.datas["Expression"] = [None, 'od_function']
        self.sub_type.append("od_revolute_joint")
        self.sub_type.append("od_translational_joint")
        self.sub_type.append("od_spherical_joint")
        self.sub_type.append("od_universal_joint")
        self.sub_type.append("od_fixed_joint")
        self.sub_type.append("od_free_joint")
        self.bodies=[]
        self.dof=0
        
    def getBodies(self):
        return self.bodies

    def dofs(self):
        return self.dof


    def get_body(self, body):
        if self.bodies[0] != body:
          return self.bodies[0]
        else:
          return self.bodies[1]

    def consolidate(self):
        self.bodies=[]
        imar = self.get_imarker()    
        imar.getBody().addJoint(self)
        self.bodies.append(imar.getBody()) 
        jmar = self.get_jmarker()    
        jmar.getBody().addJoint(self)
        self.bodies.append(jmar.getBody()) 

    def toAdm(self, admFile):
        admFile.write("Joint/%d, I=%d, J=%d, %s\n"%(self.Id(), self.get_imarker().Id(), self.get_jmarker().Id(), self.get("joint_type")))
        _expr = self.get("Expression")
        if _expr != None:
          admFile.write("Motion/%d, Joint=%d, Disp, Func=%s\n"%(self.Id(), self.Id(), self.get("Expression")))

    def reSet(self):
        self.datas["displacement"]=[]
        self.datas["velocity"]=[]
        self.datas["acceleration"]=[]

    def __repr__(self):
        temp_str = "joint\n"
        temp_str = "imarker "+self.get_imarker().composite_name() + " "
        temp_str = temp_str+"jmarker "+self.get_jmarker().composite_name()
        temp_str = "joint " + self.name() + " " + temp_str
        return temp_str + " id " + str(self.Id())
    
    def __str__(self):
       return self.composite_name()

    def set_imarker(self, mar, body=None):
        self.set("I_marker", mar)
        self._i_body = body

    def set_jmarker(self, mar, body=None):
        self.set("J_marker", mar)
        self._j_body = body

    def get_imarker(self):
        return self.get("I_marker")

    def get_jmarker(self):
        return self.get("J_marker")

    def set_expression(self, str):
        self.set("Expression", str)

    def expression(self):
        return self.get("Expression")


    def corization(self):
        self.set_core(None)
        _Core = OdJoint(self.Id(), self.name())
        self.set_core(_Core)
        self.core().set_imarker(self.get_imarker().core())
        self.core().set_jmarker(self.get_jmarker().core())
        if self.expression() != None:
            self.core().set_expression(self.expression())

    def draw(self):
        if self.core() is not None:
            temp = self.core().disp()
            for it in temp:
              self.datas["displacement"].append(it)
            temp = self.core().vel()
            for it in temp:
              self.datas["velocity"].append(it)
            temp = self.core().acc()
            for it in temp:
              self.datas["acceleration"].append(it)
            
class od_revolute_joint(od_joint):
    def __init__(self, _name = "Revolute Joint"):
        od_joint.__init__(self, _name)
        self.set("joint_type", "revolute")
        self.dof = 1
    
    def corization(self):
        od_joint.corization(self)
        self.core().revolute()

class od_translational_joint(od_joint):
    def __init__(self, _name = "Translational Joint"):
        od_joint.__init__(self, _name)
        self.set("joint_type", "translational")
        self.dof = 1

    def corization(self):
        od_joint.corization(self)
        self.core().translational()

class od_spherical_joint(od_joint):
    def __init__(self, _name = "Spherical Joint"):
        od_joint.__init__(self, _name)
        self.set("joint_type", "spherical")
        self.dof = 3

    def corization(self):
        od_joint.corization(self)
        self.core().spherical()

class od_universal_joint(od_joint):
    def __init__(self, _name = "Universal Joint"):
        od_joint.__init__(self, _name)
        self.set("joint_type", "universal")
        self.dof = 2

    def corization(self):
        od_joint.corization(self)
        self.core().universal()

class od_fixed_joint(od_joint):
    def __init__(self, _name = "Fixed Joint"):
        od_joint.__init__(self, _name)
        self.set("joint_type", "fixed")
        self.dof = 0

    def corization(self):
        od_joint.corization(self)
        self.core().fixed()

class od_free_joint(od_joint):
    def __init__(self, _name = "freed Joint"):
        od_joint.__init__(self, _name)
        self.set("joint_type", "free")
        self.dof = 6

    def corization(self):
        od_joint.corization(self)
        self.core().free()

class od_analysis(od_object):
    def __init__(self, _name=None):
        od_object.__init__(self, _name)
        self.set("type", "Analysis")
        self.sub_type.append("od_assembly")
        self.sub_type.append("od_kinematics")
        self.sub_type.append("od_statics")
        self.sub_type.append("od_dynamics")
        self.datas['Simulate'] = [1, 'Integer' ]
        self.datas['Animate'] = [1, 'Integer' ]
        self.datas['Debug'] = [0, 'Integer' ]
        self.model = None
        self.mycanvas=None

    def set_canvas(self, canvas):
        self.mycanvas=canvas

    def set_model(self, model):
        self.model = model

class od_assembly(od_analysis):
    def __init__(self, _name="Assembly"):
        od_analysis.__init__(self, _name)
        self.editable.append('Tolerance')
        self.editable.append('Iterations')
        self.editable.append('Simulate')
        self.displayable.append('Iterations')
        self.displayable.append('Tolerance')
        self.sequence.append('Tolerance')
        self.sequence.append('Iterations')
        self.datas['Tolerance'] = [1.0e-13, 'float' ]
        self.datas['Iterations'] = [6, 'Integer' ]

    def simulate(self):
        if self.get("Simulate") == 0: return
        if self.model != None:
           msg=self.model.displacement_ic()
           if msg[0] == 1:
              print msg[1]
           else:
              self.model.velocity_ic()
           if self.mycanvas != None:
              self.mycanvas.draw_all()
        return 

class od_timeadvancing(od_analysis):
    def __init__(self, _name="timeadvancing"):
        od_analysis.__init__(self, _name)
        self.displayable.append('End_Time')
        self.displayable.append('Output_Interval')
        self.displayable.append('Iterations')
        self.displayable.append('Tolerance')
        self.editable.append('End_Time')
        self.editable.append('Output_Interval')
        self.editable.append('Iterations')
        self.editable.append('Tolerance')
        self.sequence.append('End_Time')
        self.sequence.append('Output_Interval')
        self.sequence.append('Iterations')
        self.sequence.append('Tolerance')
        self.datas['End_Time'] =[1.0,  'float']
        self.datas['Output_Interval'] = [.01, 'float' ]
        self.datas['Iterations'] = [6, 'Integer' ]
 
class od_kinematics(od_timeadvancing):
    def __init__(self, _name="Kinematics"):
        od_timeadvancing.__init__(self, _name)
        self.displayable.append('Internal_Steps')
        self.editable.append('Internal_Steps')
        self.sequence.append('Internal_Steps')
        self.editable.append('Simulate')
        self.datas['Internal_Steps'] = [10, 'Integer']
        self.datas['Tolerance'] = [1.0e-13, 'float' ]

    def simulate(self): 
        if self.get("Simulate") == 0: return
        end = self.get("End_Time")
        out_interval = self.get("Output_Interval")
        tol = self.get("Tolerance")
        iter = self.get("Iterations")
        nn = self.get("Internal_Steps")
        n = end/out_interval
        for i in range(int(n)):
            end = (i+1)*out_interval
            msg = self.model.kinematic_analysis(end, int(nn), tol, int(iter))
            if msg[0] == 1:
              print msg[1]
              return
            if self.mycanvas != None:
              self.mycanvas.draw_all()
              self.model.time.append(end)
            
        
class od_statics(od_analysis):
    def __init__(self, _name="Statics"):
        od_analysis.__init__(self, _name)
        self.displayable.append('Iterations')
        self.displayable.append('Tolerance')
        self.editable.append('Iterations')
        self.editable.append('Tolerance')
        self.editable.append('Simulate')
        self.sequence.append('Iterations')
        self.sequence.append('Tolerance')
        self.datas['Tolerance'] = [1.0e-5, 'float' ]
        self.datas['Iterations'] = [25, 'Integer' ]

    def simulate(self): 
        if self.get("Simulate") == 0: return
        tol = self.get("Tolerance")
        iter = self.get("Iterations")
        msg=self.model.static_analysis(int(iter), tol)
        if msg[0] == 1:
           print msg[1]
        if self.mycanvas != None:
           self.mycanvas.draw_all()
        return 

class od_dynamics(od_timeadvancing):
    def __init__(self, _name="Dynamics"):
        od_timeadvancing.__init__(self, _name)
        self.displayable.append('Max_Stepsize')
        self.displayable.append('Min_Stepsize')
        self.displayable.append('Init_Stepsize')
        self.editable.append('Max_Stepsize')
        self.editable.append('Min_Stepsize')
        self.editable.append('Init_Stepsize')
        self.editable.append('Simulate')
        self.editable.append('Animate')
        self.editable.append('Debug')
        self.sequence.append('Max_Stepsize')
        self.sequence.append('Min_Stepsize')
        self.sequence.append('Init_Stepsize')
        self.datas['Max_Stepsize'] = [1.0e-1, 'float' ]
        self.datas['Min_Stepsize'] = [1.0e-6, 'float' ]
        self.datas['Init_Stepsize'] = [.0001, 'float' ]
        self.datas['Tolerance'] = [1.0e-3, 'float' ]

    def simulate(self):        
        if self.get("Simulate") == 0: return
        end = self.get("End_Time")
        out_interval = self.get("Output_Interval")
        tol = self.get("Tolerance")
        iter = self.get("Iterations")
        hmax = self.get("Max_Stepsize")
        hmin = self.get("Min_Stepsize")
        hinit = self.get("Init_Stepsize")
        debug = self.get("Debug")
        animate = self.get("Animate")
        self.model.reSet()
        numStep = end/out_interval
        start_time = time.time()
        cputime = None
        for i in range(int(numStep)):
          end = out_interval*(i+1)
          msg = self.model.dynamic_analysis(end, tol, int(iter), hmax, hmin, hinit, int(debug)) 
          if msg[0] == 1:
            print msg[1]
            return
          if self.mycanvas != None:
            if animate == 1:
              self.mycanvas.draw_all()
              self.model.time.append(end)
              cputime = self.model.cpu_time();
              _text = "Simulation time %f, cpu time %f, JacTime %f, RhsTime %f SolTime %f"%(end, cputime[0], cputime[1], cputime[2], cputime[3])
              self.model.frame.SetStatusText(_text)
        end_time = time.time()
        cputime = self.model.cpu_time();
        _str = "Elapsed Time: %f, CPU %f, Jac %f, Rhs %f, Sol %f"%((end_time-start_time), cputime[0], cputime[1], cputime[2], cputime[3])
        self.model.frame.SetStatusText(_str)
        return  

class od_subSystem(od_object): 
  '''This class defines the data structure  for  (sub)systems'''
  '''consist of part and kinematic connection only'''
  def __init__(self, model=None, _name="subSystem", prev_j=None, joint=None, systems=None):
    od_object.__init__(self, _name)
    attributes={"Bodies":[[], 'List'], 
                "Joints":[[], 'List'], 
                "Forces":[[], 'List'], 
                "Markers": [[], 'List']
                }
    for k, v in attributes.iteritems():
      #self.displayable.append(k)
      self.editable.append(k) 
      #self.sequence.append(k) 
      self.datas[k]=v
    self.prev=prev_j
    self.next=[]
    self.type=None
    self.root=joint
    self.model=model
    self.loop_end=None
    self.loop_start=None
    self.systems=systems
    self.loopCut=False
    if joint is not None: self.systems[joint] =self
      
  def get_bodies(self):
    return self.get("Bodies")

  def get_constraints(self):
    return self.get("Joints")

  def get_forces(self):
    return self.get("Forces")

  def toTree(self):
    thisTree=[]
    thisTree.append(self.type) #type
    jnts=[]
    for j in self.get_constraints():
      jnts.append(j)      
    thisTree.append(jnts)      #joint
    bds=[]
    for b in self.get_bodies():
      bds.append(b)     
    thisTree.append(bds)       #body
    _subTree=[]
    for i in self.next:
      tr = i.toTree()      
      _subTree.append(tr)
    thisTree.append(_subTree)    #subTree

    return thisTree

  def get_prev_subsystem(self):
    if self.prev == None: return None      
    return self.systems[self.prev] 

  def isItIn(self, b):
     val= (1==0)    
     if b.get_type() == "joint":    
       val = (b in self.get_constraints())         
     else:  
       val = (b in self.get_bodies())         
     return val

  def getList(self, bodyList, jntList):
     for b in self.get_bodies():
       bodyList.append(b)
     for j in self.get_constraints():
       jntList.append(j)      
     for _sub in self.next:  
       _sub.getList(bodyList, jntList)      
       
  def hasLoop(self):
     return self.loopCut

  def toList(self):   
     _list = []     
     _list.append(self.type)
     jnt=[]
     for j in self.get_constraints(): jnt.append(j)
     _list.append(jnt)
     _next = []
     for _n in self.next:
       n1 = _n.toList()
       _next.append(n1)
     _list.append(_next)
     return _list

class od_systemObject(od_object):
  def __init__(self, _name = "model"):
        od_object.__init__(self, _name)

  def get_bodies(self):
    return self.get("Bodies")

  def get_constraints(self):
    return self.get("Joints")

  def get_forces(self):
    return self.get("Forces")

  def add_body(self, pele):
    self.add("Bodies", pele)
    pele.set("Dady", self)

  def add_constraint(self, pcon):
    self.add("Joints", pcon)
    pcon.set('Dady', self)

  def add_force(self, pforce):
    self.add("Forces", pforce)
    pforce.set('Dady', self)



class od_systemModel(od_systemObject):
  def __init__(self, _name="subSystem"):      
    od_systemObject.__init__(self, _name)
    self.sub_type.append("od_beams")
    self.sub_type.append("od_chain")
    self.sub_type.append("od_spring")
    self.set_type("systemModel")
    self.marker_list=None
    attributes={"Bodies":[[], 'List'], 
                "Joints":[[], 'List'], 
                "Forces":[[], 'List'], 
                "Markers": [[], 'List']
                }
    for k, v in attributes.iteritems():
      self.datas[k]=v


  def initialize(self): 
    self.clear("Forces")      
    self.clear("Joints")      
    self.clear("Bodies")      

  def markerIdMap(self):
    mar_list={}
    for mar in self.marker_list:
      mar_list[mar.composite_name()]=mar.Id()      
    return mar_list  


  def draw(self, replay=None):
    for b in self.get("Bodies"):
       b.draw(replay)
    for c in self.get("Joints"):
       c.draw()
    for f in self.get("Forces"):
       f.draw(replay)

  def corization(self): 
    type_=0  
    ids=[]
    if self.core() != None:
      self.set_core(None)
    if self.__class__.__name__ is "od_beams":
      type_=1      
      _Core = OdSubSystem(self.Id(), type_, self.name())
      self.set_core(_Core)
    bodies = self.get("Bodies")  
    joints = self.get("Joints")
    forces = self.get("Forces")
    #marker and assign id
    self.marker_list=[]
    _id = self.Id()+1
    for b in bodies:
      len = b.get_num_markers()
      for i in range(len):
        self.marker_list.append(b.get_marker(i))
    for m in self.marker_list:
      m.set_id(_id)
      _id=_id+1
    ids.append(_id)  
    
    #create body and assign id
    _id = self.Id()+1
    for b in bodies:
      b.set_id(_id)
      b.corization()
      _id=_id+1
      self.core().add_body(b.core())
    ids.append(_id)  

    #create joint and assign it
    _id = self.Id()+1
    for c in self.get("Joints"):
      if c.Id() > 0:      
        c.set_id(_id)
        _id=_id+1
      c.corization()
      self.core().add_constraint(c.core())
    ids.append(_id)  

    #create force and assign id
    _id = self.Id()+1
    for f in forces:
      f.set_id(_id)
      f.corization()
      _id = _id+1
      self.core().add_force(f.core())
    ids.append(_id)  

    return max(ids)  

class od_beams(od_systemModel):
  ''' class for Beams     '''  
  def __init__(self, _name="Beams"):
    od_systemModel.__init__(self, _name)
    I = [1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0]
    attributes={
                        "Start_marker":  [None, "marker_ref"],
                        "End_marker":    [None, "marker_ref"],
                        "Emodulus":      [1.0, "float"],
                        "Gmodulus":      [1.0, "float"],
                        "Area":          [1.0, "float"],
                        "Length":        [1.0, "float"],
                        "Ixx":           [1.0, "float"],
                        "Iyy":           [1.0, "float"],
                        "Izz":           [1.0, "float"],
                        "Asy":           [0.0, "float"],
                        "Asz":           [0.0, "float"],
                        "Ratio":         [0.0, "float"], 
                        "FreeStart":     [1, "Integer"],
                        "FreeEnd":       [1, "Integer"],
                        "Segments":      [2, "Integer"],
                        "Mass":          [1.0, 'float'],
                        "Inertia":       [I,    'mat3']
               }
    keys = ("Start_marker", "End_marker",  "Emodulus", "Gmodulus", "Area", "Length", "Ixx", "Iyy", 
             "Izz", "Asy", "Asz", "Ratio", "FreeStart", "FreeEnd", "Segments", "Mass", "Inertia")       
    for k in keys:
      v = attributes[k]      
      self.displayable.append(k)
      self.editable.append(k) 
      self.sequence.append(k) 
      self.datas[k]=v

    self.displayable.append('Bodies')
    self.displayable.append('Joints')
    self.displayable.append('Forces')

  
  def initialize(self, model): 
    od_systemModel.initialize(self)      
    self.model=model      
    freeS = self.get("FreeStart")
    freeE = self.get("FreeEnd")
    vec = self.get("End_marker").absPosition()     
    startPos = self.get("Start_marker").absPosition()     
    seg = self.get("Segments")
    for i in (0,1,2):
      vec[i]=(vec[i]-startPos[i])/seg      
    prev = self.get("Start_marker")
    x= vec[0]
    #create parts
    if freeS:
      body = od_body(self.name()+"_"+"startPart")      
      body.set("Position", startPos)
      body.set("Orientation", self.get("Start_marker").get("Orientation"))
      body.set("Mass", self.get("Mass"))
      body.set("Inertia", self.get("Inertia"))
      body.adding(od_box(x, x/2, x/2))
      self.add_body(body)
      prev = body.get_cm_marker()

    keys = ("Emodulus", "Gmodulus", "Area", "Length", "Ixx", "Iyy", "Izz", "Asy", "Asz", "Ratio") 
    for i in range(seg):  
      mar = None
      if i == seg-1 and not freeE:
        mar = self.get("End_marker")
      else:
        _name = "%sPart%d"%(self.name(), i)      
        #print "create part", _name      
        body = od_body(_name)      
        pos=[0, 0, 0]
        for j in (0, 1, 2):
          pos[j]=startPos[j]+(i+1)*vec[j]
        body.set("Position", pos)
        body.set("Orientation", self.get("Start_marker").get("Orientation"))
        body.set("Mass", self.get("Mass"))
        body.set("Inertia", self.get("Inertia"))
        body.adding(od_box(x, x/2, x/2))
        self.add_body(body)
        mar = body.get_cm_marker()
      _name = self.name()+"Beam"+str(i)      
      #print "create beam", _name      
      beam=od_beam(_name)
      self.add_force(beam)
      beam.set("J_marker", prev)
      beam.set("I_marker", mar)
      for key in keys:
        beam.set(key, self.get(key))
      prev = body.get_cm_marker()
      #print 2*"Q", type(prev), prev.name(), prev.__class__.__name__
      
    #create free joint  
    for b in self.get("Bodies"):
      id = self.model.dummyId()
      jnt = od_free_joint("dummy_free_joint"+str(eval("abs(id)")))
      jnt.set_imarker(b.get_cm_marker(), b)
      jnt.set_jmarker(self.model.origin, self.model.ground())
      jnt.set_id(id)
      self.add_constraint(jnt)
    #print self.get("Start_marker"), "START"  
    return

    
class od_singleChain(od_subSystem):
  ''' class for single chain'''       
  def __init__(self, model, prev, joint, sys_, _name="singleChain"):
    od_subSystem.__init__(self, model, _name, prev, joint, sys_)
    self.type = "singleChain"
    self.last=None
    #construct a tree 
    self.model.ground().tag()
    if type(joint) is StringType: return
    print "branch with joint", joint.name()
    subsys = self.get_prev_subsystem()
    prev_body = self.model.ground()
    if subsys is not None:
      prev_body = subsys.get("last")      
    thePart = self.root.get_body(prev_body)
    theJoint=self.root
    while thePart is not None: 
      print thePart.name(),  len(thePart.getJoints()), thePart.isTagged()      
      self.loop_end=thePart
      #STOP
      if len(thePart.getJoints()) == 1:
         self.add("Bodies", thePart)
         self.add("Joints", theJoint)
         thePart.tag()
         theJoint.tag()
         self.last = thePart
         print "STOP"
         thePart = None
      #CREATE NEW CHAIN   
      elif len(thePart.getJoints())>2 and thePart.isTagged() == False:   
         loop=(1==0)     
         #check if it is the start of new chains
         self.add("Bodies", thePart)
         self.add("Joints", theJoint)
         thePart.tag()
         theJoint.tag()
         self.last = thePart
         print thePart.name()
         for j in thePart.getJoints():
           if j not in self.get_constraints() and j.isTagged() == False:
             #if j is the closing joint, do not create branch, rather declare a loop      
             anOtherPart = j.get_body(thePart)
             if anOtherPart.isTagged() == (1==1):
               self.loop_start = anOtherPart      
               j.tag()
               thePart=None
               print "find loop"
               print self.loop_start.name(), self.loop_end.name()
               break
             else:        
               sys_ = od_singleChain(self.model, self.root, j, self.systems)      
               sys_.findAndBreakLoop()
               if sys_.hasLoop() == True:
                 self.loopCut = sys_.hasLoop()      
                 thePart = None
                 loop=(1==1)      
                 break
               else:  
                 self.next.append(sys_)
                 print 20*"XX"
         thePart = None
      #CONTINUE   
      else: 
         loop=(1==0)     
         if thePart.isTagged() == False:
           thePart.tag()
           theJoint.tag()
           self.add("Bodies", thePart)
           self.add("Joints", theJoint)
           js = thePart.getJoints()
           for j in js:
             if j  not in self.get_constraints():      
               thePart = j.get_body(thePart)
               theJoint = j
               #if the part is tagged, a loop is detected 
               if thePart.isTagged() == True:
                 theJoint.tag() #tag the closing link of the loop 
                 self.loop_start = thePart
                 loop=(1==1)      
                 break
         else:  
           loop=(1==1)      
           self.loop_start = thePart
         if loop == (1==1):  
              print "find loop"
              print self.loop_start.name(), self.loop_end.name()
              thePart = None
    print 20*"X"        

  def corization(self): 
    if self.core() != None:
      self.set_core(None)
    type_=0  
    if self.type is "singleChain":
      type_=0      
    _Core = OdSubSystem(self.Id(), type_, "sysy")
    self.set_core(_Core)
    ids=[]
    for i in self.next:
      ids.append(i.Id())
    self.core().setSubSystem(ids)
    ids=[]
    for obj in self.get_constraints(): 
      ids.append(obj.Id())      
    self.core().setJoints(ids) 
   

  def findAndBreakLoop(self):
    '''find the bodies between loop_end and loop_start'''    
    if self.loop_start is None: return
    print "break loop"
    bodyList=[]
    #bodyList.append(self.loop_end) 
    jointList=[]
    _sys=self
    _b = self.loop_start;
    while _sys.isItIn(_b)==False:
      print _sys.root.name()      
      for i in range(len(_sys.parts)):
        b = _sys.parts.pop()      
        j = _sys.joints.pop()      
        bodyList.append(b)
        jointList.append(j)
      _sys = _sys.get_prev_subsystem()  
      if _sys is None: break
    bodyList.reverse()                
    jointList.reverse()                
    #append joint between start and end 
    s1 = Set(self.loop_start.getJoints())
    s2 = Set(self.loop_end.getJoints())
    s = s1 & s2
    j=s.pop()
    while j in jointList: 
      j=s.pop()
    jointList.append(j)  
    _len = len(jointList)
    for p in bodyList: print p.name()
    for p in jointList: print p.name()
    distance1=[]
    tempDof=0
    #from start+1 to end
    for i in range(_len-1): 
      j = jointList[i]       
      tempDof=tempDof+j.dofs()
      distance1.append(tempDof)
    #from end to start+1
    print distance1
    distance2=[]
    tempDof=0
    for i in range(_len-1): 
      j=jointList[_len-1-i]      
      tempDof=tempDof+j.dofs()
      distance2.append(tempDof)
    distance2.reverse()
    print distance2
    for i in range(_len-1):
      distance1[i] = abs(distance1[i]-distance2[i])      
    min_dist = min(distance1)
    indx = distance1.index(min_dist)
    b = bodyList[indx]
    j = jointList[indx]
    print b.name(), j.name()
    ijBodies = j.getBodies()
    #i==0: b is the i body of j
    #i==1: b is the j body of j
    i=0
    if b == ijBodies[1]: i=1
    #create a dummy part
    id = self.model.dummyId()
    bDup = od_body("dummy_part"+str(eval("abs(id)")))
    bDup.set_id(id)
    #create a fixed joint
    jnt=self.model.createAugJoint(b, bDup)
    #if i body, the bDup will replace b as i body of j
    if i==0:
      marker = j.get_imarker()
      id = self.model.dummyId()
      mar = od_marker("dummy_marker"+str(eval("abs(id)")), marker.get("Position"), marker.get("Orientation"))
      bDup.adding(mar)                
      mar.setBody(bDup)                
      j.set_imarker(mar, bDup)
    else:  
      marker = j.get_jmarker()
      id = self.model.dummyId()
      mar = od_marker("dummy_marker"+str(eval("abs(id)")), marker.get("Position"), marker.get("Orientation"))
      bDup.adding(mar)                
      mar.setBody(bDup)                
      j.set_jmarker(mar, bDup)
    self.model.adding(jnt)
    self.model.adding(bDup)
    self.loop_start=None
    self.loopCut=True
    


class Od_model(od_systemObject):
    def __init__(self, _name = "model"):
        od_systemObject.__init__(self, _name)
        myModel = self
        self.MarkerIDigin = 0
        self.BodyID = 1
        self.JointID = 0
        self.ForceID = 0
        self.MarkerID = 0
        self.marker_list=[]
        self.model_populated = 0
        self.displaylist_id=1;
        self.od_system = None
        self.sub_type=["Bodies", "Joints", "Forces", "SubSystems"]
        self.set("type", "model")
        self.time = []
        self.displayable.append('Gravity')
        self.displayable.append('Bodies')
        self.displayable.append('Joints')
        self.displayable.append('Forces')
        self.displayable.append('SubSystems')
        self.displayable.append('Analysis')

        #self.editable.append('Bodies')
        #self.editable.append('Joints')
        #self.editable.append('Forces')
        #self.editable.append('Analysis')
        self.editable.append('Gravity')

        self.sequence.append('Bodies')
        self.sequence.append('SubSystems')
        self.sequence.append('Joints')
        self.sequence.append('Forces')
        self.sequence.append('Analysis')
        self.sequence.append('Gravity')
        self.datas['Bodies'] = [[], 'List']; 
        self.datas['Joints'] = [[], 'List']; 
        self.datas['Forces'] = [[], 'List']; 
        self.datas['SubSystems'] = [[], 'List']; 
        self.datas['Analysis'] = [[], 'List']; 
        self.datas['Gravity'] = [[0.0, -9.8, 0.0], 'vec3']; 
        self.sub_types={}
        temp= od_body()
        self.sub_types['Bodies'] = temp.get_subtype()
        temp= od_joint()
        self.sub_types['Joints'] = temp.get_subtype()
        temp = od_force()
        self.sub_types['Forces']=temp.get_subtype()
        temp = od_systemModel()
        self.sub_types['SubSystems']=temp.get_subtype()
        temp = od_analysis()
        self.sub_types['Analysis']=temp.get_subtype()
        temp = None
        self.ground_=None
        self.canvas =  None
        self.frame=  None
        self.origin=None
        self._dummyId=-1
        self._systems={}

    def dummyId(self):
        self._dummyId = self._dummyId-1
        return self._dummyId


    def toAdm(self):
        admFile = open("model.adm", "w")
        admFile.write("! model.adm\n")
        grav = self.get("Gravity")
        admFile.write("Accgrav/IGRAV=%f, JGRAV=%f, KGRAV=%f\n"%(grav[0], grav[1], grav[2]))
        admFile.write("UNITS/FORCE = NEWTON, MASS = KILOGRAM, LENGTH = METER, TIME = SECOND\n")
        for j in  self.get_bodies():
          j.toAdm(admFile)
        for j in  self.get_constraints():
          if j.Id() > 0:      
            j.toAdm(admFile)
        for j in  self.get_forces():
          j.toAdm(admFile)
        admFile.write("variable/1, func=0\n")
        for j in  self.get_bodies():
          admFile.write(j.requestEnergy(grav[1]))
        for j in  self.get_forces():
          admFile.write(j.requestEnergy())
        admFile.write("int/si2\n")
        admFile.write("result/format\n")
        admFile.close()

        
    def reSet(self):
        self.model_populated = 0
        self.time = []
        for j in  self.get_constraints():
          j.reSet()
        for j in  self.get_bodies():
          j.reSet()
        for j in  self.get_forces():
          j.reSet()
    
    def set_canvas(self, canvas):
        self.canvas = canvas

    def set_frame(self, frame):
        self.frame = frame

    def get_bodies(self):
        return self.get("Bodies")
    
    def get_constraints(self):
        return self.get("Joints")

    def get_forces(self):
        return self.get("Forces")
    
    def get_subsystems(self):
        return self.get("SubSystems")

    def get_marker_pair(self):
        pair={}
        for b in self.get_bodies():
            len = b.get_num_markers()
            for i in range(len):
               mar=b.get_marker(i)
               pair[mar.get("Dady").name()+"."+mar.name()] = mar
        if self.model_populated == 0:
          subs = self.get_subsystems()      
          for sys_ in subs:       
            for b in sys_.get_bodies():
              len = b.get_num_markers()
              for i in range(len):
                 mar=b.get_marker(i)
                 pair[mar.get("Dady").name()+"."+mar.name()] = mar
        return pair

    def get_joint_pair(self):
        pair={}
        for j in self.get_constraints():
           #print j.name()     
           pair[j.get("Dady").name()+"."+j.name()] = j
        return pair
    
    def unselect(self):
        for j in self.get_constraints():
          j.unselect()

    def __repr__(self):
        temp_str = ""
        i = 0
        for b in self.get_bodies():
          temp_str = temp_str + "\n" + str(i)+"th body " + b.name() + " id " + str(b.Id())
          i = i + 1
        i = 0
        for j in self.get_constraints():
            temp_str = temp_str + "\n" + str(i)+"th joint " + j.name() + " id " + str(j.Id())
            i = i + 1
        return temp_str 
    
    def add_ground(self):
        if self.ground_ is not None: 
            self.ground_.set_id(0)    
            return   
        elif len(self.get("Bodies")) > 0:
          self.ground_ = (self.get("Bodies"))[0]      
        else:
          self.ground_ = od_body("ground")
          self.add("Bodies", self.ground_)
        self.ground_.set_id(0)    
        if self.origin is None:
          mar = od_marker("Origin")
          mar.setBody(self.ground_)
          self.ground_.add("Markers", mar)
          self.origin=mar
    
    def ground(self):
        if self.ground_ == None:
          self.ground_ = self.datas['Bodies'][0][0]      
        return self.ground_
                    
    def save(self, path):
         path=path.replace("odm", "xml")
         fid=open(path, "w")
         self.to_xml(fid)
         fid.close()


    def draw(self, replay=None):
        for _body in self.get_bodies():
            _body.draw(replay)
        for _j in self.get_constraints():
            _j.draw()
        for _j  in self.get_forces():
            _j.draw(replay)
        for _j in self.get_subsystems():
            _j.draw(replay)
                
            
    def element_types(self):
        return self.elements
    
    def adding(self, pele):
       if pele.get_type() == "body":
         self.add_body(pele)
       elif pele.get_type() == "joint":
         self.add_constraint(pele)
       elif pele.get_type() == "force":
         self.add_force(pele)
       elif pele.get_type() == "Analysis":
         self.add_analysis(pele)
       elif pele.get_type() == "systemModel":
         self.add_systemModel(pele)
            
    def add_analysis(self, pele):
        self.add("Analysis", pele)
        pele.set_model(self)
        if self.canvas != None:
           pele.set_canvas(self.canvas)

    
    def add_systemModel(self, psys):
        self.add("SubSystems", psys)    
        psys.set("Dady", self)

    def untagElement(self):
        for b in self.get_bodies():
          b.untag()      
          b.consolidate()                
        for c in self.get_constraints():
          c.untag()      
          c.consolidate()                
    
    def linkDanglingBody(self, individual=False):
        newJnt = False    
        if individual is True:
          for b in self.get_bodies():
            print "Comp1", b,  "Comp1"      
            print "Comp2", self.ground(),  "Comp2"      
            if len(b.getJoints())==0 and  b != self.ground():
              b.tag()      
              print "creating Joint for ", b.name()
              self.createAugJoint(b)      
              newJnt=True
        else:     
          bestBody=None      
          _len = 0
          for b in self.get_bodies():
            if b.isTagged() is False:      
              len_ = len(b.getJoints())
              if len_ > _len:
                bestBody = b
                _len = len_
          if bestBody is not None:  
            newJnt=True
            self.createAugJoint(bestBody)      
            self.untagElement()      
        return newJnt
            
    
    def createAugJoint(self, b, bDup=None):
        jnt=None    
        b.tag()
        if bDup == None:    
          id = self.dummyId()
          jnt = od_free_joint("dummy_free_joint"+str(eval("abs(id)")))
          print b.name()
          jnt.set_imarker(b.get_cm_marker(), b)
          jnt.set_jmarker(self.origin, self.ground())
          jnt.set_id(id)
          self.adding(jnt)
        else: 
          bDup.tag()      
          id = self.dummyId()
          jnt = od_fixed_joint("dummy_fixed_joint"+str(eval("abs(id)")))
          jnt.set_imarker(b.get_cm_marker())
          jnt.set_jmarker(bDup.get_cm_marker())
          jnt.set_id(id)
        return jnt  


    def consolidate(self):
        if self.origin is None: self.origin = self.ground().get_cm_marker()    
        self.untagElement()
        #link individual dangling part to ground with free joint
        self.linkDanglingBody(1==1)
#
        #make copy of body list and joint list  
        self._systems={}
        #remove the loop
        newJoint = True
        while newJoint == True:
          self.untagElement()
          js = self.ground().getJoints()
          loopFound = True
          while loopFound: 
            loopFound = False
            for j in js:
              if j.isTagged() is False:      
                tree = od_singleChain(self, None, j, self._systems) 
                tree.findAndBreakLoop()      
                if tree.hasLoop() == True:
                  self.untagElement()
                  loopFound = True
                  print "Loop Found"
                  self._systems={}
                  break  
          newJoint = self.linkDanglingBody(1==0)
        firstList=[]
        secondList=[]
        js = self.get_constraints()
        for j in js: 
          if j.Id() < 0 and j.get("joint_type") == "free":      
            firstList.append(j)      
          else:        
            secondList.append(j)      
        len1 = len(firstList)   
        for i in range(len1):
          js[i]=firstList[i]      
        len2 = len(secondList)   
        for i in range(len2):
          js[i+len1]=secondList[i]      
        self.untagElement()
        self._systems={}
        _Tree=["Tree", [], [self.ground()], ]
        js = self.ground().getJoints()
        temp_tree=[]
        _b=[]
        _j=[]
        root_ = od_singleChain(self, None, "root", self._systems)
        list_=[]
        list_.append("Root")
        list_.append([])
        _next = []
        for j in js: 
          if j.isTagged() is False:      
            #print j.name()      
            tree = od_singleChain(self, None, j, self._systems) 
            _li = tree.toList()
            _next.append(_li)
            tree.getList(_b, _j)
            _tree = tree.toTree()
            temp_tree.append(_tree)
            root_.next.append(tree)  
        list_.append(_next)    
        _Tree.append(temp_tree)    
        printTree(_Tree)
        #if j.get("joint_type") == "fixed" and j.Id() < 0: return    
        print "Bodies in Sequence"
        for b in _b: print b.name(), b.Id()
        _jj = self.get_constraints()
        _bb = self.get_bodies()
        _ff = self.get_forces()
        print "Joints in Sequence"
        i=0
        for j in _j: 
          _jj[i]=j      
          i=i+1
        i=0
        for _sys in self._systems.values():
          _sys.set_id(i)
          i=i+1
        #for _sys in self._systems.values():
        #  _sys.nextIds()      
        return list_

    def populate_model(self):
        if self.model_populated == 1:
            return
        if self.od_system != None:
            self.od_system = None
        self.consolidate()    
        self.BodyID = 1
        self.JointID = 1
        self.ForceID = 1
        self.MarkerID = 1
        #return None
        __name__ = self.name()
        self.od_system = OdSystem(__name__)
        self.od_system.setGravity(self.get("Gravity"))
        b = self.ground()
        b.corization()
        self.od_system.add_body(b.core())

        subs = self.get_subsystems()
        marker_id={}
        len = self.marker_list
        if len > 0:
          self.marker_list=[]
        for b in self.get_bodies():
            len = b.get_num_markers()
            for i in range(len):
               self.marker_list.append(b.get_marker(i))
  
        for m in self.marker_list:
            m.set_id(self.MarkerID)
            marker_id[m.composite_name()] = m.Id()
            self.MarkerID=self.MarkerID+1
        #Body corization    
        bs = self.get_bodies()    
        for b in bs[1:]:
            if b.Id() > -1:    
              b.set_id(self.BodyID)
              self.BodyID=self.BodyID+1
            b.corization()
            self.od_system.add_body(b.core())
        #joint Corization    
        joint_id={}
        for c in self.get_constraints():
            id_ = c.Id()    
            if id > -1:
              c.set_id(self.JointID)
              joint_id[c.name()]=c.Id()
              self.JointID=self.JointID+1
            c.corization()
            self.od_system.add_constraint(c.core())
        #force corization    
        tempList = self.get_forces()
        for _f in tempList:
            _f.set_id(self.ForceID)
            _expression = _f.expression()
            if _expression != None:
              for k,v in marker_id.iteritems():
                _expression=_expression.replace(k, str(v))
              for k,v in joint_id.iteritems():
                _expression=_expression.replace(k, str(v))
              _expression.strip()
              time=1.0
              eval(_expression)
              _f.set_expression(_expression)
            _f.corization()
            self.ForceID = self.ForceID+1
            #print _f.get_subtype()
            if _f.get("force_type") == "od_joint_force":
              # print _f
               self.od_system.add_joint_force(_f.core())
            else:
               self.od_system.add_force(_f.core())
        #subsystems
        subsysId=max(self.BodyID, self.JointID, self.ForceID, self.MarkerID)
        subs = self.get_subsystems()
        marker_id={}
        for sub in subs:
          sub.initialize(self)      
          sub.set_id(subsysId)
          subsysId=sub.corization()
          self.od_system.add_subsystem(sub.core())
          map_=sub.markerIdMap()
          for k,v in map_.iteritems():
            marker_id[k]=v      
        for _f in tempList:
          _expression = _f.expression()
          if _expression != None:
            for k,v in marker_id.iteritems():
              _expression=_expression.replace(k, str(v))
        self.model_populated = 1
        self.toAdm()

    def displacement_ic(self):
        self.populate_model()
        msg=self.od_system.displacement_ic()
        return msg

    def velocity_ic(self):
        self.populate_model()
        self.od_system.velocity_ic()

    def kinematic_analysis(self, end, n, tol, iter):
        self.populate_model()
        msg=self.od_system.kinematic_analysis(end, n, tol, iter)
        return msg
        
    def static_analysis(self, num, tol):
        self.populate_model()
        msg = self.od_system.static_analysis(num, tol)
        return msg
      
    def cpu_time(self):
         _time = self.od_system.cpu_time()
         return _time
      
    def dynamic_analysis(self, end, tol, iter, hmax, hmin, hinit, debug): 
        self.populate_model()
        msg=self.od_system.dynamic_analysis(end, tol, iter, hmax, hmin, hinit, debug)
        return msg

    def dependency_checking(self, model):
      nameMarPair=self.get_marker_pair()      
      nameJntPair = self.get_joint_pair()
      bag = self.get_constraints()+self.get_forces()
      for c in bag: 
        mar = c.get("I_marker")
        if type(mar) is StringType or type(mar) is UnicodeType:
          mar = nameMarPair[mar]
          c.set("I_marker", mar)
        mar = c.get("J_marker")
        if type(mar) is StringType or type(mar) is UnicodeType:
          mar = nameMarPair[mar]
          c.set("J_marker", mar)
        joint = c.get("Joint")
        if type(joint) is StringType or type(joint) is UnicodeType:
            joint=nameJntPair[joint]
            c.set("Joint", joint)
      model.add_ground()      
      bag = self.get_subsystems()
      for s in bag:
        for key in ("Start_marker", "End_marker"):      
          mar = s.get(key)      
          if type(mar) is StringType or type(mar) is UnicodeType:
            #mar = self.find_marker_by_name(mar)
            mar = nameMarPair[mar]
            s.set(key, mar)
        s.initialize(model)    
             
def readxml(node, parent_ent=None):
      tagname = node.tagName
      od_ent = None
      if tagname[:3] == "od_" or tagname[:3] == "Od_":
        od_ent = tagname+"()" 
        od_ent = eval(od_ent)
        for child in node.childNodes:
          if child.nodeType == Node.ELEMENT_NODE:
            tagname = child.tagName
            if tagname[:3] == "od_":
              item = readxml(child, od_ent)
            else:
              for _child in child.childNodes:
                 if _child.nodeType == Node.TEXT_NODE:
                  data = _child.data
                  if "\n" in data: 
                    pass
                  else:
                    if tagname == "Name" or tagname[-7:] == "_marker"  or tagname == "Expression" or tagname == "Joint" :
                      od_ent.set(tagname, data)
                      #print tagname, data.__class__.__name__
                    else:
                      od_ent.set(tagname, eval(data))
        if parent_ent: parent_ent.adding(od_ent)
      if od_ent.__class__.__name__ == "Od_model": 
        od_ent.dependency_checking(od_ent)
      return od_ent 
    
def read_newxml(path):
        doc = xml.dom.minidom.parse(path)
        root = doc.documentElement
        model = readxml(root, None)
        return model

def position_wrt_body(pos, pb):
     cm = pb.get_cm_marker()
     _pos = cm.position()
     #print pos, _pos
     pos_=[0.0, 0.0, 0.0]
     for i in [0,1,2]: pos_[i] = pos[i]-_pos[i]
     _ori = Mat33(cm.orientation()).invert()
     pos_ = _ori.transform(pos_)
     return pos_
  
def orientation_wrt_body(angles, pb):
     cm = pb.get_cm_marker()
     _ori = Mat33(cm.orientation())
     ori =Mat33(angles)
     rel = ori.wrt(_ori)
     rel_angles = rel.euler_angles()
     return rel_angles
  
def joint_create(type_, pb1, pb2, pos=[0.0, 0.0, 0.0], ori=[0.0,0.0,0.0], _name=None):
     tempStr = "od_"+type_+"_joint()"      
     if _name is not None:   
       tempStr = "od_"+type_+"_joint(\""+_name+"\")"      
     jointTemp = eval(tempStr)
     pos1 = position_wrt_body(pos, pb1)
     ori1 = orientation_wrt_body(ori, pb1)
     i_mar = od_marker("to"+pb2.name()+type_)
     pb1.adding(i_mar)
     i_mar.set("Position", pos1)
     i_mar.set("Orientation", ori1)
     pos2=position_wrt_body(pos, pb2)
     ori2 = orientation_wrt_body(ori, pb2)
     j_mar = od_marker("to"+pb1.name()+type_)
     j_mar.set("Position", pos2)
     j_mar.set("Orientation", ori2)
     pb2.adding(j_mar)
     jointTemp.set_imarker(i_mar, pb1)
     jointTemp.set_jmarker(j_mar, pb2)
     return jointTemp


