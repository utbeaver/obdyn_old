import matplotlib.pyplot as plt
import time, os, sys
import numpy as np
pi=3.1415926
zeros = [0.0, 0.0, 0.0]
from math import *
ll=sqrt(2.0)
d30=30.0*pi/180.0
lcos=ll*cos(d30)
lsin=ll*sin(d30)
h=lcos
cwd=os.getcwd()
sys.path.append(cwd)
from odsystem import *
py3=0
if sys.hexversion >  50000000: 
    py3=1
def l2v(l):
    v=V3(l[0], l[1], l[2])
    return v	

print "Four-bar linkage"
print "Topology"
print "bodies:       0->1->2->3->0"
print "connections:    1  2  3  4"

b=[]
c=[]
for i in range(4):
    b.append(OdBody(i, "part"+str(i)))
for i in range(4):
    c.append(OdJoint(i+1, "revJ"+str(i)))
cm0 = OdMarker(1, "cm0")
print b[0].add_cm_marker(cm0)
pos=[-lsin/2.0, lcos/2.0, 0.0]
ori=[d30, 0.0, 0.0]
cm1 = OdMarker(1, l2v(pos), l2v(ori), "cm1")
print b[1].add_cm_marker(cm1)
cm2 = OdMarker(2, V3(0.5, h, 0.0), V3(0.0, 0.0, 0.0), "cm2")
print b[2].add_cm_marker(cm2)
cm3 = OdMarker(3, V3(1, h/2.0, 0.0), V3(0.0, 0.0, 0.0), "cm3")
print b[3].add_cm_marker(cm3)

omar1 = OdMarker(4, V3(), V3(),"mar1")
print b[0].add_marker(omar1)
omar2 = OdMarker(5, V3(0,-ll/2.0,0), V3(d30,0,0), "mar2")
print b[1].add_marker(omar2)
print c[0].set_imarker(omar2)
print c[0].set_jmarker(omar1)
omar3 = OdMarker(6, V3(0.0, ll/2.0, 0.0), V3(d30,0,0), "mar3")
b[1].add_marker(omar3)
omar4 = OdMarker(7, V3(0.5-lsin, 0.0, 0.0), V3(), "mar4")
b[2].add_marker(omar4)
c[1].set_imarker(omar4)
c[1].set_jmarker(omar3)
omar5 = OdMarker(8, V3(0.5, 0.0, 0.0), V3(), "mar5")
b[2].add_marker(omar5)
omar6 = OdMarker(9, V3(0.0, h/2, 0.0), V3(), "mar6")
#omar6 = OdMarker(9, V3(0.0, .65, 0.0), V3(), "mar6")
b[3].add_marker(omar6)
c[2].set_imarker(omar6)
c[2].set_jmarker(omar5)
omar7 = OdMarker(10, V3(0.0, -h/2, 0.0), V3(),"mar7")
b[3].add_marker(omar7)
omar8 = OdMarker(11, V3(1.0, 0.0, 0.0), V3(),"mar8")
b[0].add_marker(omar8)
c[3].set_imarker(omar7)
c[3].set_jmarker(omar8)
#c[0].set_expression("pi/6+sin(time)")
sys_ = OdSystem("4bar")
#sys_.setGravity(V3(0,0,0])
for i in range(4):
    sys_.add_body(b[i])
for i in range(4):
    sys_.add_constraint(c[i])
start=time.time()
datas=[]
hht=0
for i in range(1000):
    t_=0.01*i
    sys_.dynamic_analysis_bdf(t_, 1.0e-5, 6, 0.1, 1.0e-6, 0.001, 0)
    data=[t_]
    for c_ in c:
        P=c_.disp()
        for i in range(c_.dofs()):
            if c_.rotation(i)==1:
                data.append(P.get(i)*180.0/pi)
            else:    
                data.append(P.get(i))
    datas.append(data)        
end=time.time()
dt=end-start
datas=np.array(datas)    
end=time.time()
plt.plot(datas[:,0], datas[:,1], datas[:,0], datas[:,2], datas[:,0], datas[:,3], datas[:,0], datas[:,4], )#, t, x3)
plt.title("hht %d, time %f"%(hht, dt))
plt.grid()
plt.show()
