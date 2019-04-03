
#ifdef _WINDOWS
# include <windows.h>
#endif
#include <functional>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "od_system.h"
#include "od_system_api.h"
#include "od_subsystem_api.h"
#include "od_body_api.h"
#include "od_constraint_api.h"
#include "od_force_api.h"
#include "od_marker_api.h"
#include "od_topo.h"
#include "od_equset.h"
#include "od_loop.h"

#define BIG_INT 100000000
//int virtual_joint_id;
//int virtual_body_id;
//int virtual_marker_id;
od_systemGeneric::od_systemGeneric(char* Name, int from_py) : _system(Name, from_py) {
	init();
}
od_systemMechanism::od_systemMechanism(char* Name, int from_py) : od_systemGeneric(Name, from_py) {
	_tree_rhs_si2 = 0;
	dofmap_.resize(0);
	relevenceLevel1 = 0;
	relevenceLevel2 = 0;
	relevenceLevel3 = 0;
}
void od_systemGeneric::init() {
	pSys = this;
	constraint_list_ = 0;
	element_list_ = 0;
	initialized = 0;
	joint_force_list_ = 0;
	body_list_ = 0;
	nbody = 0;
	njoint = 0;
	nforce = 0;
	njforce = 0;
	nsystem = 0;
	states = 0;
	dstates = 0;
	ddstates = 0;
	M_array = 0;
	J_array = 0;
	G_array.set(0.0, -9.8, 0.0);
	_tree_rhs = 0;
	_subSys = 0;

	tree_ndofs = 0;
	lambda_dof = 0;
	start_index = 0;
	start_bindex = 0;
	pGround = 0;// new od_body(id = 0, "ground");
}
int od_systemGeneric::get_num_body() {
	int num = body_list.size();
	//for (int i = 0; i < nsystem; i++) num += _subSys[i]->get_num_body();
	return num;
}
int od_systemGeneric::get_num_constraint() {
	int num = constraint_list.size();
	//for (int i = 0; i < nsystem; i++) num += _subSys[i]->get_num_constraint();
	return num;
}
void od_systemGeneric::add_expression(Expression *expr) {
	expr_list.push_back(expr);
}
void od_systemGeneric::add_subsystem(od_systemGeneric* psys) {
	if (!psys) return;
	psys->ground(this->ground());
	subSys.push_back(psys);
}
void od_systemGeneric::add_body(od_body *pb) {
	int index;
	if (pb->get_id() == 0) {
		pGround = pb;
		index = -1;
	}
	/*else if (pb->get_id() < 0) {
		body_list.push_back(pb);
		aux_element_list.push_back((od_element*)pb);
		index = body_list.size() - 1;
	}*/
	else {
		body_list.push_back(pb);
		element_list.push_back((od_element*)pb);
		index = body_list.size() - 1;
	}
	pb->set_index(index); idBody[pb->get_id()] = pb;
}
void od_systemGeneric::add_force(od_force *pf) {
	force_list.push_back(pf);
	element_list.push_back((od_element*)pf);
	int index = force_list.size() - 1;
	idForce[pf->get_id()] = pf;
}
void od_systemGeneric::add_joint_force(od_joint_force *pf) {
	joint_force_list.push_back((od_jointF*)pf);
	element_list.push_back((od_element*)pf);
}
void od_systemGeneric::add_joint_spdp(od_joint_spdp *pf) {
	joint_force_list.push_back((od_jointF*)pf);
	element_list.push_back((od_element*)pf);
}
void od_systemGeneric::add_measure(od_measure *pf) {
	measure_list.push_back(pf);
	element_list.push_back((od_element*)pf);
}
void od_systemGeneric::add_marker(od_marker *pm) {
	int index;
	marker_list.push_back(pm);
	if (pm->get_id() >= 0) {
		element_list.push_back((od_element*)pm);
	}
	else {
		aux_element_list.push_back((od_element*)pm);
	}
	index = marker_list.size() - 1;
	idMarker[pm->get_id()] = pm;
	pm->set_index(index);
}
void od_systemGeneric::add_constraint(od_constraint *pc) {
	int index;
	//add real joint,
	constraint_list.push_back(pc);
	if (pc->get_id() >= 0) {
		element_list.push_back((od_element*)pc);
	}
	else {
		//add floating joint
		aux_element_list.push_back((od_element*)pc);
	}
	index = constraint_list.size() - 1;
	idJoint[pc->get_id()] = pc;

	pc->set_index(index);
	/*int motionNum = pc->numMotions()*0;
	for (int i = 0; i < motionNum; i++) {
		od_motion* pm = pc->motion(i);
		motion_list.push_back(pm);
	}*/
}
void od_systemGeneric::add_explicit_constraint(od_constraint *pc) {
	explicit_constraint_list.push_back(pc);
	aux_element_list.push_back((od_element*)pc);
	int index = explicit_constraint_list.size() - 1;
	pc->set_index(index);
}
void od_systemGeneric::get_states() {
	int i;
	od_constraint *pC;
	double* temp_st;
	double* temp_dst;
	double* temp_ddst;
	temp_st = states;
	temp_dst = dstates;
	temp_ddst = ddstates;
	int _num_joint = num_joint();
	DVA_TYPE _type = get_dva_type();
	if (_type == od_object::DISP || _type == od_object::DISP_VEL_ACC) {
		for (i = 0; i < _num_joint; i++) {
			pC = *(constraint_list_ + i);
			temp_st = pC->get_states(temp_st, od_object::DISP);
		}
	}
	if (_type == od_object::VEL || _type == od_object::DISP_VEL_ACC) {
		for (i = 0; i < _num_joint; i++) {
			pC = *(constraint_list_ + i);
			temp_dst = pC->get_states(temp_dst, od_object::VEL);
		}
	}
	if (_type == od_object::ACC || _type == od_object::DISP_VEL_ACC) {
		for (i = 0; i < _num_joint; i++) {
			pC = *(constraint_list_ + i);
			temp_ddst = pC->get_states(temp_ddst, od_object::ACC);
		}
	}
	//unset_evaluated();
}
void od_system::get_states() {
	//for (int i = 0; i < nsystem; i++) _subSys[i]->get_states();
	od_systemGeneric::get_states();
}
void od_systemGeneric::set_states() {
	od_constraint *pC;
	double* temp_st;
	double* temp_dst;
	double* temp_ddst;
	temp_st = states;
	temp_dst = dstates;
	temp_ddst = ddstates;
	DVA_TYPE _type = get_dva_type();
	int _num_body = num_body();

	for (int i = 0; i < _num_body; i++) {
		pC = *(constraint_list_ + i);
		if (_type == od_object::DISP || _type == od_object::DISP_VEL_ACC)
			temp_st = pC->set_states(temp_st, od_object::DISP);
		if (_type == od_object::VEL || _type == od_object::DISP_VEL_ACC)
			temp_dst = pC->set_states(temp_dst, od_object::VEL);
		if (_type == od_object::ACC || _type == od_object::DISP_VEL_ACC)
			temp_ddst = pC->set_states(temp_ddst, od_object::ACC);
	}
	unset_evaluated();
}
void od_system::set_states() {
	//	for (int i = 0; i < nsystem; i++) _subSys[i]->set_states();
	od_systemGeneric::set_states();
}
int od_systemMechanism::checkEulerBryant() {
	od_joint *pC;
	int i;
	int flag = 0;
	//for (i = 0; i < nsystem; i++) {
	//	flag = _subSys[i]->checkEulerBryant();
	//	if (flag) return flag;
	//}
	for (i = 0; i < num_body(); i++) {
		pC = (od_joint*)(*(constraint_list_ + i));
		flag = pC->checkEulerBryant();
		if (flag) 
			return flag;
	}
	return flag;
}
od_marker* od_systemGeneric::get_marker_via_id(int id)
{
	//for (int i = 0; i < nsystem; i++) {
	//	od_marker* pc = _subSys[i]->get_marker_via_id(id);
	//	if (pc) return pc;
	//}
	map<int, od_marker*>::iterator it = idMarker.find(id);
	if (it != idMarker.end())
		return (*it).second;
	else
		return 0;
}
od_constraint* od_systemGeneric::get_constraint_via_id(int id) {
	//for (int i = 0; i < nsystem; i++) {
	//	od_constraint* pc = _subSys[i]->get_constraint_via_id(id);
	//	if (pc) return pc;
	//}
	map<int, od_constraint*>::iterator it = idJoint.find(id);
	if (it != idJoint.end())
		return (*it).second;
	else
		return 0;
}
void od_systemMechanism::updateQ() {
	int i;
	//for (i = 0; i < nsystem; i++) _subSys[i]->updateQ();
	od_joint *pCi;
	int num_j = num_joint();
	for (i = 0; i < num_j; i++) {
		pCi = (od_joint*)(*(constraint_list_ + i));
		pCi->updateQ();
	}
}
void od_systemMechanism::updatePartials(int pos_only) {
	int i,  k, startindex, num_dof, idx, _len;
	//for (i = 0; i < nsystem; i++) _subSys[i]->updatePartials(pos_only);
	
	Vec3 **tgt;
	od_joint* pC = 0;
	int nbody = num_body();
	updateQ();
	calculate_JR();
	calculate_JT();
	if (pos_only) return;
	calculate_JRdot();
	calculate_JTdot();

	//parOmega_Parq:  par_J/par_q*q_dot
	parOmega_parq.init();// .create_jacobian(nbody, tree_ndofs, relevenceLevel3);
	for (i = 0; i < nbody; i++) {
		pC = (od_joint*)(*(constraint_list_ + i));
		startindex = pC->get_start_index();
		num_dof = pC->dofs();
		for (k = 0; k < num_dof; k++) {
			tgt = parOmega_parq.column(startindex + k, &idx, &_len);
			pC->setPartialVec3(k, tgt);
		}
		multiplyParMatRotVec_q(nbody, constraint_list_, i, 0, 1);// , 0);
	}
	//parOmega_dot/par_q
	parOmega_dot_parq.init();// create_jacobian(nbody, tree_ndofs, relevenceLevel3);

	for (i = 0; i < nbody; i++) {
		pC = (od_joint*)(*(constraint_list_ + i));
		startindex = pC->get_start_index();
		num_dof = pC->dofs();
		for (k = 0; k < num_dof; k++) {
			tgt = parOmega_dot_parq.column(startindex + k, &idx, &_len);
			pC->setPartialVec3(k, tgt);
		}
		multiplyParMatRotVec_q(nbody, constraint_list_, i, 0, 2);// , 0);
		multiplyParMatRotVec_q(nbody, constraint_list_, i, 1, 1);// , 1);
	}

	//parOmega_dot/par_q_dot
	  //parOmega_dot_parq_dot.init();//create_jacobian(nbody, tree_ndofs, relevenceLevel3);
	parOmega_dot_parq_dot = parOmega_parq;
	parOmega_dot_parq_dot += JR_dot;
	//parVel_parq = parJp_parqp * qp_dot + parJa_parqp * qa_dot
	parVel_parq.init();//create_jacobian(nbody, tree_ndofs, relevenceLevel3);
	parVel_parqG.init();//create_jacobian(nbody, tree_ndofs, relevenceLevel2);
	for (i = 0; i < nbody; i++) {
		pC = (od_joint*)(*(constraint_list_ + i));
		num_dof = pC->dofs();
		startindex = pC->get_start_index();
		for (k = 0; k < num_dof; k++) {
			tgt = parVel_parq.column(startindex + k, &idx, &_len);
			pC->setPartialVec3(k, tgt);
		}
		multiplyParMatTraVec_q_I(nbody, constraint_list_, i, 0, /*type=0 wrt to disp*/  1);// , 0);
		multiplyParMatTraVec_q_II(nbody, constraint_list_, i, 0, /*type=0 wrt to disp*/  1);// , 1);
	}
	multiplyMat_Assembly(JMAT, parVel_parq, parVel_parqG, constraint_list_);
	//parVel_parq_dotG =JTG

  //parVel_dot_parq =  parJp_parqp * qp_ddot + parJa_parqp * qa_ddot
  //+ d( parJp_parqp)/dt *qp_dot + d(parJa_parqp)/dt * qa_dot
	parVel_dot_parq.init();//create_jacobian(nbody, tree_ndofs, relevenceLevel3);
	parVel_dot_parqG.init();//create_jacobian(nbody, tree_ndofs, relevenceLevel2);
	//parVel_dot_parq_alpha.init();
	//parVel_dot_parqG_alpha.init();
	for (i = 0; i < nbody; i++) {
		pC = (od_joint*)(*(constraint_list_ + i));
		startindex = pC->get_start_index();
		num_dof = pC->dofs();
		for (k = 0; k < num_dof; k++) {
			tgt = parVel_dot_parq.column(startindex + k, &idx, &_len);
			pC->setPartialVec3(k, tgt);
		}
		multiplyParMatTraVec_q_I(nbody, constraint_list_, i, 0, 2);// , 0);
		multiplyParMatTraVec_q_II(nbody, constraint_list_, i, 0, 2);// , 1);
		multiplyParMatTraVec_q_I(nbody, constraint_list_, i, 1, 1);// , 1);
		multiplyParMatTraVec_q_II(nbody, constraint_list_, i, 1, 1);// , 1);
	}
	//parVel_dot_parq = parVel_dot_parq_alpha;
	//multiplyMat_Assembly(JMAT, parVel_dot_parq_alpha, parVel_dot_parqG_alpha, constraint_list_);
	multiplyMat_Assembly(JMAT, parVel_dot_parq, parVel_dot_parqG, constraint_list_);
	//parVel_dot/par_qp_dot
	//parVel_dot_parq_dotG.init();//create_jacobian(nbody, tree_ndofs, relevenceLevel2);

	parVel_dot_parq_dotG = parVel_parqG;
	parVel_dot_parq_dotG += JTG_dot;
	for (i = 0; i < num_force(); i++) force_list_[i]->evaluate_Jac();
	for (i = 0; i < num_jforce(); i++) joint_force_list_[i]->evaluate_Jac();
	//for_each(force_list.begin(), force_list.end(), mem_fun(&od_force::evaluate_Jac));
	//for_each(joint_force_list.begin(), joint_force_list.end(), mem_fun(&od_joint_force::evaluate_Jac));
}

void od_systemMechanism::calculate_JR(int no_dot) {
	int temp_int, num_dofs, temp_k;
	double fltTemp[18];
	int i, j, k;
	Vec3 *pV;
	//  for(i=0; i<nsystem; i++) _subSys[i]->calculate_JR(no_dot);
	double *p = fltTemp;
	int nb = this->num_body();
	int nj = this->num_joint();
	od_joint* pJ;
	if (no_dot) {
		JR.init();// create_jacobian(nb, tree_ndofs, relevenceLevel3);
	}
	else {
		JR_dot.init(); //create_jacobian(nb, tree_ndofs, relevenceLevel3);
	}

	for (j = 0; j < nj; j++) {
		pJ = (od_joint*)(*(constraint_list_ + j));
		num_dofs = pJ->dofs();
		temp_int = pJ->get_start_index();
		fill(fltTemp, fltTemp + 18, 0.0);
		p = fltTemp;
		if (no_dot)
			p = pJ->contribute_to_JR(p, od_object::ROT_DOF);
		else
			p = pJ->contribute_to_JRdot(p, od_object::ROT_DOF);
		for (i = j; i < nj; i++) {
			if (!relevenceLevel1[i*nj + j]) {
				break;
			}
			p = fltTemp;
			for (k = 0; k < num_dofs; k++) {
				temp_k = temp_int + k;
				if (pJ->if_rotation(k)) {
					if (no_dot) pV = JR.element(i, temp_k);
					else pV = JR_dot.element(i, temp_k);
					EQ3(pV->v, p);
				}
				p += 3;
			}
		}
	}
}
void od_systemMechanism::calculate_JRdot() {
	calculate_JR(0);
}
void od_systemMechanism::calculate_JT(int no_dot) {
	int temp_int, num_dofs, num_j, temp_k;
	int i, j, k;
	double vec[18];
	double* pvec = vec;
	od_joint *pCi, *pCj;
	Vec3 *pV;
	//  for(i=0; i<nsystem; i++) _subSys[i]->calculate_JT(no_dot);
	num_j = num_joint();
	if (no_dot) {
		JTL.init(); //create_jacobian(num_body(), tree_ndofs, relevenceLevel3);
		JMAT.init(); //create_jacobian(num_body(), num_body(), relevenceLevel1);
		JTG.init(); //create_jacobian(num_body(), tree_ndofs, relevenceLevel2);
	}
	else {
		JTL_dot.init(); //create_jacobian(num_body(), tree_ndofs, relevenceLevel3);
		JMAT.init(); //create_jacobian(num_body(), num_body(), relevenceLevel1);
		JTG_dot.init(); //create_jacobian(num_body(), tree_ndofs, relevenceLevel2);
	}
	for (j = 0; j < num_j; j++) {
		pCj = (od_joint*)(*(constraint_list_ + j));
		num_dofs = pCj->dofs();
		temp_int = pCj->get_start_index();

		for (i = j; i < num_j; i++) {
			if (!relevenceLevel1[i*num_j + j]) {
				break;
			}
			fill(vec, vec + 18, 0.0);
			pvec = vec;
			pCi = (od_joint*)(*(constraint_list_ + i));
			if (i == j) {
				if (no_dot) {
					pvec = pCi->contribute_to_JT(pvec);
				}
				else {
					pvec = pCi->contribute_to_JTdot(pvec);
				}
			}
			else {
				if (no_dot) {
					pvec = pCi->contribute_to_JT(pvec, pCj);
				}
				else {
					pvec = pCi->contribute_to_JTdot(pvec, pCj);
				}
			}
			for (k = 0; k < num_dofs; k++) {
				temp_k = temp_int + k;
				if (pCj->if_rotation(k) || (i == j)) {
					if (no_dot) {
						pV = JTL.element(i, temp_k);
					}
					else pV = JTL_dot.element(i, temp_k);
					//pd = pV->v;
					//*pV=(pvec);
					EQ3(pV->v, pvec);
				}
				pvec += 3;
			}
		}
	}
	if (no_dot) {
		multiplyMat_Assembly(JMAT, JTL, JTG, constraint_list_);
	}
	else {
		multiplyMat_Assembly(JMAT, JTL_dot, JTG_dot, constraint_list_);
	}
}
void od_systemMechanism::calculate_JTdot() {
	calculate_JT(0);
}
int od_systemGeneric::init_dynamics() {
	int i, j, k, ii, size;
	od_body *pBi, *pBj;
	int iIdx, jIdx;
	Vec3 *pV;
	//for (i = 0; i < nsystem; i++) _subSys[i]->init_dynamics();
	for (i = 0; i < 3; i++) parForceparPVA[i].init(nbody, tree_dofs());
	for (i = 0; i < 3; i++) parTorqueparPVA[i].init(nbody, tree_dofs());
	for (i = 0; i < num_force(); i++) {
		iIdx = -1;
		jIdx = -1;
		pBi = force_list_[i]->get_imarker()->get_body();
		if (!pBi->ground_body()) iIdx = pBi->get_index();
		pBj = force_list_[i]->get_jmarker()->get_body();
		if (!pBj->ground_body()) jIdx = pBj->get_index();
		if (iIdx != -1) {
			for (j = 0; j < 3; j++) {
				//iforce
				size = force_list_[i]->get_iforce_partial_size(j);
				for (k = 0; k < size; k++) {
					ii = force_list_[i]->get_iforce_partial_col(k, j);
					pV = force_list_[i]->get_iforce_partial(k, j);
					parForceparPVA[j].addEntry(iIdx, ii, pV);
				}
				//itorque
				size = force_list_[i]->get_itorque_partial_size(j);
				for (k = 0; k < size; k++) {
					ii = force_list_[i]->get_itorque_partial_col(k, j);
					pV = force_list_[i]->get_itorque_partial(k, j);
					parTorqueparPVA[j].addEntry(iIdx, ii, pV);
				}
			}
		}
		if (jIdx != -1) {
			for (j = 0; j < 3; j++) {
				//jforce
				size = force_list_[i]->get_jforce_partial_size(j);
				for (k = 0; k < size; k++) {
					ii = force_list_[i]->get_jforce_partial_col(k, j);
					pV = force_list_[i]->get_jforce_partial(k, j);
					parForceparPVA[j].addEntry(jIdx, ii, pV);
				}
				//jtorque
				size = force_list_[i]->get_jtorque_partial_size(j);
				for (k = 0; k < size; k++) {
					ii = force_list_[i]->get_jtorque_partial_col(k, j);
					pV = force_list_[i]->get_jtorque_partial(k, j);
					parTorqueparPVA[j].addEntry(jIdx, ii, pV);
				}
			}
		}
	}
	for (j = 0; j < 3; j++) {
		parForceparPVA[j].init_dynamics();
		parTorqueparPVA[j].init_dynamics();
	}
	return 1;
}

