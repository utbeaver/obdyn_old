
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
od_systemTrack2D::od_systemTrack2D(char* Name, od_marker* ref, int from_py) : od_systemGeneric(Name, from_py) {
	char dup_name[80], dup_jname[80];
	char* p = dup_name;
	od_marker*  cm;
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
	freeJ = new od_free_joint(--virtual_joint_id, dup_jname, 0);
	freeJ->set_imarker(cm);
	//freeJ->set_jmarker(ref);
	p = dup_jname;
	strcpy(p, Name);
	strcat(p, "_fixed");
	fixed = new od_fixed_joint(--virtual_joint_id, dup_name, 1);
	fixed->set_imarker(cm);
	fixed->set_jmarker(ref);
	add_constraint(freeJ);
	add_explicit_constraint(fixed);
	JR = 0; JR_dot = 0; JT = 0; JT_dot = 0;
	parOmega_parq = 0; parOmega_dot_parq = 0; parOmega_dot_parq_dot = 0; // parOmega_parq_dot=JR;
	parVel_parqG = 0; parVel_dot_parqG = 0; parVel_dot_parq_dotG = 0; //parVel_parq_dotG; =JTG;
	M00 = 0;// new od_matrix_dense(6, 6);
	M01 = 0;// new od_matrix_dense(6, 6;)
	M10 = 0;
	pTriM = 0;  //new blockTri
}
od_systemTrack2D::~od_systemTrack2D() {
	remove(JR); remove(JR_dot); remove(JT); remove(JT_dot);
	remove(parOmega_parq); remove(parOmega_dot_parq); remove(parOmega_dot_parq_dot);
	remove(parVel_parqG); remove(parVel_dot_parqG); remove(parVel_dot_parq_dotG);
	delete M00; delete M01; delete M10;
}
void od_systemTrack2D::init_tree(double *_p, double *_v, double *_a, int dof_idx1) {
	int i;
	od_joint *pC;
	od_marker *pM;
	od_body *pB;
	nbody = num_body();
	M00 = new od_matrix_dense(6, 6);
	M01 = new od_matrix_dense(6, (nbody - 1) * 3);
	M10 = new od_matrix_denseB((nbody - 1) * 3, 6);
	pTriM = new od_blockTriDiagonal(nbody - 1);
	M_array = new double[nbody];
	//non_array = new double*[nbody];
	J_array = new Mat33*[nbody];
	_tree_rhs = new Vec3[nbody * 2];
	_tree_rhs_alpha = new Vec3[nbody * 2];
	constraint_list_[0]->set_prev_idx(-1);
	constraint_list_[0]->set_tail(nbody - 1);
	for (i = 1; i < nbody; i++) {
		constraint_list_[i]->set_prev_idx(0);
		constraint_list_[i]->set_head(0);
		constraint_list_[0]->set_next_idx(i);
		constraint_list_[0]->set_tail(i);
	}


	for (i = 0; i < nbody; i++) {
		pC = (od_joint*)constraint_list[i];
		pM = pC->get_imarker();
		pB = (od_body*)body_list[i];
		pB->set_from_marker(pM);
		pB = body_list[i];
		pB->update(this, 0);
		pC->initialize();
		pB->initialize();
		pB->set_ang_rhs(_tree_rhs + i);
		pB->set_pos_rhs(_tree_rhs + i + num_body());
		M_array[i] = (pB->get_M());
		J_array[i] = (pB->get_J());
	}
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
	int element_num = (int)element_list.size();
	if (element_num)element_list_ = new od_element*[element_num];
	for (i = 0; i < element_num; i++) element_list_[i] = element_list[i];
	od_systemGeneric::get_states();
	unset_evaluated();
	Update();
	updatePartials();
}
void od_systemTrack2D::calculate_JR(int no_dot) {
	int i, nb;
	double  *z, *zi,   *wxz, *wxzi;
	
	od_joint *pC, *pCi;
	nb = num_body();
	pC = (od_joint*)(*(constraint_list_));
	z = pC->get_zi_global();

	wxz = pC->get_cross_zi();
	for (i = 1; i < nb; i++) {
		pCi = (od_joint*)(*(constraint_list_ + i));
		if (no_dot) {
			JR[i][3] = z + 9; JR[i][4] = z + 12; JR[i][5] = z + 15; //rotation axes
		}
		else {
			JR_dot[i][3] = wxz + 9; JR_dot[i][4] = wxz + 12; JR_dot[i][5] = wxz + 15;
		}
		zi = pCi->get_zi_global(); wxzi = pCi->get_cross_zi();
		if (no_dot) {
			JR[i][8] = zi + 6;
		}
		else {
			JR_dot[i][8] = wxzi + 6;
		}
	}
}
void od_systemTrack2D::calculate_JT(int no_dot) {
	int i,nb;
	double *pvi, *z, *zi, *p, *r, *wxz, *wxzi,  temp[3], *pvb, pv[3];
	od_joint *pC, *pCi;
	double *prb, *pri, pr[3];
	od_body* pB;
	nb = num_body();
	pC = (od_joint*)(*(constraint_list_));
	prb = base->getPosition();
	pvb = base->getVel();
	z = pC->get_zi_global();
	wxz = pC->get_cross_zi();
	for (i = 1; i < nb; i++) {
		pCi = (od_joint*)(*(constraint_list_ + i));
		pB = *(body_list_ + i);
		r = pCi->get_Ri();  //from cm basebody to the cm of link
		pri = pB->getPosition();
		SUB3(pri, prb, pr);
		pvi = pB->getVel();
		SUB3(pvi, pvb, pv);
		if (no_dot) {
			JT[i][0] = z; JT[i][1] = z + 3; JT[i][2] = z + 6;
			p = z + 9; CROSS_X(p, pr, temp);  JT[i][3] = temp;
			p = z + 12; CROSS_X(p, pr, temp); JT[i][4] = temp;
			p = z + 15; CROSS_X(p, pr, temp); JT[i][5] = temp;   //translation axes
		}
		else {
			JT_dot[i][0] = wxz; JT_dot[i][1] = wxz + 3;  JT_dot[i][2] = wxz + 6;
			p = z + 9;
			CROSS_X(p, pv, temp); // 
			JT_dot[i][3] = temp; 
			//p = wxz + 9;    wxz+9 ptr  to 3 0s
			//CROSS_X(p, pr, temp); 
			//JT_dot[i][3] += temp;
			p = z + 12;  //e5
			CROSS_X(p, pv, temp); //e5 x dpt_R
			JT_dot[i][4] = temp; 
			p = wxz + 12;  //w4xe5   
			CROSS_X(p, pr, temp); //(w4xe5)xR 
			JT_dot[i][4] += temp;
			p = z + 15;           //e6
			CROSS_X(p, pv, temp); //e6 x dot_R
			JT_dot[i][5] = temp; 
			p = wxz + 15;     //w5xe6
			CROSS_X(p, pr, temp);  //(w5 x e6) x R
			JT_dot[i][4] += temp;
		}
		zi = pCi->get_zi_global(); wxzi = pCi->get_cross_zi();
		if (no_dot) {
			JT[i][6] = zi;
			JT[i][7] = zi + 3;
		}
		else {
			JT_dot[i][6] = wxzi;
			JT_dot[i][7] = wxzi + 3;
		}
	}
}
void od_systemTrack2D::updatePartials(int pos_only, double alpha) {
//	int     temp_int;
	int nb = num_body();
	//od_joint *pCj, *pCi;

	this->calculate_JR();
	this->calculate_JRdot();
	this->calculate_JT();
	this->calculate_JTdot();
}
void od_systemTrack2D::ParOmegaParq(int dot) {
	int i, nb;
	Vec3* vecs;
	double *z, *pz, *pQ,  *pQ9, *wxz;
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
void od_systemTrack2D::parOmegaDotParq() {
	int i, nb;
	Vec3* vecs;
	double *z, *pz, *pQ,  *pQ9, *wxz, *pwxz;
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
			pQ = pC->getQ(0, od_object::ROT_DOF, 1, 1, vecTemp); //sum_{5,6} e_i dot_q_i
			pQ9 = pCi->getQ(0, od_object::ROT_DOF, 1);  // e_9 dot_q_9
			U_ADD3(pQ, pQ9); pwxz/*wxe_4*/ = wxz + 9; CROSS_X(pwxz, pQ, temp); //wxe_4 X (sum_{5 6 9} e q)
			vecs[3] = temp; ZERO3(vecTemp);
			pQ = pC->getQ(1, od_object::ROT_DOF, 1, 1, vecTemp);
			pQ9 = pCi->getQ(1, od_object::ROT_DOF, 1);
			U_ADD3(pQ, pQ9); pz = z + 9; CROSS_X(pz, pQ, temp); //e_4 x  sum(wxe dot_q)
			vecs[3] += temp; ZERO3(vecTemp);
			pQ = pC->getQ(0, od_object::ROT_DOF, 1, 2, vecTemp);
			pQ9 = pCi->getQ(0, od_object::ROT_DOF, 2);
			U_ADD3(pQ, pQ9); pz = z + 9; CROSS_X(pz, pQ, temp);  //e_4 X  sum(e ddot_q)
			vecs[3] += temp; ZERO3(vecTemp);

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


			pQ9 = pCi->getQ(0, od_object::ROT_DOF, 1);  // e_9 dot_q_9
			pwxz/*wxe_6*/ = wxz + 15; CROSS_X(pwxz, pQ9, temp); //wxe_6 X (sum_{9} e q)
			vecs[5] = temp; ZERO3(vecTemp);

			pQ9 = pCi->getQ(1, od_object::ROT_DOF, 1);
			/*U_ADD3(pQ, pQ9); */pz = z + 15; CROSS_X(pz, pQ9, temp); //e_6 x  sum(wxe dot_q)
			vecs[5] += temp; ZERO3(vecTemp);
			pQ9 = pCi->getQ(0, od_object::ROT_DOF, 2);
			/*U_ADD3(pQ, pQ9);*/ pz = z + 15; CROSS_X(pz, pQ9, temp);  //e_6 X  sum(e ddot_q)
			vecs[5] += temp; ZERO3(vecTemp);
		
	}
}
void od_systemTrack2D::parOmegaDotParqDot() {
	for (int i = 0; i < num_body(); i++) {
		for (int j = 0; j < 9; j++) {
			parOmega_dot_parq_dot[i][j] = parOmega_parq[i][j];
			parOmega_dot_parq_dot[i][j] += JR_dot[i][j];
		}
	}
}
void od_systemTrack2D::parVelParq() {
	int i, j, nb;
	Vec3* vecs;
	double *z, *pz, *pQ, *pQc,  *pwxz, *prb, *r, *pri;
	double  vecTemp[3], temp[3];
	od_body* pB;
	od_joint *pC, *pCi;
	r = pwxz = 0;
	nb = num_body();
	pC = (od_joint*)(*(constraint_list_));
	prb = base->getPosition();
	z = pC->get_zi_global();
	//wxz = pC->get_cross_zi();
	ZERO3(vecTemp);
	for (i = 1; i < nb; i++) {
		pCi = (od_joint*)(*(constraint_list_ + 1));
		//wxz = pCi->get_cross_zi();
		pB = body_list_[i]; pri = pB->getPosition();
		SUB3(pri, prb, r);
		vecs = parVel_parqG[i];

		pQc = pC->getQ(0, od_object::ROT_DOF, 1);
		pz = z + 9;
		TRI_CROSS_X(pQc, pz, r, temp); //Q x (e_4 x R)
		vecs[3] = temp;
		pQ = pC->getQ(0, od_object::ROT_DOF, 1, 1, vecTemp);
		TRI_CROSS_XN(r, pz, pQ, temp); ZERO3(vecTemp); vecs[3] += temp; //(e_4 x Q) x r
		pQ = pCi->getQ(0, od_object::TRA_DOF, 1);    //
		CROSS_X(pz, pQ, temp); vecs[3] += temp; 

		//pQ = pC->getQ(0, od_object::ROT_DOF, 1);
		pz = z + 12;
		TRI_CROSS_X(pQc, pz, r, temp); //Q x (e_5 x R)
		vecs[4] = temp;
		pQ = pC->getQ(0, od_object::ROT_DOF, 1, 2, vecTemp);
		TRI_CROSS_XN(r, pz, pQ, temp); ZERO3(vecTemp); vecs[4] += temp; //(e_4 x Q) x r
		pQ = pCi->getQ(0, od_object::TRA_DOF, 1);    //
		CROSS_X(pz, pQ, temp); vecs[4] += temp;

		//pQ = pC->getQ(0, od_object::ROT_DOF, 1);
		pz = z + 15;
		TRI_CROSS_X(pQc, pz, r, temp); //Q x (e_5 x R)
		vecs[5] = temp;
		pQ = pCi->getQ(0, od_object::TRA_DOF, 1);
		CROSS_X(pz, pQ, temp); vecs[5] += temp;
		
		pQ=pCi->getQ(0, od_object::TRA_DOF, 1, 0); 
		ZERO3(vecTemp);
		for (j = 0; j < 3; j++) {
			vecTemp[0] += z[12 + j * 3];
			vecTemp[1] += z[12 + j * 3+1];
			vecTemp[1] += z[12 + j * 3 + 2];
		}
		CROSS_X(vecTemp, pQ, temp); 
		vecs[6] = temp;
		pQ = pCi->getQ(0, od_object::TRA_DOF, 1, 1);
		CROSS_X(vecTemp, pQ, temp);
		vecs[7] = temp;
		CROSS_X(vecTemp, pwxz, temp); vecs[6] = temp;
	}
}
void od_systemTrack2D::parVelDotParq() {
	int i,  nb;
	Vec3* vecs;
	double *z, *pz, *pQ,   *wxz, *pwxz, *prb, *r, *pri, *pvb, *v, *pvi;
	double  vecTemp[3], temp[3];
	od_body* pB;
	od_joint *pC, *pCi;
	r = v = 0;
	nb = num_body();
	pC = (od_joint*)(*(constraint_list_));
	prb = base->getPosition();
	pvb = base->getVel();
	z = pC->get_zi_global();
	wxz = pC->get_cross_zi();
	ZERO3(vecTemp);
	for (i = 1; i < nb; i++) {
		pCi = (od_joint*)(*(constraint_list_ + 1));
		pB = body_list_[i]; pri = pB->getPosition(); pvi = pB->getVel();
		SUB3(pri, prb, r);
		SUB3(pvi, pvb, v);

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

		///////

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

		///////
		ZERO3(vecTemp);
		pQ = pC->getQ(1, od_object::ROT_DOF, 1); pz = z + 15;
		TRI_CROSS_X(pQ, pz, r, temp); ZERO3(vecTemp);
		vecs[5] = temp;

		pQ = pC->getQ(0, od_object::ROT_DOF, 1); pwxz = wxz + 15;
		TRI_CROSS_X(pQ, pwxz, r, temp); vecs[5] += temp;
		TRI_CROSS_X(pQ, pz, v, temp); vecs[5] += temp; 
	}
}
void od_systemTrack2D::parVelDotParqDot() {
	for (int i = 0; i < num_body(); i++) {
		for (int j = 0; j < 9; j++) {
			parVel_dot_parq_dotG[i][j] = parVel_parqG[i][j];
			parVel_dot_parq_dotG[i][j] += JT_dot[i][j];
		}
	}
}
void od_systemTrack2D::CreateRotM(double **pM, int base) {
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
					if (k != j) M00->add(j, k, val);
				}
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
			M01->add(j, offset+8, val);
			M10->add(offset+8, j, val);
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
void od_systemTrack2D::CreateTraM(double **pM, int base) {
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
				if (j != k) M00->add(k, j, val);
			}
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
				M01->add(k, offset+j, val);
				M10->add(offset+j, k, val);
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
void od_systemTrack2D::parF_parq(double**,  double alpha) {
	//double Jac[9][9];
}