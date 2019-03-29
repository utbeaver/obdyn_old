from odsystem import *
from math import *
pi=3.1415926
zeros = [0.0, 0.0, 0.0]

##def print_svg_header():
##    """header for SVG file"""
##    head_str=[]
##    head_str.append('<?xml version="1.0" standalone="no"?>\n')
##    head_str.append('<!DOCTYPE svg SYSTEM "svg-19990812.dtd">\n')
##    head_str.append('<!--\n')
##    head_str.append('<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG August 1999//EN"\n')
##    head_str.append('  "http://www.w3.org/Graphics/SVG/svg-19990812.dtd">\n')
##    head_str.append('-->\n')
##    return head_str

##def is_free_joint(index, free_js):
##    """determine if free joint"""
##    for i in free_js:
##        if (index-1) == i:
##            return 1
##    return 0

##def create_branches(x, free_js, broken_bs, fname):
##    """create branches"""
##    branches = []
##    temp = []
##    for i in x:
##        if i != -2:
##            temp.append(i+1)
##        else:
##            branches.append(temp)
##            temp = []
##    tip_body=[]
##    num = len(branches)
##    for i in range(num-1):
##        branch = branches[i]
##        length = len(branch)
##        branch_n = branches[i+1]
##        length_n = len(branch_n)
##        tip_body.append(branch[length-1])
##        length = min(length, length_n)
##        for j in range(length):
##            if abs(branch[j]) == abs(branch_n[j]):
##                branch_n[j] = -branch_n[j]
####     for i in range(num):
####         for j in branches[i]:
####             print "%2d"%j,
####         print '\n'
##    max_len = 0
##    for i in range(num):
##        length = len(branches[i])
##        max_len =max(length, max_len)

##    max_len = max_len/2
##    branches_info=[]
##    for ii in range(len(branches)):
##        branch_info = []
##        br = branches[ii]
##        length = len(br)/2
##        for i in range(length):
##            if br[2*i] > 0:
##                temp=[(ii+1)*50, (i+1)*50, (br[2*i]), (br[2*i+1])]
##            else:
##                temp_br = branches_info[ii-1]
##                temp = temp_br[i]
##            branch_info.append(temp)
##        branches_info.append(branch_info)
                
####     for br in branches_info:   
####         for b in br:
####             print b,
####         print '\n'
                                   
##    width = (len(branches)+1)*50
##    if width < 200:
##        width = 200
##    height = (max_len+3)*50
##    body=[]
##    header = print_svg_header()
##    for line in header:
##        body.append(line)
##    body.append('<svg width=\"%d\" height=\"%d\">\n'%(width, height))
##    string_ = "<title> System Topology </title>\n"
##    body.append(string_)

##    last_x = width/2
##    last_y = height-10
##    string_ = '<circle cx=\"%d\" cy=\"%d\" r=\"%d\" style=\"fill:red; stroke:navy; stroke-width:2\" />\n'%(last_x, last_y, 8)
##    body.append(string_)
##    string_ = '<text style=\"font-size: 14; fill: black; stroke-width: 1\" x=\"%d\" y=\"%d\">%s</text>\n'%(last_x+10, height-5, 'ground')
##    body.append(string_)
##    for br in branches_info:
##        last_x = width/2
##        last_y = height-10
##        for b in br:
##            #drawline
##            if is_free_joint(b[2], free_js):
##                string_ = '<line x1=\"%d\" x2=\"%d\" y1=\"%d\" y2=\"%d\" style=\"stroke: yellow; stroke-width: 2\" />\n'%(last_x, b[0], last_y, height-b[1])
##            else:
##                string_ = '<line x1=\"%d\" x2=\"%d\" y1=\"%d\" y2=\"%d\" style=\"stroke: blue; stroke-width: 2\" />\n'%(last_x, b[0], last_y, height-b[1])
##            body.append(string_)
##            string_ = '<circle cx=\"%d\" cy=\"%d\" r=\"%d\" style=\"fill:red; stroke:navy; stroke-width:2\" />\n'%(b[0], height-b[1], 5)
##            body.append(string_)
##            string_ = '<text style=\"fontfrom odsystem import *-size: 14; fill: black; stroke-width: 1\" x=\"%d\" y=\"%d\">%s</text>\n'%((last_x+b[0])/2+3, (last_y+height-b[1])/2, 'j'+repr(b[2]))
##            body.append(string_)
##            string_ = '<text style=\"font-size: 14; fill: black; stroke-width: 1\" x=\"%d\" y=\"%d\">%s</text>\n'%(b[0]+5, height-b[1], 'b'+repr(b[3]))
##            body.append(string_)
##            last_x = b[0]
##            last_y = height-b[1]
##    #print len(broken_bs)
##    for i in range(len(broken_bs)/2):
##        b1 = broken_bs[2*i]+1
##        b2 = broken_bs[2*i+1]+1
##        string_ = '<text style=\"font-size: 14; fill: black; stroke-width: 1\" x=\"%d\" y=\"%d\">b%d is connected to b%d</text>\n'%(0, (i+1)*20, b1, b2  )
###        print string_
##        body.append(string_)
##    str="</svg>\n"
##    body.append(str)
##    fid = open(fname, "w")
##    fid.writelines(body)
##    fid.close()
        

