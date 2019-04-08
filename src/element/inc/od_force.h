// od_force.h: interface for the od_force class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OD_FORCE_H__A7B6E9B8_0872_4ED4_A5C8_BF0BCC73EED7__INCLUDED_)
#define AFX_OD_FORCE_H__A7B6E9B8_0872_4ED4_A5C8_BF0BCC73EED7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "od_element.h"
#include "mat33.h"
using namespace std;

class od_systemGeneric;
class Expression;
class od_measure;
class od_body;
class od_constraint;
class od_joint;

class od_jointF : public od_element {
protected:
	od_joint* pJoint;
	//od_systemGeneric* pSys;
	double val;
	int initialized;
	int ith, index__;
	int reversed;
public:
	od_jointF(int Id, char* name_, od_joint* pJ, int _ith );
	virtual void init() = 0;
	void init1();// { index = pJoint->get_start_index() + ith; }
	inline od_joint* Joint() const { return pJoint; }
	inline void Joint(od_joint* pJ) { pJoint = pJ; }
	//void set_system(od_systemGeneric *psys) { pSys = psys; }
	inline double value() const { return val; }
	inline int row() const {return index__;}
	virtual void re_eval_info() {}
	virtual int get_partial_size(int i = 0) const = 0;
	virtual int get_partial_col(int i, int j) const = 0;;
	virtual double get_partial(int i, int j = 0) const = 0;
	virtual inline int evaluate_rhs() = 0;// { evaluate(0); return 1; }
	virtual inline int evaluate_Jac() = 0;// { evaluate(1); return 1; }
};

class od_joint_spdp : public od_jointF {
protected:
	double K, C, L, F0;
	//double val;
public:
	od_joint_spdp(int Id, char* name_, od_joint* pJ, double k, double c, double l, double f0, int _ith = 0);
	virtual void init() { od_jointF::init1(); }
	virtual int get_partial_size(int i = 0) const { return  (i < 2) ? 1 : 0; }
	virtual int get_partial_col(int i, int j) const { 
		return index__;
	}
	virtual double get_partial(int i, int j = 0) const { return (i == 0) ? -K : -C; }
	inline int evaluate_rhs();// { return 1; }
	inline int evaluate_Jac();// { return 1; }
};

class od_joint_force : public od_jointF {
protected:
	Expression *pExpr;
	od_joint* pJoint;
	//od_systemGeneric* pSys;
	//double val;
	//int initialized;
	//int ith;
	//int reversed;
	void evaluate(int partial);
public:
	od_joint_force(int Id, char* name_, od_joint* pJ, char* _expr, int _ith = 0);
	virtual void init();
	Expression* const expr() const { return pExpr; }
	virtual int get_partial_size(int i = 0) const;
	virtual int get_partial_col(int i, int j) const;
	virtual double get_partial(int i, int j = 0) const;
	virtual inline int evaluate_rhs() { evaluate(0); return 1; }
	virtual inline int evaluate_Jac() { evaluate(1); return 1; }
};

class od_colVec {
private:
	//int row_;
	int col_;
	Vec3 v_;
public:
	od_colVec() { col_ = 0; this->init(); }
	inline void init() { v_.init(); }
	inline int col() const { return col_; }
	inline Vec3* vec() { return &v_; }
	inline double* v() { return v_.v; }
	inline void setCol(int c) { col_ = c; }
	inline void setVals(double* v) { EQ3(v_.v, v) }
	inline void neg() { NEG(v_.v) }
};

class od_force : public od_connector
{
protected:
	Expression *pExpr;
	vector<int> columns_parRi;
	int columns_parRi_size;
	vector<Vec3*> parRi;
	od_colVec* parRiparq;

	vector<int> columns_parRj;
	int columns_parRj_size;
	vector<Vec3*> parRj;
	od_colVec* parRjparq;

	vector<int> columns_iforce[3];
	int columns_iforce_size[3];
	vector<Vec3*> partial_iforce[3];
	od_colVec* partialIforce[3];

	vector<int> columns_itorque[3];
	int columns_itorque_size[3];
	vector<Vec3*> partial_itorque[3];
	od_colVec* partialItorque[3];

	vector<int> columns_jforce[3];
	int columns_jforce_size[3];
	vector<Vec3*> partial_jforce[3];
	od_colVec* partialJforce[3];

	vector<int> columns_jtorque[3];
	int columns_jtorque_size[3];
	vector<Vec3*> partial_jtorque[3];
	od_colVec* partialJtorque[3];

	//Vec3 cos_q;
	double *Ri, *Rj;
	int initialized;
	//od_systemGeneric* pSys;
	od_measure* DT;
	void init_force();
	void eval_force(int partial = 0);
	void eval_partial_RiRj(int counter);
	Force_Type force_type;
	double iForce[3];
	double jForce[3];
	double iTorque[3];
	double jTorque[3];
public:
	od_force(int Id, char* name_, od_marker*, od_marker*);
	virtual ~od_force();
	//void set_system(od_systemGeneric *psys);
	inline int get_iforce_partial_size(int i = 0) const { return columns_iforce_size[i]; }
	inline int get_jforce_partial_size(int i = 0) const { return columns_jforce_size[i]; }
	inline unsigned get_iforce_partial_col(int i, int j = 0) { return partialIforce[j][i].col(); }
	inline unsigned get_jforce_partial_col(int i, int j = 0) { return partialJforce[j][i].col(); }
	inline Vec3* get_iforce_partial(int i, int j = 0) { return partialIforce[j][i].vec(); }
	inline Vec3* get_jforce_partial(int i, int j = 0) { return partialJforce[j][i].vec(); }

