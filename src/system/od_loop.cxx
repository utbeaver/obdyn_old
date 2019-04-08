#include "od_loop.h"
#include "od_system.h"


od_loop::od_loop(od_constraint *pC, od_systemGeneric *pS) {
	int i;
	initialized = 0;
	Fixed = (od_joint*)pC;
	pSys = pS;
	num_NZ = 0;
	for (i = 0; i < 6; i++) {
		redundant[i] = 0;
		redundantOld[i] = 0;
	}
	for (i = 0; i < 3; i++) {
		temp_rhs[i] = 0.0;
	}
	row_index = col_index = 0;
	//delocMem = 0;
	notFirst = 0;
}

int od_loopr::initialize(int keep) {
	if (keep) return 0;
	clean();
	int b_index[2], i, j, k;
	int *ij_idx[2];
	int entry2ij;
//	od_joint *pC;
	Vec3 *pTempV3;
	b_index[1] = Fixed->j_body_index();
	b_index[0] = Fixed->i_body_index();
	//find the lowing bound of I body
	int num_dofs = pSys->tree_dofs();
	int njoint = pSys->num_joint();
	for (i = 0; i < 2; i++) ij_idx[i] = new int[num_dofs];
	for (i = 0; i < 2; i++) {
		//get the entries of i(j) branches
		for (j = 0; j < num_dofs; j++) {
			//if (pSys->relevenceLevel2[b_index[i] * num_dofs + j] != 0) {
			if (pSys->getL2(b_index[i] * num_dofs + j) != 0) {
				// entries2[i].push_back(j);
				ij_idx[i][j] = 1;
			}
			else {
				ij_idx[i][j] = 0;
			}
		}
	}
	for (i = 0; i < num_dofs; i++) {
		if (ij_idx[0][i] != ij_idx[1][i]) {
			if (ij_idx[0][i]) entries2[0].push_back(i);
			if (ij_idx[1][i]) entries2[1].push_back(i);
		}
	}
	for (i = 0; i < 2; i++) {
		for (j = 0; j < njoint; j++) {
			//pC = (od_joint*) (pSys->get_constraint_via_index(j));
			//if (pSys->relevenceLevel1[b_index[i] * njoint + j] != 0) {
			if (pSys->getL1(b_index[i] * njoint + j) != 0) {
				//entries1[i].push_back(j);
				ij_idx[i][j] = 1;
			}
			else {
				ij_idx[i][j] = 0;
			}
		}
	}
	for (i = 0; i < njoint; i++) {
		if (ij_idx[0][i] != ij_idx[1][i]) {
			if (ij_idx[0][i]) entries1[0].push_back(i);
			if (ij_idx[1][i]) entries1[1].push_back(i);
		}
	}
	for (i = 0; i < 2; i++) delete[] ij_idx[i];
	num_NZ = 0;
	//calculate the number of non-zeros
	num_NZ += (int)entries2[0].size();
	num_NZ += (int)entries2[1].size();
	num_NZ *= 6;
	row_index = new int[num_NZ];
	col_index = new int[num_NZ];
	values = new double[num_NZ];
	valuesV = new double[num_NZ];

	//	fltTemp = values;
	int counter = 0;
	for (i = 0; i < 2; i++) {
		int temp_int = (int)entries2[i].size();
		JR_disp[i].resize(temp_int);
		JR_vel[i].resize(temp_int);
		JR_veld[i].resize(temp_int);
		for (j = 0; j < temp_int; j++) {
			entry2ij = entries2[i][j];
			JR_disp[i][j] = new Vec3;
			pTempV3 = pSys->getJR(b_index[i], entry2ij); //JR.element(b_index[i], entry2ij);
			if (pTempV3 == 0) { pTempV3 = new Vec3; collector.push_back(pTempV3); }
			JR_vel[i][j] = pTempV3;
			pTempV3 = pSys->getJRdot_dq(b_index[i], entry2ij);// parOmega_parq.element(b_index[i], entry2ij);
			JR_veld[i][j] = pTempV3;

			for (k = 0; k < 3; k++) {
				row_index[counter] = k;
				col_index[counter++] = entry2ij;
			}
		}
		JT_disp[i].resize(temp_int);
		JT_vel[i].resize(temp_int);
		JT_veld[i].resize(temp_int);
		for (j = 0; j < temp_int; j++) {
			entry2ij = entries2[i][j];
			pTempV3 = pSys->getJT(b_index[i], entry2ij); //JTG.element(b_index[i], entry2ij);
			if (pTempV3 == 0) { pTempV3 = new Vec3; collector.push_back(pTempV3); }
			JT_disp[i][j] = pTempV3;
			JT_vel[i][j] = pTempV3;
			pTempV3 = pSys->getJTdot_dq(b_index[i], entry2ij); //parVel_parqG.element(b_index[i], entry2ij);
			JT_veld[i][j] = pTempV3;

			for (k = 0; k < 3; k++) {
				row_index[counter] = k + 3;
				col_index[counter++] = entry2ij;
			}
		}
	}
	redundant_cons.clear();
	return 1;
}


