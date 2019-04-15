#ifndef OD_CONSTRAINT_H
#define OD_CONSTRAINT_H
#include <vector>
#include <functional>
#include <algorithm>
#include <math.h>
#include "od_element.h"
#include "od_marker.h"
#include "expr.h"
#include "mat33.h"


class od_systemGeneric;
//class od_motion;

class joint_primitive : public od_object {
protected:
	od_marker * From;
	od_marker* To;
	int _dofs;
	double disp, vel, acc;
	int rotation;
	Expression *pExpr;
	int initialized;
	double fltTemp[3];
	int xyz_;
	int reversed;
	double w_cross_zi[3];
	Vec3 _zixv, _zixa;
	Vec3 _dot_zixv, _dot_zixa;
	int index;
	//od_systemGeneric* pSys;
	double F;
public:
	joint_primitive() {
		_dofs = 1; rotation = 1; disp = 0.0;
		vel = 0.0; acc = 0.0; pExpr = 0; initialized = 0; reversed = 0; //no_incre = 0;
		index = -1; F = 0.0;// pMotion = 0;
	}
	joint_primitive(double d, double v, double a)
	{
		_dofs = 1; rotation = 1; disp = d; F = 0.0;
		index = -1; //pMotion = 0;
		vel = v; acc = a; pExpr = 0; initialized = 0; reversed = 0;
	}
	joint_primitive(Expression* a)
	{
		_dofs = 0; rotation = 1; disp = 0.0; F = 0.0;
		index = -1; //pMotion = 0;
		vel = 0.0; acc = 0.0; reversed = 0;
		if (pExpr) delete pExpr;
		pExpr = a; initialized = 0;
	}
	~joint_primitive();
	inline int  const dofs()  const { return _dofs; }
	inline void setIndex(int idx) { index = idx; }
	inline int const getIndex() { return index; }
	inline void set_from_marker(od_marker *ma) { From = ma; }
	inline void set_to_marker(od_marker *ma) { To = ma; }
	inline int is_rotation() const { return rotation == 1; }
	//void set_system(od_systemGeneric* p) { pSys = p; }
	inline double p() const { return disp; }
	inline double v() const { return vel; }
	inline double a() const { return acc; }
	inline int xyz() const { return xyz_; }
	inline od_marker* from() const { return From; }
	inline od_marker* to() const { return To; }
	//inline od_motion* motion() { return pMotion; }
	void reverse() {
		od_marker *temp;
		temp = From; From = To; To = temp;
		disp = -disp;
		vel = -vel;
		acc = -acc;
		reversed = 1;
	}
	void set_expr(const char *exp);
	virtual void initialize() = 0;
	//int ifReversed() { return reversed; }
	virtual int update(od_systemGeneric *psys = 0) = 0;
	virtual void direction(Mat33::TYPE _type) = 0;
	inline double* get_zi() const { return From->get_global_axis(xyz_); }

	inline double* get_cross_zi() { return w_cross_zi; }
	inline double* get_zi_global(double* Zi/*, od_object::DOF_TYPE type = od_object::ROT_DOF*/) {
		return From->get_axis_global(Zi, xyz_);
	}

	double* get_states(double* X, od_object::DVA_TYPE type = od_object::DISP, int for_drive = 0) {
		//if (this->pExpr) return X;
		if (type == od_object::DISP) {
			if (this->pExpr) {
				disp = this->pExpr->eval();
				*X++ = F;
			}
			else {
				*X++ = disp;
			}
		}
		else if (type == od_object::VEL) {
			if (this->pExpr)
				vel = this->pExpr->eval_wrt_time();
			*X++ = vel;
		}
		else if (type == od_object::ACC) {
			if (this->pExpr) {
				acc = this->pExpr->eval_wrt_time_2();
				*X++ = F;
			}
			else {
				*X++ = acc;
			}
		}
		return X;
	}

