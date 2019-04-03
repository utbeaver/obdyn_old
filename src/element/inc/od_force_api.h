
#ifndef OD_FORCE_API_H
#define OD_FORCE_API_H

#ifdef _WINDOWS
#define LINKELEMENTSDLL  __declspec(dllexport)
#else
#define LINKELEMENTSDLL
#endif
#include "od_force.h"
class OdMarker;
class OdJoint;
class LINKELEMENTSDLL OdForce {
protected:
	od_force *pF;
	int _id;
	od_object::Force_Type _type;
	OdMarker *pi, *pj;
	char *_expr;
	char* _name;
	double K, C, Dist, For;
	double Ixx, Iyy, Izz, Area, Asy, Asz, Ratio, Emodulus, Gmodulus;
	double bushingK[6], bushingC[6], bushingF[6];
public:
	OdForce(int id, char* name_ = 0);
	~OdForce();
	void sforce();
	void storque();
	void spdpt();
	void beam();
	void bushing();
	od_force* core();
	void set_imarker(OdMarker *pM);
	void set_jmarker(OdMarker *pM);
	void set_expr(char* pexpr);
	void set_stiffness(double val);// {K=val;}
	void set_damping(double val);// {C=val;}
	void set_distance(double val);// {Dist=val;}
	void set_force(double val);// { For = val;}
	void set_ixx(double val) { Ixx = val; }
	void set_iyy(double val) { Iyy = val; }
	void set_izz(double val) { Izz = val; }
	void set_area(double val) { Area = val; }
	void set_asy(double val) { Asy = val; }
	void set_asz(double val) { Asz = val; }
	void set_ratio(double val) { Ratio = val; }
	void set_emodulus(double val) { Emodulus = val; }
	void set_gmodulus(double val) { Gmodulus = val; }
	void set_Kt(double* _k) {
		for (int i = 0; i < 3; i++) bushingK[i] = _k[i];
	}
	void set_Ct(double* _c) {
		for (int i = 0; i < 3; i++) bushingC[i] = _c[i];
	}
	void set_F(double* _f) {
		for (int i = 0; i < 3; i++) bushingF[i] = _f[i];
	}
	void set_Kr(double* _k) {
		for (int i = 0; i < 3; i++) bushingK[i + 3] = _k[i];
	}
	void set_Cr(double* _c) {
		for (int i = 0; i < 3; i++) bushingC[i + 3] = _c[i];
	}
	void set_T(double* _f) {
		for (int i = 0; i < 3; i++) bushingF[i + 3] = _f[i];
	}
	char* info(char* msg);

};

class LINKELEMENTSDLL OdJointForce {
protected:
	int _id;
	od_joint_force *pJF;
	OdJoint* pJ;
	char *_expr;
	char* _name;
public:
	OdJointForce(int id, char* name_ = 0);
	~OdJointForce();
	od_joint_force* core();
	void setJoint(OdJoint *pJ);
	void set_expr(char* pexpr);
	char* info(char* msg) {
		return msg;
	}

};

class LINKELEMENTSDLL OdJointSPDP {
protected:
	int _id, ith;
	od_joint_spdp *pJF;
	OdJoint* pJ;
	double K, C, L, F;
	char* _name;
public:
	OdJointSPDP(int id, char* name_ = 0);
	~OdJointSPDP() {}
	od_joint_spdp* core();
	void setJoint(OdJoint *pj, int ith_ = 0) { pJ = pj; ith = ith_; }
	
	void set_stiffness(double val) { K = val; }
	void set_damping(double val) { C = val; }
	void set_distance(double val) { L = val; }
	void set_force(double val) { F = val; }
	char* info(char* msg) {
		return msg;
	}

};
#endif