##def print_permu(perm):
##    print "permutation Vector"
##    for x in perm:
##	print '%2d'%(x),
##    print '\n'
##def print_mats(n_b, n_j, inc, rel, perm_b, perm_j, topb):
##    print "permutation vector"+"\n"
##    for x in perm_b:
##        print '%2d'%(x+1),
##    print '-->',
##    for x in perm_j:
##        print '%2d'%(x+1),
##    print '\n'
##    for i in range(n_j):
##        for j in range(n_b):
##            x = inc[i*n_b+j]
##            print '%2d'%(x),
##        print '-->',
##        temp_int = n_j
##        for j in range(temp_int):
##            x = rel[i*temp_int+j]
##            print '%2d'%(x),
##        print '\n'
##    for x in topb:
##        if (x+1) == -1:
##            print  "\n" 
##        else:
##            print '%2d'%(x+1),
##    print '\n'
	

##print "Test1: system with two loops"
##print "Topology:"
##print " bodies      0->1->2->3->0    1->4->5->6->2->1"
##print " connections  1  2  3  7       4  5  6  8  1" 
###ground
##ground = OdBody(0)
##mar1 = OdMarker(1)
##mar2 = OdMarker(2)
##marfrom7 = OdMarker(77)
##ground.add_marker(mar1)
##ground.add_marker(mar2)
##ground.add_marker(marfrom7)
###body 1
##b1 = OdBody(1)
##mar3 = OdMarker(3)
##mar4 = OdMarker(4)
##mar1to4 =OdMarker(20)
##b1_cm = OdMarker(5)
##b1.add_marker(mar3)
##b1.add_marker(mar4)
##b1.add_marker(mar1to4)
##b1.add_cm_marker(b1_cm)
###body 2
##b2 = OdBody(2)
##m6 = OdMarker(6)
##m7 = OdMarker(7)
##marfrom6 = OdMarker(26)
##b2_cm = OdMarker(18)
##b2.add_marker(m6)
##b2.add_marker(m7)
##b2.add_marker(marfrom6)
##b2.add_cm_marker(b2_cm)
###body 3
##b3 = OdBody(3)
##m8 = OdMarker(8)
##m81 = OdMarker(81)
##m9 = OdMarker(9)
##b3_cm = OdMarker(10)
##b3.add_marker(m8)
##b3.add_marker(m81)
##b3.add_marker(m9)
##b3.add_cm_marker(b3_cm)
###body 4
##b4 = OdBody(4)
##m11 = OdMarker(11)
##m12 = OdMarker(12)
##b4_cm = OdMarker(99)
##b4.add_marker(m11)
##b4.add_marker(m12)
##b4.add_cm_marker(b4_cm)

###body 5
##b5 = OdBody(5)
##m13 = OdMarker(13)
##m14 = OdMarker(14)
##b5_cm = OdMarker(19)
##b5.add_marker(m13)
##b5.add_marker(m14)
##b5.add_cm_marker(b5_cm)

