import numpy as np
import os, sys
py3=0
if sys.hexversion == 50660592: 
    py3=1
    def execfile(f):
        exec(open(f).read())
import time
home=os.getenv("HOMEPATH")
cadpy=os.path.join(home, "pen1.py")
obdir=os.path.join(home, 'OneDrive', 'Desktop', 'obdyn')
odpy=os.path.join(obdir, 'obdyn', 'src', 'python')
sys.path.append(odpy)
if py3==1:
    libdir=os.path.join(obdir, 'od_solver3', 'odsystem', 'x64', "debug" )
else:    
    libdir=os.path.join(obdir, 'od_solver', 'odsystem', 'x64', "debug" )
sys.path.append(libdir)
testdir=os.path.join(obdir, 'obdyn', 'test')
#os.chdir(testdir)
cwd=os.getcwd()
sys.path.append(cwd)

from odsystem import *
def V(x=0.0, y=0.0, z=0.0):
    return V3(x, y,z)



from math import *
ll=sqrt(2.0)
d30=30.0*np.pi/180.0
d2r=np.pi/180.0
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
b[0].add_cm_marker(cm0)
pos=V3(lsin/2.0, lcos/2.0, 0.0)
ori=V3(d30, 0.0, 0.0)
cm1 = OdMarker(1, pos, ori, "cm1")
b[1].add_cm_marker(cm1)
cm2 = OdMarker(2,V(0.5, h, 0.0),V(0.0, 0.0, 0.0), "cm2")
b[2].add_cm_marker(cm2)
cm3 = OdMarker(3,V(1, h/2.0, 0.0),V(0.0, 0.0, 0.0), "cm3")
b[3].add_cm_marker(cm3)

omar1 = OdMarker(4, V3(), V3(),"mar1")
b[0].add_marker(omar1)
omar2 = OdMarker(5,V(0,-ll/2.0,0),V(-d30,0,0), "mar2")
b[1].add_marker(omar2)
b1base = OdMarker(55, omar2, V(),V(d30,-90*d2r,0), "mar2")
b[1].add_global_marker(b1base)
c[0].set_imarker(omar2)
c[0].set_jmarker(omar1)
omar3 = OdMarker(6,V(0.0, ll/2.0, 0.0),V(-d30,0,0), "mar3")
b[1].add_marker(omar3)
omar4 = OdMarker(7,V(-0.5-lsin, 0.0, 0.0), V3(), "mar4")
b[2].add_marker(omar4)
c[1].set_imarker(omar4)
c[1].set_jmarker(omar3)
omar5 = OdMarker(8,V(0.5, 0.0, 0.0), V3(), "mar5")
b[2].add_marker(omar5)
b2base = OdMarker(8,V(0.5, 0.0, 0.0),V(90*d2r, -90*d2r, 0), "b2base")
b[2].add_marker(b2base)
omar6 = OdMarker(9,V(0.0, h/2, 0.0), V3(), "mar6")
#omar6 = OdMarker(9,V(0.0, .65, 0.0), V3(), "mar6")
b[3].add_marker(omar6)
c[2].set_imarker(omar6)
c[2].set_jmarker(omar5)
omar7 = OdMarker(10,V(0.0, -h/2, 0.0), V3(),"mar7")
b[3].add_marker(omar7)
b3base = OdMarker(10,V(0.0, -h/2, 0.0), V3(0, -90*d2r, 0),"b3base")
b[3].add_marker(b3base)
omar8 = OdMarker(11,V(1.0, 0.0, 0.0), V3(),"mar8")
b[0].add_marker(omar8)
c[3].set_imarker(omar7)
c[3].set_jmarker(omar8)
#c[0].set_expression("np.pi/6+sin(time)")
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
p1=App.ActiveDocument.addObject("Part::Feature", "part1")
p1.Shape=Part.makeBox(0.1, 0.1, ll)
p2=App.ActiveDocument.addObject("Part::Feature", "part2")
p2.Shape=Part.makeBox(0.1, 0.1, lsin+1.0)
p3=App.ActiveDocument.addObject("Part::Feature", "part3")
p3.Shape=Part.makeBox(0.1, 0.1, h)
hht=1
for i in range(5000):
    time_=i*0.01
    if hht==1:
        sys_.dynamic_analysis_bdf(time_, 1.0e-5, 6, 0.1, 1.0e-6, 0.001, 0)
    else:
        sys_.dynamic_analysis_hht(time_, 1.0e-5, 6, 0.1, 1.0e-6, 0.001, 0)
    p=b1base.P()
    m=b1base.Q()
    pl=toPL(p,m)
    p1.Placement=pl
    p=b2base.P()
    m=b2base.Q()
    pl=toPL(p,m)
    p2.Placement=pl
    p=b3base.P()
    m=b3base.Q()
    pl=toPL(p,m)
    p3.Placement=pl
    Gui.updateGui()
