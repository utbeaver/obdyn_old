#include "od_marker.h"
#include "od_marker_api.h"
#include "od_body.h"
#include "od_body_api.h"
#include "od_system.h"

od_marker::od_marker(int id, char* name_, int Real) : od_element(id, name_, Real) {
	pbody = 0;
	a_mat.set_type(Mat33::GENERAL);
	r_mat.set_type(Mat33::GENERAL);
	//fill(ifric, ifric+3, 0); fill(iftic, iftic+3, 0);
	for (int i = 0; i < 3; i++) {
		r_pos[i] = 0.0; a_r_pos[i] = 0.0;
		a_pos[i] = 0.0;
	}
}

od_marker::~od_marker()
{
	//int i = 0;
}

od_marker::od_marker(int id, od_body *pBody, char* name_, int Real) : od_element(id, name_, Real) {
	pbody = pBody;  //fill(ifric, ifric+3, 0); fill(iftic, iftic+3, 0.0);
	for (int i = 0; i < 3; i++) {
		r_pos[i] = 0.0; a_r_pos[i] = 0.0;
		a_pos[i] = 0.0;
	}
}
od_marker::od_marker(int ID, od_marker* ref, double pos[3], double ang[3], char *_name,int Real) 
	: od_element(id, _name, Real) {
	pbody = 0;
	a_mat.set_type(Mat33::GENERAL);
	for (int i = 0; i < 3; i++) {
		a_pos[i] = ref->a_pos[i] + pos[i];
		 r_pos[i] = pos[i]=0.0; r_euler[i] = ang[i];// r_pos[i] = 0.0;
		vel[i] = omega_global[i] = omega_dot_global[i] = acc[i] = 0.0;
	}
	a_mat.set_type(Mat33::GENERAL);
	Mat33 tmat;
	tmat.set_type(Mat33::EULER);
	tmat.update(ang);
	a_mat = tmat;
	a_mat = ref->a_mat*a_mat;
	pos = a_mat * pos;
	for (int i = 0; i < 3; i++) {
		a_pos[i] = ref->a_pos[i] + pos[i];
	}
}
od_marker::od_marker(int id, double pos[3], double ang[3], char* name_,
	int Real) : od_element(id, name_, Real) {
	pbody = 0;
	a_mat.set_type(Mat33::GENERAL);
	for (int i = 0; i < 3; i++) {
		a_pos[i] = r_pos[i] = pos[i]; r_euler[i] = ang[i];// r_pos[i] = 0.0;
		vel[i] = omega_global[i] = omega_dot_global[i] = acc[i] = 0.0;
	}
	r_mat.set_type(Mat33::EULER);
	r_mat.update(r_euler);
	a_mat = r_mat;
	cm_marker = 0;  
}

od_marker::od_marker(int id, double mat[16], char* name_,
	int Real) : od_element(id, name_, Real) {
	pbody = 0;
	a_mat.set_type(Mat33::GENERAL);
	for (int i = 0; i < 3; i++) {
		a_pos[i] = r_pos[i] = mat[3 + i * 4]; r_euler[i] = 0.0;// r_pos[i] = 0.0;
		vel[i] = omega_global[i] = omega_dot_global[i] = acc[i] = 0.0;
	}
	r_mat.set_type(Mat33::GENERAL);
	r_mat.update(mat);
	a_mat.update(mat);
	cm_marker = 0; 
}

od_body* od_marker::get_body() {
	return pbody;
}

void od_marker::re_eval_info() {
	ostringstream ost;
	info_str.clear();
	ost << "Marker with Id: " << id;
	info_str += ost.str();
}

void od_marker::set_cm_marker()
{
	cm_marker = 1;
	a_pos[0] = r_pos[0]; a_pos[1] = r_pos[1]; a_pos[2] = r_pos[2];
	a_mat = r_mat;
	fill(r_pos, r_pos + 3, 0.0);
	r_mat.init();
}

void od_marker::set_body(od_body *pB) {
	pbody = pB;
}

void od_marker::make_local_to(od_marker* ref) {
	int i;
	r_mat = a_mat;
	r_mat = ref->a_mat^r_mat;
	for (i = 0; i < 3; i++) {
		this->a_r_pos[i] = this->a_pos[i] - ref->a_pos[i];
		this->r_pos[i] = this->a_r_pos[i];
	}
	ref->a_mat^r_pos; //project relative vector from ground to ref frame 
}

