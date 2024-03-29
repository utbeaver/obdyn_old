######################################################
import numpy as np
import matplotlib.pyplot as plt
import time, os, sys
sys.path.append(os.getcwd())
from odsystem import *
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
gmar1=OdMarker(2, V3(), V3(45*d2r, 90*d2r, 0), "gndJ") 
b[0].add_marker(gmar1)
cm1 = OdMarker(3, V3(), V3(), "cm1")
b[1].add_cm_marker(cm1)
b1mar1=OdMarker(4, V3(), V3(45*d2r, 90*d2r, 0), "b1I") 
b[1].add_marker(b1mar1)

c[0].translational()
c[0].set_imarker(b1mar1)
c[0].set_jmarker(gmar1)

spdp=OdJointSPDP(10, "spdpt")
spdp.setJoint(c[0])
spdp.set_stiffness(3.14*3.14*4*40)
spdp.set_damping(1)
spdp.set_distance(0)
spdp.set_force(0)
fs.append(spdp)

sys_ = OdSystem("tra")

for i in b: sys_.add_body(i)
for i in c: sys_.add_constraint(i)
for i in fs: sys_.add_joint_spdp(i)    
datas=[]
hht=1
if len(sys.argv)>1:
    if sys.argv[1]=="-h":
        hht=1
    if sys.argv[1]=="-b":
        hht=0
start=time.time()
for i in range(500):
    t_=i*0.01
    data=[t_]
    if hht==1:
        sys_.dynamic_analysis_hht(t_, 1.0e-5, 10, 0.01, 1.0e-6, 1.0e-3, 0)
    else:    
        sys_.dynamic_analysis_bdf(t_, 1.0e-5, 10, 0.01, 1.0e-6, 1.0e-3, 0)
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
dt= end-start
plt.plot(datas[:,0], datas[:,1])#, datas[:,0], datas[:,2], datas[:,0], datas[:,3])#, datas[:,0], datas[:,4], )#, t, x3)

plt.title("hht "+str(hht))
plt.grid()
if len(sys.argv)>1:
	name_=os.path.splitext(os.path.basename(__file__))[0]
	np.save(name_, datas)
else:
	plt.show()
sys.exit(0)
