##############################################
import numpy as np
import matplotlib.pyplot as plt
import time, sys, os
sys.path.append(os.getcwd())
from odsystem import *
zeros = [0.0, 0.0, 0.0]
d2r=np.pi/180.0
d45=45*d2r
c45=np.cos(d45)
s45=np.sin(d45)


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
b1mar1=OdMarker(4, V3(1, 0, 0), V3(), "b1I") 
b[1].add_marker(b1mar1)
b1mar23=OdMarker(4, V3(1, 0, 0), V3(), "b2_3") 
b[1].add_marker(b1mar23)

c[0].spherical()
c[0].set_imarker(b1mar1)
c[0].set_jmarker(gmar1)

#spdp=OdJointSPDP(10, "spdpt")
#spdp.set_joint(c[0], 0)
#spdp.set_stiffness(3.14*3.14*4*40)
#spdp.set_damping(1)
#spdp.set_distance(0)
#spdp.set_force(0)
#fs.append(spdp)


cm2 = OdMarker(3, V3(c45*3, -s45*3, 0), V3(d45, 0, 0), "cm1")
b[2].add_cm_marker(cm2)
b2mar1=OdMarker(4, V3(1, 0, 0), V3(), "b2I") 
b[2].add_marker(b2mar1)

c[1].set_imarker(b2mar1)
c[1].set_jmarker(b1mar23)
c[1].txyrz()
sys_ = OdSystem("revJT2R1")

for i in b: sys_.add_body(i)
for i in c: sys_.add_constraint(i)
for i in fs: sys_.add_joint_spdp(i)    
hht=0
if len(sys.argv)>1:
    if sys.argv[1]=="-h":
        hht=1
    if sys.argv[1]=="-b":
        hht=0
start=time.time()
datas=[]
for i in range(905):
    t_=i*0.01
    data=[t_]
    types=["time"]
    if hht==1:
        sys_.dynamic_analysis_hht(t_, 1.0e-3, 10, 0.01, 1.0e-6, 1.0e-3, 0)
    else:    
        sys_.dynamic_analysis_bdf(t_, 1.0e-5, 10, 0.01, 1.0e-6, 1.0e-3, 0)
    for c_ in c:
        P=c_.disp()
        for i in range(c_.dofs()):
            types.append(c_.type(i))
            if c_.rotation(i)==1:
                data.append(P.get(i)*180.0/np.pi)
            else:    
                data.append(P.get(i))
    datas.append(data)        
datas=np.array(datas)
end=time.time()
dt= end-start

name_=os.path.splitext(os.path.basename(__file__))[0]
import postutils
postutils.ppt(datas, types, dt, hht, name_, sys.argv)
sys.exit(0)
