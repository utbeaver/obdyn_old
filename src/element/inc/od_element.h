#ifndef OD_ELEMENT_H
#define OD_ELEMENT_H

#define DELARY(Y) {if(Y) delete[] Y; Y=0;}
#define DEL(Y) {if(Y) delete Y; Y=0;}

#ifdef _WINDOWS
//disable warnings on 255 char debug symbols
#pragma warning (disable : 4786)
//disable warnings on extern before template instantiation
#pragma warning (disable : 4231)
#include <string>
#endif
#include <string.h>
#include <sstream>
#include <vector>
using namespace std;
class od_marker;
class od_systemGeneric;

#define DOF 1
#define RDT 0
#define MOTION -1
#define LAMBDA 2
static int virtual_joint_id=0;
static int virtual_body_id=0;
static int virtual_marker_id=0;
static double pi = 3.1415926535897931; 
//unsigned int fps = 0;

#ifdef _WINDOWS
static  unsigned int fp_control_state = _controlfp(_EM_INEXACT, _MCW_EM);
//static  unsigned int fp_control_state = _controlfp(_DN_FLUSH, _MCW_DN);
//static  unsigned int fp_control_state = _controlfp(_EM_INVALID, _MCW_EM);
#endif

class equ_exception {
private:
	int iterations;
	double variation;
	int var_row;
	double rhs;
	int rhs_row;
	double __time;
	int type; //do not converge;1 matrix singular 2
public:
	equ_exception(int iter, double resi, int _row, double _rhs, int _rhsrow, double _time = 0.0) {
		iterations = iter;
		variation = resi;
		var_row = _row;
		rhs = _rhs;
		rhs_row = _rhsrow;
		__time = _time;
		type = 1;
	}
	equ_exception(int row_, double _time = 0.0) {
		var_row = row_;
		__time = _time;
		type = 2;
	}
	inline int get_num_iteration() const { return iterations; }
	inline double getVariation() const { return variation; }
	inline double getRhs() const { return rhs; }
	inline int getVarRow() const { return var_row; }
	inline int getRhsRow() const { return rhs_row; }
	inline double time() const { return __time; }
	string& msg(string& _msg) const {
		ostringstream _str;
		if (type == 1) {
			_str << "At time " << time() << "with Iterations " << get_num_iteration() << endl;
			_str << "Maximum RHS " << getRhs() << " with " << getRhsRow() << "th variable" << endl;
			_str << "Maximum Variation " << getVariation() << " with " << getVarRow() << "th variable" << endl;
			_msg += _str.str();
		}
		else {
			_str << "At time " << time() << " Jacobian  is singular with zero pivot in line " << var_row << endl;
			_msg += _str.str();
		}
		return _msg;
	}
};
class od_object {
 public:
  enum System_Type {SINGLE_CHIAN, BEAMS, CLOSE_CHAIN};       
  enum Element_Type {Type_Body, Type_Maker, Type_Constraint, 
			 Type_Force};
  enum JOINT_TYPE {FREE, REVOLUTE, TRANSLATIONAL, CYLINDRICAL, SPHERICAL, 
		   UNIVERSAL, HOOKE, SCREW, FIXED, TXYRZ};
  enum Force_Type {BEAM, BUSHING, SFORCE, STORQUE, SFORCE_AO, STORQUE_AO, VFORCE, VTORQUE, GFORCE, SPDPT, SPDPR, JOINT_FORCE};
  enum DVA_TYPE {DISP, VEL, ACC, DISP_VEL_ACC};
  enum JAC_TYPE {JAC_INIT_DISP, JAC_DISP, JAC_VEL, JAC_ACC, JAC_STATIC, JAC_DYNAMIC};
  enum DOF_TYPE {ROT_DOF, TRA_DOF};
  enum Analysis_Type {STATIC, DYNAMIC, KINEMATIC, INIT_DISP, INIT_VEL, INIT_ACC, ACC_FORCE};
  string _name;
  int _evaluated;
  int id;
  od_systemGeneric* pSys;
  od_object(string name_) {
	  _evaluated = 0;
	  _name = name_;
	  pSys = 0;
  }
  od_object(char* name_=0) {
	  id = 0;
	  pSys = 0;
	  _evaluated = 0;
	  _name = "element";
	  if (name_)
		_name = name_;
  }
  inline int is_evaluated() const {return _evaluated;}
  inline int evaluated() {_evaluated = 1; return 1;}
  inline int unevaluate() {_evaluated = 0; return 1;}
  inline string name() const {return _name;}
  inline void setName(char *pname) {_name = pname;}
  inline int get_id() const { return id;}
  inline void set_id(int id_) {id = id_;}
  inline bool is_id(int Id) const { return id==Id;}
  inline void set_system(od_systemGeneric *psys) { pSys = psys; }
};


class  od_element : public od_object {
 protected:
  od_object::Element_Type element_type;
  int _prev_idx;
  int _next_idx[10];
  int _num_nex_idx;
  int real;
  int tag_;
  int index;
  string info_str;
 
 public:
  od_element(int Id, char* name_, int Real=1) : od_object(name_) {
	id = Id;
	_prev_idx = -1;
   // _next_idx = 0;
	for (int i = 0; i < 10; i++) _next_idx[i] = -1;
	_num_nex_idx=0;
	real = Real;
	tag_ = 0;
	info_str="";
	unevaluate();
	index = -1;
  }
  virtual  ~od_element() {
   // DELARY(_next_idx)
  }; 
  inline int get_prev_idx() const {return _prev_idx;}
  inline int get_num_next() const {return _num_nex_idx;}
  inline int get_next(int i) const {return _next_idx[i];}
  inline void set_next_idx(int i)  {
   // int ii;
	_next_idx[_num_nex_idx] = i;
	_num_nex_idx++;
	
  }
  inline void set_prev_idx(int val) {_prev_idx = val;}
  inline bool is_real() const {return real==1;}
  inline int is_tagged() const {return tag_;}
  inline int tag() { tag_ = 1; return 1;}
  inline int untag() { tag_ = 0; return 1;}

  inline void set_index(int in) {index = in;}
  virtual inline int get_index(void) const {return index;}
  inline int info_(ostringstream* ost)  { *ost << info() << endl; return 1;}
  inline string  info() { re_eval_info(); return info_str;}
  inline char* info(char* msg) {
	  std::string _msg = info();
	  strncpy(msg, _msg.c_str(), 80);
	  return msg;
  }
  virtual void re_eval_info() = 0;
}; 

class od_connector : public od_element {
protected:
  od_marker *i_marker, *j_marker;
  double reversesign;
  //od_systemGeneric* pSys;
  int iNotGround, jNotGround;
public:
  od_connector(int Id, char* name_, od_marker* i=0, od_marker *j=0, int Real=1) : od_element(Id, name_, Real) {
	  i_marker = i; j_marker = j; iNotGround = jNotGround = 1; reversesign = 1.0;
  }
  inline void set_imarker(od_marker *i) {i_marker = i;}
  inline void set_jmarker(od_marker *j) {j_marker = j;}
  inline od_marker *get_imarker() const {return i_marker;}
  inline od_marker *get_jmarker() const {return j_marker;}
  int i_body_index();
  int j_body_index();
  void swap_markers() {
	  od_marker* temp_mar = i_marker;
	  i_marker = j_marker;
	  j_marker = temp_mar;
	  reversesign = -1.0;
  }
  int check();
};

#endif
