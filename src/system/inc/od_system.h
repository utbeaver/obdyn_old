#ifndef OD_SYSTEM_H
#define OD_SYSTEM_H
//disable warnings on 255 char debug symbols
//#pragma warning (disable : 4786)
//disable warnings on extern before template instantiation
//#pragma warning (disable : 4231)
#include <string>
#include <functional>
#include <iomanip>
#include <fstream>

#include "od_body.h"
#include "od_constraint.h"
#include "od_jacobian.h"
#include "od_system_api.h"
#include "expr.h"
#include "od_force.h"
#include "od_measure.h"
#include "od_subsystem.h"

#define parOmega_parq_dot JR
#define parVel_parq_dotG JTG
using namespace std;
class od_loopr;
class od_loop;
class od_measure;
class od_equation_kinematic;
class od_equation_bdf_I;
class od_equation_hhti3;


extern "C" void multiplyMatT_vec(const int num_rows, Vec3* const Input, Vec3* const Output, od_constraint**   prev);
extern "C" void multiplyMatT_Rotq_Vec(int const num_rows, Vec3* const Input, double* const Output, od_constraint** C);
extern "C" void multiplyMatT_Traq_Vec(int const num_rows, Vec3* const Input, double* const Output, od_constraint** C);
extern "C" void multiplyMat_vec(int const, Vec3* const Input, Vec3* const Output, od_constraint** C);
extern "C" void multiplyMat_RotVec_q(int const num_rows, Vec3* const Output, od_constraint** C, int const type, int const v1a2);
//extern "C" void multiplyMat_TraVec_q(int const, Vec3* const, od_constraint**, int const, int const);
extern "C" void multiplyParMatT_Rotq_Vec_I(Vec3* const Input, od_constraint** C, int const ith, int const dofs, int);
extern "C" void multiplyParMatT_Traq_Vec_I(Vec3* const Input, od_constraint** C, int const ith, int const dofs, int);
extern "C" void multiplyParMatT_Traq_Vec_II(Vec3* const Input, od_constraint** C, int const ith, int const dofs, int);
extern "C" void multiplyParMatRotVec_q(int const num_rows, od_constraint** C, int ith,
	int const type, int const v1a2);// , int const keep);
extern "C" void multiplyParMatTraVec_q_I(int const num_rows, od_constraint** C, /*od_constraint* const pCj*/int ith,
	int const type, int const v1a2);// , int const keep);
extern "C" void multiplyParMatTraVec_q_II(int const num_rows, od_constraint** C, /*od_constraint* const pCj*/int ith,
	int const type, int const v1a2);// , int const keep);

class od_system;

class analysis_attr {
public:
	int disp_maxit;
	double disp_tolerance;
	int disp_pattern[10];

	analysis_attr(int maxit = 25, double toler = 5.0e-16)
	{
		disp_maxit = maxit;
		disp_tolerance = toler;
		fill(disp_pattern, disp_pattern + 10, 1);
	}
	// void set_iteration(int it) {disp_maxit = it);
};

class analysis_flags {
public:
	int disp_ic;
	int vel_ic;
	int acc_ic;
	int _static;
	analysis_flags() {
		disp_ic = 0;
		vel_ic = 0;
		acc_ic = 0;
		_static = 0;
	}
};

class _system : public od_object {
protected:
	int _from_py;
	double _time;
public:
	_system(char* Name, int from_py = 0) : od_object(Name) { _from_py = from_py; _time = 0.0; }
	double time(double t = -1.0) {
		if (t >= 0.0) { _time = t; }
		return _time;
	}
};

