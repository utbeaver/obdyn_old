from od_utils import *
from od_model import *
from math import *
import sixbar

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
     jnt1 = sixbar.joint_create("revolute", lnk1, ground, [0.0, 0.0, 0.0], [0.0, 0.0, 0.0])
  #create link 2
     lnk2 = od_body("link2")
     lnk2.set("Position", [sin(d30), -2-cos(d30), 0.0])
     lnk2.set("Orientation", [30.0, 0.0, 0.0])
     lnk2.set("Ref_Orientation", [0.0, 90.0, 0.0])
     lnk2.adding(od_link(.2, .2, 2))

     jnt2 = sixbar.joint_create("revolute", lnk2, lnk1, [0.0, -2.0, 0.0], [0.0, 0.0, 0.0])
  #create link 3
     lnk3 = od_body("link3")
     lnk3.set("Position", [2.0, -1.0, 0])
     lnk3.set("Ref_Orientation", [0.0, 90.0, 0.0])
     lnk3.adding(od_link(.2, .2, 2))

     jnt3 = sixbar.joint_create("revolute", lnk3, ground, [2.0, 0.0,0.0], [0.0, 0.0, 0.0])

     #create link4
     lnk4 = od_body("link4")
     lnk4.set("Position", [2-sin(d30), -2-cos(d30), 0.0])
     lnk4.set("Orientation", [-30.0, 0.0, 0.0])
     lnk4.set("Ref_Orientation", [0.0, 90.0, 0.0])
     lnk4.adding(od_link(.2, .2, 2))
     jnt4 = sixbar.joint_create("revolute", lnk4, lnk3, [2.0, -2.0,0.0], [0.0, 0.0, 0.0])
     jnt5 = sixbar.joint_create("revolute", lnk4, lnk2, [1.0, -2.0,0.0], [0.0, 0.0, 0.0])

     force = od_spdpt("force1")
     force.set("Stiffness", 10.0)
     force.set("Damping", 1.0)
     force.set("Distance", 1.5)
     force.set_imarker(jnt2.get_jmarker())
     force.set_jmarker(jnt4.get_jmarker())
     model.adding(lnk1)
     model.adding(lnk2)
     model.adding(lnk3)
     model.adding(lnk4)
     model.adding(jnt1)
     model.adding(jnt2)
     model.adding(jnt3)
     model.adding(jnt4)
     model.adding(jnt5)
     model.adding(force)
     print model
     return model

if __name__ == "__main__":
    _model = model_create()
    setModel(_model)
