#ifndef OD_MARKER_H
#define OD_MARKER_H

//disable warnings on 255 char debug symbols
#pragma warning (disable : 4786)
//disable warnings on extern before template instantiation
#pragma warning (disable : 4231)
#include <string.h>
#include "od_element.h"
#include "mat33.h"
class od_body;
class od_systemGeneric;



class od_marker :public od_element {
public:
	od_body * pbody;
	double r_pos[3]; //relative position wrt cm marker
	double a_r_pos[3]; //absolute position wrt cm marker
	double r_euler[3];
	double a_pos[3];
	Mat33  a_mat;
	Mat33  r_mat;
	Mat33 temp_mat;
	int cm_marker;
	int ref_marker;
	double omega_global[3];
	double vel[3];
	double omega_dot_global[3];
	double acc[3];
	double pmat[9];
	od_marker(int ID, char* name_, int Real = 1);
	od_marker(int ID, od_body *pBody = 0, char* name_ = 0, int Real = 1);
	od_marker(int ID, double pos[3], double ang[3], char* name_, int Real = 1);
	od_marker(int ID, double matrix[16], char* name_, int Real = 1);
	od_marker(int ID, od_marker* ref, double pos[3], double ang[3], char* name_, int Real = 1);
	void set_position(double *pos) { EQ3(r_pos, pos); EQ3(a_pos, pos); }
	void set_angles(double *ang) { EQ3(r_euler, ang); r_mat.update(r_euler); }
	double* get_angles(double p[3]) {
		return a_mat.toBryant(p);
	}
	void init();
	void set_cm_marker();
	int is_cm_marker() { return cm_marker; }
	virtual ~od_marker();
	od_body* get_body();
	void set_body(od_body *pB);
	void equal(od_marker* pM);

	void update_cm_marker(od_marker* from, od_systemGeneric* psys = 0);
	void update_marker(od_marker* cm, od_systemGeneric* psys = 0);
	virtual void re_eval_info();
	void rotation(od_marker *wrt, Mat33& mat);

	void translation(od_marker *wrt, Vec3& vec);
	void translation(od_marker *wrt, double* vec);
	Vec3& get_axis_global(Vec3& zi, int xyz = 2);
	double* get_axis_global(double* zi, int xyz = 2);
	inline double* get_global_axis(int i) const { return a_mat.v[i]; }
	inline Mat33* const  mat() { return &a_mat; }
	void set_omega(double *ome) {
		omega_global[0] = ome[0];
		omega_global[1] = ome[1];
		omega_global[2] = ome[2];
		//if (ic) for (int i = 0; i < 3; i++) ifric[i] = ic[i];
	}
	inline double* get_omega() const { return (double*)omega_global; }
	double* get_omega(double *ome, od_marker* pJ = 0, od_marker* pK = 0) {
		EQ3(ome, omega_global);
		if (pJ) {
			double *pd = pJ->get_omega();
			U_SUB3(ome, pd);
		}
		if (pK) ome = pK->vec_wrt_this(ome);
		return ome;
	}
	Vec3& get_omega(Vec3& vec, od_marker* pJ = 0, od_marker* pK = 0) {
		get_omega(vec.v, pJ, pK);// vec.test_zero();
		return vec;
	}

	void set_omega_dot(double *ome) {
		omega_dot_global[0] = ome[0];
		omega_dot_global[1] = ome[1];
		omega_dot_global[2] = ome[2];
	}
	inline double* get_omega_dot() const { return (double*)omega_dot_global; }
	double* get_omega_dot(double *ome, od_marker* pJ = 0, od_marker* pK = 0) {
		EQ3(ome, omega_dot_global);
		if (pJ) {
			double *pd = pJ->get_omega_dot();
			U_SUB3(ome, pd);
		}
		if (pK) ome = pK->vec_wrt_this(ome);
		return ome;
	}


	double* get_rotation(double* vec, od_marker* pJ = 0);

	inline double* get_velocity(double *vec, od_marker *pJ = 0, od_marker *pK = 0) {
		EQ3(vec, vel);
		if (pJ) { U_SUB3(vec, pJ->vel); }
		if (pK) pK->vec_wrt_this(vec);
		return vec;
	}
	inline double* get_velocity() const { return (double*)vel; }
	void set_velocity(double *vec) {
		EQ3(vel, vec);
		//	if (ic) { EQ3(iftic, ic) }
	}
	double* get_acceleration(double *vec, od_marker *pJ = 0, od_marker *pK = 0) {
		EQ3(vec, acc);
		if (pJ) { U_SUB3(vec, pJ->acc); }
		if (pK) pK->vec_wrt_this(vec);
		return vec;
	}
	inline double* get_acceleration() { return acc; }
	void set_acceleration(double *Acc) {
		EQ3(acc, Acc);
	}


	inline  double* const absolute_rel_pos() { return a_r_pos; }
	double* get_position(double* vec, od_marker *pJ = 0, od_marker *pK = 0) {
		EQ3(vec, a_pos);
		if (pJ) { U_SUB3(vec, pJ->a_pos); }
		if (pK) pK->vec_wrt_this(vec);
		return vec;
	}

	inline double*  get_position() const { return (double*)a_pos; }

	double* get_orientation_matrix();/* {
		double* p = pmat;
		p = temp_mat.to_double(p);
		return p;
	}*/
	double* get_orientation_matrix(double* mat, od_marker* pJ = 0); 
	void for_ground_marker_only() {
		for (int i = 0; i < 3; i++) {
			a_r_pos[i] = 0.0;
			a_pos[i] = r_pos[i];
			r_pos[i] = 0.0;
			omega_global[i] = 0.0;
			omega_dot_global[i] = 0.0;
			vel[i] = 0.0;
		}
		a_mat = r_mat;
		r_mat.init();
	}
	inline double* vec_wrt_this(double* temp_d) { return a_mat ^ temp_d; }
	inline Vec3& vec_wrt_this(Vec3& temp_d) { return a_mat ^ temp_d; }
	inline double* vec_wrt_ground(double* temp_d) { return a_mat * temp_d; }
	inline Vec3& vec_wrt_ground(Vec3& temp_d) { return a_mat * temp_d; }
	void make_local_to(od_marker* ref);
};


#endif