class mat_d {
public:
	double **_mat;
	int r, c;
	mat_d(int r_, int c_) {
		_mat = new double*[3 * r_]; r = r_;
		for (int i = 0; i < 3 * r; i++) {
			_mat[i] = new double[c_];
		}
		c = c_;
		zeros();
	}
	void zeros() { for (int i = 0; i < 3 * r; i++) fill(_mat[i], _mat[i] + c, 0.0); }
	~mat_d() { for (int i = 0; i < 3 * r; i++) DELARY(_mat[i]); DELARY(_mat); }
	void fromd(int i, int j, double *pd) {
		_mat[3 * i][j] += pd[0]; _mat[3 * i + 1][j] += pd[1]; _mat[3 * i + 2][j] += pd[2];
	}
	void toFile(ofstream*  pOutFile, const char* pName) {
		int i, j;
		*pOutFile << pName << "=[" << endl;
		for (i = 0; i < r * 3; i++) {
			for (j = 0; j < c; j++) {
				*pOutFile << scientific << setprecision(6);
				*pOutFile << setw(9) << _mat[i][j] << " ";
			}
			*pOutFile << ";" << endl;
		}
	}
};
class sparseMat {
private:
	int maxr, maxc;
	vector<int> cols;
	vector<int> rows;
	vector<Vec3*> listVec;
	vector<int> *idxContainer;
	int* _cols;
	int* _rows;
	Vec3** _listVec;
	int **_idxContainer;
	int numNZ, numCol;
	int* lenPercol;
	void cleanContainer() {
		cols.resize(0); rows.resize(0); listVec.resize(0); DELARY(idxContainer)
	}
	double **mat;
public:
	sparseMat() {
		cols.resize(0); rows.resize(0); listVec.resize(0); idxContainer = 0;
		_cols = _rows = 0;
		numNZ = 0;
		_listVec = 0;
		_idxContainer = 0;
		lenPercol = 0;
		mat = 0;
		maxr = maxc = -1;
	}

	~sparseMat() {
		cleanContainer();
		DELARY(_cols);
		DELARY(_rows);
		DELARY(_listVec);
		DELARY(lenPercol);
		if (_idxContainer) {
			for (int i = 0; i < numCol; i++) { DELARY(_idxContainer[i]); }
			DELARY(_idxContainer);
		}
	}

	void init(int nbody, int numcol) {
		numCol = numcol;
		maxc = numCol;
		maxr = nbody;
		lenPercol = new int[numCol];
		idxContainer = new vector<int>[numcol];
		for (int i = 0; i < numcol; i++) idxContainer[i].resize(0);
	}

	void addEntry(int row, int col, Vec3* pV) {
		numNZ++;
		int idx = (int)cols.size();
		pV->idx = idx;
		cols.push_back(col);
		rows.push_back(row);
		listVec.push_back(pV);
		idxContainer[col].push_back(idx);
	}
	inline Vec3* ithVec(int i, int _col, int& _row) const {
		int idx = _idxContainer[_col][i];
		_row = _rows[idx];
		return _listVec[idx];
	}
	inline int length(int j) const { return lenPercol[j]; }
	void init_dynamics() {
		int i, j;
		_cols = new int[numNZ];
		_rows = new int[numNZ];
		_listVec = new Vec3*[numNZ];
		for (i = 0; i < numNZ; i++) {
			_cols[i] = cols[i];
			_rows[i] = rows[i];
			_listVec[i] = listVec[i];
		}
		_idxContainer = new int*[numCol];
		for (i = 0; i < numCol; i++) {
			lenPercol[i] = (int)idxContainer[i].size();
			if (lenPercol[i] == 0) {
				_idxContainer[i] = 0;
				continue;
			}
			_idxContainer[i] = new int[lenPercol[i]];
			for (j = 0; j < lenPercol[i]; j++) {
				_idxContainer[i][j] = idxContainer[i][j];
			}
		}
		cleanContainer();
	}
	void toFile(ofstream*  pOutFile, const char* pName) {
		int i, j, k, _size;
		Vec3* pv;
		mat_d _mat(maxr, maxc);
		for (j = 0; j < maxc; j++) {
			_size = length(j);
			for (k = 0; k < _size; k++) {
				pv = ithVec(k, j, i);
				_mat.fromd(i, j, pv->v);
			}
		}
		_mat.toFile(pOutFile, pName);
	}
};



