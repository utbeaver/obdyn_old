#include <iostream>
#include "od_constraint.h"
#include "od_constraint_api.h"
#include "od_body.h"
#include "od_marker_api.h"
#include "od_system.h"
class OdSystem;

joint_primitive::~joint_primitive() {
	if (pExpr) delete pExpr;
	//if (pMotion) delete pMotion;
}

void joint_primitive::set_expr(const char *exp) {
	pExpr = new Expression(exp);
	_dofs = 0;
	//pExpr = exp;
	//pMotion = new od_motion(this, pExpr);
}

void jprim_rot::initialize()
{
	Vec3 Zi, Zj;
	Vec3 Xi, Xj, Yj;
	Zi = To->get_axis_global(Zi);
	Zj = From->get_axis_global(Zj);
	if (initialized == 0) {
		if (pExpr) {
			double tempD = 1.0;
			pExpr->set_system(pSys);
			if (reversed) tempD = -1.0;
			disp = pExpr->eval()*tempD;
			vel = pExpr->eval_wrt_time()*tempD;
			acc = pExpr->eval_wrt_time_2()*tempD;
		}
		else {
//			Vec3 Zi, Zj;
//			Vec3 Xi, Xj, Yj;
//			Zi = To->get_axis_global(Zi);
//			Zj = From->get_axis_global(Zj);
			double temp_x;
			double temp_y;
			if (Zi != Zj) {
				Xi = To->get_axis_global(Xi, 0);
				Xj = From->get_axis_global(Xj, 0);
				Yj = From->get_axis_global(Yj, 1);
				temp_x = Xj * Xi;
				temp_y = Yj * Xi;
				disp = atan2(temp_y, temp_x);
				vel = 0.0;
				acc = 0.0;
			}
			else {
				disp = 0.0;
				vel = 0.0;
				acc = 0.0;
			}
		}
	}

	initialized = 1;
}

int jprim_rot::update(od_systemGeneric *psys) {
	int i;
	od_object::DVA_TYPE _type;
	double *pd;
	double* p = 0;
	if (psys) _type = psys->get_dva_type();
	else _type = od_object::DISP_VEL_ACC;

	if (_type == od_object::DISP || _type == od_object::DISP_VEL_ACC) {
		rot_mat.update(&disp);
		//update I marker
		To->rotation(From, rot_mat);
		//EQ3(To->a_pos, From->a_pos);
	}
	//update local omega  
	if (_type == od_object::VEL || _type == od_object::DISP_VEL_ACC) {
		//update global omega
		p = From->get_omega();
		pd = From->get_global_axis(xyz_);
		for (i = 0; i < 3; i++) To->omega_global[i] = vel * pd[i] + p[i];
		EQ3(To->vel, From->vel);	
		CROSS_X(From->omega_global, pd, w_cross_zi);
	}
	if (_type == od_object::ACC || _type == od_object::DISP_VEL_ACC) {
		//update global omega dot
		p = From->omega_dot_global;
		//omega_dot_1 = omega_dot_0 + z0*acc + omega_0 x z0 * vel
		pd = From->get_global_axis(xyz_);
		for (i = 0; i < 3; i++) {
			To->omega_dot_global[i] = pd[i] * acc + w_cross_zi[i] * vel + p[i];
			//To->omega_dot_global_[i] = pd[i] * acc + From->omega_dot_global_[i];
		}
		EQ3(To->acc, From->acc);
	}
	return 1;
}

void jprim_tra::initialize() {
	if (initialized == 0) {
		if (pExpr) {
			double tempD = 1.0;
			if (reversed) tempD = -1.0;
			pExpr->set_system(pSys);
			disp = pExpr->eval()*tempD;
			vel = pExpr->eval_wrt_time()*tempD;
			acc = pExpr->eval_wrt_time_2()*tempD;
		}
		else {
			double *pd = fltTemp;
			pd = To->get_position(pd, From, From);
			disp = fltTemp[xyz_];
			vel = 0.0;
			acc = 0.0;
		}
	}
	initialized = 1;
}

