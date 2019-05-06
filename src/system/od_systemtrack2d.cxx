
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
od_systemLocal::od_systemLocal(char* Name, od_marker* ref, int from_py) : od_systemGeneric(Name, from_py) {
	char dup_name[80], dup_jname[80];
	char* p = dup_name;
	od_marker*  cm;
	{//ctreate virtual body
		strcpy(p, Name);
		strcat(p, "_base");
		p = dup_jname;
		strcpy(p, Name);
		strcat(p, "_basej");
		refM = ref;
		base = new od_body(--virtual_body_id, dup_name, 0);
		cm = new od_marker(--virtual_marker_id, (char*)"dup_cm", 0);
		cm->equal(ref);
		base->add_cm_marker(cm);
		this->add_body(base);
	}
	{
		freeJ = new od_free_joint(--virtual_joint_id, dup_jname, 0);
		freeJ->set_imarker(cm);
		//freeJ->set_jmarker(ref);
		add_constraint(freeJ);
	}
	{
		p = dup_jname;
		strcpy(p, Name);
		strcat(p, "_fixed");
		fixed = new od_fixed_joint(--virtual_joint_id, dup_name, 1);
		fixed->set_imarker(cm);
		fixed->set_jmarker(ref);
		add_explicit_constraint(fixed);
		
	}
	JR = 0; JR_dot = 0; JT = 0; JT_dot = 0;
	parOmega_parq = 0; parOmega_dot_parq = 0; parOmega_dot_parq_dot = 0; // parOmega_parq_dot=JR;
	parVel_parqG = 0; parVel_dot_parqG = 0; parVel_dot_parq_dotG = 0; //parVel_parq_dotG; =JTG;
	M00 = 0;// new od_matrix_dense(6, 6);
	M01 = 0;// new od_matrix_dense(6, 6;)
	M10 = 0;
	pTriM = 0;  //new blockTri
	for (int i = 0; i < 10; i++) for (int j = 0; j < 9; j++) bits[i][j] = 0;
}
od_systemLocal::~od_systemLocal() {
	remove(JR); remove(JR_dot); remove(JT); remove(JT_dot);
	remove(parOmega_parq); remove(parOmega_dot_parq); remove(parOmega_dot_parq_dot);
	remove(parVel_parqG); remove(parVel_dot_parqG); remove(parVel_dot_parq_dotG);
	delete M00; delete M01; delete M10;
}
void od_systemLocal::setParent(od_systemGeneric* ps) {
		pSys = ps;
		ploop = new od_loopv((od_constraint*)fixed, this, pSys);
	}
