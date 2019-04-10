#include "od_system.h"
#include "od_equset.h"
#include "od_gstiff.h"
od_equation::~od_equation() {
	//int i;
	if (info_str.size())
		cout << info_str << " is done!" << endl;
	DELARY(pRhs);
	if (this->permuV) delete[] this->permuV;
	this->permuV = 0;
	if (_dofmap) delete[] _dofmap;
	delete SysJac;
	DELARY(error); DELARY(_X); DELARY(_Xdot); DELARY(_Xddot);
}
void
od_equation::createPermuV(int len_){// vector<int>& dofmap) {
	vector<int> tempV;
	vector<int> tempV_;
	int i, size_;
	size_ = (int)dofmap.size();
	if (len_) size_ = len_;
	if (!_dofmap) _dofmap = new int[size_];
	if (!permuV) permuV = new int[size_];
	for (i = 0; i < size_; i++) {
		_dofmap[i] = dofmap[i];
		if (dofmap[i] != 0) tempV.push_back(i);
		else tempV_.push_back(i);
	}
	effDim = (int)tempV.size();
	for (i = 0; i < effDim; i++) permuV[i] = tempV[i];
	for (i = 0; i < (int)tempV_.size(); i++) permuV[effDim + i] = tempV_[i];
}
void
od_equation::
set_lambda(double* lambda, int inc) {
	int i;
	od_loop *pl;
	for (i = 0; i < numLoops; i++) {
		pl = pSys->loop_list[i];
		lambda = pl->set_lambda(lambda, inc);
	}
	/*for(i=0; i<numMotions; i++) {
	  lambda = pSys->get_motion_via_index(i)->set_lambda(lambda);
	}*/
}
void
od_equation::
set_lambda_si2(double* lambda) {
	int i;
	od_loop *pl;
	for (i = 0; i < numLoops; i++) {
		pl = pSys->loop_list[i];
		lambda = pl->set_lambda_si2(lambda);
	}
	/* for(i=0; i<numMotions; i++) {
	   lambda = pSys->get_motion_via_index(i)->set_lambda_si2(lambda);
	 }*/
}
double od_equation::startRecord() { return pSys->startRecord(); }
void od_equation::stopRecord(double s, int d) { pSys->stopRecord(s, d); }
double od_equation::getKPE() { return pSys->getKPE(); }

int od_equation::checkEulerBryant() { return pSys->checkEulerBryant(); }

ofstream* od_equation::msgFile() { return pSys->msgFile(); }

double od_equation::time(double t) {
	return pSys->time(t);
}

int
od_equation::
initialize() {
	pSys->initialize(_dva_type, _jac_type, Ana_Type);
	states = pSys->pstates();
	dstates = pSys->pdstates();
	ddstates = pSys->pddstates();
	//get the number of ndof
	dim_rows = pSys->TreeDofs();
	tree_ndofs = pSys->tree_dofs();
	//vector<int>& releget the number of constraint
	numLoops = pSys->get_num_explicit_constraints();
	//numMotions = pSys->getNumMotions();
   // for(int i=0; i<numLoops; i++) {
   //   pSys->loop_list[i]->initialize(time()>1.e-9);
	//}
	_X = _Xdot = _Xddot = 0;
	_dofmap = 0;
	return 0;
}

od_equation_disp_ic::
od_equation_disp_ic(od_system *psys) :od_equation_kin_and_static(psys) {
	info_str = "Displacement Reconcilation";
	cout << info_str << " begins..." << endl;
	_dva_type = od_object::DISP;
	//_dva_type = od_object::DISP;
	_jac_type = od_object::JAC_INIT_DISP;
	Ana_Type = od_object::INIT_DISP;
}

int
od_equation_kin_and_static::
initialize() {
	if (initialized) return 0;
	int i;// , j;
	od_equation::initialize();
	//allocate space for pJac
	//dim_rows = numLoops * 6 + tree_ndofs;//+numMotions;
	dimJac = dim_cols = dim_rows;
	dofmap.resize(dim_rows);
	fill(dofmap.begin(), dofmap.end(), 1);
	for (i = 0; i < tree_ndofs; i++) this->dofmap[i] = pSys->dofmap_[i];
	//pJac = new double*[dim_rows];
	//for (j = 0; j < dim_cols; j++) { pJac[j] = new double[dim_cols]; }

	SysJac = (od_matrix*)(new od_matrix_dense(dim_rows, dim_rows));
	pJac = SysJac->values();
	pRhs = new double[dim_rows];
	//for (i = 0; i < dim_rows; i++) { fill(pJac[i], pJac[i] + dim_cols, 0.0); }
	for (i = 0; i < tree_ndofs; i++) { pJac[i][i] = 1.0; }
	error = new double[dim_rows - tree_ndofs];
	fill(error, error + dim_rows - tree_ndofs, 5.0e-16);
	return 0;
}

void od_equation_kin_and_static::
process_void_row_and_col(int ii, double val) {
	for (int i = 0; i < dim_rows; i++) {
		pJac[i][ii] = pJac[ii][i] = 0.0;
		pJac[ii][ii] = val;
	}
}

int od_equation_kin_and_static::evaluate(int eval_jac) {
	int i, ii, j, jj, base;
	int temp_int = 0;
	double value, skew;
	od_loop* pl;
	od_object::Analysis_Type _type = pSys->get_analysis_type();
	skew = 1.0;
	//if (_type == od_object::Analysis_Type::ACC_FORCE) skew = -1.0;
	if (_type == ACC_FORCE) skew = -1.0;
	pSys->update(eval_jac);
	fill(pRhs, pRhs + dim_rows, 0.0);
	if (eval_jac) {
		for (i = 0; i < dim_rows; i++) { fill(pJac[i], pJac[i] + dim_rows, 0.0); }
		fill(dofmap.begin(), dofmap.end(), 1);
		for (i = 0; i < tree_ndofs; i++) this->dofmap[i] = pSys->dofmap_[i];
		for (i = 0; i < numLoops; i++) {
			pl = pSys->loop_list[i];
			pl->evaluate(0);
			pl->checkRedundancy();
			base = tree_ndofs + i * 6;
			for (j = 0; j < pSys->loop_list[i]->num_nonzero(); j++) {
				ii = pl->row(j) + base;
				jj = pl->col(j);
				value = pl->jac(j);
				pJac[ii][jj] += value;
				pJac[jj][ii] += value * skew;
				//ACC_FORCE: the constraining force is the same as applied force on the LHS
				//
			}
			for (ii = 0; ii < 6; ii++) {
				temp_int = base + ii;
				if (pl->if_redundant(ii)) {
					dofmap[temp_int] = 0;
				}
			}
		}
		if (_type == ACC_FORCE) {
		//if (_type == od_object::Analysis_Type::ACC_FORCE) {
			for (i = 0; i < tree_ndofs; i++) {
				if (dofmap[i] == 0) dofmap[i] = -1; // motioned freedom
			}
		}
		createPermuV();// dofmap);
		//base = tree_ndofs + numLoops * 6;

		pSys->evaluate_rhs(pRhs);
		pSys->evaluate_Jac(pJac);
		//for (i = 0; i < dim_rows; i++) pRhs[i] = -pRhs[i];
		if (_type == ACC_FORCE || _type == STATIC) {
		//if (_type == od_object::Analysis_Type::ACC_FORCE || _type == od_object::Analysis_Type::STATIC) {
			for (i = 0; i < tree_ndofs; i++) {
				if (dofmap[i] == -1) {
					for (j = 0; j < dim_rows; j++) { pJac[j][i] = 0.0; }
					pJac[i][i] = 1.0*skew;
				}
			}
		}
	}
	else {
		pSys->evaluate_rhs(pRhs);
		//for (i = 0; i < dim_rows; i++) pRhs[i] = -pRhs[i];
		for (i = 0; i < numLoops; i++) { pSys->loop_list[i]->evaluate(1); }
	}
	return 0;
}