int jprim_tra::update(od_systemGeneric *psys) {
	double *pd, *pd1;
	double vec[3], vec1[3];
	od_object::DVA_TYPE _type;
	if (psys) {
		_type = psys->get_dva_type();
	}
	else {
		_type = od_object::DISP_VEL_ACC;
	}
	if (_type == od_object::DISP || _type == od_object::DISP_VEL_ACC) {
		vec[0] = 0.0; vec[1] = 0.0; vec[2] = 0.0; vec[xyz_] = disp;
		To->translation(From, vec);
	}
	if (_type == od_object::VEL || _type == od_object::DISP_VEL_ACC) {
		//update global omega
		EQ3(To->omega_global, From->omega_global);
		//To->set_omega(From->get_omega());
		//update velocity: translation velocity
		//p = From->get_velocity();
		EQ3(To->vel, From->vel);
		double xyz[3];
		xyz[0] = 0.0; xyz[1] = 0.0; xyz[2] = 0.0; xyz[xyz_] = vel;
		From->vec_wrt_ground(xyz);
		U_ADD3(To->vel, xyz);
		//p[0] += xyz[0];p[1] += xyz[1];p[2] += xyz[2];
		//update velocity: w*disp
		pd = From->get_omega();
		To->get_position(vec1, From);
		CROSS_X(pd, vec1, vec);
		//p[0] += vec.v[0];p[1] += vec.v[1];p[2] += vec.v[2];
		U_ADD3(To->vel, vec);
		//To->set_velocity(p);
		//update w_cross_zi
		pd1 = From->get_global_axis(xyz_);
		CROSS_X(pd, pd1, w_cross_zi);
	}
	if (_type == od_object::ACC || _type == od_object::DISP_VEL_ACC) {
		//update omega_dot 
		To->set_omega_dot(From->get_omega_dot());
		//EQ3(To->omega_dot_global_, From->omega_dot_global_);
		//update accleration
		//get acc0
		double _acc[3];
		From->get_acceleration(_acc);
		//add relative translational acc
		pd = From->get_global_axis(xyz_);
		vec[0] = pd[0]*acc; vec[1] = pd[1] * acc; vec[2] = pd[2] * acc; 
		//From->vec_wrt_ground(vec);

		//p[0] += vec.v[0];p[1] += vec.v[1];p[2] += vec.v[2];
		U_ADD3(_acc, vec);
		//add tangent acc: omega_dot*disp
		double xyz[3];
		xyz[0] = pd[0] * disp; xyz[1] = pd[1] * disp; xyz[2] = pd[2] * disp; //xyz[xyz_] = disp;
		//From->vec_wrt_ground(xyz);
		pd1 = From->get_omega_dot();
		CROSS_X(pd1, xyz, vec);

		//p[0] += vec.v[0];p[1] += vec.v[1];p[2] += vec.v[2];
		U_ADD3(_acc, vec);
		//add normal  acc
		//vec = omega x R
		To->get_position(vec1, From);
		pd1 = From->get_omega();
		CROSS_X(pd1, vec1, vec);
		//vec1 = omega * vec
		CROSS_X(pd1, vec, vec1);
		//p[0] += vec1.v[0];p[1] += vec1.v[1];p[2] += vec1.v[2];
		U_ADD3(_acc, vec1);
		//add Coriolis acc: 2*omega cross vel
		xyz[0] = pd[0] * vel; xyz[1] = pd[1] * vel; xyz[2] = pd[2] * vel;
		//From->vec_wrt_ground(xyz);

		CROSS_X(pd1, xyz, vec1);

		// for(i=0; i<3; i++) p[i] += (vec1[i]+vec1[i]);
		U_ADD3(_acc, vec1);
		U_ADD3(_acc, vec1);
		To->set_acceleration(_acc);
	}
	return 1;
}

od_constraint::od_constraint(int Id, char* name_, int Real)
	:od_connector(Id, name_, 0, 0, Real)
{
	num_dofs = 0;
//	pExpr = 0;
	initialized = 0;
	reversesign = 1.0;
	par_col = 0;
	//  parVec3col=0;
	//  parVec3colLen=0;
}



void od_constraint::re_eval_info() {
	ostringstream ost;
	od_body* pB;
	info_str.clear();
	ost << "Joint with Id: " << id << endl;
	info_str += ost.str();
	pB = i_marker->get_body();
	if (pB) {
		info_str += "  I marker connected to " + pB->info();
	}
	else {
		info_str += "  I marker connected to unkown body";
	}
	pB = j_marker->get_body();
	if (pB) {
		info_str += "  J marker connected to " + pB->info();
	}
	else {
		info_str += "  J marker connected to unkown body";
	}
}