	double* set_states
	(double* X, od_object::DVA_TYPE _type = od_object::DISP, int for_drive = 0) {
		if (_type == od_object::DISP) {
			if (this->pExpr) {
				disp = this->pExpr->eval();
				F = *X++;
			}
			else {
				disp = *X++;
			}
		}
		else if (_type == od_object::VEL) {
			vel = *X++;
			if (this->pExpr) vel = this->pExpr->eval_wrt_time();

		}
		else {
			if (this->pExpr) {
				F = *X++;
				acc = this->pExpr->eval_wrt_time_2();
			}
			else {
				acc = *X++;
			}
		}
		return X;
	}

	//void set_no_increment() { no_incre = 1; }
	void updateQ() {
		double* _zi = get_zi();
		double _v = vel;
		double _a = acc;
		double* pv = w_cross_zi;
		fltTemp[0] = _zi[0] * _v;
		fltTemp[1] = _zi[1] * _v;
		fltTemp[2] = _zi[2] * _v;
		_zixv = fltTemp;
		fltTemp[0] = _zi[0] * _a;
		fltTemp[1] = _zi[1] * _a;
		fltTemp[2] = _zi[2] * _a;
		_zixa = fltTemp;
		fltTemp[0] = pv[0] * _v;
		fltTemp[1] = pv[1] * _v;
		fltTemp[2] = pv[2] * _v;
		_dot_zixv = fltTemp;
		fltTemp[0] = pv[0] * _a;
		fltTemp[1] = pv[1] * _a;
		fltTemp[2] = pv[2] * _a;
		_dot_zixa = fltTemp;

	}
	inline double* zixv() { return _zixv.v; }
	inline double* zixa() { return _zixa.v; }
	inline double* dot_zixv() { return _dot_zixv.v; }
	inline double* dot_zixa() { return _dot_zixa.v; }
};

class jprim_rot : public joint_primitive {
protected:
	Mat33 rot_mat;
public:
	jprim_rot(Mat33::TYPE _type, double d = 0.0, double v = 0.0, double a = 0.0) :joint_primitive(d, v, a) {
		rot_mat.set_type(_type);
		direction(_type);
	}
	jprim_rot(Mat33::TYPE _type, Expression* pexpr = 0) :joint_primitive(pexpr) {
		rot_mat.set_type(_type);
		direction(_type);
	}
	virtual int update(od_systemGeneric *psys = 0);
	virtual void initialize();
	void direction(Mat33::TYPE _type) {
		rot_mat.set_type(_type);
		if (_type == Mat33::ZAXIS) {
			xyz_ = 2;
		}
		else if (_type == Mat33::XAXIS) {
			xyz_ = 0;
		}
		else {
			xyz_ = 1;
		}
	}
};

class jprim_tra : public joint_primitive {
protected:
public:
	jprim_tra(Mat33::TYPE _type, Expression* pexpr = 0) :joint_primitive(pexpr) {
		direction(_type);
		rotation = 0;
	}
	jprim_tra(Mat33::TYPE _type, double d, double v = 0.0, double a = 0.0) :joint_primitive(d, v, a) {
		direction(_type);
		rotation = 0;
	}
	void direction(Mat33::TYPE _type) {
		if (_type == Mat33::ZAXIS) {
			xyz_ = 2;
		}
		else if (_type == Mat33::XAXIS) {
			xyz_ = 0;
		}
		else {
			xyz_ = 1;
		}
	}
	virtual int update(od_systemGeneric *psys = 0);
	virtual void initialize();
};

class od_constraint : public od_connector {
protected:
	int num_dofs;
	//double reversesign;
	od_object::JOINT_TYPE jtype;
	//Expression *pExpr;
	int initialized;

	double *par_col;
	Vec3** parVec3col[6];
	int tail, head;
public:
	od_constraint(int Id, char* name_, int Real = 1);
	virtual ~od_constraint() {
		DELARY(par_col);
		//if (pExpr) delete pExpr;
	}
	virtual int initialize() = 0;
	//	inline Expression* expr() { return pExpr; }

	inline int dofs() const { return num_dofs; }
	int tag_attached_body();
	int untag_attached_body();
	virtual void re_eval_info();

	virtual int if_rotation(int i) = 0;
	virtual	void set_expr(const char* pexpr, int = 0) = 0;