void od_systemMechanism::parF_parq_dot(double **pM, int base) {
	//parF_par_dot = T^t parFr_parq_dot + T^t parFt_parq_dot
	int i, j, k, _size, idx, _len;
	//for (i = 0; i < nsystem; i++) {
	//	_subSys[i]->parF_parq_dot(pM, base);
	//	base += _subSys[i]->tree_dofs();
	//}
	double tempVec[3], *pd;
	int nbody = num_body();
	pd = tempVec;
	Vec3 **tgt;
	Vec3 **dWdot_dq;
	od_body *pB = 0;
	int tree_dof = tree_dofs();
	//parFr_parq_dot
	Vec3* parFparq = new Vec3[nbody * 2];
	double* par_col = new double[tree_dof];
	Vec3 *vecTemp1 = new Vec3[nbody];
	double vecTemp[3];
	//vector<Vec3*> vecList(nbody);;
	//for (i = 0; i < (int)loop_list.size(); i++) loop_list[i]->evaluate(0);
	for (j = 0; j < tree_dof; j++) {
		for (i = 0; i < nbody + nbody; i++) parFparq[i].init();
		//J*parOmega_dot_parq_dot
		tgt = parOmega_dot_parq_dot.column(j, &idx, &_len);
		dWdot_dq = parOmega_parq_dot.column(j, &idx, &_len);
		for (i = idx; i < idx + _len; i++) {
			pB = body_list_[i];
			pB->J_v(vecTemp, tgt[i]->v);
			parFparq[i] -= vecTemp;
			pd = dWdot_dq[i]->v;
			pB->parWxJW_parq(vecTemp, pd);
			parFparq[i] -= vecTemp;
		}

		//Par from loop_list: see evalJac in od_equation.cxx
		// par from Torque
		_size = parTorqueparPVA[1].length(j);
		for (k = 0; k < _size; k++) {
			Vec3* pV = parTorqueparPVA[1].ithVec(k, j, i);
			if (pV) parFparq[i] += pV;
		}
		tgt = parVel_dot_parq_dotG.column(j, &idx, &_len);
		for (i = idx; i < idx + _len; i++) {
			parFparq[i + nbody] = tgt[i];// ->v;
			parFparq[i + nbody].multiplied_by(-(body_list_[i]->get_M()));
		}
		//Par from loop_list: see evalJac in od_equation.cxx
		//par from Force 
		_size = parForceparPVA[1].length(j);
		for (k = 0; k < _size; k++) {
			Vec3* pV = parForceparPVA[1].ithVec(k, j, i);
			if (pV) parFparq[i + nbody] += pV;
		}
		fill(par_col, par_col + tree_dof, 0.0);
		multiplyMatT_Rotq_Vec(nbody, parFparq, par_col, constraint_list_);
		multiplyMatT_vec(nbody, parFparq + nbody, vecTemp1, constraint_list_);
		multiplyMatT_Traq_Vec(nbody, vecTemp1, par_col, constraint_list_);
		for (k = 0; k < tree_dof; k++) {
			pM[base + k][base + j] += par_col[k];
		}
	}
	int len = num_jforce();
	for (i = 0; i < len; i++) {
		od_jointF* pf = joint_force_list_[i];
		int row = pf->row();
		int size = pf->get_partial_size(1);
		for (j = 0; j < size; j++) {
			int col = pf->get_partial_col(1, j);
			double tempd = pf->get_partial(1, j);
			pM[base + row][base + col] += tempd;
		}
	}
	DELARY(par_col);  DELARY(vecTemp1);  DELARY(parFparq);
}
void od_systemMechanism::topology_analysis_level2() {
	int temp_int, num_dofs, i, j, _value, if_rot;
	int num_nz = 1;
	//int ar_num_nz = 1;
	int num_nz3 = 1;
	//int ar_num_nz3 = 1;
	int sign = 1;
	tree_ndofs = 0;
	Tree_Ndofs = 0;

	vector<int> relevence_level2;
	vector<int> relevence_level3;

	//find the number of dofs

	for (std::vector<od_constraint*>::iterator it = constraint_list.begin(); it != constraint_list.end(); ++it) {
		((od_joint*)(*it))->set_start_index(tree_ndofs);
		//((od_joint*)(*it))->dofMap(dofmap);
		//if (((od_joint*)(*it))->expr()) 
		//	continue;
		tree_ndofs += ((od_joint*)(*it))->dofs();
	}
	int num_body = body_list.size();
	temp_int = num_body * tree_ndofs;
	relevence_level2.resize(temp_int);
	relevence_level3.resize(temp_int);
	fill(relevence_level2.begin(), relevence_level2.end(), 0);
	fill(relevence_level3.begin(), relevence_level3.end(), 0);


	for (i = 0; i < num_body; i++) {
		for (j = 0; j < num_joint(); j++) {
			temp_int = 0;
			sign = (i == j) ? 1 : -1;
			_value = relevenceLevel1[i*num_joint() + j];
			if (_value) {
				od_joint* pJ = (od_joint*)constraint_list[j];
				num_dofs = pJ->dofs();
				temp_int = pJ->get_start_index();
				for (int k = 0; k < num_dofs; k++) {
					if_rot = pJ->if_rotation(k);
					if (if_rot || (i == j)) {
						relevence_level3[i*tree_ndofs + temp_int] = num_nz3 * sign; num_nz3++;
					}
					relevence_level2[i*tree_ndofs + temp_int] = num_nz * sign;
					num_nz++;
					temp_int++;
				}
			}
		}
	}
	if (relevenceLevel2) delete[] relevenceLevel2;
	relevenceLevel2 = new int[relevence_level2.size()];
	for (i = 0; i < (int)relevence_level2.size(); i++) relevenceLevel2[i] = relevence_level2[i];
	if (relevenceLevel3) delete[] relevenceLevel3;
	relevenceLevel3 = new int[relevence_level3.size()];
	for (i = 0; i < (int)relevence_level3.size(); i++) relevenceLevel3[i] = relevence_level3[i];
	cout << "Rele 1 " << endl;
	for (i = 0; i < num_body; i++) {
		for (j = 0; j < num_joint(); j++) {
			cout << setw(4) << relevenceLevel1[i*num_joint() + j];
		}
		cout << endl;
	}
	cout << "rele 2" << endl;
	for (i = 0; i < num_body; i++) {
		for (j = 0; j < tree_ndofs; j++) {
			cout << setw(4) << relevence_level2[i*tree_ndofs + j];
		}
		cout << endl;
	}

	cout << "rele 3" << endl;
	for (i = 0; i < num_body; i++) {
		for (j = 0; j < tree_ndofs; j++) {
			cout << setw(4) << relevence_level3[i*tree_ndofs + j];
		}
		cout << endl;
	}
}
void od_systemMechanism::parF_parq(double **pM, int base)
{
	int i, j, k, start, num_dofs, _size, idx, _len;
	//for (i = 0; i < nsystem; i++) {
	//	_subSys[i]->parF_parq(pM, base);
	//	base += _subSys[i]->tree_dofs();
	//}
	od_joint* pC;
	Vec3 **tgt;
	Vec3 **dWdot_dq;
	Vec3 **dW_dq;
	double tempVec[3], *pd;
	int nbody = num_body();
	int treedof = tree_dofs();
	//parF_parq = T^t parFr_parq + parT_parq Fr
	// parT_parq Fr
	// pd = tempVec;
	for (i = 0; i < nbody; i++) {
		pC = (od_joint*)(*(constraint_list_ + i));
		start = pC->get_start_index();
		num_dofs = pC->dofs();
		//partTr_parq Fr
		multiplyParMatT_Rotq_Vec_I(_tree_rhs, constraint_list_, i, treedof, nbody);
		for (j = 0; j < num_dofs; j++) {
			pd = pC->get_ith_par_col(j, treedof);
			for (k = 0; k < treedof; k++) {
				pM[base + k][base + start + j] += pd[k];
			}
		}
	}
	for (i = 0; i < nbody; i++) {
		pC = (od_joint*)(*(constraint_list_ + i));
		start = pC->get_start_index();
		num_dofs = pC->dofs();
		//partTt_parq Ft
		multiplyParMatT_Traq_Vec_I(_tree_rhs + nbody, constraint_list_, i, treedof, nbody);
		for (j = 0; j < num_dofs; j++) {
			pd = pC->get_ith_par_col(j, treedof);
			for (k = 0; k < treedof; k++) {
				pM[base + k][base + start + j] += pd[k];
			}
		}
	}
	for (i = 0; i < nbody; i++) {
		pC = (od_joint*)(*(constraint_list_ + i));
		start = pC->get_start_index();
		num_dofs = pC->dofs();
		multiplyParMatT_Traq_Vec_II(_tree_rhs + nbody, constraint_list_, i, treedof, nbody);
		for (j = 0; j < num_dofs; j++) {
			pd = pC->get_ith_par_col(j, treedof);
			for (k = 0; k < treedof; k++) {
				pM[base + k][base + start + j] += pd[k];
			}
		}
	}
	// T^t parF_parq
	// Calcultae parF_parq
	Vec3 *parFparq = new Vec3[nbody * 2];
	Vec3 *vecTemp1 = new Vec3[nbody];
	double* par_col = new double[treedof];
	od_body* pB = 0;
	double* pQQ = 0;

	for (j = 0; j < treedof; j++) {
		for (i = 0; i < nbody * 2; i++) parFparq[i].init();
		tgt = JR.column(j, &idx, &_len);
		dWdot_dq = parOmega_dot_parq.column(j, &idx, &_len);
		dW_dq = parOmega_parq.column(j, &idx, &_len);
		for (i = idx; i < idx + _len; i++) {
			pB = body_list_[i];
			//parJ_parq omega_dot
			pQQ = tgt[i]->v;
			pd = dWdot_dq[i]->v;
			pB->parJparq_v(tempVec, pQQ, pd, /*omega_dot=*/ 1);
			parFparq[i] -= tempVec;
			//omega x parJ_parq omega
			pd = dW_dq[i]->v;
			pB->v_x_parJparq_v(tempVec, pQQ, pd);
			parFparq[i] -= tempVec;
			pQQ = pB->J_W();
			CROSS_X(pd, pQQ, tempVec);
			parFparq[i] -= tempVec;
		}

		//Par from loop_list
		//par from Force and Torque
		_size = parTorqueparPVA[0].length(j);
		for (k = 0; k < _size; k++) {
			Vec3* pV = parTorqueparPVA[0].ithVec(k, j, i);
			if (pV) parFparq[i] += pV;
		}
		tgt = parVel_dot_parqG.column(j, &idx, &_len); pd = 0;
		for (i = idx; i < idx + _len; i++) {
			double m = body_list_[i]->get_M();
			parFparq[i + nbody] = tgt[i];// ->v;
			parFparq[i + nbody].multiplied_by(-m);
		}
		//Par from loop_list
		//par from Force and Torque
		_size = parForceparPVA[0].length(j);
		for (k = 0; k < _size; k++) {
			Vec3* pV = parForceparPVA[0].ithVec(k, j, i);
			if (pV) parFparq[i + nbody] += pV;
		}
		fill(par_col, par_col + treedof, 0.0);
		multiplyMatT_Rotq_Vec(nbody, parFparq, par_col, constraint_list_);
		multiplyMatT_vec(nbody, parFparq + nbody, vecTemp1, constraint_list_);
		multiplyMatT_Traq_Vec(nbody, vecTemp1, par_col, constraint_list_);
		for (k = 0; k < treedof; k++) {
			pM[base + k][base + j] += par_col[k];
		}
	}
	int len = num_jforce();
	for (i = 0; i < len; i++) {
		od_jointF* pf = joint_force_list_[i];
		int row = pf->row();
		int size = pf->get_partial_size(0);
		for (j = 0; j < size; j++) {
			int col = pf->get_partial_col(0, j);
			double tempd = pf->get_partial(0, j);
			pM[base + row][base + col] += tempd;
		}
	}
	DELARY(par_col);  DELARY(vecTemp1);  DELARY(parFparq);
}
double** od_systemMechanism::evaluateJac(double **pM, int base) {
//	int i;
	//for (i = 0; i < nsystem; i++) {
	//	_subSys[i]->evaluate_Jac(pM, base);
	//	base += _subSys[i]->tree_dofs();
	//}
	od_object::Analysis_Type _type = get_analysis_type();
	//if (_type == od_object::DYNAMIC || _type == od_object::STATIC || _type == od_object::ACC_FORCE) {
		//for (i = 0; i < num_force(); i++) force_list_[i]->evaluate_rhs();
		//for (i = 0; i < num_jforce(); i++) joint_force_list_[i]->evaluate_Jac();
	//}
	int tree_dof = tree_dofs();
	if (_type == od_object::INIT_DISP ||
		_type == od_object::INIT_VEL ||
		_type == od_object::INIT_ACC) {
		for (int i = 0; i < tree_dof; i++) {
			pM[base + i][base + i] = 1.0;
		}
	}
	else if (_type == od_object::ACC_FORCE) {
		od_systemGeneric::getM(pM, base);
	}
	else if (_type == od_object::STATIC) {
		od_joint* pC;
		int i, j, k, start, num_dofs;
		int nbody = num_body();
		int nbody1 = nbody + nbody;
		for (i = 0; i < num_force(); i++) force_list_[i]->evaluate_Jac();
		for (i = 0; i < num_jforce(); i++) joint_force_list_[i]->evaluate_Jac();
		//parMatT *(G+Fc+Fa)
		for (i = 0; i < nbody; i++) {
			pC = (od_joint*)(*(constraint_list_ + i));
			start = pC->get_start_index();
			num_dofs = pC->dofs();
			multiplyParMatT_Rotq_Vec_I(_tree_rhs, constraint_list_, i, tree_dof, nbody);
			for (j = 0; j < num_dofs; j++) {
				for (k = 0; k < tree_dof; k++) {
					pM[base + k][base + start + j] += (pC->get_ith_par_col(j, tree_dof))[k];
				}
			}
			multiplyParMatT_Traq_Vec_I(_tree_rhs + nbody, constraint_list_, i, tree_dof, nbody);
			for (j = 0; j < num_dofs; j++) {
				for (k = 0; k < tree_dof; k++) {
					pM[base + k][base + start + j] += (pC->get_ith_par_col(j, tree_dof))[k];
				}
			}
			multiplyParMatT_Traq_Vec_II(_tree_rhs + nbody, constraint_list_, i, tree_dof, nbody);
			for (j = 0; j < num_dofs; j++) {
				for (k = 0; k < tree_dof; k++) {
					pM[base + k][base + start + j] += (pC->get_ith_par_col(j, tree_dof))[k];
				}
			}
		}
		//MatT*Par(G+Fc+Fa): ParG = 0, Fc: Constraining force, Fa: External force
		Vec3* vec_list = 0;
		//   int _index[2];
		Vec3* _vecTemp3;
		Vec3* vecTemp1 = new Vec3[nbody];
		//    double vals[6];
		//Lagrangian multipliers has no contribution to JAC
		/*   if(loop_list.size() > 0) {
	  } */
		if (num_force() > 0) {
			if (vec_list == 0) {
				vec_list = new Vec3[nbody1*tree_dof];
			}
			for (i = 0; i < num_force(); i++) {
				od_body *pBi, *pBj;
				int index, col, _size;
				pBi = force_list_[i]->get_imarker()->get_body();
				pBj = force_list_[i]->get_jmarker()->get_body();
				if (pBi->ground_body() == 0) {
					index = pBi->get_index();
					_size = force_list_[i]->get_itorque_partial_size();
					for (j = 0; j < _size; j++) {
						col = force_list_[i]->get_itorque_partial_col(j);
						vec_list[nbody1*col + index] += *force_list_[i]->get_itorque_partial(j);
					}
					_size = force_list_[i]->get_iforce_partial_size();
					for (j = 0; j < _size; j++) {
						col = force_list_[i]->get_iforce_partial_col(j);
						vec_list[nbody1*col + index + nbody] += *force_list_[i]->get_iforce_partial(j);
					}
				}
				if (pBj->ground_body() == 0) {
					index = pBj->get_index();
					_size = force_list_[i]->get_jtorque_partial_size();
					for (j = 0; j < _size; j++) {
						col = force_list_[i]->get_jtorque_partial_col(j);
						vec_list[nbody1*col + index] += *force_list_[i]->get_jtorque_partial(j);
					}
					_size = force_list_[i]->get_jforce_partial_size();
					for (j = 0; j < _size; j++) {
						col = force_list_[i]->get_jforce_partial_col(j);
						vec_list[nbody1*col + index + nbody] += *force_list_[i]->get_jforce_partial(j);
					}
				}
			}
		}
		if (vec_list) {
			double* par_col = new double[tree_dof];
			for (i = 0; i < tree_dof; i++) {
				_vecTemp3 = &(vec_list[nbody1*i]);
				fill(par_col, par_col + tree_dof, 0.0);
				multiplyMatT_Rotq_Vec(nbody, _vecTemp3, par_col, constraint_list_);
				multiplyMatT_vec(nbody, _vecTemp3 + nbody, vecTemp1, constraint_list_);
				multiplyMatT_Traq_Vec(nbody, vecTemp1, par_col, constraint_list_);
				for (k = 0; k < tree_dof; k++) {
					pM[base + k][base + i] += par_col[k];
				}
			}
			DELARY(par_col);      DELARY(vec_list);
		}
		DELARY(vecTemp1);
		int len = num_jforce();
		for (i = 0; i < len; i++) {
			int row = joint_force_list_[i]->row();
			for (j = 0; j < joint_force_list_[i]->get_partial_size(0); j++) {
				int col = joint_force_list_[i]->get_partial_col(0, j);
				double tempd = joint_force_list_[i]->get_partial(0, j);
				pM[base + row][base + col] += tempd;
			}
		}
	}
	return pM;
}
double* od_systemMechanism::evaluateRhs(double *pRhs) {
	int i, index, i_index, j_index;
//	double _m;
	od_loop* pl;
	//for (i = 0; i < nsystem; i++) pRhs = _subSys[i]->evaluateRhs(pRhs, _alphaonly);
	od_body *pBi, *pBj;
	od_object::Analysis_Type _type = get_analysis_type();
	int nbody = num_body();
	int nbodyx2 = nbody + nbody;
	Vec3 *pV = 0;
	double *pd = 0;
	int tree_dof = tree_dofs();
	if (_type == od_object::DYNAMIC || _type == od_object::STATIC || _type == od_object::ACC_FORCE) {
		for (i = 0; i < num_force(); i++)
			force_list_[i]->evaluate_rhs();
		for (i = 0; i < num_jforce(); i++)
			joint_force_list_[i]->evaluate_rhs();
	}
	if (_type == od_object::DYNAMIC) {
		for (i = 0; i < nbodyx2; i++) { (_tree_rhs + i)->init(); }
		//SumofForce: J*w_dot+w x Jw,  Mx_ddot
		for (i = 0; i < nbody; i++) {
			pBi = body_list_[i];
			*(_tree_rhs + i) -= pBi->J_Wdot();
			*(_tree_rhs + i) -= pBi->wxJw();
			*(_tree_rhs + i + nbody) -= pBi->Mx_ddot();
		}

		if (num_force()) {
			for (i = 0; i < num_force(); i++) {
				force_list_[i]->evaluate_rhs();
				pBi = force_list_[i]->get_imarker()->get_body();
				pBj = force_list_[i]->get_jmarker()->get_body();
				if (pBi->ground_body() == 0) {
					index = pBi->get_index();
					pd = force_list_[i]->itorque();
					_tree_rhs[index] += pd;
					pd = force_list_[i]->iforce();
					_tree_rhs[index + nbody] += pd;
				}
				if (pBj->ground_body() == 0) {
					index = pBj->get_index();
					pd = force_list_[i]->jtorque();
					_tree_rhs[index] += pd;
					pd = force_list_[i]->jforce();
					_tree_rhs[index + nbody] += pd;
				}
			}
		}
		//gravity
		//pvec3 += nbody;
		for (i = 0; i < nbody; i++) {
			//_tree_rhs[i+nbody] = 
			G_array.VxD(_tree_rhs[i + nbody].v, M_array[i]);
		}
		//from constraining forces
		for (i = 0; i < (int)loop_list.size(); i++) {
			pl = loop_list[i];
			pl->evaluate(1);
			i_index = pl->get_i_index();
			j_index = pl->get_j_index();
			_tree_rhs[i_index] += pl->moment(0);
			_tree_rhs[i_index + nbody] += pl->force(0);
			_tree_rhs[j_index] += pl->moment(1);
			_tree_rhs[j_index + nbody] += pl->force(1);
		}
		//	}
		multiplyMatT_Rotq_Vec(nbody, _tree_rhs, pRhs, constraint_list_);
		multiplyMatT_vec(nbody, _tree_rhs + nbody, _tree_rhs + nbody, constraint_list_);
		multiplyMatT_Traq_Vec(nbody, _tree_rhs + nbody, pRhs, constraint_list_);
		int len = num_jforce();
		for (i = 0; i < len; i++) {
			int row = joint_force_list_[i]->row();
			double tempd = joint_force_list_[i]->value();
			pRhs[row] += tempd;
		}
	}
	else if (_type == od_object::STATIC) {
		//Vec3* vecTemp1 = new Vec3[nbody];
		for (i = 0; i < nbodyx2; i++)   _tree_rhs[i].init();// (pvec3 + i)->init();
		if (num_force()) {
			//for (i = 0; i < num_force(); i++) force_list_[i]->evaluate_rhs();
			//for_each(force_list.begin(), force_list.end(), mem_fun(&od_force::evaluate_rhs));
			//T^t*SumOf_Force = rhs
			//SumOfForce
			//external force and torque
			for (i = 0; i < num_force(); i++) {
				int index;
				force_list_[i]->evaluate_rhs();
				pBi = force_list_[i]->get_imarker()->get_body();
				pBj = force_list_[i]->get_jmarker()->get_body();
				if (pBi->ground_body() == 0) {
					index = pBi->get_index();
					_tree_rhs[index] += force_list_[i]->itorque();
					_tree_rhs[index + nbody] += force_list_[i]->iforce();
				}
				if (pBj->ground_body() == 0) {
					index = pBj->get_index();
					_tree_rhs[index] += force_list_[i]->jtorque();
					_tree_rhs[index + nbody] += force_list_[i]->jforce();
				}
			}
		}
		//gravity
		//pvec3 += nbody;
		//Vec3 vecTemp;
		for (i = 0; i < nbody; i++) {
			G_array.VxD(_tree_rhs[nbody + i].v, M_array[i]);
		}
		//from constraining forces
		for (i = 0; i < (int)loop_list.size(); i++) {
			pl = loop_list[i];
			i_index = pl->get_i_index();
			j_index = pl->get_j_index();
			_tree_rhs[i_index] += pl->moment(0);
			_tree_rhs[i_index + nbody] += pl->force(0);
			_tree_rhs[j_index] += pl->moment(1);
			_tree_rhs[j_index + nbody] += pl->force(1);
		}
		multiplyMatT_Rotq_Vec(nbody, _tree_rhs, pRhs, constraint_list_);
		//multiplyMatT_vec(nbody, _tree_rhs+nbody, vecTemp1, constraint_list_);
		multiplyMatT_vec(nbody, _tree_rhs + nbody, _tree_rhs + nbody, constraint_list_);

		multiplyMatT_Traq_Vec(nbody, _tree_rhs + nbody, pRhs, constraint_list_);
		int len = num_jforce();
		for (i = 0; i < len; i++) {
			int row = joint_force_list_[i]->row();
			pRhs[row] += joint_force_list_[i]->value();
		}
		for (i = 0; i < tree_ndofs; i++) {
			if (dofmap_[i] != 1) {
				pRhs[i] += states[i];
			}
		}
		//DELARY(vecTemp1)
	}
	else if (_type == od_object::ACC_FORCE) {
		//m a_t = - m a_n + F + J^t\Lambda
		//m a_t + J^t\Lambda = -m a_n + F
		double fltTemp[3];
		//ZERO3(fltTemp);
		//-nonlinear_acc + F + Mg
		//Vec3* vecTemp1 = new Vec3[nbodyx2];
		//Vec3* vecTemp2 = new Vec3[nbody];
		for (i = 0; i < nbodyx2; i++) (_tree_rhs + i)->init();

		//nonlinear acc 
		for (i = 0; i < nbody; i++) {
			pBi = body_list_[i];
			*(_tree_rhs + i) -= pBi->J_Wdot();
			*(_tree_rhs + i) -= pBi->wxJw();
			*(_tree_rhs + i + nbody) -= pBi->Mx_ddot();


		}
		for (i = 0; i < nbody; i++) {
			//ZERO3(fltTemp);
			_tree_rhs[i + nbody] += G_array.VxD(fltTemp, M_array[i], 1); //mg
		}

		//from constraining forces
		for (i = 0; i < (int)loop_list.size(); i++) {
			pl = loop_list[i];
			i_index = pl->get_i_index();
			j_index = pl->get_j_index();
			_tree_rhs[i_index] += pl->moment(0);
			_tree_rhs[i_index + nbody] += pl->force(0);
			_tree_rhs[j_index] += pl->moment(1);
			_tree_rhs[j_index + nbody] += pl->force(1);
		}
		if (num_force()) {
			//for (i = 0; i < num_force(); i++) force_list_[i]->evaluate_rhs();
			//for_each(force_list.begin(), force_list.end(), mem_fun(&od_force::evaluate_rhs));
			//external torque should be applied here
			for (i = 0; i < num_force(); i++) {
				force_list_[i]->evaluate_rhs();
				int index;
				pBi = force_list_[i]->get_imarker()->get_body();
				pBj = force_list_[i]->get_jmarker()->get_body();
				if (pBi->ground_body() == 0) {
					index = pBi->get_index();
					_tree_rhs[index] += force_list_[i]->itorque();
					_tree_rhs[index + nbody] += force_list_[i]->iforce();
				}
				if (pBj->ground_body() == 0) {
					index = pBj->get_index();
					_tree_rhs[index] += force_list_[i]->jtorque();
					_tree_rhs[index + nbody] += force_list_[i]->jforce();
				}
			}
		}
		//mapping onto torque space
		multiplyMatT_Rotq_Vec(nbody, _tree_rhs, pRhs, constraint_list_);

		//mapping onto torque space
		multiplyMatT_vec(nbody, _tree_rhs + nbody, _tree_rhs + nbody, constraint_list_);
		multiplyMatT_Traq_Vec(nbody, _tree_rhs + nbody, pRhs, constraint_list_);
		int len = num_jforce();
		for (i = 0; i < len; i++) {
			int row = joint_force_list_[i]->row();
			pRhs[row] += joint_force_list_[i]->value();
		}
		for (i = 0; i < tree_ndofs; i++) {
			if (dofmap_[i] != 1) {
				pRhs[i] += ddstates[i];
			}
		}
	}
	else if (_type == od_object::INIT_DISP ||
		_type == od_object::INIT_VEL ||
		_type == od_object::INIT_ACC) {
		fill(pRhs, pRhs + tree_dof, 0.0);
	}
	return pRhs + tree_dof;
}
void od_systemGeneric::unset_evaluated() {
	unsigned i;
	unsigned _size = element_list.size();
	for (i = 0; i < _size; i++) (*(element_list_ + i))->unevaluate();
	for_each(aux_element_list.begin(), aux_element_list.end(), mem_fun(&od_element::unevaluate));
	for_each(expr_list.begin(), expr_list.end(), mem_fun(&Expression::unevaluate));
}
int od_system::displacement_ic() {
	if (Analysis_Flags.disp_ic == 0) {
		od_equation_disp_ic disp_ic(this);
		disp_ic.initialize();
		try {
			disp_ic.solve();
		}
		catch (equ_exception error) {
			string msg;
			msg = error.msg(msg);
			throw msg;
		}
		Analysis_Flags.disp_ic = 1;
	}
	return 0;
}
od_systemGeneric::~od_systemGeneric() {
	DELARY(constraint_list_);
	DELARY(element_list_);
	//	DELARY(motion_list_);
	DELARY(joint_force_list_);
	DELARY(M_array);
	DELARY(J_array);
	//DELARY(non_array);
	//DELARY(jwdot);
	//DELARY(mvdot);
	DELARY(_tree_rhs);/*
	DELARY(relevenceLevel1);
	DELARY(relevenceLevel2);
	DELARY(relevenceLevel3);
	*/
	DELARY(_subSys);
	for (int i = 0; i < (int)loop_list.size(); i++) delete loop_list[i];
	loop_list.resize(0);
}
/*
od_beams::od_beams(char* Name, int from_py) : od_systemGeneric(Name, from_py) { }

void od_beams::topology_analysis_level1() {
	int k;
	int size_ = num_body();
	for (k = 0; k < size_; k++) {
		body_list[k]->set_index(k);
		constraint_list[k]->set_index(k);
	}
	relevenceLevel1 = new int[size_*size_];
	fill(relevenceLevel1, relevenceLevel1 + size_ * size_, 0);
	for (k = 0; k < size_; k++) {
		relevenceLevel1[k*size_ + k] = k + 1;
	}
}
*/
od_system::od_system(char* Name, int from_py) : od_systemMechanism(Name, from_py) {
	_init();
}
void od_system::_init() {
	od_systemGeneric::init();
	_cputime = 0.0;
	_cputimeJac = 0.0;
	_cputimeRhs = 0.0;
	_cputimeInv = 0.0;
	virtual_joint_id = 0;
	virtual_body_id = 0;
	virtual_marker_id = 0;
	ndofs = 0;
	//_tree_rhs_si2 = 0;
	pKin = 0; pDyn = 0; pDynHHT = 0;
	string name__ = name() + ".msg";
	pMsgFile = new ofstream(name__.c_str(), ios_base::out);
	pOutFile = 0;
}

