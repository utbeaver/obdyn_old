#    Test 1
#
# bodies      0->1->2->3->0    1->4->5->6->2->1
# connections  1  2  3  7       4  5  6  8  1 
#incidence matrix
import od_struct

def print_permu(perm):
    print "permutation Vector"
    for x in perm:
	print '%2d'%(x),
    print '\n'
def print_mats(n_b, n_j, inc, rel):
    for i in range(n_j):
        for j in range(n_b):
            x = inc[i*n_b+j]
            print '%2d'%(x),
        print '-->',
        temp_int = n_j
        for j in range(temp_int):
            x = rel[i*temp_int+j]
            print '%2d'%(x),
        print '\n'

    
print "Test1: system with two loops"
print "Topology:"
print " bodies      0->1->2->3->0    1->4->5->6->2->1"
print " connections  1  2  3  7       4  5  6  8  1" 
incidence  = [ 0,  -1, 0,  0,  0, 1,
               0,  0,  -1,  0,  0, 0,
               1,  0,  0,  0,  0, 0,
               -1, 1,  0,  0,  0, 0,
               0, -1,  1,  0,  0, 0,
               -1, 0,  0,  1,  0, 0,
               0,  0,  0, -1,  1, 0,
               0,  0,  0,  0, -1, 1
               ]
#relevence matrix

relevence = [
	0, 0, 0, 0, 0, 0, 0, 1,
	0, 0, 0, 0, 0, 0, 1, 0,
	1, 0, 0, 0, 0, 0, 0, 0,
	0, 1, 0, 0, 0, 0, 0, 0,
	0, 0, 1, 0, 0, 0, 0, 0,	
	0, 0, 0, 1, 0, 0, 0, 0,
	0, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 0, 0, 1, 0, 0
        ]
num_bodies = 6
num_joints = 8
test1 = od_struct.od_structure()
test1.set_num_joint(num_joints)
test1.set_num_body(num_bodies)
test1.set_incidence(incidence)
test1.set_relevence(relevence)
test1.process()
num_body=test1.get_num_body();
num_joint=test1.get_num_joint();
inci = test1.get_incidence()
rele = test1.get_relevence()
permu = test1.get_permu_vec()
print_permu(permu)
print_mats(num_body, num_joint, inci, rele)
del test1
    
print "Test2: rootless single chain"
print "Topology:"
print " bodies      1->2->3->4"
print " connections  1  2  3"

incidence = [ -1, 1, 0, 0,
              0, -1, 1, 0,
              0, 0, -1, 1]
relevence = [ 1, 0, 0,
              0, 1, 0,
              0, 0, 1]
num_bodies = 4
num_joints = 3
test2 = od_struct.od_structure()
test2.set_num_joint(num_joints)
test2.set_num_body(num_bodies)
test2.set_incidence(incidence)
test2.set_relevence(relevence)
test2.process()
num_body=test2.get_num_body();
num_joint=test2.get_num_joint();
inci = test2.get_incidence()
rele = test2.get_relevence()
permu = test2.get_permu_vec()
print_permu(permu)
print_mats(num_body, num_joint, inci, rele)
del test2

print "Test3: rootless single loop"
print "Topology:"
print " bodies      1->2->3->4->1"
print " connections  1  2  3  4"

incidence = [ -1, 1, 0, 0,
              0, -1, 1, 0,
              0, 0, -1, 1,
              1, 0,  0, -1]
relevence = [ 1, 0, 0, 0,
              0, 1, 0, 0,
              0, 0, 1, 0,
              0, 0, 0, 1]
num_bodies = 4
num_joints = 4
test3 = od_struct.od_structure()
test3.set_num_joint(num_joints)
test3.set_num_body(num_bodies)
test3.set_incidence(incidence)
test3.set_relevence(relevence)
test3.process()
num_body=test3.get_num_body();
num_joint=test3.get_num_joint();
inci = test3.get_incidence()
rele = test3.get_relevence()
permu = test3.get_permu_vec()
print_permu(permu)
print_mats(num_body, num_joint, inci, rele)
del test3

print "Test4: one rootless single chain and one rooted chain"
print "Topology:"
print "bodies        0->1->4->5  7->2->3->6 "
print "connections:   1  2  3     4  5  6 "