int od_constraint::tag_attached_body() {
	od_body *pB;
	od_marker *pM;
	pM = get_imarker();
	if (pM) {
		pB = pM->get_body();
		if (pB) {
			pB->tag();
		}
	}
	pM = get_jmarker();
	if (pM) {
		pB = get_jmarker()->get_body();
		if (pB) {
			pB->tag();
		}
	}
	return 1;
}

int od_connector::i_body_index() {
	od_marker *pM = get_imarker();
	od_body *pB = pM->get_body();
	int index_ = pB->get_index();
	return index_;
}

int od_connector::j_body_index() {
	od_marker *pM = get_jmarker();
	od_body *pB = pM->get_body();
	int index_ = pB->get_index();
	return index_;
}

int od_constraint::untag_attached_body() {
	od_body *pB;
	od_marker *pM;
	pM = get_imarker();
	if (pM) {
		pB = pM->get_body();
		if (pB) {
			pB->untag();
		}
	}
	pM = get_jmarker();
	if (pM) {
		pB = pM->get_body();
		if (pB) {
			pB->untag();
		}
	}
	return 1;
}
int od_joint::updateReldistance(od_systemGeneric *pSys) {
	int i;
	double vi[3];
	double vj[3];
	double vm[3];
	double *pv;
	od_marker *pm;
	od_body *pb;
	od_object::DVA_TYPE _type;
	if (pSys) {
		_type = pSys->get_dva_type();
	}
	else {
		_type = od_object::DISP;
	}

	for (i = 0; i < 3; i++) { vm[i] = 0.0; vi[i] = 0.0; vj[i] = 0.0; }
	if (_type == od_object::DISP || _type == od_object::DISP_VEL_ACC) {
		pv = j_marker->absolute_rel_pos();
		EQ3(vj, pv);
		pv = i_marker->absolute_rel_pos();
		EQ3(vi, pv);
		if (num_tra_joints) {
			double vt[3], vr[3];
			pm = tra_joints[num_tra_joints - 1]->to();
			pv = j_marker->get_position(); EQ3(vt, pv);
			pv = pm->get_position(); EQ3(vr, pv);
			for (i = 0; i < 3; i++) vm[i] = vr[i] - vt[i];
		}
		for (i = 0; i < 3; i++) {
			vm[i] = -vi[i] + vj[i] + vm[i];
			vi[i] = -vi[i];
		}
		//distance_between_cm_markers = vm;
		EQ3(distance_between_cm_markers, vm);
		EQ3(distance_to_i_cm_marker, vi);
	}
	if (_type == od_object::VEL || _type == od_object::DISP_VEL_ACC) {
		pb = j_marker->get_body();
		pm = pb->cm_marker();
		if (pm == NULL) { ZERO3(vj); }
		else {
		pv = pm->get_velocity(); EQ3(vj, pv);}
		pb = i_marker->get_body();
		pm = pb->cm_marker();
		pv = pm->get_velocity(); EQ3(vi, pv);
		if (num_tra_joints) {
			pm = tra_joints[num_tra_joints - 1]->to();
		}
		else {
			pm = i_marker;
		}
		pv = pm->get_velocity(); EQ3(vm, pv);
		for (i = 0; i < 3; i++) {
			vj[i] = vi[i] - vj[i];
			vi[i] = vi[i] - vm[i];
		}
		EQ3(velocity_between_cm_markers, vj);
		EQ3(velocity_to_i_cm_marker, vi);
	}
	return 1;
}

int od_joint::update(od_systemGeneric *psys) {
	int i;
	joint_primitive* pJ = 0;
	double *pzi = zi, *pd;
	double *pcz = cross_zi;
	for (i = 0; i < num_tra_joints; i++) {
		tra_joints[i]->update(psys);
		//pJ = (joint_primitive*)tra_joints[i];
		pd = tra_joints[i]->get_zi(); EQ3(pzi, pd); pzi += 3;
		pd = tra_joints[i]->get_cross_zi(); EQ3(pcz, pd); pcz += 3;
	}
	for (i = 0; i < num_rot_joints; i++) {
		rot_joints[i]->update(psys);
		//pJ = (joint_primitive*)rot_joints[i];
		pd = rot_joints[i]->get_zi(); EQ3(pzi, pd); pzi += 3;
		pd = rot_joints[i]->get_cross_zi(); EQ3(pcz, pd); pcz += 3;
	}
	return 1;
}

