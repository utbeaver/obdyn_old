################################################
import numpy as np
import matplotlib.pyplot as plt
import time, sys, os
sys.path.append(os.getcwd())
d2r=np.pi/180.0
d45=60*d2r
c45=np.cos(d45)
s45=np.sin(d45)

from odsystem import *

b=[]
c=[]
fs=[]
for i in range(3):
    b.append(OdBody(i, "part"+str(i)))
for i in range(2):
    c.append(OdJoint(i+1, "Js"+str(i)))
cm0 = OdMarker(1, "cm0")
b[0].add_cm_marker(cm0)
gmar1=OdMarker(2, V3(), V3(45*d2r*0, 90*d2r*0, 0), "gndJ")
b[0].add_marker(gmar1)
cm1 = OdMarker(3, V3(c45, -s45, 0), V3(d45, 0, 0), "cm1")
b[1].add_cm_marker(cm1)
#b1mar1=OdMarker(4, V3(0, 0, 0), V3(), "b1I") 
b1mar1=OdMarker(4, gmar1, V3(0, 0, 0), V3(), "b1I") 
b[1].add_global_marker(b1mar1)
b1mar23=OdMarker(4, V3(1, 0, 0), V3(), "b2_3") 
b[1].add_marker(b1mar23)

c[0].free()
#c[0].spherical()
#c[0].revolute()
c[0].set_imarker(b1mar1)
c[0].set_jmarker(gmar1)
scale=10
cscale=0.0
spdp=OdJointSPDP(10, "spdpt")
spdp.setJoint(c[0], 0)
spdp.set_stiffness(3.14*3.14*scale)
spdp.set_damping(1*cscale)
spdp.set_distance(0)
spdp.set_force(0)
fs.append(spdp)

spdp1=OdJointSPDP(11, "spdpt1")
spdp1.setJoint(c[0], 1)
spdp1.set_stiffness(3.14*3.14*scale)
spdp1.set_damping(2*cscale)
spdp1.set_distance(0)
spdp1.set_force(0)
fs.append(spdp1)

spdp2=OdJointSPDP(12, "spdpt2")
spdp2.setJoint(c[0], 5)
spdp2.set_stiffness(3.14*3.14*scale)
spdp2.set_damping(2*cscale)
spdp2.set_distance(0)
spdp2.set_force(0)
fs.append(spdp2)

cm2 = OdMarker(3, V3(c45*3, -s45*3, 0), V3(d45, 0, 0), "cm2")
b[2].add_cm_marker(cm2)
b2mar1=OdMarker(4, V3(1, 0, 0), V3(), "b2I") 
b[2].add_marker(b2mar1)

c[1].set_imarker(b2mar1)
c[1].set_jmarker(b1mar23)
c[1].txyrz()
spdp3=OdJointSPDP(13, "spdpt3")
spdp3.setJoint(c[1], 0)
spdp3.set_stiffness(3.14*3.14*scale)
spdp3.set_damping(2*cscale)
spdp3.set_distance(0)
spdp3.set_force(0)
fs.append(spdp3)

spdp4=OdJointSPDP(13, "spdpt4")
spdp4.setJoint(c[1], 0)
spdp4.set_stiffness(3.14*3.14*scale)
spdp4.set_damping(2*cscale)
spdp4.set_distance(0)
spdp4.set_force(0)
fs.append(spdp4)
spdp5=OdJointSPDP(14, "spdpt5")
spdp5.setJoint(c[1], 1)
spdp5.set_stiffness(3.14*3.14*scale)
spdp5.set_damping(2*cscale)
spdp5.set_distance(0)
spdp5.set_force(0)
fs.append(spdp5)
spdp6=OdJointSPDP(14, "spdpt6")
spdp6.setJoint(c[1], 2)
spdp6.set_stiffness(3.14*3.14*scale)
spdp6.set_damping(2*cscale)
spdp6.set_distance(0)
spdp6.set_force(0)
fs.append(spdp6)

sys_ = OdSystem("FREET2R1")

for i in b: sys_.add_body(i)
for i in c: sys_.add_constraint(i)
for i in fs: sys_.add_joint_spdp(i)    
hht=0
if len(sys.argv)>1:
    if sys.argv[1]=="-h" or sys.argv[1]=="-H" :
        hht=1
    if sys.argv[1]=="-b" or sys.argv[1]=="-B":
        hht=0
start=time.time()
datas=[]
for i in range(150):
    t_=i*0.01
    data=[t_]
    types=["time"]
    if hht==1:
        sys_.dynamic_analysis_hht(t_, 1.0e-5, 10, 0.01, 1.0e-6, 1.0e-3, 0)
    else:    
        sys_.dynamic_analysis_bdf(t_, 1.0e-3, 10, 0.01, 1.0e-6, 1.0e-3, 0)
    for c_ in c:
        P=c_.disp()
        for i in range(c_.dofs()):
            types.append(c_.type(i))
            if c_.rotation(i)==1:
                data.append(P.get(i)*180.0/np.pi)
            else:    
                data.append(P.get(i))
    datas.append(data)        
sys_.numdif()    
end=time.time()
dt= end-start
datas=np.array(datas)    
name_=os.path.splitext(os.path.basename(__file__))[0]

import postutils
postutils.ppt(datas, types, dt, hht, name_, sys.argv)
sys_.numdif()
sys.exit(0)