incidence = [ 1, 0, 0, 0, 0, 0, 0,
             -1, 0, 0, 1, 0, 0, 0,
              0, 0, 0,-1, 1, 0, 0,
              0, 1, 0, 0, 0, 0,-1,
              0,-1, 1, 0, 0, 0, 0,
              0, 0,-1, 0, 0, 1, 0]

relevence = [ 1, 0, 0, 0, 0, 0,
              0, 1, 0, 0, 0, 0,
              0, 0, 1, 0, 0, 0,
              0, 0, 0, 1, 0, 0,
              0, 0, 0, 0, 1, 0,
              0, 0, 0, 0, 0, 1]

num_bodies = 7
num_joints = 6
test4 = od_struct.od_structure()
test4.set_num_joint(num_joints)
test4.set_num_body(num_bodies)
test4.set_incidence(incidence)
test4.set_relevence(relevence)
test4.process()
num_body=test4.get_num_body();
num_joint=test4.get_num_joint();
inci = test4.get_incidence()
rele = test4.get_relevence()
permu = test4.get_permu_vec()
print_permu(permu)
print_mats(num_body, num_joint, inci, rele)
del test4
print "Test5: one rootless loop and one rooted chain"
print "Topology:"
print "bodies        0->1->4->5  7->2->3->6->7 "
print "connections:   1  2  3     4  5  6  7"

incidence = [ 1, 0, 0, 0, 0, 0, 0,
             -1, 0, 0, 1, 0, 0, 0,
              0, 0, 0,-1, 1, 0, 0,
              0, 1, 0, 0, 0, 0,-1,
              0,-1, 1, 0, 0, 0, 0,
              0, 0,-1, 0, 0, 1, 0,
              0, 0, 0, 0, 0,-1, 1]

relevence = [ 1, 0, 0, 0, 0, 0, 0,
              0, 1, 0, 0, 0, 0, 0,
              0, 0, 1, 0, 0, 0, 0,
              0, 0, 0, 1, 0, 0, 0,
              0, 0, 0, 0, 1, 0, 0,
              0, 0, 0, 0, 0, 1, 0,
              0, 0, 0, 0, 0, 0, 1]
num_bodies = 7
num_joints = 7
test5 = od_struct.od_structure()
test5.set_num_joint(num_joints)
test5.set_num_body(num_bodies)
test5.set_incidence(incidence)
test5.set_relevence(relevence)
test5.process()
num_body=test5.get_num_body();
num_joint=test5.get_num_joint();
inci = test5.get_incidence()
rele = test5.get_relevence()
permu = test5.get_permu_vec()
print_permu(permu)
print_mats(num_body, num_joint, inci, rele)
del test5
print "Test6: two rootless loop and one rooted chain"
print "Topology:"
print "bodies        0->1->2  3->8->4->7->3  9->6->10->5->9"
print "connections:   1  2     7  8  9  10    3  4   5  6"

incidence = [
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   -1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 1, 0, 0,-1, 0,
    0, 0, 0, 0, 0,-1, 0, 0, 0, 1,
    0, 0, 0, 0, 1, 0, 0, 0, 0,-1,
    0, 0, 0, 0,-1, 0, 0, 0, 1, 0,
    0, 0,-1, 0, 0, 0, 0, 1, 0, 0,
    0, 0, 0, 1, 0, 0, 0,-1, 0, 0,
    0, 0, 0,-1, 0, 0, 1, 0, 0, 0,
    0, 0, 1, 0, 0, 0,-1, 0, 0, 0]

relevence = [ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
              0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
              0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
              0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
              0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
              0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
              0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
              0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
              0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
              0, 0, 0, 0, 0, 0, 0, 0, 0, 1]
num_bodies = 10
num_joints = 10
test5 = od_struct.od_structure()
test5.set_num_joint(num_joints)
test5.set_num_body(num_bodies)
test5.set_incidence(incidence)
test5.set_relevence(relevence)
test5.process()
num_body=test5.get_num_body();
num_joint=test5.get_num_joint();
inci = test5.get_incidence()
rele = test5.get_relevence()
permu = test5.get_permu_vec()
print_permu(permu)
print_mats(num_body, num_joint, inci, rele)
del test5
