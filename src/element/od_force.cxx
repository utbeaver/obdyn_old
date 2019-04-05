// od_force.cxx: implementation of the od_force class.
//
//////////////////////////////////////////////////////////////////////
#include <iostream>
#include "od_force.h"
#include "od_force_api.h"
#include "od_constraint_api.h"
#include "od_marker_api.h"
#include "od_marker.h"
#include "od_system.h"
#include "od_measure.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

od_jointF::od_jointF(int Id, char* name_, od_joint* pJ, int _ith) :od_element(Id, name_) {
	pJoint = pJ;
	initialized = 0;
	val = 0.0;
	ith = _ith;
	reversed = 0;
}

od_joint_spdp::od_joint_spdp(int Id, char* name_, od_joint* pJ, double k, double c, double l, double f0, int _ith) : od_jointF(Id, name_, pJ, _ith) {
	K = k; L = l; C = c; F0 = f0;
}

int od_joint_spdp::evaluate_rhs() {
	double d, v;// , a;
	od_object::Analysis_Type _type = pSys->get_analysis_type();
	d = pJoint->get_P(ith);
	v = pJoint->get_V(ith);// , od_object::VEL);
	val = 0;
	if (_type == DYNAMIC || _type == ACC_FORCE) {
		val = -K * (d - L) - C * v + F0;
	}
	else if (pSys->get_analysis_type() == STATIC) {
		val = -K * (d - L) + F0;
	}
	//a = pJ->get_temp_pva(ith, od_object::ACC);
	return 1;
}

od_joint_force::od_joint_force(int Id, char* name_, od_joint* pJ, char* _expr, int _ith) : od_jointF(Id, name_, pJ, _ith)
{
	pJoint = pJ;
	initialized = 0;
	if (_expr) pExpr = new Expression(_expr, pSys);
	val = 0.0;
	ith = _ith;
	reversed = 0;
}

void od_joint_force::init() {
	od_jointF::init1();
	pExpr->set_system(pSys);
	pExpr->init();
	initialized = 1;
	//reversed = ((od_joint*)pJoint)->ifReversed(ith);
}

int od_joint_force::get_partial_size(int i) const { return (int)pExpr->get_partial_size(i); }

int od_joint_force::get_partial_col(int i, int j)const { return pExpr->get_partial_col(i, j); }

double od_joint_force::get_partial(int i, int j)const {
	double tempd = pExpr->get_partial(i, j);
	if (reversed)
		tempd = -tempd;
	return tempd;
}

void od_joint_force::evaluate(int partial) {
	val = pExpr->eval(partial);
	if (reversed) val = -val;
}

void od_jointF::init1() { index__ = pJoint->get_start_index() + ith; }

od_single_force::od_single_force(int Id, char* name_, od_marker* I, od_marker* J) : od_force(Id, name_, I, J) {
	//pExpr = 0;
	//columns_fdirection.resize(0);
	columns_fdirection_size = 0;
	partial_fdirection.resize(0);
	partialDirection = 0;
	DT = 0;
}
od_single_force::~od_single_force() {
	DELARY(partialDirection);
	//if (DT) delete DT; DT = 0;
}

void od_single_force::init_single_force() {
	int i, j, col;
	i = i_marker->get_id();
	if (j_marker) j = j_marker->get_id();
	else j = 0;
	//DT = new od_measure(pSys, od_measure::DT, i, j);
	//DT->init();
	columns_fdirection_size = DT->get_partial_size();
	//columns_fdirection.resize(columns_fdirection_size);
	partialDirection = new od_colVec[columns_fdirection_size];
	for (i = 0; i < columns_fdirection_size; i++) {
		col = DT->get_partial_col(0, i);
		partialDirection[i].setCol(col);
	}
}

void od_single_force::eval_single_force(int _partial) {

	DT->eval(_partial);
	cos_q = DT->get_val();
	double len = cos_q.mag();
	if (_partial) {
		double temp, xyz[3], *p,  *pd;
		Vec3 *pV;
		p = cos_q.v;
		double len2 = len * len;
		double len3 = len2 * len;
		for (int i = 0; i < columns_fdirection_size; i++) {
			pV = DT->get_partial(0, i);
			pd=pV->v;
			pV->VxD(xyz, 1.0 / len, 1);   //dr_dq/r 
			temp = -DOT_X(p, pd) / len3;
			cos_q.VxD(xyz, temp);      //
			partialDirection[i].setVals(xyz);
		}
	}
	cos_q.multiplied_by(1.0/len);
}

od_force::od_force(int Id, char* name_, od_marker* I, od_marker* J) : od_connector(Id, name_, I, J)
{
	int i;
	initialized = 0;
	pSys = 0;
	//DT = 0;
	pExpr = 0;
	//columns_fdirection.resize(0);

	for (i = 0; i < 3; i++) {
		columns_iforce[i].resize(0);
		partial_iforce[i].resize(0);
		columns_itorque[i].resize(0);
		partial_itorque[i].resize(0);

		columns_jforce[i].resize(0);
		partial_jforce[i].resize(0);
		columns_jtorque[i].resize(0);
		partial_jtorque[i].resize(0);
	}

	//columns_fdirection_size = 0;
	columns_parRi_size = 0;
	columns_parRj_size = 0;
	columns_parRi.resize(0);
	columns_parRj.resize(0);
	parRi.resize(0);
	parRj.resize(0);
	//  ZERO3(Ri) ZERO3(Rj)
	iNotGround = (int)(I->get_body()->ground_body() == 0);
	jNotGround = (int)(J->get_body()->ground_body() == 0);
	parRiparq = parRjparq = 0; //partialDirection = 0;
	for (i = 0; i < 3; i++) {
		partialIforce[i] = partialJforce[i] = partialItorque[i] = partialJtorque[i] = 0;
		columns_iforce_size[i] = columns_jforce_size[i] = columns_itorque_size[i] = columns_jtorque_size[i] = 0;
	}
}

