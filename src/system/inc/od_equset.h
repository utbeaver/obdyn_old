#ifndef OD_EQUSET_H
#define OD_EQUSET_H
#include <vector>
#include <fstream>
#include <iomanip>
#include "od_loop.h"
#include "od_linsolv.h"
#include "od_element.h"



class od_system;
class od_gstiff;
class od_hhti3;



class od_equation : public od_object {
protected:
	od_system *pSys;
	int tree_ndofs;
	int numLoops;
	int dim_rows, dim_cols, dimJac, effDim;
	double *pRhs;
	double *states;
	double *std;
	double *dstates;
	double *ddstates;
	int initialized;
	double **pJac;
	double *error;
	double *_X;
	double *_Xdot;
	double *_Xddot;
	//od_gstiff* pIntegrator;
	int pattern[10];
	od_matrix* SysJac;
	string info_str;
	vector<int> removed_entries;
	ofstream *pMsg;
	ofstream *pOut;
	int* Vars;
	int _debug;
	int dofs_calculated;
	//int* permuV;
	vector<int> dofmap;
	int* _dofmap;
	od_object::DVA_TYPE _dva_type;
	od_object::JAC_TYPE _jac_type;
	od_object::Analysis_Type Ana_Type;
public:
	od_equation(od_system *psys) : od_object() {
		pSys = psys; pRhs = 0; //numMotions=0;
		ddstates = 0; dstates = 0; states = 0; initialized = 0; pJac = 0; //permuV = 0;
		error = 0; SysJac = 0; removed_entries.resize(0);
		_X = _Xdot = 0;  pMsg = 0; _debug = 0; pOut = 0; Vars = 0; dofs_calculated = 0;
	}
	virtual ~od_equation();
	inline double* X() const { return _X; }
	inline double* Xdot() const { return _Xdot; }
	inline double *Xddot() const { return _Xddot; }
	virtual int initialize();
	virtual int solve(double = 0.0) = 0;
	inline int num_rows() const { return dim_rows; }
	inline int num_cols() const { return dim_cols; }
	inline int treeDofs() const { return tree_ndofs; }
	double time(double t = -1.0);
	ofstream* msgFile();
	inline int debug() const { return _debug; }
	double getKPE();
	int checkEulerBryant();
	inline int Var(int i) const { return Vars[i]; }
	inline  int* Var() const { return Vars; }
	inline void print_out() { SysJac->print_out(); }
	void set_lambda(double*, int = 1);
	void set_lambda_si2(double*);
	od_system* model() { return pSys; }
	double startRecord();// {return pSys->startRecord();}
	void stopRecord(double s, int d);// {pSys->stopRecord(s, d);}
	void createPermuV(int=0);// vector<int>& dofmap);
};

class od_equation_kin_and_static : public od_equation {
protected:
	int eval, icInt;
	//vector<int> dofmap;
	void process_void_row_and_col(int i, double val);
public:
	od_equation_kin_and_static(od_system *psys) : od_equation(psys) {}
	int initialize();
	virtual int evaluate(int eval_jac = 0);
};

class od_equation_disp_ic : public od_equation_kin_and_static {
protected:
public:
	od_equation_disp_ic(od_system *psys);
	virtual int solve(double = 0.0);
	virtual int evaluate(int eval_jac = 0);
};

class od_equation_vel_ic : public od_equation_kin_and_static {
protected:
public:
	od_equation_vel_ic(od_system *psys);
	virtual int solve(double = 0.0);
	virtual int evaluate(int eval_jac = 0);
};

class od_equation_acc_ic : public od_equation_kin_and_static {
protected:
public:
	od_equation_acc_ic(od_system *psys);
	virtual int solve(double = 0.0);
	virtual int evaluate(int eval_jac = 0);
};

class od_equation_kinematic : public od_equation_kin_and_static {
protected:
	double _end_time, delta;
	int _steps;
	int solve_for_disp();
	int solve_for_vel();
	int solve_for_acc_force();
	int evaluate_dva(int = 0, int = 0);
public:
	od_equation_kinematic(od_system *psys, double end_time = 1.0, int steps = 1);
	virtual int initialize();
	virtual int solve(double = 0.0);
	void simulate(double);
};

class od_equation_static : public od_equation_kin_and_static {
protected:
	int _steps;
	double _tol;
public:
	od_equation_static(od_system *psys, int steps = 15, double err_tol = 1.0e-15);
	virtual int initialize();
	virtual int solve(double = 0.0);
	virtual int evaluate(int eval_jac = 0);
};

class od_equation_dynamic : public od_equation {
protected:
	double _end, _tol;
	int _steps, _corrNum;
	double _initStep, hMax, hMin, h;
	int reEval;
	double** M_a;
	double** M_v;
	double** M_d;
	int modifiedLU;
public:
	od_equation_dynamic(od_system *psys, double end = 1.0, int steps = 10, double tol = 1.0e-4);
	~od_equation_dynamic();
	virtual int initialize();
	virtual double* evalRhs(double* prhs = 0);
	void evalJac();
	void setSteps(int steps);
	void setMinStepSize(double val);
	void setMaxStepSize(double val);
	void setInitStep(double val);
	void setLocalErrorTol(double val);
	void setMaxCorrectNum(int val);
	int checkRedundancy();
	void numDif();
};

class od_equation_bdf_I : public od_equation_dynamic {
public:
	od_gstiff* pIntegrator;
	od_matrix_bdf *SysJacBDF;
	double *pprhs;
public:
	od_equation_bdf_I(od_system *psys, double end = 1, int steps = 1, double tol = 1.0e-3);
	~od_equation_bdf_I(); 
	int solve(double = 0.0);
	int solveBDF(double = 0.0);
	void evalJac(double tinu);
	void evalJacBdf(double);
	virtual int initialize();
	virtual void simulate(double, int);
	virtual double* evalRhs();
	void set_states();
	void get_states();
};

class od_equation_hhti3 : public od_equation_dynamic {
private:
	double alpha, beta, gamma;
	od_hhti3* pIntegrator;
	double* pQ;
	double* Q;
	double *Mq;
	od_matrix_hht *SysJacHHT;
public:
	double *pprhs;
public:
	od_equation_hhti3(od_system *psys, double end = 1, int steps = 1, double tol = 1.0e-3, double al =  -1.0 / 3.0);
	~od_equation_hhti3(); 
	int solve(double = 0.0);
	int solveHHT(double = 0.0);
	double* evalRhs();
	void calNonLinQ(int m_q=1);
	//void calMq();
	//void calMa(double = -1.0);
	void evalJac();
	void evalJacHHT();
	virtual int initialize();
	void set_states(int = 0, double scale=1.0);
	void get_states();
	virtual void simulate(double, int);
	void updatepQ(); // { copy(Q, Q + tree_ndofs, pQ); }
};
#endif
