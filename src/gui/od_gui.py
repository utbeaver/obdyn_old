import wx
import time
import sys, os
from math import *
from od_tree_ctrl import *
from od_canvas import *
from od_plot import *
from od_utils import *
from string import *
from od_model import *
global myModel

myModel = None
class Od_Frame(wx.Frame):
    def __init__(self, parent,  id, title):
        wx.Frame.__init__(self, parent, id, title, pos=(150,150),size=(1000,700))
        self.wildcard = "XML file (*.xml) | *.xml|" \
                                      "Model file (*.odm)|*.odm"  
        self.tree = None
        self.mycanvas=None
        self.filename = None
        self.model = None
        self.save_need = False
        self.CenterOnScreen()
        self.CreateStatusBar()
        self.SetStatusText("Statusbar")
        
        self.menuBar = wx.MenuBar()
        file_menu = wx.Menu()
        file_menu.Append(wx.ID_NEW,       "&New Model", "Create a new model.")
        file_menu.Append(wx.ID_OPEN,      "&Open",      "Load a existing model.")
        file_menu.Append(wx.ID_SAVE,      "&Save",      "Save current Model.")
        file_menu.Append(wx.ID_SAVEAS,    "&Save As",   "Save model as different file")
        file_menu.Append(wx.ID_CLOSE,     "&Close",     "Delete the model")
        file_menu.Append(wx.ID_EXIT,      "&Quit",      "Quit then application")
        self.menuBar.Append(file_menu, "&File")
 
        tool_ids =[]
        for i in range(4):
            tool_ids.append(wx.NewId())
        tool_menu = wx.Menu()
        tool_menu.Append(tool_ids[0], "Pyshell", "An Interactive Shell")
        tool_menu.Append(tool_ids[1], "Changing Work directory", "Change Working Directroy")
        tool_menu.Append(tool_ids[2], "Plot", "Plot")
        tool_menu.Append(tool_ids[3], "Replay", "Replay")
        self.menuBar.Append(tool_menu, "&Tools")
        

        self.SetMenuBar(self.menuBar)
        
        self.split = wx.SplitterWindow(self, -1, style = wx.SP_3D)
        self.tree = Od_TreeCtrlPanel(self.split, self.model)
        self.split.SetMinimumPaneSize(5)
        self.mycanvas = od_canvas(self.split)
        self.split.SplitVertically(self.tree, self.mycanvas)
        self.split.SetSashPosition(200)
        box = wx.BoxSizer(wx.HORIZONTAL)
        box.Add(self.split, 1, wx.ALIGN_CENTRE|wx.ALL|wx.EXPAND, 5)
        self.SetSizer(box)
        self.SetAutoLayout(True)


        
        self.Bind(wx.EVT_MENU, self.doExit, id=wx.ID_EXIT)
        self.Bind(wx.EVT_MENU, self.doClose, id=wx.ID_CLOSE)
        self.Bind(wx.EVT_MENU, self.doOpen, id=wx.ID_OPEN)
        self.Bind(wx.EVT_MENU, self.doNewModel, id=wx.ID_NEW)
        self.Bind(wx.EVT_MENU, self.doSave, id=wx.ID_SAVE)
        self.Bind(wx.EVT_MENU, self.doSaveAs, id=wx.ID_SAVEAS)
        #self.Bind(wx.EVT_MENU, self.doNewBody, id=build_ids[1])
        #self.Bind(wx.EVT_MENU, self.doAssembly, id=anal_ids[0])
        #self.Bind(wx.EVT_MENU, self.doKinAnal, id=anal_ids[1])
        #self.Bind(wx.EVT_MENU, self.doStatic, id=anal_ids[2])
        self.Bind(wx.EVT_MENU, self.doPyshell, id=tool_ids[0])
        self.Bind(wx.EVT_MENU, self.doCwd, id=tool_ids[1])
        self.Bind(wx.EVT_MENU, self.doPlot, id=tool_ids[2])
        self.Bind(wx.EVT_MENU, self.doReplay, id=tool_ids[3])
        startup_xml_file = os.path.join(os.getcwd(), "odi.xml")
        if  os.path.exists(startup_xml_file):
          if os.path.exists(startup_xml_file):
            self.model=read_newxml(startup_xml_file)
            #self.model.add_ground()
          self.mycanvas.SetModel(self.model)
          self.tree.SetModel(self.model)
          self.tree.OnUpdate()
          self.mycanvas.draw_all()
          self.model.set_canvas(self.mycanvas)
          self.model.set_frame(self)
        
    def SetModel(self, m):
        if self.mycanvas is not None:
           self.mycanvas.SetModel(m)
        if self.tree is not None:
           self.tree.update_tree_external(m)
        self.model = m
        self.model.set_canvas(self.mycanvas)
        self.model.set_frame(self)
        
    def doPlot(self, event):
        if self.model != None:
           win = PPTPlot(self, self.model, -1, "PPT Plotter")
           win.Show()
     
    def doCwd(self, event):
        dlg = wx.DirDialog(self, "Choose a directory:",
                          style=wx.DD_DEFAULT_STYLE|wx.DD_NEW_DIR_BUTTON)

        # If the user selects OK, then we process the dialog's data.
        # This is done by getting the path data from the dialog - BEFORE
        # we destroy it. 
        if dlg.ShowModal() == wx.ID_OK:
            os.chdir(dlg.GetPath())
        # Only destroy a dialog after you're done with it.
        dlg.Destroy()


    def doReplay(self, event):
        if self.model == None: return
        num = len(self.model.time)
        start_time = time.time()
        for i in range(num):
            self.mycanvas.draw_all(i)
            _text = "Time %f"%self.model.time[i]
            self.SetStatusText(_text)
        end_time = time.time()
        self.SetStatusText(str(end_time-start_time))
        
        
    def Refresh(self, back=None, Rec=None):       
        self.mycanvas.draw_all()

    def doNewModel(self, event):
        dlg = wx.TextEntryDialog(self, "Enter the name of the model:",
                                    "Model Creation", "model")
        if dlg.ShowModal() == wx.ID_OK:
                if self.model != None:
                    self.model = None
                    self.tree.reSet()
                name_ = dlg.GetValue()
                self.model = Od_model(name_)
                self.model.add_ground()
                if self.mycanvas is  not None:
                    self.mycanvas.SetModel(self.model)
                    self.model.set_canvas(self.mycanvas)
                    self.model.set_frame(self)
                if self.tree is not None:
                    self.tree.SetModel(self.model)
                self.tree.OnUpdate()
                self.save_need = True
                dlg.Destroy()