void od_force::re_eval_info() {
	ostringstream ost;
	od_body* pB;
	info_str.clear();
	ost << "Force with Id: " << id << endl;
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

od_force::~od_force()
{
	unsigned i, j;
	//if (DT) delete DT; DT = 0;
	for (j = 0; j < 3; j++) {
		for (i = 0; i < partial_iforce[j].size(); i++) delete partial_iforce[j][i];
		for (i = 0; i < partial_jforce[j].size(); i++)  delete partial_jforce[j][i];
		for (i = 0; i < partial_itorque[j].size(); i++) delete partial_itorque[j][i];
		for (i = 0; i < partial_jtorque[j].size(); i++)  delete partial_jtorque[j][i];
	}
	for (i = 0; i < parRi.size(); i++) delete parRi[i];
	for (i = 0; i < parRj.size(); i++) delete parRj[i];
	DELARY(parRiparq); DELARY(parRjparq); //DELARY(partialDirection);
	for (i = 0; i < 3; i++) {
		DELARY(partialIforce[i]);
		DELARY(partialJforce[i]);
		DELARY(partialItorque[i]);
		DELARY(partialJtorque[i]);
	}
}

/*void od_force::set_system(od_systemGeneric *psys)
{
	pSys = psys;
}*/

void od_force::init_force() {
	//initialization involves allocation of the space for partial derivative for dcosx_dq 
	//if necessary
	int i, j;// , col;
	Vec3 *pV = 0;
	od_body *pB;
	int b_index;
	int num_j;
	int tempInt;
	Ri = Rj = 0;

	num_j = pSys->tree_dofs();
	if (i_marker) Ri = i_marker->absolute_rel_pos();
	if (j_marker) Rj = j_marker->absolute_rel_pos();
	switch (force_type) {
	case SFORCE:
	case SPDPT:
	case BEAM:
	case BUSHING:
		DT = new od_measure(pSys, od_measure::DT, i_marker->get_id(), j_marker->get_id());
		DT->init();
		//for i marker: torque
		pB = i_marker->get_body();
		if (pB->ground_body() == 0) {
			b_index = pB->get_index();
			// num_j = pSys->tree_dofs();
			tempInt = b_index * num_j;
			for (i = 0; i < num_j; i++) {
			//	if (pSys->relevenceLevel2[tempInt + i] != 0) {
				if (pSys->getL2(tempInt + i) != 0) {
					columns_parRi.push_back(i);
					parRi.push_back(pV);
				}
			}
		}
		columns_parRi_size = (int)columns_parRi.size();
		parRiparq = new od_colVec[columns_parRi_size];
		for (i = 0; i < columns_parRi_size; i++) {
			parRiparq[i].setCol(columns_parRi[i]);
			//	parRiparq[i].setRow(b_index);
		}
		//for j marker: torque
		pB = j_marker->get_body();
		b_index = pB->get_index();
		if (pB->ground_body() == 0) {
			//  num_j = pSys->tree_dofs();
			tempInt = b_index * num_j;
			for (i = 0; i < num_j; i++) {
				//if (pSys->relevenceLevel2[tempInt + i] != 0) {
				if (pSys->getL2(tempInt + i) != 0) {
					columns_parRj.push_back(i);
					parRj.push_back(pV);
				}
			}
		}
		columns_parRj_size = (int)columns_parRj.size();
		if (columns_parRj_size) {
			parRjparq = new od_colVec[columns_parRj_size];
			for (i = 0; i < columns_parRj_size; i++) {
				parRjparq[i].setCol(columns_parRj[i]);
				//	parRjparq[i].setRow(b_index);
			}
		}
		break;
	case STORQUE:
	case SPDPR:
		b_index = i_marker->get_body()->get_index();
		pB = i_marker->get_body();
		if (pB->ground_body() == 0) {
			//  num_j = pSys->tree_dofs();
			tempInt = b_index * num_j;
			for (j = 0; j < num_j; j++) {
			//	if (pSys->relevenceLevel2[tempInt + j] != 0) {
				if (pSys->getL2(tempInt + j) != 0) {
					columns_itorque[0].push_back(j);
					pV = new Vec3();
					partial_itorque[0].push_back(pV);
				}
			}
		}
		b_index = j_marker->get_body()->get_index();
		pB = j_marker->get_body();
		if (pB->ground_body() == 0) {
			//  num_j = pSys->tree_dofs();
			tempInt = b_index * num_j;
			for (j = 0; j < num_j; j++) {
				//if (pSys->relevenceLevel2[tempInt + j] != 0) {
				if (pSys->getL2(tempInt + j) != 0) {
					columns_jtorque[0].push_back(j);
					pV = new Vec3();
					partial_jtorque[0].push_back(pV);
				}
			}
		}
		break;
	default: break;
	}
}

void od_force::eval_force(int _partial) {
	if (!_partial) return;
	int i, j;
	int b_index, num_j, tempInt;
	//double len;
	double xyz[3], *pd;
	//double len2;
	//double len3, temp;
	od_body *pB;
	Vec3 *pV;
	switch (force_type) {
	case SFORCE:
	case SPDPT:
	case BEAM:
	case BUSHING:

		//calculate ek x Ri
		pB = i_marker->get_body();
		if (pB->ground_body() == 0) {
			b_index = pB->get_index();
			num_j = pSys->tree_dofs();
			tempInt = b_index * num_j;
			for (i = 0; i < columns_parRi_size; i++) {
				j = parRiparq[i].col();
				pV = pSys->getJR(b_index, j);// .element(b_index, j);
				if (pV) {
					pd = pV->v;
					CROSS_X(pd, Ri, xyz);
					parRiparq[i].setVals(xyz);
				}
			}
		}
		pB = j_marker->get_body();
		if (pB->ground_body() == 0) {
			b_index = pB->get_index();
			tempInt = b_index * num_j;
			//size_ = columns_parRj.size();
			for (i = 0; i < columns_parRj_size; i++) {
				j = parRjparq[i].col();
				pV = pSys->getJR(b_index, j);//JR.element(b_index, j);
				pd = pV->v;
				CROSS_X(pd, Rj, xyz);
				parRjparq[i].setVals(xyz);
			}
		}

		break;
	case STORQUE:
	case SPDPR:

		pB = i_marker->get_body();
		if (pB->ground_body() == 0) {
			b_index = i_marker->get_body()->get_index();
			for (i = 0; i < (int)columns_itorque[0].size(); i++) {
				pV = pSys->getJR(b_index, columns_itorque[0][i]);//JR.element(b_index, columns_itorque[0][i]);
				if (pV) *(partial_itorque[0][i]) = *pV;
			}
		}
		pB = j_marker->get_body();
		if (pB->ground_body() == 0) {
			b_index = j_marker->get_body()->get_index();
			for (i = 0; i < (int)columns_jtorque[0].size(); i++) {
				pV = pSys->getJR(b_index, columns_jtorque[0][i]); //JR.element(b_index, columns_jtorque[0][i]);
				if (pV) *(partial_jtorque[0][i]) = *pV;
			}
		}

		break;
	default: break;
	}
}

void od_force::eval_partial_RiRj(int counter) {
	int i, tempInt;
	double *pd, tempd3[3];
	for (i = 0; i < columns_parRi_size; i++) {
		//par_Ti += par_Ri * Fi
		tempInt = i + counter;
		//cross_product(*parRi[i], iForce, *partial_itorque[0][tempInt]);
		pd = parRiparq[i].v();
		CROSS_X(pd, iForce, tempd3);
		partialItorque[0][tempInt].setVals(tempd3);

	}
	//_size = columns_parRj.size();
	for (i = 0; i < columns_parRj_size; i++) {
		//par_Tj += par_Rj * Fj
		tempInt = i + counter;
		//cross_product(*parRj[i], jForce, *partial_jtorque[0][tempInt]);
		pd = parRjparq[i].v();
		CROSS_X(pd, jForce, tempd3);
		partialJtorque[0][tempInt].setVals(tempd3);
	}
}

od_storque::od_storque(int Id, char* name, od_marker* I, od_marker* J, char* _expr) : od_force(Id, name, I, J) {
	if (_expr) pExpr = new Expression(_expr, pSys);
	force_type = STORQUE;
}

int od_storque::init() {
	unsigned i;
	Vec3 *pV;
	init_force();
	iCount = (int)columns_itorque[0].size();
	jCount = (int)columns_jtorque[0].size();
	if (pSys) pExpr->set_system(pSys);
	pExpr->init();
	for (int j = 0; j < 3; j++) {
		for (i = 0; i < (unsigned)pExpr->get_partial_size(j); i++) {
			pV = new Vec3();
			columns_itorque[j].push_back(pExpr->get_partial_col(j, i));
			partial_itorque[j].push_back(pV);
			pV = new Vec3();
			columns_jtorque[j].push_back(pExpr->get_partial_col(j, i));
			partial_jtorque[j].push_back(pV);
		}
	}
	return 1;
}

void od_storque::evaluate(int _partial) {
	if (is_evaluated()) return;
	unsigned i, j;
	double temp_d;
	double *pd;
	Vec3 vecTemp;
	double xyz[3];
	eval_force(_partial);
	temp_d = pExpr->eval(_partial);
	vecTemp = j_marker->get_axis_global(vecTemp);
	vecTemp.multiplied_by(temp_d);
	pd = vecTemp.v;
	EQ3(iTorque, pd) EQ3(jTorque, pd) NEG(jTorque)
		if (_partial) {
			pd = xyz;
			for (i = 0; i < (unsigned)iCount; i++) {
				(partial_itorque[0][i])->multiplied_by(temp_d);
			}
			for (i = 0; i < (unsigned)jCount; i++) {
				(partial_jtorque[0][i])->multiplied_by(-temp_d);
			}
			vecTemp = j_marker->get_axis_global(vecTemp);
			for (j = 0; j < 3; j++) {
				for (i = 0; i < (unsigned)pExpr->get_partial_size(j); i++) {
					temp_d = pExpr->get_partial_col(j, i);
					pd = vecTemp.VxD(pd, temp_d, 1);
					if (j == 0) {
						*partial_itorque[j][iCount + i] = pd;
						*partial_jtorque[j][jCount + j] = pd; partial_jtorque[0][j]->negate();
					}
					else {
						*partial_itorque[j][i] = pd;
						*partial_jtorque[j][i] = pd; partial_jtorque[0][j]->negate();
					}
				}
			}
		}
	evaluated();
}

od_sforce::od_sforce(int Id, char* name, od_marker* I, od_marker* J, char* _expr) : od_single_force(Id, name, I, J) {
	if (_expr) pExpr = new Expression(_expr, pSys);
	force_type = SFORCE;
}

int od_sforce::init() {
	int i, j, _size;
	Vec3 *pV;
	init_force();
	init_single_force();
	if (pSys && pExpr) {
		pExpr->set_system(pSys);
		pExpr->init();
	}
	//columns_fdirection_size =columns_fdirection.size();

	for (i = 0; i < columns_fdirection_size; i++) {
		//j =  columns_fdirection[i];
		j=partialDirection[i].col();
		columns_iforce[0].push_back(j);
		columns_jforce[0].push_back(j);
		pV = new Vec3();
		partial_iforce[0].push_back(pV);
		pV = new Vec3();
		partial_jforce[0].push_back(pV);

		columns_itorque[0].push_back(j);
		columns_jtorque[0].push_back(j);
		pV = new Vec3();
		partial_itorque[0].push_back(pV);
		pV = new Vec3();
		partial_jtorque[0].push_back(pV);
	}
	for (int jj = 0; jj < 3; jj++) {
		_size = (unsigned)pExpr->get_partial_size(jj);
		for (i = 0; i < _size; i++) {
			j = pExpr->get_partial_col(jj, i);
			columns_iforce[jj].push_back(j);
			columns_jforce[jj].push_back(j);
			pV = new Vec3();
			partial_iforce[jj].push_back(pV);
			pV = new Vec3();
			partial_jforce[jj].push_back(pV);

			columns_itorque[jj].push_back(j);
			columns_jtorque[jj].push_back(j);
			pV = new Vec3();
			partial_itorque[jj].push_back(pV);
			pV = new Vec3();
			partial_jtorque[jj].push_back(pV);
		}
	}
	_size = (int)columns_parRi.size();
	for (i = 0; i < _size; i++) {
		pV = new Vec3();
		columns_itorque[0].push_back(columns_parRi[i]);
		partial_itorque[0].push_back(pV);
	}
	_size = (int)columns_parRj.size();
	for (i = 0; i < _size; i++) {
		pV = new Vec3();
		columns_jtorque[0].push_back(columns_parRj[i]);
		partial_jtorque[0].push_back(pV);
	}
	return 1;
}

void od_sforce::evaluate(int _partial) {
	if (is_evaluated()) return;
	unsigned i, j, tempInt, counter = 0;
	double temp_d;
	Vec3 vecTemp;
	double xyz[3];
	double* pd; // = xyz;
	eval_force(_partial);
	eval_single_force(_partial);
	temp_d = pExpr->eval(_partial);
	cos_q.multiplied_by(temp_d);
	pd = cos_q.v;
	EQ3(iForce, pd);//iForce = pd;
	EQ3(jForce, pd); NEG(jForce);
	CROSS_X(Ri, iForce, iTorque);
	CROSS_X(Rj, jForce, jTorque);
	if (_partial) {
		//contribute from cos part
		tempInt = columns_fdirection_size;// columns_fdirection.size();
		for (i = 0; i < tempInt; i++) {
			pd = partialDirection[i].v(); EQ3(xyz, pd);
			pd = xyz;
			for (j = 0; j < 3; j++) pd[j] *= temp_d;
			// par_Fi = par_cos * val
			if (iNotGround) *partial_iforce[0][i] = pd;
			if (jNotGround) {
				*partial_jforce[0][i] = pd;
				partial_jforce[0][i]->negate();
			}
			//par_Ti = Ri * (par_cos *v)
			if (iNotGround) cross_product_with_doubles(Ri, pd, *partial_itorque[0][i]);
			if (jNotGround) {
				cross_product_with_doubles(Rj, pd, *partial_jtorque[0][i]);
				partial_jtorque[0][i]->negate();
			}
		}
		counter = tempInt;
		tempInt = (int)pExpr->get_partial_size();
		for (i = 0; i < tempInt; i++) {
			temp_d = pExpr->get_partial(i);
			pd = cos_q.VxD(pd, temp_d);
			//par_Fi += (cos*par_val)
			if (iNotGround) *partial_iforce[0][i + counter] = pd;
			if (jNotGround) {
				*partial_jforce[0][i + counter] = pd;
				partial_jforce[0][i + counter]->negate();
			}
			//par_Ti += Ri*(cos*par_val)
			if (iNotGround) cross_product_with_doubles(Ri, pd, *partial_itorque[0][i + counter]);
			if (jNotGround) {
				cross_product_with_doubles(Rj, pd, *partial_jtorque[0][i + counter]);
				partial_jtorque[0][i + counter]->negate();
			}
		}
		counter += tempInt;
		tempInt = (int)columns_parRi.size();
		for (i = 0; i < tempInt; i++) {
			//par_Ti += par_Ri * Fi
			j = i + counter;
			cross_product_with_double(*parRi[i], iForce, *partial_itorque[0][j]);
		}
		tempInt = (int)columns_parRj.size();
		for (i = 0; i < tempInt; i++) {
			//par_Tj += par_Rj * Fj
			j = i + counter;
			cross_product_with_double(*parRj[i], jForce, *partial_jtorque[0][j]);
		}
	}
	evaluated();
}


od_spdpt::od_spdpt(int Id, char* name_, od_marker* I, od_marker* J, double k,
	double c, double d, double f) : od_single_force(Id, name_, I, J)
{
	K = k; C = c; distance = d; force = f;
	force_type = SPDPT; VT = 0;
}

od_spdpt::~od_spdpt() {/*if(VT) delete VT;*/ }

int od_spdpt::init() {
	int i, j, _size;
	Vec3 *pV = 0;
	od_force::init_force();
	od_single_force::init_single_force();
	if (C > 0.0) {
		VT = new od_measure(pSys, od_measure::VT, i_marker, j_marker);
		VT->init();
	}
    //F = -K(DT-distance*e)-e*CDV*e + force*e
	//parF_parE
	for (i = 0; i < columns_fdirection_size; i++) {
		//j = columns_fdirection[i];
		j = partialDirection[i].col();
		if (iNotGround) {
			columns_iforce[0].push_back(j);
			columns_itorque[0].push_back(j);
		}
		if (jNotGround) {
			columns_jforce[0].push_back(j);
			columns_jtorque[0].push_back(j);
		}
	}

	//parF_parDT
	_size = DT->get_partial_size();
	for (i = 0; i < _size; i++) {
		j = DT->get_partial_col(0, i);
		if (iNotGround) {
			columns_iforce[0].push_back(j);
			columns_itorque[0].push_back(j);
		}
		if (jNotGround) {
			columns_jforce[0].push_back(j);
			columns_jtorque[0].push_back(j);
		}
	}
	//velocity variable will be added later to list
	//ParF_parDV
	if (C > 0.0) {
		for (int jj = 0; jj < 2; jj++) {
			_size = VT->get_partial_size(jj);
			for (i = 0; i < _size; i++) {
				j = VT->get_partial_col(jj, i);
				//wrt position
				if (iNotGround) {
					columns_iforce[jj].push_back(j);
					columns_itorque[jj].push_back(j);
				}

				if (jNotGround) {
					columns_jforce[jj].push_back(j);
					columns_jtorque[jj].push_back(j);
				}
			}
		}
	}
	//_size = columns_parRi.size();
	for (i = 0; i < columns_parRi_size; i++) {
		columns_itorque[0].push_back(columns_parRi[i]);
	}
	//_size = columns_parRj.size();
	for (i = 0; i < columns_parRj_size; i++) {
		columns_jtorque[0].push_back(columns_parRj[i]);
	}
	for (i = 0; i < 3; i++) {
		columns_iforce_size[i] = (int)columns_iforce[i].size();
		if (0 != columns_iforce_size[i]) {
			partialIforce[i] = new od_colVec[columns_iforce_size[i]];
			for (j = 0; j < columns_iforce_size[i]; j++) {
				partialIforce[i][j].setCol(columns_iforce[i][j]);
			}
		}
		columns_jforce_size[i] = (int)columns_jforce[i].size();
		if (0 != columns_jforce_size[i]) {
			partialJforce[i] = new od_colVec[columns_jforce_size[i]];
			for (j = 0; j < columns_jforce_size[i]; j++) {
				partialJforce[i][j].setCol(columns_jforce[i][j]);
			}
		}
		columns_itorque_size[i] = (int)columns_itorque[i].size();
		if (0 != columns_itorque_size[i]) {
			partialItorque[i] = new od_colVec[columns_itorque_size[i]];
			for (j = 0; j < columns_itorque_size[i]; j++) {
				partialItorque[i][j].setCol(columns_itorque[i][j]);
			}
		}
		columns_jtorque_size[i] = (int)columns_jtorque[i].size();
		if (0 != columns_jtorque_size[i]) {
			partialJtorque[i] = new od_colVec[columns_jtorque_size[i]];
			for (j = 0; j < columns_jtorque_size[i]; j++) {
				partialJtorque[i][j].setCol(columns_jtorque[i][j]);
			}
		}
	}
	return 1;
}

double od_spdpt::getPE() {
	double* vecTemp = DT->get_val();
	double mag = MAG(vecTemp);
	mag -= distance;
	mag *= mag;
	return .5*mag*K;
}

void od_spdpt::evaluate(int _partial) {
	int i, j, tempInt, _size, counter = 0;
	//double *pV;
	double xyz[3], tempd3[3], tempd, *dt, *dv, e_sth, de_dv, e_dv;
	double* pd = xyz;
	double* pe=0;
	Vec3 *pV3;
	dv = 0;
	if (!is_evaluated() || _partial) {
        //F = -K(DT-distance*e)-e*CDV*e + force*e
		eval_force(_partial);
		eval_single_force(_partial);
		dt = DT->get_val();
        EQ3(xyz, dt); //DT
		cos_q.VxD(xyz, -distance);  //DT-dist*e
		MULT_S(xyz, -K);                    //-K(DT-dist*e)
        //ZERO3(tempd3); 
		cos_q.VxD(tempd3, force, 1);    //force*e
		U_ADD3(xyz, tempd3);                 //+force*e 
		pe = cos_q.v;
        dv=0;
		if (C > 0.00 && pSys->get_analysis_type() == DYNAMIC) {
			VT->eval(_partial);
			dv = VT->get_val(); 
			tempd = (-C)*DOT_X(pe, dv);  //-C e* DV
            cos_q.VxD(xyz, tempd); // -C e*dv *e
		}
		if (iNotGround) {
			EQ3(iForce, xyz);
			CROSS_X(Ri, xyz, iTorque);
		}
		if (!jNotGround) {
			ZERO3(jForce); ZERO3(jTorque);
		} else {
			NEG(xyz);
			EQ3(jForce, xyz);
			CROSS_X(Rj, xyz, jTorque);
		}
	}
	if (_partial) {
		//contribute from cos part
		for (i = 0; i < columns_fdirection_size; i++) {
        //F = -K(DT-distance*e)-e*CDV*e + force*e
        //   (K*distance+force)*de-de*CDV*e-e*CDV*de
            tempd=K*distance+force;  //only K
			pV3 = partialDirection[i].vec();
			pV3->VxD(xyz, tempd, 1);
            if (C>0.0 && dv!=0) {
				de_dv = -C*((*pV3)*dv); //-de*DV*C
		        e_dv = -C*(DOT_X(pe, dv)); //-e*DV*C
			    for (j = 0; j < 3; j++) { 
                    xyz[j]+= (de_dv*pe[j] + e_dv*(pV3->v[j]));
                }
            }
			// par_Fi = par_cos * val
			if (iNotGround) {
				partialIforce[0][i].setVals(xyz);
				CROSS_X(Ri, xyz, tempd3);
				partialItorque[0][i].setVals(tempd3);
			}
			if (jNotGround) {
				NEG(xyz);
				partialJforce[0][i].setVals(xyz);
				CROSS_X(Rj, xyz, tempd3);
				partialJtorque[0][i].setVals(tempd3);
			}
		}
		counter = columns_fdirection_size;
		//parF_parDT
		_size = DT->get_partial_size();
		for (i = 0; i < _size; i++) {
        //F = -K(DT-distance*e)-e*CDV*e + force*e
        //  -Kdt_dq
			tempInt = counter + i;
			pV3 = DT->get_partial(0, i);// ->v;

			pV3->VxD(xyz, -K, 1);
			if (iNotGround) {
				partialIforce[0][tempInt].setVals(xyz);
				CROSS_X(Ri, xyz, tempd3);
				partialItorque[0][tempInt].setVals(tempd3);
			}
			if (jNotGround) {
				NEG(xyz);
				partialJforce[0][tempInt].setVals(xyz);
				CROSS_X(Rj, xyz, tempd3);
				partialJtorque[0][tempInt].setVals(tempd3);
			}
		}
		if (C > 0.0) {
			counter += _size;
			_size = VT->get_partial_size();
			if (pSys->get_analysis_type() == DYNAMIC) {
        //F = -K(DT-distance*e)-e*CDV*e + force*e
        // -e*C *dv_dq *e
				for (i = 0; i < _size; i++) {
					tempInt = counter + i;
					pV3 = VT->get_partial(0, i);// ->v;
					e_sth = -C * ((*pV3)*pe); //ZERO3(xyz);
					cos_q.VxD(xyz, e_sth, 1);
					if (iNotGround) {
						partialIforce[0][tempInt].setVals(xyz);
						CROSS_X(Ri, xyz, tempd3);
						partialItorque[0][tempInt].setVals(tempd3);
					}
					if (jNotGround) {
						NEG(xyz);
						partialJforce[0][tempInt].setVals(xyz);
						CROSS_X(Rj, xyz, tempd3);
						partialJtorque[0][tempInt].setVals(tempd3);
					}
				}
			}
			else {
				for (i = 0; i < _size; i++) {
					tempInt = counter + i;
					if (iNotGround) {
						partialIforce[0][tempInt].init();
						partialItorque[0][tempInt].init();
					}
					if (jNotGround) {
						partialJforce[0][tempInt].init();
						partialJtorque[0][tempInt].init();
					}
				}
			}
			if (pSys->get_analysis_type() == DYNAMIC) {
        //F = -K(DT-distance*e)-e*CDV*e + force*e
        //// -C e*DV_dq_dot*e
				for (i = 0; i < VT->get_partial_size(1); i++) {
					pV3 = VT->get_partial(1, i);// ->v;
					e_sth = -C * ((*pV3)*pe); //ZERO3(xyz);
					cos_q.VxD(xyz, e_sth, 1);
			    
					if (iNotGround) {
						partialIforce[1][i].setVals(xyz);
						CROSS_X(Ri, xyz, tempd3);
						partialItorque[1][i].setVals(tempd3);
					}
					if (jNotGround) {
						NEG(xyz);
						partialJforce[1][i].setVals(xyz);
						CROSS_X(Rj, xyz, tempd3);
						partialJtorque[1][i].setVals(tempd3);
					}
				}
			} else {
				for (i = 0; i < VT->get_partial_size(1); i++) {
					if (iNotGround) {
						partialIforce[1][i].init();
						partialItorque[1][i].init();
					}
					if (jNotGround) {
						partialJforce[1][i].init();
						partialJtorque[1][i].init();
					}
				}
			}
		}
		counter += _size;
		//_size = columns_parRi.size();
		for (i = 0; i < columns_parRi_size; i++) {
			//par_Ti += par_Ri * Fi
			tempInt = i + counter;
			pd = parRiparq[i].v();
			CROSS_X(pd, iForce, tempd3);
			partialItorque[0][tempInt].setVals(tempd3);

		}
		//_size = columns_parRj.size();
		for (i = 0; i < columns_parRj_size; i++) {
			//par_Tj += par_Rj * Fj
			tempInt = i + counter;
			pd = parRjparq[i].v();
			CROSS_X(pd, jForce, tempd3);
			partialJtorque[0][tempInt].setVals(tempd3);
		}
	}
	evaluated();
}

od_beam::od_beam(int Id, char* _name, od_marker* pI, od_marker* pJ, double l, double ixx, double iyy, double izz,
	double area, double e, double g, double r, double asy, double asz)
	: od_force(Id, _name, pI, pJ) {
	length = l;
	Ixx = ixx; Iyy = iyy; Izz = izz; Area = area; emodulus = e; gmodulus = g; Asy = asy; Asz = asz;
	ratio = r;
	damp = 1;
	force_type = BEAM;
	DDT = VT = DR = VR = 0;
	parFparq = parTparq = parFparq_dot = parTparq_dot = 0;
	parFjparq = parTjparq = parFjparq_dot = parTjparq_dot = 0;
	if (ratio > SMALL_VALUE) damp = 1; else damp = 0;
}

od_beam::od_beam(int Id, char* _name, od_marker* pI, od_marker* pJ, double l, double ixx, double iyy, double izz,
	double area, double e, double g, double** _C, double asy, double asz)
	: od_force(Id, _name, pI, pJ) {
	length = l;
	Ixx = ixx; Iyy = iyy; Izz = izz; Area = area; emodulus = e; gmodulus = g; Asy = asy; Asz = asz;
	ratio = 0.0;
	damp = 0;
	force_type = BEAM;
	DDT = VT = DR = VR = 0;
	parFparq = parTparq = parFparq_dot = parTparq_dot = 0;
	parFjparq = parTjparq = parFjparq_dot = parTjparq_dot = 0;
}

od_beam::~od_beam() {
	DELARY(parTparq) DELARY(parFparq) DELARY(parTparq_dot) DELARY(parFparq_dot)
		DELARY(parTjparq) DELARY(parFjparq) DELARY(parTjparq_dot) DELARY(parFjparq_dot)
}

int od_beam::init() {
	vector<int>  cols;
	int ii, i, j, tempInt, _size, b_index;
	double l2 = length * length;
	double l3 = l2 * length;
	double py = 12.0*Izz*emodulus*Asy / (gmodulus*Area*l2);
	double pz = 12.0*Iyy*emodulus*Asz / (gmodulus*Area*l2);
	int num_j = pSys->tree_dofs();
	od_body* bodys[2] = { i_marker->get_body(), j_marker->get_body() };
	init_force();
	for (j = 0; j < 2; j++) {
		if (bodys[j]->ground_body() == 0) {
			b_index = bodys[j]->get_index();
			tempInt = b_index * num_j;
			for (i = 0; i < num_j; i++) {
				//if (pSys->relevenceLevel2[tempInt + i] != 0) {
				if(pSys->getL2(tempInt + i) !=0 ) {
					if (!j) iIds.push_back(i);
					else jIds.push_back(i);
				}
			}
		}
	}
	k00 = emodulus * Area / length;                    c00 = k00 * ratio;
	k11 = 12.0*emodulus*Izz / (l3*(1.0 + py));         c11 = k11 * ratio;
	k22 = 12.0*emodulus*Iyy / (l3*(1.0 + pz));         c22 = k22 * ratio;
	k33 = gmodulus * Ixx / length;                     c33 = k33 * ratio;
	k44 = (4.0 + pz)*emodulus*Iyy / (length*(1 + pz));   c44 = k44 * ratio;
	k55 = (4.0 + py)*emodulus*Izz / (length*(1 + py));   c55 = k55 * ratio;
	k24 = 6.0*emodulus*Iyy / (l2*(1 + pz));            c24 = k24 * ratio;
	k15 = 6.0*emodulus*Izz / (l2*(1 + py));            c15 = k15 * ratio;
	kj44 = (2.0 - pz)*emodulus*Iyy / (length*(1 + pz));  cj44 = kj44 * ratio;
	kj55 = (2.0 - py)*emodulus*Izz / (length*(1 + py));  cj55 = kj55 * ratio;

	DDT = new od_measure(pSys, od_measure::DT, i_marker, j_marker, j_marker);
	DDT->init();
	DR = new od_measure(pSys, od_measure::DR, i_marker, j_marker, j_marker);
	DR->init();
	/* Fi = Aj F     Ti = AjT + Rii x AjF
	Fj = -Fi      Tj = -AiT - ((DT(i,j)+Rjj) x AjF)
	*/
	//DDT, VT, DR and VR has the same number of partial values with the same pattern
	//parDDTparq
	cols.resize(0);
	_size = DDT->get_partial_size(0);
	parFTparq_size = _size;
	for (i = 0; i < _size; i++) {
		j = DDT->get_partial_col(0, i);
		cols.push_back(j);
	}
	//parAjparq
	_size = (int)jIds.size();
	parFTparq_size += _size;
	for (i = 0; i < _size; i++) {
		j = jIds[i];
		cols.push_back(j);
	}

	parFparq = new od_colVec[parFTparq_size];
	parTparq = new od_colVec[parFTparq_size];
	parFjparq = new od_colVec[parFTparq_size];
	parTjparq = new od_colVec[parFTparq_size];
	for (i = 0; i < parFTparq_size; i++) {
		j = cols[i];
		parFparq[i].setCol(j);
		parTparq[i].setCol(j);
		parFjparq[i].setCol(j);
		parTjparq[i].setCol(j);
		columns_iforce[0].push_back(j);
		columns_jforce[0].push_back(j);
		columns_itorque[0].push_back(j);
		columns_jtorque[0].push_back(j);
	}
	parFTparq_dot_size = 0;
	if (damp) {
		VT = new od_measure(pSys, od_measure::VT, i_marker, j_marker, j_marker);
		VT->init();
		VR = new od_measure(pSys, od_measure::VR, i_marker, j_marker, j_marker);
		VR->init();
		cols.resize(0);
		//parDRparq_dot
		_size = VT->get_partial_size(1);
		parFTparq_dot_size = _size;
		for (i = 0; i < _size; i++) {
			j = VT->get_partial_col(1, i);
			cols.push_back(j);
		}
		parFparq_dot = new od_colVec[parFTparq_dot_size];
		parTparq_dot = new od_colVec[parFTparq_dot_size];
		parFjparq_dot = new od_colVec[parFTparq_dot_size];
		parTjparq_dot = new od_colVec[parFTparq_dot_size];
		for (i = 0; i < parFTparq_dot_size; i++) {
			j = cols[i];
			parFparq_dot[i].setCol(j);
			parTparq_dot[i].setCol(j);
			parFjparq_dot[i].setCol(j);
			parTjparq_dot[i].setCol(j);
			columns_iforce[1].push_back(j);
			columns_jforce[1].push_back(j);
			columns_itorque[1].push_back(j);
			columns_jtorque[1].push_back(j);
		}
	}

	// partial of AjFi
	//partial of Ri x (AjTi)
	_size = (int)iIds.size();
	for (i = 0; i < _size; i++) {
		columns_itorque[0].push_back(iIds[i]);
	}
	//partial of Rj x (AjTi)
	_size = (int)jIds.size();
	for (i = 0; i < _size; i++) {
		columns_jtorque[0].push_back(jIds[i]);
	}
	//allocate memory for partialIforce
	for (ii = 0; ii < 2; ii++) {
		columns_iforce_size[ii] = (int)columns_iforce[ii].size();
		partialIforce[ii] = new od_colVec[columns_iforce_size[ii]];
		for (i = 0; i < columns_iforce_size[ii]; i++) {
			j = columns_iforce[ii][i];
			partialIforce[ii][i].setCol(j);
		}
		columns_jforce_size[ii] = (int)columns_jforce[ii].size();
		partialJforce[ii] = new od_colVec[columns_jforce_size[ii]];
		for (i = 0; i < columns_jforce_size[ii]; i++) {
			j = columns_jforce[ii][i];
			partialJforce[ii][i].setCol(j);
		}
		columns_itorque_size[ii] = (int)columns_itorque[ii].size();
		partialItorque[ii] = new od_colVec[columns_itorque_size[ii]];
		for (i = 0; i < columns_itorque_size[ii]; i++) {
			j = columns_itorque[ii][i];
			partialItorque[ii][i].setCol(j);
		}
		columns_jtorque_size[ii] = (int)columns_jtorque[ii].size();
		partialJtorque[ii] = new od_colVec[columns_jtorque_size[ii]];
		for (i = 0; i < columns_jtorque_size[ii]; i++) {
			j = columns_jtorque[ii][i];
			partialJtorque[ii][i].setCol(j);
		}
	}
	return 0;
}

void od_beam::evaluate(int partial) {
	int i, j, col, lastCount;
	double vecTemp[3];
	double tempD[6], tempDi[6], *pd, *pV, tempDj[6];
	double* tempD3 = tempD + 3;
	double pureFi[3];
	double pureFj[3];
	double pureTi[3];
	double pureTj[3];
	double v11, v22, v33, v44, v55, v00, v51, v15, v24, v42, vj44, vj55;
	eval_force(partial);
	DDT->eval(partial);
	DR->eval(partial);
	pd = DDT->get_val(); EQ3(tempD, pd); tempD[0] -= length;
	pd = DR->get_val();  EQ3(tempD3, pd);
	v00 = k00 * tempD[0]; v11 = k11 * tempD[1]; v22 = k22 * tempD[2];
	v33 = k33 * tempD[3]; v44 = k44 * tempD[4]; v55 = k55 * tempD[5];
	vj44 = kj44 * tempD[4]; vj55 = kj55 * tempD[5];
	v15 = k15 * tempD[5]; v51 = k15 * tempD[1];
	v24 = k24 * tempD[4]; v42 = k24 * tempD[2];
	pureFi[0] = -v00; pureFi[1] = -v11 + v15; pureFi[2] = -v22 - v24;
	pureTi[0] = -v33; pureTi[1] = -v44 - v42; pureTi[2] = -v55 + v51;
	pureFj[0] = v00;  pureFj[1] = v11 - v15;  pureFj[2] = v22 + v24;
	pureTj[0] = v33;  pureTj[1] = -vj44 - v42; pureTj[2] = -vj55 + v51;
	if (pSys->get_analysis_type() == DYNAMIC && damp) {
		VT->eval(partial);
		VR->eval(partial);
		pd = VT->get_val();
		EQ3(tempD, pd);
		pd = VR->get_val();
		EQ3(tempD3, pd);
		v00 = c00 * tempD[0]; v11 = c11 * tempD[1]; v22 = c22 * tempD[2];
		v33 = c33 * tempD[3]; v44 = c44 * tempD[4]; v55 = c55 * tempD[5];
		vj44 = cj44 * tempD[4]; vj55 = cj55 * tempD[5];
		v15 = c15 * tempD[5]; v51 = c15 * tempD[1];
		v24 = c24 * tempD[4]; v42 = c24 * tempD[2];
		pureFi[0] += -v00; pureFi[1] += -v11 + v15;  pureFi[2] += -v22 - v24;
		pureTi[0] += -v33; pureTi[1] += -v44 - v42;  pureTi[2] += -v55 + v51;
		pureFj[0] += v00;  pureFj[1] += v11 - v15;   pureFj[2] += v22 + v24;
		pureTj[0] += v33;  pureTj[1] += -vj44 - v42; pureTj[2] += -vj55 + v51;
	}
	EQ3(iForce, pureFi) EQ3(iTorque, pureTi) j_marker->vec_wrt_ground(iForce); j_marker->vec_wrt_ground(iTorque);
	EQ3(jForce, pureFj) EQ3(jTorque, pureTj) j_marker->vec_wrt_ground(jForce); j_marker->vec_wrt_ground(jTorque);
	CROSS_X(Ri, iForce, vecTemp) U_ADD3(iTorque, vecTemp) CROSS_X(Rj, jForce, vecTemp) U_ADD3(jTorque, vecTemp);
	if (partial) {
		lastCount = 0;
		int _size = DDT->get_partial_size(0);
		for (i = 0; i < (int)_size; i++) {
			pd = DDT->get_partial(0, i)->v;
			EQ3(tempD, pd);
			pd = DR->get_partial(0, i)->v;
			EQ3(tempD3, pd);
			v00 = k00 * tempD[0]; v11 = k11 * tempD[1]; v22 = k22 * tempD[2];
			v33 = k33 * tempD[3]; v44 = k44 * tempD[4]; v55 = k55 * tempD[5];
			vj44 = kj44 * tempD[4]; vj55 = kj55 * tempD[5];
			v15 = k15 * tempD[5]; v51 = k15 * tempD[1];
			v24 = k24 * tempD[4]; v42 = k24 * tempD[2];
			tempDi[0] = -v00; tempDi[1] = -v11 + v15;  tempDi[2] = -v22 - v24;
			tempDi[3] = -v33; tempDi[4] = -v44 - v42;  tempDi[5] = -v55 + v51;
			tempDj[0] = v00;  tempDj[1] = v11 - v15;   tempDj[2] = v22 + v24;
			tempDj[3] = v33;  tempDj[4] = -vj44 - v42; tempDj[5] = -vj55 + v51;
			if (pSys->get_analysis_type() == DYNAMIC && damp) {
				pd = VT->get_partial(0, i)->v; EQ3(tempD, pd);
				pd = VR->get_partial(0, i)->v; EQ3(tempD3, pd);
				v00 = c00 * tempD[0]; v11 = c11 * tempD[1]; v22 = c22 * tempD[2];
				v33 = c33 * tempD[3]; v44 = c44 * tempD[4]; v55 = c55 * tempD[5];
				vj44 = cj44 * tempD[4]; vj55 = cj55 * tempD[5];
				v15 = c15 * tempD[5]; v51 = c15 * tempD[1];
				v24 = c24 * tempD[4]; v42 = c24 * tempD[2];
				tempDi[0] += -v00; tempDi[1] += -v11 + v15;  tempDi[2] += -v22 - v24;
				tempDi[3] += -v33; tempDi[4] += -v44 - v42;  tempDi[5] += -v55 + v51;
				tempDj[0] += v00;  tempDj[1] += v11 - v15;   tempDj[2] += v22 + v24;
				tempDj[3] += v33;  tempDj[4] += -vj44 - v42; tempDj[5] += -vj55 + v51;
			}
			j_marker->vec_wrt_ground(tempDi);
			j_marker->vec_wrt_ground(tempDi + 3);
			j_marker->vec_wrt_ground(tempDj);
			j_marker->vec_wrt_ground(tempDj + 3);
			parFparq[i].setVals(tempDi);
			parTparq[i].setVals(tempDi + 3);
			parFjparq[i].setVals(tempDj);
			parTjparq[i].setVals(tempDj + 3);
		}
		lastCount += _size;
		_size = (int)jIds.size();
		//parAj_parq
		for (i = 0; i < (int)_size; i++) {
			j = lastCount + i;
			int b_index = j_marker->get_body()->get_index();
			col = parFparq[j].col();
			pd = pSys->getJR(b_index, col)->v; //JR.element(b_index, col)->v;
			pV = parFparq[j].v();
			CROSS_X(pd, iForce, pV);
			pV = parTparq[j].v();
			CROSS_X(pd, iTorque, pV);
			pV = parFjparq[j].v();
			CROSS_X(pd, jForce, pV);
			pV = parTjparq[j].v();
			CROSS_X(pd, jTorque, pV);
		}
		lastCount += _size;
		for (i = 0; i < lastCount; i++) {
			pd = parFparq[i].v();
			partialIforce[0][i].setVals(pd);

			pV = parTparq[i].v();
			//parTi + Ri x parFi
			CROSS_X(Ri, pd, vecTemp);
			ADD3(pV, vecTemp, tempD);
			partialItorque[0][i].setVals(tempD);

			pd = parFjparq[i].v();
			partialJforce[0][i].setVals(pd);
			//-(parTj + Rj x parFj)
			pV = parTjparq[i].v();
			CROSS_X(Rj, pd, vecTemp)
				ADD3(pV, vecTemp, tempD)
				partialJtorque[0][i].setVals(tempD);
		}
		_size = (int)iIds.size();
		//parRi x Fi
		for (i = 0; i < _size; i++) {
			pd = parRiparq[i].v();
			CROSS_X(pd, iForce, vecTemp) partialItorque[0][lastCount + i].setVals(vecTemp);
		}
		//lastCount +=_size;
		_size = (int)jIds.size();
		//parRj x Fi
		for (i = 0; i < _size; i++) {
			pd = parRjparq[i].v();
			CROSS_X(pd, jForce, vecTemp) partialJtorque[0][lastCount + i].setVals(vecTemp);
		}
		//PAR_PARQ_DOT
		lastCount = 0;
		if (pSys->get_analysis_type() == DYNAMIC && damp) {
			_size = VT->get_partial_size(1);
			for (i = 0; i < (int)_size; i++) {
				pd = VT->get_partial(1, i)->v;
				EQ3(tempD, pd);
				pd = VR->get_partial(1, i)->v;
				EQ3(tempD3, pd);
				v00 = c00 * tempD[0]; v11 = c11 * tempD[1]; v22 = c22 * tempD[2];
				v33 = c33 * tempD[3]; v44 = c44 * tempD[4]; v55 = c55 * tempD[5];
				vj44 = cj44 * tempD[4]; vj55 = cj55 * tempD[5];
				v15 = c15 * tempD[5]; v51 = c15 * tempD[1];
				v24 = c24 * tempD[4]; v42 = c24 * tempD[2];
				tempDi[0] = -v00; tempDi[1] = -v11 + v15;  tempDi[2] = -v22 - v24;
				tempDi[3] = -v33; tempDi[4] = -v44 - v42;  tempDi[5] = -v55 + v51;
				tempDj[0] = v00;  tempDj[1] = v11 - v15;   tempDj[2] = v22 + v24;
				tempDj[3] = v33;  tempDj[4] = -vj44 - v42; tempDj[5] = -vj55 + v51;
				j_marker->vec_wrt_ground(tempDi);
				j_marker->vec_wrt_ground(tempDi + 3);
				j_marker->vec_wrt_ground(tempDj);
				j_marker->vec_wrt_ground(tempDj + 3);
				parFparq_dot[i].setVals(tempDi);
				parTparq_dot[i].setVals(tempDi + 3);
				parFjparq_dot[i].setVals(tempDj);
				parTjparq_dot[i].setVals(tempDj + 3);
			}
			lastCount += _size;
			for (i = 0; i < lastCount; i++) {
				pd = parFparq_dot[i].v();
				partialIforce[1][i].setVals(pd);
				pV = parTparq_dot[i].v();
				//parTi + Ri x parFi
				CROSS_X(Ri, pd, vecTemp);
				ADD3(pV, vecTemp, tempD);
					partialItorque[1][i].setVals(tempD);

				pd = parFjparq_dot[i].v();
				partialJforce[1][i].setVals(pd);
				pV = parTjparq_dot[i].v();
				//-(parTj + RiL x parFj)
				CROSS_X(Rj, pd, vecTemp);
					ADD3(pV, vecTemp, tempD);
					partialJtorque[1][i].setVals(tempD);
			}
		}
	}
}

od_bushing::od_bushing(int Id, char* _name, od_marker *pI, od_marker *pJ,
	double _ktx, double _kty, double _ktz,
	double _krx, double _kry, double _krz,
	double _ctx, double _cty, double _ctz,
	double _crx, double _cry, double _crz,
	double* _f, double* _t) : od_force(Id, _name, pI, pJ) {
	ktx = _ktx; kty = _kty; ktz = _ktz;
	krx = _krx; kry = _kry; krz = _krz;
	ctx = _ctx; cty = _cty; ctz = _ctz;
	crx = _crx; cry = _cry; crz = _crz;
	EQ3(f, _f) EQ3(t, _t)
		force_type = BUSHING;
	DDT = VT = DR = VR = 0;
	parFparq = parTparq = parFparq_dot = parTparq_dot = 0;
	parFjparq = parTjparq = parFjparq_dot = parTjparq_dot = 0;
}

od_bushing::~od_bushing() {
	DELARY(parTparq) DELARY(parFparq) DELARY(parTparq_dot) DELARY(parFparq_dot);
	DELARY(parTjparq) DELARY(parFjparq) DELARY(parTjparq_dot) DELARY(parFjparq_dot);

}

int od_bushing::init() {
	vector<int>  cols;
	int ii, i, j, _size;
	init_force();
	int num_j = pSys->tree_dofs();
	od_body* bodys[2] = { i_marker->get_body(), j_marker->get_body() };
	for (j = 0; j < 2; j++) {
		if (bodys[j]->ground_body() == 0) {
			int b_index = bodys[j]->get_index();
			int tempInt = b_index * num_j;
			for (i = 0; i < num_j; i++) {
			//	if (pSys->relevenceLevel2[tempInt + i] != 0) {
				if (pSys->getL2(tempInt + i) != 0) {
					if (!j) iIds.push_back(i);
					else jIds.push_back(i);
				}
			}
		}
	}
	DDT = new od_measure(pSys, od_measure::DT, i_marker, j_marker, j_marker);
	DDT->init();
	VT = new od_measure(pSys, od_measure::VT, i_marker, j_marker, j_marker);
	VT->init();
	DR = new od_measure(pSys, od_measure::DR, i_marker, j_marker, j_marker);
	DR->init();
	VR = new od_measure(pSys, od_measure::VR, i_marker, j_marker, j_marker);
	VR->init();
	//parDDTparq
	cols.resize(0);
	_size = DDT->get_partial_size(0);
	parFTparq_size = _size;
	for (i = 0; i < _size; i++) {
		j = DDT->get_partial_col(0, i);
		cols.push_back(j);
	}
	//parAjparq
	_size = (int)jIds.size();
	parFTparq_size += _size;
	for (i = 0; i < _size; i++) {
		j = jIds[i];
		cols.push_back(j);
	}
	parFparq = new od_colVec[parFTparq_size];
	parTparq = new od_colVec[parFTparq_size];
	for (i = 0; i < parFTparq_size; i++) {
		j = cols[i];
		parFparq[i].setCol(j);
		parTparq[i].setCol(j);
		columns_iforce[0].push_back(j);
		columns_itorque[0].push_back(j);
		columns_jforce[0].push_back(j);
		columns_jtorque[0].push_back(j);
	}
	parFTparq_dot_size = 0;
	cols.resize(0);
	_size = VT->get_partial_size(1);
	parFTparq_dot_size = _size;
	for (i = 0; i < _size; i++) {
		j = VT->get_partial_col(1, i);
		cols.push_back(j);
	}
	parFparq_dot = new od_colVec[parFTparq_dot_size];
	parTparq_dot = new od_colVec[parFTparq_dot_size];
	for (i = 0; i < parFTparq_dot_size; i++) {
		j = cols[i];
		parFparq_dot[i].setCol(j);
		parTparq_dot[i].setCol(j);
		columns_iforce[1].push_back(j);
		columns_itorque[1].push_back(j);
		columns_jforce[1].push_back(j);
		columns_jtorque[1].push_back(j);
	}
	// partial of AjFi
	//partial of Ri x (AjFi)
	_size = (int)iIds.size();
	for (i = 0; i < _size; i++) {
		columns_itorque[0].push_back(iIds[i]);
	}
	//partial of Rj x (AjFi)
	_size = (int)jIds.size();
	for (i = 0; i < _size; i++) {
		columns_jtorque[0].push_back(jIds[i]);
	}

	//partial of DT x (AjFj)
	_size = DT->get_partial_size(0);
	for (i = 0; i < _size; i++) {
		j = DT->get_partial_col(0, i);
		columns_jtorque[0].push_back(j);
	}

	for (ii = 0; ii < 2; ii++) {
		columns_iforce_size[ii] = (int)columns_iforce[ii].size();
		if (columns_iforce_size[ii]) partialIforce[ii] = new od_colVec[columns_iforce_size[ii]];
		for (i = 0; i < columns_iforce_size[ii]; i++) {
			j = columns_iforce[ii][i];
			partialIforce[ii][i].setCol(j);
		}
		columns_itorque_size[ii] = (int)columns_itorque[ii].size();
		if (columns_itorque_size[ii]) partialItorque[ii] = new od_colVec[columns_itorque_size[ii]];
		for (i = 0; i < columns_itorque_size[ii]; i++) {
			j = columns_itorque[ii][i];
			partialItorque[ii][i].setCol(j);
		}
		columns_jforce_size[ii] = (int)columns_jforce[ii].size();
		if (columns_jforce_size[ii]) partialJforce[ii] = new od_colVec[columns_jforce_size[ii]];
		for (i = 0; i < columns_jforce_size[ii]; i++) {
			j = columns_jforce[ii][i];
			partialJforce[ii][i].setCol(j);
		}
		columns_jtorque_size[ii] = (int)columns_jtorque[ii].size();
		if (columns_jtorque_size[ii]) partialJtorque[ii] = new od_colVec[columns_jtorque_size[ii]];
		for (i = 0; i < columns_jtorque_size[ii]; i++) {
			j = columns_jtorque[ii][i];
			partialJtorque[ii][i].setCol(j);
		}
	}


	return 1;
}

void od_bushing::evaluate(int partial) {
	int i, j;
	double *pt, *pr, *dt;
	double *ptc, *prc, *pd, *pV;
	double tempF[3];
	double tempT[3];
	double tempD[3];
	double vecTemp[3];
	eval_force(partial);
	DDT->eval(partial);
	VT->eval(partial);
	DR->eval(partial);
	VR->eval(partial);
	pt = DDT->get_val();
	pr = DR->get_val();
	ptc = VT->get_val();
	prc = VR->get_val();
	iForce[0] = -ktx * pt[0] - ctx * ptc[0] + f[0];
	iForce[1] = -kty * pt[1] - cty * ptc[1] + f[1];
	iForce[2] = -ktz * pt[2] - ctz * ptc[2] + f[2];
	iTorque[0] = -krx * pr[0] - crx * prc[0] + t[0];
	iTorque[1] = -kry * pr[1] - cry * prc[1] + t[1];
	iTorque[2] = -krz * pr[2] - crz * prc[2] + t[2];
	j_marker->vec_wrt_ground(iForce);
	j_marker->vec_wrt_ground(iTorque);
	EQ3(jForce, iForce) NEG(jForce);
	EQ3(jTorque, iTorque) NEG(jTorque);
	CROSS_X(Ri, iForce, vecTemp) U_ADD3(iTorque, vecTemp);
	dt = DT->get_val();
	ADD3(dt, Rj, tempD);
	CROSS_X(tempD, jForce, vecTemp) U_ADD3(jTorque, vecTemp);

	if (partial) {
		int lastCount = 0;
		int _size = DDT->get_partial_size(0);
		for (i = 0; i < (int)_size; i++) {
			pt = DDT->get_partial(0, i)->v;
			pr = DR->get_partial(0, i)->v;
			ptc = VT->get_partial(0, i)->v;
			prc = VR->get_partial(0, i)->v;
			tempF[0] = -ktx * pt[0] - ctx * ptc[0];
			tempF[1] = -kty * pt[1] - cty * ptc[1];
			tempF[2] = -ktz * pt[2] - ctz * ptc[2];
			tempT[0] = -krx * pr[0] - crx * prc[0];
			tempT[1] = -kry * pr[1] - cry * prc[1];
			tempT[2] = -krz * pr[2] - crz * prc[2];
			j_marker->vec_wrt_ground(tempF);
			j_marker->vec_wrt_ground(tempT);
			parFparq[i].setVals(tempF);
			parTparq[i].setVals(tempT);
		}
		lastCount += _size;
		_size = (int)jIds.size();
		//parAj_parq
		for (i = 0; i < (int)_size; i++) {
			j = lastCount + i;
			int b_index = j_marker->get_body()->get_index();
			int col = parFparq[j].col();
			pd = pSys->getJR(b_index, col)->v; //JR.element(b_index, col)->v;
			pV = parFparq[j].v();
			CROSS_X(pd, iForce, pV);
			pV = parTparq[j].v();
			CROSS_X(pd, iTorque, pV)
		}
		lastCount += _size;
		for (i = 0; i < lastCount; i++) {
			pd = parFparq[i].v();
			partialIforce[0][i].setVals(pd);
			partialJforce[0][i].setVals(pd);
			partialJforce[0][i].neg();

			pV = parTparq[i].v();
			//parTi + Ri x parFi
			CROSS_X(Ri, pd, vecTemp);
			ADD3(pV, vecTemp, tempD);
			partialItorque[0][i].setVals(tempD);
			//-(parTi + (Ri+DT) x parFi)
			ADD3(Rj, dt, vecTemp);
			CROSS_X(vecTemp, pd, tempD);
			ADD3(pV, tempD, vecTemp) NEG(vecTemp);
			partialJtorque[0][i].setVals(vecTemp);
		}
		//parRi x Fi
		_size = (int)iIds.size();
		for (i = 0; i < _size; i++) {
			pd = parRiparq[i].v();
			CROSS_X(pd, iForce, vecTemp) partialItorque[0][lastCount + i].setVals(vecTemp);
		}
		//parRj x Fi
		_size = (int)jIds.size();
		for (i = 0; i < _size; i++) {
			pd = parRjparq[i].v();
			CROSS_X(pd, jForce, vecTemp) partialJtorque[0][lastCount + i].setVals(vecTemp);
		}
		//parDT x Fj
		lastCount += _size;
		_size = DT->get_partial_size(0);
		for (i = 0; i < _size; i++) {
			pd = DT->get_partial(0, i)->v;
			CROSS_X(pd, jForce, vecTemp) partialJtorque[0][lastCount + i].setVals(vecTemp);
		}
		//PAR_PARQ_DOT
		lastCount = 0;
		if (pSys->get_analysis_type() == DYNAMIC) {
			_size = VT->get_partial_size(1);
			for (i = 0; i < _size; i++) {
				ptc = VT->get_partial(1, i)->v;
				prc = VR->get_partial(1, i)->v;
				tempF[0] = -ctx * ptc[0];
				tempF[1] = -cty * ptc[1];
				tempF[2] = -ctz * ptc[2];
				tempT[0] = -crx * prc[0];
				tempT[1] = -cry * prc[1];
				tempT[2] = -crz * prc[2];
				j_marker->vec_wrt_ground(tempF);
				j_marker->vec_wrt_ground(tempT);
				parFparq_dot[i].setVals(tempF);
				parTparq_dot[i].setVals(tempT);
			}
			lastCount += _size;
			for (i = 0; i < lastCount; i++) {
				pd = parFparq_dot[i].v();
				partialIforce[1][i].setVals(pd);
				partialJforce[1][i].setVals(pd);
				partialJforce[1][i].neg();

				pV = parTparq_dot[i].v();
				//parTi + Ri x parFi
				CROSS_X(Ri, pd, vecTemp);
				ADD3(pV, vecTemp, tempD);
				partialItorque[1][i].setVals(tempD);
				//parTi + (Ri+DT) x parFi
				ADD3(Rj, dt, vecTemp);
				CROSS_X(vecTemp, pd, tempD);
				ADD3(pV, tempD, vecTemp) NEG(vecTemp);
				partialJtorque[1][i].setVals(vecTemp);
			}
		}
	}
}



OdForce::OdForce(int id, char* name_) {
	_id = id;
	pi = pj = 0;
	pF = 0;
	_expr = 0;
	_name = name_;
	K = 0.0; C = 0.0; Dist = 0.0; For = 0.0;
	Ixx = 0.0; Iyy = 0.0; Izz = 0.0; Area = 0.0; Asy = 0.0; Asz = 0.0; Ratio = 0.0; Emodulus = 0.0; Gmodulus = 0.0;
}

OdForce::~OdForce() {
	/*  if (pF) {
	delete pF;
	}
	  if(_expr)   delete[] _expr;*/
}

void OdForce::sforce() {
	_type = od_object::SFORCE;
}

void OdForce::storque() {
	_type = od_object::STORQUE;
}

void OdForce::spdpt() {
	_type = od_object::SPDPT;
}

void OdForce::beam() {
	_type = od_object::BEAM;
}

void OdForce::bushing() {
	_type = od_object::BUSHING;
}

void OdForce::set_imarker(OdMarker *pM) {
	pi = pM;
}
void OdForce::set_jmarker(OdMarker *pM) {
	pj = pM;
}

void OdForce::set_expr(char* pexpr) {
	int len = (int)strlen(pexpr);
	_expr = new char[len + 1];
	strcpy(_expr, pexpr);
}


od_force* OdForce::core() {
	if (pF) return pF;
	od_marker* pmi = pi->core();
	od_marker* pmj = pj->core();
	switch (_type) {
	case od_object::SFORCE:
		pF = (od_force*)(new od_sforce(_id, _name, pmi, pmj, _expr)); break;
	case od_object::STORQUE:
		pF = (od_force*)(new od_storque(_id, _name, pmi, pmj, _expr)); break;
	case od_object::SPDPT:
		pF = (od_force*)(new od_spdpt(_id, _name, pmi, pmj, K, C, Dist, For));
		break;
	case od_object::BEAM:
		pF = (od_force*)(new od_beam(_id, _name, pmi, pmj, Dist, Ixx, Iyy, Izz, Area, Emodulus, Gmodulus, Ratio, Asy, Asz));
		break;
	case od_object::BUSHING:
		pF = (od_force*)(new od_bushing(_id, _name, pmi, pmj,
			bushingK[0], bushingK[1], bushingK[2], bushingK[3], bushingK[4], bushingK[5],
			bushingC[0], bushingC[1], bushingC[2], bushingC[3], bushingC[4], bushingC[5],
			bushingF, bushingF + 3));
		break;
	default:
		pF = 0; break;
	}
	return pF;
}
char* OdForce::info(char* msg) {
	msg = pF->info(msg);
	return msg;
}

void OdForce::set_stiffness(double val) {
	K = val;
}
void OdForce::set_damping(double val) {
	C = val;
}
void OdForce::set_distance(double val) {
	Dist = val;
}
void OdForce::set_force(double val) {
	For = val;
}

OdJointSPDP::OdJointSPDP(int id, char* name) {
	_id = id;
	pJF = 0; pJ = 0; K = C = L = F = 0.0; _name = name;
}

od_joint_spdp* OdJointSPDP::core() {
	if (pJF) return pJF;
	od_joint* joint = pJ->core();
	pJF = new od_joint_spdp(_id, _name, joint, K, C, L, F, ith);
	return pJF;
}
OdJointForce::OdJointForce(int id, char* name) {
	_id = id;
	pJF = 0; pJ = 0; _expr = 0; _name = name;
}

OdJointForce::~OdJointForce() {}

od_joint_force* OdJointForce::core() {
	if (pJF) return pJF;
	od_joint* joint = pJ->core();
	pJF = new od_joint_force(_id, _name, joint, _expr);
	return pJF;
}

void OdJointForce::setJoint(OdJoint *_pJ) {
	pJ = _pJ;
}

void OdJointForce::set_expr(char* pexpr) {
	int len = (int)strlen(pexpr);
	_expr = new char[len + 1];
	strcpy(_expr, pexpr);
}