void od_joint::updateQ() {
	int i;

	joint_primitive* pJ = 0;
	_zixaRot.init(); _zixvRot.init(); _dot_zixaRot.init(); _dot_zixvRot.init();
	_zixaTra.init(); _zixvTra.init(); _dot_zixaTra.init(); _dot_zixvTra.init();
	for (i = 0; i < num_tra_joints; i++) {
		pJ = (joint_primitive*)tra_joints[i];
		pJ->updateQ();
		_zixvTra += pJ->zixv();
		_zixaTra += pJ->zixa();
		_dot_zixvTra += pJ->dot_zixv();
		_dot_zixaTra += pJ->dot_zixa();
	}
	for (i = 0; i < num_rot_joints; i++) {
		pJ = (joint_primitive*)rot_joints[i];
		pJ->updateQ();
		_zixvRot += pJ->zixv();
		_zixaRot += pJ->zixa();
		_dot_zixvRot += pJ->dot_zixv();
		_dot_zixaRot += pJ->dot_zixa();
	}
}

double* od_joint::getQ(int type, od_object::DOF_TYPE rt, int v1a2, int start_j, double* vec) {
	double *pd;
	if (type == 0) { // Q
		for (int i = start_j; i < num_rot_joints; i++) {
			pd = (v1a2 == 1) ? (rot_joints[i]->zixv()) : (rot_joints[i]->zixa());
			U_ADD3(vec, pd);
		}
	}
	else { //Q_dot
		for (int i = start_j; i < num_rot_joints; i++) {
			pd = (v1a2 == 1) ? (rot_joints[i]->dot_zixv()) : (rot_joints[i]->dot_zixa());
			U_ADD3(vec, pd);
		}
	}
	return vec;
}

double* od_joint::getQ(int type, od_object::DOF_TYPE rt, int v1a2, int idx) {
	double *pd = 0;
	if (0 == type) {
		if (od_object::ROT_DOF == rt) {
			if (idx == -1) {
				pd = (v1a2 == 1) ? _zixvRot.v : _zixaRot.v;
			}
			else {
				pd = (v1a2 == 1) ? (rot_joints[idx]->zixv()) : (rot_joints[idx]->zixa());
			}
		}
		else {
			if (idx == -1) {
				pd = (v1a2 == 1) ? _zixvTra.v : _zixaTra.v;
			}
			else {
				pd = (v1a2 == 1) ? (tra_joints[idx]->zixv()) : (tra_joints[idx]->zixa());
			}
		}
	}
	else {
		if (od_object::ROT_DOF == rt) {
			if (idx == -1) {
				pd = (v1a2 == 1) ? _dot_zixvRot.v : _dot_zixaRot.v;
			}
			else {
				pd = (v1a2 == 1) ? (rot_joints[idx]->dot_zixv()) : (rot_joints[idx]->dot_zixa());
			}
		}
		else {
			if (idx == -1) {
				pd = (v1a2 == 1) ? _dot_zixvTra.v : _dot_zixaTra.v;
			}
			else {
				pd = (v1a2 == 1) ? (tra_joints[idx]->dot_zixv()) : (tra_joints[idx]->dot_zixa());
			}
		}
	}
	return pd;
}

