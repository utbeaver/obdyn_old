#################################################
import matplotlib.pyplot as plt
import numpy as np
import time
import sys, os
cwd=os.getcwd()
sys.path.append(cwd)
py3=0
if sys.hexversion >  50000000: 
    py3=1
from odsystem import *
pi=3.1415926
py3=0
if sys.hexversion == 50660592: 
    py3=1
def l2v(l):
    v=V3(l[0], l[1], l[2])
    return v
zeros = l2v([0.0, 0.0, 0.0])
from math import *
ll=sqrt(2.0)
d30=30.0*pi/180.0
lcos=ll*cos(d30)
lsin=ll*sin(d30)
h=lcos

b=[]
c=[]
fs=[]
for i in range(4):
    b.append(OdBody(i, "part"+str(i)))
for i in range(4):
    c.append(OdJoint(i+1, "revJ"+str(i)))
cm0 = OdMarker(1, "cm0")
b[0].add_cm_marker(cm0)
pos=[-lsin/2.0, lcos/2.0, 0.0]
ori=[d30, 0.0, 0.0]
cm1 = OdMarker(1, l2v(pos), l2v(ori), "cm1")
b[1].add_cm_marker(cm1)
cm2 = OdMarker(2, l2v([0.5, h, 0.0]), l2v([0.0, 0.0, 0.0]), "cm2")
b[2].add_cm_marker(cm2)
cm3 = OdMarker(3, l2v([1, h/2.0, 0.0]), l2v([0.0, 0.0, 0.0]), "cm3")
b[3].add_cm_marker(cm3)

omar1 = OdMarker(4, zeros, zeros,"mar1")
b[0].add_marker(omar1)
omar2 = OdMarker(5, l2v([0,-ll/2.0,0]), l2v([d30,0,0]),"mar2")
b[1].add_marker(omar2)
c[0].set_imarker(omar2)
c[0].set_jmarker(omar1)
omar3 = OdMarker(6, l2v([0.0, ll/2.0, 0.0]),l2v([d30,0,0]),"mar3")
b[1].add_marker(omar3)
omar4 = OdMarker(7, l2v([0.5-lsin, 0.0, 0.0]),zeros, "mar4")
b[2].add_marker(omar4)
c[1].set_imarker(omar4)
c[1].set_jmarker(omar3)
omar5 = OdMarker(8, l2v([0.5, 0.0, 0.0]),zeros, "mar5")
b[2].add_marker(omar5)
omar6 = OdMarker(9, l2v([0.0, h/2, 0.0]),zeros, "mar6")
#omar6 = OdMarker(9, [0.0, .65, 0.0], zeros, "mar6")
b[3].add_marker(omar6)
c[2].set_imarker(omar6)
c[2].set_jmarker(omar5)
omar7 = OdMarker(10, l2v([0.0, -h/2, 0.0]),zeros,"mar7")
b[3].add_marker(omar7)
omar8 = OdMarker(11, l2v([1.0, 0.0, 0.0]),zeros,"mar8")
b[0].add_marker(omar8)
c[3].set_imarker(omar7)
c[3].set_jmarker(omar8)
#c[0].set_expression("pi/6+sin(time)")
sys_ = OdSystem("4bar")
#sys_.setGravity([0,0,0])
omar9 = OdMarker(12, l2v([ll, 0.0, 0.0]),zeros,"mar112")
b[0].add_marker(omar9)
spdp=OdForce(13, "spdp")
spdp.spdpt()
spdp.set_imarker(omar3)
spdp.set_jmarker(omar9)
spdp.set_stiffness(10000)
spdp.set_damping(10)
spdp.set_distance(ll*ll)
fs.append(spdp)
spdp1=OdForce(14, "spdp1")
spdp1.spdpt()
spdp1.set_imarker(cm1)
spdp1.set_jmarker(cm3)
spdp1.set_stiffness(10000)
spdp1.set_damping(10)
spdp1.set_distance(0.7)
fs.append(spdp1)
for i in range(4):
    sys_.add_body(b[i])
for i in range(4):
    sys_.add_constraint(c[i])
for i in range(1):
    sys_.add_force(fs[i])
#sys_.displacement_ic()
#sys_.velocity_ic()
#sys_.acceleration_and_force_ic()
#sys_.static_analysis(15, 1.0e-10)
#sys_.kinematic_analysis(0.1, 10, 1.0e-10, 20)
#sys_.dynamic_analysis_bdf(0.1, 1.0e-3, 10, 0.1, 1.0e-6, 1.0e-3, 0)
start=time.time()
datas=[]
hht=0
if len(sys.argv)>1:
    if sys.argv[1]=="-h":
        hht=1
    if sys.argv[1]=="-b":
        hht=0
for i in range(500):
    t_=i*0.01
    data=[t_]
    types=["time"]
    if hht==1:
        sys_.dynamic_analysis_hht(t_, 1.0e-6, 6, 0.1, 1.0e-6, 0.001, 0)
    else:    
        sys_.dynamic_analysis_bdf(t_, 1.0e-5, 6, 0.1, 1.0e-6, 0.001, 0)
    for c_ in c:
        P=c_.disp()
        for i in range(c_.dofs()):
            types.append(c_.type(i))
            if c_.rotation(i)==1:
                data.append(P.get(i)*180.0/pi)
            else:    
                data.append(P.get(i))
    datas.append(data)        
end=time.time()
dt=end-start
datas=np.array(datas)    
end=time.time()
dimx, dimy=datas.shape
name_=os.path.splitext(os.path.basename(__file__))[0]
for i in range(1, dimy):
    plt.subplot(dimy-1, 1, i)
    plt.xlabel("%s %s %5.2f hht %d"%(name_, types[i], dt, hht))
    plt.plot(datas[:,0], datas[:, i])
    plt.grid()

if len(sys.argv)>1:
	np.save(name_, datas)
else:
	plt.show()
if len(sys.argv)>1:
    if sys.argv[1]=="-B" or sys.argv[1]=="-H" :
        plt.tight_layout()
	plt.show()


if hht==1:
    sys_.numdif()
sys.exit(0)

