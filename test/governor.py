############################################
import matplotlib.pyplot as plt
import time, sys, os
import numpy as np
cwd=os.getcwd()
sys.path.append(cwd)
from odsystem import *
py3=0
if sys.hexversion == 50660592: 
    py3=1
d2r=np.pi/180.0
zeros = [0.0, 0.0, 0.0]
from math import *
r90=-90.0*d2r

b=[]
c=[]
fs=[]
for i in range(8):
    b.append(OdBody(i, "part"+str(i)))
for i in range(8):
    c.append(OdJoint(i+1, "revJ"+str(i)))

cm0 = OdMarker(1, V3(0,0,0), V3(0, r90, 0), "cm0")
b[0].add_cm_marker(cm0)

cm1 = OdMarker(10, V3(0,0.5,0), V3(0, r90, 0), "cm1")
b[1].add_cm_marker(cm1)
b1_2gnd = OdMarker(11, V3(0,0.0,0), V3(0, r90, 0), "b1_2gnd")
b[1].add_global_marker(b1_2gnd)
b1_top = OdMarker(12, V3(0,1.0,0), V3(0, r90, 0), "b1_top")
b[1].add_global_marker(b1_top)
b[1].setMass(200.0)
b[1].setI(V6(25, 50, 25,0, 0,0))

c[0].set_imarker(b1_2gnd)
c[0].set_jmarker(cm0)
c[0].set_expr("3*sin(time*20*pi)")
#c[0].set_expr("2.23*time*time*time*(10-5*time+0.6*time*time)")
#c[0].set_expr("2.23*time^3*(10-5*time+0.6*time^2)")
#c[0].set_expr("50*time")

cm2 = OdMarker(20, V3(0, 0.5, 0), V3(0, r90, 0), "cm2")
b[2].add_cm_marker(cm2)
scale=1
c[1].set_imarker(cm2)
c[1].set_jmarker(b1_top)
c[1].translational()
jforce=OdJointSPDP(25, "spring")
jforce.setJoint(c[1])
jforce.set_stiffness(1000000*scale)
jforce.set_damping(200)
jforce.set_distance(-0.5)
fs.append(jforce)
#
b1_2_b3=OdMarker(13, V3(0.0, 1.0, 0), V3(), "b1_2_b3")
b[1].add_global_marker(b1_2_b3)
b1_2_b5=OdMarker(51, V3(0.0, 1.0, 0), V3(), "b1_2_b5")
b[1].add_global_marker(b1_2_b5)
#
cm3=OdMarker(30, V3(0.5, 0, 0.5), V3(-45*d2r, 0, 0), "cm3")
b[3].add_cm_marker(cm3)
b3_2_b1=OdMarker(31, b1_2_b3, V3(), V3(), "b3_2_b1")
b[3].add_global_marker(b3_2_b1)
c[2].set_imarker(b3_2_b1)
c[2].set_jmarker(b1_2_b3)
#
cm5=OdMarker(50, V3(-0.5, 0, 0.5), V3(45*d2r, 0, 0), "cm5")
b[5].add_cm_marker(cm5)
b5_2_b1=OdMarker(51, b1_2_b5, V3(), V3(), "b5_2_b1")
b[5].add_global_marker(b5_2_b1)
c[5].set_imarker(b5_2_b1)
c[5].set_jmarker(b1_2_b5)
#
cm4=OdMarker(40, V3(0.5/4, 0.5+0.5/4, 0), V3(45*d2r,0, 0), "cm4")
b[4].add_cm_marker(cm4)
b4_2_b3=OdMarker(41, V3(0.5/2, 0.75, 0), V3(), "b4_2_b3")
b[4].add_global_marker(b4_2_b3)
b3_2_b4=OdMarker(33, b4_2_b3, V3(), V3(), "b3_2_b4")
b[3].add_global_marker(b3_2_b4)

c[3].set_imarker(b3_2_b4)
c[3].set_jmarker(b4_2_b3)

cm6=OdMarker(60, V3(-0.5/4, 0.5+0.5/4, 0), V3((90+45)*d2r,0, 0), "cm6")
b[6].add_cm_marker(cm6)
b6_2_b5=OdMarker(61, V3(-0.5/2, 0.75, 0), V3(), "b6_2_b5")
b[6].add_global_marker(b6_2_b5)
b5_2_b6=OdMarker(53, V3(-0.5/2, 0.75, 0), V3(), "b5_2_b6")
b[5].add_global_marker(b5_2_b6)

c[6].set_imarker(b6_2_b5)
c[6].set_jmarker(b5_2_b6)

b4_2_b2=OdMarker(42, V3(0, 0.5, 0), V3(), "B4_2B2")
b[4].add_global_marker(b4_2_b2)
b2_2_b4=OdMarker(24, V3(0, 0.5, 0), V3(), "B2_2B4")
b[2].add_global_marker(b2_2_b4)
c[4].set_imarker(b4_2_b2)
c[4].set_jmarker(b2_2_b4)

b6_2_b2=OdMarker(62, V3(0, 0.5, 0), V3(), "B6_2B2")
b[6].add_global_marker(b6_2_b2)
b2_2_b6=OdMarker(26, V3(0, 0.5, 0), V3(), "B2_2B6")
b[2].add_global_marker(b2_2_b6)
c[7].set_imarker(b6_2_b2)
c[7].set_jmarker(b2_2_b6)

sys_ = OdSystem("governor")
for i in b:
    sys_.add_body(i)
for i in c:
    sys_.add_constraint(i)
for i in fs: sys_.add_joint_spdp(i)
hht=1    
if len(sys.argv)>1:
    if sys.argv[1]=="-h":
        hht=1
    if sys.argv[1]=="-b":
        hht=0
datas=[]
start=time.time()
#msg=sys_.static_analysis(20, 10e-3)
#if msg != "Success!":
#    print msg
#    sys.exit(0)
for i in range(150):
    t_=i*0.01
    data=[t_]
    types=["time"]
    if hht==1:
        sys_.dynamic_analysis_hht(t_, 1.0e-8, 6, 0.01, 1.0e-6, 0.001, 1)
    else:    
        sys_.dynamic_analysis_bdf(t_, 1.0e-3, 6, 0.01, 1.0e-6, 0.001, 1)
    for c_ in c:
        P=c_.vel()
        for i in range(c_.dofs()):
            types.append(c_.type(i))
            if c_.rotation(i)==1:
                data.append(P.get(i)*180.0/np.pi)
            else:    
                data.append(P.get(i))
    datas.append(data)        
datas=np.array(datas)    
end=time.time()
dt=end-start
name_=os.path.splitext(os.path.basename(__file__))[0]

import postutils
postutils.ppt(datas, types, dt, hht, name_, sys.argv)
sys.exit(0)