#        else:
#            wx.MessageBox("A model already exists.", "Error", wx.OK | wx.CENTRE | wx.ICON_EXCLAMATION)

    def doSave(self, event):
        if self.filename == None:
          self.doSaveAs(event)
        elif self.model != None:
          self.model.save(self.filename)
        else:
            wx.MessageBox("No Model to be saved.", "Error", wx.OK | wx.CENTRE | wx.ICON_EXCLAMATION)

    def doSaveAs(self, event):
        if self.model != None:
            self.save_need = self.model.save_need
            _filename  = ""
            if self.filename is not None:
               _filename = self.filename
            if self.save_need == True:
                dlg = wx.FileDialog(
                    self, message="Save file as ...", defaultDir=os.getcwd(), 
                    defaultFile=_filename, wildcard=self.wildcard, style=wx.SAVE
                    )
                if dlg.ShowModal() == wx.ID_OK:
                    path = dlg.GetPath()
                    self.model.save(path)
                    dlg.Destroy()
                    self.save_need = False
        else:
            wx.MessageBox("No Model to be saved.", "Error", wx.OK | wx.CENTRE | wx.ICON_EXCLAMATION)

    def doExit(self, event):
        self.mycanvas.Destroy()
        self.Destroy()
        
    def doClose(self, event):
        self.doSave(event);
        self.model = None
        self.mycanvas.SetModel(self.model)

    def doNewBody(self, event):
        return
 
      
    def doOpen(self, event):
            dlg = wx.FileDialog(
                self, message="Open file...", defaultDir=os.getcwd(), 
                defaultFile="", wildcard=self.wildcard, style=wx.SAVE
                )
            if dlg.ShowModal() == wx.ID_OK:
                if self.model != None:
                    self.model = None
                    self.tree.reSet()
                path = dlg.GetPath()
                self.filename = os.path.splitext(os.path.basename(path))[0]
                path = dlg.GetPath()
                if path[-3:] == "xml":
                   self.model=read_newxml(path)
                   #self.model.add_ground()
            dlg.Destroy()
            self.tree.SetModel(self.model)
            self.tree.OnUpdate()
            self.mycanvas.SetModel(self.model)
            self.mycanvas.draw_all()
            if self.model is not None:
                self.model.set_canvas(self.mycanvas)
                self.model.set_frame(self)


    def doPyshell(self, event):
        dlg = od_pyshell()
        dlg.Show(True)


class Od_App(wx.App):
    def OnInit(self):
        self.frame = Od_Frame(None, -1, "LRMAS")
        self.frame.Show(True)
        self.SetTopWindow(self.frame)
        return True
      
    def SetModel(self, m):
       self.frame.SetModel(m)

def main():
    global _app
    _app = Od_App()
    _app.MainLoop()
    

def setModel(model):
   _app.SetModel(model)

if __name__ == "__main__":
    main()
