// od_measure.h: interface for the od_measure class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OD_MEASURE_H__757CDA9E_9681_4621_A9F8_55D71C1CABD2__INCLUDED_)
#define AFX_OD_MEASURE_H__757CDA9E_9681_4621_A9F8_55D71C1CABD2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "od_marker.h"
#include "od_constraint.h"
class od_systemGeneric;


class od_joint_measure : od_element {
private:
	od_joint* pJ;
	double pva[3], pva_partial[3];
	int ith;
	od_systemGeneric *pSys;
	int p0v1a2;
public:
	od_joint_measure(int i, char* name, od_systemGeneric*, int _p0v1a2, int _ith = 0);
	void init();
	inline double val(int i = 0) { return pva[i]; }
	void eval();
	inline int get_partial_size(int i = 0) { return 1; }
	inline int get_partial_col() { return pJ->get_start_index() + ith; }
	inline double get_partial(int i = 0) { return pva_partial[i]; }
	void re_eval_info() {};
	inline double value(int i = 0) { return pva[i]; }
};

class od_measure : public od_element
{
public:
	enum Measure_Type { DT, VT, AT, DR, VR, AR };
private:
	unsigned ii;
	int I, J, K;
	Vec3 **partialp;
	Vec3 **partialv;
	int *variablep, *variablev;
	int *variableijkp[3], *variableijkv[3];
	int b_indices[3], lijkp[3], lijkv[3], lenP, lenV;
	od_marker* pM[3];
	od_systemGeneric *pSys;
	Measure_Type _type;
	//int* columns_[3];
	//int size_[3];
	//Vec3  **partial_[3];
	double vals[4];
	//int  pva_ijk[3][3];
public:
	od_measure(od_systemGeneric* psys, Measure_Type type, int i, int = 0, int = 0);
	od_measure(od_systemGeneric* psys, Measure_Type type, od_marker* i, od_marker* j = 0, od_marker* k = 0);
	virtual ~od_measure();
	void init();
	void eval(int eva_partial = 0);
	inline  double* get_val(int i = 0) { return vals + i; }
	inline int get_partial_size(int _p0v1a2 = 0) const { return (_p0v1a2==0)? lenP:lenV; }// size_[_p0v1a2];

	inline int get_partial_col(int _p0v1a2, int j) const {//{ return columns_[_p0v1a2][j]; }
		return (_p0v1a2 == 0) ?  variablep[j]: variablev[j];
	}
	Vec3* get_partial(int pva, int j) const {// return partial_[pva][j]; }
		return (pva == 0) ? partialp[j] : partialv[j];
	}
	void set_system(od_systemGeneric *psys) { pSys = psys; }
	inline void re_eval_info() {};
	double get_DM_partial(int p0v1a2, int i);
};

#endif // !defined(AFX_OD_MEASURE_H__757CDA9E_9681_4621_A9F8_55D71C1CABD2__INCLUDED_)