int od_loopv::initialize(int keep) {
	if (keep) return 0;
	clean();
	int b_index[2], i, j, k;

	int entry2ij;
//	od_joint *pC;
	Vec3 *pTempV3;
	b_index[1] = Fixed->j_body_index();
	b_index[0] = Fixed->i_body_index();
	//find the lowing bound of I body
	int num_dofs = pParent->tree_dofs();
	int njoint = pParent->num_joint();



	for (j = 0; j < num_dofs; j++) {
		if (pParent->getL2(b_index[1] * num_dofs + j) != 0) {
			entries2[1].push_back(j);
		}
	}

	for (i = 0; i < 6; i++) {
		entries2[0].push_back(pSys->startIndex() + 6 + i);
	}

	for (j = 0; j < njoint; j++) {
		//if (pParent->relevenceLevel1[b_index[1] * njoint + j] != 0) {
		if (pParent->getL1(b_index[1] * njoint + j) != 0) {
			entries1[1].push_back(j);
		}
	}

	//for (i = 0; i < 6; i++) {
	entries1[0].push_back(pSys->startBIndex());
	//}

	num_NZ = 0;
	//calculate the number of non-zeros
	num_NZ += (int)entries2[0].size();
	num_NZ += (int)entries2[1].size();
	num_NZ *= 6;
	row_index = new int[num_NZ];
	col_index = new int[num_NZ];
	values = new double[num_NZ];
	valuesV = new double[num_NZ];

	//	fltTemp = values;
	int counter = 0;
	for (i = 1; i < 2; i++) {
		int temp_int = (int)entries2[i].size();
		JR_disp[i].resize(temp_int);
		JR_vel[i].resize(temp_int);
		JR_veld[i].resize(temp_int);
		for (j = 0; j < temp_int; j++) {
			entry2ij = entries2[i][j];
			JR_disp[i][j] = new Vec3;
			if (i == 1) {
				pTempV3 = pParent->getJR(b_index[i], entry2ij); //JR.element(b_index[i], entry2ij);
			}
			else {
				pTempV3 = pSys->getJR(b_index[i] - pSys->startBIndex(), entry2ij);
			}
			if (pTempV3 == 0) { pTempV3 = new Vec3; collector.push_back(pTempV3); }
			JR_vel[i][j] = pTempV3;
			if (i == 1) {
				pTempV3 = pParent->getJRdot_dq(b_index[i], entry2ij); //parOmega_parq.element(b_index[i], entry2ij);
			}
			else {
				pTempV3 = pSys->getJRdot_dq(b_index[i] - pSys->startBIndex(), entry2ij);
			}
			JR_veld[i][j] = pTempV3;

			for (k = 0; k < 3; k++) {
				row_index[counter] = k;
				col_index[counter++] = entry2ij;
			}
		}
		JT_disp[i].resize(temp_int);
		JT_vel[i].resize(temp_int);
		JT_veld[i].resize(temp_int);
		for (j = 0; j < temp_int; j++) {
			entry2ij = entries2[i][j];
			if (i == 1) {
				pTempV3 = pParent->getJT(b_index[i], entry2ij); // JTG.element(b_index[i], entry2ij);
			}
			else {
				pTempV3 = pSys->getJT(b_index[i] - pSys->startBIndex(), entry2ij);
			}
			if (pTempV3 == 0) {
				pTempV3 = new Vec3;
				collector.push_back(pTempV3);
			}
			JT_disp[i][j] = pTempV3;
			JT_vel[i][j] = pTempV3;
			if (i == 1) pTempV3 = pParent->getJTdot_dq(b_index[i], entry2ij); //parVel_parqG.element(b_index[i], entry2ij);
			else pTempV3 = pSys->getJTdot_dq(b_index[i] - pSys->startBIndex(), entry2ij);
			JT_veld[i][j] = pTempV3;

			for (k = 0; k < 3; k++) {
				row_index[counter] = k + 3;
				col_index[counter++] = entry2ij;
			}
		}
	}
	redundant_cons.clear();
	return 1;
}

