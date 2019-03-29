#ifndef OD_SUBSYSTEM_API_H
#define OD_SUBSYSTEM_API_H

#ifdef _WINDOWS
  #define LINKELEMENTSDLL  __declspec(dllexport)
#else
  #define LINKELEMENTSDLL
#endif
#include <string.h>
//#include "od_subsystem.h"
class OdBody;
class OdJoint;
class OdForce;
class OdJointForce;
class OdMarker;

class od_systemGeneric;
class od_systemTrack2D;

class LINKELEMENTSDLL odSystemTrack2D {
private:
  od_systemTrack2D *pS;
  char _name[80];
  int Id;
public:
  odSystemTrack2D(char*);
  ~odSystemTrack2D();// {}
  void set_ref(OdMarker *pm);
  void add_body(OdBody* pB);
  void add_constraint(OdJoint* pJ);// {}
  void add_force(OdForce *pF);// {}
  od_systemTrack2D* core(){return pS;}
};

#endif
