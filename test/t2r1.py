######################################
import numpy as np
import matplotlib.pyplot as plt
import time, os, sys
sys.path.append(os.getcwd())
from odsystem import *
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
gmar1=OdMarker(2, V3(), V3(), "gndJ") 
b[0].add_marker(gmar1)

cm1 = OdMarker(3, V3(lc30, -ls30, 0), V3(30*d2r, 0*d2r,0), "cm1")
b[1].add_cm_marker(cm1)
b1mar1=OdMarker(4, V3(ll,0,0), V3(0, 0*d2r, 0), "b1I") 
b[1].add_marker(b1mar1)

c[0].set_imarker(b1mar1)
c[0].set_jmarker(gmar1)
c[0].txyrz()
scale=1000
spdp=OdJointSPDP(10, "spdptx")
spdp.setJoint(c[0], 0)
spdp.set_stiffness(3.14*scale)
spdp.set_damping(1)
spdp.set_distance(0)
spdp.set_force(0)
fs.append(spdp)

spdpy=OdJointSPDP(11, "spdpty")
spdpy.setJoint(c[0], 1)
spdpy.set_stiffness(3.14*scale)
spdpy.set_damping(1)
spdpy.set_distance(0)
spdpy.set_force(0)
fs.append(spdpy)

spdpz=OdJointSPDP(12, "spdptz")
spdpz.setJoint(c[0], 2)
spdpz.set_stiffness(3.14*scale)
spdpz.set_damping(1)
spdpz.set_distance(0)
spdpz.set_force(0)
fs.append(spdpz)
sys_ = OdSystem("tra")

for i in b: sys_.add_body(i)
for i in c: sys_.add_constraint(i)
for i in fs: sys_.add_joint_spdp(i)    
hht=1
start=time.time()
datas=[]
for i in range(1000):
    t_=i*0.01
    data=[t_]
    if hht==1:
        sys_.dynamic_analysis_hht(i*0.01, 1.0e-5, 6, 0.1, 1.0e-6, 0.001, 0)
    else:
        sys_.dynamic_analysis_bdf(i*0.01, 1.0e-3, 6, 0.1, 1.0e-6, 0.001, 0)
    for c_ in c:
        P=c_.disp()
        for i in range(c_.dofs()):
            if c_.rotation(i)==1:
                data.append(P.get(i)*180.0/np.pi)
            else:    
                data.append(P.get(i))
    datas.append(data)        
end=time.time()
datas=np.array(datas)    
plt.plot(datas[:,0], datas[:,1], datas[:,0], datas[:,2], datas[:,0], datas[:,3])#, datas[:,0], datas[:,4], )#, t, x3)
dt= end-start
plt.title("hht "+str(hht))
plt.grid()
end=time.time()
if len(sys.argv)>1:
	name_=os.path.splitext(os.path.basename(__file__))[0]
	np.save(name_, datas)
else:
	plt.show()
sys.exit(0)