//class bc_pairs {
//public:
//	od_body *pB;
//	vector<od_constraint*> pCs;
//	vector<od_constraint*> fromCs;
//	vector<int> indexb;
//public:
//	bc_pairs(od_body* B = 0) { pB = B; }
//	void add_Cs(vector<od_constraint*>& clist) {
//		int bidx = pB->get_index();
//		for (vector<od_constraint*>::iterator it = clist.begin(); it != clist.end(); ++it) {
//			if ((*it)->is_tagged() == 0) continue;
//			if ((*it)->i_body_index() == bidx) {
//				pCs.push_back(*it);
//				(*it)->untag();
//				indexb.push_back((*it)->j_body_index());
//				//(*it)->swap_markers();
//			}
//			else if ((*it)->j_body_index() == bidx) {
//				pCs.push_back(*it);
//				(*it)->untag();
//				indexb.push_back((*it)->i_body_index());
//			}
//		}
//	}
//	void add_fromCs(od_constraint* val) {
//		this->fromCs.push_back(val);
//	int numFromCs(void) const { return this->fromCs.size(); }
//};

class od_systemGeneric : public _system {
	//friend class od_force;
private:
	map<int, od_body*> idBody;
	map<int, od_constraint*> idJoint;
	map<int, od_force*> idForce;
	map<int, od_marker*> idMarker;
	//map<int, int> bc_map;

public:
	vector<int> dofmap_;
	od_body * pGround;
	od_systemGeneric **_subSys;
	od_body** body_list_;
	od_force** force_list_;
	od_constraint** constraint_list_;
	od_element** element_list_;
	od_jointF** joint_force_list_;
	vector<od_loop*> loop_list;
	sparseMat parForceparPVA[3], parTorqueparPVA[3];
	int initialized;
	int nsystem;
	int nbody;
	int njoint;
	int nforce;
	int njforce;
	int tree_ndofs;
	int lambda_dof;
	int element_num;
	int aux_element_num;
	int start_index, start_bindex;
	double *states; //states
	double *dstates; //state derivatives
	double *ddstates; //acc
	double* M_array;
	Mat33** J_array;

	Vec3 G_array;
	Vec3* _tree_rhs;
	//analysis type
	DVA_TYPE _dva_type;
	JAC_TYPE _jac_type;
	Analysis_Type Ana_Type;

	//temp Variables
	vector<od_element*> element_list;
	vector<od_element*> aux_element_list;
	vector<od_body*> body_list;
	vector<od_marker*> marker_list;
	vector<od_force*> force_list;
	vector<od_constraint*> constraint_list;
	vector<od_jointF*> joint_force_list;
	vector<od_systemGeneric*> subSys;
	vector<Expression*> expr_list;
	vector<od_measure*> measure_list;
	vector<od_constraint*> explicit_constraint_list;


public:
	od_systemGeneric(char* Name, int from_py = 0);
	~od_systemGeneric();
	inline void init();
	inline int num_body() const { return nbody; }
	inline int num_joint() const { return njoint; }
	inline int num_force() const { return nforce; }
	inline int num_jforce() const { return njforce; }
	inline od_constraint* get_constraint_via_index(int index) const { return constraint_list_[index]; }
	inline od_force* get_force_via_index(int idx) const { return force_list_[idx]; }
	inline od_jointF* get_jforce_via_index(int idx) const { return joint_force_list_[idx]; }
	od_marker* get_marker_via_id(int id);
	od_constraint* get_constraint_via_id(int id);
	inline double* pstates() const { return states; }
	inline double* pdstates() const { return dstates; }
	inline double* pddstates() const { return ddstates; }
	inline void setGravity(double *val) { G_array = val; }
	//inline Vec3* tree_rhs(int i) const { return _tree_rhs + i; }
	inline Analysis_Type get_analysis_type() const { return Ana_Type; }
	inline JAC_TYPE get_jac_type() const { return _jac_type; }
	inline DVA_TYPE get_dva_type() const { return _dva_type; }