od_system::~od_system() {
	unsigned ii;
	od_element *pE;
	unsigned size_ = element_list.size();
	if (pMsgFile) pMsgFile->close();
	delete pMsgFile;
	if (pOutFile) { pOutFile->close(); delete pOutFile; }
	if (_from_py) {
		for (ii = 0; ii < size_; ii++) {
			pE = element_list[ii];
			if (pE) delete pE;
		}
	}
	size_ = aux_element_list.size();
	for (ii = 0; ii < size_; ii++) {
		pE = aux_element_list[ii];
		if (pE) delete pE;
	}
	size_ = expr_list.size();
	for (ii = 0; ii < size_; ii++) {
		delete expr_list[ii];
	}

	//DELARY(_tree_rhs_si2);
	if (pDyn) delete pDyn;
	DELARY(states);
	DELARY(dstates);
	DELARY(ddstates);
}
void od_systemMechanism::create_incidence(vector<int>& incid, int num_joint)
{
	//This function create the incidence matrix. It first finds all the 
	//unconnected bodies and defines the free joint between bodies and ground.
	//
	int i_index, j_index, i;
	od_constraint* pC;
	od_marker *pMi, *pMj;
	int num_body = body_list.size();
	//tag the connected bodies
	for_each(constraint_list.begin(), constraint_list.end(),
		mem_fun(&od_constraint::tag_attached_body));
	for_each(explicit_constraint_list.begin(), explicit_constraint_list.end(),
		mem_fun(&od_constraint::tag_attached_body));
	//add virtual joint(s)
	for (unsigned ii = 0; ii < body_list.size(); ii++) {
		if (body_list[ii]->is_tagged() == 0) {
			pC = new od_free_joint(--virtual_joint_id, 0);
			od_marker* im = new od_marker(--virtual_marker_id, (char*)"im_freejoint", 0);
			im->init();
			body_list[ii]->add_marker(im);
			pC->set_imarker(im);
			od_marker* jm = new od_marker(--virtual_marker_id, (char*)"jm_freejoint", 0);
			//      jm->equal(pGround->cm_marker());
			jm->init();
			pGround->add_marker(jm);
			pC->set_jmarker(jm);
			add_constraint(pC);
		}
	}
	for_each(body_list.begin(), body_list.end(), mem_fun(&od_body::untag));
	//fill in the incidence matrix
	//if (!num_joint)
	num_joint = constraint_list.size();
	for (i = 0; i < num_joint; i++) {
		i_index = j_index = 0;
		for (int j = 0; j < num_body; j++) incid.push_back(0);
		pC = constraint_list[i];
		pMi = pC->get_imarker();
		if (pMi) {
			i_index = pMi->get_body()->get_index();
		}
		else {
			i_index = -1;
		}
		pMj = pC->get_jmarker();
		if (pMj) {
			j_index = pMj->get_body()->get_index();
		}
		else {
			j_index = -1;
		}
		if (i_index >= 0) incid[i*num_body + i_index] = 1;
		if (j_index >= 0) incid[i*num_body + j_index] = -1;
	}
	nbody = body_list.size();
	njoint = constraint_list.size();
}

void od_systemMechanism::create_relevence(vector<int>& relev, int num_joint)
{
	if (num_joint == 0) num_joint = constraint_list.size();
	for (int i = 0; i < num_joint; i++) {
		for (int j = 0; j < num_joint; j++) relev.push_back(0);
		relev[i*num_joint + i] = 1;
	}
}
void od_systemMechanism::topology_analysis(vector_int& perm, vector_int& inci, vector_int& rele) {
	vector<int> _perm, _inci, _rele;
	int size, i;
	topology_analysis(_perm, _inci, _rele);
	size = _perm.size();
	perm.resize(size);
	for (i = 0; i < size; i++) perm[i] = _perm[i];
	size = _inci.size();
	inci.resize(size);
	for (i = 0; i < size; i++) inci[i] = _inci[i];
	size = _rele.size();
	rele.resize(size);
	for (i = 0; i < size; i++) rele[i] = _rele[i];
}
void od_systemMechanism::topology_analysis(vector<int>& perm, vector<int>& incidence, vector<int>& relevence/*, int njoint1*/)
{
	int *inci = 0;
	int *rele = 0;
	unsigned i;
	int partial = 0;
	obdyn_topology od_topo;

	nbody = body_list.size();
	njoint = constraint_list.size();
	create_incidence(incidence, partial);
	create_relevence(relevence, partial);
	perm.resize((njoint > nbody) ? njoint : nbody);
	for (i = 0; i < perm.size(); i++) perm[i] = i;
	print_inci(incidence, nbody, njoint);
	print_rele(relevence, nbody, njoint);
	od_topo.set_num_joint(njoint);
	od_topo.set_num_body(nbody);
	od_topo.set_incidence(incidence);
	od_topo.set_relevence(relevence);
	od_topo.process_topo();
	od_topo.get_incidence(incidence);
	od_topo.get_relevence(relevence);
	od_topo.get_permutation_vector(perm);
	print_inci(incidence, nbody, njoint);
	print_rele(relevence, nbody, njoint);
}