int od_joint::initRotXYZ() {
	if (jtype != od_object::FREE && jtype != od_object::SPHERICAL) return 0;
	int i, flag = 0;
	double dispDelta[9], vDelta[3], aDelta[3], tempD[6];
	double* newAxes = 0;
	Mat33 fromOmega;
	double ang[3], c[3], s[3];
	Vec3 vecTemp;
	double *p = dispDelta;
	od_marker *pM = get_jmarker();
	int numTra = num_tra();
	p = get_imarker()->get_orientation_matrix(p, pM);
	if (fabs(dispDelta[8]) > .984) { //to bryant if theta < 10
		euler_or_not = 0;
		p = vDelta; p = get_imarker()->get_omega(p, pM);
		p = aDelta; p = get_imarker()->get_omega_dot(p, pM);
		ang[0] = atan2(-dispDelta[5], dispDelta[8]);
		ang[2] = atan2(-dispDelta[1], dispDelta[0]);
		s[0] = sin(ang[0]);
		ang[1] = asin(dispDelta[2]);
		rot_joints[0]->direction(Mat33::XAXIS);
		rot_joints[1]->direction(Mat33::YAXIS);
		rot_joints[2]->direction(Mat33::ZAXIS);
		flag = 1;
		for (i = 1; i < 3; i++) {
			c[i] = cos(ang[i]); s[i] = sin(ang[i]);
		}
		double D[9];
		D[0] = c[2];         D[1] = -s[2];       D[2] = 0.0;
		D[3] = s[2] * c[1];  D[4] = c[2] * c[1]; D[5] = 0.0;
		D[6] = -c[2] * s[1]; D[7] = s[2] * s[1]; D[8] = c[1];
		for (int j = 0; j < 9; j++) D[j] /= c[1];
		fromOmega = D;
	}
	else { //to euler
		euler_or_not = 1;
		p = vDelta; p = get_imarker()->get_omega(p, pM);
		p = aDelta; p = get_imarker()->get_omega_dot(p, pM);
		ang[0] = atan2(dispDelta[2], -dispDelta[5]);
		ang[2] = atan2(dispDelta[6], dispDelta[7]);
		s[2] = sin(ang[2]);
		c[2] = cos(ang[2]);
	
		ang[1] = acos(dispDelta[8]);
		rot_joints[0]->direction(Mat33::ZAXIS);
		rot_joints[1]->direction(Mat33::XAXIS);
		rot_joints[2]->direction(Mat33::ZAXIS);
		flag = 1;
		for (i = 0; i < 2; i++) {
			c[i] = cos(ang[i]); s[i] = sin(ang[i]);
		}
		double D[9];
		D[0] = s[2];          D[1] = c[2];         D[2] = 0.0;
		D[3] = c[2] * s[1];   D[4] = -s[2] * s[1]; D[5] = 0.0;
		D[6] = -s[2] * c[1];  D[7] = -c[2] * c[1]; D[8] = s[1];
		for (int j = 0; j < 9; j++) D[j] /= s[1];
		fromOmega = D;
	}
	//update disp
	get_states(tempD);
	for (i = numTra; i < 3 + numTra; i++) tempD[i] = ang[i - numTra];
	p = set_states(tempD); update();
	newAxes = get_zi_global();
	//solver for omega:
	vecTemp = vDelta;
	vecTemp = get_imarker()->vec_wrt_this(vecTemp);
	vecTemp = fromOmega * vecTemp;
	get_states(tempD, od_object::VEL);
	for (i = numTra; i < 3 + numTra; i++) tempD[i] = vecTemp.v[i - numTra];
	set_states(tempD, od_object::VEL); update();
	//solve for acceleration
	//w_cross_z1 = 0
	//w_cross_z2:
	Vec3 w_cross_z2, w_cross_z3;
	vecTemp = rot_joints[1]->from()->get_omega(vecTemp, pM);
	cross_product_with_double(vecTemp, newAxes + 3 + numTra * 3, w_cross_z2);
	w_cross_z2.multiplied_by(rot_joints[1]->v());
	vecTemp = rot_joints[2]->from()->get_omega(vecTemp, pM);
	cross_product_with_double(vecTemp, newAxes + 6 + numTra * 3, w_cross_z3);
	w_cross_z3.multiplied_by(rot_joints[2]->v());
	vecTemp = aDelta;
	vecTemp -= w_cross_z2;
	vecTemp -= w_cross_z3;
	vecTemp = get_imarker()->vec_wrt_this(vecTemp);
	vecTemp = fromOmega * vecTemp;
	get_states(tempD, od_object::ACC);
	for (i = numTra; i < 3 + numTra; i++) tempD[i] = vecTemp.v[i - numTra];
	set_states(tempD, od_object::ACC); update();
	return flag;
}