	virtual double* set_states(double* state, od_object::DVA_TYPE state_type = od_object::DISP, int = 0) = 0;
	virtual double* get_states(double* state, od_object::DVA_TYPE state_type = od_object::DISP, int = 0) = 0;
	//virtual double get_temp_pva(int, od_object::DVA_TYPE state_type = od_object::DISP) = 0;
	virtual void set_temp_pva(double *p, double *v, double *a) = 0;
	inline virtual int get_start_index() = 0;
	virtual void set_tail(int t) = 0;
	inline virtual int get_tail() = 0;
	virtual void set_head(int t) = 0;
	inline virtual int get_head() = 0;
	//virtual int numMotions() = 0;
	//virtual od_motion* motion(int i) = 0;
};

class od_joint :public od_constraint {
protected:
	int euler_or_not;
	vector<od_marker*> betweenij;
	vector<string> prinames;
	jprim_rot** rot_joints;
	jprim_tra** tra_joints;
	//vector<od_motion*> motions;
	double *temp_pos, *temp_vel, *temp_acc;
	int num_rot_joints;
	int num_tra_joints;
	int start_index;
	int axisVecLen;
	double distance_between_cm_markers[3];
	double velocity_between_cm_markers[3];
	double distance_to_i_cm_marker[3];
	double velocity_to_i_cm_marker[3];
	//Vec3 vecTemp;
	double zi[18];
	double cross_zi[18];
	//int funcNum;
	//char* func[6];

	Vec3 _zixaRot, _zixvRot, _dot_zixaRot, _dot_zixvRot;
	Vec3 _zixaTra, _zixvTra, _dot_zixaTra, _dot_zixvTra;


public:
	od_joint(int Id, char* name_, int Real) :od_constraint(Id, name_, Real)
	{
		rot_joints = 0;
		tra_joints = 0;
		start_index = 0; axisVecLen = 0;
		num_rot_joints = 0;
		num_tra_joints = 0;
		fill(zi, zi + 18, 0.0); fill(cross_zi, cross_zi + 18, 0.0);
		euler_or_not = 1;
	}
	~od_joint() {
		int i;
		for (i = 0; i < num_rot_joints; i++) delete rot_joints[i]; DELARY(rot_joints);
		for (i = 0; i < num_tra_joints; i++) delete tra_joints[i]; DELARY(tra_joints);
		for (unsigned ii = 0; ii < betweenij.size(); ii++) delete betweenij[ii];
		betweenij.clear();

	}

	void dofMap(vector<int> &dofm) {
		int i;
		for (i = 0; i < num_tra_joints; i++) dofm.push_back(tra_joints[i]->dofs());
		for (i = 0; i < num_rot_joints; i++) dofm.push_back(rot_joints[i]->dofs());
	}
	inline  double* get_Ri() { return distance_between_cm_markers; }
	inline  double* get_Rii() { return distance_to_i_cm_marker; }
	inline  double* get_Ri_dot() { return velocity_between_cm_markers; }
	inline  double* get_Rii_dot() { return velocity_to_i_cm_marker; }
	void updateQ();

	inline virtual int num_tra() { return num_tra_joints; }
	inline virtual int num_rot() { return num_rot_joints; }
	inline virtual int if_rotation(int i) { return (i < num_tra_joints) ? 0 : 1; }
	inline const char* type(int i) const { return prinames[i].c_str(); }
	void set_expr(const char* pexpr, int i = 0) {
		if (i < num_tra_joints) {
			tra_joints[i]->set_expr(pexpr);
		}
		else if (i < (num_tra_joints + num_rot_joints)) {
			rot_joints[i-num_tra_joints]->set_expr(pexpr);
		}
	}

	inline virtual void set_temp_pva(double *p, double *v, double *a) {
		temp_pos = p;
		temp_vel = v;
		temp_acc = a;
	}