/*void od_system::connnect_floating_subsystems_or_reorder()
{
	int k, temp_int;
	od_constraint* pC;
	vector<int> perm, inci, rele;
	int has_floating_body = 0;
	topology_analysis(perm, inci, rele);
	for (unsigned i = 0; i < perm.size(); i++) {
		if (perm[i] < 0) {
			has_floating_body = 1;
			pC = new od_free_joint(--virtual_joint_id, 0);
			pC->set_imarker(body_list[i]->cm_marker());
			pC->set_jmarker(pGround->cm_marker());
			add_constraint(pC);
		}
	}

	if (has_floating_body != 0) {
		perm.clear(); inci.clear(); rele.clear();
		topology_analysis(perm, inci, rele);
	}

	//reorder the constraint elements
	vector<od_constraint*> temp_j;
	vector<od_body*> temp_b;
	vector<int> body_permu_vec;
	vector<int> joint_permu_vec;
	copy(constraint_list.begin(), constraint_list.end(), back_inserter(temp_j));
	for (k = 0; k < num_joint(); k++) {
		temp_int = perm[k];
		constraint_list[k] = temp_j[perm[k]];
		constraint_list[k]->set_index(k);
	}
	temp_j.clear();
	perm.clear(); inci.clear(); rele.clear();

	topology_analysis(perm, inci, rele);
	sort_branch(rele, body_permu_vec, joint_permu_vec);
	//reorder the body_list, make a copy of original body_list
	copy(body_list.begin(), body_list.end(), back_inserter(temp_b));
	for (k = 0; k < num_body(); k++) {
		temp_int = body_permu_vec[k];
		body_list[k] = temp_b[body_permu_vec[k]];
		body_list[k]->set_index(k);
	}
	temp_b.clear();
	//reorder the constraint elements
	copy(constraint_list.begin(), constraint_list.end(), back_inserter(temp_j));
	for (k = 0; k < num_body(); k++) {
		temp_int = joint_permu_vec[k];
		constraint_list[k] = temp_j[joint_permu_vec[k]];
		constraint_list[k]->set_index(k);
	}
	temp_j.clear();
}*/

void od_systemMechanism::break_body(int bidx, int cidx1) {
	od_marker *I, *J, *cm;
	od_body *pB, *pBdup;
	od_constraint *pC;
	char dup_name[80];
	char* p = dup_name;
	double m;
	Mat33 *Ia;
	double _Ia[9];
	p = strcpy(p, "dup_");
	pB = body_list[bidx];
	m = pB->get_M();
	Ia = pB->get_J();
	Ia->to_double(_Ia);
	for (int i = 0; i < 9; i++) _Ia[i] *= 0.5;
	(*Ia) = _Ia;
	pB->set_mass(m*0.5);

	pC = constraint_list[cidx1];
	I = pC->get_imarker(); J = pC->get_jmarker();
	p = strcat(p, pB->name().c_str());
	pBdup = new od_body(--virtual_body_id, dup_name, 0);
	pBdup->set_mass(0.5*m);
	Ia = pBdup->get_J();
	(*Ia) = _Ia;
	cm = new od_marker(--virtual_marker_id, (char*)"dup_cm", 0);
	cm->equal(pB->cm_marker());
	pBdup->add_cm_marker(cm);
	this->add_body(pBdup);
	this->add_marker(cm);
	if (pC->i_body_index() == bidx) {
		pBdup->add_marker(I);
		pB->remove_marker(I);
	}
	else {
		pBdup->add_marker(J);
		pB->remove_marker(J);
	}
	pC = new od_fixed_joint(--virtual_joint_id, 0);
	pC->set_imarker(pB->cm_marker());
	pC->set_jmarker(pBdup->cm_marker());
	add_explicit_constraint(pC);
}
string od_system::info() {
	ostringstream ost;
	pGround->info_(&ost);
	for_each(body_list.begin(), body_list.end(), bind2nd(mem_fun(&od_element::info_), &ost));
	for_each(constraint_list.begin(), constraint_list.end(), bind2nd(mem_fun(&od_element::info_), &ost));
	for_each(explicit_constraint_list.begin(), explicit_constraint_list.end(), bind2nd(mem_fun(&od_element::info_), &ost));
	return ost.str();
}
void od_systemMechanism::topology_analysis_level1() {
	int i, j, k;

	vector<int> relevence_level1, temp_inci, temp_perm, temp_vec;

	topology_analysis(temp_perm, temp_inci, relevence_level1);

	nbody = body_list.size();
	njoint = constraint_list.size();
	int num_joints = num_joint();
	for (i = 0; i < num_body(); i++) {
		for (j = 0; j < num_joints; j++) {
			k = relevence_level1[num_joints*i + j];
			temp_vec.push_back(k);
		}
	}
	if (relevenceLevel1) delete[] relevenceLevel1;
	relevenceLevel1 = new int[temp_vec.size()];
	for (i = 0; i < (int)temp_vec.size(); i++) relevenceLevel1[i] = temp_vec[i];
}
void od_systemMechanism::print_rele1(void) {
	cout << "rele" << endl;
	for (int i = 0; i < num_body(); i++) {
		for (int j = 0; j < num_joint(); j++)
			cout << setw(4) << relevenceLevel1[i*num_joint() + j];
		cout << endl;
	}
}
double od_system::getKPE() {
	int i;
	double energy = 0.0;
	double vec[3];
	double KE, PE;
	KE = 0.0; PE = 0.0;
	for (i = 0; i < nbody; i++) {
		double ke = body_list_[i]->getKE();
		KE += ke;
	}
	for (i = 0; i < nbody; i++) {
		double pe = G_array * (body_list_[i]->getPosition());
		PE += pe;
	}
	energy = KE - PE;
	for (i = 0; i < num_force(); i++) {
		energy += force_list_[i]->getPE();
	}
	return energy;
}
void od_systemMechanism::init_tree(double *_p, double *_v, double *_a, int dof_idx) {
	int i, j, temp_int, sign_ = 1;

	int dof_sidx = 0;
	od_joint *pC;
	od_marker *pM;
	od_body *pB;
	nbody = num_body();
	JMAT.no_instance();

	for (i = 0; i < nbody; i++) {
		pC = (od_joint*)constraint_list[i];
		for (j = 0; j < i; j++) {
			if (relevenceLevel1[i*num_joint() + j] != 0) {
				pC->set_prev_idx(j);
			}
		}
	}

	for (i = 0; i < nbody; i++) {
		for (j = 0; j < nbody; j++) {
			if (relevenceLevel1[j*nbody + i] != 0) {
				temp_int = j;
			}
		}
		constraint_list[i]->set_tail(temp_int);
		temp_int = -1;
		for (j = 0; j < nbody; j++) {
			if (relevenceLevel1[i*nbody + j] != 0) {
				temp_int = j;
				break;
			}
		}
		constraint_list[i]->set_head(temp_int);
	}

	for (i = 0; i < nbody; i++) {
		temp_int = constraint_list[i]->get_prev_idx();
		if (temp_int != -1) constraint_list[temp_int]->set_next_idx(i);
	}

	M_array = new double[nbody];
	//non_array = new double*[nbody];
	J_array = new Mat33*[nbody];
	_tree_rhs = new Vec3[nbody * 2];

	for (i = 0; i < nbody; i++) {
		pC = (od_joint*)constraint_list_[i];
		pM = pC->get_imarker();
		pB = (od_body*)body_list_[i];
		pB->set_from_marker(pM);
		pB = body_list_[i];
		pB->update(this, 0);
		pC->initialize();
		pB->initialize();
		pB->set_ang_rhs(_tree_rhs + i);
		pB->set_pos_rhs(_tree_rhs + i + num_body());
		M_array[i] = (pB->get_M());
		J_array[i] = (pB->get_J());
	}
	nforce = num_force();
	for (i = 0; i < num_force(); i++) {
		force_list_[i]->set_system(this);
		force_list_[i]->init();
	}
	for (i = 0; i < num_jforce(); i++) {
		joint_force_list_[i]->set_system(this);
		joint_force_list_[i]->init();
	}

	double* p = states;
	double* v = dstates;
	double* a = ddstates;


	for (i = 0; i < njoint; i++) {
		pC = (od_joint*)constraint_list[i];
		int c_dofs = pC->dofs();
		pC->set_temp_pva(p, v, a);
		p += c_dofs; v += c_dofs; a += c_dofs;
	}
	for (i = 0; i < explicit_constraint_list.size(); i++) {
		p += 6; v += 6; a += 6;
	}
	dof_sidx = this->blockSize();// this->TreeDofs();
	for (i = 0; i < nsystem; i++) {
		_subSys[i]->init_tree(p, v, a, dof_sidx);
		j = _subSys[i]->blockSize();
		p += j; v += j; a += j;
	}
	nbody = num_body();
	JR.create_jacobian(nbody, tree_ndofs, relevenceLevel3);
	JR_dot.create_jacobian(nbody, tree_ndofs, relevenceLevel3);
	JTL.create_jacobian(nbody, tree_ndofs, relevenceLevel3);
	JMAT.create_jacobian(nbody, num_body(), relevenceLevel1);
	JTG.create_jacobian(nbody, tree_ndofs, relevenceLevel2);
	JTL_dot.create_jacobian(nbody, tree_ndofs, relevenceLevel3);
	JTG_dot.create_jacobian(nbody, tree_ndofs, relevenceLevel2);
	parOmega_parq.create_jacobian(nbody, tree_ndofs, relevenceLevel3);
	parOmega_dot_parq.create_jacobian(nbody, tree_ndofs, relevenceLevel3);
	//parOmega_dot_parq_alpha.create_jacobian(nbody, tree_ndofs, relevenceLevel3);
	parOmega_dot_parq_dot.create_jacobian(nbody, tree_ndofs, relevenceLevel3);
	parVel_parq.create_jacobian(nbody, tree_ndofs, relevenceLevel3);
	parVel_parqG.create_jacobian(nbody, tree_ndofs, relevenceLevel2);
	parVel_dot_parq.create_jacobian(nbody, tree_ndofs, relevenceLevel3);
	//parVel_dot_parq_alpha.create_jacobian(nbody, tree_ndofs, relevenceLevel3);
	parVel_dot_parqG.create_jacobian(nbody, tree_ndofs, relevenceLevel2);
	//parVel_dot_parqG_alpha.create_jacobian(nbody, tree_ndofs, relevenceLevel2);
	parVel_dot_parq_dotG.create_jacobian(nbody, tree_ndofs, relevenceLevel2);
	int _size = element_list.size();
	if (_size) element_list_ = new od_element*[_size];
	for (i = 0; i < _size; i++) element_list_[i] = element_list[i];

	vector<od_constraint*>::iterator iter;
	for (iter = explicit_constraint_list.begin(); iter != explicit_constraint_list.end(); iter++) {
		od_loopr* ploop = new od_loopr(*iter, this);
		ploop->initialize();
		loop_list.push_back((od_loop*)ploop);
	}
	od_systemGeneric::get_states();
	unset_evaluated();
	Update();
	updatePartials();
}
/*void od_system::depthFirstOrdering() {
	std::vector<od_body*> tempb;
	std::vector<od_constraint*> tempj;
	for_each(constraint_list.begin(), constraint_list.end(), mem_fun(&od_constraint::untag));
	od_body *curObj = this->pGround;
	tempb.push_back(curObj);

	int bidx, sum, pbidx, cidx, cjdx, k;
	do {
		pbidx = curObj->get_index();
		sum = 0;
		curObj = 0;
		for (std::vector<od_constraint*>::iterator cit = this->constraint_list.begin(); cit != constraint_list.end(); ++cit) {
			if ((*cit)->is_tagged()) continue;
			sum++;
			cidx = (*cit)->i_body_index();
			cjdx = (*cit)->j_body_index();
			if (cjdx == pbidx) {
				(*cit)->tag();
				bidx = cidx;
				curObj = body_list[bidx];
			}
			else if (cidx == pbidx) {
				(*cit)->tag();
				bidx = cjdx;
				curObj = body_list[bidx];
			}
			if (curObj) {
				tempb.push_back(curObj);
				tempj.push_back(*cit);
				break;
			}
		}
		if (curObj == 0 && sum > 0) { //iterate to the root and find the body with joint
			for (std::vector<od_body*>::reverse_iterator rbit = tempb.rbegin(); rbit != tempb.rend(); ++rbit) {
				pbidx = (*rbit)->get_index();
				for (std::vector<od_constraint*>::iterator cit = this->constraint_list.begin(); cit != constraint_list.end(); ++cit) {
					if ((*cit)->is_tagged()) continue;
					if ((*cit)->i_body_index() == pbidx || (*cit)->j_body_index() == pbidx) {
						curObj = (*rbit);
						break;
					}
				}
				if (curObj) break;
			}
		}

	} while (sum > 0);
	body_list.clear();
	k = -1;
	for (std::vector<od_body*>::iterator it = tempb.begin(); it != tempb.end(); it++) {
		(*it)->set_index(k++);
	}
	copy(tempb.begin() + 1, tempb.end(), back_inserter(body_list));
	constraint_list.clear();
	k = 0;
	for (std::vector<od_constraint*>::iterator it = tempj.begin(); it != tempj.end(); it++) {
		(*it)->set_index(k++);
	}
	copy(tempj.begin(), tempj.end(), back_inserter(constraint_list));
}*/
void od_systemMechanism::DFSrecur(int idxb, std::vector<int> &bidx, std::vector<int> &cidx) {
	for (std::vector<od_constraint*>::iterator cit = this->constraint_list.begin(); cit != constraint_list.end(); ++cit) {
		if ((*cit)->is_tagged()) continue;
		int jdx = (*cit)->j_body_index();
		if (jdx == idxb) {
			int idx = (*cit)->i_body_index();
			cidx.push_back((*cit)->get_index());
			bidx.push_back(idx);
			(*cit)->tag();
			this->DFSrecur(idx, bidx, cidx);
		}
	}
}
int od_systemMechanism::graphD() {
	std::vector<int> bidx, cidx;
	std::vector<od_body*> tempb;
	std::vector<od_constraint*> tempj;
	copy(constraint_list.begin(), constraint_list.end(), back_inserter(tempj));
	copy(body_list.begin(), body_list.end(), back_inserter(tempb));

	for_each(constraint_list.begin(), constraint_list.end(), mem_fun(&od_constraint::untag));
	//for_each(body_list.begin(), body_list.end(), mem_fun(&od_body::untag));
	this->DFSrecur(-1, bidx, cidx);
	body_list.clear();
	constraint_list.clear();
	//body_list.push_back(this->pGround);
	//this->pGround->set_index(-1);
	int k = 0;
	for (std::vector<int>::iterator ii = bidx.begin(); ii != bidx.end(); ++ii) {
		tempb[*ii]->set_index(k); k++;
		body_list.push_back(tempb[*ii]);
	}
	k = 0;
	for (std::vector<int>::iterator ii = cidx.begin(); ii != cidx.end(); ++ii) {
		tempj[*ii]->set_index(k); k++;
		constraint_list.push_back(tempj[*ii]);
	}

	return 1;
}
int od_systemMechanism::graphW() {
	std::map<int, int> bc_map;
	std::vector<od_body*> last_layer, tempb;
	last_layer.push_back(this->ground());
	for_each(constraint_list.begin(), constraint_list.end(), mem_fun(&od_constraint::untag));
	for_each(body_list.begin(), body_list.end(), mem_fun(&od_body::untag));

	do {
		tempb.clear();
		for (std::vector<od_body*>::iterator pbit = last_layer.begin(); pbit != last_layer.end(); ++pbit) {
			int pbidx = (*pbit)->get_index();
			for (std::vector<od_constraint*>::iterator cit = constraint_list.begin(); cit != constraint_list.end(); ++cit) {
				if ((*cit)->is_tagged()) continue;
				int cidx = (*cit)->i_body_index();
				int cjdx = (*cit)->j_body_index();
				int bidx = -1;
				if (cjdx == pbidx) {
					bidx = cidx;
				}
				else if (cidx == pbidx) {
					bidx = cjdx;
					(*cit)->swap_markers();
				}
				if (bidx >= 0) {
					(*cit)->tag();
					(*pbit)->tag();
					tempb.push_back(body_list[bidx]);
					if (bc_map.find(bidx) == bc_map.end()) {
						bc_map[bidx] = (*cit)->get_index();
					}
					else {
						std::cout << "body " << bidx << endl;
						std::cout << "1st C " << bc_map[bidx] << endl;
						std::cout << "2nd C " << (*cit)->get_index() << endl;
						this->break_body(bidx, bc_map[bidx]);
						return 0;
					}
				}
			}
		}
		last_layer.clear();
		copy(tempb.begin(), tempb.end(), back_inserter(last_layer));
	} while (tempb.size() > 0);
	return 1;
}
int od_systemGeneric::initialize(int start_idx, int start_bidx) {
	size_t i;
	//size_t _size;
	//int temp;

	if (initialized) return  tree_ndofs + lambda_dof;
	start_index = start_idx;
	start_bindex = start_bidx;
	topology_analysis_level1();
	topology_analysis_level2();

	DELARY(body_list_);
	nbody = body_list.size();
	if (nbody) body_list_ = new od_body*[nbody];
	for (i = 0; i < nbody; i++) {
		body_list_[i] = body_list[i];
		body_list_[i]->set_index(start_bindex + i);
	}
	body_list.clear();

	njoint = constraint_list.size();
	DELARY(constraint_list_);
	if (njoint) constraint_list_ = new od_constraint*[njoint];
	else constraint_list_ = 0;
	for (i = 0; i < njoint; i++) {
		constraint_list_[i] = constraint_list[i];
		constraint_list_[i]->set_system(this);
		constraint_list_[i]->set_index(start_bindex + i);
		((od_joint*)constraint_list_[i])->dofMap(dofmap_);
	}
	nforce = force_list.size();
	if (nforce) force_list_ = new od_force*[nforce];
	for (i = 0; i < num_force(); i++) {
		force_list[i]->set_system(this);
		Expression* expr__ = force_list[i]->expr();
		if (expr__) {
			add_expression(expr__);
			expr__->set_system(this);
		}
		force_list_[i] = force_list[i];
	}
	force_list.clear();
	njforce = joint_force_list.size();
	if (njforce) joint_force_list_ = new od_jointF*[njforce];
	for (i = 0; i < njforce; i++) {
		joint_force_list_[i] = joint_force_list[i];
	}
	joint_force_list.clear();

	lambda_dof = 6 * explicit_constraint_list.size();

	initialized = 1;
	return tree_dofs() + lambda_dof;
}
void od_system::set_analysis_type(Analysis_Type _type) {
	Ana_Type = _type;
}
void od_system::set_jac_type(JAC_TYPE _type) {
	_jac_type = _type;
}
void od_system::set_dva_type(DVA_TYPE _type) {
	_dva_type = _type;
}
int od_system::initialize(DVA_TYPE dva, JAC_TYPE jac, Analysis_Type Ana) {
	set_dva_type(dva);
	set_jac_type(jac);
	set_analysis_type(Ana);
	if (initialized) {
		return 1;
	}
	od_systemMechanism::initialize();
	initialized = 1;
	return 1;
}
int od_systemMechanism::initialize() {
	int i, j, sindex, noloop = 1;
	do {
		noloop = graphW();
	} while (noloop == 0);
	this->graphD();
	//this->depthFirstOrdering();
	Tree_Ndofs = od_systemGeneric::initialize();
	for (i = 0; i < tree_ndofs; i++) mapdof.push_back(dofmap_[i]);
	for (i = tree_ndofs; i < blockSize(); i++) mapdof.push_back(LAMBDA);
	nsystem = subSys.size();
	sindex = this->num_body();
	if (nsystem) _subSys = new od_systemGeneric*[nsystem];
	for (i = 0; i < tree_dofs(); i++) mapdof.push_back(DOF);
	for (i = 0; i < lambda_dofs(); i++) mapdof.push_back(LAMBDA);
	for (i = 0; i < nsystem; i++) {
		_subSys[i] = subSys[i];
		Tree_Ndofs += _subSys[i]->initialize(Tree_Ndofs, sindex);
		sindex += _subSys[i]->num_body();
		for (j = 0; j < _subSys[i]->lambda_dofs(); j++) mapdof.push_back(LAMBDA);
		for (j = 0; j < _subSys[i]->tree_dofs(); j++) mapdof.push_back(DOF);
	}

	if (Tree_Ndofs) {
		DELARY(states) DELARY(dstates) DELARY(ddstates);
		states = new double[Tree_Ndofs];
		dstates = new double[Tree_Ndofs];
		ddstates = new double[Tree_Ndofs];
	}
	init_tree();
	_tree_rhs_si2 = new Vec3[num_body() * 2];
	//pGround->for_ground_body_only();

	return 1;
}
void od_systemMechanism::print_inci(vector<int>& inci, int nb, int nj) {
	int i, j, max_body_str_len = 0;
	int  max_joint_str_len = 0;
	od_constraint *pC;
	od_body* pB;
	for (i = 0; i < nbody; i++) {
		pB = body_list[i];
		if (strlen((pB->name()).c_str()) > (unsigned)max_body_str_len) {
			max_body_str_len = strlen((pB->name()).c_str()) + 1;
		}
	}
	for (i = 0; i < njoint; i++) {
		pC = constraint_list[i];
		if (strlen((pC->name()).c_str()) > (unsigned) max_joint_str_len) {
			max_joint_str_len = strlen((pC->name()).c_str()) + 1;
		}
	}
	//print title
	cout << "incidence" << endl;
	//print header -- the name of joint
	cout << setw(max_body_str_len) << " ";
	for (i = 0; i < num_body(); i++) {
		pB = body_list[i];
		cout << setw(max_body_str_len) << (pB->name()).c_str();
	}
	cout << endl;
	// print inci
	for (i = 0; i < nj; i++) {
		pC = constraint_list[i];
		cout << setw(max_joint_str_len) << (pC->name()).c_str();
		for (j = 0; j < nb; j++) {
			cout << setw(max_body_str_len) << inci[i*nb + j];
		}
		cout << endl;
	}
	cout << endl;
}
void od_systemMechanism::print_rele(vector<int>& rele, int nb, int nj) {
	int i, j;
	size_t max_body_str_len = 0;
	size_t  max_joint_str_len = 0;
	od_constraint *pC;
	od_body* pB;
	for (i = 0; i < num_body(); i++) {
		pB = body_list[i];
		if (strlen((pB->name()).c_str()) > (unsigned)max_body_str_len) {
			max_body_str_len = strlen((pB->name()).c_str()) + 1;
		}
	}
	for (i = 0; i < num_joint(); i++) {
		pC = constraint_list[i];
		if (strlen((pC->name()).c_str()) > (unsigned)max_joint_str_len) {
			max_joint_str_len = strlen((pC->name()).c_str()) + 1;
		}
	}
	//print title
	cout << "relevence" << endl;
	//print header -- the name of joint
	cout << setw(max_joint_str_len) << " ";
	for (i = 0; i < nj; i++) {
		pC = constraint_list[i];
		cout << setw(max_joint_str_len) << (pC->name()).c_str();
	}
	cout << endl;
	// print inci
	for (i = 0; i < nj; i++) {
		pC = constraint_list[i];
		cout << setw(max_joint_str_len) << (pC->name()).c_str();
		for (j = 0; j < nj; j++) {
			cout << setw(max_body_str_len) << rele[i*nj + j];
		}
		cout << endl;
	}
	cout << endl;
}
void od_system::update(int evaJ) {
	double _start = startRecord();
	Update(evaJ);
	stopRecord(_start);
}
int od_systemMechanism::Update(int evaJ) {
	int i;
	//for (i = 0; i < nsystem; i++) _subSys[i]->Update(flag);
	od_joint* pC;
	od_body *pB;
	//if (evaJ != 1) {
	for (i = 0; i < num_body(); i++) {
		pC = (od_joint*)(*(constraint_list_ + i));
		pB = body_list_[i];
		pC->update(this);
		pB->update(this);
		pC->updateReldistance(this);
	}
	//}

	if (evaJ == 1) {
		updateQ();
		calculate_JR();
		calculate_JT();
		calculate_JRdot();
		calculate_JTdot();
	}
	return 1;
}
int od_systemTrack2D::Update(int evaJ) {
	int i, nb;
	//double   // , temp[3];// , temp1[3];
	od_joint *pC;
	od_body *pB;
	nb = num_body();
	for (i = 0; i < nb; i++) {
		pC = (od_joint*)(*(constraint_list_ + i));
		pB = body_list_[i];
		pC->update(this);
		pB->update(this);
		pC->updateReldistance(this);
	}
	if (evaJ) {
		if (!JR) allocate(JR);
		if (!JR_dot) allocate(JR_dot);
		if (!JT) allocate(JT);
		if (!JT_dot) allocate(JT_dot);
		this->calculate_JR();
		this->calculate_JRdot();
		this->calculate_JT();
		this->calculate_JTdot();
		//this->createJ();
		if (!parOmega_parq) allocate(parOmega_parq);
		if (!parOmega_dot_parq) allocate(parOmega_dot_parq);
		if (!parOmega_dot_parq_dot) allocate(parOmega_dot_parq_dot);
		if (!parVel_parqG) allocate(parVel_parqG);
		if (!parVel_dot_parqG) allocate(parVel_dot_parqG);
		if (!parVel_dot_parq_dotG) allocate(parVel_dot_parq_dotG);
	}
	return 1;
}
Vec3* od_systemGeneric::getOmega(Vec3* vec) {
	od_marker* pM = 0;
	for (int i = 0; i < num_body(); i++) {
		pM = body_list_[i]->cm_marker();
		vec[i] = pM->get_omega(vec[i]);
	}
	return vec;
}
Vec3* od_systemGeneric::getOmegaDot(Vec3* vec) {
	od_marker* pM = 0;
	for (int i = 0; i < num_body(); i++) {
		pM = body_list_[i]->cm_marker();
		vec[i] = pM->get_omega_dot();
	}
	return vec;
}
Vec3* od_systemGeneric::getVel(Vec3* vec) {
	od_marker* pM = 0;
	for (int i = 0; i < num_body(); i++) {
		pM = body_list_[i]->cm_marker();
		vec[i] = pM->get_velocity();
	}
	return vec;
}
Vec3* od_systemGeneric::getVelDot(Vec3* vec) {
	od_marker* pM = 0;
	for (int i = 0; i < num_body(); i++) {
		pM = body_list_[i]->cm_marker();
		vec[i] = pM->get_acceleration(/*vec[i]*/);
	}
	return vec;
}
void od_system::toFile(ofstream* pf, string& __name) {
	string tempStr = __name;
	if (__name == "parOmega_parq") {
		parOmega_parq.toFile(pf, tempStr.c_str());
	}
	else if (__name == "parOmega_parq_dot") {
		parOmega_parq_dot.toFile(pf, tempStr.c_str());
	}
	else if (__name == "parOmega_dot_parq") {
		parOmega_dot_parq.toFile(pf, tempStr.c_str());
	}
	else if (__name == "parOmega_dot_parq_dot") {
		parOmega_dot_parq_dot.toFile(pf, tempStr.c_str());
	}
	else if (__name == "parVel_parq") {
		parVel_parqG.toFile(pf, tempStr.c_str());
	}
	else if (__name == "parVel_parq_dot") {
		parVel_parq_dotG.toFile(pf, tempStr.c_str());
	}
	else if (__name == "parVel_dot_parq") {
		parVel_dot_parqG.toFile(pf, tempStr.c_str());
	}
	else if (__name == "parVel_dot_parq_dot") {
		parVel_dot_parq_dotG.toFile(pf, tempStr.c_str());
	}
}