	virtual void get_states();
	virtual void set_states();
	//int checkEulerBryant();
	inline int tree_dofs() const { return tree_ndofs; }
	inline int lambda_dofs() const { return lambda_dof; }
	virtual void calculate_JR(int no_dot = 1) = 0;
	virtual void calculate_JRdot() = 0;
	virtual void calculate_JT(int no_dot = 1) = 0;
	virtual void calculate_JTdot() = 0;
	void add_body(od_body *pb);
	void add_marker(od_marker *pm);
	void add_constraint(od_constraint *pc);
	void add_force(od_force *pf);
	void add_joint_force(od_joint_force *pf);
	void add_joint_spdp(od_joint_spdp *pf);
	void add_expression(Expression *expr);
	void add_measure(od_measure*);
	void add_subsystem(od_systemGeneric*);
	void add_explicit_constraint(od_constraint* pC);
	od_body* ground(od_body *pb = 0) { if (pb) { pGround = pb; } return pGround; }
	virtual void updatePartials(int pos_only = 0) = 0;
	virtual void parF_parq(double**) = 0;
	virtual void parF_parq_dot(double**) = 0;
	int init_dynamics();
	virtual void init_tree(double* = 0, double* = 0, double* = 0, int = 0) = 0;
	virtual void topology_analysis_level1() = 0;
	virtual void topology_analysis_level2() = 0;
	int get_num_body();
	int get_num_constraint();
	//int getNumMotions() const { return motion_list.size(); }
	virtual int Update(int = 0) = 0;
	virtual double* evaluateRhs(double* rhs) = 0;
	virtual double** evaluateJac(double** pM, int base = 0) = 0;
	virtual void CreateTraM(double **pM, int = 0) = 0;
	virtual void CreateRotM(double **pM, int = 0) = 0;
	virtual int initialize(int = 0, int = 0);
	int blockSize() const { return lambda_dof + tree_ndofs; }
	inline int startIndex() const { return start_index; }
	inline int startBIndex() const { return start_bindex; }
	void unset_evaluated();
	virtual void getM(double**, int = 0);
	Vec3* getOmega(Vec3*);
	Vec3* getOmegaDot(Vec3*);
	Vec3* getVel(Vec3*);
	Vec3* getVelDot(Vec3*);
	virtual inline Vec3* getJR(int, int) const = 0;
	virtual inline Vec3* getJT(int, int) const = 0;
	virtual inline Vec3* getJRdot_dq(int, int) const = 0;
	virtual inline Vec3* getJTdot_dq(int, int) const = 0;
	virtual inline int getL1(int i) const = 0;
	virtual inline int getL2(int i) const = 0;
	virtual inline int getL3(int i) const = 0;
	virtual void numDif() = 0;
};

class od_systemMechanism : public od_systemGeneric {
protected:

	void create_incidence(vector<int>&, int = 0);
	void create_relevence(vector<int>&, int = 0);
	Vec3* _tree_rhs_si2;
	void break_body(int bidx, int cidx1);
	vector<int> mapdof;
	int Tree_Ndofs;

	int* relevenceLevel1;
	int* relevenceLevel2;
	int* relevenceLevel3;