int od_equation_disp_ic::evaluate(int eval_jac) {
	int i, j, base;
	od_loop* pl;
	od_equation_kin_and_static::evaluate(eval_jac);
	for (i = 0; i < numLoops; i++) {
		pl = pSys->loop_list[i];
		base = tree_ndofs + i * 6;
		for (j = 0; j < 6; j++) {
			if (pl->if_redundant(j)) {
				pRhs[base + j] = 0.0;
			}
			else {
				pRhs[base + j] = pl->rhs(j);
			}
		}
	}
	return 0;
}

int od_equation_disp_ic::solve(double _temp) {
	int i;
	int max_rhs_row, max_var_row;
	double max_rhs, max_var;
	int num_dofs;
	int error_code = 0;
	int num_iterations = 0;
	//int dofs_calculated = 0;
	int repar = 0;
	pSys->unset_evaluated();
	for (;;) {
		max_rhs = 0.0;
		max_var = 0.0;
		pSys->get_states();
		if (pSys->Ic.disp_pattern[num_iterations % 10] == 1) {
			evaluate(1);
			//SysJac->print_out();  
			//SysJac->evaluate(); 
			repar = 1;
		}
		else {
			evaluate(0);
			repar = 0;
		}
		if (!dofs_calculated) {
			num_dofs = 0;
			for (i = 0; i < tree_ndofs; i++) { num_dofs += this->dofmap[i]; }
			for (i = 0; i < numLoops; i++) {
				int temp_dof = pSys->loop_list[i]->num_of_redundant_constraints();
				num_dofs -= (6 - temp_dof);
			}
			//num_dofs -= numMotions;
			pSys->set_dofs(num_dofs);
			dofs_calculated = 1;
		}
		if (!numLoops) {
			cout << "This is a system without loop. " << endl;
			cout << "Displacement Reconcilation achieved without iteration" << endl;
			break;
		}

		for (i = tree_ndofs; i < dim_rows; i++) {
			if (fabs(pRhs[i]) > max_rhs) {
				max_rhs = fabs(pRhs[i]);
				max_rhs_row = i;
			}
		}

		if (fabs(max_rhs) <= pSys->Ic.disp_tolerance && num_iterations != 0) {
			cout << "The displacement Reconcilation is achieved after " << num_iterations << " iterations" << endl;
			cout << " with maximum residual as " << max_rhs << endl;
			cout << " The system has " << pSys->get_dofs() << " degree-of-freedom(s)." << endl;
			evaluate(0);
			pSys->Analysis_Flags.disp_ic = 1;
			break;
		}
		SysJac->print_out();
		pRhs = SysJac->solve(pRhs, repar, effDim, permuV);
		for (i = 0; i < dim_rows; i++) {
			if (fabs(pRhs[i]) > fabs(max_var)) {
				max_var = pRhs[i];
				max_var_row = i;
			}
		}

		num_iterations++;
		if (num_iterations > pSys->Ic.disp_maxit) {
			evaluate(1);
			SysJac->print_out();
			throw equ_exception(num_iterations, max_var, max_var_row, max_rhs, max_rhs_row);
		}
		for (i = 0; i < tree_ndofs; i++) { states[i] -= pRhs[i]; }
		pSys->set_states();
	}
	pSys->set_states();
	evaluate(0);
	return error_code;
}

od_equation_vel_ic::od_equation_vel_ic(od_system *psys) :od_equation_kin_and_static(psys) {
	_dva_type = od_object::VEL;
	_jac_type = od_object::JAC_VEL;
	Ana_Type = od_object::INIT_VEL;
	info_str = "Velocity Reconcilation";
	cout << info_str << " begins..." << endl;
}

int
od_equation_vel_ic::evaluate(int eval_jac) {
	int i, j, base;
	od_loop* pl;
	od_equation_kin_and_static::evaluate(eval_jac);
	for (i = 0; i < numLoops; i++) {
		pl = pSys->loop_list[i];
		base = tree_ndofs + i * 6;
		for (j = 0; j < 6; j++) {
			if (pl->if_redundant(j)) {
				pRhs[base + j] = 0.0;
			}
			else {
				pRhs[base + j] = pl->velRhs(j);
			}
		}
	}
	return 0;
}