void od_marker::update_cm_marker(od_marker* from, od_systemGeneric *psys)
{
	if (!is_cm_marker()) return;
	if (this == from) { evaluated(); return; }
	if (is_evaluated()) return;
	int i;
	double vec[3], vec1[3];;
	//double* p=0;
	double* from_arpos = 0;
	// Vec3 vecTemp;
	//double* pv = 0;// vecTemp.v;
	od_object::DVA_TYPE _type;
	od_object::Analysis_Type _aType = psys->get_analysis_type();
	if (psys) {
		_type = psys->get_dva_type();
	}
	else {
		_type = od_object::DISP;
	}
	from_arpos = from->a_r_pos;
	if (_type == od_object::DISP || _type == od_object::DISP_VEL_ACC) {
		//update global orientation matrix
		/*temp_mat = */(from->r_mat).transpose(temp_mat);
		mat_mat3(from->a_mat, temp_mat, a_mat);
		//update global position
		mat_doubles(a_mat, from->r_pos, vec);
		EQ3(from_arpos, vec);
		//  p = from->a_pos;        
		SUB3((from->a_pos), vec, a_pos);
		ZERO3(a_r_pos);
	}
	if (_type == od_object::VEL || _type == od_object::DISP_VEL_ACC) {
		//update omega_global
		//  pv = from->omega_global;
		if (_aType != od_object::INIT_VEL) {
			//for(i=0; i<3; i++) omega_global[i] = from->omega_global[i];
			EQ3(omega_global, from->omega_global)
		}
		else {
			for (i = 0; i < 3; i++) {
				//if(!ifric[i]) 
				omega_global[i] = from->omega_global[i];
			}
		}
		//update velocity
		//omega_global * r
		//cross_product_with_doubles(omega_global, from->a_r_pos, vec);
		CROSS_X(omega_global, from_arpos, vec);
		//Vc = Vf - omega_global*r
		//pv = from->vel;
		if (_aType != od_object::INIT_VEL) {
			//for(i=0; i<3; i++)  vel[i] = from->vel[i] - vec[i];
			SUB3(from->vel, vec, vel);
		}
		else {
			for (i = 0; i < 3; i++) {
				//if(!iftic[i])
				vel[i] = from->vel[i] - vec[i];
			}
		}
	}
	if (_type == od_object::ACC || _type == od_object::DISP_VEL_ACC) {
		//update omega_dot
		EQ3(omega_dot_global, from->omega_dot_global);
		//EQ3(omega_dot_global_, from->omega_dot_global_);
		//get from_acc
	
		//add tangent acc
		//cross_product_with_doubles(pv, from->a_r_pos, vec);
		CROSS_X(omega_dot_global, from_arpos, vec);
		//CROSS_X(omega_dot_global_, from_arpos, vec1);
		
		//acc = from->acc - vec
		SUB3(from->acc, vec, acc);
		//add normal acc: omega * R
	
		CROSS_X(omega_global, from_arpos, vec);
		//omega*(omega*R)
		//cross_product_with_doubles(pd, vec.v, vec1);
		CROSS_X(omega_global, vec, vec1);

		U_SUB3(acc, vec1);
		//No Coriolis acceleration
		//set_acceleration(p);
	}
	evaluated();

}



