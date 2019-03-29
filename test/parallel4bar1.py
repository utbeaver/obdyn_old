from odsystem import *
import matplotlib.pyplot as plt
import time
pi=3.1415926
zeros = [0.0, 0.0, 0.0]
from math import *
ll=sqrt(2.0)
d30=30.0*pi/180.0
lcos=ll*cos(d30)
lsin=ll*sin(d30)
h=lcos

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
cm1 = OdMarker(1, pos, ori, "cm1")
print b[1].add_cm_marker(cm1)
cm2 = OdMarker(2, [0.5, h, 0.0], [0.0, 0.0, 0.0], "cm2")
print b[2].add_cm_marker(cm2)
cm3 = OdMarker(3, [1, h/2.0, 0.0], [0.0, 0.0, 0.0], "cm3")
print b[3].add_cm_marker(cm3)

omar1 = OdMarker(4, zeros, zeros,"mar1")
print b[0].add_marker(omar1)
omar2 = OdMarker(5, [0,-ll/2.0,0], [-d30,0,0], "mar2")
print b[1].add_marker(omar2)
print c[0].set_imarker(omar2)
print c[0].set_jmarker(omar1)
omar3 = OdMarker(6, [0.0, ll/2.0, 0.0], [-d30,0,0], "mar3")
b[1].add_marker(omar3)
omar4 = OdMarker(7, [-0.5-lsin, 0.0, 0.0], zeros, "mar4")
b[2].add_marker(omar4)
c[1].set_imarker(omar4)
c[1].set_jmarker(omar3)
omar5 = OdMarker(8, [0.5, 0.0, 0.0], zeros, "mar5")
b[2].add_marker(omar5)
omar6 = OdMarker(9, [0.0, h/2, 0.0], zeros, "mar6")
#omar6 = OdMarker(9, [0.0, .65, 0.0], zeros, "mar6")
b[3].add_marker(omar6)
c[2].set_imarker(omar6)
c[2].set_jmarker(omar5)
omar7 = OdMarker(10, [0.0, -h/2, 0.0], zeros,"mar7")
b[3].add_marker(omar7)
omar8 = OdMarker(11, [1.0, 0.0, 0.0], zeros,"mar8")
b[0].add_marker(omar8)
c[3].set_imarker(omar7)
c[3].set_jmarker(omar8)
#c[0].set_expression("pi/6+sin(time)")
sys_ = OdSystem("4bar")
#sys_.setGravity([0,0,0])
for i in range(4):
    sys_.add_body(b[i])
for i in range(4):
    sys_.add_constraint(c[i])
#sys_.displacement_ic()
#sys_.velocity_ic()
#sys_.acceleration_and_force_ic()
#sys_.static_analysis(15, 1.0e-10)
#sys_.kinematic_analysis(0.1, 10, 1.0e-10, 20)
#sys_.dynamic_analysis_bdf(0.1, 1.0e-3, 10, 0.1, 1.0e-6, 1.0e-3, 0)
x1=[]
y1=[]
x2=[]
y2=[]
x3=[]
y3=[]
start=time.time()
for i in range(1000):
    sys_.dynamic_analysis_bdf(i*0.01, 1.0e-5, 6, 0.1, 1.0e-6, 0.001, 0)
    xyz=omar4.position(0)
    xyz2=omar6.position(0)
    xyz3=cm3.position(0)
    x1.append(xyz[0])
    y1.append(xyz[1])
    x2.append(xyz2[0])
    y2.append(xyz2[1])
    x3.append(xyz3[0])
    y3.append(xyz3[1])
end=time.time()
print end-start
plt.plot(x1, y1, x2, y2, x3, y3)
plt.grid()
plt.show()