void od_system::parSi2_parq(double** pM) {
	int i, j, k, start, num_dofs;
	int nbody = num_body();
	od_joint* pC;
	int tree_dof = tree_dofs();
	for (i = 0; i < nbody; i++) {
		pC = (od_joint*)(*(constraint_list_ + i));
		start = pC->get_start_index();
		num_dofs = pC->dofs();
		//partTr_parq Fr
		multiplyParMatT_Rotq_Vec_I(_tree_rhs_si2, constraint_list_, i, tree_dof, nbody);
		for (j = 0; j < num_dofs; j++) {
			for (k = 0; k < tree_dof; k++) {
				pM[k][start + j] += (pC->get_ith_par_col(j, tree_dof))[k];
			}
		}
	}
	for (i = 0; i < nbody; i++) {
		pC = (od_joint*)(*(constraint_list_ + i));
		start = pC->get_start_index();
		num_dofs = pC->dofs();
		//partTt_parq Ft
		multiplyParMatT_Traq_Vec_I(_tree_rhs_si2 + nbody, constraint_list_, i, tree_dof, nbody);
		for (j = 0; j < num_dofs; j++) {
			for (k = 0; k < tree_dof; k++) {
				pM[k][start + j] += (pC->get_ith_par_col(j, tree_dof))[k];
			}
		}
	}
	for (i = 0; i < nbody; i++) {
		pC = (od_joint*)(*(constraint_list_ + i));
		start = pC->get_start_index();
		num_dofs = pC->dofs();
		multiplyParMatT_Traq_Vec_II(_tree_rhs_si2 + nbody, constraint_list_, i, tree_dof, nbody);
		for (j = 0; j < num_dofs; j++) {
			for (k = 0; k < tree_dof; k++) {
				pM[k][start + j] += (pC->get_ith_par_col(j, tree_dof))[k];
			}
		}
	}
}

void od_systemMechanism::numDif() {
	int i, j, idx, jdx;
	double delta =  1.0e-2;
	double invDelta = .5 / delta;
	Vec3 tli, tri, fli, fri;
	Vec3 tlj, trj, flj, frj;
	set_states();
	Update(1);
	updatePartials();
	ofstream f_, *pf;
	f_.open("Jac.txt");
	pf = &f_;
	Vec3* lOmega = new Vec3[tree_ndofs];
	Vec3* lOmegaDot = new Vec3[tree_ndofs];
	Vec3* lVel = new Vec3[tree_ndofs];
	Vec3* lAcc = new Vec3[tree_ndofs];
	Vec3* rOmega = new Vec3[tree_ndofs];
	Vec3* rOmegaDot = new Vec3[tree_ndofs];
	Vec3* rVel = new Vec3[tree_ndofs];
	Vec3* rAcc = new Vec3[tree_ndofs];
	Vec3 *pV;
	od_jacobian<Vec3> parVel_parq_;
	od_jacobian<Vec3> parVel_parq_dot_;
	od_jacobian<Vec3> parVel_dot_parq_;
	od_jacobian<Vec3> parVel_dot_parq_dot_;

	od_jacobian<Vec3> parOmega_parq_;
	od_jacobian<Vec3> parOmega_parq_dot_; //JR;
	od_jacobian<Vec3> parOmega_dot_parq_;
	od_jacobian<Vec3> parOmega_dot_parq_dot_; //parOmega_parq
	parVel_parq_.create_jacobian(num_body(), tree_ndofs, relevenceLevel2);
	parVel_parq_dot_.create_jacobian(num_body(), tree_ndofs, relevenceLevel2);
	parVel_dot_parq_.create_jacobian(num_body(), tree_ndofs, relevenceLevel2);
	parVel_dot_parq_dot_.create_jacobian(num_body(), tree_ndofs, relevenceLevel2);

	parOmega_parq_.create_jacobian(num_body(), tree_ndofs, relevenceLevel3);
	parOmega_parq_dot_.create_jacobian(num_body(), tree_ndofs, relevenceLevel3);
	parOmega_dot_parq_.create_jacobian(num_body(), tree_ndofs, relevenceLevel3);
	parOmega_dot_parq_dot_.create_jacobian(num_body(), tree_ndofs, relevenceLevel3);

	mat_d dTdq(num_body(), tree_ndofs);
	mat_d dTdqt(num_body(), tree_ndofs);
	mat_d dFdq(num_body(), tree_ndofs);
	mat_d dFdqt(num_body(), tree_ndofs);
	od_force *pF = 0;
	
	if (num_force()) {
		pF = force_list_[0];
	}
	get_states();
	for (j = 0; j < tree_ndofs; j++) {
		states[j] += delta; set_states();
		Update(1);  getVel(lVel); getVelDot(lAcc); getOmega(lOmega); getOmegaDot(lOmegaDot);
		if (pF) {
			for (int ii = 0; ii < num_force(); ii++) 
				force_list_[ii]->evaluate(0);
			tli = pF->itorque(); tlj = pF->jtorque(); fli = pF->iforce(); flj = pF->jforce();
			idx = pF->i_body_index(); jdx = pF->j_body_index();
		}
		states[j] -= 2 * delta; set_states();
		Update(1);  getVel(rVel); getVelDot(rAcc); getOmega(rOmega); getOmegaDot(rOmegaDot);
		if (pF) {
			for (int ii = 0; ii < num_force(); ii++) 
				force_list_[ii]->evaluate(0);
			tri = pF->itorque(); trj = pF->jtorque(); fri = pF->iforce(); frj = pF->jforce();
			tli -= tri; tli.multiplied_by(invDelta); tlj -= trj; tlj.multiplied_by(invDelta); 
			fli -= fri; fli.multiplied_by(invDelta); flj -= frj; flj.multiplied_by(invDelta);
			if (idx >= 0) {
				dTdq.fromd(idx, j, tli.v); dFdq.fromd(idx, j, fli.v);
			}
			if (jdx >= 0) {
				dTdq.fromd(jdx, j, tlj.v); dFdq.fromd(jdx, j, flj.v);
			}
		}
		states[j] += delta;
		for (i = 0; i < num_body(); i++) {
			lVel[i] -= rVel[i]; lVel[i].multiplied_by(invDelta);
			pV = parVel_parq_.element(i, j); if (pV) *pV = lVel[i];
			lAcc[i] -= rAcc[i]; lAcc[i].multiplied_by(invDelta);
			pV = parVel_dot_parq_.element(i, j); if (pV) *pV = lAcc[i];
			lOmega[i] -= rOmega[i]; lOmega[i].multiplied_by(invDelta);
			pV = parOmega_parq_.element(i, j); if (pV) *pV = lOmega[i];
			lOmegaDot[i] -= rOmegaDot[i]; lOmegaDot[i].multiplied_by(invDelta);
			pV = parOmega_dot_parq_.element(i, j); if (pV) *pV = lOmegaDot[i];
		}
		dstates[j] += delta; set_states();
		Update();  getVel(lVel); getVelDot(lAcc); getOmega(lOmega); getOmegaDot(lOmegaDot);
		if (pF) {
			for (int ii = 0; ii < num_force(); ii++)
				force_list_[ii]->evaluate(0);
			tli = pF->itorque(); tlj = pF->jtorque(); fli = pF->iforce(); flj = pF->jforce();
			idx = pF->i_body_index(); jdx = pF->j_body_index();
		}
		dstates[j] -= 2 * delta; set_states();
		Update();  getVel(rVel); getVelDot(rAcc); getOmega(rOmega); getOmegaDot(rOmegaDot);
		dstates[j] += delta;
		if (pF) {
			for (int ii = 0; ii < num_force(); ii++)
				force_list_[ii]->evaluate(0);
			tri = pF->itorque(); trj = pF->jtorque(); fri = pF->iforce(); frj = pF->jforce();
			tli -= tri; tli.multiplied_by(invDelta); tlj -= trj; tlj.multiplied_by(invDelta);
			fli -= fri; fli.multiplied_by(invDelta); flj -= frj; flj.multiplied_by(invDelta);
			if (idx >= 0) {
				dTdqt.fromd(idx, j, tli.v); dFdqt.fromd(idx, j, fli.v);
			}
			if (jdx >= 0) {
				dTdqt.fromd(jdx, j, tlj.v); dFdqt.fromd(jdx, j, flj.v);
			}
		}
		for (i = 0; i < num_body(); i++) {
			lVel[i] -= rVel[i]; lVel[i].multiplied_by(invDelta);
			pV = parVel_parq_dot_.element(i, j); if (pV) *pV = lVel[i];
			lAcc[i] -= rAcc[i]; lAcc[i].multiplied_by(invDelta);
			pV = parVel_dot_parq_dot_.element(i, j); if (pV) *pV = lAcc[i];
			lOmega[i] -= rOmega[i]; lOmega[i].multiplied_by(invDelta);
			pV = parOmega_parq_dot_.element(i, j); if (pV) *pV = lOmega[i];
			lOmegaDot[i] -= rOmegaDot[i]; lOmegaDot[i].multiplied_by(invDelta);
			pV = parOmega_dot_parq_dot_.element(i, j); if (pV) *pV = lOmegaDot[i];
		}
	}
	DELARY(lOmega); DELARY(lOmegaDot) DELARY(lVel) DELARY(lAcc);// DELARY(wVec);
	DELARY(rOmega) DELARY(rOmegaDot) DELARY(rVel) DELARY(rAcc);// DELARY(wVec)

	//To files
	string name_;
	if (compare_Jac(parOmega_parq_, parOmega_parq)) {
		name_ = "num_parOmega_parq"; parOmega_parq_.toFile(pf, name_.c_str());
		name_ = "parOmega_parq"; parOmega_parq.toFile(pf, name_.c_str()); *pf << "%%%%%%%%%%%%%%%%%%%%" << endl;
	}
	if (compare_Jac(parOmega_parq_dot_, JR)) {
		name_ = "num_parOmega_parq_dot"; parOmega_parq_dot_.toFile(pf, name_.c_str());
		name_ = "parOmega_parq_dot"; JR.toFile(pf, name_.c_str()); *pf << "%%%%%%%%%%%%%%%%%%%%" << endl;
	}
	if (compare_Jac(parOmega_dot_parq_, parOmega_dot_parq)) {
		name_ = "num_parOmega_dot_parq"; parOmega_dot_parq_.toFile(pf, name_.c_str());
		name_ = "parOmega_dot_parq"; parOmega_dot_parq.toFile(pf, name_.c_str()); *pf << "%%%%%%%%%%%%%%%%%%%%" << endl;
	}
	if (compare_Jac(parOmega_dot_parq_dot_, parOmega_dot_parq_dot)) {
		name_ = "num_parOmega_dot_parq_dot"; parOmega_dot_parq_dot_.toFile(pf, name_.c_str());
		name_ = "parOmega_dot_parq_dot"; parOmega_dot_parq_dot.toFile(pf, name_.c_str()); *pf << "%%%%%%%%%%%%%%%%%%%%" << endl;
	}
	if (compare_Jac(parVel_parq_, parVel_parqG)) {
		name_ = "num_parVel_parq"; parVel_parq_.toFile(pf, name_.c_str());
		name_ = "parVel_parq"; parVel_parqG.toFile(pf, name_.c_str()); *pf << "%%%%%%%%%%%%%%%%%%%%" << endl;
	}
	if (compare_Jac(parVel_parq_dot_, parVel_parq_dotG)) {
		name_ = "num_parVel_parq_dot"; parVel_parq_dot_.toFile(pf, name_.c_str());
		name_ = "parVel_parq_dot"; parVel_parq_dotG.toFile(pf, name_.c_str()); *pf << "%%%%%%%%%%%%%%%%%%%%" << endl;
	}
	if (compare_Jac(parVel_dot_parq_, parVel_dot_parqG)) {
		name_ = "num_parVel_dot_parq"; parVel_dot_parq_.toFile(pf, name_.c_str());
		name_ = "parVel_dot_parq"; parVel_dot_parqG.toFile(pf, name_.c_str()); *pf << "%%%%%%%%%%%%%%%%%%%%" << endl;
	}
	if (compare_Jac(parVel_dot_parq_dot_, parVel_dot_parq_dotG)) {
		name_ = "num_parVel_dot_parq_dot"; parVel_dot_parq_dot_.toFile(pf, name_.c_str());
		name_ = "parVel_dot_parq_dot"; parVel_dot_parq_dotG.toFile(pf, name_.c_str()); *pf << "%%%%%%%%%%%%%%%%%%%%" << endl;
	}
	if (pF) {
		name_ = "num_dTdq"; dTdq.toFile(pf, name_.c_str());
		name_ = "dTdq"; parTorqueparPVA[0].toFile(pf, name_.c_str()); *pf << "%%%%%%%%%%%%%%%%%%%%" << endl;
		name_ = "num_dFdq"; dFdq.toFile(pf, name_.c_str());
		name_ = "dFdq"; parForceparPVA[0].toFile(pf, name_.c_str()); *pf << "%%%%%%%%%%%%%%%%%%%%" << endl;
		name_ = "num_dTdqt"; dTdqt.toFile(pf, name_.c_str());
		name_ = "dTdqt"; parTorqueparPVA[1].toFile(pf, name_.c_str()); *pf << "%%%%%%%%%%%%%%%%%%%%" << endl;
		name_ = "num_dFdqt"; dFdqt.toFile(pf, name_.c_str());
		name_ = "dFdqt"; parForceparPVA[1].toFile(pf, name_.c_str()); *pf << "%%%%%%%%%%%%%%%%%%%%" << endl;
	}
	pf->close();
}

