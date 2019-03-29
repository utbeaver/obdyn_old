#!/usr/bin/env python
from math import  *
from od_utils import *
from od_model import *

def force_creation(i, pb1, pb2, r1, r2, stiffness=None):
	###create forces
	angle = pi/180.0*i*60.0
	position1 = [cos(angle)*r1, 0.0, sin(angle)*r1 ]
	position2 = [cos(angle)*r2, 0.0, sin(angle)*r2 ]
	i_mar = od_marker("imarker_"+str(i))
	i_mar.set("Position",  position1)
	pb1.adding(i_mar)
	j_mar = od_marker("jmarker_"+str(i))
	j_mar.set("Position", position2)
	pb2.adding(j_mar)
	spdp = od_spdpt("spdp"+str(i))
	spdp.set_imarker(i_mar, pb1)
	spdp.set_jmarker(j_mar, pb2)
	if stiffness is None: stiffness = 10.0
	spdp.set("Stiffness", stiffness)
	spdp.set("Damping", 0.0)
	spdp.set("Distance", 2.0)
	return spdp
	
	
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
			for i  in range(6):
				 force = force_creation(i, plate, ground, 1.0, 2.0, 1000.0)
				 model.adding(force)
			return model
				
if __name__ == "__main__":
    _model = model_create()
    setModel(_model)			

			