void od_loop::clean() {
	int i, temp_int, j;
	for (i = 0; i < (int)collector.size(); i++) delete collector[i];
	collector.resize(0);
	if (num_NZ) {
		DELARY(row_index); DELARY(col_index);  DELARY(values); DELARY(valuesV);
	}
	for (i = 0; i < 2; i++) {
		entries1[i].resize(0);
		entries2[i].resize(0);
	}
	redundant_cons.resize(0);
	//if (delocMem) {
	//	delocMem = 0;
	for (i = 0; i < 2; i++) {
		temp_int = (int)entries2[i].size();
		for (j = 0; j < temp_int; j++) {
			delete JR_disp[i][j];
			//delete JR_veld[i][j];
			//delete JT_veld[i][j];
		}
	}
	//}
}

od_loop::~od_loop() {
	clean();
}

void od_loop::init() {
	unsigned i, j;
	for (i = 0; i < 2; i++) {
		for (j = 0; j < entries2[i].size(); j++) {
			JR_disp[i][j]->init();
		}
	}
}

double od_loop::element(int i, int j, int ij, int si2) {
	Vec3 *pV=0;
	od_object::JAC_TYPE __type = pSys->get_jac_type();
	double fltTemp = 0.0;
	if (i < 3) {
		if (
			__type == od_object::JAC_INIT_DISP ||
			__type == od_object::JAC_DISP ||
			__type == od_object::JAC_STATIC ||
			__type == od_object::JAC_DYNAMIC) {
			pV = JR_disp[ij][j];
			//if (si2) pV= JR_veld[ij][j];
		}
		else if (__type == od_object::JAC_VEL || __type == od_object::JAC_ACC) {
			pV = JR_vel[ij][j];
		}
		fltTemp += (pV->v)[i];
	}
	else {
		if (
			__type == od_object::JAC_INIT_DISP ||
			__type == od_object::JAC_DISP ||
			__type == od_object::JAC_STATIC ||
			__type == od_object::JAC_DYNAMIC) {
			pV = JT_disp[ij][j];
			//if (si2) pV = JT_veld[ij][j];
		}
		else if (__type == od_object::JAC_VEL || __type == od_object::JAC_ACC) {
			pV = JT_vel[ij][j];
		}
		fltTemp += (pV->v)[i - 3];
	}
	fltTemp = (ij == 0) ? fltTemp : -fltTemp;
	return fltTemp;
}

void od_loop::find_redundants() {
	int i, j, temp_int;
	int six = 6;
	double *temp_mat = 0;
	int total_leni = (int)entries2[0].size();
	int total_lenj = (int)entries2[1].size();
	int total_len = total_leni + total_lenj;
	redundant_cons.clear();
	temp_int = total_len * 6;
	temp_mat = new double[temp_int];
	fill(temp_mat, temp_mat + temp_int, 0.0);
	for (j = 0; j < total_leni; j++) {
		for (i = 0; i < 6; i++) {
			temp_mat[i*total_len + j] = element(i, j, 0);
		}
	}
	for (j = 0; j < total_lenj; j++) {
		for (i = 0; i < 6; i++) {
			temp_mat[i*total_len + total_leni + j] = element(i, j, 1);
		}
	}
	zero_pivot(six, total_len, temp_mat, redundant_cons);
	DELARY(temp_mat);
}