int od_joint::checkEulerBryant() {
	if (jtype != od_object::FREE && jtype != od_object::SPHERICAL) return 0;
	int i, flag = 0;
	double dispDelta[9], vDelta[3], aDelta[3], tempD[6];
	double* newAxes = 0;
	Mat33 fromOmega;
	double ang[3], c[3], s[3];
	Vec3 vecTemp;
	double *p = dispDelta;
	od_marker *pM = get_jmarker();
	int numTra = num_tra();
	p = get_imarker()->get_orientation_matrix(p, pM);
	if (rot_joints[0]->xyz() == 2) { //euler
		if (fabs(dispDelta[8]) > .8) { //to bryant
			p = vDelta; p = get_imarker()->get_omega(p, pM);
			p = aDelta; p = get_imarker()->get_omega_dot(p, pM);
			ang[0] = atan2(-dispDelta[5], dispDelta[8]);
			ang[2] = atan2(-dispDelta[1], dispDelta[0]);
			s[0] = sin(ang[0]);
			c[0] = cos(ang[0]);
			if (fabs(c[0]) > fabs(s[0])) {
				ang[1] = atan2(dispDelta[2], dispDelta[8] / c[0]);
			} else {
				ang[1] = atan2(dispDelta[2], -dispDelta[5] / s[0]);
			}
			rot_joints[0]->direction(Mat33::XAXIS);
			rot_joints[1]->direction(Mat33::YAXIS);
			rot_joints[2]->direction(Mat33::ZAXIS);
			flag = 1;
			for (i = 1; i < 3; i++) {
				c[i] = cos(ang[i]); s[i] = sin(ang[i]);
			}
			double D[9];
			D[0] = c[2];       D[1] = -s[2];     D[2] = 0.0;
			D[3] = s[2] * c[1];  D[4] = c[2] * c[1]; D[5] = 0.0;
			D[6] = -c[2] * s[1]; D[7] = s[2] * s[1]; D[8] = c[1];
			for (int j = 0; j < 9; j++) D[j] /= c[1];
			fromOmega = D;
		}
	}
	else { //bryant
		if (fabs(dispDelta[2]) > .8) { //to euler
			p = vDelta; p = get_imarker()->get_omega(p, pM);
			p = aDelta; p = get_imarker()->get_omega_dot(p, pM);
			ang[0] = atan2(dispDelta[2], -dispDelta[5]);
			ang[2] = atan2(dispDelta[6], dispDelta[7]);
			s[2] = sin(ang[2]);
			c[2] = cos(ang[2]);
			if (fabs(c[2]) > fabs(s[2])) {
				ang[1] = atan2(dispDelta[7] / c[2], dispDelta[8]);
			} else {
				ang[1] = atan2(dispDelta[6] / s[2], dispDelta[8]);
			}
			rot_joints[0]->direction(Mat33::ZAXIS);
			rot_joints[1]->direction(Mat33::XAXIS);
			rot_joints[2]->direction(Mat33::ZAXIS);
			flag = 1;
			for (i = 0; i < 2; i++) {
				c[i] = cos(ang[i]); s[i] = sin(ang[i]);
			}
			double D[9];
			D[0] = s[2];       D[1] = c[2];       D[2] = 0.0;
			D[3] = c[2] * s[1];  D[4] = -s[2] * s[1]; D[5] = 0.0;
			D[6] = -s[2] * c[1]; D[7] = -c[2] * c[1]; D[8] = s[1];
			for (int j = 0; j < 9; j++) D[j] /= s[1];
			fromOmega = D;
		}
	}
	if (!flag) return flag;
	//update disp
	get_states(tempD);
	for (i = numTra; i < 3 + numTra; i++) tempD[i] = ang[i - numTra];
	p = set_states(tempD); update();
	newAxes = get_zi_global();
	//solver for omega:
	vecTemp = vDelta;
	vecTemp = get_imarker()->vec_wrt_this(vecTemp);
	vecTemp = fromOmega * vecTemp;
	get_states(tempD, od_object::VEL);
	for (i = numTra; i < 3 + numTra; i++) tempD[i] = vecTemp.v[i - numTra];
	set_states(tempD, od_object::VEL); update();
	//solve for acceleration
	//w_cross_z1 = 0
	//w_cross_z2:
	Vec3 w_cross_z2, w_cross_z3;
	vecTemp = rot_joints[1]->from()->get_omega(vecTemp, pM);
	cross_product_with_double(vecTemp, newAxes + 3 + numTra * 3, w_cross_z2);
	w_cross_z2.multiplied_by(rot_joints[1]->v());
	vecTemp = rot_joints[2]->from()->get_omega(vecTemp, pM);
	cross_product_with_double(vecTemp, newAxes + 6 + numTra * 3, w_cross_z3);
	w_cross_z3.multiplied_by(rot_joints[2]->v());
	vecTemp = aDelta;
	vecTemp -= w_cross_z2;
	vecTemp -= w_cross_z3;
	vecTemp = get_imarker()->vec_wrt_this(vecTemp);
	vecTemp = fromOmega * vecTemp;
	get_states(tempD, od_object::ACC);
	for (i = numTra; i < 3 + numTra; i++) tempD[i] = vecTemp.v[i - numTra];
	set_states(tempD, od_object::ACC); update();
	return flag;
}