int od_equation_vel_ic::solve(double _temp) {
	int i;
	// J dq + dv =0    da = - J^{-1} dv
	pSys->unset_evaluated();
	for (i = 0; i < tree_ndofs; i++) dstates[i] = 0.0;
	pSys->set_states();
	evaluate(1);

	SysJac->print_out();

	pRhs = SysJac->solve(pRhs, 1, effDim, permuV);
	for (i = 0; i < tree_ndofs; i++) { dstates[i] = -pRhs[i]; }
	pSys->set_states();
	pSys->Analysis_Flags.vel_ic = 1;
	evaluate(0);
	return 0;
}
od_equation_acc_ic::
od_equation_acc_ic(od_system *psys) :od_equation_kin_and_static(psys) {
	_dva_type = od_object::ACC;
	_jac_type = od_object::JAC_ACC;
	Ana_Type = od_object::ACC_FORCE;
	//Ana_Type = od_object::INIT_ACC;
	info_str = "Acceleration and Force Reconcilation";
	cout << info_str << " begins..." << endl;
}
int od_equation_acc_ic::
solve(double _temp) {
	int i;
	for (i = 0; i < numLoops; i++) { pSys->loop_list[i]->init_lambda(); }
	pSys->get_states();
	for (i = 0; i < tree_ndofs; i++) ddstates[i] = 0.0;
	pSys->set_states();
	pSys->unset_evaluated();
	evaluate(1);
	SysJac->print_out();
	pRhs = SysJac->solve(pRhs, 1, effDim, permuV);
	//the sign is taken care by the skew in Jac evalution
	//for (i = tree_ndofs; i < dim_cols; i++) { pRhs[i] = -pRhs[i]; } 
	copy(pRhs, pRhs + tree_ndofs, ddstates);
	pSys->set_states();
	for (i = 0; i < numLoops; i++) { pSys->loop_list[i]->set_lambda(pRhs + tree_ndofs + 6 * i, 0); }
	pSys->Analysis_Flags.acc_ic = 1;
	evaluate(0);
	return 1;
}
int
od_equation_acc_ic::evaluate(int eval_jac) {
	int i, j, base;
	od_loop* pl;
	od_equation_kin_and_static::evaluate(eval_jac);
	for (i = 0; i < numLoops; i++) {
		pl = pSys->loop_list[i];
		base = tree_ndofs + i * 6;
		for (j = 0; j < 6; j++) {
			if (pl->if_redundant(j)) {
				pRhs[base + j] = 0.0;
			}
			else {
				pRhs[base + j] = pl->accRhs(j);
			}
		}
	}
	return 0;
}
od_equation_kinematic::od_equation_kinematic(od_system *psys, double end_time, int steps)
	: od_equation_kin_and_static(psys) {
	_end_time = end_time;
	_steps = steps;
	if (_steps <= 0) _steps = 1;
	Ana_Type = od_object::KINEMATIC;
	//pSys->set_analysis_type(od_object::KINEMATIC);
	eval = 1;
	//cout << "Displacement Reconcilation begins..." << endl;
}
int
od_equation_kinematic::initialize() {
	if (pSys->get_dofs() != 0) {
		cout << "This system has " << pSys->get_dofs() << " degrees of freedom." << endl;
		return 1;
	}
	eval = 1;
	if (initialized) return 0;
	od_equation_kin_and_static::initialize();
	initialized = 1;
	return 0;
}
int od_equation_kinematic::solve_for_disp() {
	int i, fail_num;
	int max_rhs_row, max_var_row;
	double max_rhs, max_var;
	int error_code = 0;
	int repar = 0;
	int num_iterations = 0;
	pSys->set_analysis_type(od_object::INIT_DISP);
	pSys->set_dva_type(od_object::DISP);
	pSys->set_jac_type(od_object::JAC_DISP);
	//prediction
	for (i = 0; i < tree_ndofs; i++) { states[i] += dstates[i] * delta; }
	max_rhs = 0.0;
	max_var = 0.0;
	fail_num = 0;
	//eval = 1;
	pSys->unset_evaluated();
	pSys->set_states();
	for (;;) {
		max_rhs = 0.0;
		max_var = 0.0;
		pSys->get_states();
		if (pSys->kinematics.disp_pattern[num_iterations % 10] == 1) {
			//if (eval) {
			evaluate_dva(1, 0);
			//SysJac->evaluate();
			repar = 1;
			eval = 0;
		}
		else {
			evaluate_dva(0, 0);;
		}
		if (!numLoops) {
			break;
		}
		for (i = tree_ndofs; i < dim_rows; i++) {
			if (fabs(pRhs[i]) > max_rhs) {
				max_rhs = fabs(pRhs[i]);
				max_rhs_row = i;
			}
		}
		if (fabs(max_rhs) <= pSys->kinematics.disp_tolerance) {
			break;
		}
		pRhs = SysJac->solve(pRhs, repar, effDim, permuV);
		//pRhs = SysJac->solve(pRhs, 0); 
		for (i = 0; i < dim_rows; i++) {
			if (fabs(pRhs[i]) > fabs(max_var)) {
				max_var = pRhs[i];
				max_var_row = i;
			}
		}

		num_iterations++;
		if (num_iterations > pSys->kinematics.disp_maxit) {
			if (fail_num > 5) {
				cout << " The reconcilation fails with "
					<< fail_num << " tries " << "at time "
					<< pSys->time() << endl;
				error_code = 1;
				evaluate(1);
				SysJac->print_out();
				throw equ_exception(num_iterations, max_var, max_var_row, max_rhs, max_rhs_row, pSys->time());
			}
			num_iterations = 0;
			fail_num++;
			// eval = 1;
		}
		/*if eval is needed, do not update the state which causes the failure*/
		//if (eval != 1) {
		for (i = 0; i < tree_ndofs; i++) { states[i] -= pRhs[i]; }
		//}
		pSys->set_states();
	}
	return error_code;
}
int od_equation_kinematic::solve_for_vel() {
	int i;
	pSys->unset_evaluated();
	pSys->set_analysis_type(od_object::INIT_VEL);
	pSys->set_dva_type(od_object::VEL);
	pSys->set_jac_type(od_object::JAC_VEL);
	pSys->get_states();
	evaluate_dva(1, 1);
	SysJac->print_out();

	pRhs = SysJac->solve(pRhs, 1, effDim, permuV);

	for (i = 0; i < tree_ndofs; i++) { dstates[i] -= pRhs[i]; }
	pSys->set_states();
	pSys->Analysis_Flags.vel_ic = 1;
	evaluate_dva(0, 1);
	return 0;
}
int od_equation_kinematic::solve_for_acc_force() {
	int i;
	pSys->set_analysis_type(od_object::ACC_FORCE);
	pSys->set_dva_type(od_object::ACC);
	pSys->set_jac_type(od_object::JAC_ACC);
	pSys->unset_evaluated();
	for (i = 0; i < tree_ndofs; i++) { ddstates[i] = 0.0; }
	for (i = 0; i < numLoops; i++) { pSys->loop_list[i]->init_lambda(); }
	pSys->set_states();
	evaluate_dva(1, 2);
	SysJac->print_out();
	SysJac->print_out(permuV);
	pRhs = SysJac->solve(pRhs, 1, effDim, permuV);
	//for (i = 0; i < dim_cols; i++) pRhs[i] = -pRhs[i];
	for (i = 0; i < tree_ndofs; i++) { ddstates[i] = pRhs[i]; }
	for (i = 0; i < numLoops; i++) { pSys->loop_list[i]->set_lambda(pRhs + tree_ndofs + i * 6, 0); }
	pSys->set_states();
	pSys->Analysis_Flags.acc_ic = 1;
	evaluate_dva(1, 2);
	return 0;
}
int od_equation_kinematic::solve(double _temp) {
	int i;
	double start_time = pSys->time();
	delta = (_end_time - start_time) / (double)_steps;
	if (delta <= 0.0) return 0;
	double _time;
	int error_code = 0;
	//if (_end_time < 1.0e-17) return error_code;
	for (i = 1; i <= _steps; i++) {
		_time = start_time + i * delta;
		pSys->time(_time);

		//solve for disp
		error_code = solve_for_disp();
		if (error_code) break;

		//solve for vel
		error_code = solve_for_vel();
		if (error_code) break;

		//solve for acc and force
		error_code = solve_for_acc_force();
		if (error_code) break;

	}
	return error_code;
}
void od_equation_kinematic::simulate(double toTime) {
	_end_time = toTime;
	this->solve();
}
int od_equation_kinematic::evaluate_dva(int eval_jac, int dva) {
	int i, j, base;
	od_loop* pl;
	od_equation_kin_and_static::evaluate(eval_jac);
	for (i = 0; i < numLoops; i++) {
		pl = pSys->loop_list[i];
		base = tree_ndofs + i * 6;
		for (j = 0; j < 6; j++) {
			if (pl->if_redundant(j)) {
				pRhs[base + j] = 0.0;
			}
			else {
				if (dva == 0) {
					pRhs[base + j] = pl->rhs(j);
				}
				else if (dva == 1) {
					pRhs[base + j] = pl->velRhs(j);
				}
				else {
					pRhs[base + j] = -pl->accRhs(j);
				}
			}
		}
	}
	return 0;
}
od_equation_static::od_equation_static(od_system *psys, int steps, double  err_tol)
	: od_equation_kin_and_static(psys) {
	Ana_Type = od_object::STATIC;
	//pSys->set_analysis_type(od_object::STATIC);
	_dva_type = od_object::DISP;
	//pSys->set_dva_type(od_object::DISP);
	_jac_type = od_object::JAC_STATIC;
	//pSys->set_jac_type(od_object::JAC_STATIC);
	info_str = "Static Analysis";
	cout << info_str << " begins..." << endl;
	_tol = err_tol;
	_steps = steps;
	if (_steps <= 0) _steps = 1;
	eval = 1;
}
int od_equation_static::initialize() {
	eval = 1;
	if (initialized) return 0;
	od_equation_kin_and_static::initialize();
	return 0;
}
int
od_equation_static::
evaluate(int eval_jac) {
	int i, j, base;
	od_loop* pl;
	od_equation_kin_and_static::evaluate(eval_jac);
	for (i = 0; i < numLoops; i++) {
		pl = pSys->loop_list[i];
		base = tree_ndofs + i * 6;
		for (j = 0; j < 6; j++) {
			if (pl->if_redundant(j)) {
				pRhs[base + j] = 0.0;
			}
			else {
				pRhs[base + j] = pl->rhs(j);
			}
		}
	}
	return 0;
}
int
od_equation_static::
solve(double _temp) {
	int i, ii;
	int num_dofs;
	int max_rhs_row, max_var_row;
	double max_rhs, max_var;
	int dofs_calculated = 0;
	int converged = 0;
	//	int void_row;
	int last_row = -1;
	std = new double[tree_ndofs];
	//removed_entries.resize(0);
	for (i = 0; i < numLoops; i++) { pSys->loop_list[i]->init_lambda(); }
	for (ii = 0; ii < _steps; ii++) {
		pSys->get_states();
		for (i = 0; i < tree_ndofs; i++) std[i] = states[i] / 3.14159 * 180;
		evaluate(1);
		max_rhs = 0.0;
		max_var = 0.0;
		if (!dofs_calculated) {
			num_dofs = 0;
			for (i = 0; i < tree_ndofs; i++) {
				num_dofs += this->dofmap[i];
			}
			for (i = 0; i < numLoops; i++) {
				int temp_dof = pSys->loop_list[i]->num_of_redundant_constraints();
				num_dofs -= (6 - temp_dof);
			}
			//num_dofs -= numMotions;
			pSys->set_dofs(num_dofs);
			dofs_calculated = 1;
		}
		if (num_dofs == 0) {
			cout << "This system has no degrees_of_freedom. Static analysis is aborted\n" << endl;
			break;
		}
		for (i = 0; i < tree_ndofs; i++) {
			if (fabs(pRhs[i]) > max_rhs) {
				max_rhs = fabs(pRhs[i]);
				max_rhs_row = i;
			}
		}
		SysJac->print_out();
		pRhs = SysJac->solve(pRhs, 1, effDim, permuV);
		//pRhs = SysJac->solve(pRhs, 0); 
		for (i = 0; i < dim_rows; i++) {
			pRhs[i] = -pRhs[i];
			if (fabs(pRhs[i]) > fabs(max_var)) {
				max_var = fabs(pRhs[i]);
				max_var_row = i;
			}
		}
		double d30 = 3.1415926 / 6.0;
		for (i = 0; i < tree_ndofs; i++) {
			//if (fabs(max_var) > d30) pRhs[i] *= d30/fabs(max_var);
			states[i] += pRhs[i];
		}
		pSys->set_states();
		set_lambda(pRhs + tree_ndofs);
		if (fabs(max_var) < _tol) {
			converged = 1;
			break;
		}
	}
	if (converged == 0) {
		evaluate(1);
		throw equ_exception(_steps, max_var, max_var_row, max_rhs, max_rhs_row);
	}
	delete[] std;
	return 0;
}

