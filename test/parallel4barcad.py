
import os, sys
import time
home=os.getenv("HOMEPATH")
cadpy=os.path.join(home, "pen1.py")
obdir=os.path.join(home, 'OneDrive', 'Desktop', 'obdyn')
odpy=os.path.join(obdir, 'obdyn', 'src', 'sipcode')
sys.path.append(odpy)
sipdir=os.path.join(obdir, 'sip-3.10.2', 'siplib' )
sys.path.append(sipdir)
libdir=os.path.join(obdir, 'obdyn', 'lib' )
sys.path.append(libdir)
testdir=os.path.join(obdir, 'obdyn', 'test')
#os.chdir(testdir)
cwd=os.getcwd()

from odsystem import *
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
b1base = OdMarker(5, [0,-ll/2.0,0], [0,0,0], "mar2")
print b[1].add_marker(b1base)
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
b2base = OdMarker(8, [0.5, 0.0, 0.0], [90, 0, 0], "b2base")
omar6 = OdMarker(9, [0.0, h/2, 0.0], zeros, "mar6")
b[2].add_marker(b2base)
#omar6 = OdMarker(9, [0.0, .65, 0.0], zeros, "mar6")
b[3].add_marker(omar6)
c[2].set_imarker(omar6)
c[2].set_jmarker(omar5)
omar7 = OdMarker(10, [0.0, -h/2, 0.0], zeros,"mar7")
b[3].add_marker(omar7)
b3base = OdMarker(10, [0.0, -h/2, 0.0], zeros,"b3base")
b[3].add_marker(b3base)
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
p1=App.ActiveDocument.addObject("Part::Feature", "part1")
p1.Shape=Part.makeBox(0.1, 0.1, ll)
p2=App.ActiveDocument.addObject("Part::Feature", "part2")
p2.Shape=Part.makeBox(0.1, 0.1, lsin+1.0)
p3=App.ActiveDocument.addObject("Part::Feature", "part3")
p3.Shape=Part.makeBox(0.1, 0.1, 1.0)
for i in range(100):
    time_=i*0.01
    if hht==1:
        sys_.dynamic_analysis_bdf(time_, 1.0e-5, 6, 0.1, 1.0e-6, 0.001, 0)
    else:
        sys_.dynamic_analysis_hht(time_, 1.0e-5, 6, 0.1, 1.0e-6, 0.001, 0)
    p=b1base.position(0)
    m=b1base.orientation(0)
    pl=toPL(p,m)
    p1.Placement=pl
    p=b2base.position(0)
    m=b2base.orientation(0)
    pl=toPL(p,m)
    p2.Placement=pl
    p=b3base.position(0)
    m=b3base.orientation(0)
    pl=toPL(p,m)
    p3.Placement=pl
    Gui.updateGui()
