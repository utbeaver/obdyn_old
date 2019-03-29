#ifndef OD_SYSTEM_API_H
#define OD_SYSTEM_API_H
#ifdef _WINDOWS
  #define LINKSYSTEMDLL __declspec(dllexport)
#else
 #define LINKSYSTEMDLL
#endif
class od_system;
class OdBody;
class OdJoint;
class OdForce;
class OdJointForce;
class OdJointSPDP;
class OdExpression;
class OdSubSystem;
class vector_int;

class LINKSYSTEMDLL OdSystem {
 private:
  od_system *pS;
 public:
  OdSystem(char *pn, int r); 
  ~OdSystem();
  double cpuTime();
  double jacTime();
  double rhsTime();
  double solTime();
  void setGravity(double*);
  void setName(char*);
  void add_body(OdBody* pB); 
  void add_constraint(OdJoint* pJ); 
  void add_force(OdForce *pF);
  void add_joint_force(OdJointForce *pJF);
  void add_joint_spdp(OdJointSPDP *pJF);
  
  char* info(char* msg); 
  int get_num_constraint(); 
  int get_num_body(); 
 
  int displacement_ic();
  int velocity_ic();
  int acceleration_and_force_ic();
  int kinematic_analysis(double=1.0, int=1, double tol=5.0e-16, int iters=26);
  int static_analysis(int iter = 15, double tol=5.0e-5);
  int dynamic_analysis_bdf(double end_time, double tol=1.0e-3, int iter=6, double maxH=.1, double minH=1.0e-6, double _initStep=.005, int = 0);
  int dynamic_analysis_hht(double end_time, double tol = 1.0e-3, int iter = 6, double maxH = .1, double minH = 1.0e-6, double _initStep = .005, int = 0);
  void numdif();
};
#endif