od_equation_dynamic::od_equation_dynamic(od_system *psys, double end, int steps, double tol) : od_equation(psys) {
	_end = end; _steps = steps; _tol = tol;
	Ana_Type = od_object::DYNAMIC;
	_dva_type = od_object::DISP_VEL_ACC;
	_jac_type = od_object::JAC_DYNAMIC;
	reEval = 1;
}

od_equation_dynamic::~od_equation_dynamic() {
	int i;
	DELARY(Vars);// delete  pIntegrator;
	for (i = 0; i < tree_ndofs; i++) DELARY(M_a[i]); DELARY(M_a);
	for (i = 0; i < tree_ndofs; i++) DELARY(M_v[i]); DELARY(M_v);
	for (i = 0; i < tree_ndofs; i++) DELARY(M_d[i]); DELARY(M_d);
}

int od_equation_dynamic::initialize() {
	if (initialized) return 0;
	int j;// , base;

	od_equation::initialize();
	pSys->init_dynamics();
	hht = 0;
	M_a = new double*[tree_ndofs];
	for (j = 0; j < tree_ndofs; j++) {
		M_a[j] = new double[tree_ndofs];
		fill(M_a[j], M_a[j] + tree_ndofs, 0.0);
	}
	M_v = new double*[tree_ndofs];
	for (j = 0; j < tree_ndofs; j++) {
		M_v[j] = new double[tree_ndofs];
		fill(M_v[j], M_v[j] + tree_ndofs, 0.0);
	}
	M_d = new double*[tree_ndofs];
	for (j = 0; j < tree_ndofs; j++) {
		M_d[j] = new double[tree_ndofs];
		fill(M_d[j], M_d[j] + tree_ndofs, 0.0);

	}
	initialized = 1;
	pSys->getKPE();
	return 0;
}

int od_equation_dynamic::checkRedundancy() {
	int flag = 0;
	for (int i = 0; i < numLoops; i++) {
		flag += pSys->loop_list[i]->checkRedundancy();
		if (flag) return 1;
	}
	return 0;
}

void od_equation_bdf_I::set_states() {
	int i;
	int intTemp = tree_ndofs * sizeof(double);
	memcpy(dstates, _X, intTemp); //V
	memcpy(states, _X + tree_ndofs, intTemp); //U
	memcpy(ddstates, _Xdot, intTemp);  //Vdot
	pSys->set_states();
	for (i = 0; i < numLoops; i++) {
		od_loop* loop = pSys->loop_list[i];
		loop->set_lambda_si2(_X + tree_ndofs * 2 + 6 * i, 0);
		loop->set_lambda(_X + tree_ndofs * 2 + 6 * (i + numLoops), 0);
	}

}

void od_equation_bdf_I::get_states() {
	int i;
	int intTemp = tree_ndofs * sizeof(double);
	double *pf, *pm, *pd;
	pSys->get_states();
	memcpy(_X, dstates, intTemp);
	memcpy(_X + tree_ndofs, states, intTemp);
	memcpy(_Xdot, ddstates, intTemp);
	memcpy(_Xdot + tree_ndofs, dstates, intTemp);
	for (i = 0; i < numLoops; i++) {
		od_loop* loop = pSys->loop_list[i];
		pf = loop->force_si2();
		pm = loop->moment_si2(); pd = _X + tree_ndofs * 2 + 6 * i; EQ3(pd, pm) pd += 3; EQ3(pd, pf)
			pf = loop->force();
		pm = loop->moment();
		pd = _X + tree_ndofs * 2 + 6 * (numLoops + i);
		EQ3(pd, pm) pd += 3; EQ3(pd, pf)
	}

}

void od_equation_hhti3::set_states(int dva) {
	//dva:  0 update all
	//dav:  1 calculate nonlinear term
	//dav:  2 calculate linear term
	int i;
	int intTemp = tree_ndofs * sizeof(double);
	memcpy(states, _X, intTemp);
	memcpy(dstates, _Xdot, intTemp);
	if (dva == 2) fill(dstates, dstates + tree_ndofs, 0.0);
	memcpy(ddstates, _Xddot, intTemp);
	if (dva == 1) fill(ddstates, ddstates + tree_ndofs, 0.0);
	pSys->set_states();
	for (i = 0; i < numLoops; i++) {
		pSys->loop_list[i]->set_lambda(_Xddot + tree_ndofs + 6 * i, 0);
	}
}