	inline int get_itorque_partial_size(int i = 0) const { return columns_itorque_size[i]; }
	inline int get_jtorque_partial_size(int i = 0) const { return columns_jtorque_size[i]; }
	inline unsigned get_itorque_partial_col(int i, int j = 0) { return partialItorque[j][i].col(); }
	inline unsigned get_jtorque_partial_col(int i, int j = 0) { return partialJtorque[j][i].col(); }
	inline Vec3* get_itorque_partial(int i, int j = 0) { return partialItorque[j][i].vec(); }
	inline Vec3* get_jtorque_partial(int i, int j = 0) { return partialJtorque[j][i].vec(); }

	virtual void evaluate(int) = 0;
	virtual int init() = 0;
	inline int evaluate_rhs() {
		evaluate(0);
		return 1;
	}
	inline int  evaluate_Jac() {
		unevaluate();
		evaluate(1);
		return 1;
	}
	virtual double getPE() = 0;
	inline double* iforce() { return iForce; }
	inline double* itorque() { return iTorque; }
	inline double* jforce() { return jForce; }
	inline double* jtorque() { return jTorque; }
	void re_eval_info();
	inline Expression* const expr() const { return pExpr; }

};

class od_single_force : public od_force {
protected:
	Vec3 cos_q;
	//vector<int> columns_fdirection;
	int columns_fdirection_size;
	vector<Vec3*> partial_fdirection;
	od_colVec* partialDirection;
	//Vec3 cos_q;
	//od_measure* DT;
public:
	od_single_force(int Id, char* name_, od_marker*, od_marker*);
	~od_single_force();
	virtual int init() = 0;
	void init_single_force();
	void eval_single_force(int partial = 0);
	virtual void evaluate(int partial) = 0;


};

class od_storque : public od_force {
protected:
	//Expression *pExpr;
	int iCount, jCount;
public:
	od_storque(int Id, char* name, od_marker*, od_marker*, char *_expr = 0);
	virtual void evaluate(int partial);
	virtual int init();
	virtual double getPE() { return 0.0; }
};

class od_sforce : public od_single_force {
	//Expression *pExpr;
public:
	od_sforce(int Id, char* name, od_marker*, od_marker*, char *_expr = 0);
	virtual int init();
	virtual double getPE() { return 0.0; }
	virtual void evaluate(int partial);
};

class od_spdpt : public od_single_force {
protected:
	double K, C, distance, force;
	od_measure* VT;
	//double F0MinusKD0;
	Vec3 cos_q_old;

public:
	od_spdpt(int Id, char* name, od_marker*, od_marker*, double k,
		double c = 0.0, double d = 0.0, double f = 0.0);
	~od_spdpt();
	virtual void evaluate(int partial);
	virtual int init();
	virtual double getPE();
};

class od_beam : public od_force {
protected:
	double length;
	double Ixx, Iyy, Izz, Area, Asy, Asz, emodulus, gmodulus, ratio;
	double k00, k11, k22, k33, k44, k55, k15, k24, kj44, kj55;
	double c00, c11, c22, c33, c44, c55, c15, c24, cj44, cj55;
	od_measure *VT, *DR, *VR, *DDT;
	vector<int> iIds, jIds;
	od_colVec* parFparq, *parTparq, *parFparq_dot, *parTparq_dot;
	od_colVec* parFjparq, *parTjparq, *parFjparq_dot, *parTjparq_dot;
	int parFTparq_size, parFTparq_dot_size;
	int damp;

public:
	od_beam(int Id, char* name, od_marker* pI, od_marker* pJ, double l, double ixx, double iyy, double izz,
		double area, double e, double g, double r, double asy = 0.0, double asz = 0.0);
	od_beam(int Id, char* name, od_marker* pI, od_marker* pJ, double l, double ixx, double iyy, double izz,
		double area, double e, double g, double** _C, double asy = 0.0, double asz = 0.0);
	~od_beam();
	virtual void evaluate(int partial);
	virtual int init();
	virtual double getPE() { return 0.0; }
};

class od_bushing : public od_force {
protected:
	double ktx, kty, ktz, krx, kry, krz;
	double ctx, cty, ctz, crx, cry, crz;

	double f[3], t[3];
	od_measure *VT, *DR, *VR, *DDT;
	vector<int> iIds, jIds;
	od_colVec* parFparq, *parTparq, *parFparq_dot, *parTparq_dot;
	od_colVec* parFjparq, *parTjparq, *parFjparq_dot, *parTjparq_dot;
	int parFTparq_size, parFTparq_dot_size;
public:
	od_bushing(int Id, char* name, od_marker *pI, od_marker *pJ,
		double _ktx, double _kty, double _ktz,
		double _krx, double _kry, double _krz,
		double _ctx, double _cty, double _ctz,
		double _crx, double _cry, double _crz,
		double* _f, double* _t);
	~od_bushing();
	virtual void evaluate(int partial);
	virtual int init();
	virtual double getPE() { return 0.0; }
};
#endif // !defined(AFX_OD_FORCE_H__A7B6E9B8_0872_4ED4_A5C8_BF0BCC73EED7__INCLUDED_)