void od_loop::zero_pivot(int dim_i, int dim_j,
	double* mat, vector<int>& pivots) {
	int i, j, k;
	int row_max, col_max;
	double temp_d, temp_d_inv;
	int *permu_row = new int[dim_i];
	int *permu_col = new int[dim_j];
	for (i = 0; i < dim_i; i++) permu_row[i] = i;
	for (i = 0; i < dim_j; i++) permu_col[i] = i;
	for (i = 0; i < dim_i; i++) {
		//find the biggest element
		temp_d = 0.0;
		for (j = i; j < dim_i; j++) {
			for (k = i; k < dim_j; k++) {
				if (fabs(mat[permu_row[j] * dim_j + permu_col[k]]) > temp_d) {
					temp_d = fabs(mat[permu_row[j] * dim_j + permu_col[k]]);
					row_max = j;
					col_max = k;
				}
			}
		}
		//if temp_d is very small, 
		//we found the redundant constraints down ward
		if (temp_d < PIVOT_TOLERANCE) {
			for (j = i; j < dim_i; j++) {
				pivots.push_back(permu_row[j]);
			}
			i = dim_i;
		}
		else {
			// switch elements so the max will 
			//stay at the left_upper corder
			k = permu_row[i]; permu_row[i] = permu_row[row_max];
			permu_row[row_max] = k;
			k = permu_col[i]; permu_col[i] = permu_col[col_max];
			permu_col[col_max] = k;
			//after element switch, zero the ith column
			temp_d_inv = 1.0 / temp_d;
			//temp_d_inv = 1.0 / mat[permu_row[i] * dim_j + permu_col[i]];
			for (j = i + 1; j < dim_i; j++) {
				temp_d = mat[permu_row[j] * dim_j + permu_col[i]];
				//if (fabs(temp_d) > PIVOT_TOLERANCE) {
				temp_d *= temp_d_inv;
				mat[permu_row[j] * dim_j + permu_col[i]] = 0.0;
				for (k = i + 1; k < dim_j; k++) {
					mat[permu_row[j] * dim_j + permu_col[k]]
						-= temp_d * mat[permu_row[i] * dim_j + permu_col[k]];
				}
				//}
			}
		}
	}
	DELARY(permu_row);
	DELARY(permu_col);
}