###body 6
##b6 = OdBody(6)
##m15 = OdMarker(15)
##m16 = OdMarker(16)
##b6_cm = OdMarker(17)
##b6.add_marker(m15)
##b6.add_marker(m16)
##b6.add_cm_marker(b6_cm)
###constraints(ground <-> b1)
##pc1 = OdRevoluteJoint(1)
##pc1.set_imarker(mar3)
##pc1.set_jmarker(mar1)
###constraints(b1 <-> b2)
##pc2 = OdRevoluteJoint(2)
##pc2.set_imarker(m6)
##pc2.set_jmarker(mar4)
###constraints(b2 <-> b3)
##pc3 = OdRevoluteJoint(3)
##pc3.set_imarker(m7)
##pc3.set_jmarker(m8)
###constraints(b3 <-> ground)
##pc7 = OdRevoluteJoint(7)
##pc7.set_imarker(m81)
##pc7.set_jmarker(marfrom7)
###constraints(b1 <-> b4)
##pc4 = OdRevoluteJoint(4)
##pc4.set_imarker(mar1to4)
##pc4.set_jmarker(m11)
###constraints(b4 <-> b5)
##pc5 = OdRevoluteJoint(5)
##pc5.set_imarker(m12)
##pc5.set_jmarker(m13)
###constraints(b5 <-> b6)
##pc6 = OdRevoluteJoint(6)
##pc6.set_imarker(m14)
##pc6.set_jmarker(m15)
###constraints(b6 <-> b1)
##pc8 = OdRevoluteJoint(8)
##pc8.set_imarker(m16)
##pc8.set_jmarker(marfrom6)

##sys_ = OdSystem("test1")
##sys_.add_body(ground)
##sys_.add_body(b1)
##sys_.add_body(b2)
##sys_.add_body(b3)
##sys_.add_body(b4)
##sys_.add_body(b5)
##sys_.add_body(b6)
##sys_.add_constraint(pc1)
##sys_.add_constraint(pc2)
##sys_.add_constraint(pc3)
##sys_.add_constraint(pc4)
##sys_.add_constraint(pc5)
##sys_.add_constraint(pc6)
##sys_.add_constraint(pc7)
##sys_.add_constraint(pc8)
###msg=sys_.info()
###print msg

###inci=sys_.get_incidence()
###print inci
###rele=sys_.get_relevence()
###print rele
##res=sys_.topology_analysis()
##num_b = sys_.get_number_of_bodies()
##num_j = sys_.get_number_of_joints()
##print_mats(num_b, num_j, res[0], res[1], res[2], res[3], res[5])
##print_mats(num_b, num_j, res[0], res[4], res[2], res[3], res[5])
##create_branches(res[5], res[6], res[7], "test1.svg")

##del sys_;


##print "Test2: rootless single chain"
##print "Topology:"
##print " bodies      1->2->3->4"
##print " connections  1  2  3"

##ground = OdBody(0)
##b=[]
##cm=[]
##mari=[]
##marj=[]
##pc=[]
##for i in range(4):
##    b.append(OdBody(i+1))
##    cm.append(OdMarker(i+10))
##    b[i].add_cm_marker(cm[i])
##for i in range(3):
##    marj.append(OdMarker(i))
##    b[i].add_marker(marj[i])
##for i in range(3):
##    mari.append(OdMarker(i+3))
##    b[1+i].add_marker(mari[i])
##for i in range(3):
##    pc.append(OdRevoluteJoint(i+1))
##    pc[i].set_imarker(mari[i])
##    pc[i].set_jmarker(marj[i])
##sys_ = OdSystem("test2")
##sys_.add_body(ground)
##for bb in b:
##    sys_.add_body(bb)
##for cc in pc:
##    sys_.add_constraint(cc)
##msg=sys_.info()
###print msg
##res=sys_.topology_analysis()
##num_b = sys_.get_number_of_bodies()
##num_j = sys_.get_number_of_joints()
##print_mats(num_b, num_j, res[0], res[1], res[2], res[3], res[5])
##print_mats(num_b, num_j, res[0], res[4], res[2], res[3], res[5])
##create_branches(res[5], res[6], res[7], "test2.svg")

