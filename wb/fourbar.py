# This model contains 5 bars in a closed loop. This model is intially created for static analysis,

from od_utils import *
from od_model import *
from math import *


     
def model_create():
     model = Od_model()
     model.add_ground()
     ground = model.ground()
    
  #create link 1
     d30 = 30.0*pi/180.0
     lnk1 = od_body("link1")
     lnk1.set("Position", [0.0, -1.0, 0])
     lnk1.set("Ref_Orientation", [0.0, 90.0, 0.0])
     lnk1.adding(od_link(.2, .2, 2))
     
  #create a rev joint between ground and link1
     jnt1 = joint_create("revolute", lnk1, ground, [0.0, 0.0, 0.0], [0.0, 0.0, 0.0])
     
  #create joint betw
     lnk2 = od_body("link2")
     lnk2.set("Position", [sin(d30) , -2.0-cos(d30), 0])
     lnk2.set("Ref_Orientation", [0.0, 90.0, 0.0])
     lnk2.adding(od_link(.2, .2, 2))
     lnk2.set("Orientation", [30.0, 0.0, 0.0])
    
     jnt2 = joint_create("revolute", lnk2, lnk1, [0.0, -2.0, 0.0], [0.0, 0.0, 0.0])

     lnk4 = od_body("link4")
     lnk4.set("Position", [2.0, -1.0, 0])
     lnk4.set("Ref_Orientation", [0.0, 90.0, 0.0])
     lnk4.adding(od_link(.2, .2, 2))
     
     jnt4 = joint_create("revolute", lnk4, ground, [2.0, 0.0,0.0], [0.0, 0.0, 0.0])

     lnk5 = od_body("link5")
     lnk5.set("Position", [2.0-sin(d30), -2.0-cos(d30), 0])
     lnk5.set("Ref_Orientation", [0.0, 90.0, 0.0])
     lnk5.adding(od_link(.2, .2, 2))
     lnk5.set("Orientation", [-30.0, 0.0, 0.0])
     
     jnt5 = joint_create("revolute", lnk5, lnk4, [2.0, -2.0, 0.0], [0.0,0.0,0.0])

     jnt7 = joint_create("revolute", lnk5, lnk2, [1.0, -2.0-2.0*cos(d30), 0.0], [0.0, 0.0, 0.0])
     model.adding(lnk1)
     model.adding(lnk2)
     model.adding(lnk4)
     model.adding(lnk5)
     model.adding(jnt1)
     model.adding(jnt2)
     model.adding(jnt4)
     model.adding(jnt5)
     model.adding(jnt7)
     return model
    
if __name__ == "__main__":
    _model = model_create()
    setModel(_model)