void od_loopv::evaluate_orientation(int rhs_only) {
	unsigned i, j, k, jdofs;//, ij;
	//int nbody, njoint;
	int index[2];//, start_index;
	Vec3 Vi0, Vj0;
	Vec3 Vi1, Vj1;
	Vec3 Vi2, Vj2;
	Vec3 *pV;
	od_joint *pC;
	double	  temp_d[3], temp_d1[3];// , temp_d2[3];
	int temp_counter;
	double		 *pd1 = 0;
	//nbody = pParent->num_body();
	//njoint = pParent->num_joint();
	//find the two bodies of Fixed joint
	index[0] = Fixed->i_body_index();
	index[1] = Fixed->j_body_index();
	od_marker* pMi = Fixed->get_imarker();
	od_marker* pMj = Fixed->get_jmarker();
	od_object::JAC_TYPE __type = pSys->get_jac_type();
	if (__type == od_object::JAC_INIT_DISP ||
		__type == od_object::JAC_DISP ||
		__type == od_object::JAC_STATIC ||
		__type == od_object::JAC_DYNAMIC) {
		/*angular constraint
		I_1 \dot J_2 = 0*/
		Vi0 = pMi->get_axis_global(Vi0, 0);
		Vj1 = pMj->get_axis_global(Vj1, 1);
		temp_d[2] = Vi0 * Vj1;
		/* J_1 \dot K_2 = 0*/
		Vi1 = pMi->get_axis_global(Vi1, 1);
		Vj2 = pMj->get_axis_global(Vj2, 2);
		temp_d[0] = Vi1 * Vj2;
		/*K_1 \dot I_2 = 0*/
		Vi2 = pMi->get_axis_global(Vi2, 2);
		Vj0 = pMj->get_axis_global(Vj0, 0);
		temp_d[1] = Vi2 * Vj0;
		//EQ3(temp_d1, temp_d);
		ori_rhs = pMj->vec_wrt_ground(temp_d);
		if (__type == od_object::JAC_DYNAMIC) {
			pMi->get_omega(omega_rhs, pMj);
		}
		//if it is not for init_disp, 
		//no need to evaluate partial derivatives
		//if only rhs evaluation is specified, 
		//skip partial derivative eval. 
		if (rhs_only == 0) {
			if (__type == od_object::JAC_INIT_DISP ||
				__type == od_object::JAC_DISP ||
				__type == od_object::JAC_STATIC   // ||__type == od_object::JAC_DYNAMIC
				) {
				init();
				Vj1 = Vi0 ^ Vj1;
				Vj2 = Vi1 ^ Vj2;
				Vj0 = Vi2 ^ Vj0;
				for (k = 0; k < 2; k++) {
					temp_counter = 0;
					double *pTemp = 0;
					for (j = 0; j < entries1[k].size(); j++) {
						if (k == 1) {
							pC = (od_joint*)(pParent->get_constraint_via_index(entries1[k][j]));
						} else {
							pC = (od_joint*)(pSys->get_constraint_via_index(entries1[k][j]));
						}
						jdofs = pC->dofs();
						int numtra = pC->num_tra();
						pTemp = pC->get_zi_global();
						for (i = 0; i < (unsigned)numtra; i++) {
							pV = JR_disp[k][temp_counter++];
							pV->init();
						}
						for (i = numtra; i < jdofs; i++) {
							pV = JR_disp[k][temp_counter++];
							vecTemp = (pTemp + 3 * i);
							temp_d[2] = vecTemp * Vj1;
							temp_d[0] = vecTemp * Vj2;
							temp_d[1] = vecTemp * Vj0;
							(*pV) = pMj->vec_wrt_ground(temp_d);
							if (k == 1) {
								EQ3(temp_d1, ori_rhs.v);
								(*pV) += vecTemp ^ temp_d1;
							}
							//the sign issue has been taken care of 
							//in element function internally	 
							//in evaluate fucntion externally
						}
					}
				}
			}
		}
	}
	else if (pSys->get_jac_type() == od_object::JAC_VEL) {
		//eva rhs
		pMi->get_omega(omega_rhs, pMj);
		//eval jac

	}
	else if (pSys->get_jac_type() == od_object::JAC_ACC) {
		//eval rhs
		pMi->get_omega_dot(temp_d, pMj);
		domega_rhs = temp_d;
	}
	return;
}