##print "Test3: rootless single loop"
##print "Topology:"
##print " bodies      1->2->3->4->1"
##print " connections  1  2  3  4"
##ground = OdBody(0)
##b=[]
##cm=[]
##mari=[]
##marj=[]
##pc=[]
##for i in range(4):
##    b.append(OdBody(i+1))
##    cm.append(OdMarker(i+10))
##    b[i].add_cm_marker(cm[i])
##for i in range(3):
##    marj.append(OdMarker(i))
##    b[i].add_marker(marj[i])
##for i in range(3):
##    mari.append(OdMarker(i+3))
##    b[1+i].add_marker(mari[i])
##marjonb4 = OdMarker(20)
##marionb1 = OdMarker(21)
##b[3].add_marker(marjonb4)
##b[0].add_marker(marionb1)
##for i in range(3):
##    pc.append(OdRevoluteJoint(i+1))
##    pc[i].set_imarker(mari[i])
##    pc[i].set_jmarker(marj[i])
##pc.append(OdRevoluteJoint(4))
##pc[3].set_imarker(marjonb4)
##pc[3].set_jmarker(marionb1)
##sys_ = OdSystem("test2")
##sys_.add_body(ground)
##for bb in b:
##    sys_.add_body(bb)
##for cc in pc:
##    sys_.add_constraint(cc)
##msg=sys_.info()
###print msg
##res=sys_.topology_analysis()
##num_b = sys_.get_number_of_bodies()
##num_j = sys_.get_number_of_joints()
##print_mats(num_b, num_j, res[0], res[1], res[2], res[3], res[5])
##print_mats(num_b, num_j, res[0], res[4], res[2], res[3], res[5])
##del sys_
##create_branches(res[5], res[6], res[7], "test3.svg")
##print "Test4: one rootless single chain and one rooted chain"
##print "Topology:"
##print "bodies        0->1->4->5  7->2->3->6 "
##print "connections:   1  2  3     4  5  6 "

##b=[]
##cm=[]
##mari=[]
##marj=[]
##marc=[]
##pc=[]
##for i in range(8):
##    b.append(OdBody(i))
##    mari.append(OdMarker(i+10))
##    b[i].add_marker(mari[i])
##    marj.append(OdMarker(i+20))
##    b[i].add_marker(marj[i])
##    if i != 0:
##        marc.append(OdMarker(i+1))
##        b[i].add_cm_marker(marc[i-1])

##for i in range(6):
##    pc.append(OdRevoluteJoint(i+1))
###pc[0] between ground and b1
##pc[0].set_imarker(mari[1])    
##pc[0].set_jmarker(marj[0])    
##pc[1].set_imarker(mari[4])    
##pc[1].set_jmarker(marj[1])    
##pc[2].set_imarker(mari[5])    
##pc[2].set_jmarker(marj[4])    
##pc[3].set_imarker(mari[2])    
##pc[3].set_jmarker(marj[7])    
##pc[4].set_imarker(mari[3])    
##pc[4].set_jmarker(marj[2])    
##pc[5].set_imarker(mari[6])    
##pc[5].set_jmarker(marj[3])    
##sys_ = OdSystem("test1")
##for bb in b:
##    sys_.add_body(bb)

##for cc in pc:
##    sys_.add_constraint(cc)
###msg=sys_.info()
###print msg
##res=sys_.topology_analysis()
##num_b = sys_.get_number_of_bodies()
##num_j = sys_.get_number_of_joints()
##print_mats(num_b, num_j, res[0], res[1], res[2], res[3], res[5])
##print_mats(num_b, num_j, res[0], res[4], res[2], res[3], res[5])
##create_branches(res[5], res[6], res[7], "test4.svg")
##del sys_
##print "Test5: one rootless loop and one rooted chain"
##print "Topology:"
##print "bodies        0->1->4->5  7->2->3->6->7 "
##print "connections:   1  2  3     4  5  6  7"
##b=[]
##cm=[]
##mari=[]
##marj=[]
##pc=[]
##marc=[]
##for i in range(8):
##    b.append(OdBody(i))
##    mari.append(OdMarker(i+10))
##    b[i].add_marker(mari[i])
##    marj.append(OdMarker(i+20))
##    b[i].add_marker(marj[i])
##    if i != 0:
##        marc.append(OdMarker(i+1))
##        b[i].add_cm_marker(marc[i-1])