	virtual double* set_states(double* state, od_object::DVA_TYPE state_type = od_object::DISP, int for_drive = 0) {
		int i;
		for (i = 0; i < num_tra_joints; i++) state = tra_joints[i]->set_states(state, state_type, for_drive);
		for (i = 0; i < num_rot_joints; i++) state = rot_joints[i]->set_states(state, state_type, for_drive);
		return state;
	}
	virtual double* get_states(double* state, od_object::DVA_TYPE state_type = od_object::DISP, int for_drive = 0) {
		int i;
		for (i = 0; i < num_tra_joints; i++) state = tra_joints[i]->get_states(state, state_type, for_drive);
		for (i = 0; i < num_rot_joints; i++) state = rot_joints[i]->get_states(state, state_type, for_drive);
		return state;
	}
	inline  double get_P(int idx) const { return temp_pos[idx]; }
	inline double get_V(int idx) const { return temp_vel[idx]; }
	inline double get_A(int idx) const { return temp_acc[idx]; }
	/*virtual double get_temp_pva(int idx, od_object::DVA_TYPE state_type = od_object::DISP) {
		double temp = 0.0;
		if (state_type == od_object::DISP)
			temp = temp_pos[idx];
		else if (state_type == od_object::VEL)
			temp = temp_vel[idx];
		else
			temp = temp_acc[idx];
		return temp;
	}*/
	virtual double* pva(double *value, int p0v1a2 = 0) {
		int i;
		int tempInt;
		double fltTemp1;
		tempInt = 0;
		for (i = 0; i < num_tra_joints; i++) {
			if (p0v1a2 == 0) {
				fltTemp1 = tra_joints[i]->p();
			}
			else if (p0v1a2 == 1) {
				fltTemp1 = tra_joints[i]->v();
			}
			else {
				fltTemp1 = tra_joints[i]->a();
			}
			value[tempInt++] = fltTemp1;
		}
		for (i = 0; i < num_rot_joints; i++) {
			if (p0v1a2 == 0) {
				fltTemp1 = rot_joints[i]->p();
			}
			else if (p0v1a2 == 1) {
				fltTemp1 = rot_joints[i]->v();
			}
			else {
				fltTemp1 = rot_joints[i]->a();
			}
			value[tempInt++] = fltTemp1;
		}
		return value;
	}
	virtual int initialize() = 0;
	//virtual int set_reversed();
	void set_start_index(int in) {
		int i;
		start_index = in;
		for (i = 0; i < num_tra_joints; i++) tra_joints[i]->setIndex(in++);
		for (i = 0; i < num_rot_joints; i++) rot_joints[i]->setIndex(in++);
	}
	inline int get_start_index() { return start_index; }
	int update(od_systemGeneric* psys = 0);
	int updateReldistance(od_systemGeneric* psys);
	inline double* get_zi_global() const { return (double*)zi; }
	inline double* get_cross_zi() const { return (double*)cross_zi; }
	//double* getQ(int type, od_object::DOF_TYPE rot_tra, int v1a2, int i) { double *p = 0;  return p; }
	double* getQ(int type, od_object::DOF_TYPE rot_tra, int v1a2, int, double*);
	double* getQ(int type, od_object::DOF_TYPE rot_tra, int v1a2, int idx = -1);
	double* contribute_to_JR(double* vec, od_constraint::DOF_TYPE);
	double* contribute_to_JT(double* vec, od_constraint* pC = 0);
	double* contribute_to_JRdot(double* vec, od_constraint::DOF_TYPE);
	double* contribute_to_JTdot(double* vec, od_constraint* pC = 0);
	int checkEulerBryant();
	int initRotXYZ();
	inline void  setPartialVec3(int j, Vec3** vec) {
		parVec3col[j] = vec;
	}
	inline Vec3* getPartialVec3(int i, int j) {
		return parVec3col[j][i];
	}
	double* get_ith_par_col(int i, int len_) {
		double* pd;
		if (!par_col) {
			int __size = (num_tra_joints + num_rot_joints)*len_;
			par_col = new double[__size];
			fill(par_col, par_col + __size, 0.0);
		}
		pd = par_col + i * len_;
		return pd;
	}
	virtual void set_tail(int t) { tail = t; }
	inline virtual int get_tail() { return tail; }
	virtual void set_head(int t) { head = t; }
	inline virtual int get_head() { return head; }
};

