from odsystem import *
pi=3.1415926
zeros = [0.0, 0.0, 0.0]

print "Four-bar linkage"
print "Topology"
print "bodies:       0->1->2->3->0"
print "connections:    1  2  3  4"

b=[]
c=[]
for i in range(4):
    b.append(OdBody(i))
for i in range(4):
    c.append(OdRevoluteJoint(i))
cm1 = OdMarker(1)
cm1.set_position([0.0, 0.5, 0.0])
cm1.set_angles([0.0, 0.0, 0.0])
b[1].add_cm_marker(cm1)
cm2 = OdMarker(2, [0.5, 1.0, 0.0], [0.0, 0.0, 0.0])
b[2].add_cm_marker(cm2)
cm3 = OdMarker(3, [1, 0.5, 0.0], [0.0, 0.0, 0.0])
b[3].add_cm_marker(cm3)

omar1 = OdMarker(4, zeros, zeros)
b[0].add_marker(omar1)
omar2 = OdMarker(5, [0.0, -0.5, 0.0], zeros)
b[1].add_marker(omar2)
c[0].set_imarker(omar2)
c[0].set_jmarker(omar1)
omar3 = OdMarker(6, [0.0, 0.5, 0.0], zeros)
b[1].add_marker(omar3)
omar4 = OdMarker(7, [-0.5, 0.0, 0.0], zeros)
b[2].add_marker(omar4)
c[1].set_imarker(omar4)
c[1].set_jmarker(omar3)
omar5 = OdMarker(8, [0.5, 0.0, 0.0], zeros)
b[2].add_marker(omar5)
omar6 = OdMarker(9, [0.0, .5, 0.0], zeros)
b[3].add_marker(omar6)
c[2].set_imarker(omar6)
c[2].set_jmarker(omar5)
omar7 = OdMarker(10, [0.0, -.5, 0.0], zeros)
b[3].add_marker(omar7)
omar8 = OdMarker(11, [1.0, 0.0, 0.0], zeros)
b[0].add_marker(omar8)
c[3].set_imarker(omar7)
c[3].set_jmarker(omar8)

c[0].set_expression("0.0*pi/180.0")

sys_ = OdSystem("4bar")
for i in range(4):
    sys_.add_body(b[i])
for i in range(4):
    sys_.add_constraint(c[i])
sys_.displacement_ic()
