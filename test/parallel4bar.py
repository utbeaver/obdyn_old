####################################################
import matplotlib.pyplot as plt
import numpy as np
import sys, os, time
sys.path.append(os.getcwd())
from odsystem import *


b=[]
c=[]
for i in range(4):
    b.append(OdBody(i, "part"+str(i)))
for i in range(4):
    c.append(OdJoint(i+1, "revJ"+str(i)))
cm0 = OdMarker(1, "cm0")
b[0].add_cm_marker(cm0)
omar1 = OdMarker(4, V3(), V3(),"mar1")
b[0].add_marker(omar1)

cm1 = OdMarker(1, V3(0, 0.5, 0), V3(),"cm1")
b[1].add_cm_marker(cm1)
omar2 = OdMarker(5, V3(0.0, -0.5, 0.0), V3(), "mar2")
b[1].add_marker(omar2)
omar3 = OdMarker(6, V3(0.0, 0.5, 0.0), V3(), "mar3")
b[1].add_marker(omar3)

c[0].set_imarker(omar2)
c[0].set_jmarker(omar1)

cm2 = OdMarker(2, V3(0.5, 1.0, 0.0), V3(0.0, 0.0, 0.0), "cm2")
b[2].add_cm_marker(cm2)
omar4 = OdMarker(7, V3(-0.5, 0.0, 0.0), V3(), "mar4")
b[2].add_marker(omar4)
omar5 = OdMarker(8, V3(0.5, 0.0, 0.0), V3(), "mar5")
b[2].add_marker(omar5)

c[1].set_imarker(omar4)
c[1].set_jmarker(omar3)

cm3 = OdMarker(3, V3(1, 0.5, 0.0), V3(0.0, 0.0, 0.0), "cm3")
b[3].add_cm_marker(cm3)

omar6 = OdMarker(9, V3(0.0, .5, 0.0), V3(), "mar6")
#omar6 = OdMarker(9, V3(0.0, .65, 0.0), V3(), "mar6")
b[3].add_marker(omar6)

c[2].set_imarker(omar6)
c[2].set_jmarker(omar5)

omar7 = OdMarker(10, V3(0.0, -.5, 0.0), V3(),"mar7")
b[3].add_marker(omar7)
omar8 = OdMarker(11, V3(1.0, 0.0, 0.0), V3(),"mar8")
b[0].add_marker(omar8)
c[3].set_imarker(omar7)
c[3].set_jmarker(omar8)
c[0].set_expr("sin(time)")
sys_ = OdSystem("4bar")
#sys_.setGravity([0,0,0])
for i in range(4):
    sys_.add_body(b[i])
for i in range(4):
    sys_.add_constraint(c[i])
sys_.displacement_ic()
#sys_.velocity_ic()
#sys_.acceleration_and_force_ic()
#sys_.static_analysis(15, 1.0e-10)
#sys_.kinematic_analysis(0.1, 10, 1.0e-10, 20)
#sys_.dynamic_analysis_bdf(0.1, 1.0e-3, 10, 0.1, 1.0e-6, 1.0e-3, 0)
datas=[]
hht=3
if len(sys.argv)>1:
    if sys.argv[1]=="-h":
        hht=1
    if sys.argv[1]=="-b":
        hht=0
start=time.time()
for i in range(50):
    t_=i*0.1
    data=[t_]
    types=["time"]
    sys_.kinematic_analysis(t_, 10, 1.0e-10, 20)
    #if hht==1:
    #    sys_.dynamic_analysis_hht(t_, 1.0e-6, 6, 0.1, 1.0e-6, 0.001, 1)
    #else:    
    #    sys_.dynamic_analysis_bdf(t_, 1.0e-6, 6, 0.1, 1.0e-6, 0.001, 0)
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