	od_jacobian<Vec3> JTL, JTL_dot; //Jacobian of tree: local translation
	od_jacobian<Vec3> JTG, JTG_dot; //Jacobian of tree: global trannslation
	od_jacobian<Vec3> JR, JR_dot; //Jacobian of tree: rotation
	od_jacobian<Mat33> JMAT; //tramsformation matrix from local to global
	//Partial derivatives: Omega
	od_jacobian<Vec3> parOmega_parq, parOmega_dot_parq, parOmega_dot_parq_dot; // parOmega_parq_dot=JR;
	//Partial derivatives : Vel Local
	od_jacobian<Vec3> parVel_parq, parVel_dot_parq, parVel_dot_parq_dot;  //parVel_parq_dot; //JTL;
	//Vel Global
	od_jacobian<Vec3> parVel_parqG, parVel_dot_parqG, parVel_dot_parq_dotG; //parVel_parq_dotG; =JTG;

public:
	od_systemMechanism(char* pn, int from_py = 0);
	~od_systemMechanism() {
		DELARY(_tree_rhs_si2);
		DELARY(relevenceLevel1);
		DELARY(relevenceLevel2);
		DELARY(relevenceLevel3);
	}
	int get_num_explicit_constraints() const { return (int)explicit_constraint_list.size(); }
	int initialize();
	virtual void calculate_JR(int no_dot = 1);
	virtual void calculate_JRdot();
	virtual void calculate_JT(int no_dot = 1);
	virtual void calculate_JTdot();
	virtual int Update(int = 0);
	virtual void CreateTraM(double **pM, int = 0);
	virtual void CreateRotM(double **pM, int = 0);
	virtual double* evaluateRhs(double* rhs);//, double alpha = 1.0);
	virtual double** evaluateJac(double** pM, int base = 0);
	inline int TreeDofs() const { return Tree_Ndofs; }
	void updateQ();
	virtual void updatePartials(int pos_only = 0);
	int checkEulerBryant();
	virtual void topology_analysis_level1();
	virtual void topology_analysis_level2();
	void topology_analysis(vector<int>&, vector<int>&, vector<int>&/*, int=0*/);
	void topology_analysis(vector_int& perm, vector_int& inci, vector_int& rele);
	int graphW(void);
	int graphD(void);
	void print_inci(vector<int>&, int, int);
	void print_rele(vector<int>&, int, int);
	void print_rele1();
	void DFSrecur(int, std::vector<int> &, std::vector<int> &);
	virtual void init_tree(double* = 0, double* = 0, double* = 0, int = 0);
	virtual void parF_parq(double**);
	virtual void parF_parq_dot(double**);
	virtual inline Vec3* getJR(int i, int j) const { return JR.element(i, j); }
	virtual inline Vec3* getJT(int i, int j) const { return JTG.element(i, j); }
	virtual inline Vec3* getJRdot_dq(int i, int j) const { return parOmega_parq.element(i, j); }
	virtual inline Vec3* getJTdot_dq(int i, int j) const { return parVel_parqG.element(i, j); }
	virtual inline int getL1(int i) const { return relevenceLevel1[i]; }
	virtual inline int getL2(int i) const { return relevenceLevel2[i]; }
	virtual inline int getL3(int i) const { return relevenceLevel3[i]; }
	virtual void numDif();// {};
};

class od_systemTrack2D : public od_systemGeneric {
protected:
	od_marker* refM;
	od_body* base;
	od_free_joint *freeJ;
	od_fixed_joint* fixed;
	od_matrix_dense *M00, *M01;
	od_matrix_denseB *M10;
	od_blockTriDiagonal* pTriM;
	Vec3 **JR, **JR_dot, **JT, **JT_dot;
	//Partial derivatives: Omega
	Vec3 **parOmega_parq, **parOmega_dot_parq, **parOmega_dot_parq_dot; // parOmega_parq_dot=JR;
	Vec3 **parVel_parqG, **parVel_dot_parqG, **parVel_dot_parq_dotG; //parVel_parq_dotG; =JTG;
	void allocate(Vec3** v) {
		v = new Vec3*[num_body()];
		for (int i = 0; i < num_body(); i++) v[i] = new Vec3[9];
	}
	void remove(Vec3** v) {
		for (int i = 0; i < num_body(); i++) DELARY(v[i]);
		delete[] v;  v = 0;
	}
	void ParOmegaParq(int dot = 0);
	void parOmegaDotParq();
	void parOmegaDotParqDot();
	void parVelParq();
	void parVelDotParq();
	void parVelDotParqDot();
public:
	od_systemTrack2D(char* pn, od_marker* ref = 0, int from_py = 0);
	~od_systemTrack2D();
	void setRef(od_marker *ref) { refM = ref; }
	virtual void CreateTraM(double **pM, int = 0);
	virtual void CreateRotM(double **pM, int = 0);
	virtual double* evaluateRhs(double* rhs) { double* x = 0;  return x; }
	virtual double** evaluateJac(double** pM, int base = 0) { double** x = 0; return x; }
	void create_incidence(vector<int>&, int = 0) {}
	void create_relevence(vector<int>&, int = 0) {}
	virtual void calculate_JR(int no_dot = 1);
	virtual void calculate_JRdot() { calculate_JR(0); }
	virtual void calculate_JT(int no_dot = 1);
	virtual void calculate_JTdot() { calculate_JT(0); }
	virtual void updatePartials(int pos_only = 0);// {}
	virtual int Update(int = 0);// { return 0; }
	virtual void topology_analysis_level1() {}
	virtual void topology_analysis_level2() {}
	virtual void init_tree(double* = 0, double* = 0, double* = 0, int = 0);
	virtual inline Vec3* getJR(int i, int j) const { return &(JR[i][j]); }
	virtual inline Vec3* getJT(int i, int j) const { return &(JT[i][j]); }
	virtual inline Vec3* getJRdot_dq(int i, int j) const { return &(JR_dot[i][j]); }
	virtual inline Vec3* getJTdot_dq(int i, int j) const { return &(JT_dot[i][j]); }
	virtual void parF_parq(double**);
	virtual void parF_parq_dot(double**) {}
	virtual inline int getL1(int i) const { return 1; }
	virtual inline int getL2(int i) const { return 1; }
	virtual inline int getL3(int i) const { return 1; }
	virtual void numDif() {};
};

