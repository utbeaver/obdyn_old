import numpy as np
from odsystem import *
import matplotlib.pyplot as plt
import time
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
gmar1=OdMarker(2, zeros, [45*d2r*0, 90*d2r*0, 0], "gndJ") 
b[0].add_marker(gmar1)
cm1 = OdMarker(3, [c45, -s45, 0], [-d45, 0, 0], "cm1")
b[1].add_cm_marker(cm1)
b1mar1=OdMarker(4, [0, 0, 0], [-d45, 0, 0], "b1I") 
b[1].add_markerg(b1mar1)
b1mar23=OdMarker(4, [c45*2, -s45*2, 0], [-d45, 0, 0], "b2_3") 
b[1].add_markerg(b1mar23)

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


cm2 = OdMarker(3, [c45*3, -s45*3, 0], [-d45, 0, 0], "cm1")
b[2].add_cm_marker(cm2)
b2mar1=OdMarker(4, [-1, 0, 0], zeros, "b2I") 
b[2].add_marker(b2mar1)

c[1].set_imarker(b2mar1)
c[1].set_jmarker(b1mar23)
#c[1].txyrz()
sys_ = OdSystem("revJT2R1")

for i in b: sys_.add_body(i)
for i in c: sys_.add_constraint(i)
for i in fs: sys_.add_joint_spdp(i)    
x1=[]
y1=[]
t=[]
hht=1
start=time.time()
for i in range(905):
    t_=i*0.01
    if hht==1:
        sys_.dynamic_analysis_hht(t_, 1.0e-3, 10, 0.01, 1.0e-6, 1.0e-3, 0)
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
if hht==1:
    sys_.numdif()

