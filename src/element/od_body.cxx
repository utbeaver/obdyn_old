#include <algorithm>
#include <functional>
#include <iostream>
#include "od_body.h"
#include "od_body_api.h"
#include "od_marker_api.h"
#include "od_system.h"

od_body::od_body(int Id, char* name_, int Re) : od_element(Id, name_, Re) {
	double temps[] = { 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 };
	double zeros[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	element_type = Type_Body;
	pos_rhs = ang_rhs = 0;
	cm = 0;
	if (Id == 0) {
		is_ground = 1;
	}
	else {
		is_ground = 0;
	}
	from = 0;
	if (real) {
		_mass = (1.0);
		_I = (temps);
	}
	else {
		_mass = (0.0);
		_I = (zeros);
	}
	//nonlinear_term.init();
	ZERO3(Jw);
	ZERO3(nonlinear_term);
	ZERO3(Jw_dot);
}

void od_body::re_eval_info() {
	ostringstream ost;
	info_str.clear();
	if (id == 0) {
		ost << "Ground body Id: " << id << endl;
	}
	else {
		ost << "Body Id: " << id << endl;
	}
	info_str += ost.str();
}
void od_body::add_cm_marker(od_marker *CM) {
	cm = CM;
	//cm->set_body(this);
	cm->pbody = this;
	cm->set_cm_marker();
	marker_list.push_back(cm);
}

void od_body::add_marker(od_marker *mar, int _global) {
	marker_list.push_back(mar);
	mar->set_body(this);
	if (this->ground_body() == 1) {
		mar->for_ground_marker_only();
	}else if (this->cm != 0 ) {
		if (_global == 1) {
			mar->make_local_to(this->cm_marker());
		}
		mar->update_marker(this->cm_marker());
	}
}



void od_body::remove_marker(od_marker* pM) {
	vector<od_marker*>::iterator matched =
		find_if(marker_list.begin(), marker_list.end(), bind2nd(mem_fun(&od_element::is_id), pM->get_id()));
	marker_list.erase(matched);
}

int od_body::update(od_systemGeneric *psys, int ini) {
	if (is_evaluated()) return 1;
	double dou[9];
	double *pd = dou;
	od_marker* mar;
	//  unsigned numMars;
	//update cm marker  wrt to from marker
	  //from->evaluated();
	if (ini) {
		cm->update_cm_marker(from, psys);
		evaluated();
	}
	int numMars = (int)marker_list.size();
	for (int i = 0; i < numMars; i++) {
		mar = marker_list[i];
		if (mar != from && mar != cm ) {
			mar->update_marker(cm, psys);
		}
	}
	/*vector<od_marker*>::iterator iter=marker_list.begin();
	vector<od_marker*>::iterator end_=marker_list.end();
	while(iter != end_) {
		(*iter)->update_marker(cm,psys); iter++;
	}*/

	if (!ini) return 1;
	od_object::DVA_TYPE _type;
	if (psys) {
		_type = psys->get_dva_type();
	}
	else {
		_type = od_object::DISP;
	}
	if (_type == od_object::ACC || _type == od_object::DISP_VEL_ACC) {
		// Vec3 _omega;
		 //update global tensor, _Ia is used first to hold transformation mtx
		_Ia = *(cm->mat());
		_Ia = _I.TMTt(_Ia);
		pd = cm->get_omega();
		//update nonlinear term: omega X J omega
		//1. vecTemp=J*omega
		mat_doubles(_Ia, pd, Jw);
		//2. _omega^J*_omega
		CROSS_X(pd, Jw, nonlinear_term);
		mat_doubles(_Ia, cm->get_omega_dot(), Jw_dot);
		pd = cm->get_acceleration();
		for (int i = 0; i < 3; i++) mx_dot[i] = _mass * pd[i]; 
	}
	evaluated();
	return 1;
}


double od_body::getKE() {
	double ke = 0.0;
	double *vec;
	double vecTemp1[3];
	vec = cm->get_omega();
	mat_doubles(_Ia, vec, vecTemp1);
	ke += DOT_X(vecTemp1, vec);
	vec = cm->get_velocity();
	//for(int i=0; i<3; i++) ke += get_M()*vec[i]*vec[i];
	ke += DOT_X(vec, vec)*get_M();
	ke *= .5;
	return ke;
}

double* od_body::getPosition() const {
	return cm->get_position();
}

double* od_body::getVel() const {
	return cm->get_velocity();
}
void od_body::parJparq_v(double *pd, double *e, double *e1, int omega_dot) {
	double  vecTemp2[3];
	double vecTemp1[3], vecTemp[3];
	double *pd1;
	//e x Jw
	if (!omega_dot) {
		pd1 = cm->get_omega();
		CROSS_X(e, Jw, vecTemp1);
	}
	else {
		pd1 = cm->get_omega_dot();
		CROSS_X(e, Jw_dot, vecTemp1);
	}

	//J[(e x w)-pW(dot)_parq]
	CROSS_X(e, pd1, vecTemp);
	U_SUB3(vecTemp, e1);
	mat_doubles(_Ia, vecTemp, vecTemp2);
	SUB3(vecTemp1, vecTemp2, pd);
}

void  od_body::v_x_parJparq_v(double *pd, double *e, double* e1) {
	double vecTemp[3];
	double* _omega = cm->get_omega();
	parJparq_v(vecTemp, e, e1);
	CROSS_X(_omega, vecTemp, pd);
}

void od_body::parWxJW_parq(double* vecTemp, double* parWparq) {
	double vecTemp1[3];
	double *_omega = cm->get_omega();
	J_v(vecTemp, parWparq);
	CROSS_X(_omega, vecTemp, vecTemp1);
	CROSS_X(parWparq, Jw, vecTemp);
	//vecTemp += vecTemp1;
	U_ADD3(vecTemp, vecTemp1)
}

void od_body::initialize() {
	int markers;
	markers = (int)marker_list.size();
	for (int i = 0; i < markers; i++) {
		marker_list[i]->update_marker(cm);
	}
}

void od_body::for_ground_body_only() {
	if (is_ground)
		for (unsigned i = 0; i < marker_list.size(); i++) {
			marker_list[i]->for_ground_marker_only();
		}
}



OdBody::OdBody(int id, char* name_) {
	pB = new od_body(id, name_);
}

char* OdBody::info(char* msg) {
	msg = pB->info(msg);
	return msg;
}
void OdBody::add_cm_marker(OdMarker *pM) {
	od_marker *pm = pM->core();
	pB->add_cm_marker(pm);
}
void OdBody::add_marker(OdMarker *pM) {
	od_marker *pm = pM->core();
	pB->add_marker(pm, 0);
}
void OdBody::add_global_marker(OdMarker *pM) {
	od_marker *pm = pM->core();
	pB->add_marker(pm, 1);
}
od_body* OdBody::core() { return pB; }

OdBody::~OdBody() {
	// if (pB) delete pB;
}

void OdBody::setMass(double ma) {
	pB->set_mass(ma);
}
void OdBody::setI(V6* a) {
	pB->set_I(a->ptr());
}
