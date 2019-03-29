import wx
import string
from od_model import *
#from od_body_dlg import *
#from od_joint_dlg import *
import re

class Od_TreeCtrlPanel(wx.Panel):
    def __init__(self, parent, model, plotpanel=None):
        wx.Panel.__init__(self, parent, -1, style=wx.WANTS_CHARS)
        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.model = model
        self.model_root = None
        self.selected_type = None
        self.selected_item = None
        self.float_val=re.compile("[-+]?(\d+(\.\d*)?|\d*\.\d+)([eE][-+]?\d+)?")
        self.int_val  =re.compile("[-+]?\d+")
        self.create_ids={}
        self.tree = wx.TreeCtrl(self, wx.NewId(),
                                style=wx.TR_HAS_BUTTONS)
        self.root = self.tree.AddRoot("Model Database")
        self.tree.SetPyData(self.root, None)
        self.tree.Expand(self.root)
        self.Bind(wx.EVT_TREE_SEL_CHANGED, self.OnSelChanged, self.tree)
        self.tree.Bind(wx.EVT_RIGHT_DOWN, self.OnRightClick)
        #self.tree.Bind(wx.EVT_RIGHT_UP, self.OnRightClickUp)
        self.PyObj_TreeId={}
        self.List_Folder={}
        self.panel = plotpanel

    def reSet(self):
       self.model=None
       self.tree.DeleteChildren(self.root)
       self.tree.Expand(self.root)
      
    def OnSelChanged(self, event):
        item = event.GetItem()
        text = self.tree.GetItemText(item)
        if text == "Model Database": return
        if self.panel != None and item is not None:
            parent = self.tree.GetItemParent(item)
            pyparent = self.tree.GetPyData(parent)
            if hasattr(pyparent, "get_plotable") and pyparent.get_plotable != []:
              self.panel.drawObj(pyparent, text)
        if self.model != None: self.model.unselect()
        obj = self.tree.GetPyData(item)
        if  hasattr(obj, "select") is True:
            obj.select()
        event.Skip()
        
    def OnRightClick(self, event):
        pt = event.GetPosition()
        item, flags = self.tree.HitTest(pt)
        obj = self.tree.GetPyData(item)
        self.selected_item = item
        self.tree.SelectItem(item)
        text = self.tree.GetItemText(item)
        if text == "Model Database":
            return
        elif obj is None: # It is a list attributes of some thing, so talk to its parent
            self.obj_dady = self.tree.GetItemParent(item) 
            text = (text.split(" "))[0]
            obj = self.tree.GetPyData(self.obj_dady)
            editable = obj.get_editable()
            if text not in editable:
                wx.MessageBox(text+" can not be modified.", "Error", wx.OK | wx.CENTRE | wx.ICON_EXCLAMATION)
                return
            dlg = od_dlg(obj, [text], self.model, "Modifying "+obj.get_type()+":"+text)
            if dlg.ShowModal() == wx.ID_OK:
                dlg.OnOk()
                self.process_entity(None, obj)
                self.tree.Expand(self.root)
        elif obj is "List":
            self.obj_dady = self.tree.GetItemParent(item) 
            self.data = self.tree.GetPyData(self.obj_dady)
            self.sub_types = self.data.get_subtype(text)
            menu = wx.Menu()
            for sub_type in self.sub_types:
                if self.create_ids.has_key(sub_type) == False:
                    self.create_ids[sub_type] = wx.NewId()
                self.Bind(wx.EVT_MENU, self.OnAdd, id=self.create_ids[sub_type])
                menu.Append(self.create_ids[sub_type], "Add "+ sub_type[3:])
            self.PopupMenu(menu, event.GetPosition())
            menu.Destroy()
        else:
            dlg = od_dlg(obj, obj.get_editable(), self.model, "Modifying "+obj.get_type())
            if dlg.ShowModal() == wx.ID_OK:
                dlg.OnOk()
                self.process_entity(None, obj)
                self.tree.Expand(self.root)

    def OnAdd(self, event):
          id = event.GetId()
          for sub_type in self.sub_types:
                if self.create_ids[sub_type] == id:
                    od_type = sub_type
          #creation 
          obj=od_type+"()"
          ##print obj
          obj = eval(obj)
          dlg = od_dlg(obj, obj.get_editable(), self.model, "Creating "+obj.get_type())
          if dlg.ShowModal() == wx.ID_OK:
                dlg.OnOk()
          else:
              obj = None
          if obj is not None:
              type = obj.get_type()
              if type == "marker" or type == "shape":
                  temp = self.tree.GetPyData(self.obj_dady)
                  temp.adding(obj)
              elif type == "joint" or  type == "body" or type == "force" or type == "systemModel":
                  self.model.adding(obj)
              elif type == "Analysis":
                  self.model.adding(obj)
                  obj.simulate()
              temp = obj.get_data('Start') 
              self.process_entity(self.selected_item, obj)
              


    
    def OnAddGeomBox(self, event):
        self.selected_type = "box"
        self.OnAddGeom(event)

    def OnAddGeomCyl(self, event):
        self.selected_type = "cylinder"
        self.OnAddGeom(event)
    def OnAddGeomLink(self, event):
        self.selected_type = "link"
        self.OnAddGeom(event)

    def OnAddGeom(self, event):
        dlg = od_box_dlg(self.selected_type)
        if dlg.ShowModal() == wx.ID_OK:
            _3vals =dlg.OnOk()
            if self.selected_type == "box":
                _shape = od_box(_3vals[0], _3vals[1], _3vals[2])
            elif self.selected_type == "cylinder":
                _shape = od_cylinder(_3vals[0], _3vals[1], _3vals[2])
            elif self.selected_type == "link":
                _shape = od_link(_3vals[0], _3vals[1], _3vals[2])
            _parent_id = self.tree.GetItemParent(self.selected_item)
            b_obj = self.tree.GetPyData(_parent_id)
            b_obj.add_geometry(_shape)
            shape_item = self.tree.AppendItem(self.selected_item, self.selected_type)
            self.tree.SetPyData(shape_item, _shape)
        dlg.Destroy()

    def OnSize(self, event):
        w,h = self.GetClientSizeTuple()
        self.tree.SetDimensions(0, 0, w, h)

    def SetModel(self, model):
        self.model = model
        
    def OnUpdate(self):
        if self.model is None:
            return
        self.process_entity(self.root, self.model)
        self.tree.Expand(self.root)
        
    def process_entity(self, parent, pyobj, nn=0):
        #print nn*" ", "PYNAME", pyobj.name()    
        #create this object with name
        if parent is not None:
            this_item = self.tree.AppendItem(parent, pyobj.name())
            self.tree.SetPyData(this_item, pyobj)
            self.PyObj_TreeId[pyobj] = this_item
        else:
            this_item = self.PyObj_TreeId[pyobj]
            self.tree.DeleteChildren(this_item)
        if self.panel is None or pyobj.get_plotable() == []: 
          #print nn*" ", "DISPLAY",  pyobj.get_displayable()     
          for key in pyobj.get_displayable():
            datas, type = pyobj.get_data(key)
            #print nn*" ", key, datas, type 
            if type is "List":
                container = self.tree.AppendItem(this_item, key)
                self.tree.SetPyData(container, type)
                if key not in self.List_Folder.keys():  self.List_Folder[key] = container
                for data in datas:
                    self.process_entity(container, data, nn+1)
            elif type is "float" or type is "vec3" or type is 'str' or type is "Integer":
                item = self.tree.AppendItem(this_item, key)
                self.tree.SetPyData(item, None)
            elif type is type is 'od_function':
                item = self.tree.AppendItem(this_item, key + " " + str(datas))
                self.tree.SetPyData(item, None)
            elif type is "mat3":
                item = self.tree.AppendItem(this_item, key)
                self.tree.SetPyData(item, None)
            elif type == "od_marker":
                self.process_entity(this_item, datas, nn+1)
            elif type is "marker_ref":
                #print key, datas.__class__.__name__    
                item = self.tree.AppendItem(this_item, key + " " + datas.composite_name())
            elif type is "joint_ref":
                item = self.tree.AppendItem(this_item, key + " " + datas.composite_name())
        else:
          for key in pyobj.get_plotable():
             item = self.tree.AppendItem(this_item, key)
  
    def update_tree_external(self,  pyobj):
        type_ = pyobj.get_type()
        if type_ in ["body", "joint", "force"]:
            _map = {'body':'Bodies', 'joint':'Joints', 'force':'Forces'}
            key_ = _map[type_]
            parent_ = self.List_Folder[key_]
            self.process_entity(parent_, pyobj)
        elif type_ is "model":
           self.tree.DeleteChildren(self.root)      
           self.model = pyobj
           self.process_entity(self.root, self.model)
           self.tree.Expand(self.root)