void od_equation_hhti3::get_states() {
	int i;
	int intTemp = tree_ndofs * sizeof(double);
	double *pf, *pm, *pd;
	pSys->get_states();
	memcpy(_X, states, intTemp);
	memcpy(_Xdot, dstates, intTemp);
	memcpy(_Xddot, ddstates, intTemp);
	for (i = 0; i < numLoops; i++) {
		od_loop* loop = pSys->loop_list[i];
		//pf = loop->force_si2();
		//pm = loop->moment_si2(); 
		//pd = _X + tree_ndofs+ 6 * i; EQ3(pd, pm) pd += 3; 
		//EQ3(pd, pf);
		pf = loop->force();
		pm = loop->moment();
		pd = _Xddot + tree_ndofs + 6 * i;
		EQ3(pd, pm) pd += 3; EQ3(pd, pf)
	}

}

void od_equation_dynamic::setSteps(int steps) { _steps = steps; }
void od_equation_dynamic::setMinStepSize(double val) { hMin = val; }
void od_equation_dynamic::setMaxStepSize(double val) { hMax = val; }
void od_equation_dynamic::setInitStep(double val) { _initStep = val; h = val; }
void od_equation_dynamic::setLocalErrorTol(double val) { _tol = val; }
void od_equation_dynamic::setMaxCorrectNum(int val) { _corrNum = val; }

void od_equation_bdf_I::simulate(double end_time, int debug_) {
	int i = 0;
	double expectedTime = 0;
	double _delta;
	_end = end_time;
	_steps = 1;
	expectedTime = pSys->time();
	_delta = _end / (double)_steps;
	_delta = (_end - expectedTime) / (double)_steps;
	_debug = debug_;
	if (expectedTime < 1.0e-15) {
		pIntegrator->setMinStepSize(hMin);
		pIntegrator->setMaxStepSize(hMax);
		pIntegrator->setInitStep(_initStep);
		pIntegrator->setLocalErrTol(_tol);
		pIntegrator->setMaxCorrectNum(_corrNum);
	}

	for (i = 0; i < _steps; i++) {
		expectedTime += _delta;
		int errorCode = pIntegrator->toTime(expectedTime);
		if (errorCode) {
			throw equ_exception(0, 0.0, 0, 0.0, 0, pSys->time());
		}
	}
	pIntegrator->output(expectedTime);
	pSys->update();
}

void od_equation_hhti3::simulate(double end_time, int debug_) {
	int i;// , j;
	double expectedTime = 0.0;
	double _delta;
	_end = end_time;
	_steps = 1;
	expectedTime = pSys->time();
	_delta = _end / (double)_steps;
	_delta = (_end - expectedTime) / (double)_steps;
	_debug = debug_;
	if (expectedTime < 1.0e-15) {
		pIntegrator->setMinStepSize(hMin);
		pIntegrator->setMaxStepSize(hMax);
		pIntegrator->setInitStep(_initStep);
		pIntegrator->setLocalErrTol(_tol);
		pIntegrator->setMaxCorrectNum(_corrNum);
	}

	for (i = 0; i < _steps; i++) {
		expectedTime += _delta;
		int errorCode = pIntegrator->toTime(expectedTime);
		if (errorCode) {
			throw equ_exception(0, 0.0, 0, 0.0, 0, pSys->time());
		}
	}
	//pIntegrator->output(expectedTime);
	pSys->update();
}

od_equation_bdf_I::od_equation_bdf_I(od_system *psys, double end, int steps, double tol)
	: od_equation_dynamic(psys, end, steps, tol) {
	reEval = 1;
	pIntegrator = 0;
}

od_equation_bdf_I::~od_equation_bdf_I(){
	delete pIntegrator;
	delete SysJacBDF;
	delete[] pprhs;
}

od_equation_hhti3::od_equation_hhti3(od_system *psys, double end, int steps, double tol, double al)
	: od_equation_dynamic(psys, end, steps, tol) {
	alpha = al;
	reEval = 1;
	pIntegrator = 0;
	pQ = 0;
	Q = 0;
}

od_equation_hhti3::~od_equation_hhti3(){
	   delete pIntegrator; DELARY(pQ); DELARY(Q); pIntegrator = 0;
	   delete SysJacHHT;
	   delete[] pprhs; DELARY(Mq);
   }

int od_equation_hhti3::initialize() {
	int  j, base;
	od_equation_dynamic::initialize();
	//allocate space for pJac
	dim_cols = dim_rows = 1 * (numLoops * 6 + tree_ndofs);//+numMotions); 
	Vars = new int[dim_rows];
	pQ = new double[tree_ndofs];
	Q = new double[tree_ndofs];
	Mq = new double[tree_ndofs];
	for (j = 0; j < dim_rows; j++) Vars[j] = 0;
	for (j = 0; j < numLoops; j++)
	{
		int k;
		base = 1 * tree_ndofs + 6 * j;
		//for (k = 0; k < 6; k++) Vars[base + k] = 2;
		//base += numLoops * 6;
		for (k = 0; k < 6; k++) Vars[base + k] = 1;
	}
	hht = 1;
	dofmap.resize(dim_rows);
	fill(dofmap.begin(), dofmap.end(), 1);
	_X = new double[dim_rows];
	_Xdot = new double[dim_rows];
	_Xddot = new double[dim_rows];

	fill(_X, _X + dim_rows, 0.0);
	fill(_Xdot, _Xdot + dim_rows, 0.0);
	fill(_Xddot, _Xddot + dim_rows, 0.0);
	this->get_states();
	pIntegrator = new od_hhti3(this, 1.0e-3, alpha);
	beta = pIntegrator->getBeta();
	gamma = pIntegrator->getGamma();
	dimJac = dim_rows;
	

	pRhs = new double[dim_rows];
	pprhs = new double[dim_rows];
	fill(pRhs, pRhs + dim_rows, 0.0);
	updatepQ();
	SysJac = (od_matrix*)(new od_matrix_dense(dim_rows, dim_rows));
	SysJacHHT = (new od_matrix_hht(dim_rows, dim_rows));
	pJac = SysJac->values();
	return 1;
}

