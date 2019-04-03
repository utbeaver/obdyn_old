#ifndef OD_BODY_H
#define OD_BODY_H

//disable warnings on 255 char debug symbols
#pragma warning (disable : 4786)
//disable warnings on extern before template instantiation
#pragma warning (disable : 4231)
#include <vector>
#include "od_element.h"
#include "od_marker.h"


using namespace std;
class od_system;
class od_systemGeneric;

class od_body : public od_element {
protected:
	double _mass;
	Mat33 _I;
	Mat33 _Ia;
	od_marker* cm;

	vector<od_marker*> marker_list;
	int is_ground;
	od_marker* from;
	double nonlinear_term[3];
	Vec3 *pos_rhs, *ang_rhs;
	double mx_dot[3];
	double Jw[3], Jw_dot[3];
public:
	//std::vector<int> fromCs;
	od_body(int Id = 0, char* name_ = 0, int Real = 1);
	virtual ~od_body() {};
	virtual inline int get_index(void) const { return index; }
	inline void set_pos_rhs(Vec3* vec) { pos_rhs = vec; }
	inline void set_ang_rhs(Vec3* vec) { ang_rhs = vec; }
	inline void set_mass(double m) { _mass = m; /*if (is_real() == 0) _mass = 0.0;*/ }
	inline void set_I(double *val) { _I = val; /*if (is_real() == 0) _I.init(); */}
	void add_cm_marker(od_marker *mar);
	void add_marker(od_marker *mar, int _global=0);
	inline od_marker* cm_marker(void) { return cm; }
	void remove_marker(od_marker* pM);
	inline od_marker* get_ith_marker(int i) const { return marker_list[i]; }
	int get_marker_number() const { return (int)marker_list.size(); }
	inline int ground_body() const { return is_ground; }
	virtual void re_eval_info();
	inline int set_from_marker(od_marker *pM) { from = pM; return 1; }
	int update(od_systemGeneric* psys, int=1);
	void for_ground_body_only();
	void initialize();
	inline double get_M() { return _mass; }
	inline Mat33* get_J() { return &_Ia; }
	inline double* J_Wdot() { return Jw_dot; }
	inline double* J_W() { return Jw; }
	inline void J_v(double* pd, double* v) { mat_doubles(_Ia, v, pd); }
	void parJparq_v(double *pd, double *e, double *e1, int omega_dot = 0); //for partial
	void v_x_parJparq_v(double *pd, double *e, double*);
	void parWxJW_parq(double*, double*);
	inline double* wxJw() { return nonlinear_term; }
	inline double* Mx_ddot() { return mx_dot; }
	double getKE();
	double* getPosition() const;
	double* getVel() const;
	
};
#endif