class  od_system : public od_systemMechanism {
public:
	analysis_attr Ic;
	analysis_attr kinematics;
	analysis_flags Analysis_Flags;
	int ndofs;
private:
	ofstream * pMsgFile;
	//ofstream* pOutFile;
	void _init();

	od_equation_kinematic *pKin;
	od_equation_bdf_I *pDyn;
	od_equation_hhti3 *pDynHHT;

	double _cputime;
	double _cputimeJac;
	double _cputimeRhs;
	double _cputimeInv;

public:
	void numDif();
	inline void set_dofs(int num_d) { ndofs = num_d; }
	void set_jac_type(JAC_TYPE _type);// { _jac_type = _type;}
	void set_dva_type(DVA_TYPE _type);// { _dva_type = _type;}
	void set_analysis_type(Analysis_Type type);// {Ana_Type = type;}
	inline int get_dofs() const { return ndofs; }
	double cputime();
	double cpuTime() { return _cputime; }
	double jacTime() { return _cputimeJac; }
	double rhsTime() { return _cputimeRhs; }
	double invTime() { return _cputimeInv; }
	double startRecord() { return cputime(); }
	inline void stopRecord(double s, int Sol2Jac1Rhs0 = 0) {
		double _delta = (cputime() - s)*1.0e-7;
		if (Sol2Jac1Rhs0 == 1) _cputimeJac += _delta;
		else if (Sol2Jac1Rhs0 == 0) _cputimeRhs += _delta;
		else _cputimeInv += _delta;
	}
	void parSi2_parq(double**);

	virtual void getM(double **pM, int base = 0);
	od_system(char* pn, int from_py = 0);
	~od_system();
	ofstream* msgFile() const { return pMsgFile; }
	/*ofstream* outFile(int __debug) {
		if (__debug) {
			if (!pOutFile) {
				string name__ = name() + ".m";
				pOutFile = new ofstream(name__.c_str(), ios::out);
			}
		}
		return pOutFile;
	}*/
	int initialize(DVA_TYPE, JAC_TYPE, Analysis_Type);
	virtual void get_states();
	virtual void set_states();
	void toFile(ofstream *pf, string&);
	double getKPE();
	string info();
	char* info(char* msg) {
		std::string _msg = info();
		strcpy(msg, (char*)_msg.c_str());
		return msg;
	}

	void update(int = 0);
	//inline Vec3* tree_rhs_si2(int i) const { return _tree_rhs_si2 + i; }
	double* evaluate_rhs(double* rhs, double alpha=1.0);
	double** evaluate_Jac(double**);
	int displacement_ic();
	int velocity_ic();
	int acceleration_and_force_ic();
	int kinematic_analysis(double end_time, int num_steps, double tol = 5.0e-16, int iter = 5);
	int static_analysis(int iter = 15, double tol = 5.0e-16);
	int dynamic_analysis_bdf(double end_time, double tol = 1.0e-3, int iter = 6, double maxH = .1, double minH = 1.0e-6, double _initStep = .005, int _debug = 0);
	int dynamic_analysis_hht(double end_time, double tol = 1.0e-3, int iter = 6, double maxH = .1, double minH = 1.0e-6, double _initStep = .005, int _debug = 0);// { return 0; }
};

/*class  od_beams : public od_systemGeneric {
private:
	od_blockTriDiagonal * pM, *pK, *pC;
	int *dim_info;
public:
	od_beams(char* pn, int from_py = 0);
	~od_beams() {
		DELARY(dim_info);
		delete pM;
		delete pK;
		delete pC;
	}
	void topology_analysis_level1();
};*/
#endif