/*
int od_joint::set_reversed() {
	int i, size_;
	if (reversed == 0) {
		reversed = 1;
		if (num_rot_joints) {
			for (i = 0; i < num_rot_joints; i++) (rot_joints[i])->reverse();
			vector<jprim_rot*> temp_rj(num_rot_joints);
			for (i = 0; i < num_rot_joints; i++) temp_rj[i] = rot_joints[i];
			for (i = 0; i < num_rot_joints; i++) rot_joints[i] = temp_rj[i];
			temp_rj.clear();
		}
		if (num_tra_joints) {
			for (i = 0; i < num_tra_joints; i++) (tra_joints[i])->reverse();
			vector<jprim_tra*> temp_tj(num_tra_joints);
			for (i = 0; i < num_tra_joints; i++) temp_tj[i] = tra_joints[i];
			for (i = 0; i < num_tra_joints; i++) tra_joints[i] = temp_tj[i];
			temp_tj.clear();
		}
		size_ = betweenij.size();
		if (size_) {
			vector<od_marker*> pmarkers(size_);
			for (i = 0; i < size_; i++) pmarkers[i] = betweenij[i];
			for (i = 0; i < size_; i++) betweenij[i] = pmarkers[i];
			pmarkers.clear();
		}
		od_marker* temp_mar = i_marker;
		i_marker = j_marker;
		j_marker = temp_mar;
	}
	return 1;
}*/



double* od_joint::
contribute_to_JR(double* p, od_constraint::DOF_TYPE __type) {
	double *_p = 0;
	int i;
	int temp = 3 * num_tra_joints;
	int temp1 = 3 * num_rot_joints;
	_p = get_zi_global();
	fill(p, p + temp, 0.0);
	for (i = 0; i < temp1; i++) p[i + temp] = _p[i + temp];
	return p;
}

double* od_joint::
contribute_to_JRdot(double* p, od_constraint::DOF_TYPE __type) {
	double *_p = 0;
	int temp = 3 * num_tra_joints;
	int temp1 = 3 * num_rot_joints;
	_p = get_cross_zi();
	if (num_tra_joints) fill(p, p + temp, 0.0);
	for (int i = 0; i < temp1; i++) p[i + temp] = _p[i + temp];
	return p;
}

double* od_joint::
contribute_to_JT(double* vec, od_constraint* pC1) {
	int j, tempInt,j3;
	double *pd, *pd1, *d_v, *pd2;
	od_joint* pC = (od_joint*)pC1;
	if (pC) {
		tempInt = 3 * pC->num_tra();
		pd1 = pC->get_zi_global();
		d_v = distance_between_cm_markers;
		for (j = 0; j < ((od_joint*)pC)->num_rot(); j++) {
			j3 = tempInt + j + j + j;
			pd = pd1 + j3;
			pd2 = vec + j3;
			CROSS_X(pd, d_v, pd2);
		}
	}
	else {
		pd1 = this->get_zi_global();
		tempInt = 3 * num_tra();
		for (j = 0; j < tempInt; j++) vec[j] = pd1[j];
		d_v = distance_to_i_cm_marker;
		for (j = 0; j < num_rot_joints; j++) {
			j3 = tempInt + j+j+j; pd = pd1 + j3; pd2 = vec + j3;
			CROSS_X(pd, d_v, pd2);
		}
	}
	return vec;
}


double* od_joint::
contribute_to_JTdot(double* vec, od_constraint* pC1) {
	int i, j, tempInt , tempInt1;
	double vec1[3], vec2[3];
	double *pd = 0, *z, *wxz;
	double *pd1 = 0, *pd3=0;
	double *d_v, *v_v;
	od_joint* pC = (od_joint*)pC1;
	if (pC) {
		int _num_rot = pC->num_tra();
		tempInt = 0;
		if (_num_rot) {
			tempInt = 3 * _num_rot;
			for (i = 0; i < tempInt; i++) vec[i] = 0.0;
		}
		_num_rot = pC->num_rot();
		if (_num_rot) {
			d_v = distance_between_cm_markers;
			v_v = velocity_between_cm_markers;

			z = pC->get_zi_global();
			wxz = pC->get_cross_zi();
			pd3 = vec;
			for (j = 0; j < pC->num_rot(); j++) {
				tempInt1 = tempInt + j + j + j;
				pd1 =wxz+ tempInt1;
				pd =z+ tempInt1;
				//(omega x Zj) X Rij
				CROSS_X(pd1, d_v, vec1);
				//Zj x Rij_dot
				CROSS_X(pd, v_v, vec2);
				pd3 = vec + tempInt1;
				ADD3(vec1, vec2, pd3);
			}
		}
	}
	else {
		tempInt = 3 * num_tra();
		for (i = 0; i < tempInt; i++) vec[i] = 0.0;
		z = get_zi_global();
		wxz = get_cross_zi();
		//Omega x Zt
		d_v = distance_to_i_cm_marker;
		v_v = velocity_to_i_cm_marker;

		for (j = 0; j < tempInt; j++) vec[j] = wxz[j];
		for (j = 0; j < num_rot_joints; j++) {
			tempInt1 = tempInt + j + j + j;
			pd1 =wxz+ tempInt1;
			pd = z+ tempInt1;
			//(omega x Zj) X Ri
			CROSS_X(pd1, d_v, vec1);
			//Zj X Ri_dot
			CROSS_X(pd, v_v, vec2);
			pd3 = vec + tempInt1;
			ADD3(vec1, vec2, pd3);
		}
	}
	return vec;
}

