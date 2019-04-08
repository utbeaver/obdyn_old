
#ifndef OD_CONSTRAINT_API_H
#define OD_CONSTRAINT_API_H

#ifdef _WINDOWS
#define LINKELEMENTSDLL  __declspec(dllexport)
#else
#define LINKELEMENTSDLL
#endif
#include "od_constraint.h"
class OdMarker;
class V6;
class LINKELEMENTSDLL OdJoint {
protected:
	od_joint * pJ;
	int _id;
	od_object::JOINT_TYPE _type;
	OdMarker *pi, *pj;
	char *_expr;
	char _name[100];
	string tmpstr;
	int _impl;
	std::map<int, std::string> funs;
public:
	OdJoint(int id, char* name_ = 0);
	~OdJoint();
	void revolute();
	void translational();
	void spherical();
	void universal();
	void fixed();
	void free();
	void txyrz();
	od_joint* core();
	void set_imarker(OdMarker *pM);
	void set_jmarker(OdMarker *pM);
	void set_expr(char* pexpr, int=0);
	char* info(char* msg);
	double* disp(double*);
	double* vel(double*);
	double* acc(double*);
	V6 disp();
	V6 vel();
	V6 acc();
	int dofs();
	int rotation(int i);
	const char* type(int i);
	void explicitFixed(int);
	int explicitFixed();
};

#endif