int od_equation_bdf_I::initialize() {
	int  j, base;
	od_equation_dynamic::initialize();
	//allocate space for pJac
	dim_cols = dim_rows = 2 * (numLoops * 6 + tree_ndofs);//+numMotions); 
	Vars = new int[dim_rows];
	for (j = 0; j < dim_rows; j++) Vars[j] = 0;
	for (j = 0; j < numLoops; j++)
	{
		int k;
		base = 2 * tree_ndofs + 6 * j;
		for (k = 0; k < 6; k++) Vars[base + k] = 2;
		base += numLoops * 6;
		for (k = 0; k < 6; k++) Vars[base + k] = 1;
	}

	dofmap.resize(dim_rows);
	fill(dofmap.begin(), dofmap.end(), 1);
	_X = new double[dim_rows];
	_Xdot = new double[dim_rows];
	fill(_X, _X + dim_rows, 0.0);
	fill(_Xdot, _Xdot + dim_rows, 0.0);
	pIntegrator = new od_gstiff(this, 1.0e-3);
	dimJac = dim_rows;

	pRhs = new double[dim_rows];
	pprhs = new double[dim_rows];

	SysJac = (od_matrix*)(new od_matrix_dense(dim_rows, dim_rows));
	SysJacBDF = new od_matrix_bdf(2*tree_ndofs, dim_rows - 2*tree_ndofs);
	pJac = SysJac->values();
	return 1;
}
int od_equation_bdf_I::solve(double tinu) {
	int i, errorCode = 0;
	double mu = 1.0 / tinu;
	double s = pSys->startRecord();
	int intTemp = 2 * tree_ndofs;
	//scaling the diff rhs because the the Jac scaling
	for (i = 0; i < intTemp; i++) pRhs[i] *=mu;
	
	//SysJac->print_out();
	pRhs = SysJac->solve(pRhs, reEval, effDim, permuV);
	reEval = 0;
	for (i = intTemp; i < dim_cols; i++) pRhs[i] /= mu;
	pSys->stopRecord(s, 2);
	return errorCode;
}
int od_equation_bdf_I::solveBDF(double tinu) {
	int i, errorCode = 0;
	double temp;
	double mu = 1.0 / tinu;
	double s = pSys->startRecord();
	int intTemp = 2 * tree_ndofs;
	//scaling the diff rhs because the the Jac scaling
	for (i = 0; i < tree_ndofs; i++) {
		temp = pprhs[i]; pprhs[i] = pprhs[i + tree_ndofs]; pprhs[i + tree_ndofs] = temp;
	}

	for (i = 0; i < intTemp; i++) { pprhs[i] *=mu; }
	//SysJac->print_out();
	pprhs = SysJacBDF->solve(pprhs, reEval);// , effDim, permuV);
	reEval = 0;
	for (i = intTemp; i < dim_cols; i++) pprhs[i] /=mu;
	pSys->stopRecord(s, 2);
	return errorCode;
}
int od_equation_hhti3::solve(double beta_hh) {
	int i, errorCode = 0;
	double s = pSys->startRecord();
	/*
	M           Phi^T   X    rhs1
						  =
	Phi*beta_hh  0      L    rhs2
	====>
	M           Phi^T  X      rhs1
						 =
	Phi            0   L    rhs2/beta_hh

	*/

	for (i = tree_ndofs; i < dim_cols; i++) pRhs[i] /= beta_hh;
	//scaling the diff rhs because the the Jac scaling
//	SysJac->print_out();
	pRhs = SysJac->solve(pRhs, reEval, effDim, permuV);
	reEval = 0;
	pSys->stopRecord(s, 2);
	return errorCode;
}
double* od_equation_bdf_I::evalRhs() {
	this->set_states();
	//this->_evalJac = 1;
	od_equation_dynamic::evalRhs(pRhs + tree_ndofs);
	//this->_evalJac = 0;
	int i, j, base;
	int intTemp = 2 * tree_ndofs;
	for (i = tree_ndofs; i < intTemp; i++) pRhs[i - tree_ndofs] += _X[i - tree_ndofs] - _Xdot[i]; //V - Udot

	for (i = 0; i < numLoops; i++) {
		base = intTemp + (i + numLoops) * 6;
		pSys->loop_list[i]->evaluate(1);
		for (j = 0; j < 6; j++) {
			if (pSys->loop_list[i]->if_redundant(j)) { pRhs[base + j] = 0.0; }
			else { pRhs[base + j] = -pSys->loop_list[i]->rhs(j); }
		}
	}
	for (i = 0; i < numLoops; i++) {
		base = intTemp + i * 6;
		for (j = 0; j < 6; j++) {
			if (pSys->loop_list[i]->if_redundant(j)) { pRhs[base + j] = 0.0; }
			else { pRhs[base + j] = -pSys->loop_list[i]->velRhs(j); }
		}
	}
	for (i = 0; i < intTemp; i++) {
		pRhs[i] = -pRhs[i]; pprhs[i] = pRhs[i];
	}
	for (i = 2 * tree_ndofs; i < 2 * tree_ndofs + 2 * 6 * numLoops; i++) pprhs[i] = pRhs[i];
	return pRhs;
}
double* od_equation_hhti3::evalRhs() {
	int i, j, base;
	double temp, ap1;
	ap1 = 1.0 + alpha;
	this->calMq();
	//RHS   M\ddot{q}+(1+alpha)*NonLinQ-alpha/(1+alpha)*pQ=0
	this->calNonLinQ();
	/*{//calclate inertail without velocity
		fill(pRhs, pRhs + dim_rows, 0.0);
		this->set_states(2);
		od_equation_dynamic::evalRhs(0, 1);
	}*/
	fill(pRhs, pRhs + dim_rows, 0.0);
	for (i = 0; i < tree_ndofs; i++) {
		pRhs[i] = Mq[i];//
		//temp = 0.0;
		//for (j = 0; j < tree_ndofs; j++) temp += M_a[i][j] * _Xddot[j];
		temp = Q[i] * ap1 - pQ[i] * alpha;
		pRhs[i] += temp;
	}
	for (i = 0; i < numLoops; i++) {
		base = tree_ndofs + (i*numLoops) * 6;
		pSys->loop_list[i]->evaluate(1);
		for (j = 0; j < 6; j++) {
			if (pSys->loop_list[i]->if_redundant(j)) { pRhs[base + j] = 0.0; }
			else { pRhs[base + j] = -pSys->loop_list[i]->rhs(j); }
		}
	}
	for (i = 0; i < tree_ndofs; i++) pRhs[i] = -pRhs[i];
	for (i = 0; i < dim_rows; i++) pprhs[i] = pRhs[i];
	return pRhs;
}
double* od_equation_dynamic::evalRhs(double *prhs, double alpha) {
	fill(pRhs, pRhs + dim_rows, 0.0);
	if (!prhs) prhs = pRhs;
	pSys->update();
	pSys->evaluate_rhs(prhs, alpha);
	return pRhs;
}
void od_system::numDif() {
	od_systemMechanism::numDif();
	if (pDyn) pDyn->numDif();
	else pDynHHT->numDif();
}
void od_equation_dynamic::numDif() {
	int i, j;
	double delta_ = 1.0e-6;
	double **_M, **_Md, **_Mv, *lrhs, *rrhs;
	this->evalJac();
	_M = new double*[tree_ndofs]; _Md = new double*[tree_ndofs]; _Mv = new double*[tree_ndofs];
	for (i = 0; i < tree_ndofs; i++) {
		_M[i] = new double[tree_ndofs]; _Md[i] = new double[tree_ndofs]; _Mv[i] = new double[tree_ndofs];
	}
	lrhs = new double[tree_ndofs];
	rrhs = new double[tree_ndofs];
	for (i = 0; i < tree_ndofs; i++) {
		fill(lrhs, lrhs + tree_ndofs, 0.0); fill(rrhs, rrhs + tree_ndofs, 0.0);
		states[i] += delta_;  pSys->set_states(); evalRhs(lrhs);
		states[i] -= 2.0 * delta_; pSys->set_states(); evalRhs(rrhs);
		states[i] += delta_; pSys->set_states();
		for (j = 0; j < tree_ndofs; j++) _Md[j][i] = (lrhs[j] - rrhs[j]) / (2.0*delta_);

		fill(lrhs, lrhs + tree_ndofs, 0.0); fill(rrhs, rrhs + tree_ndofs, 0.0);
		dstates[i] += delta_;  pSys->set_states(); evalRhs(lrhs);
		dstates[i] -= 2.0 * delta_; pSys->set_states(); evalRhs(rrhs);
		dstates[i] += delta_; pSys->set_states();
		for (j = 0; j < tree_ndofs; j++) _Mv[j][i] = (lrhs[j] - rrhs[j]) / (2.0*delta_);

		fill(lrhs, lrhs + tree_ndofs, 0.0); fill(rrhs, rrhs + tree_ndofs, 0.0);
		ddstates[i] += delta_;  pSys->set_states(); evalRhs(lrhs);
		ddstates[i] -= 2.0 * delta_; pSys->set_states(); evalRhs(rrhs);
		ddstates[i] += delta_; pSys->set_states();
		for (j = 0; j < tree_ndofs; j++) _M[j][i] = (lrhs[j] - rrhs[j]) / (2.0*delta_);

	}
	for (i = 0; i < tree_ndofs; i++) {
		for (j = 0; j < tree_ndofs; j++) {
			if (fabs(_M[i][j]) < SMALL_VALUE_T) _M[i][j] = 0.0;
			if (fabs(_Md[i][j]) < SMALL_VALUE_T) _Md[i][j] = 0.0;
			if (fabs(_Mv[i][j]) < SMALL_VALUE_T) _Mv[i][j] = 0.0;
		}
	}
	ofstream f_;// , *pf;
	f_.open("M.txt");
	f_ << scientific << setprecision(6);
	f_ << "M_num" << "=[" << endl;
	for (i = 0; i < tree_ndofs; i++) {
		for (j = 0; j < tree_ndofs; j++) { f_ << setw(9) << "   " << _M[i][j]; } f_ << endl;
	}
	f_ << "]" << endl;
	f_ << "M" << "=[" << endl;
	for (i = 0; i < tree_ndofs; i++) {
		for (j = 0; j < tree_ndofs; j++) { f_ << setw(9) << "   " << M_a[i][j]; } f_ << endl;
	}
	f_ << "]" << endl;

	f_ << "Md_num" << "=[" << endl;
	for (i = 0; i < tree_ndofs; i++) {
		for (j = 0; j < tree_ndofs; j++) { f_ << setw(9) << "   " << _Md[i][j]; } f_ << endl;
	}
	f_ << "]" << endl;
	f_ << "Md" << "=[" << endl;
	for (i = 0; i < tree_ndofs; i++) {
		for (j = 0; j < tree_ndofs; j++) { f_ << setw(9) << "   " << M_d[i][j]; } f_ << endl;
	}
	f_ << "]" << endl;
	f_ << "Md_diff" << "=[" << endl;
	for (i = 0; i < tree_ndofs; i++) {
		for (j = 0; j < tree_ndofs; j++) { f_ << setw(9) << "   " << M_d[i][j]- _Md[i][j]; } f_ << endl;
	}
	f_ << "]" << endl;
	f_ << "Mv_num" << "=[" << endl;
	for (i = 0; i < tree_ndofs; i++) {
		for (j = 0; j < tree_ndofs; j++) { f_ << setw(9) << "   " << _Mv[i][j]; } f_ << endl;
	}
	f_ << "]" << endl;

	f_ << "Mv" << "=[" << endl;
	for (i = 0; i < tree_ndofs; i++) {
		for (j = 0; j < tree_ndofs; j++) { f_ << setw(9) << "   " << M_v[i][j]; } f_ << endl;
	}
	f_ << "]" << endl;
	f_.close();
	DELARY(lrhs); DELARY(rrhs);
	for (i = 0; i < tree_ndofs; i++) { DELARY(_M[i]); DELARY(_Md[i]); DELARY(_Mv[i]); }
	DELARY(_M); DELARY(_Md); DELARY(_Mv);
}
//M\ddot{q}+Q=\Phi^T \lambda+ T
//M\ddot{q}+Q-\Phi^T\Lambda - T=0
//RHS   M\ddot{q}+(Q-\Phi^T\Lambda - T)*(1+alpha)-()*alpha=0
// NonLinQ = (Q-\Phi^T\Lambda - T)
//RHS   M\ddot{q}+NonLinQ*(1+alpha)-()*alpha=0
void od_equation_hhti3::calNonLinQ() {
	//---------------------------------------------------
//phs=-M*nlinear_acc+\Phi^T\Lambda + T=-(Q-\Phi^T\Lambda -T)
//(Q-\Phi^T\Lambda -T) = -rhs
	this->set_states(0);  //calculate with acc != 0, and then cut the linear inertia force Mq
	od_equation_dynamic::evalRhs();
	for (int i = 0; i < tree_ndofs; i++) Q[i] = -pRhs[i]-Mq[i];
}
void od_equation_hhti3::calMq() {
	//---------------------------------------------------
//rhs= -Mq
	this->set_states(2);  //zero \dot{q}
	od_equation_dynamic::evalRhs(0, alpha);
	for (int i = 0; i < tree_ndofs; i++) Mq[i] = -pRhs[i];
}
void od_equation_hhti3::updatepQ() {
	this->calMq();
	this->calNonLinQ();
	copy(Q, Q + tree_ndofs, pQ);
}
void od_equation_hhti3::calMa(double _h) {
	int i;
	double _time = pSys->time();
	if (_h > 0) h = _h;
	//double alpha = pIntegrator->getAlpha();
	pSys->time(_time + (1.0 + alpha)*h);
	for (i = 0; i < tree_ndofs; i++) {
		_X[i] += _Xdot[i] * (1.0 + alpha)*h;
		fill(M_a[i], M_a[i] + tree_ndofs, 0.0);
	}
	this->set_states(2); //zero \dot{q}
	pSys->update();
	pSys->updatePartials(1);
	pSys->getM(M_a);
	for (i = 0; i < tree_ndofs; i++) _X[i] -= _Xdot[i] * (1.0 + alpha)*h;
	pSys->time(_time);
	this->set_states();
}
void  od_equation_dynamic::evalJac(double alpha) {
	int i;
	pSys->update();
	pSys->updatePartials(0, alpha);
	for (i = 0; i < tree_ndofs; i++) {
		//if (!hht)fill(M_a[i], M_a[i] + tree_ndofs, 0.0);
		fill(M_a[i], M_a[i] + tree_ndofs, 0.0);
		fill(M_v[i], M_v[i] + tree_ndofs, 0.0);
		fill(M_d[i], M_d[i] + tree_ndofs, 0.0);
	}
	//if (!hht) pSys->getM(M_a);
    pSys->getM(M_a);
	for (i = 0; i < (int)(pSys->loop_list.size()); i++) pSys->loop_list[i]->evaluate(0);
	this->checkRedundancy();
	pSys->parF_parq_dot(M_v, alpha);
	pSys->parF_parq(M_d, alpha);
	reEval = 1;
}
void od_equation_hhti3::evalJac() {
	int i, j, ii, jj, base;
	double value, ap1;
	fill(dofmap.begin(), dofmap.end(), 1);
	for (i = 0; i < tree_ndofs; i++) {
		int val = pSys->dofmap_[i];
		if (val == 0) val = -1;
		this->dofmap[i] = val;
	}
	//RHS   M\ddot{q}+NonLinQ*(1+alpha)-()*alpha=0

	od_equation_dynamic::evalJac(alpha);
	ap1 = 1.0 + alpha;
	h = pIntegrator->getH();
	//for (i = 0; i < dim_rows; i++) fill(pJac[i], pJac[i] + dim_cols, 0.0);
	SysJac->zeros();
	SysJac->equalsub(tree_ndofs, tree_ndofs, M_a);
	//value = -h * gamma*ap1;
	SysJac->addsub(tree_ndofs, tree_ndofs, M_v, -h * gamma*ap1);
	//value = -h * h* beta*ap1;
	SysJac->addsub(tree_ndofs, tree_ndofs, M_d, -h * h* beta*ap1);
	
	/*for (i = 0; i < tree_ndofs; i++) {
		for (j = 0; j < tree_ndofs; j++) {
			pJac[i][j] = M_a[i][j];// / (1.0 + alpha);
			pJac[i][j] -= h * (gamma*M_v[i][j] + h * beta*M_d[i][j])*ap1;
		}
	}*/
	for (i = 0; i < numLoops; i++) {
		base = tree_ndofs;
		for (j = 0; j < pSys->loop_list[i]->num_nonzero(); j++) {
			ii = pSys->loop_list[i]->row(j) + base + i * 6;
			jj = pSys->loop_list[i]->col(j);
			value = pSys->loop_list[i]->jac(j);
			pJac[ii][jj] += value; //if (tempJac) tempJac[ii][jj] += value;//parV_parq_dot
			pJac[jj][ii] -= value; //if (tempJac) tempJac[jj][ii] -= value;//parV_parq_dot^T
		}
		for (ii = 0; ii < 6; ii++) {
			if (pSys->loop_list[i]->if_redundant(ii)) {
				dofmap[base + i * 6 + ii] = 0;
			}
		}
	}
	createPermuV();// dofmap);
	
}