OdJoint::OdJoint(int id, char* name_)
{
	_id = id;
	pJ = 0;
	_expr = 0;
	pi = 0;
	pj = 0;
	strcpy(_name, name_);
	_impl = 0;
	this->revolute();
}


OdJoint::~OdJoint() {

}

void OdJoint::revolute() {
	_type = od_object::REVOLUTE;
}

void OdJoint::translational() {
	_type = od_object::TRANSLATIONAL;
}

void OdJoint::spherical() {
	_type = od_object::SPHERICAL;
}

void OdJoint::universal() {
	_type = od_object::UNIVERSAL;
}

void OdJoint::free() {
	_type = od_object::FREE;
}

void OdJoint::fixed() {
	_type = od_object::FIXED;
}

void OdJoint::txyrz() {
	_type = od_object::TXYRZ;
}

void OdJoint::set_imarker(OdMarker *pM) {
	pi = pM;
}
void OdJoint::set_jmarker(OdMarker *pM) {
	pj = pM;
}
char* OdJoint::info(char* msg) {
	if (pJ) msg = pJ->info(msg);
	return msg;
}

void OdJoint::set_expr(char* pexpr, int i) {
	/*int len = strlen(pexpr);
	_expr = new char[len + 1];
	strcpy(_expr, pexpr);*/
	funs[i] = std::string(pexpr);
}

od_joint* OdJoint::core()
{
	if (pJ) return pJ;
	od_marker *pm;
	if (_type == od_object::REVOLUTE) {
		pJ = (od_joint*)(new od_revolute_joint(_id, _name, 1));
	}
	else if (_type == od_object::TRANSLATIONAL) {
		pJ = (od_joint*)(new od_translational_joint(_id, _name, 1));
	}
	else if (_type == od_object::SPHERICAL) {
		pJ = (od_joint*)(new od_spherical_joint(_id, _name, 1));
	}
	else if (_type == od_object::UNIVERSAL) {
		pJ = (od_joint*)(new od_universal_joint(_id, _name, 1));
	}
	else if (_type == od_object::FIXED) {
		pJ = (od_joint*)(new od_fixed_joint(_id, _name, 1));
		if (_id < 0) explicitFixed(1);
	}
	else if (_type == od_object::TXYRZ) {
		pJ = (od_joint*)(new od_txyrz_joint(_id, _name, 1));
	}
	else if (_type == od_object::FREE) {
		pJ = (od_joint*)(new od_free_joint(_id, _name, 1));
	}
	//if (_expr) pJ->set_expr(_expr);
	for (std::map<int, string>::iterator it = funs.begin(); it != funs.end(); ++it) {
		pJ->set_expr((it->second).c_str(), it->first);
	}
	if (pi) {
		pm = pi->core();
		pJ->set_imarker(pm);
	}
	if (pj) {
		pm = pj->core();
		pJ->set_jmarker(pm);
	}
	return pJ;
}

double* OdJoint::disp(double *vals) {
	return (pJ)->pva(vals, 0);
}

double* OdJoint::vel(double *vals) {
	return (pJ)->pva(vals, 1);
}

double* OdJoint::acc(double *vals) {
	return (pJ)->pva(vals, 2);
}

int OdJoint::dofs() {
	return pJ->dofs();
}

void OdJoint::explicitFixed(int im) {
	_impl = im;
}

int OdJoint::explicitFixed() {
	return _impl;
}