void od_systemLocal::init_tree(double *_p, double *_v, double *_a, int dof_idx1) {
	int i;
	od_joint *pC;
	nbody = num_body();
	M00 = new od_matrix_dense(6, 6);
	M01 = new od_matrix_dense(6, (nbody - 1) * 3);
	M10 = new od_matrix_denseB((nbody - 1) * 3, 6);
	pTriM = new od_matrix_blockTriDiagonal (nbody - 1);

	constraint_list_[0]->set_prev_idx(-1);
	constraint_list_[0]->set_tail(nbody - 1);
	for (i = 1; i < nbody; i++) {
		constraint_list_[i]->set_prev_idx(0);
		constraint_list_[i]->set_head(0);
		constraint_list_[i]->set_next_idx(i);
		constraint_list_[i]->set_tail(i);
	}

	od_systemGeneric::init_tree_();

	double *p, *v, *a;
	p = states = _p;
	v = dstates = _v;
	a = ddstates = _a;
	for (i = 0; i < explicit_constraint_list.size(); i++) {
		p = p + 6; v = v + 6;
	}
	for (i = 0; i < njoint; i++) {
		pC = (od_joint*)constraint_list[i];
		int c_dofs = pC->dofs();

		pC->set_temp_pva(p, v, a);
		p += c_dofs; v += c_dofs; a += c_dofs;
	}
	//int element_num = (int)element_list.size();
	//if (element_num)element_list_ = new od_element*[element_num];
	//for (i = 0; i < element_num; i++) element_list_[i] = element_list[i];
	{
		if (!JR) allocate(JR);
		if (!JR_dot) allocate(JR_dot);
		if (!JT) allocate(JT);
		if (!JT_dot) allocate(JT_dot);
		if (!parOmega_parq) allocate(parOmega_parq);
		if (!parOmega_dot_parq) allocate(parOmega_dot_parq);
		if (!parOmega_dot_parq_dot) allocate(parOmega_dot_parq_dot);
		if (!parVel_parqG) allocate(parVel_parqG);
		if (!parVel_dot_parqG) allocate(parVel_dot_parqG);
		if (!parVel_dot_parq_dotG) allocate(parVel_dot_parq_dotG);
	}
	od_systemGeneric::get_states();
	unset_evaluated();
	Update();
	updatePartials();
}
void od_systemLocal::calculate_JR(int no_dot) {
	int i, nb;
	double  *z, *zi, *wxz, *wxzi;

	od_joint *pC, *pCi;
	nb = num_body();
	pC = (od_joint*)(*(constraint_list_));
	z = pC->get_zi_global();
	wxz = pC->get_cross_zi();
	
	bits[0][3] = 1; bits[0][4] = 1; bits[0][5] = 1; bits[0][8] = 1;
	for (i = 0; i < nb; i++) {
		if (no_dot) {
			JR[i][3] = z + 9; JR[i][4] = z + 12; JR[i][5] = z + 15; //rotation axes
		}
		else {
			JR_dot[i][3] = wxz + 9; JR_dot[i][4] = wxz + 12; JR_dot[i][5] = wxz + 15;
		}
		if (i == 0) continue;
		pCi = (od_joint*)(*(constraint_list_ + i));
		if (no_dot) {
			zi = pCi->get_zi_global();
			JR[i][8] = zi + 6;
		}
		else {
			wxzi = pCi->get_cross_zi();
			JR_dot[i][8] = wxzi + 6;
		}
	}
}
void od_systemLocal::calculate_JT(int no_dot) {
	int i, nb;
	double *z, *zi, *p, *r, *wxz, *wxzi, temp[3], *v;
	nb = num_body();
	od_joint *pC, *pCi;
	pC = (od_joint*)(*(constraint_list_));
	z = pC->get_zi_global();
	if (no_dot) {
		i = 0; JT[i][0] = z; JT[i][1] = z + 3; JT[i][2] = z + 6;
	}
	wxz = pC->get_cross_zi();
	fill(bits[1], bits[1] + 7, 1);
	for (i = 1; i < nb; i++) {
		pCi = (od_joint*)(*(constraint_list_ + i));
		r = pCi->get_Ri();  //from cm basebody to the cm of link
		if (no_dot) {
			JT[i][0] = z; JT[i][1] = z + 3; JT[i][2] = z + 6;
			p = z + 9; CROSS_X(p, r, temp);  JT[i][3] = temp;
			p = z + 12; CROSS_X(p, r, temp); JT[i][4] = temp;
			p = z + 15; CROSS_X(p, r, temp); JT[i][5] = temp;   //translation axes
		}
		else {
			v = pCi->get_Rii_dot();
			JT_dot[i][0] = wxz; JT_dot[i][1] = wxz + 3;  JT_dot[i][2] = wxz + 6;
			p = z + 9;
			CROSS_X(p, v, temp); //e4 x dot_R
			JT_dot[i][3] = temp;	
			p = z + 12;  //e5
			CROSS_X(p, v, temp); //e5 x dot_R
			JT_dot[i][4] = temp;
			p = wxz + 12;  //w4xe5   
			CROSS_X(p, r, temp); //(w4xe5)xR 
			JT_dot[i][4] += temp;
			p = z + 15;           //e6
			CROSS_X(p, v, temp); //e6 x dot_R
			JT_dot[i][5] = temp;
			p = wxz + 15;     //w5xe6
			CROSS_X(p, r, temp);  //(w5 x e6) x R
			JT_dot[i][5] += temp;
		}	
		if (no_dot) {
			zi = pCi->get_zi_global();
			JT[i][6] = zi;
			JT[i][7] = zi + 3;
		}
		else {
			wxzi = pCi->get_cross_zi();
			JT_dot[i][6] = wxzi;
			JT_dot[i][7] = wxzi + 3;
		}
	}
}
void od_systemLocal::updatePartials(int pos_only) {
	int nb = num_body();

	for (int i = 0; i < 2; i++) for (int j = 0; j < 9; j++) bits[i][j] = 0;
	updateQ();
	this->calculate_JR();
	this->calculate_JRdot();
	this->calculate_JT();
	this->calculate_JTdot();
	this->ParOmegaParq();
	this->parOmegaDotParq();
	this->parOmegaDotParqDot();
	this->parVelParq();
	this->parVelDotParq();
	this->parVelDotParqDot();
}
void od_systemLocal::ParOmegaParq() {
	int i, nb;
	Vec3* vecs;
	double *z, *pz, *pQ, *pQ9, *wxz;
	double  vecTemp[3], temp[3];
	od_joint *pC, *pCi;
	nb = num_body();
	pC = (od_joint*)(*(constraint_list_));
	z = pC->get_zi_global();
	wxz = pC->get_cross_zi();
	for (i = 1; i < nb; i++) {
		pCi = (od_joint*)(*(constraint_list_ + 1));
		vecs = parOmega_parq[i]; ZERO3(vecTemp);
		pQ = pC->getQ(0, od_object::ROT_DOF, 1, 1, vecTemp);  // sum(e_i v) i=5,6 == pQ
		pQ9 = pCi->getQ(0, od_object::ROT_DOF, 1);            // e_9 v  == pQ9
		U_ADD3(pQ, pQ9); pz = z + 9; CROSS_X(pz, pQ, temp);   //e_4 x (pQ+pQ9)
		vecs[3] = temp; ZERO3(vecTemp);

		pQ = pC->getQ(0, od_object::ROT_DOF, 1, 2, vecTemp); // pQ=e_6 v
		U_ADD3(pQ, pQ9); pz = z + 12; CROSS_X(pz, pQ, temp); //e_5 x (pQ+pQ9)
		vecs[4] = temp; ZERO3(vecTemp);

		pz = z + 15; CROSS_X(pz, pQ9, temp);
		vecs[5] = temp; ZERO3(vecTemp);   //e_6 x pQ9
	}
}
void od_systemLocal::parOmegaDotParq() {
	int i, nb;
	Vec3* vecs;
	double *z, *pz, *pQ, *pQ9, *wxz, *pwxz;
	double  vecTemp[3], temp[3];
	od_joint *pC, *pCi;
	nb = num_body();
	pC = (od_joint*)(*(constraint_list_));
	z = pC->get_zi_global();
	wxz = pC->get_cross_zi();
	ZERO3(vecTemp);
	for (i = 1; i < nb; i++) {
		pCi = (od_joint*)(*(constraint_list_ + 1));
		vecs = parOmega_dot_parq[i];
		{
			pQ = pC->getQ(0, od_object::ROT_DOF, 1, 1, vecTemp); //sum_{5,6} e_i dot_q_i
			pQ9 = pCi->getQ(0, od_object::ROT_DOF, 1);  // e_9 dot_q_9
			U_ADD3(pQ, pQ9); pwxz/*wxe_4*/ = wxz + 9; CROSS_X(pwxz, pQ, temp); //wxe_4 X (sum_{5 6 9} e q)
			vecs[3] = temp; ZERO3(vecTemp);
			pQ = pC->getQ(1, od_object::ROT_DOF, 1, 1, vecTemp);
			pQ9 = pCi->getQ(1, od_object::ROT_DOF, 1);
			U_ADD3(pQ, pQ9); pz = z + 9; CROSS_X(pz, pQ, temp); //e_4 x  sum(wxe dot_q)
			vecs[3] += temp; ZERO3(vecTemp);
			pQ = pC->getQ(0, od_object::ROT_DOF, 2, 1, vecTemp);
			pQ9 = pCi->getQ(0, od_object::ROT_DOF, 2);
			U_ADD3(pQ, pQ9); pz = z + 9; CROSS_X(pz, pQ, temp);  //e_4 X  sum(e ddot_q)
			vecs[3] += temp; ZERO3(vecTemp);
		}
		{
			pQ = pC->getQ(0, od_object::ROT_DOF, 1, 2, vecTemp); //sum_{6} e_i dot_q_i
			pQ9 = pCi->getQ(0, od_object::ROT_DOF, 1);  // e_9 dot_q_9
			U_ADD3(pQ, pQ9); pwxz/*wxe_4*/ = wxz + 12; CROSS_X(pwxz, pQ, temp); //wxe_5 X (sum_{ 6 9} e q)
			vecs[4] = temp; ZERO3(vecTemp);
			pQ = pC->getQ(1, od_object::ROT_DOF, 1, 2, vecTemp);
			pQ9 = pCi->getQ(1, od_object::ROT_DOF, 1);
			U_ADD3(pQ, pQ9); pz = z + 12; CROSS_X(pz, pQ, temp); //e_5 x  sum(wxe dot_q)
			vecs[4] += temp; ZERO3(vecTemp);
			pQ = pC->getQ(0, od_object::ROT_DOF, 2, 2, vecTemp);
			pQ9 = pCi->getQ(0, od_object::ROT_DOF, 2);
			U_ADD3(pQ, pQ9); pz = z + 12; CROSS_X(pz, pQ, temp);  //e_5 X  sum(e ddot_q)
			vecs[4] += temp; ZERO3(vecTemp);
		}
		{
			pQ9 = pCi->getQ(0, od_object::ROT_DOF, 1);  // e_9 dot_q_9
			pwxz/*wxe_6*/ = wxz + 15; CROSS_X(pwxz, pQ9, temp); //wxe_6 X (sum_{9} e q)
			vecs[5] = temp; ZERO3(vecTemp);

			pQ9 = pCi->getQ(1, od_object::ROT_DOF, 1);
			pz = z + 15; CROSS_X(pz, pQ9, temp); //e_6 x  sum(wxe dot_q)
			vecs[5] += temp; ZERO3(vecTemp);
			pQ9 = pCi->getQ(0, od_object::ROT_DOF, 2);
			pz = z + 15; CROSS_X(pz, pQ9, temp);  //e_6 X  sum(e ddot_q)
			vecs[5] += temp; ZERO3(vecTemp);
		}
	}
}
void od_systemLocal::parOmegaDotParqDot() {
	for (int i = 0; i < num_body(); i++) {
		for (int j = 0; j < 9; j++) {
			parOmega_dot_parq_dot[i][j] = parOmega_parq[i][j];
			parOmega_dot_parq_dot[i][j] += JR_dot[i][j];
		}
	}
}
void od_systemLocal::parVelParq() {
	int i, nb;
	Vec3* vecs;
	double *z, *pz, *pQ, *pQc, *pwxz, *r;
	double  vecTemp[3], temp[3];
	od_joint *pC, *pCi;
	r = pwxz = 0;
	nb = num_body();
	pC = (od_joint*)(*(constraint_list_));
	z = pC->get_zi_global();
	//wxz = pC->get_cross_zi();
	ZERO3(vecTemp);
	for (i = 1; i < nb; i++) {
		pCi = (od_joint*)(*(constraint_list_ + i));
		r = pCi->get_Rii();
		vecs = parVel_parqG[i];
		{
			pQc = pC->getQ(0, od_object::ROT_DOF, 1);
			pz = z + 9;
			TRI_CROSS_X(pQc, pz, r, temp); //Q x (e_4 x R)
			vecs[3] = temp;
			pQ = pC->getQ(0, od_object::ROT_DOF, 1, 1, vecTemp);
			TRI_CROSS_XN(r, pz, pQ, temp); ZERO3(vecTemp); vecs[3] += temp; //(e_4 x Q) x r
			pQ = pCi->getQ(0, od_object::TRA_DOF, 1);    //e_4x(e_7, e_8) q_dot
			CROSS_X(pz, pQ, temp); vecs[3] += temp;
		}
		{
			pz = z + 12;
			TRI_CROSS_X(pQc, pz, r, temp); //Q x (e_5 x R)
			vecs[4] = temp;
			pQ = pC->getQ(0, od_object::ROT_DOF, 1, 2, vecTemp);
			TRI_CROSS_XN(r, pz, pQ, temp); ZERO3(vecTemp); vecs[4] += temp; //(e_5 x Q) x r
			pQ = pCi->getQ(0, od_object::TRA_DOF, 1);    //e_5x(e_7, e_8) q_dot
			CROSS_X(pz, pQ, temp); vecs[4] += temp;
		}
		{
			pz = z + 15;
			TRI_CROSS_X(pQc, pz, r, temp); //Q x (e_6 x R)
			vecs[5] = temp;
			pQ = pCi->getQ(0, od_object::TRA_DOF, 1);//e_6x(e_7, e_8) q_dot
			CROSS_X(pz, pQ, temp); vecs[5] += temp;
		}
		/*pQ = pCi->getQ(0, od_object::TRA_DOF, 1, 0);
		ZERO3(vecTemp);
		for (j = 0; j < 3; j++) {
			vecTemp[0] += z[12 + j * 3];
			vecTemp[1] += z[12 + j * 3 + 1];
			vecTemp[1] += z[12 + j * 3 + 2];
		}
		CROSS_X(vecTemp, pQ, temp);
		vecs[6] = temp;
		pQ = pCi->getQ(0, od_object::TRA_DOF, 1, 1);
		CROSS_X(vecTemp, pQ, temp);
		vecs[7] = temp;
		CROSS_X(vecTemp, pwxz, temp); vecs[6] = temp;*/
	}
}
void od_systemLocal::parVelDotParq() {
	int i, nb;
	Vec3* vecs;
	double *z, *pz, *pQ, *wxz, *pwxz, *r, *v;
	double  vecTemp[3], temp[3];
	//od_body* pB;
	od_joint *pC, *pCi;
	r = v = 0;
	nb = num_body();
	pC = (od_joint*)(*(constraint_list_));
	//prb = base->getPosition();
	//pvb = base->getVel();
	z = pC->get_zi_global();
	wxz = pC->get_cross_zi();
	ZERO3(vecTemp);
	for (i = 1; i < nb; i++) {
		pCi = (od_joint*)(*(constraint_list_ + 1));
		//pB = body_list_[i]; pri = pB->getPosition(); pvi = pB->getVel();
		//SUB3(pri, prb, r);
		//SUB3(pvi, pvb, v);
		r = pCi->get_Rii();
		v = pCi->get_Rii_dot();
		{
			vecs = parVel_dot_parqG[i];
			pQ = pC->getQ(1, od_object::ROT_DOF, 1); pz = z + 19;
			TRI_CROSS_X(pQ, pz, r, temp);
			vecs[3] = temp;

			pQ = pC->getQ(0, od_object::ROT_DOF, 1); pwxz = wxz + 19;
			TRI_CROSS_X(pQ, pwxz, r, temp); vecs[3] += temp;
			TRI_CROSS_X(pQ, pz, v, temp); vecs[3] += temp;

			pQ = pC->getQ(0, od_object::ROT_DOF, 1, 1);
			TRI_CROSS_XN(r, pwxz, pQ, temp); vecs[3] += temp;

			pQ = pC->getQ(1, od_object::ROT_DOF, 1, 1);
			TRI_CROSS_XN(r, pz, pQ, temp); vecs[3] += temp;

			pQ = pC->getQ(0, od_object::ROT_DOF, 1, 1);
			TRI_CROSS_XN(v, pz, pQ, temp); vecs[3] += temp;

			pQ = pCi->getQ(0, od_object::TRA_DOF, 1);
			CROSS_X(pwxz, pQ, temp); vecs[3] += temp;

			pQ = pCi->getQ(1, od_object::TRA_DOF, 1);
			CROSS_X(pz, pQ, temp); vecs[3] += temp;
		}
		///////
		{
			ZERO3(vecTemp);
			pQ = pC->getQ(1, od_object::ROT_DOF, 1); pz = z + 12;
			TRI_CROSS_X(pQ, pz, r, temp); ZERO3(vecTemp);
			vecs[4] = temp;

			pQ = pC->getQ(0, od_object::ROT_DOF, 1); pwxz = wxz + 12;
			TRI_CROSS_X(pQ, pwxz, r, temp); vecs[4] += temp;
			TRI_CROSS_X(pQ, pz, v, temp); vecs[4] += temp; ZERO3(vecTemp);

			pQ = pC->getQ(0, od_object::ROT_DOF, 1, 2, vecTemp);
			TRI_CROSS_XN(r, pwxz, pQ, temp); vecs[4] += temp; ZERO3(vecTemp);

			pQ = pC->getQ(1, od_object::ROT_DOF, 1, 2, vecTemp);
			TRI_CROSS_XN(r, pz, pQ, temp); vecs[4] += temp; ZERO3(vecTemp);

			pQ = pC->getQ(0, od_object::ROT_DOF, 1, 2, vecTemp);
			TRI_CROSS_XN(v, pz, pQ, temp); vecs[4] += temp;

			pQ = pCi->getQ(0, od_object::TRA_DOF, 1);
			CROSS_X(pwxz, pQ, temp); vecs[4] += temp;

			pQ = pCi->getQ(1, od_object::TRA_DOF, 1);
			CROSS_X(pz, pQ, temp); vecs[4] += temp;
		}
		///////
		{
			ZERO3(vecTemp);
			pQ = pC->getQ(1, od_object::ROT_DOF, 1); pz = z + 15;
			TRI_CROSS_X(pQ, pz, r, temp); ZERO3(vecTemp);
			vecs[5] = temp;

			pQ = pC->getQ(0, od_object::ROT_DOF, 1); pwxz = wxz + 15;
			TRI_CROSS_X(pQ, pwxz, r, temp); vecs[5] += temp;
			TRI_CROSS_X(pQ, pz, v, temp); vecs[5] += temp;
		}
	}
}
void od_systemLocal::parVelDotParqDot() {
	for (int i = 0; i < num_body(); i++) {
		for (int j = 0; j < 9; j++) {
			parVel_dot_parq_dotG[i][j] = parVel_parqG[i][j];
			parVel_dot_parq_dotG[i][j] += JT_dot[i][j];
		}
	}
}
void od_systemLocal::CreateRotM(double **pM, int base) {
	Vec3 vecTemp;
	Vec3 *pv, *pvt;
	double *pd, *pdt, val;
	int i, j, k, offset;
	od_matrix *pD;
	pd = 0;
	for (i = 1; i < num_body(); i++) {
		//calculate the first 6x6 matrix
		for (j = 3; j < 6; j++) {
			pv = JR[i] + j;
			mat_vec(*J_array[j], *pv, vecTemp);
			pd = vecTemp.v;
			for (k = 0; k <= j; k++) {
				if (k <= j) {
					pvt = JR[i] + k; pdt = pvt->v;
					val = DOT_X(pdt, pd);
					M00->add(k, j, val);
				}
				if (k != j) M00->element(j, k, val);
			}
		}
	}
	for (i = 1; i < num_body(); i++) {
		//calculate B and C
		offset = 6 + 3 * (i - 1);
		pv = JR[i] + 8; mat_vec(*J_array[j], *pv, vecTemp);
		pd = vecTemp.v;
		for (j = 3; j < 6; j++) {
			pvt = JR[i] + j; pdt = pvt->v;
			val = DOT_X(pdt, pd);
			M01->add(j, offset + 8, val);
			M10->add(offset + 8, j, val);
		}
	}
	for (i = 1; i < num_body(); i++) {
		pv = JR[i] + 8; mat_vec(*J_array[j], *pv, vecTemp);
		//block tridiagonal
		pD = pTriM->D(i - 1);
		pdt = pv->v;
		val = DOT_X(pdt, pd);
		pD->add(2, 2, val);
	}
}
void od_systemLocal::CreateTraM(double **pM, int base) {
	Vec3 vecTemp;
	Vec3 *pv, *pvt, *pvi;
	double *pd, *pdt, val, m;
	int i, j, k, offset;
	od_matrix *pD;
	for (i = 1; i < num_body(); i++) {
		//calculate the first 6x6 matrix
		m = M_array[i];
		pvi = JT[i];
		for (j = 0; j < 6; j++) {
			pv = pvi + j;
			for (k = 0; k <= j; k++) {
				pvt = pvi + k;
				val = m * DOT_X(pv, pvt);
				M00->add(j, k, val);

			}
			if (j != k) M00->element(k, j, val);
		}
	}
	for (i = 1; i < num_body(); i++) {
		//calculate B and C
		offset = 6 + (i - 1) * 3;
		m = M_array[i];
		for (j = 6; j < 8; j++) {
			pv = JT[i] + j; pd = pv->v;
			for (k = 0; k < 6; k++) {
				pvt = JT[i] + k; pdt = pvt->v;
				val = m * DOT_X(pd, pdt);
				M01->add(k, offset + j, val);
				M10->add(offset + j, k, val);
			}
		}
	}
	for (i = 0; i < num_body(); i++) {
		m = M_array[i];
		pD = pTriM->D(i - 1);
		for (j = 0; j < 2; j++) {
			pv = JT[i] + 6 + j; pd = pv->v;
			for (k = 0; k <= j; k++) {
				pvt = JT[i] + 6 + k; pdt = pvt->v;
				val = m * DOT_X(pd, pdt);
				pD->add(j, k, val);
				if (j != k) pD->add(k, j, val);
			}
		}
	}
}
void od_systemLocal::Project2Q(double *rhs, int idx) {
	int i, ii;
	double *z, *r, *v, vec[3], *pv;
	od_joint *pCi, *pC;
	int nbody = num_body();
	Vec3 baseT, baseF;
	baseT = _tree_rhs;
	baseF = _tree_rhs + nbody;
	for (i = 1; i < nbody; i++) {
		pCi = (od_joint*)(*(constraint_list_ + i));
		z = pCi->get_zi_global();
		r = pCi->get_Rii();
		v = (_tree_rhs + nbody + i)->v;
		CROSS_X(r, v, vec);
		baseT += _tree_rhs + i;
		baseT += vec;
		baseF += v;
		ii = 6 + (i - 1) * 3;
		rhs[ii] = DOT_X(z, v);  pv = z + 3;
		rhs[ii + 1] = DOT_X(pv, v); pv = z + 6;
		rhs[ii + 2] = (*(_tree_rhs + nbody + i))*pv;
	}
	pC = (od_joint*)(*(constraint_list_));
	z = pC->get_zi_global();
	v = baseF.v;
	rhs[0] += DOT_X(z, v); pv = z + 3; rhs[1] += DOT_X(pv, v); pv = z + 6; rhs[2] += DOT_X(pv, v);
	v = baseT.v; pv = z + 9;
	rhs[3] += DOT_X(pv, v); pv = z + 3; rhs[4] += DOT_X(pv, v); pv = z + 6; rhs[5] += DOT_X(pv, v);
}
void od_systemLocal::parF_parq(double**) {
	od_joint *pCi, *pC;
	double Jac[9][9], *zi, *z, *pz, *pzi, vec[3], *pv, *pf, *r, vec1[3];
	int i;
	Vec3 Fi, Ti, F0, T0, F, T;
	int nbody = num_body();
	pC = (od_joint*)(*(constraint_list_));
	z = pC->get_zi_global();
	for (i = 0; i < 9; i++) fill(Jac[i], Jac[i] + 9, 0.0);
	for (i = 1; i < nbody; i++) {
		Fi = _tree_rhs + nbody + i; pv = Fi.v;
		Ti = _tree_rhs + i;
		pCi = (od_joint*)(*(constraint_list_ + i));
		r = pCi->get_Rii();
		zi = pCi->get_zi_global();
		for (int j = 0; j < 3; j++) {
			pzi = zi + j * 3;
			pv = (j > 1) ? Ti.v : Fi.v;
			for (int k = 3; k < 6; k++) {
				pz = z  + k;
				CROSS_X(pz, pzi, vec); //(pz x pzi)*Fi
				Jac[6 + j][k] += DOT_X(vec, pv);
			}
 		}
		CROSS_X(r, pv, vec);
		Ti += vec;
		pv = Ti.v;
		pf = Fi.v;
		for (int j = 0; j < 3; j++) {
			pz = z + 3 * j;
			for (int k = 3; k < 6; k++) {
				pzi = z + 3 * k;
				if (k < j) {
					CROSS_X(pzi, pz, vec);   //(pz x pzi)*Ti
					Jac[j + 3][k] += DOT_X(vec, pv);
				}
				CROSS_X(pzi, r, vec); //pz*((pzi x r) x Fi)
				CROSS_X(vec, pf, vec1);
				Jac[j + 3][k] += DOT_X(pz, vec1);
			}
		}
		pzi = z;
		CROSS_X(pzi, pf, vec);   //pz*(pzixFi)
		pz = z + 9;  Jac[3][6] += DOT_X(pz, vec);
		pz = z + 12; Jac[4][6] += DOT_X(pz, vec);
		pz = z + 15; Jac[5][6] += DOT_X(pz, vec);
		pzi = z+3;
		CROSS_X(pzi, pf, vec);   //pz*(pzixFi)
		pz = z + 9;  Jac[3][7] += DOT_X(pz, vec);
		pz = z + 12; Jac[4][7] += DOT_X(pz, vec);
		pz = z + 15; Jac[5][7] += DOT_X(pz, vec);
	}
}
/*
void od_systemLocal::numDif() {
	int i, j, idx, jdx, ii;
	double delta = 1.0e-2;
	double invDelta = .5 / delta;
	Vec3 tli, tri, fli, fri;
	Vec3 tlj, trj, flj, frj;
	std::map<int, Vec3*> Fi, Fj, Ti, Tj;
	set_states();
	//	Update(1);
	//	updatePartials();
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
		Update();  getVel(lVel); getVelDot(lAcc); getOmega(lOmega); getOmegaDot(lOmegaDot);
		Fi.clear(); Fj.clear(); Ti.clear(); Tj.clear();
		if (num_force()) {
			for (ii = 0; ii < num_force(); ii++)
				force_list_[ii]->evaluate(0);
			pF = force_list_[ii];
			tli = pF->itorque(); tlj = pF->jtorque(); fli = pF->iforce(); flj = pF->jforce();
			Fi[ii] = &fli;
			Fj[ii] = &flj;
			Ti[ii] = &tli;
			Tj[ii] = &tlj;
		}
		states[j] -= 2 * delta; set_states();
		Update();  getVel(rVel); getVelDot(rAcc); getOmega(rOmega); getOmegaDot(rOmegaDot);
		if (num_force()) {
			for (ii = 0; ii < num_force(); ii++)
				force_list_[ii]->evaluate(0);
			pF = force_list_[ii];
			tri = pF->itorque(); trj = pF->jtorque(); fri = pF->iforce(); frj = pF->jforce();
			//tli -= tri; tli.multiplied_by(invDelta); tlj -= trj; tlj.multiplied_by(invDelta);
			tri -= *Ti[ii]; tri.multiplied_by(-invDelta);
			// tlj -= trj; tlj.multiplied_by(invDelta);
			trj -= *Tj[ii]; trj.multiplied_by(-invDelta);
			//fli -= fri; fli.multiplied_by(invDelta);
			fri -= *Fi[ii]; fri.multiplied_by(-invDelta);
			//flj -= frj; flj.multiplied_by(invDelta);
			frj -= *Fj[ii]; frj.multiplied_by(-invDelta);
			idx = pF->i_body_index(); jdx = pF->j_body_index();
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
		Fi.clear(); Fj.clear(); Ti.clear(); Tj.clear();
		if (num_force()) {
			for (int ii = 0; ii < num_force(); ii++)
				force_list_[ii]->evaluate(0);
			//tli = pF->itorque(); tlj = pF->jtorque(); fli = pF->iforce(); flj = pF->jforce();
			//idx = pF->i_body_index(); jdx = pF->j_body_index();
			pF = force_list_[ii];
			tli = pF->itorque(); tlj = pF->jtorque(); fli = pF->iforce(); flj = pF->jforce();
			Fi[ii] = &fli;
			Fj[ii] = &flj;
			Ti[ii] = &tli;
			Tj[ii] = &tlj;
		}
		dstates[j] -= 2 * delta; set_states();
		Update();  getVel(rVel); getVelDot(rAcc); getOmega(rOmega); getOmegaDot(rOmegaDot);
		dstates[j] += delta;
		if (num_force()) {
			for (int ii = 0; ii < num_force(); ii++)
				force_list_[ii]->evaluate(0);
			pF = force_list_[ii];
			tri = pF->itorque(); trj = pF->jtorque(); fri = pF->iforce(); frj = pF->jforce();
			//tli -= tri; tli.multiplied_by(invDelta); tlj -= trj; tlj.multiplied_by(invDelta);
			tri -= *Ti[ii]; tri.multiplied_by(-invDelta);
			// tlj -= trj; tlj.multiplied_by(invDelta);
			trj -= *Tj[ii]; trj.multiplied_by(-invDelta);
			//fli -= fri; fli.multiplied_by(invDelta);
			fri -= *Fi[ii]; fri.multiplied_by(-invDelta);
			//flj -= frj; flj.multiplied_by(invDelta);
			frj -= *Fj[ii]; frj.multiplied_by(-invDelta);
			//tri = pF->itorque(); trj = pF->jtorque(); fri = pF->iforce(); frj = pF->jforce();
			//tli -= tri; tli.multiplied_by(invDelta); tlj -= trj; tlj.multiplied_by(invDelta);
			//fli -= fri; fli.multiplied_by(invDelta); flj -= frj; flj.multiplied_by(invDelta);
			idx = pF->i_body_index(); jdx = pF->j_body_index();
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
	set_states();
	Update();
	updatePartials();
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
*/
