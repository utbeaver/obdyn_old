#include <vector>
#include <string>
#include <stack>
#include <map>

#ifndef EXPR_H
#define EXPR_H

#ifdef _WINDOWS
#define LINKEXPRDLL   
#else
#define LINKEXPRDLL
#endif
class od_systemGeneric;
class od_measure;
class od_joint_measure;
#include "od_element.h"
using namespace std;

class LINKEXPRDLL Expression : public od_object {
public:
  enum Expr_Type {GEN, SIN, ASIN, COS, ACOS, TAN,
    ATAN, CONSTANT, PAREN, EXP, LOG, PI, TIME, SQRT,
    DX, DY, DZ, DM, RX, RY, RZ, DVX, DVY, DVZ, DVM, RVX, RVY, RVZ, JOINTD, JOINTV, JOINTA};
private:
  string _expr;
  vector<Expression*> sub_expr;
  Expression** sub_expr_;
  vector<Expression*> config_expr;
  Expression** config_expr_;
  vector<int> op_stack;
  stack<int> op_erator; 
  Expr_Type e_type;
  map<char, int> op_priority;
  //od_systemGeneric *pSys;
  int initialized;
  int evaled;
  int devaled;
  int ddevaled;
  double value, str_value;
  double dvalue, str_dvalue;
  double ddvalue, str_ddvalue;
  int I, J, K; //for DX, DY.....
  od_measure *Measure;
  od_joint_measure *jointMeasure;
  vector<int> columns[3];
  int *columns_[3];
  vector<double> partial[3];
  double *partial_[3];
  int eval_partial();
  Expression* parent;
  double pi; 
  int x0y1z2;
public:
  Expression(const char* expr, od_systemGeneric* psys=0, Expr_Type type = GEN);
  Expression(string& expr, od_systemGeneric* psys=0, Expr_Type type = GEN);
  
  //inline void set_system(od_systemGeneric *psys) { pSys = psys;}
  inline void add_config_expr(Expression *cexpr) {config_expr.push_back(cexpr);}
  ~Expression();
  double eval(int =0);
  double eval_wrt_time();
  double eval_wrt_time_2();
  inline double val() const { 
	  return value;
  }
  inline double const dval() const { 
	  return dvalue; 
  }
  //inline double const ddval() const { return ddvalue; }
  int init();
  inline void set_parent(Expression *pp) {parent=pp;}
  inline int is_evaluated() {return evaled;}
  inline int evaluated() {evaled = 0;}
  int unevaluate();
  string::size_type fmp(string::size_type);
  inline size_t get_partial_size(int i=0) const {return columns[i].size();}
  inline int get_partial_col(int i, int j) const {return columns_[i][j];}
  inline double get_partial(int i, int j=0) const {return partial_[i][j];}
  
};
#endif