void od_loopr::evaluate_orientation(int rhs_only) {
	unsigned i, j, k, jdofs;//, ij;
	//int nbody, njoint;
	int index[2];//, start_index;
	Vec3 Vi0, Vj0;
	Vec3 Vi1, Vj1;
	Vec3 Vi2, Vj2;
	Vec3 *pV;
	od_joint *pC;
	double	  temp_d[3], temp_d1[3];// , temp_d2[3];
	int temp_counter;
	double		 *pd1 = 0;
	//nbody = pSys->num_body();
	//njoint = pSys->num_joint();
	//find the two bodies of Fixed joint
	index[0] = Fixed->i_body_index();
	index[1] = Fixed->j_body_index();
	od_marker* pMi = Fixed->get_imarker();
	od_marker* pMj = Fixed->get_jmarker();
	od_object::JAC_TYPE __type = pSys->get_jac_type();
	if (__type == od_object::JAC_INIT_DISP ||
		__type == od_object::JAC_DISP ||
		__type == od_object::JAC_STATIC ||
		__type == od_object::JAC_DYNAMIC) {
		/*angular constraint
		I_1 \dot J_2 = 0*/
		Vi0 = pMi->get_axis_global(Vi0, 0);
		Vj1 = pMj->get_axis_global(Vj1, 1);
		temp_d[2] = Vi0 * Vj1;
		/* J_1 \dot K_2 = 0*/
		Vi1 = pMi->get_axis_global(Vi1, 1);
		Vj2 = pMj->get_axis_global(Vj2, 2);
		temp_d[0] = Vi1 * Vj2;
		/*K_1 \dot I_2 = 0*/
		Vi2 = pMi->get_axis_global(Vi2, 2);
		Vj0 = pMj->get_axis_global(Vj0, 0);
		temp_d[1] = Vi2 * Vj0;
		//EQ3(temp_d1, temp_d);
		ori_rhs = pMj->vec_wrt_ground(temp_d);
		if (__type == od_object::JAC_DYNAMIC) {
			pMi->get_omega(omega_rhs, pMj);
		}
		//if it is not for init_disp, 
		//no need to evaluate partial derivatives
		//if only rhs evaluation is specified, 
		//skip partial derivative eval. 
		if (rhs_only == 0) {
			if (__type == od_object::JAC_INIT_DISP ||
				__type == od_object::JAC_DISP ||
				__type == od_object::JAC_STATIC //||__type == od_object::JAC_DYNAMIC
				) {
				init();
				Vj1 = Vi0 ^ Vj1;
				Vj2 = Vi1 ^ Vj2;
				Vj0 = Vi2 ^ Vj0;
				for (k = 0; k < 2; k++) {
					temp_counter = 0;
					double *pTemp = 0;
					for (j = 0; j < entries1[k].size(); j++) {
						pC = (od_joint*)(pSys->get_constraint_via_index(entries1[k][j]));
						jdofs = pC->dofs();
						int numtra = pC->num_tra();
						pTemp = pC->get_zi_global();
						for (i = 0; i < (unsigned)numtra; i++) {
							pV = JR_disp[k][temp_counter++];
							pV->init();
						}
						for (i = numtra; i < jdofs; i++) {
							pV = JR_disp[k][temp_counter++];
							vecTemp = (pTemp + 3 * i);
							temp_d[2] = vecTemp * Vj1;
							temp_d[0] = vecTemp * Vj2;
							temp_d[1] = vecTemp * Vj0;
							(*pV) = pMj->vec_wrt_ground(temp_d);
							if (k == 1) {
								EQ3(temp_d1, ori_rhs.v);
								(*pV) += vecTemp ^ temp_d1;
							}
							//the sign issue has been taken care of 
							//in element function internally	 
							//in evaluate fucntion externally
						}
					}
				}
			}
		}
	}
	else if (pSys->get_jac_type() == od_object::JAC_VEL) {
		//eva rhs
		pMi->get_omega(omega_rhs, pMj);
		//eval jac

	}
	else if (pSys->get_jac_type() == od_object::JAC_ACC) {
		//eval rhs
		pMi->get_omega_dot(temp_d, pMj);
		domega_rhs = temp_d;
	}
	return;
}

void od_loop::evaluate_position() {
	int i;//,j, _size, start_index, ij;
	double temp_d[3], *pd;//, temp_u;
	double	 temp_d1[3], *pd1 = 0;
	Vec3 *pV = 0;
	int index[2];
	index[0] = Fixed->i_body_index();
	index[1] = Fixed->j_body_index();
	od_marker* pMi = Fixed->get_imarker();
	od_marker* pMj = Fixed->get_jmarker();
	pd = temp_d;
	pd1 = temp_d1;
	od_object::JAC_TYPE __type = pSys->get_jac_type();
	for (i = 0; i < 3; i++) { temp_d[i] = temp_d1[i] = temp_rhs[i] = 0.0; }

	if (__type == od_object::JAC_INIT_DISP ||
		__type == od_object::JAC_DISP ||
		__type == od_object::JAC_STATIC ||
		__type == od_object::JAC_DYNAMIC) {
		pos_rhs = pMi->get_position(pd, pMj);
		if (pSys->get_jac_type() == od_object::JAC_DYNAMIC) {
			pd1 = temp_d1;
			pMi->get_velocity(pd1, pMj);
			vel_rhs = pd1;
		}
	} else if (__type == od_object::JAC_VEL) {
		pd1 = temp_d1;
		pMi->get_velocity(pd1, pMj);
		vel_rhs = pd1;
	} else if (__type == od_object::JAC_ACC) {
		pd1 = temp_d1;
		pMi->get_acceleration(pd1, pMj);
		acc_rhs = pd1;
	}
}