void od_systemMechanism::CreateTraM(double **pM, int base)
{
	int tree_dof = tree_dofs();
	if (tree_dof == 0) return;
	int i, j, idx, _len;
	int not_all_zero;
	int nbody = num_body();
	Vec3* vecTemp1 = new Vec3[nbody];
	Vec3* vecTemp2 = new Vec3[nbody];
	double* col = new double[tree_dof];
	Vec3** column = 0;
	for (i = 0; i < tree_dof; i++) {
		for (j = 0; j < nbody; j++) vecTemp1[j].init();
		fill(col, col + tree_dof, 0.0);
		column = JTG.column(i, &idx, &_len);
		not_all_zero = 0;
		for (j = idx; j < idx + _len; j++) {
			not_all_zero = 1;
			vecTemp1[j] = *column[j];
			vecTemp1[j].multiplied_by(M_array[j]);
		}
		if (not_all_zero) {
			multiplyMatT_vec(nbody, vecTemp1, vecTemp2, constraint_list_);
			multiplyMatT_Traq_Vec(nbody, vecTemp2, col, constraint_list_);
			for (j = 0; j < tree_dof; j++) pM[base + j][base + i] += col[j];
		}
	}
	DELARY(vecTemp1)  DELARY(vecTemp2)  DELARY(col)
}

void od_systemMechanism::CreateRotM(double **pM, int base)
{
	int tree_dof = tree_dofs();
	if (tree_dof == 0) return;
	int i, j, idx, _len;
	int not_all_zero;
	int nbody = num_body();
	Vec3* vecTemp1 = new Vec3[nbody];
	double* col = new double[tree_ndofs];
	Vec3** column = 0;
	for (i = 0; i < tree_dof; i++) {
		for (j = 0; j < nbody; j++) vecTemp1[j].init();
		fill(col, col + tree_dof, 0.0);
		column = JR.column(i, &idx, &_len);
		not_all_zero = 0;
		for (j = idx; j < idx + _len; j++) {
			mat_vec(*J_array[j], *(column[j]), vecTemp1[j]);
			not_all_zero = 1;
		}
		if (not_all_zero) {
			multiplyMatT_Rotq_Vec(nbody, vecTemp1, col, constraint_list_);
			for (j = 0; j < tree_dof; j++) pM[base + j][base + i] += col[j];
		}
	}
	DELARY(vecTemp1)  DELARY(col)
}
void od_systemGeneric::getM(double **pM, int base) {
	CreateRotM(pM, base);
	CreateTraM(pM, base);
}
void od_system::getM(double **pM, int base) {
	//for (int i = 0; i < nsystem; i++) {
	//	_subSys[i]->getM(pM, base);
	//	base += _subSys[i]->tree_dofs();
	//}
	od_systemGeneric::getM(pM, base);
	od_object::Analysis_Type _type = get_analysis_type();
	/*	if (_type == od_object::DYNAMIC) {
			int len = num_jforce();
			for (int i = 0; i < len; i++) {
				int row = joint_force_list_[i]->row();
				for (int j = 0; j < joint_force_list_[i]->get_partial_size(2); j++) {
					int col = joint_force_list[i]->get_partial_col(2, j);
					double tempd = joint_force_list_[i]->get_partial(2, j);
					pM[base + row][base + col] += tempd;
				}
			}
		}*/
}
double* od_system::evaluate_rhs(double *pRhs) {
	double _start = startRecord();
	evaluateRhs(pRhs);
	stopRecord(_start);
	return pRhs;
}
double** od_system::evaluate_Jac(double** pJac) {
	double _start = startRecord();
	evaluateJac(pJac);
	stopRecord(_start);
	return pJac;
}
int od_system::velocity_ic() {
	displacement_ic();
	if (Analysis_Flags.vel_ic == 0) {
		od_equation_vel_ic vel_ic(this);
		vel_ic.initialize();
		vel_ic.solve();
		Analysis_Flags.vel_ic = 1;
	}
	return 1;
}
int od_system::acceleration_and_force_ic() {
	velocity_ic();
	if (Analysis_Flags.acc_ic == 0) {
		od_equation_acc_ic acc_ic(this);
		acc_ic.initialize();
		acc_ic.solve();
	}
	Analysis_Flags.disp_ic = 0;
	Analysis_Flags.vel_ic = 0;
	return 1;
}

double od_system::cputime() {
#ifdef _WINDOWS
	HANDLE hProcess = GetCurrentProcess();
	FILETIME lpCreationTime, lpExitTime, kTime, uTime;
	struct {
		__int64 val;
	} ftKernel, ftUser;
	int isok = GetProcessTimes(hProcess, &lpCreationTime, &lpExitTime, &kTime, &uTime);
	memcpy(&ftKernel, &kTime, sizeof(FILETIME));
	memcpy(&ftUser, &uTime, sizeof(FILETIME));
	return (double)(ftKernel.val + ftUser.val);
#else
	return 0.0;
#endif

}

int od_system::dynamic_analysis_bdf(double end_time, double tol, int iter, double maxH, double minH, double _initStep, int debug) {
	int errorCode = 0;
	if (!pDyn) {
		displacement_ic();
		velocity_ic();
		acceleration_and_force_ic();
		pDyn = new od_equation_bdf_I(this);
		pDyn->initialize();
		pDyn->setMaxStepSize(maxH);
		pDyn->setMinStepSize(minH);
		pDyn->setInitStep(_initStep);
		pDyn->setLocalErrorTol(tol);
		pDyn->setMaxCorrectNum(iter);
	}
	try {
		double _start = cputime();
		pDyn->simulate(end_time, debug);
		double _delta = cputime() - _start;
		_cputime += _delta * 1.0e-7;
	}
	catch (equ_exception error) {
		errorCode = 1;
		string msg;
		msg = error.msg(msg);
		throw msg;
	}
	return errorCode;
}

int od_system::dynamic_analysis_hht(double end_time, double tol, int iter, double maxH, double minH, double _initStep, int debug) {
	int errorCode = 0;
	if (!pDynHHT) {
		displacement_ic();
		velocity_ic();
		acceleration_and_force_ic();
		pDynHHT = new od_equation_hhti3(this);
		pDynHHT->initialize();
		pDynHHT->setMaxStepSize(maxH);
		pDynHHT->setMinStepSize(minH);
		pDynHHT->setInitStep(_initStep);
		pDynHHT->setLocalErrorTol(tol);
		pDynHHT->setMaxCorrectNum(iter);
	}
	try {
		double _start = cputime();
		pDynHHT->simulate(end_time, debug);
		double _delta = cputime() - _start;
		_cputime += _delta * 1.0e-7;
	}
	catch (equ_exception error) {
		errorCode = 1;
		string msg;
		msg = error.msg(msg);
		throw msg;
	}
	return errorCode;
}
int od_system::kinematic_analysis(double end_time, int nums, double tol, int iters) {
	int old_iters;
	double old_tol;
	int errorCode = 0;
	old_tol = kinematics.disp_tolerance;
	old_iters = kinematics.disp_maxit;
	kinematics.disp_tolerance = tol;
	kinematics.disp_maxit = iters;
	if (!pKin) {
		displacement_ic();
		velocity_ic();
		acceleration_and_force_ic();
		pKin = new od_equation_kinematic(this, end_time, nums);
		pKin->initialize();
	}
	try {
		pKin->simulate(end_time);
	}
	catch (equ_exception error) {
		errorCode = 1;
		string msg;
		msg = error.msg(msg);
		kinematics.disp_tolerance = old_tol;
		kinematics.disp_maxit = old_iters;
		throw msg;
	}
	kinematics.disp_tolerance = old_tol;
	kinematics.disp_maxit = old_iters;
	return errorCode;
}

int od_system::static_analysis(int iter, double tol)
{
	int error_code;
	displacement_ic();
	od_equation_static stat_anal(this, iter, tol);
	stat_anal.initialize();
	try {
		error_code = stat_anal.solve();
	}
	catch (equ_exception error) {
		string msg;
		msg = error.msg(msg);
		throw msg;
	}
	return error_code;
}


//OdSystem::OdSystem(int i) {pS = new od_system(i);}
OdSystem::OdSystem(char *pn, int r) { pS = new od_system(pn, r); }
OdSystem::~OdSystem() {
	if (pS) delete pS;
	pS = 0;
}
void OdSystem::setGravity(double *val) { pS->setGravity(val); }
void OdSystem::setGravity(V3* t) {
	 setGravity(t->ptr());
 }
void OdSystem::setName(char *val) { pS->setName(val); }
void OdSystem::add_body(OdBody* pB) {
	od_body *pb = pB->core();
	pS->add_body(pb);
	for (int i = 0; i < pb->get_marker_number(); i++) {
		pS->add_marker(pb->get_ith_marker(i));
	}
}
void OdSystem::add_force(OdForce *pF) {
	od_force *pf = pF->core();
	pS->add_force(pf);
}
void OdSystem::add_joint_spdp(OdJointSPDP *pF) {
	od_joint_spdp* pf = pF->core();
	pS->add_joint_spdp(pf);
}
void OdSystem::add_joint_force(OdJointForce *pF) {
	od_joint_force* pf = pF->core();
	pS->add_joint_force(pf);
}

/*void OdSystem::add_subsystem(OdSubSystem* pSub) {
  od_systemGeneric *ps = pSub->core();
  pS->add_subsystem(ps);
}*/

void OdSystem::add_constraint(OdJoint* pJ) {
	od_joint *pj = pJ->core();
	int expl = pJ->explicitFixed();
	if (!expl) pS->add_constraint((od_constraint*)pj);
	else pS->add_explicit_constraint((od_constraint*)pj);
}

//void OdSystem::create_incidence(vector_int& inci) {
	//pS->create_incidence(inci);
//}
//void OdSystem::create_incidence() {
	//vector<int> inci;
	//pS->create_incidence(inci);
//}
//void OdSystem::create_relevence(vector_int& inci) {
	//pS->create_relevence(inci);