class od_fixed_joint :public od_joint {
protected:
public:
	od_fixed_joint(int Id, char* name_, int Real = 0) :od_joint(Id, name_, Real) {
		num_dofs = 0;
		jtype = od_object::FIXED;
	}
	virtual int initialize() { 
		prinames.push_back(string("tx"));
		prinames.push_back(string("ty"));
		prinames.push_back(string("tz"));
		prinames.push_back(string("rx"));
		prinames.push_back(string("ry"));
		prinames.push_back(string("rz"));
		return 1; }
};

class od_txyrz_joint : public od_joint {
public:
	od_txyrz_joint(int Id, char* name_, int Real = 1)
		:od_joint(Id, name_, Real) {
		num_dofs = 3;
		num_rot_joints = 1;
		num_tra_joints = 2;
		axisVecLen = 9;
		jtype = od_object::TXYRZ;
		rot_joints = new jprim_rot*[num_rot_joints];
		rot_joints[0] = new jprim_rot(Mat33::ZAXIS, 0.0);
		tra_joints = new jprim_tra*[num_tra_joints];
		tra_joints[0] = new jprim_tra(Mat33::XAXIS, 0.0);
		tra_joints[1] = new jprim_tra(Mat33::YAXIS, 0.0);
	}

	virtual int initialize() {
		if (initialized) return 1;
		int i;
		double xyz[3];// , xyz1[3];
		Mat33 mat3;
		prinames.push_back(string("tx"));
		prinames.push_back(string("ty"));
		//prinames.push_back(string("tz"));
		//prinames.push_back(string("rx"));
		//prinames.push_back(string("ry"));
		prinames.push_back(string("rz"));
		od_marker* tx = new od_marker(-1, (char*)"tx");
		od_marker* ty = new od_marker(-1, (char*)"ty");
		tx->equal(get_jmarker());
		ty->equal(get_jmarker());
		betweenij.push_back(tx);
		betweenij.push_back(ty);
		//betweenij.push_back(tz);
		tra_joints[0]->set_from_marker(get_jmarker());
		tra_joints[0]->set_to_marker(tx);
		tra_joints[1]->set_from_marker(tx);
		tra_joints[1]->set_to_marker(ty);


		//tz->equal(get_jmarker());
		get_imarker()->get_position(xyz, get_jmarker(), get_jmarker());
		tra_joints[0]->set_states(xyz);
		tra_joints[1]->set_states(xyz + 1);
		/*fill(xyz1, xyz1 + 3, 0.0); xyz1[0] = xyz[0];
		tx->set_position(xyz1);
		fill(xyz1, xyz1 + 3, 0.0); xyz1[1] = xyz[1];
		ty->set_position(xyz1);*/
		rot_joints[0]->set_from_marker(ty);
		rot_joints[0]->set_to_marker(get_imarker());
		for (i = 0; i < num_tra_joints; i++)
			tra_joints[i]->initialize();
		for (i = 0; i < num_rot_joints; i++)
			rot_joints[i]->initialize();
		return 1;
	}
};

class od_free_joint :public od_joint {
protected:
	Mat33 rot_mat;
	double pos[3];