int od_loopr::checkRedundancy() {
	int i;
	find_redundants();
	int temp_int = num_of_redundant_constraints();
	for (i = 0; i < 6; i++) {
		redundantOld[i] = redundant[i];
		redundant[i] = 0;
	}
	for (i = 0; i < temp_int; i++) {
		redundant[redundant_cons[i]] = 1;
	}
	int flag = 0;
	for (i = 0; i < 6; i++) {
		if (redundantOld[i] != redundant[i] && notFirst) {
			flag = 1;
		}
	}
	notFirst = 1;
	return flag;
}

void od_loop::evaluate(int rhs_only) {
	int i, j, temp_int;
	double *fltTemp;
	double *fltTempV;
	int counter = 0;
	JAC_TYPE jtype = pSys->get_jac_type();
	evaluate_orientation(rhs_only);
	evaluate_position();
	if (rhs_only) return;

	//double temp_d[3];

	fltTemp = values;
	fltTempV = valuesV;
	for (i = 0; i < 2; i++) {
		temp_int = (int)entries2[i].size();
		for (j = 0; j < temp_int; j++) {
			if (
				jtype == od_object::JAC_INIT_DISP ||
				jtype == od_object::JAC_DISP ||
				jtype == od_object::JAC_STATIC) {
				fltTemp = JR_disp[i][j]->to_double(fltTemp);
			}
			else if (pSys->get_jac_type() == od_object::JAC_VEL ||
				jtype == od_object::JAC_ACC) {
				fltTemp = JR_vel[i][j]->to_double(fltTemp);
			}
			else if (jtype == od_object::JAC_DYNAMIC) {
				fltTemp = /*JR_disp*/ JR_vel[i][j]->to_double(fltTemp);//use JR instead of JR_disp as calculated 
				fltTempV = JR_veld[i][j]->to_double(fltTempV);
			}
			if (i == 1) {
				for (int k = 0; k < 3; k++) {
					fltTemp[k] = -fltTemp[k];
					if (jtype == od_object::JAC_DYNAMIC) fltTempV[k] = -fltTempV[k];
				}
			}
			fltTemp += 3;
			fltTempV += 3;
			counter += 3;
		}
		for (j = 0; j < temp_int; j++) {
			//ZERO3(temp_d);
			if (
				jtype == od_object::JAC_INIT_DISP ||
				jtype == od_object::JAC_DISP ||
				jtype == od_object::JAC_STATIC) {
				fltTemp = JT_disp[i][j]->to_double(fltTemp);

			}
			else if (jtype == od_object::JAC_VEL ||
				jtype == od_object::JAC_ACC) {
				fltTemp = JT_vel[i][j]->to_double(fltTemp);
				//fltTemp = pMj->vec_wrt_this(fltTemp);
			}
			else if (jtype == od_object::JAC_DYNAMIC) {
				fltTemp = JT_disp[i][j]->to_double(fltTemp);
				fltTempV = JT_veld[i][j]->to_double(fltTempV);
			}
			if (i == 1) {
				for (int k = 0; k < 3; k++) {
					fltTemp[k] = -fltTemp[k];
					if (jtype == od_object::JAC_DYNAMIC) fltTempV[k] = -fltTempV[k];
				}
				//U_ADD3(fltTemp, temp_d);
			}
			fltTempV += 3;
			fltTemp += 3;
			counter += 3;
		}
	}

	return;
}


/*double* od_loop::parMF_parq(int i0j1, int idx, double *vals) {
	double* pd = vals;
	Vec3* pTempV3 = JR_disp[i0j1][idx];
	cross_product(*pTempV3, _moment[i0j1], vecTemp);
	pd = vecTemp.to_double(pd);
	pd += 3;
	cross_product(*pTempV3, _force[i0j1], vecTemp);
	pd = vecTemp.to_double(pd);
	return vals;
}*/

