
# This model contains a long list of links joined by revolute joints

from od_utils import *
from od_model import *
from math import *

bodyList=[]
jointList=[]

def create_link(num, pos=[0.0, 0.0, 0.0], ori=[0.0, 0.0, 0.0]):
  link= od_body("link"+str(num))
  link.set("Position", pos)
  link.adding(od_box(2, .5, .5))
  link.set("Ref_Orientation", [0.0, 90.0, 0.0])
  return link


def model_create():
  model = Od_model()
  model.add_ground()
  ground = model.ground()
  bodyList.append(ground)
  #create N links
  N=10
  for i in range(N):
    x = i*2+1
    link=create_link(i+1, [x, 0, 0])
    bodyList.append(link)
  for b in bodyList[1:]:
    model.adding(b)
  #create joints
  for i in range(N):
    x = i*2
    joint=joint_create("revolute", bodyList[i+1], bodyList[i], [x, 0.0, 0.0], [0.0, 0.0, 0.0], "joint"+str(i+1))
    jointList.append(joint)
  for j in jointList:  
    model.adding(j)      

  return model

if __name__ == "__main__":
    _model = model_create()
    setModel(_model)