	//	int euler_or_not;
public:
	od_free_joint(int Id, char* name_, int Real = 0) :od_joint(Id, name_, Real) {
		num_dofs = 6;
		num_rot_joints = 3;
		num_tra_joints = 3;
		axisVecLen = 18;
		rot_joints = new jprim_rot*[3];
		tra_joints = new jprim_tra*[3];
		jtype = od_object::FREE;
		rot_joints = new jprim_rot*[3];
		rot_joints = new jprim_rot*[3];
		rot_joints = new jprim_rot*[3];
		rot_joints[0] = new jprim_rot(Mat33::XAXIS, 0.0);
		rot_joints[1] = new jprim_rot(Mat33::YAXIS, 0.0);
		rot_joints[2] = new jprim_rot(Mat33::ZAXIS, 0.0);
		tra_joints[0] = new jprim_tra(Mat33::XAXIS, 0.0);
		tra_joints[1] = new jprim_tra(Mat33::YAXIS, 0.0);
		tra_joints[2] = new jprim_tra(Mat33::ZAXIS, 0.0);

	}
	virtual int initialize() {
		if (initialized) return 1;
		int i;
		double xyz[3], xyz1[3];
		Mat33 mat3;
		prinames.push_back(string("tx"));
		prinames.push_back(string("ty"));
		prinames.push_back(string("tz"));
		prinames.push_back(string("rx"));
		prinames.push_back(string("ry"));
		prinames.push_back(string("rz"));

		od_marker* tx = new od_marker(-1, (char*)"tx");
		od_marker* ty = new od_marker(-1, (char*)"ty");
		od_marker* tz = new od_marker(-1, (char*)"tz");
		betweenij.push_back(tx);
		betweenij.push_back(ty);
		betweenij.push_back(tz);
		tra_joints[0]->set_from_marker(get_jmarker());
		tra_joints[0]->set_to_marker(tx);
		tra_joints[1]->set_from_marker(tx);
		tra_joints[1]->set_to_marker(ty);
		tra_joints[2]->set_from_marker(ty);
		tra_joints[2]->set_to_marker(tz);
		tx->equal(get_jmarker());
		ty->equal(get_jmarker());
		tz->equal(get_jmarker());
		get_imarker()->get_position(xyz, get_jmarker(), get_jmarker());
		fill(xyz1, xyz1 + 3, 0.0); xyz1[0] = xyz[0];
		tx->set_position(xyz1);
		fill(xyz1, xyz1 + 3, 0.0); xyz1[1] = xyz[1];
		ty->set_position(xyz1);
		fill(xyz1, xyz1 + 3, 0.0); xyz1[2] = xyz[2];
		tz->set_position(xyz1);
		od_marker* pm1 = new od_marker(-1, (char*)"r1");
		od_marker* pm2 = new od_marker(-1, (char*)"r2");
		betweenij.push_back(pm1);
		betweenij.push_back(pm2);
		rot_joints[0]->set_from_marker(tz);
		rot_joints[0]->set_to_marker(pm1);
		rot_joints[1]->set_from_marker(pm1);
		rot_joints[1]->set_to_marker(pm2);
		rot_joints[2]->set_from_marker(pm2);
		rot_joints[2]->set_to_marker(get_imarker());
		for (i = 0; i < num_tra_joints; i++)
			tra_joints[i]->initialize();
		for (i = 0; i < num_rot_joints; i++)
			rot_joints[i]->initialize();
		initRotXYZ();
		return 1;
	}
};

class   od_revolute_joint :public od_joint {
protected:
public:
	od_revolute_joint(int Id, char* name_, int Real = 1) :od_joint(Id, name_, Real) {
		num_dofs = 1;
		num_rot_joints = 1;
		num_tra_joints = 0;
		axisVecLen = 3;
		jprim_rot* pJp = new jprim_rot(Mat33::ZAXIS, 0.0);
		//pJp->set_from_marker(get_jmarker());
		//pJp->set_to_marker(get_imarker());
		rot_joints = new jprim_rot*[1];
		rot_joints[0] = (pJp);
		jtype = od_object::REVOLUTE;

	}

	od_revolute_joint(int Id, char* name_, double p = 0.0, double v = 0.0, double a = 0.0, int Real = 1)
		:od_joint(Id, name_, Real) {
		num_dofs = 1;
		num_rot_joints = 1;
		num_tra_joints = 0;
		jprim_rot* pJp = new jprim_rot(Mat33::ZAXIS, p, v, a);
		//pJp->set_from_marker(this->get_jmarker());
		//pJp->set_to_marker(this->get_imarker());
		rot_joints = new jprim_rot*[1];
		rot_joints[0] = (pJp);
		jtype = od_object::REVOLUTE;
	}

	od_revolute_joint(int Id, char *pexpr, char* name_, int Real = 1)
		:od_joint(Id, name_, Real) {
		num_dofs = 0;
		num_rot_joints = 1;
		num_tra_joints = 0;
		jprim_rot* pJp = new jprim_rot(Mat33::ZAXIS, 0.0);
		//pJp->set_from_marker(get_jmarker());
		//pJp->set_to_marker(get_imarker());
		rot_joints = new jprim_rot*[1];
		rot_joints[0] = (pJp);
		jtype = od_object::REVOLUTE;
		this->set_expr(pexpr);
	}
	~od_revolute_joint() {
	}
	virtual int initialize() {
		if (initialized == 1) return 1;
		//prinames.push_back(string("tx"));
		//prinames.push_back(string("ty"));
		//push_back(string("tz"));
		//prinames.push_back(string("rx"));
		//prinames.push_back(string("ry"));
		prinames.push_back(string("rz"));
		rot_joints[0]->set_system(pSys);
		//}
		rot_joints[0]->set_from_marker(this->get_jmarker());
		rot_joints[0]->set_to_marker(this->get_imarker());
		rot_joints[0]->initialize();
		initialized = 1;

		return 1;
	}
};