##for i in range(7):
##    pc.append(OdRevoluteJoint(i+1))
###pc[0] between ground and b1
##pc[0].set_imarker(mari[1])    
##pc[0].set_jmarker(marj[0])    
##pc[1].set_imarker(mari[4])    
##pc[1].set_jmarker(marj[1])    
##pc[2].set_imarker(mari[5])    
##pc[2].set_jmarker(marj[4])    
##pc[3].set_imarker(mari[2])    
##pc[3].set_jmarker(marj[7])    
##pc[4].set_imarker(mari[3])    
##pc[4].set_jmarker(marj[2])    
##pc[5].set_imarker(mari[6])    
##pc[5].set_jmarker(marj[3])    
##pc[6].set_imarker(mari[7])    
##pc[6].set_jmarker(marj[6])    
##sys_ = OdSystem("test1")
##for bb in b:
##    sys_.add_body(bb)
##for cc in pc:
##    sys_.add_constraint(cc)
###msg=sys_.info()
###print msg
##res=sys_.topology_analysis()
##num_b = sys_.get_number_of_bodies()
##num_j = sys_.get_number_of_joints()
##print_mats(num_b, num_j, res[0], res[1], res[2], res[3], res[5])
##print_mats(num_b, num_j, res[0], res[4], res[2], res[3], res[5])
##create_branches(res[5], res[6], res[7], "test5.svg")
##del sys_
##print "Test6: two rootless loop and one rooted chain"
##print "Topology:"
##print "bodies        0->1->2  3->8->4->7->3  9->6->10->5->9"
##print "connections:   1  2     7  8  9  10    3  4   5  6"
##b=[]
##cm=[]
##mari=[]
##marj=[]
##marc=[]
##pc=[]
##for i in range(11):
##    b.append(OdBody(i))
##    mari.append(OdMarker(i+20))
##    b[i].add_marker(mari[i])
##    marj.append(OdMarker(i+40))
##    b[i].add_marker(marj[i])
##    if i != 0:
##        marc.append(OdMarker(i+1))
##        b[i].add_cm_marker(marc[i-1])
##for i in range(10):
##    pc.append(OdRevoluteJoint(i+1))

###root chain
##pc[0].set_imarker(mari[1])
##pc[0].set_jmarker(marj[0])
##pc[1].set_imarker(mari[2])
##pc[1].set_jmarker(marj[1])
###first unrooted loop
##pc[6].set_imarker(mari[8])
##pc[6].set_jmarker(marj[3])
##pc[7].set_imarker(mari[4])
##pc[7].set_jmarker(marj[8])
##pc[8].set_imarker(mari[7])
##pc[8].set_jmarker(marj[4])
##pc[9].set_imarker(mari[3])
##pc[9].set_jmarker(marj[7])
###second unrooted loop
##pc[2].set_imarker(mari[6])
##pc[2].set_jmarker(marj[9])
##pc[3].set_imarker(mari[10])
##pc[3].set_jmarker(marj[6])
##pc[4].set_imarker(mari[5])
##pc[4].set_jmarker(marj[10])
##pc[5].set_imarker(mari[9])
##pc[5].set_jmarker(marj[5])
##sys_ = OdSystem("test1")
##for bb in b:
##    sys_.add_body(bb)
##for cc in pc:
##    sys_.add_constraint(cc)
###msg=sys_.info()
###print msg
##res=sys_.topology_analysis()
##num_b = sys_.get_number_of_bodies()
##num_j = sys_.get_number_of_joints()
##num_j = sys_.get_number_of_joints()
##print_mats(num_b, num_j, res[0], res[1], res[2], res[3], res[5])
##print_mats(num_b, num_j, res[0], res[4], res[2], res[3], res[5])
##create_branches(res[5], res[6], res[7], "test6.svg")
##print "Test7: Steward platform"
##print "Topology:"
##print "bodies        0->1->7->13  0->2->8->13 0->3->9->13 0->4->10->13 0->5->11->13 0->6->12->13"
##print "connection     1  7  13       2  8 14     3  9 15     4  10 16     5  11 17   6  12  18"
##b=[]
##cm=[]
##mari=[]
##marj=[]
##marc=[]
##pc=[]
##for i in range(14):
##    b.append(OdBody(i))
##for i in range(18):
##    mari.append(OdMarker(i))
##    marj.append(OdMarker(i+18))
##for i in range(14):
##    if i != 0:
##    	marc.append(OdMarker(100+i))
##	b[i].add_cm_marker(marc[i-1])
##for i in range(6):
##    b[0].add_marker(marj[i])
##for i in range(6):
##    b[i+1].add_marker(mari[i])
##    b[i+1].add_marker(marj[i+6])
##    b[i+7].add_marker(mari[i+6])
##    b[i+7].add_marker(marj[i+12])
##for i in range(6):
##    b[13].add_marker(mari[i+12])
##for i in range(18):
##    pc.append(OdRevoluteJoint(i+1))
##    pc[i].set_jmarker(marj[i])
##    pc[i].set_imarker(mari[i])
##sys_ = OdSystem("SixLegs")
##for bb in b:
##    sys_.add_body(bb)
##for cc in pc:
##    sys_.add_constraint(cc)
###msg=sys_.info()
###print msg
##res=sys_.topology_analysis()
##num_b = sys_.get_number_of_bodies()
##num_j = sys_.get_number_of_joints()
##print_mats(num_b, num_j, res[0], res[1], res[2], res[3], res[5])
##print_mats(num_b, num_j, res[0], res[4], res[2], res[3], res[5])
##create_branches(res[5], res[6], res[7], "sixlegs.svg")
#del sys_