void od_marker::update_marker(od_marker* cm_marker_, od_systemGeneric *psys)
{
	if (cm_marker_ == 0) return;
	if (is_cm_marker()) return;
	double *pd = 0;
	double vec[3], vec1[3];
	od_object::DVA_TYPE _type;
	if (psys) {
		_type = psys->get_dva_type();
	}
	else {
		_type = od_object::DISP;
	}
	//update global orientation matrix
	if (_type == od_object::DISP || _type == od_object::DISP_VEL_ACC) {
 		mat_mat3(cm_marker_->a_mat, r_mat, a_mat);
		//update global vector
		mat_doubles(cm_marker_->a_mat, r_pos, a_r_pos);
		ADD3(cm_marker_->a_pos, a_r_pos, a_pos);
	}
	if (_type == od_object::VEL || _type == od_object::DISP_VEL_ACC) {
		//update omega_global
		//p=cm_marker->omega_global;
		EQ3(omega_global, cm_marker_->omega_global);
		//update velocity
		//omega_global * r
		//cross_product_with_doubles(omega_global, a_r_pos, vec);
		CROSS_X(omega_global, a_r_pos, vec);
		//V= Vc + omega_global*r
		//p=cm_marker->vel;
		ADD3(cm_marker_->vel, vec, vel);
		//vel[0] = cm_marker->vel[0] + vec.v[0];vel[1] = cm_marker->vel[1] + vec.v[1];vel[2] = cm_marker->vel[2] + vec.v[2];
	}
	if (_type == od_object::ACC || _type == od_object::DISP_VEL_ACC) {
		//update omega_dot
		EQ3(omega_dot_global, cm_marker_->omega_dot_global);
		//EQ3(omega_dot_global_, cm_marker->omega_dot_global_);
		//get from acc

		//add tangent acc
		//cross_product_with_doubles(pd, a_r_pos, vec);
		CROSS_X(omega_dot_global, a_r_pos, vec);
		//CROSS_X(omega_dot_global_, a_r_pos, vec1);
		ADD3(cm_marker_->acc, vec, acc);
		pd = cm_marker_->get_omega();
		//add normal acc     omega x (omega x r)
		CROSS_X(pd, a_r_pos, vec);
		CROSS_X(pd, vec, vec1);
		U_ADD3(acc, vec1);
	}
	evaluated();
}

void od_marker::equal(od_marker* pM) {
	if (pM) {
		for (int i = 0; i < 3; i++) {
			r_pos[i] = pM->r_pos[i];
			a_pos[i] = pM->a_pos[i];
			omega_global[i] = pM->omega_global[i];
			r_euler[i] = pM->r_euler[i];
			omega_dot_global[i] = pM->omega_dot_global[i];
			vel[i] = pM->vel[i];
			acc[i] = pM->acc[i];
		}
		a_mat = pM->a_mat;
		r_mat = pM->a_mat;
	}
}


void od_marker::init() {
	for (int i = 0; i < 3; i++) {
		r_pos[i] = 0.0;
		r_euler[i] = 0.0;
		a_pos[i] = 0.0;
		omega_global[i] = 0.0;
		omega_dot_global[i] = 0.0;
		//omega_dot_global_[i] = 0.0;
		vel[i] = 0.0;
		acc[i] = 0.0;
		a_r_pos[i] = 0.0;
		cm_marker = 0;
	}
	a_mat.init();
}

void od_marker::rotation(od_marker *wrt, Mat33& mat) {
	a_mat = mat;
	a_mat = wrt->a_mat*a_mat;
	a_pos[0] = wrt->a_pos[0];
	a_pos[1] = wrt->a_pos[1];
	a_pos[2] = wrt->a_pos[2];
}

void od_marker::translation(od_marker *wrt, Vec3& vec) {
	a_mat = wrt->a_mat;
	vec = a_mat * vec;
	double* pd = wrt->a_pos;
	a_pos[0] = pd[0] + vec.v[0];
	a_pos[1] = pd[1] + vec.v[1];
	a_pos[2] = pd[2] + vec.v[2];
}
double* od_marker::get_orientation_matrix() {
		double* p = pmat;
		p = a_mat.to_double(p);
		return p;
	}
double* od_marker::get_orientation_matrix(double* mat, od_marker* pJ){
		if (pJ) {
			 temp_mat = a_mat;
			temp_mat = (pJ->a_mat) ^ temp_mat;
			mat = temp_mat.to_double(mat);
		}
		else {
			mat = a_mat.to_double(mat);
		}
		return mat;
	}
void od_marker::translation(od_marker *wrt, double* vec) {
	a_mat = wrt->a_mat;
	vec = a_mat * vec;
	double* pd = wrt->a_pos;
	ADD3(pd, vec, a_pos);
}
Vec3& od_marker::get_axis_global(Vec3& zi, int xyz) {
	double* pd = a_mat.v[xyz];
	zi.v[0] = *pd++;
	zi.v[1] = *pd++;
	zi.v[2] = *pd;
	return zi;
}