class od_translational_joint :public od_joint {
protected:
public:
	od_translational_joint(int Id, char* name_, double p = 0.0, double v = 0.0, double a = 0.0, int Real = 1)
		:od_joint(Id, name_, Real) {
		num_dofs = 1;
		num_rot_joints = 0;
		num_tra_joints = 1;
		axisVecLen = 3;
		jprim_tra* pJp = new jprim_tra(Mat33::ZAXIS, p, v, a);
		//pJp->set_from_marker(get_jmarker());
		//pJp->set_to_marker(get_imarker());
		tra_joints = new jprim_tra*[1];
		tra_joints[0] = (pJp);
		jtype = od_object::TRANSLATIONAL;
	}
	od_translational_joint(int Id, char *pexpr, char* name_, int Real = 1)
		:od_joint(Id, name_, Real) {
		num_dofs = 0;
		num_rot_joints = 0;
		num_tra_joints = 1;
		jprim_tra* pJp = new jprim_tra(Mat33::ZAXIS, 0.0);
		//pJp->set_from_marker(get_jmarker());
		//pJp->set_to_marker(get_imarker());
		tra_joints = new jprim_tra*[1];
		tra_joints[0] = (pJp);
		jtype = od_object::TRANSLATIONAL;
		this->set_expr(pexpr);
	}
	~od_translational_joint() {
	}
	virtual int initialize() {
		if (initialized) return 1;
		//if (funcNum > 0) {
		//	tra_joints[0]->set_expr(func[0]);
		tra_joints[0]->set_system(pSys);
		//prinames.push_back(string("tx"));
		//prinames.push_back(string("ty"));
		prinames.push_back(string("tz"));
		//prinames.push_back(string("rx"));
		//prinames.push_back(string("ry"));
		//prinames.push_back(string("rz"));
		tra_joints[0]->set_from_marker(get_jmarker());
		tra_joints[0]->set_to_marker(get_imarker());
		tra_joints[0]->initialize();
		initialized = 1;
		return 1;
	}
};


