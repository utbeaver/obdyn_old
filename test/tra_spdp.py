import numpy as np
from odsystem import *
import matplotlib.pyplot as plt
import time
zeros = [0.0, 0.0, 0.0]
d2r=np.pi/180.0


b=[]
c=[]
fs=[]
for i in range(2):
    b.append(OdBody(i, "part"+str(i)))
for i in range(1):
    c.append(OdJoint(i+1, "traJ"+str(i)))
cm0 = OdMarker(1, "cm0")
b[0].add_cm_marker(cm0)
gmar1=OdMarker(2, zeros, [45*d2r, 90*d2r, 0], "gndJ") 
b[0].add_marker(gmar1)
cm1 = OdMarker(3, zeros, zeros, "cm1")
b[1].add_cm_marker(cm1)
b1mar1=OdMarker(4, zeros, [45*d2r, 90*d2r, 0], "b1I") 
b[1].add_marker(b1mar1)

c[0].translational()
c[0].set_imarker(b1mar1)
c[0].set_jmarker(gmar1)

spdp=OdJointSPDP(10, "spdpt")
spdp.set_joint(c[0])
spdp.set_stiffness(3.14*3.14*4*40)
spdp.set_damping(1)
spdp.set_distance(0)
spdp.set_force(0)
fs.append(spdp)

sys_ = OdSystem("tra")

for i in b: sys_.add_body(i)
for i in c: sys_.add_constraint(i)
for i in fs: sys_.add_joint_spdp(i)    
x1=[]
y1=[]
t=[]
hht=1
start=time.time()
for i in range(1000):
    t_=i*0.01
    if hht==1:
        sys_.dynamic_analysis_hht(t_, 1.0e-5, 10, 0.01, 1.0e-6, 1.0e-3, 0)
    else:    
        sys_.dynamic_analysis_bdf(t_, 1.0e-5, 10, 0.01, 1.0e-6, 1.0e-3, 0)
    xyz=cm1.position(0)
    t.append(t_)
    x1.append(xyz[0])
    y1.append(xyz[1])
end=time.time()
print end-start

plt.plot(t, x1, t, y1)#, t, x3)
#plt.plot(t, y1, t, y2, t, y3)
plt.title("hht "+str(hht))
plt.grid()
plt.show()