//}
/*void OdSystem::connnect_floating_subsystems() {
	//pS->connnect_floating_subsystems_or_reorder();
}
void OdSystem::break_loops() {
	//pS->break_loops();
}
void OdSystem::topology_analysis(vector_int& perm, vector_int& inci, vector_int& rele) {
	pS->topology_analysis(perm, inci, rele);
}
void OdSystem::topology_analysis() {
	vector<int> perm, inci, rele;
	pS->topology_analysis(perm, inci, rele);
}
void OdSystem::sort_branch(vector_int& rele, vector_int& b_perm, vector_int& j_perm) {
	//pS->sort_branch(rele, b_perm, j_perm);
}
void OdSystem::free_joints(vector_int& f_j) {
	//pS->free_joints(f_j);
}*/
char* OdSystem::info(char* msg) {
	msg = pS->info(msg);
	return msg;
}
int OdSystem::get_num_constraint() {
	int  num = pS->get_num_constraint();
	return num;
}
int OdSystem::get_num_body() {
	int num = pS->get_num_body();
	return num;
}
int OdSystem::displacement_ic() {
	return pS->displacement_ic();
}
int OdSystem::velocity_ic() {
	return pS->velocity_ic();
}
int OdSystem::acceleration_and_force_ic() {
	return pS->acceleration_and_force_ic();
}
int OdSystem::kinematic_analysis(double end, int nums, double tol, int iters) {
	return pS->kinematic_analysis(end, nums, tol, iters);
}
int OdSystem::static_analysis(int nums, double tol) {
	return pS->static_analysis(nums, tol);
}
int OdSystem::dynamic_analysis_bdf(double end_time, double tol, int iter, double maxH, double minH, double _initStep, int _debug) {
	return pS->dynamic_analysis_bdf(end_time, tol, iter, maxH, minH, _initStep, _debug);
}
int OdSystem::dynamic_analysis_hht(double end_time, double tol, int iter, double maxH, double minH, double _initStep, int _debug) {
	return pS->dynamic_analysis_hht(end_time, tol, iter, maxH, minH, _initStep, _debug);
}
double OdSystem::cpuTime() { return pS->cpuTime(); }
double OdSystem::jacTime() { return pS->jacTime(); }
double OdSystem::rhsTime() { return pS->rhsTime(); }
double OdSystem::solTime() { return pS->invTime(); }
void  OdSystem::numdif() { pS->numDif(); }
odSystemTrack2D::odSystemTrack2D(char *name_) { pS = new od_systemTrack2D(name_); }
odSystemTrack2D::~odSystemTrack2D() { if (pS) delete pS; pS = 0; }
void odSystemTrack2D::set_ref(OdMarker *pM) {
	od_marker* p = pM->core();
	pS->setRef(p);
}
void odSystemTrack2D::add_constraint(OdJoint* pJ) {
	od_joint *pj = pJ->core();
	//int expl = pJ->explicitFixed();
	pS->add_constraint((od_constraint*)pj);
	//else pS->add_explicit_constraint((od_constraint*)pj);
}
void odSystemTrack2D::add_force(OdForce *pF) {
	od_force *pf = pF->core();
	pS->add_force(pf);
}
void odSystemTrack2D::add_body(OdBody *pB) {
	od_body* pb = pB->core();
	pS->add_body(pb);
	for (int i = 0; i < pb->get_marker_number(); i++) {
		pS->add_marker(pb->get_ith_marker(i));
	}
}
void multiplyMatT_vec(const int num_rows, Vec3* const Input, Vec3* const Output,
	od_constraint**  C) {
	int i, j, ii, _size, temp_int;
	Vec3* Q = 0;
	od_element* pC;
	Q = new Vec3[num_rows];
	for (i = 0; i < num_rows; i++) {
		ii = num_rows - i - 1; //ZERO3(Output[ii].v);//Output[ii].init();
		pC = (od_element*)C[ii];
		//recursive calculation
		if (Output != Input) {
			//ZERO3(Output[ii].v);
			EQ3(Output[ii].v, Input[ii].v);
		}
		_size = (int)(pC->get_num_next());
		for (j = 0; j < _size; j++) {
			temp_int = pC->get_next(j);
			U_ADD3(Output[ii].v, Q[temp_int].v);
		}
		EQ3(Q[ii].v, Output[ii].v);
	}
	DELARY(Q);
	return;
}
void multiplyMatT_Rotq_Vec(int const num_rows, Vec3* const Input,
	double* const Output, od_constraint** C) {
	/*
	vi * ej   Q = Sum(vi)
*/
	int i, j, ii, temp_int, start, num_tra, num_rot, _size;
	double *zi, *pd, *pQQ;
	double* QQ = new double[3 * num_rows];
	for (i = 0; i < 3 * num_rows; i++) QQ[i] = 0.0;
	od_joint* pCj_;
	for (ii = 0; ii < num_rows; ii++) {
		i = num_rows - ii - 1;
		pCj_ = (od_joint*)C[i];
		pQQ = QQ + 3 * i;
		_size = (int)pCj_->get_num_next();
		for (j = 0; j < _size; j++) {
			temp_int = pCj_->get_next(j);
			pd = QQ + 3 * temp_int;
			U_ADD3(pQQ, pd);
		}
		start = pCj_->get_start_index(); num_tra = pCj_->num_tra(); num_rot = pCj_->num_rot();
		zi = pCj_->get_zi_global();
		//Q[i] += Input[i]; //accumulate the torque applied to ith body
		U_ADD3(pQQ, Input[i].v);

		for (j = 0; j < num_rot; j++) {
			temp_int = start + num_tra + j;
			pd = zi + (num_tra + j) * 3;
			//project the torque to the rotational axes of the joint
			Output[temp_int] += DOT_X(pQQ, pd);//pQQ[0]*pd[0]+pQQ[1]*pd[1]+pQQ[2]*pd[2];      
		}

	}
	DELARY(QQ);
	return;

}
void multiplyParMatT_Rotq_Vec_I(Vec3* const Input, od_constraint** C, int const ith, int const dofs, int num_rows) {
	/*
	vi*(ek x ej) : Q = Sum(vi)
	  */
	int i, j, k, temp_int, _size, start, start_index;
	int num_rotk, num_rotj, num_trak, num_traj;
	double *pd, *pQQ;
	double vecTemp[3];
	double *ek, *col[3], *zi;
	double *pcol, *pej, *pek;
	od_joint *pC, *pC1;
	temp_int = 3 * num_rows;
	double* QQ = new double[temp_int];
	for (i = 0; i < temp_int; i++) QQ[i] = 0.0;
	pC1 = (od_joint*)C[ith];
	int last = pC1->get_tail();
	num_rotk = pC1->num_rot();
	num_trak = pC1->num_tra();
	pcol = pC1->get_ith_par_col(0, dofs);
	fill(pcol, pcol + (num_trak + num_rotk)*dofs, 0.0);

	for (i = 0; i < num_rotk; i++) { col[i] = pC1->get_ith_par_col(i + num_trak, dofs); }
	ek = pC1->get_zi_global();
	for (i = last; i >= ith; i--) {
		pQQ = QQ + 3 * i;
		pC = (od_joint*)C[i];
		_size = (int)pC->get_num_next();
		for (j = 0; j < _size; j++) {
			temp_int = pC->get_next(j);
			pd = QQ + 3 * temp_int;
			U_ADD3(pQQ, pd);
		}

		start = pC->get_start_index();  num_traj = pC->num_tra(); num_rotj = pC->num_rot();
		zi = pC->get_zi_global();
		pd = Input[i].v;
		U_ADD3(pQQ, pd);

		for (j = 0; j < num_rotj; j++) {
			temp_int = start + num_traj + j; pej = zi + (num_traj + j) * 3;
			start_index = (pC != pC1) ? 0 : (j - 1);
			if (start_index < 0) continue;
			for (k = start_index; k < num_rotk; k++) {
				pek = ek + (num_trak + k) * 3;
				CROSS_X(pek, pej, vecTemp);
				col[k][temp_int] += DOT_X(pQQ, vecTemp);//pQQ[0]*vecTemp[0] + pQQ[1]*vecTemp[1] + pQQ[2]*vecTemp[2];
			}
		}
	}
	DELARY(QQ);
	return;
}
void multiplyParMatT_Traq_Vec_I(Vec3* const Input, od_constraint** C, int const ith, int const dofs, int num_rows) {
	/*Vi * [(ek x ej) x Ri] = (ek x ej) * (Ri x Vi) for rotational joint
	Vi * (ek x ej) for translational joint
	  */
	int i, j, k, num_rotk, num_trak, num_traj, num_rotj, _size, temp_int, start;
	double* ptrR; // = pC->get_Ri();
	int start_index;
	od_joint *pC, *pCk;
	double *ej, *pej, *pek, *ek, *pd, *pQQ;
	double vecTemp[3], RiVi[3], RiiVi[3];
	temp_int = 3 * num_rows; double* QQ = new double[temp_int]; for (i = 0; i < temp_int; i++) QQ[i] = 0.0;
	pCk = (od_joint*)C[ith];
	int last = pCk->get_tail();
	num_trak = pCk->num_tra(); num_rotk = pCk->num_rot();
	pej = pCk->get_ith_par_col(0, dofs);
	fill(pej, pej + (num_trak + num_rotk)*dofs, 0.0);
	ek = pCk->get_zi_global();
	for (i = last; i >= ith; i--) {
		pC = (od_joint*)C[i];
		//get the downstream comp.
		_size = (int)pC->get_num_next();
		pQQ = QQ + 3 * i;
		for (j = 0; j < _size; j++) {
			temp_int = pC->get_next(j);
			pd = QQ + 3 * temp_int;
			U_ADD3(pQQ, pd);
		}
		num_traj = pC->num_tra(); num_rotj = pC->num_rot();
		start = pC->get_start_index();
		ej = pC->get_zi_global();
		pd = Input[i].v;
		if (num_traj && i != ith) {
			for (j = 0; j < num_traj; j++) {
				temp_int = start + j;
				pej = ej + 3 * j;
				for (k = 0; k < num_rotk; k++) {
					double* col = pCk->get_ith_par_col(num_trak+k, dofs); 
					pek = ek + (num_trak + k) * 3;
					//ek * ej
					CROSS_X(pek, pej, vecTemp);
					col[temp_int] += DOT_X(pd, vecTemp);
				}
			}
		}
		ptrR = pC->get_Ri();
		CROSS_X(ptrR, pd, RiVi);
		start += num_traj;
		if (num_rotj) {
			ptrR = pC->get_Rii();
			CROSS_X(ptrR, pd, RiiVi);
			for (j = 0; j < num_rotj; j++) {
				temp_int = start + j;
				pej = ej + 3 * (j + num_traj);
				//start_index = (pC == pCk) ? (j + 1) : 0;
				start_index = 0;
				for (k = start_index; k < num_rotk; k++) {
					if (pC == pCk && j <= k) continue;
					pek = ek + 3 * (k + num_trak);
					//e_k*e_j
					CROSS_X(pek, pej, vecTemp);
					double* col = pCk->get_ith_par_col(num_trak + k, dofs);
					col[temp_int] += (RiiVi[0] + pQQ[0])*vecTemp[0];
					col[temp_int] += (RiiVi[1] + pQQ[1])*vecTemp[1];
					col[temp_int] += (RiiVi[2] + pQQ[2])*vecTemp[2];
				}
			}
		}
		U_ADD3(pQQ, RiVi);
	}
	DELARY(QQ);
	return;
}

void multiplyParMatT_Traq_Vec_II(Vec3* const Input, od_constraint** C, int const ith, int const dofs, int num_rows) {
	/*Vi * [ej x (ek x Ri)] = (ej) * [(ek x Ri) x Vi] for rotational joint
	  */
	int i, j, k, num_rotk, num_trak, num_traj, num_rotj, _size, temp_int, start;
	od_joint *pC, *pCk;
	double *ej, *pej, *pek, *ek, *pd, *pQQ, *pd2;
	double vecTemp[3], EkRiVi[3], EkRiiVi[3];
	double ParR_ParQkVi[3]; //for translation only
	temp_int = 3 * num_rows; 
	pCk = (od_joint*)C[ith];
	int last = pCk->get_tail(); int head = pCk->get_head();
	num_trak = pCk->num_tra(); num_rotk = pCk->num_rot();
    double** QQ = new double*[num_rotk]; 
    for(i=0; i<num_rotk;i++) {QQ[i]=new double[temp_int]; fill(QQ[i], QQ[i]+temp_int, 0.0);}
	pej = pCk->get_ith_par_col(0, dofs);
	fill(pej, pej + (num_trak + num_rotk)*dofs, 0.0);
	ek = pCk->get_zi_global();
	pd2 = Input[ith].v;
	for (k = 0; k < num_trak; k++) {
		pek = ek + 3 * k;
		CROSS_X(pek, pd2, ParR_ParQkVi);
		double* col = pCk->get_ith_par_col(k, dofs);
		for (i = head; i < ith; i++) {
			pC = (od_joint*)C[i]; num_rotj = pC->num_rot(); num_traj = pC->num_tra();
			ej = pC->get_zi_global();
			start = pC->get_start_index();
			for (j = 0; j < num_rotj; j++) {
				temp_int = start + num_traj + j;
				pej = ej + (num_traj + j) * 3;
				col[temp_int] += DOT_X(ParR_ParQkVi, pej);
			}
		}
	}
	for (k = 0; k < num_rotk; k++) {
		pek = ek + 3 * (k + num_trak);
		double* col = pCk->get_ith_par_col(num_trak + k, dofs);
		for (i = last; i >= ith; i--) {
			pQQ = QQ[k] + 3 * i;
			pC = (od_joint*)C[i];
			ej = pC->get_zi_global();
			//get the downstream comp.
			_size = (int)pC->get_num_next();
			for (j = 0; j < _size; j++) {
				temp_int = pC->get_next(j);
				pd = QQ[k] + 3 * temp_int;
				U_ADD3(pQQ, pd);
			}
			num_traj = pC->num_tra(); num_rotj = pC->num_rot(); ZERO3(EkRiiVi); ZERO3(EkRiiVi);
			if (num_rotj) {
				start = pC->get_start_index();
				//EkRiVi = (e_k x Ri) x Vi
				pd2 = pC->get_Ri(); CROSS_X(pek, pd2, vecTemp);
				pd2 = Input[i].v; CROSS_X(vecTemp, pd2, EkRiVi);
				//EkRiiVi = (e_k x Rii) x Vi
				pd2 = pC->get_Rii(); CROSS_X(pek, pd2, vecTemp);
				pd2 = Input[i].v; CROSS_X(vecTemp, pd2, EkRiiVi);
				ADD3(EkRiiVi, pQQ, vecTemp);
				start += num_traj;
				ej = pC->get_zi_global();
				for (j = 0; j < num_rotj; j++) {
					temp_int = start + j; pej = ej + 3 * (j + num_traj);
					col[temp_int] += DOT_X(vecTemp, pej);
				}
			}
			if (i == ith) {
				U_ADD3(pQQ, EkRiiVi);
			}
			else {
				U_ADD3(pQQ, EkRiVi);
			}
		}
		pQQ = QQ[k] + 3 * ith;
		int end_idx;
		for (i = head; i < ith; i++) {
			pC = (od_joint*)C[i]; num_rotj = pC->num_rot(); ej = pC->get_zi_global();
			start = pC->get_start_index();
			if (i == ith) end_idx = k;
			else end_idx = num_rotj;
			num_traj = pC->num_tra();
			for (j = 0; j < end_idx; j++) {
				temp_int = start + num_traj + j; pej = ej + 3 * (j + num_traj);
				col[temp_int] += DOT_X(pQQ, pej);
			}
		}
	}
    for(i=0; i<num_rotk;i++) DELARY(QQ[i]);
	DELARY(QQ);
	return;
}