print "Four-bar linkage"
print "Topology"
print "bodies:       0->1->2->3->0"
print "connections:    1  2  3  4"
half = sqrt(2.0)/2
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
cm2 = OdMarker(2, [0.5, 1.5, 0.0], [pi/4, 0.0, 0.0])
b[2].add_cm_marker(cm2)
cm3 = OdMarker(3, [1.0, 1.0, 0.0], [0.0, 0.0, 0.0])
b[3].add_cm_marker(cm3)

omar1 = OdMarker(4, zeros, zeros)
b[0].add_marker(omar1)
omar2 = OdMarker(5, [0.0, -0.5, 0.0], zeros)
b[1].add_marker(omar2)
c[0].set_imarker(omar2)
c[0].set_jmarker(omar1)
omar3 = OdMarker(6, [0.0, 0.5, 0.0], zeros)
b[1].add_marker(omar3)
omar4 = OdMarker(7, [-half, 0.0, 0.0], zeros)
b[2].add_marker(omar4)
c[1].set_imarker(omar4)
c[1].set_jmarker(omar3)
omar5 = OdMarker(8, [half, 0.0, 0.0], zeros)
b[2].add_marker(omar5)
omar6 = OdMarker(9, [0.0, 1.0, 0.0], zeros)
b[3].add_marker(omar6)
c[2].set_imarker(omar6)
c[2].set_jmarker(omar5)
omar7 = OdMarker(10, [0.0, -1.0, 0.0], zeros)
b[3].add_marker(omar7)
omar8 = OdMarker(11, [1.0, 0.0, 0.0], zeros)
b[0].add_marker(omar8)
c[3].set_imarker(omar7)
c[3].set_jmarker(omar8)

#c[0].set_expression("50/180.0*pi")
c[0].set_expression("45*pi/180.0+time*pi")

sys_ = OdSystem("4bar")
for i in range(4):
    sys_.add_body(b[i])
for i in range(4):
    sys_.add_constraint(c[i])
##res=sys_.topology_analysis()
##num_b = sys_.get_number_of_bodies()
##num_j = sys_.get_number_of_joints()
##print_mats(num_b, num_j, res[0], res[1], res[2], res[3], res[5])
##print_mats(num_b, num_j, res[0], res[4], res[2], res[3], res[5])
##create_branches(res[5], res[6], res[7], "4bar.svg")
##del sys_
sys_.displacement_ic()
sys_.velocity_ic()
pos=omar3.position()
print pos, sqrt(pos[0]*pos[0]+pos[1]*pos[1]+pos[2]*pos[2])
pos=omar5.position()
print pos, sqrt((pos[0]-1)*(pos[0]-1)+pos[1]*pos[1]+pos[2]*pos[2])
pos=cm3.position()
print pos, sqrt((pos[0]-1)*(pos[0]-1)+pos[1]*pos[1]+pos[2]*pos[2])
del sys_
