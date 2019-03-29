#if !defined(OD_SUBSYSTEM_H)
#define OD_SUBSYSTEM_H


#include <sstream>
#include <vector>
#include "od_element.h"
#include "od_linsolv.h"
using namespace std;
class od_system;

class od_subsystem : public od_element {
protected:
  od_object::System_Type _type;
  od_subsystem **nextSys;
  int numNext;
  int numJnt, numB, numF;
  int* jnts;
  int* parts;
  int* forces;
  od_system *pS;
  int _dofs;
public:
  od_subsystem(int Id, /*int type,*/ char* name, int _numJnt=0, int _numBody=0, int numF=0,
      int* _jnt=0, int* _bs=0, int* _fs=0, int _numNext=0, int* next=0) ;
  ~od_subsystem() { 
    DELARY(jnts)
    DELARY(nextSys) 
    DELARY(parts) 
    DELARY(forces)
  }
  inline int dofs() const {return _dofs;}
  inline void setSystem(od_system* ps) {pS=ps;}
  void initialize();
  virtual void re_eval_info(){};
};


#endif
