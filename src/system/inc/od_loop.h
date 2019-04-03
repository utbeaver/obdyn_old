
#ifndef OD_LOOP_H
#define OD_LOOP_H
#include "od_constraint.h"
#include "mat33.h"

#define PIVOT_TOLERANCE 1.0E-12

class od_systemMechanism;
class od_systemGeneric;

class od_loop : public od_object {
protected:
	//int low_bound[2], up_bound[2];
	vector<int> entries1[2];
	vector<int> entries2[2];

	vector<Vec3*> JR_disp[2];
	vector<Vec3*> JT_disp[2];

	vector<Vec3*> JR_vel[2];
	vector<Vec3*> JT_vel[2];

	vector<Vec3*> JR_veld[2];
	vector<Vec3*> JT_veld[2];

	vector<Vec3*> collector;
	Vec3 ori_rhs, vel_rhs, acc_rhs;
	Vec3 pos_rhs, omega_rhs, domega_rhs;
	double pos_rhs_[3];
	Vec3 _moment[2];
	Vec3 _force[2];
	Vec3 _moment_si2[2];
	Vec3 _force_si2[2];
	double* lambda;
	double* lambda_si2;
	od_joint *Fixed;
	od_systemGeneric *pSys, *pParent;
	int initialized, notFirst;
	vector<int> redundant_cons;
	void zero_pivot(int dim_i, int dim_j,
		double* mat, vector<int>& pivots);
	//the init function is used in INIT_DISP mode.
	int nz_start_index, nz_end_index;
	int num_NZ;
	int *row_index;
	int *col_index;
	double *values;
	double *valuesV;
	int redundant[6], redundantOld[6];
	void init();
	virtual void evaluate_orientation(int rhs_only = 1)=0;
	void evaluate_position();
	Vec3 vecTemp;
	double temp_rhs[3];
	double element(int i, int j, int ij = 0, int si2 = 0);
	void clean();
	void find_redundants();

public:
	friend class od_systemGeneric;
	od_loop(od_constraint* pC, od_systemGeneric*);
	~od_loop();
	virtual int initialize(int = 0)=0;
	inline int num_nonzero() const { return num_NZ; }
	inline int get_i_index() const { return Fixed->i_body_index(); }
	inline int get_j_index() const { return Fixed->j_body_index(); }
	inline int get_num_entries(int i) const { return (int)entries2[i].size(); }
	virtual int checkRedundancy()=0;
	inline int num_of_redundant_constraints() const { return (int)redundant_cons.size(); }
	inline double jac(int i) const { return values[i]; }
	inline double jacV(int i) const { return valuesV[i]; }
	inline double* force(int i = 0) { return _force[i].v; }
	inline double* force_si2(int i = 0) { return _force_si2[i].v; }
	inline double* moment(int i = 0) { return _moment[i].v; }
	inline double* moment_si2(int i = 0) { return _moment_si2[i].v; }
	inline int row(int i) const { return row_index[i]; }
	inline int col(int i) const { return col_index[i]; }
	void evaluate(int rhs_only = 0);
	inline double rhs(int i)  const { return (i < 3) ? ori_rhs.v[i] : pos_rhs.v[i - 3]; }
	inline double velRhs(int i) const { return (i < 3) ? omega_rhs.v[i] : vel_rhs.v[i - 3]; }
	inline double accRhs(int i) const { return (i < 3) ? domega_rhs.v[i] : acc_rhs.v[i - 3]; }
	inline int if_redundant(int i) const { return redundant[i]; }

	double* set_lambda(double* lamb, int inc = 1) {
		double* p = lamb + 3;
		if (inc) { _force[0] += p; 		_moment[0] += lamb; }
		else { _force[0] = p; 		_moment[0] = lamb; }
		_force[1] = _force[0]; _force[1].negate();
		_moment[1] = _moment[0]; _moment[1].negate();
		return lamb + 6;
	}
	void set_temp_pva(double *p_, double *v=0, double *a = 0) {
		lambda = p_;
		lambda_si2 = v;
	}
	void init_lambda() {
		_force[0].init(); _force[1].init();
		_moment[0].init(); _moment[1].init();
	}

	double* set_lambda_si2(double* lamb, int inc = 1) {
		double* p = lamb + 3;
		if (inc) { _force_si2[0] += p;		_moment_si2[0] += lamb; }
		else { _force_si2[0] = p;		_moment_si2[0] = lamb; }
		_force_si2[1] = _force_si2[0]; _force_si2[1].negate();
		_moment_si2[1] = _moment_si2[0]; _moment_si2[1].negate();
		return lamb + 6;
	}
	// double* parMF_parq(int i0j1, int idx, double *vals);
};
class od_loopr : public od_loop {
protected:
public:
	od_loopr(od_constraint* pC, od_systemGeneric* pS):od_loop(pC, pS) {}
	virtual int initialize(int = 0);	
	virtual void evaluate_orientation(int rhs_only = 1);
	virtual int checkRedundancy();

};
class od_loopv : public od_loop {
public:
	od_loopv(od_constraint* pC, od_systemGeneric* pS, od_systemGeneric* pP):od_loop(pC, pS) {
		pParent = pP;
	}
	virtual int initialize(int = 0);
	virtual void evaluate_orientation(int rhs_only = 1);
	virtual int checkRedundancy() { return 0; }
};
#endif
