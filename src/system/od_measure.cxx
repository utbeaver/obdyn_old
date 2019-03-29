// od_measure.cxx: implementation of the od_measure class.
//
//////////////////////////////////////////////////////////////////////

#include "od_measure.h"
#include "od_system.h"

od_joint_measure::od_joint_measure(int Id, char* name_, od_systemGeneric *psys, int _p0v1a2, int _ith) : od_element(Id, name_)
{
	ith = _ith; pJ = 0; pSys = psys; p0v1a2 = _p0v1a2;
	if (p0v1a2 == 0) {
		pva_partial[0] = 1.0; pva_partial[1] = 0.0; pva_partial[2] = 0.0;
	}
	else if (p0v1a2 == 1) {
		pva_partial[0] = 0.0; pva_partial[1] = 1.0; pva_partial[2] = 0.0;
	}
	else {
		pva_partial[0] = 0.0; pva_partial[1] = 0.0; pva_partial[2] = 1.0;
	}
}

void od_joint_measure::init() {
	pJ = (od_joint*)pSys->get_constraint_via_id(id);
}


void od_joint_measure::eval() {
	pva[0] = pJ->get_P(ith);// get_temp_pva(ith);
	pva[1] = pJ->get_V(ith);// get_temp_pva(ith, od_object::VEL);
	pva[2] = pJ->get_A(ith);// get_temp_pva(ith, od_object::ACC);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

od_measure::od_measure(od_systemGeneric* psys, od_measure::Measure_Type type, int i, int j, int k)
	: od_element(0, "")
{
	I = i; J = j; K = k; partialp = 0; partialv = 0;
	pSys = psys;
	pM[0] = 0;  pM[1] = 0; pM[2] = 0;
	if (I) pM[0] = pSys->get_marker_via_id(I);
	if (J) pM[1] = pSys->get_marker_via_id(J);
	if (K) pM[2] = pSys->get_marker_via_id(K);
	_type = type;
	pSys = psys;
	pSys->add_measure(this);
	/*for (int ii = 0; ii < 3; ii++) {
		partial_[ii] = 0;
		columns_[ii] = 0;
		for (int jj = 0; jj < 3; jj++) pva_ijk[ii][jj] = 0;
	}*/
}

od_measure::od_measure(od_systemGeneric* psys, Measure_Type type, od_marker* i, od_marker* j, od_marker* k)
	: od_element(0, "")
{
	pM[0] = i;  pM[1] = j; pM[2] = k; partialp = 0; partialv = 0;
	if (pM[0]) I = pM[0]->get_id();
	if (pM[1]) J = pM[1]->get_id();
	if (pM[2]) K = pM[2]->get_id();
	_type = type;
	pSys = psys;
	pSys->add_measure(this);
	/*for (int ii = 0; ii < 3; ii++) {
		partial_[ii] = 0;
		columns_[ii] = 0;
		for (int jj = 0; jj < 3; jj++) pva_ijk[ii][jj] = 0;
	}*/
}

od_measure::~od_measure()
{
	int i;
	for (i = 0; i < 3; i++) {
		//for (ii = 0; ii < size_[i]; ii++) delete partial_[i][ii];
		//if (columns_[i]) delete[] columns_[i];
		//if (partial_[i]) delete[] partial_[i];
		if (variableijkp[i]) delete[] variableijkp[i];
		if (variableijkv[i]) delete[] variableijkv[i];
	}
	DELARY(partialp); DELARY(partialv); DELARY(variablep); DELARY(variablev);
	//for (i = 0; i < numVec; i++) delete partialVec[i];
	//DELARY(partialVec);
}

void od_measure::init()
{
	variablep = 0;
	variablev = 0;
	//	vector<int> columns[3], columns1[3];
	vector<int> columnp, columnv;
	vector<int> _variablep, _variablev;
	vector<int> _variableijkp[3], rows[3], _variableijkv[3];
	int num_j, i, j, _size;// , idx;
//	od_body *pB;

	vector<Vec3*> _partialp;
	vector<Vec3*> _partialv;
	num_j = pSys->tree_dofs();

	columnp.resize(num_j); _partialp.resize(0);
	columnv.resize(num_j);
	_partialv.resize(0);
	fill(columnp.begin(), columnp.end(), 0);
	fill(columnv.begin(), columnv.end(), 0);

	fill(b_indices, b_indices + 3, -1);
	for (i = 0; i < 3; i++) {
		_variableijkp[i].resize(0); _variableijkv[i].resize(0);
		rows[i].resize(num_j); fill(rows[i].begin(), rows[i].end(), 0);
		if (!pM[i]) continue;
		if (pM[i]->get_body()->ground_body()) continue;
		b_indices[i] = pM[i]->get_body()->get_index();
		for (j = 0; j < num_j; j++) {
			//rows[i][j] = abs(pSys->relevenceLevel2[b_indices[i] * num_j + j]);
			rows[i][j] = abs(pSys->getL2(b_indices[i] * num_j + j));
		}
	}
	for (i = 0; i < num_j; i++) {
		columnp[i] = rows[0][i] + rows[1][i] + rows[2][i];
		columnv[i] = rows[0][i] + rows[1][i];
	}


	for (i = 0; i < num_j; i++) {
		if (columnp[i]) { _partialp.push_back(new Vec3); _variablep.push_back(i); }
		if (columnv[i]) { _partialv.push_back(new Vec3); _variablev.push_back(i); }
		if (rows[0][i]) _variableijkp[0].push_back(_variablep.size() - 1);
		if (rows[1][i]) _variableijkp[1].push_back(_variablep.size() - 1);
		if (rows[2][i]) _variableijkp[2].push_back(_variablep.size() - 1);
		if (rows[0][i]) _variableijkv[0].push_back(_variablev.size() - 1);
		if (rows[1][i]) _variableijkv[1].push_back(_variablev.size() - 1);
		if (rows[2][i]) _variableijkv[2].push_back(_variablev.size() - 1);
	}
	_size = _partialp.size();
	if (_size) { partialp = new Vec3*[_size]; variablep = new int[_size]; }
	else { partialp = 0; variablep = 0; }
	for (i = 0; i < _size; i++) { partialp[i] = _partialp[i]; variablep[i] = _variablep[i]; }
	lenP = _size;
	_size = _partialv.size();
	if (_size) { partialv = new Vec3*[_size]; variablev = new int[_size]; }
	else { partialv = 0; variablev = 0; }
	for (i = 0; i < _size; i++) { partialv[i] = _partialv[i]; variablev[i] = _variablev[i]; }
	lenV = _size;

	for (i = 0; i < 3; i++) {
		_size = _variableijkp[i].size();
		if (_size) { variableijkp[i] = new int[_size]; }
		else { variableijkp[i] = 0; }
		for (j = 0; j < _size; j++) variableijkp[i][j] = _variableijkp[i][j];
		lijkp[i] = _size;
		_size = _variableijkv[i].size(); if (_size) { variableijkv[i] = new int[_size]; }
		else { variableijkv[i] = 0; }
		for (j = 0; j < _size; j++) variableijkv[i][j] = _variableijkv[i][j];
		lijkv[i] = _size;
	}


}

void od_measure::eval(int eva_partial) {
	if (is_evaluated() && !eva_partial) return;
	double* pd = vals;

	switch (_type) {
	case DT:
		pd = pM[0]->get_position(pd, pM[1], pM[2]); break;
	case VT:
		pd = pM[0]->get_velocity(pd, pM[1], pM[2]); break;
	case AT:
		pd = pM[0]->get_acceleration(pd, pM[1], pM[2]); break;
	case DR:
		pd = pM[0]->get_rotation(pd, pM[1]); break;
	case VR:
		pd = pM[0]->get_omega(pd, pM[1], pM[2]); break;
	case AR:
		pd = pM[0]->get_omega_dot(pd, pM[1], pM[2]); break;
	default:
		pd[0] = pd[1] = pd[2] = pd[3] = 0.0; break;
	}
	pd[3] = sqrt(DOT_X(vals, vals));

	if (eva_partial) {
		double vecTemp[3], vecTemp1[3], temp_d[3];
		int i, j, k, offset = 0, idx = 0;
		od_body *pB = 0;
		double *pdTemp = 0;
		Vec3 *pV = 0;
		double *xi, *yi, *zi, *xj, *yj, *zj;
		for (i = 0; i < lenP; i++) { partialp[i]->init(); }
		for (i = 0; i < lenV; i++) { partialv[i]->init(); }
		//for(i=0; i<3; i++) for(j=0; j<(int)partial[i].size(); j++) partial_[i][j]->init();
		switch (_type) {
		case DT:
			pB = pM[0]->get_body();
			if (pB->ground_body() == 0) {
				//because r = r0 + r1
				//par_r = par_r0 + e x r1 
				idx = pB->get_index();
				/*for (i = 0; i < pva_ijk[0][0]; i++) {
					pV = pSys->JTG.element(idx, columns_[0][i]);
					if (pV) *(partial_[0][i]) = pV; //par_r0
					pV = pSys->JR.element(idx, columns_[0][i]); //e
					if (pV) {
						pd = pV->v;
						pdTemp = pM[0]->absolute_rel_pos(); //r1
						CROSS_X(pd, pdTemp, vecTemp); //e x r1
						*(partial_[0][i]) += vecTemp;
					}
				}*/
				for (i = 0; i < lijkp[0]; i++) {
					j = variableijkp[0][i];
					k = variablep[j];
					pV = pSys->getJT(idx, k);// JTG.element(idx, k);
					if (pV) *(partialp[j]) += pV->v; //par_r0
					pV = pSys->getJR(idx, k);// JR.element(idx, k); //e
					if (pV) {
						pd = pV->v; //e
						pdTemp = pM[0]->absolute_rel_pos(); //r1
						CROSS_X(pd, pdTemp, vecTemp); //e x r1
						*(partialp[j]) += vecTemp;
					}
				}
			}
			if (pM[1]) {
				pB = pM[1]->get_body();
				if (pB->ground_body() == 0) {
					idx = pB->get_index();
					/*for (i = pva_ijk[0][0]; i < pva_ijk[0][1]; i++) {
						pV = pSys->JTG.element(idx, columns_[0][i]);
						if (pV) *(partial_[0][i]) = pV;
						pV = pSys->JR.element(idx, columns_[0][i]);
						if (pV) {
							pd = pV->v;
							pdTemp = pM[1]->absolute_rel_pos();
							CROSS_X(pd, pdTemp, vecTemp);
							*(partial_[0][i]) += vecTemp;
						}
						partial_[0][i]->negate();
					}*/
					for (i = 0; i < lijkp[1]; i++) {
						j = variableijkp[1][i];
						k = variablep[j];
						pV = pSys->getJT(idx, k);// JTG.element(idx, k);
						if (pV) *(partialp[j]) -= pV->v; //par_r0
						pV = pSys->getJR(idx, k);// JR.element(idx, k); //e
						if (pV) {
							pd = pV->v; //e
							pdTemp = pM[1]->absolute_rel_pos(); //r1
							CROSS_X(pd, pdTemp, vecTemp); //e x r1
							*(partialp[j]) -= vecTemp;
						}
					}
				}
			}
			if (pM[2]) {
				pB = pM[2]->get_body();
				if (pB->ground_body() == 0) {
					idx = pB->get_index();
					/*for (i = 0; i < pva_ijk[0][1]; i++) pM[2]->vec_wrt_this(*(partial_[0][i]));
					pM[0]->get_position(vecTemp1, pM[1]);
					for (i = pva_ijk[0][1]; i < pva_ijk[0][2]; i++) {
						pV = pSys->JR.element(idx, columns_[0][i]);
						if (pV) {
							pd = pV->v;
							CROSS_X(pd, vecTemp1, vecTemp);
							NEG(vecTemp);
							pM[2]->vec_wrt_this(vecTemp);
							*(partial_[0][i]) += vecTemp;
						}
					}*/
					for (i = 0; i < lenV; i++) {
						k = variablev[i]; pM[2]->vec_wrt_this(*(partialp[k])); //I J only
					}
					pM[0]->get_position(vecTemp1, pM[1]);
					for (i = 0; i < lijkp[2]; i++) {
						j = variableijkp[2][i]; k = variablep[j]; //for position 
						pV = pSys->getJR(idx, k);// JR.element(idx, k);
						if (pV) {
							pd = pV->v;
							CROSS_X(pd, vecTemp1, vecTemp);
							//NEG(vecTemp);
							pM[2]->vec_wrt_this(vecTemp);
							*(partialp[j]) -= vecTemp;
						}
					}
				}
			}
			break;
		case VT:
			if (pSys->get_dva_type() != od_object::DISP_VEL_ACC) break;
			//because v=v0+omega x r
			pB = pM[0]->get_body();
			pdTemp = pM[0]->absolute_rel_pos(); //r
			if (pB->ground_body() == 0) {
				idx = pB->get_index();
				//par_v_parq = par_v0_parq + par_omega_parq x r + omega x par_r_parq
				/*for (i = 0; i < pva_ijk[0][0]; i++) {
					*(partial_[0][i]) = *(pSys->parVel_parqG.element(idx, columns_[0][i]));
					pV = pSys->parOmega_parq.element(idx, columns_[0][i]); //par_omega_parq
					if (pV) {
						pd = pV->v;
						CROSS_X(pd, pdTemp, vecTemp); //par_omega_parq x r
						*(partial_[0][i]) += vecTemp;
					}
					if (pV) {
						pV = pSys->JR.element(idx, columns_[0][i]); //e
						pd = pV->v;
						CROSS_X(pd, pdTemp, vecTemp); //e x r = par_r_parq
						pd = pB->cm_marker()->get_omega();
						CROSS_X(pd, vecTemp, temp_d); //omega *(e x r)
						*(partial_[0][i]) += temp_d;
					}
				}*/
				for (i = 0; i < lijkp[0]; i++) {
					j = variableijkp[0][i]; k = variablep[j];
					*(partialp[j]) += *(pSys->getJTdot_dq(idx, k));// .element(idx, k));
					pV = pSys->getJRdot_dq(idx, k);// parOmega_parq.element(idx, k); //par_omega_parq
					if (pV) {
						pd = pV->v;
						CROSS_X(pd, pdTemp, vecTemp); //par_omega_parq x r
						*(partialp[j]) += vecTemp;
					}
					if (pV) {
						pV = pSys->getJR(idx, k);// JR.element(idx, k); //e
						pd = pV->v;
						CROSS_X(pd, pdTemp, vecTemp); //e x r = par_r_parq
						pd = pB->cm_marker()->get_omega();
						CROSS_X(pd, vecTemp, temp_d); //omega *(e x r)
						*(partialp[j]) += temp_d;
					}
				}
				for (i = 0; i < lijkv[0]; i++) {
					j = variableijkv[0][i]; k = variablev[j];
					//par_v_parq_dot = par_v0_par1_dot + par_omega_parq_dot x r
					*(partialv[j]) += pSys->getJT(idx, k);//parVel_parq_dotG.element(idx, k);
					pV = pSys->getJR(idx, k);//parOmega_parq_dot.element(idx, k); //par_omega_parq_dot
					if (pV) {
						pd = pV->v;
						CROSS_X(pd, pdTemp, vecTemp); //par_omega_parq_dot x r
						*(partialv[j]) += vecTemp;
					}
				}
				/*for (i = 0; i < pva_ijk[1][0]; i++) {
					//par_v_parq_dot = par_v0_par1_dot + par_omega_parq_dot x r
					*(partial_[1][i]) = (pSys->parVel_parq_dotG.element(idx, k));
					pV = pSys->parOmega_parq_dot.element(idx, columns_[1][i]); //par_omega_parq_dot
					if (pV) {
						pd = pV->v;
						CROSS_X(pd, pdTemp, vecTemp); //par_omega_parq_dot x r
						*(partial_[1][i]) += vecTemp;
					}
				}*/
			}
			if (pM[1]) {
				pB = pM[1]->get_body();
				pdTemp = pM[1]->absolute_rel_pos(); //r
				if (pB->ground_body() == 0) {
					idx = pB->get_index();
					//offset = size_[0];
					/*for (i = pva_ijk[0][0]; i < pva_ijk[0][1]; i++) {
						partial_[0][i]->init();
						//par_v_parq = par_v0_parq + par_omega_parq x r + omega x par_r_parq
						*(partial_[0][i]) -= (pSys->parVel_parqG.element(idx, columns_[0][i]));
						pV = pSys->parOmega_parq.element(idx, columns_[0][i]); //par_omega_parq
						if (pV) {
							pd = pV->v;
							CROSS_X(pd, pdTemp, vecTemp); //par_omega_parq x r
							*(partial_[0][i]) -= vecTemp;
						}
						pV = pSys->JR.element(idx, columns_[0][i]); //e
						if (pV) {
							pd = pV->v;
							CROSS_X(pd, pdTemp, vecTemp); //e x r = par_r_parq
							pd = pB->cm_marker()->get_omega();
							CROSS_X(pd, vecTemp, temp_d); //omega *(e x r)
							*(partial_[0][i]) -= temp_d;
						}
					}*/
					//par_v_parq = par_v0_parq + par_omega_parq x r + omega x par_r_parq
					for (i = 0; i < lijkp[1]; i++) {
						j = variableijkp[1][i]; k = variablep[j];
						*(partialp[j]) -= *(pSys->getJTdot_dq(idx, k));// .element(idx, k));// par_v0_parq
						pV = pSys->getJRdot_dq(idx, k);// parOmega_parq.element(idx, k); //par_omega_parq
						if (pV) {
							pd = pV->v;
							CROSS_X(pd, pdTemp, vecTemp); //par_omega_parq x r
							*(partialp[j]) -= vecTemp;
						}
						if (pV) {
							pV = pSys->getJR(idx, k);//JR.element(idx, k); //e
							pd = pV->v;
							CROSS_X(pd, pdTemp, vecTemp); //e x r = par_r_parq
							pd = pB->cm_marker()->get_omega();
							CROSS_X(pd, vecTemp, temp_d); //omega *(e x r)
							*(partialp[j]) -= temp_d;
						}
					}
					for (i = 0; i < lijkv[1]; i++) {
						j = variableijkv[1][i]; k = variablev[j];
						//par_v_parq_dot = par_v0_par1_dot + par_omega_parq_dot x r
						*(partialv[j]) -= pSys->getJT(idx, k);//parVel_parq_dotG.element(idx, k);
						pV = pSys->getJR(idx, k);//parOmega_parq_dot.element(idx, k); //par_omega_parq_dot
						if (pV) {
							pd = pV->v;
							CROSS_X(pd, pdTemp, vecTemp); //par_omega_parq_dot x r
							*(partialv[j]) -= vecTemp;
						}
					}
					/*for (i = pva_ijk[1][0]; i < pva_ijk[1][1]; i++) {
						partial_[1][i]->init();
						//par_v_parq_dot = par_v0_par1_dot + par_omega_parq_dot x r
						*(partial_[1][i]) -= (pSys->parVel_parq_dotG.element(idx, columns_[1][i]));
						pV = pSys->parOmega_parq_dot.element(idx, columns_[1][i]); //par_omega_parq_dot
						if (pV) {
							pd = pV->v;
							CROSS_X(pd, pdTemp, vecTemp); //par_omega_parq_dot x r
							*(partial_[1][i]) -= vecTemp;
						}
					}*/
				}
			}
			if (pM[2]) {
				pB = pM[2]->get_body();
				if (pB->ground_body() == 0) {
					idx = pB->get_index();
					/*offset += size_[1];
					for (i = 0; i < pva_ijk[0][1]; i++) {
						*(partial_[0][i]) = pM[2]->vec_wrt_this(*(partial_[0][i]));
						*(partial_[1][i]) = pM[2]->vec_wrt_this(*(partial_[1][i]));
					}
					pM[0]->get_velocity(vals, pM[1]);
					for (i = pva_ijk[0][1]; i < pva_ijk[0][2]; i++) {
						pd = pSys->JR.element(idx, columns_[0][i])->v;
						CROSS_X(pd, vals, vecTemp);
						NEG(vecTemp);
							pM[2]->vec_wrt_this(vecTemp);
						*(partial_[0][i]) += vecTemp;
					}*/
					/*for (i = 0; i < lijkp[0]; i++) {
						j = variableijkp[0][i]; k = variablep[j];
						*partialv[k] = pM[2]->vec_wrt_this(*partialv[k]);
						*partialp[k] = pM[2]->vec_wrt_this(*partialp[k]);
					}
					for (i = 0; i < lijkp[1]; i++) {
						j = variableijkp[1][i]; k = variablep[j];
						*partialv[k] = pM[2]->vec_wrt_this(*partialv[k]);
						*partialp[k] = pM[2]->vec_wrt_this(*partialp[k]);
					}*/
					for (i = 0; i < lenV; i++) {
						*partialp[k] = pM[2]->vec_wrt_this(*partialp[k]);
						*partialv[k] = pM[2]->vec_wrt_this(*partialv[k]);
					}
					pM[0]->get_velocity(vals, pM[1]);
					for (i = 0; i < lijkp[2]; i++) {
						j = variableijkp[2][i]; k = variablep[j];
						pd = pSys->getJR(idx, k)->v;//JR.element(idx, k)->v;
						CROSS_X(pd, vals, vecTemp); pM[2]->vec_wrt_this(vecTemp);
						*partialp[j] -= vecTemp;
					}

				}
			}
			break;
		case DR:
			double yiyj, yizj, zixj, zizj, xiyj, xixj;
			xi = pM[0]->get_global_axis(0);
			yi = pM[0]->get_global_axis(1);
			zi = pM[0]->get_global_axis(2);
			if (pM[1]) {
				xj = pM[1]->get_global_axis(0);
				yj = pM[1]->get_global_axis(1);
				zj = pM[1]->get_global_axis(2);
			}
			else {
				xj = new double[3]; xj[0] = 1.0; xj[1] = 0.0; xj[2] = 0.0;
				yj = new double[3]; yj[0] = 0.0; yj[1] = 1.0; yj[2] = 0.0;
				zj = new double[3]; zj[0] = 0.0; zj[1] = 0.0; zj[2] = 1.0;
			}
			yiyj = DOT_X(yi, yj);
			yizj = DOT_X(yi, zj);
			zixj = DOT_X(zi, xj);
			zizj = DOT_X(zi, zj);
			xiyj = DOT_X(xi, yj);
			xixj = DOT_X(xi, xj);
			//offset=0;

			pB = pM[0]->get_body();
			idx = pB->get_index();
			//	for (i = 0; i < pva_ijk[0][0]; i++) {
			if (pB->ground_body() == 0) {
				for (i = 0; i < lijkp[0]; i++) {
					j = variableijkp[0][i]; k = variablep[j];
					pV = pSys->getJR(idx, j);//JR.element(idx, j);
					if (!pV) continue;
					pd = pV->v;
					CROSS_X(pd, yi, vecTemp);
					temp_d[0] = DOT_X(vecTemp, zj)*yiyj - yizj * DOT_X(vecTemp, yj);
					CROSS_X(pd, zi, vecTemp);
					temp_d[1] = DOT_X(vecTemp, xj)*zizj - zixj * DOT_X(vecTemp, zj);
					CROSS_X(pd, xi, vecTemp);
					temp_d[2] = DOT_X(vecTemp, yj)*xixj - xiyj * DOT_X(vecTemp, xj);
					*partialp[j] += temp_d;
				}
			}

			if (pM[1]) {
				pB = pM[1]->get_body();
				if (pB->ground_body() == 0) {
					idx = pB->get_index();
					//					for (i = pva_ijk[0][0]; i < pva_ijk[0][1]; i++) {
					for (i = 0; i < lijkp[1]; i++) {
						j = variableijkp[1][i]; k = variablep[j];
						pV = pSys->getJR(idx, k);//JR.element(idx, k);
						if (!pV) continue;
						pd = pV->v;
						CROSS_X(pd, zj, vecTemp); CROSS_X(pd, yj, vecTemp1);
						temp_d[0] = DOT_X(vecTemp, yi)*yiyj - yizj * DOT_X(vecTemp1, yi);
						CROSS_X(pd, xj, vecTemp); CROSS_X(pd, zj, vecTemp1);
						temp_d[1] = DOT_X(vecTemp, zi)*zizj - zixj * DOT_X(vecTemp1, zi);
						CROSS_X(pd, yj, vecTemp); CROSS_X(pd, xj, vecTemp1);
						temp_d[2] = DOT_X(vecTemp, xi)*xixj - xiyj * DOT_X(vecTemp1, xi);
						*partialp[j] += temp_d;
					}
				}
			}
			else {
				delete[] xj; delete[] yj; delete[] zj;
			}

			break;
		case VR:
			if (pSys->get_dva_type() != od_object::DISP_VEL_ACC) break;
			pB = pM[0]->get_body();
			if (pB->ground_body() == 0) {
				idx = pB->get_index();
				/*for (i = 0; i < pva_ijk[0][0]; i++) {
					*(partial_[0][i]) = (pSys->parOmega_parq.element(idx, columns_[0][i]));
				}
				for (i = 0; i < pva_ijk[1][0]; i++) {
					*(partial_[1][i]) = (pSys->parOmega_parq_dot.element(idx, columns_[1][i]));
				}*/
				for (i = 0; i < lijkp[0]; i++) {
					j = variableijkp[0][i]; k = j;// variablep[j];
					*partialp[j] += pSys->getJRdot_dq(idx, k);//parOmega_parq.element(idx, k));
					*partialv[j] += pSys->getJR(idx, k);//parOmega_parq_dot.element(idx, k));
				}

			}
			if (pM[1]) {
				pB = pM[1]->get_body();
				if (pB->ground_body() == 0) {
					idx = pB->get_index();
					/*offset = size_[0];
					for (i = pva_ijk[0][0]; i < pva_ijk[0][1]; i++) {
						partial_[0][i]->init();
						*(partial_[0][i]) -= (pSys->parOmega_parq.element(idx, columns_[0][i]));
					}
					for (i = pva_ijk[1][0]; i < pva_ijk[1][1]; i++) {
						partial_[1][i]->init();
						*(partial_[1][i]) -= (pSys->parOmega_parq_dot.element(idx, columns_[1][i]));
					}*/
					for (i = 0; i < lijkp[1]; i++) {
						j = variableijkp[1][i]; k = variablep[j];
						*partialp[k] -= pSys->getJRdot_dq(idx, k);//parOmega_parq.element(idx, k));
						*partialv[k] -= pSys->getJR(idx, k);//parOmega_parq_dot.element(idx, k));
					}
				}
			}
			if (pM[2]) {
				pB = pM[2]->get_body();
				if (pB->ground_body() == 0) {
					//offset += size_[1];
					//for (i = 0; i < pva_ijk[0][1]; i++) pM[2]->vec_wrt_this(*(partial_[0][i]));
					for (k = 0; k < lenV; k++) {
						*partialp[k] = pM[2]->vec_wrt_this(*partialp[k]);
						*partialv[k] = pM[2]->vec_wrt_this(*partialv[k]);
					}
					//for (i = pva_ijk[0][1]; i < pva_ijk[0][2]; i++) {
					for (i = 0; i < lijkp[2]; i++) {
						j = variableijkp[2][i]; k = variablep[j];
						pV = pSys->getJR(idx, k);//JR.element(idx, k);
						pd = pV->v;
						CROSS_X(pd, vals, vecTemp);
						//NEG(vecTemp);
						pM[2]->vec_wrt_this(vecTemp);
						*partialp[k] -= vecTemp;
					}
				}
			}
			break;
		default:
			break;
		}
	}
	evaluated();
}

double od_measure::get_DM_partial(int p0v1a2, int i) {
	Vec3* pV = get_partial(p0v1a2, i);//partial[p0v1a2][i];
	double fltTemp = vals[0] * (pV->v)[0] + vals[1] * (pV->v)[1] + vals[2] * (pV->v)[2];
	fltTemp /= vals[3];
	return fltTemp;
}