void multiplyMatT_Traq_Vec(int const num_rows, Vec3* const Input, double* const Output,
	od_constraint** C)
{
	// RiVi --- Ri \cross Veci    
	//RiiVi --- Rii \cross Veci
	int i, j, ii, temp_int, start, num_tra, num_rot, _size;
	double *zi;
	double *pd, *pQQ, *pd2, *pd3;
	od_joint* pC;
	double RiVi[3], RiiVi[3];
	temp_int = 3 * num_rows; double* QQ = new double[temp_int]; for (i = 0; i < temp_int; i++) QQ[i] = 0.0;
	for (ii = 0; ii < num_rows; ii++) {
		i = num_rows - ii - 1;
		pQQ = QQ + 3 * i;
		pd3 = Input[i].v;
		pC = (od_joint*)C[i];
		//get the downstream comp.
		_size = (int)pC->get_num_next();
		for (j = 0; j < _size; j++) {
			temp_int = pC->get_next(j);
			pd = QQ + 3 * temp_int;
			U_ADD3(pQQ, pd);
		}

		start = pC->get_start_index();
		num_tra = pC->num_tra();
		zi = pC->get_zi_global();
		if (num_tra) {
			for (j = 0; j < num_tra; j++) {
				temp_int = start + j;
				pd = zi + j + j + j;
				//pd2 = Input[i].v;
				Output[temp_int] += DOT_X(pd3, pd);
			}
		}
		pd2 = pC->get_Ri();
		CROSS_X(pd2, pd3, RiVi);
		start += num_tra;
		num_rot = pC->num_rot();
		if (num_rot) {
			pd2 = pC->get_Rii();
			CROSS_X(pd2, pd3, RiiVi);
			U_ADD3(RiiVi, pQQ);
			for (j = 0; j < num_rot; j++) {
				temp_int = start + j;
				pd = zi + 3 * (j + num_tra);
				Output[temp_int] += DOT_X(RiiVi, pd);
			}
		}
		U_ADD3(pQQ, RiVi);
	}
	DELARY(QQ);
	return;
}
void multiplyMat_vec(int const num_rows, Vec3* const Input, Vec3* const Output, od_constraint** C) {
	int i, j, prev_id;
	double *pd, *pQQ;
	int temp_int = 3 * num_rows; double* QQ = new double[temp_int]; for (i = 0; i < temp_int; i++) QQ[i] = 0.0;
	od_joint* pC;
	for (j = 0; j < num_rows; j++) {
		pC = (od_joint*)(*(C + j));
		pQQ = QQ + 3 * j;
		prev_id = ((od_element*)pC)->get_prev_idx();
		if (prev_id != -1) {
			pd = QQ + 3 * prev_id;
			EQ3(pQQ, pd);
		}
		pd = Input[j].v;
		U_ADD3(pQQ, pd);
		pd = Output[j].v;
		EQ3(pd, pQQ);
	}
	DELARY(QQ);
	return;
}
void multiplyParMatTraVec_q_I(int const num_rows, od_constraint** C, /*od_constraint* const pCk_*/int ith, int const type, int const v1a2)//, int keep)
{
	//This function is for par_ej/par_qk x Ri
	//the condition for this value to be nonz-zero is that the index of pCk
	//is between pC[i]->head() and pC[i]->tail()
	int i, k, num_rot, num_tra, numTra, numRot, start, tail, temp_int, prev_id;
	double  *zk, *zk_dot;
	double  *pzk, *pzk_dot;
	double *pd, *pd_dot, *pout;
	double *pQQ, *pQQ_dot;
	double *pRi, *pRii, *pRi_dot, *pRii_dot;
	double vecTemp[3], vecTemp1[3], vecTemp2[3];
	Vec3 *vecRef;
	od_joint *pCi = 0;
	temp_int = 3 * num_rows;
	zk_dot = pzk_dot = pQQ_dot = 0;
	od_joint *pCk = (od_joint*)(*(C + ith)); //(od_joint*)pCk_;
	start = pCk->get_index();
	tail = pCk->get_tail(); num_tra = pCk->num_tra(); num_rot = pCk->num_rot();
	double** QQ_dot = 0;
	double** QQ = new double*[num_rot];
	for (i = 0; i < num_rot; i++) { QQ[i] = new double[temp_int]; fill(QQ[i], QQ[i] + temp_int, 0.0); }
	if (type) {
		QQ_dot = new double*[num_rot];
		for (i = 0; i < num_rot; i++) { QQ_dot[i] = new double[temp_int]; fill(QQ_dot[i], QQ_dot[i] + temp_int, 0.0); }
	}
	zk = pCk->get_zi_global();
	if (type == 1) zk_dot = pCk->get_cross_zi();
	for (k = 0; k < num_rot; k++) {
		pzk = zk + 3 * (k + num_tra);
		if (type) pzk_dot = zk_dot + 3 * (k + num_tra);
		for (i = 0; i < num_rows * 3; i++) { QQ[k][i] = 0.0; if (type) QQ_dot[k][i] = 0.0; }
		for (i = start; i <= tail; i++) {
			pCi = (od_joint*)(*(C + i));
			prev_id = pCi->get_prev_idx();
			if (prev_id < ith && i>ith) continue;
			pQQ = QQ[k] + 3 * i; if (type) pQQ_dot = QQ_dot[k] + 3 * i;
			pRii = pCi->get_Rii();
			pRi = pCi->get_Ri();
			vecRef = pCk->getPartialVec3(i, k + num_tra);
			pout = vecRef->v;
			numTra = pCi->num_tra(); numRot = pCi->num_rot();
			if ((pCi != pCk) && numTra) { // for the same joint, rot dof does not affect tra dof
				if (type == 0) {
					//vecTemp is the sum of ej*qq
					pd = pCi->getQ(type, od_object::TRA_DOF, v1a2);
					// ek x SumEj*qq
					CROSS_X(pzk, pd, vecTemp1);
					U_ADD3(pout, vecTemp1);
				}
				else {
					//vecTemp is the sum of ej*qq; ek_dot*vecTemp
					//vectEmp2 is the sum of dot_ej*qq; ek*vectTemp2
					pd = pCi->getQ(0, od_object::TRA_DOF, v1a2);
					pd_dot = pCi->getQ(type, od_object::TRA_DOF, v1a2);
					CROSS_X(pzk, pd_dot, vecTemp1); CROSS_X(pzk_dot, pd, vecTemp2);
					U_ADD3(pout, vecTemp1); U_ADD3(pout, vecTemp2);
				}
			}
			if (prev_id != -1) {
				pd = QQ[k] + 3 * prev_id;
				EQ3(pQQ, pd);
				if (type) {
					pd_dot = QQ_dot[k] + 3 * prev_id;
					EQ3(pQQ_dot, pd_dot);
				}
			}
			int start_j = 0;
			//j is for rotatonal dof
			if (type == 0) { //(ek x ej) x r:   Q is sigma ej
				ZERO3(vecTemp);
				if (pCi == pCk) {
					start_j = k + 1; //qk affects e_(k+1) only
					pd = pCi->getQ(type, od_object::ROT_DOF, v1a2, start_j, vecTemp);
				}
				else {
					pd = pCi->getQ(type, od_object::ROT_DOF, v1a2);
				}
				TRI_CROSS_XN(pzk, pQQ, pRi, vecTemp1);
				U_ADD3(pout, vecTemp1);
				U_ADD3(pQQ, pd);
				TRI_CROSS_XN(pzk, pd, pRii, vecTemp1);
				U_ADD3(pout, vecTemp1);
			}
			else { //(dot_ek*ej)*r + (ek*dot_ej)*r + (ek*ej)*dot_r
				ZERO3(vecTemp); ZERO3(vecTemp2);
				double _vecTemp[3];
				if (pCi == pCk) {
					start_j = k + 1; //qk affects e_(k+1) only
					pd = pCi->getQ(0, od_object::ROT_DOF, v1a2, start_j, vecTemp);
					pd_dot = pCi->getQ(type, od_object::ROT_DOF, v1a2, start_j, vecTemp2);
				}
				else {
					pd = pCi->getQ(0, od_object::ROT_DOF, v1a2);
					pd_dot = pCi->getQ(type, od_object::ROT_DOF, v1a2);
				}
				/* (ek*ej)*dot_r and  (dot_ek*ej)*r */

				pRi_dot = pCi->get_Ri_dot();
				CROSS_X(pzk, pQQ, vecTemp1);
				CROSS_X(vecTemp1, pRi_dot, _vecTemp);
				U_ADD3(pout, _vecTemp);
				TRI_CROSS_XN(pzk_dot, pQQ, pRi, _vecTemp);
				U_ADD3(pout, _vecTemp);

				U_ADD3(pQQ, pd);
				CROSS_X(pzk, pd, vecTemp1);
				pRii_dot = pCi->get_Rii_dot();
				CROSS_X(vecTemp1, pRii_dot, _vecTemp);
				U_ADD3(pout, _vecTemp);
				TRI_CROSS_XN(pzk_dot, pd, pRii, _vecTemp);
				U_ADD3(pout, _vecTemp);

				/* (ek*dot_ej)*r*/
				TRI_CROSS_XN(pzk, pQQ_dot, pRi, _vecTemp);
				U_ADD3(pout, _vecTemp);

				U_ADD3(pQQ_dot, pd_dot);
				TRI_CROSS_XN(pzk, pd_dot, pRii, _vecTemp);
				U_ADD3(pout, _vecTemp);
			}
		}
	}
	for (i = 0; i < num_rot; i++) { DELARY(QQ[i]); }
	DELARY(QQ);
	if (QQ_dot) {
		for (i = 0; i < num_rot; i++) DELARY(QQ_dot[i]);
		DELARY(QQ_dot);
	}
}
void multiplyParMatTraVec_q_II(int const num_rows, od_constraint** C,
	int ith, int const type,
	int const v1a2)//, int const keep)
{
	//This function is for ej x par_Ri/par_qk
	//pCk only affects the joints starting from k
	int i, /*j,*/ k, num_rot, num_tra, numTra, numRot, start, head, tail, temp_int, prev_id;
	double *zk, *zk_dot=0;
	double *pzk, *pzk_dot, *pout, *pQQ, *pd, *pd_dot, *pQQ_dot, *pTemp;
	double vecTemp[3], vecTemp1[3], vecTemp2[3];
	Vec3 *vecRef;
	pQQ_dot = zk_dot = pzk_dot = pd_dot = 0;
	//Vec3* Q=0; //(num_rows); //Q is for Sigma ej
	//Vec3* Q_dot=0; //(num_rows); //Q_dot is for Sigma dot_ej
	temp_int = 3 * num_rows;
	double* QQ_dot = 0;
	double* QQ = new double[temp_int]; for (i = 0; i < temp_int; i++) QQ[i] = 0.0;
	if (type) {
		QQ_dot = new double[temp_int]; for (i = 0; i < temp_int; i++) QQ_dot[i] = 0.0;
	}
	od_joint *pCi = 0;
	od_joint *pCk = (od_joint*)(C[ith]);// (od_joint*)pCk_;
	start = pCk->get_index();
	tail = pCk->get_tail(); num_tra = pCk->num_tra(); num_rot = pCk->num_rot();
	head = pCk->get_head();
	//the translational dof in pCk has no affect to all the joints
	//The rotational dof in pCk has no affect on all the translational dofs
	zk = pCk->get_zi_global();
	zk_dot = pCk->get_cross_zi();
	for (k = 0; k < num_tra; k++) {
		pzk = zk + 3 * k;
		pzk_dot = zk_dot + 3 * k;
		ZERO3(vecTemp);
		vecRef = pCk->getPartialVec3(start, k);
		pout = vecRef->v;
		if (type == 0) {
			if (v1a2 == 1) {
				U_ADD3(pout, pzk_dot);
			}
		}
		else {
			ZERO3(vecTemp2);
			/*for (int ii = head; ii < start; ii++) {
				pCi = (od_joint*)C[ii];
				prev_id=pCi->get_prev_idx();
				if(prev_id<head && ii>head) continue;
				pCi = (od_joint*)C[ii];
				pd = pCi->getQ(0, od_object::ROT_DOF, v1a2);
				U_ADD3(vecTemp, pd);
				pd_dot = pCi->getQ(type, od_object::ROT_DOF, v1a2);
				U_ADD3(vecTemp2, pd_dot);
			}
			CROSS_X(vecTemp2, pzk, vecTemp1);
			U_ADD3(pout, vecTemp1);
			CROSS_X(vecTemp, pzk_dot, vecTemp1);
			U_ADD3(pout, vecTemp1);
			*/
			pd = pCk->get_jmarker()->get_omega_dot();
			CROSS_X(pd, pzk, vecTemp);
			U_ADD3(pout, vecTemp);
			pd = pCk->get_jmarker()->get_omega();
			CROSS_X(pd, pzk_dot, vecTemp);
			U_ADD3(pout, vecTemp);
		}
	}
	for (k = 0; k < num_rot; k++) {
		for (i = 0; i < num_rows * 3; i++) { QQ[i] = 0.0; if (type) QQ_dot[i] = 0.0; }
		for (i = start; i <= tail; i++) {
			pCi = (od_joint*)C[i];
			prev_id = pCi->get_prev_idx();
			if (prev_id < ith && i>ith) continue;
			vecRef = pCk->getPartialVec3(i, k + num_tra);
			pout = vecRef->v;

			pQQ = QQ + 3 * i;
			if (type) pQQ_dot = QQ_dot + 3 * i;
			numTra = pCi->num_tra(); numRot = pCi->num_rot();
			if (prev_id != -1) {
				pd = QQ + 3 * prev_id;
				EQ3(pQQ, pd);
				if (type) {
					pd = QQ_dot + 3 * prev_id;
					EQ3(pQQ_dot, pd);
				}
			}
			if (type == 0) {
				//ej x (ek x r)
				if (pCk == pCi) {
					ZERO3(vecTemp);
					for (int ii = head; ii <= start; ii++) {
						pd = ((od_joint*)C[ii])->getQ(type, od_object::ROT_DOF, v1a2);
						U_ADD3(vecTemp, pd);
					}
				}
				else {
					pd = pCi->getQ(type, od_object::ROT_DOF, v1a2);
					EQ3(vecTemp, pd);
				}
				pzk = zk + 3 * (num_tra + k); pTemp = pCi->get_Rii();
				TRI_CROSS_X(vecTemp, pzk, pTemp, vecTemp2);
				U_ADD3(pout, vecTemp2);

				pTemp = pCi->get_Ri();
				TRI_CROSS_X(pQQ, pzk, pTemp, vecTemp2);
				U_ADD3(pout, vecTemp2);
				U_ADD3(pQQ, vecTemp);
			}
			else { //dot_ej*(ek*R) + ej x (dot_ek*R + ek*dot_R)
				if (pCk == pCi) {
					ZERO3(vecTemp) ZERO3(vecTemp2);
					for (int ii = head; ii <= start; ii++) {
						pd = ((od_joint*)C[ii])->getQ(0, od_object::ROT_DOF, v1a2);
						pd_dot = ((od_joint*)C[ii])->getQ(type, od_object::ROT_DOF, v1a2);
						U_ADD3(vecTemp, pd); U_ADD3(vecTemp2, pd_dot);
					}
				}
				else {
					pd = pCi->getQ(0, od_object::ROT_DOF, v1a2);
					pd_dot = pCi->getQ(type, od_object::ROT_DOF, v1a2);
					EQ3(vecTemp, pd); EQ3(vecTemp2, pd_dot);
				}

				double _vecTemp[3];
				pzk_dot = zk_dot + 3 * (num_tra + k); pTemp = pCi->get_Rii();
				CROSS_X(pzk_dot, pTemp, vecTemp1); //dot_ek x Rii
				pzk = zk + 3 * (num_tra + k); pTemp = pCi->get_Rii_dot();
				CROSS_X(pzk, pTemp, _vecTemp); //ek x Rii_dot
				U_ADD3(vecTemp1, _vecTemp); CROSS_X(vecTemp, vecTemp1, _vecTemp);
				U_ADD3(pout, _vecTemp);
				pTemp = pCi->get_Ri();
				CROSS_X(pzk_dot, pTemp, vecTemp1); //dot_ek x Ri
				pTemp = pCi->get_Ri_dot();
				CROSS_X(pzk, pTemp, _vecTemp); //ek x Ri_dot
				U_ADD3(vecTemp1, _vecTemp); CROSS_X(pQQ, vecTemp1, _vecTemp);
				U_ADD3(pout, _vecTemp);
				/*dot_ej * (ek x R)*/
				pTemp = pCi->get_Rii();
				TRI_CROSS_X(vecTemp2, pzk, pTemp, _vecTemp);
				U_ADD3(pout, _vecTemp);
				pTemp = pCi->get_Ri();
				TRI_CROSS_X(pQQ_dot, pzk, pTemp, _vecTemp);
				//*vecRef += _vecTemp;
				U_ADD3(pout, _vecTemp); U_ADD3(pQQ, vecTemp); U_ADD3(pQQ_dot, vecTemp2);
			}
		}
	}
	DELARY(QQ); DELARY(QQ_dot);
}
void multiplyParMatRotVec_q(int const num_rows, od_constraint** C,
	int ith, int const type, int const v1a2)//, int const keep)
{
	//type: 0 for parJR/parq, 1 for parJR_dot/parq 
	int i, j, temp_int, prev_id, num_rot, num_tra, head, tail;
	int numRot, numTra, samejnt = 0;
	double *zk, *zk_dot;
	double* pdk, *pQQ, *pQQ_dot, *pd, *pd_dot, *pout;
	double* pdk_dot;
	double  vecTemp[3];
	double vecTemp1[3];
	Vec3  *vecRef;
	pQQ_dot = zk_dot =  pd_dot = pdk_dot= 0;
	//Vec3* Q=0;//(num_rows); //Q is for Sigma ej
	//Vec3* Q_dot=0;//(num_rows); //Q_dot is for Sigma dot_ej
	//od_joint* pCj = (od_joint*)pCj_;
	od_joint* pCj = (od_joint*)C[ith];
	num_tra = pCj->num_tra();
	num_rot = pCj->num_rot();
	temp_int = 3 * num_rows;
	double** QQ_dot = 0;
	double** QQ = new double*[num_rot];
	for (i = 0; i < num_rot; i++) {
		QQ[i] = new double[temp_int];
		fill(QQ[i], QQ[i] + temp_int, 0.0);
	}
	if (type) {
		QQ_dot = new double*[num_rot];
		for (i = 0; i < num_rot; i++) {
			QQ_dot[i] = new double[temp_int];
			fill(QQ_dot[i], QQ_dot[i] + temp_int, 0.0);
		}
	}
	od_joint* pCi = 0;
	head = pCj->get_index();
	tail = pCj->get_tail();
	zk = pCj->get_zi_global();
	if (type == 1) zk_dot = pCj->get_cross_zi();
	for (j = 0; j < num_rot; j++) {
		pdk = zk + (num_tra + j) * 3;
		if (type == 1) pdk_dot = zk_dot + (num_tra + j) * 3;
		for (i = 0; i < 3 * num_rows; i++) { QQ[j][i] = 0.0; if (type) QQ_dot[j][i] = 0.0; }
		for (i = head; i <= tail; i++) {
			pCi = (od_joint*)C[i];
			prev_id = pCi->get_prev_idx();
			if (prev_id < ith && i > ith) continue;
			pQQ = QQ[j] + 3 * i; if (type) pQQ_dot = QQ_dot[j] + 3 * i;
			if (pCi == pCj) samejnt = 1;
			else samejnt = 0;
			if (prev_id != -1) {
				pd = QQ[j] + 3 * prev_id;
				EQ3(pQQ, pd);
				if (type) {
					pd = QQ_dot[j] + 3 * prev_id;
					EQ3(pQQ_dot, pd);
				}
			}
			numRot = pCi->num_rot();
			numTra = pCi->num_tra();
			if (numRot) {
				int start_j = 0;
				if (samejnt) {
					start_j = j + 1;
					ZERO3(vecTemp);
					ZERO3(vecTemp1);
					pd = pCi->getQ(0, od_object::ROT_DOF, v1a2, start_j, vecTemp);
					if (type)
						pd_dot = pCi->getQ(type, od_object::ROT_DOF, v1a2, start_j, vecTemp1);
				}
				else {
					pd = pCi->getQ(0, od_object::ROT_DOF, v1a2);
					if (type)
						pd_dot = pCi->getQ(type, od_object::ROT_DOF, v1a2);
				}
				vecRef = pCj->getPartialVec3(i, j + num_tra);
				pout = vecRef->v;
				if (type == 0) {
					U_ADD3(pQQ, pd);
					CROSS_X(pdk, pQQ, vecTemp1);
					U_ADD3(pout, vecTemp1);
				}
				else {
					U_ADD3(pQQ, pd);
					U_ADD3(pQQ_dot, pd_dot);
					CROSS_X(pdk_dot, pQQ, vecTemp1);
					U_ADD3(pout, vecTemp1);
					CROSS_X(pdk, pQQ_dot, vecTemp1);
					U_ADD3(pout, vecTemp1);
				}
			}
		}
	}

	if (QQ) {
		for (i = 0; i < num_rot; i++) DELARY(QQ[i]);
		DELARY(QQ);
	}
	if (QQ_dot) {
		for (i = 0; i < num_rot; i++) DELARY(QQ_dot[i]);
		DELARY(QQ_dot);
	}
}
void multiplyMat_RotVec_q(int const num_rows, Vec3* const Output, od_constraint** C, int const type, int const v1a2)
{
	//type: 0 for JR, 1 for JRdot 
	int i, temp_int;
	double *pd, *pQQ, *pout;
	temp_int = 3 * num_rows;
	double* QQ = new double[temp_int]; for (i = 0; i < temp_int; i++) QQ[i] = 0.0;
	od_joint* pC;
	for (i = 0; i < num_rows; i++) {
		pC = (od_joint*)C[i];
		pQQ = QQ + 3 * i;
		pout = Output[i].v;
		temp_int = ((od_element*)pC)->get_prev_idx();
		if (temp_int != -1) {
			pd = QQ + 3 * temp_int;
			EQ3(pQQ, pd);
		}
		pd = pC->getQ(type, od_object::ROT_DOF, v1a2);
		U_ADD3(pQQ, pd);
		U_ADD3(pout, pQQ);
	}
	DELARY(QQ);
	return;
}
/*
void multiplyMat_TraVec_q(int const num_rows, Vec3* const Output, od_constraint** C, int const type, int const v1a2)
{
	//type: 0 for JTL, 1 for JTLdot 
	double _vecTemp[3];
	int i, temp_int, num_tra, num_rot;
	double *pd, *pQQ, *pd_d, *pQQ_d, *pout;
	double *pRi, *pRii, *pRi_dot, *pRii_dot;
	temp_int = 3 * num_rows;
	double* QQ_dot = 0;
	pd_d = 0;
	double* QQ = new double[temp_int]; for (i = 0; i < temp_int; i++) QQ[i] = 0.0;
	if (type) {
		QQ_dot = new double[temp_int]; for (i = 0; i < temp_int; i++) QQ_dot[i] = 0.0;
	}
	od_joint* pCi_;
	for (i = 0; i < num_rows; i++) {
		pCi_ = (od_joint*)C[i];
		pout = Output[i].v;
		pQQ = QQ + 3 * i;  if (type) pQQ_d = QQ_dot + 3 * i;
		temp_int = ((od_element*)pCi_)->get_prev_idx();
		if (temp_int != -1) {
			pd = QQ + 3 * temp_int; if (type) pd_d = QQ_dot + 3 * temp_int;
			EQ3(pQQ, pd);
			if (type == 1) EQ3(pQQ_d, pd_d); //Q_dot[i] = Q_dot[temp_int];
		}
		num_tra = pCi_->num_tra();
		num_rot = pCi_->num_rot();
		if (num_tra) {
			pd = pCi_->getQ(type, od_object::TRA_DOF, v1a2);
			U_ADD3(pout, pd);
		}
		if (num_rot) {
			pd = pCi_->getQ(0, od_object::ROT_DOF, v1a2);
			if (type == 1) {
				pd_d = pCi_->getQ(1, od_object::ROT_DOF, v1a2);
			}
		}
		if (type == 0) {
			pRi = pCi_->get_Ri();
			pRii = pCi_->get_Rii();
			CROSS_X(pQQ, pRi, _vecTemp);
			U_ADD3(pout, _vecTemp);
			U_ADD3(pQQ, pd);
			CROSS_X(pd, pRii, _vecTemp);
			U_ADD3(pout, _vecTemp);
		}
		else if (type == 1) {
			pRi = pCi_->get_Ri();
			pRii = pCi_->get_Rii();
			pRi_dot = pCi_->get_Ri_dot();
			pRii_dot = pCi_->get_Rii_dot();
			//e * dot_r 
			CROSS_X(pQQ, pRi_dot, _vecTemp);
			U_ADD3(pout, _vecTemp);
			U_ADD3(pQQ, pd);
			CROSS_X(pd, pRii_dot, _vecTemp);
			U_ADD3(pout, _vecTemp);
			//(omega * e) * r 
			CROSS_X(pQQ_d, pRi, _vecTemp);
			U_ADD3(pout, _vecTemp);
			if (pd_d) {
				U_ADD3(pQQ_d, pd_d);
				CROSS_X(pd_d, pRii, _vecTemp);
				U_ADD3(pout, _vecTemp);
			}
		}
	}
	DELARY(QQ); DELARY(QQ_dot);
	return;
}
*/