double* od_marker::get_axis_global(double* zi, int xyz) {
	double* pd = a_mat.v[xyz];
	*zi++ = *pd++;
	*zi++ = *pd++;
	*zi++ = *pd;
	return zi;
}


double* od_marker::get_rotation(double *vec, od_marker *pJ)
{
	double tempx, tempy;
	double *x1, *y1, *z1, *x2, *y2, *z2;
	x1 = get_global_axis(0);
	y1 = get_global_axis(1);
	z1 = get_global_axis(2);
	if (pJ) {
		x2 = pJ->get_global_axis(0);
		y2 = pJ->get_global_axis(1);
		z2 = pJ->get_global_axis(2);
	}
	else {
		x2 = new double[3]; y2 = new double[3]; z2 = new double[3];
		x2[1] = x2[2] = 0.0;
		y2[0] = y2[2] = 0.0;
		z2[0] = z2[1] = 0.0;
		x2[0] = y2[1] = z2[2] = 1.0;
	}
	//calculate RX = ATAN2(y1*z2, y1*y2)
	tempy = DOT_X(z2, y1);//z2[0]*y1[0]+z2[1]*y1[1]+z2[2]*y1[2];
	tempx = DOT_X(y2, y1);//y2[0]*y1[0]+y2[1]*y1[1]+ y2[2]*y1[2];
	vec[0] = atan2(tempy, tempx);
	tempy = DOT_X(z1, x2);//z1[0]*x2[0] + z1[1]*x2[1] + z1[2]*x2[2];
	tempx = DOT_X(z1, z2);//z1[0]*z2[0] + z1[1]*z2[1] + z1[2]*z2[2];
	vec[1] = atan2(tempy, tempx);
	tempy = DOT_X(x1, y2);//x1[0]*y2[0] + x1[1]*y2[1] + x1[2]*y2[2];
	tempx = DOT_X(x1, x2);//x1[0]*x2[0] + x1[1]*x2[1] + x1[2]*x2[2];
	vec[2] = atan2(tempy, tempx);
	if (!pJ) { delete[] x2; delete[] y2; delete[] z2; }
	return vec;
}

OdMarker::OdMarker(int id, char* name_)
{
	pM = new od_marker(id, name_, 0);
}

OdMarker::OdMarker(int id, OdBody *pB, char* name_) {
	od_body *pb = pB->core();
	pM = new od_marker(id, pb, name_);
}

OdMarker::OdMarker(int id, double *pos, double *ang, char* name_) {
	pM = new od_marker(id, pos, ang, name_);
}
OdMarker::OdMarker(int id, V3* P, V3* Q, char* name_) {
	pM = new od_marker(id, P->ptr(), Q->ptr(), name_);
}

OdMarker::OdMarker(int id, double *mat, char* name_) {
	pM = new od_marker(id, mat, name_);
}
OdMarker::OdMarker(int id, V16* P, char* name_) {
	pM = new od_marker(id, P->ptr(), name_);
}

OdMarker::OdMarker(int id, OdMarker* ref, V3* P, V3* ang, char* name_) {
	pM = new od_marker(id, ref->core(), P->ptr(), ang->ptr(), name_);

}
OdMarker::~OdMarker() {/*if (pM) delete pM;*/ }

char* OdMarker::info(char* msg) {
	msg = pM->info(msg);
	return msg;
}



od_marker* OdMarker::core() { return pM; }
/*
void OdMarker::set_position(double *pos) {
	core()->set_position(pos);
}

void OdMarker::set_omega(double* ome, int *idx) {
	core()->set_omega(ome, idx);
}

void OdMarker::set_velocity(double* vel, int *idx) {
	core()->set_velocity(vel, idx);
}

void OdMarker::set_angles(double *ang) {
	core()->set_angles(ang);
}
*/
double* OdMarker::position(int pva) {
	double *pos;
	if (pva == 0) pos = core()->get_position();
	else if (pva == 1) pos = core()->get_velocity();
	else pos = core()->get_acceleration();
	return pos;
}

double* OdMarker::orientation(int pva) {
	double *pos;
	if (pva == 0) pos = core()->get_orientation_matrix();
	else if (pva == 1) pos = core()->get_omega();
	else pos = core()->get_omega_dot();
	return pos;
}