void od_equation_hhti3::evalJacHHT(int __debug) {
	int i, j, ii, jj, base;
	double value, ap1;
	fill(dofmap.begin(), dofmap.end(), 1);
	for (i = 0; i < tree_ndofs; i++) {
		int val = pSys->dofmap_[i];
		if (val == 0) val = -1;
		this->dofmap[i] = val;
	}
	//RHS   M\ddot{q}/(1+alpha)+NonLinQ-()*alpha/(1+alpha)=0
	ap1 = 1.0 + alpha;
	h = pIntegrator->getH();
	od_equation_dynamic::evalJac(alpha);
	SysJacHHT->M()->equalsub(tree_ndofs, tree_ndofs, M_a);
	SysJacHHT->Mv()->equalsub(tree_ndofs, tree_ndofs, M_v, - h * gamma*ap1);
	SysJacHHT->Md()->equalsub(tree_ndofs, tree_ndofs, M_d, -h * h* beta*ap1);
	
	for (i = 0; i < numLoops; i++) {
		base = tree_ndofs;
		for (j = 0; j < pSys->loop_list[i]->num_nonzero(); j++) {
			ii = pSys->loop_list[i]->row(j) + base + i * 6;
			jj = pSys->loop_list[i]->col(j);
			value = pSys->loop_list[i]->jac(j);
			SysJacHHT->J()->addr(ii, jj, value);
			//pJac[ii][jj] += value; //if (tempJac) tempJac[ii][jj] += value;//parV_parq_dot
			//pJac[jj][ii] -= value; //if (tempJac) tempJac[jj][ii] -= value;//parV_parq_dot^T
		}
		for (ii = 0; ii < 6; ii++) {
			if (pSys->loop_list[i]->if_redundant(ii)) {
				dofmap[base + i * 6 + ii] = 0;
			}
		}
	}
	createPermuV();// dofmap);
	SysJacHHT->update(h, permuV, effDim);
}

