################################################
import numpy as np
import matplotlib.pyplot as plt
import time, sys, os
sys.path.append(os.getcwd())
d2r=np.pi/180.0
d45=45*d2r
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
gmar1=OdMarker(2, V3(), V3(), "gndJ") 
b[0].add_marker(gmar1)
cm1 = OdMarker(3, V3(1, -0, 0), V3(), "cm1")
b[1].add_cm_marker(cm1)
b12gnd = OdMarker(3, V3(0, 0, 0), V3(), "b12gnd")
b[1].add_global_marker(b12gnd)
c[0].set_imarker(b12gnd)
c[0].set_jmarker(gmar1)

b12lkn2 = OdMarker(3, V3(1*2, -0*2, 0), V3(), "b1_2lnk2")
b[1].add_global_marker(b12lkn2)
cm2 = OdMarker(3, b12lkn2, V3(c45, -s45, 0), V3(-d45, 0, 0), "cm2")
b[2].add_cm_marker(cm2)
b22lkn1 = OdMarker(3, V3(), V3(), "b2_2lnk1")
b[2].add_global_marker(b22lkn1)
c[1].set_imarker(b22lkn1)
c[1].set_jmarker(b12lkn2)
c[1].spherical()


sys_ = OdSystem("links")

for i in b: sys_.add_body(i)
for i in c: sys_.add_constraint(i)
for i in fs: sys_.add_joint_spdp(i)    
hht=1
if len(sys.argv)>1:
    if sys.argv[1]=="-h":
        hht=1
    if sys.argv[1]=="-b":
        hht=0
datas=[]
start=time.time()
for i in range(905):
    t_=i*0.01
    data=[t_]
    types=["time"]
    if hht==1:
        sys_.dynamic_analysis_hht(t_, 1.0e-3, 10, 0.01, 1.0e-6, 1.0e-3, 1)
    else:    
        sys_.dynamic_analysis_bdf(t_, 1.0e-5, 10, 0.01, 1.0e-6, 1.0e-3, 1)
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
