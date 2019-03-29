#!/usr/bin/env python
from math import  *
from od_utils import *
from od_model import *

def create_force_pair(i, pb1, pb2, r1, r2, stiffness=None):
	###create a pair of forces
	_60d=pi/3.0
	_120d=2.0*_60d
	angle = i*_120d
	position1 = [cos(angle)*r1, 0.0, sin(angle)*r1]
	_angle=angle-_60d
	position2 = [cos(_angle)*r2, 0.0, sin(_angle)*r2]
	_angle=angle+_60d
	position3 = [cos(_angle)*r2, 0.0, sin(_angle)*r2]	
	i_mar1=od_marker("imarker"+str(2*i))
	i_mar2=od_marker("imarker"+str(2*i+1))
	j_mar1=od_marker("jmarker"+str(2*i))
	j_mar2=od_marker("jmarker"+str(2*i+1))
	i_mar1.set("Position",  position1)
	pb1.adding(i_mar1)
	i_mar2.set("Position",  position1)
	pb1.adding(i_mar2)
	j_mar1.set("Position",  position2)
	pb2.adding(j_mar1)
	j_mar2.set("Position",  position3)
	pb2.adding(j_mar2)
	spdp = od_spdpt("spdp"+str(2*i))
	spdp.set_imarker(i_mar1, pb1)
	spdp.set_jmarker(j_mar1, pb2)
	if stiffness is None: stiffness = 10.0
	spdp.set("Stiffness", stiffness)
	spdp.set("Damping", 100.0)
	spdp.set("Distance", 1.0)
	spdp1 = od_spdpt("spdp"+str(2*i+1))
	spdp1.set_imarker(i_mar2, pb1)
	spdp1.set_jmarker(j_mar2, pb2)
	if stiffness is None: stiffness = 10.0
	spdp1.set("Stiffness", stiffness)
	spdp1.set("Damping", 100.0)
	spdp1.set("Distance", 1.0)
	return (spdp, spdp1)

def model_create():
	###this fucntion is to create a model with a plate supported by 6 SPDPs###
			#crerate plate 
			model = Od_model()
			model.add_ground()
			ground = model.ground()
			plate = od_body("platform")
			plate.set("Position", [0.0, 1.0, 0])
			plate.set("Ref_Orientation", [0.0, 90.0, 0.0])
			model.adding(plate)
			#disc
			plate.adding(od_cylinder(1.0, 1.0, .1))
			for i  in range(3):
			 forces = create_force_pair(i, plate, ground, 1.0, 2.0, 1000.0)
			 for force in forces:
			  model.adding(force)
			return model
		
if __name__ == "__main__":
    _model = model_create()
    setModel(_model)			