class od_spherical_joint :public od_joint {
protected:
	//int euler_or_not;
public:
	od_spherical_joint(int Id, char* name_, int Real = 1)
		:od_joint(Id, name_, Real) {
		num_dofs = 3;
		num_rot_joints = 3;
		num_tra_joints = 0;
		axisVecLen = 9;
		jtype = od_object::SPHERICAL;
		rot_joints = new jprim_rot*[num_rot_joints];
		rot_joints[0] = new jprim_rot(Mat33::XAXIS, 0.0);
		rot_joints[1] = new jprim_rot(Mat33::YAXIS, 0.0);
		rot_joints[2] = new jprim_rot(Mat33::ZAXIS, 0.0);
	}
	~od_spherical_joint() {
	}
	virtual int initialize() {
		if (initialized) return 1;
		//prinames.push_back(string("tx"));
		//prinames.push_back(string("ty"));
		//prinames.push_back(string("tz"));
		prinames.push_back(string("rx"));
		prinames.push_back(string("ry"));
		prinames.push_back(string("rz"));

		od_marker* pm1 = new od_marker(-1, (char*)"r1");
		od_marker* pm2 = new od_marker(-1, (char*)"r2");
		betweenij.push_back(pm1);
		betweenij.push_back(pm2);
		rot_joints[0]->set_from_marker(get_jmarker());
		rot_joints[0]->set_to_marker(pm1);
		rot_joints[1]->set_from_marker(pm1);
		rot_joints[1]->set_to_marker(pm2);
		rot_joints[2]->set_from_marker(pm2);
		rot_joints[2]->set_to_marker(get_imarker());
		initRotXYZ();
		return 1;
	}
};
class od_universal_joint :public od_joint {
protected:
public:
	od_universal_joint(int Id, char* name_, int Real = 1)
		:od_joint(Id, name_, Real) {
		num_dofs = 2;
		num_rot_joints = 2;
		num_tra_joints = 0;
		axisVecLen = 6;
		jtype = od_object::UNIVERSAL;
		jprim_rot* p1 = new jprim_rot(Mat33::XAXIS, 0.0);
		jprim_rot* p2 = new jprim_rot(Mat33::YAXIS, 0.0);
		rot_joints = new jprim_rot*[2];
		rot_joints[0] = (p1);
		rot_joints[1] = (p2);
	}
	~od_universal_joint() {
	}
	virtual int initialize() {
		if (initialized) return 1;
		int i;
		//prinames.push_back(string("tx"));
		//prinames.push_back(string("ty"));
		//prinames.push_back(string("tz"));
		prinames.push_back(string("rx"));
		prinames.push_back(string("ry"));
		//prinames.push_back(string("rz"));
		double phi1, phi2;
		double rel_mat[9], vec[2];
		double *p = rel_mat;
		od_marker* pm1 = new od_marker(-1, (char*)"r1");
		betweenij.push_back(pm1);
		rot_joints[0]->set_from_marker(get_jmarker());
		rot_joints[0]->set_to_marker(pm1);
		rot_joints[1]->set_from_marker(pm1);
		rot_joints[1]->set_to_marker(get_imarker());
		p = get_imarker()->get_orientation_matrix(p, get_jmarker());
		phi1 = atan2(-p[5], p[8]);
		if (fabs(cos(phi1)) > fabs(sin(phi1))) {
			phi2 = atan2(p[3], p[8] / cos(phi1));
		}
		else {
			phi2 = atan2(p[3], -p[5] / sin(phi1));
		}
		vec[0] = phi1;
		vec[1] = phi2;
		p = vec;
		for (i = 0; i < 3; i++) {
			p = rot_joints[i]->set_states(p, od_object::DISP);
		}
		return 1;
	}
};

/*class od_motion : public od_object {
private:
	joint_primitive * pJ;
	Expression *pExpr;
	int reversed;
	double p, v, a;
	double lambda, lambda_si2;
public:
	od_motion(joint_primitive *pj, Expression *pexpr) {
		pJ = pj; pExpr = pexpr; //reversed = pJ->ifReversed();
		lambda = 0.0; lambda_si2 = 0.0;
	}
	double* set_lambda(double *lamb, int inc = 1) {
		if (inc) { lambda += *lamb; }
		else { lambda = *lamb; }
		return lamb + 1;
	}
	double get_lambda() { return lambda; }
	double get_lambda_si2() { return lambda_si2; }
	double* set_lambda_si2(double *lamb, int inc = 0) {
		if (inc) {
			lambda_si2 += *lamb;
		}
		else {
			lambda_si2 = *lamb;
		}
		return lamb + 1;
	}
	int getIndex() {
		return pJ->getIndex();
	}
	double rhs(od_object::DVA_TYPE dva = od_object::DISP, int value = 0) {
		double state = 0.0;
		double _val, val;
		//reversed = pJ->ifReversed();
		switch (dva) {
		case od_object::DISP:
			state = pJ->p();
			p = _val = pExpr->eval()*(reversed ? -1.0 : 1.0);
			if (pJ->is_rotation())
				_val = fmod(_val, pi + pi);
			p = _val;
			val = state - _val;
			break;
		case od_object::VEL:
			state = pJ->v();
			v = _val = pExpr->eval_wrt_time()*(reversed ? -1.0 : 1.0);
			if (!value) val = state - _val;
			else val = -_val;
			break;
		case od_object::ACC:
			state = pJ->a();
			a = _val = pExpr->eval_wrt_time_2()*(reversed ? -1.0 : 1.0);
			if (!value) val = state - _val;
			else val = -_val;
			break;
		default:
			state = 0.0;
			_val = 0.0;
			break;
		}

		return val;
	}
};*/

#endif
