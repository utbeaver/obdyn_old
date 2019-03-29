import numpy as np
from odsystem import *
import matplotlib.pyplot as plt
import time
zeros = [0.0, 0.0, 0.0]
d2r=np.pi/180.0
d30=30*d2r
ll=1.0
lc30=np.cos(d30)*ll
ls30=np.sin(d30)*ll

b=[]
c=[]
fs=[]
for i in range(2):
    b.append(OdBody(i, "part"+str(i)))
for i in range(1):
    c.append(OdJoint(i+1, "traJ"+str(i)))
cm0 = OdMarker(1, "cm0")
b[0].add_cm_marker(cm0)
gmar1=OdMarker(2, zeros, zeros, "gndJ") 
b[0].add_marker(gmar1)

cm1 = OdMarker(3, [lc30, -ls30, 0], [-30*d2r, 0*d2r,0], "cm1")
b[1].add_cm_marker(cm1)
b1mar1=OdMarker(4, [-ll,0,0], [0, 0*d2r, 0], "b1I") 
b[1].add_marker(b1mar1)

c[0].set_imarker(b1mar1)
c[0].set_jmarker(gmar1)
c[0].txyrz()
scale=1000
spdp=OdJointSPDP(10, "spdptx")
spdp.set_joint(c[0], 0)
spdp.set_stiffness(3.14*scale)
spdp.set_damping(1)
spdp.set_distance(0)
spdp.set_force(0)
fs.append(spdp)

spdpy=OdJointSPDP(11, "spdpty")
spdpy.set_joint(c[0], 1)
spdpy.set_stiffness(3.14*scale)
spdpy.set_damping(1)
spdpy.set_distance(0)
spdpy.set_force(0)
fs.append(spdpy)

spdpz=OdJointSPDP(12, "spdptz")
spdpz.set_joint(c[0], 2)
spdpz.set_stiffness(3.14*scale)
spdpz.set_damping(1)
spdpz.set_distance(0)
spdpz.set_force(0)
fs.append(spdpz)
sys_ = OdSystem("tra")

for i in b: sys_.add_body(i)
for i in c: sys_.add_constraint(i)
for i in fs: sys_.add_joint_spdp(i)    
x1=[]
y1=[]
a1=[]
ts=[]
hht=0
start=time.time()
for i in range(1000):
    t_=i*0.01
    ts.append(t_)
    if hht==1:
        sys_.dynamic_analysis_hht(i*0.01, 1.0e-5, 6, 0.1, 1.0e-6, 0.001, 0)
    else:
        sys_.dynamic_analysis_bdf(i*0.01, 1.0e-3, 6, 0.1, 1.0e-6, 0.001, 0)
    #xyz=cm1.position(0)
    #x1.append(xyz[0])
    #y1.append(xyz[1])
    xya=c[0].disp() 
    x1.append(xya[0])
    y1.append(xya[1])
    a1.append(xya[2])
plt.plot(x1, y1)#, x2, y2, x3, y3
plt.grid()
end=time.time()
print end-start
plt.show()

