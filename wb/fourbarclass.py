from math import *
from od_model import *
from od_utils import *

class fourbar(Od_model):
   def __init__(self, _name = "fourbar"):
     Od_model.__init__(self, _name)
     #self.displayable.append("angle");
     self.editable.append("angle");
     #self.displayable.append("lengthLink1");
     self.editable.append("lengthLink1");
     #self.displayable.append("lengthLink2");
     self.editable.append("lengthLink2");
     #self.displayable.append("lengthLink3");
     self.editable.append("lengthLink3");
     #self.displayable.append("lengthLink4");
     self.editable.append("lengthLink4");
     self.datas["angle"]=[90.0, "float"]        
     self.datas["lengthLink1"]=[1.0, "float"]        
     self.datas["lengthLink2"]=[1.0, "float"]        
     self.datas["lengthLink3"]=[1.0, "float"]        
     self.datas["lengthLink4"]=[1.0, "float"]        
     self.initialized = False

   def initialize(self, sys=self): 
     #define the cm location
     if self.initialized is True: return
     self.initialized = True
     angle = 3.1415926/180.0*self.get("angle")
     link1 = self.get("lengthLink1")
     link2 = self.get("lengthLink2")
     link3 = self.get("lengthLink3")
     link4 = self.get("lengthLink4")
     x1 = cos(angle)*link1/2.0
     y1 = sin(angle)*link1/2.0
     x2 = 2*x1+link2*.5
     y2 = 2*y1
     x4 = link4
     y4 = 0.0
     x3 = x4
     y3 = link3*.5
     
     self.add_ground()
     ground = self.ground()
     
     #create crank
     crank = od_body("crank")
     crank.set("Position", [x1, y1, 0.0])
     crank.set("Ref_Orientation", [0.0, 90.0, 0.0])
     crank.adding(od_link(.2, .2, link1))
     #create a joint
     jnt1 = joint_create("revolute", crank, ground, [0.0, 0.0, 0.0], [0.0, 0.0, 0.0])

     #create link
     link = od_body("link")
     link.set("Position", [x2, y2, 0.0])
     link.set("Ref_Orientation", [90.0, 90.0, 0.0])
     link.adding(od_link(.2, .2, link2))
     #lnk2.set("Orientation", [30.0, 0.0, 0.0])
     jnt2 = joint_create("revolute", link, crank, [0.0, y1*2, 0.0], [0.0, 0.0, 0.0])

     #create coupler
     coupler = od_body("coupler")
     coupler.set("Position", [x3, y3, 0.0])
     coupler.set("Ref_Orientation", [0.0, 90.0, 0.0])
     coupler.adding(od_link(.2, .2, link3))
     jnt4 = joint_create("revolute", coupler, ground, [0.0, x4, 0.0], [0.0, x4, 0.0])
     jnt3 = joint_create("revolute", link, coupler, [2*x1+link2, 2*y1, 0.0], [0.0, 0.0, 0.0])
     self.adding(crank)
     self.adding(link)
     self.adding(coupler)
     self.adding(jnt1)
     self.adding(jnt2)
     self.adding(jnt3)
     self.adding(jnt4)