void od_equation_bdf_I::evalJac(double tinu) {
	int i, j, base, ii, jj, baseV, val, tempint;
	double value, valueV;
	double s = pSys->startRecord();
	double mu = 1.0 / tinu;
	fill(dofmap.begin(), dofmap.end(), 1);
	for (i = 0; i < tree_ndofs; i++) {
		val = pSys->dofmap_[i];
		if (val == 0) val = -1;
		this->dofmap[i] = val;
		this->dofmap[tree_ndofs + i] = val; // pSys->dofmap[i];
	}
	od_equation_dynamic::evalJac();
	//clear the Jacobian
	for (i = 0; i < dim_rows; i++) fill(pJac[i], pJac[i] + dim_cols, 0.0);

	for (i = 0; i < tree_ndofs; i++) {
		for (j = 0; j < tree_ndofs; j++) {
			pJac[i + tree_ndofs][j] = -M_a[i][j]; 
			pJac[i + tree_ndofs][j] += M_v[i][j] * mu;// / tinu;
			pJac[i + tree_ndofs][j + tree_ndofs] += M_d[i][j] * mu;// / tinu;
		}
		pJac[i][i + tree_ndofs] = -1;
		pJac[i][i] = mu;// 1.0 / tinu;
	}
	base = 2 * tree_ndofs;
	baseV = base + numLoops * 6;
	for (i = 0; i < numLoops; i++) {
		tempint = pSys->loop_list[i]->num_nonzero();
		for (j = 0; j < tempint; j++) {
			ii = pSys->loop_list[i]->row(j) + base + i * 6;
			jj = pSys->loop_list[i]->col(j);
			value = pSys->loop_list[i]->jac(j);
			valueV = pSys->loop_list[i]->jacV(j);
			pJac[ii][jj] += value; 
			pJac[jj][ii] += value; 
			pJac[ii][jj + tree_ndofs] += valueV;
			ii = pSys->loop_list[i]->row(j) + baseV + i * 6;
			jj = pSys->loop_list[i]->col(j) + tree_ndofs;
			pJac[ii][jj] += value;
			pJac[jj][ii] += value;
		}
		for (ii = 0; ii < 6; ii++) {
			if (pSys->loop_list[i]->if_redundant(ii)) {
				dofmap[base + i * 6 + ii] = 0;
				dofmap[baseV + i * 6 + ii] = 0;
			}
		}
	}
	createPermuV();// dofmap);
	pSys->stopRecord(s, 1);
	return;
}
void od_equation_bdf_I::evalJacBdf(double tinu) {
	int i, j, base, ii, jj, baseV, val, tempint;
	double value, valueV;
	od_matrix_dense3C *pJ, *pJv;
	fill(dofmap.begin(), dofmap.end(), 1);
	for (i = 0; i < tree_ndofs; i++) {
		val = pSys->dofmap_[i];
		if (val == 0) val = -1;
		this->dofmap[i] = val;
		this->dofmap[tree_ndofs + i] = val; // pSys->dofmap[i];
	}
	od_equation_dynamic::evalJac();
	SysJacBDF->M()->equalsub(tree_ndofs, tree_ndofs, M_a);
	SysJacBDF->Mv()->equalsub(tree_ndofs, tree_ndofs, M_v);
	SysJacBDF->Md()->equalsub(tree_ndofs, tree_ndofs, M_d);
	pJ = SysJacBDF->J();
	pJv = SysJacBDF->Jv();
	base = 2 * tree_ndofs;
	baseV = base + numLoops * 6;
	for (i = 0; i < numLoops; i++) {
		tempint=pSys->loop_list[i]->num_nonzero();
		for (j = 0; j < tempint; j++) {
			ii = pSys->loop_list[i]->row(j) + i * 6;
			jj = pSys->loop_list[i]->col(j);
			value = pSys->loop_list[i]->jac(j);
			valueV = pSys->loop_list[i]->jacV(j);
			pJ->addr(ii, jj, value);
			pJv->addr(ii, jj, valueV);
		}
		for (ii = 0; ii < 6; ii++) {
			if (pSys->loop_list[i]->if_redundant(ii)) {
				dofmap[base + i * 6 + ii] = 0;
				dofmap[baseV + i * 6 + ii] = 0;
			}
		}
	}
	createPermuV();// dofmap);
	SysJacBDF->update(tinu,  permuV, effDim);
	//SysJacBDF->LU();
	//SysJacBDF->solve(pprhs, 1);
}
