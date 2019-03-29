%Module odsystem 

%HeaderCode
#include <string>
#include <iostream>
#include "od_marker_api.h"
#include "od_constraint_api.h"
#include "od_body_api.h"
#include "od_force_api.h"
#include "od_system_api.h"
#include "od_subsystem_api.h"
#include "expr.h"
%End
/*
class OdSubSystem
{
   public:
     OdSubSystem(int, int, char*);
%MemberCode
     int id;
     int type;
     PyObject *pystr;
     if (sipParseArgs(&sipArgsParsed,sipArgs, "iiO", &id, &type, &pystr)) {
	    char* pstr = PyString_AsString(pystr);
	    Py_BEGIN_ALLOW_THREADS
	    sipCpp = new OdSubSystem(id, type, pstr);
      	    Py_END_ALLOW_THREADS
     }  
%End

  void add_body(OdBody*); 
%MemberCode
     OdBody *pb;
     OdSubSystem *pS;
     PyObject *pO;
     int flag=0;
     if (sipParseArgs(&sipArgsParsed,sipArgs, "mO", sipThisObj, sipClass_OdSubSystem, &pS, &pO)) {
        int iserp = 0;
        pb = sipForceConvertTo_OdBody(pO, &iserp);
        if(pS && !iserp) {
	       pS->add_body(pb);
	       flag = 1;
        }
     } 
     return Py_BuildValue("i", flag);
%End
  void add_constraint(OdJoint*);
%MemberCode
     OdJoint *pc;
     OdSubSystem *pS;
     PyObject *pO;
     int flag=0;
     if (sipParseArgs(&sipArgsParsed,sipArgs, "mO", sipThisObj, sipClass_OdSubSystem, &pS, &pO)) {
    	int iserp = 0;
        pc = sipForceConvertTo_OdJoint(pO, &iserp);
       	if (pS && !iserp) {
	       pS->add_constraint(pc);
	       flag = 1;
       	}
     } 
     return Py_BuildValue("i", flag);
%End

  void add_force(OdForce*);
%MemberCode
     OdForce *pc;
     OdSubSystem *pS;
     PyObject *pO;
     int flag=0;
     if (sipParseArgs(&sipArgsParsed,sipArgs, "mO", sipThisObj, sipClass_OdSubSystem, &pS, &pO)) {
    	int iserp = 0;
        pc = sipForceConvertTo_OdForce(pO, &iserp);
       	if (pS && !iserp) {
	       pS->add_force(pc);
	       flag = 1;
       	}
     } 
     return Py_BuildValue("i", flag);
%End
};
*/

class OdBody 
{
   public:
     OdBody(int , int);
%MemberCode
     int id;
     PyObject *pystr;
     if (sipParseArgs(&sipArgsParsed,sipArgs, "iO", &id, &pystr)) {
	    char* pstr = PyString_AsString(pystr);
	    Py_BEGIN_ALLOW_THREADS
	    sipCpp = new OdBody(id, pstr);
      Py_END_ALLOW_THREADS
     }  
%End
    void setMass(double);
%MemberCode
       double mass;
       OdBody *pB;
       if (sipParseArgs(&sipArgsParsed,sipArgs,  "md", sipThisObj, sipClass_OdBody, &pB, &mass)){  
         if(pB) {
          pB->setMass(mass);
         }
       }
       return Py_BuildValue("s", "value set!");
%End
    void setI(double*);
%MemberCode
      int i;
      double I[9];
      OdBody *pB;
      PyObject *temp_py;
      PyObject *_I;
      if (sipParseArgs(&sipArgsParsed,sipArgs,  "mO", sipThisObj, sipClass_OdBody, &pB, &_I)) {  
         if(pB) {
           for(i=0; i<9; i++) {
	            temp_py = PyList_GetItem(_I, i);
              I[i] = PyFloat_AsDouble(temp_py);
           }
         }
         pB->setI(I);
      }
      return Py_BuildValue("s", "value set!");
%End

    char* info();
%MemberCode
//     std::string msg;
     char msg[1000];
     OdBody *pB;
     char* pmsg = msg;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdBody, &pB))  
     {
	if (pB) { 	
	  pmsg= pB->info(pmsg);
	} 
     }
     return  Py_BuildValue("s", msg);
%End
    void add_cm_marker(OdMarker*);
%MemberCode
     OdMarker *pM;
     PyObject* pO;
     OdBody *pB;
     int flag = 0;
     std::string msg="";
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mO", sipThisObj, sipClass_OdBody, &pB, &pO))  
     {
    	int iserrp = 0;
        pM = sipForceConvertTo_OdMarker(pO, &iserrp);
	      if(iserrp) msg += "conversion to OdMarker fails";
	      if (!iserrp && pB) { 	
	        pB->add_cm_marker(pM);
	        flag = 1;
	        msg += "Cm Marker is added!";
	      } 
     }
     return  Py_BuildValue("s", msg.c_str());
%End

    void add_marker(OdMarker* );
%MemberCode
     int flag = 0;
     OdMarker *pM;
     PyObject* pO;
     OdBody *pB;
     std::string msg="";
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mO", sipThisObj, sipClass_OdBody, &pB, &pO))  
     {
    	int iserrp = 0;
        pM = sipForceConvertTo_OdMarker(pO, &iserrp);
	if(iserrp) msg += "conversion to OdMarker fails";
	if(!iserrp && pB) {
	  pB->add_marker(pM, 0);
	  flag = 1;
	  msg += "Global Marker is added!";
 	} 
     }
     return  Py_BuildValue("s", msg.c_str());
%End	

    void add_markerg(OdMarker* );
%MemberCode
     int flag = 0;
     OdMarker *pM;
     PyObject* pO;
     OdBody *pB;
     std::string msg="";
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mO", sipThisObj, sipClass_OdBody, &pB, &pO))  
     {
    	int iserrp = 0;
        pM = sipForceConvertTo_OdMarker(pO, &iserrp);
	if(iserrp) msg += "conversion to OdMarker fails";
	if(!iserrp && pB) {
	  pB->add_marker(pM, 1);
	  flag = 1;
	  msg += "Marker is added!";
 	} 
     }
     return  Py_BuildValue("s", msg.c_str());
%End	
};



class OdMarker
{
   public:
    OdMarker(int, int); 
%MemberCode
     int id, real;
     PyObject *pystr;
     if (sipParseArgs(&sipArgsParsed,sipArgs, "iO", &id, &pystr)) {
	char* pstr = PyString_AsString(pystr);
	Py_BEGIN_ALLOW_THREADS
	sipCpp = new OdMarker(id, pstr);
        Py_END_ALLOW_THREADS
     }
%End
    OdMarker(int, OdBody*, int); 
%MemberCode
     int id, real;
     PyObject *pystr;
     OdBody *pB;
     if (sipParseArgs(&sipArgsParsed,sipArgs, "imOO", &id, sipClass_OdBody, &pB, &pystr )) {
       char* pstr = PyString_AsString(pystr);
       Py_BEGIN_ALLOW_THREADS
       sipCpp = new OdMarker(id, pB, pstr);
       Py_END_ALLOW_THREADS
     }
%End
    OdMarker(int, double*, double*, int); 
%MemberCode
     int id, real, i;
     double pos[3], ang[3];
     PyObject *Pos;
     PyObject *Ang;
     PyObject *temp_py;
     PyObject *pystr;
     if (sipParseArgs(&sipArgsParsed,sipArgs, "iOOO", &id, &Pos, &Ang, &pystr ))     {
       char* pstr = PyString_AsString(pystr);
       for(i=0; i<3; i++) {
	        temp_py = PyList_GetItem(Pos, i);
	        pos[i] = PyFloat_AsDouble(temp_py);
       }
       for(i=0; i<3; i++) {
	        temp_py = PyList_GetItem(Ang, i);
	        ang[i] = PyFloat_AsDouble(temp_py);
       }
	    Py_BEGIN_ALLOW_THREADS
	    sipCpp = new OdMarker(id, pos, ang, pstr);
      Py_END_ALLOW_THREADS
     }
%End
    OdMarker(int, double*, int); 
%MemberCode
     int id, real, i;
     double Mat[16];
     PyObject *Pos;
     PyObject *temp_py;
     PyObject *pystr;
     if (sipParseArgs(&sipArgsParsed,sipArgs, "iOO", &id, &Pos,  &pystr ))     {
       char* pstr = PyString_AsString(pystr);
       for(i=0; i<16; i++) {
	        temp_py = PyList_GetItem(Pos, i);
	        Mat[i] = PyFloat_AsDouble(temp_py);
       }
	    Py_BEGIN_ALLOW_THREADS
	    sipCpp = new OdMarker(id, Mat, pstr);
      Py_END_ALLOW_THREADS
     }
%End
     void set_velocity(double*);
%MemberCode
     int i;
     OdMarker *pM;
     PyObject *vel;
     double Vel[3];
     int Switch[3];
     PyObject *_switch;
     PyObject *temp_py;
     std::string msg="";
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mOO", sipThisObj, sipClass_OdMarker, &pM, &vel, &_switch))  {
	      for(i=0; i<3; i++) {
      	  temp_py = PyList_GetItem(vel, i);
	        Vel[i] = PyFloat_AsDouble(temp_py);
      	}
	      for(i=0; i<3; i++) {
      	  temp_py = PyList_GetItem(_switch, i);
	        Switch[i] = PyInt_AsLong(temp_py);
      	}
      	if(pM) {
	        pM->set_velocity(Vel, Switch);
	        msg += "Success!";
        } else {
	        msg += "Fails in setting velocity!";
        }
     }
     return  Py_BuildValue("s", msg.c_str());
%End
     void set_omega(double*);
%MemberCode
     int i;
     OdMarker *pM;
     PyObject *omega;
     double Omega[3];
     int Switch[3];
     PyObject *_switch;
     PyObject *temp_py;
     std::string msg="";
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mOO", sipThisObj, sipClass_OdMarker, &pM, &omega, &_switch))  {
	      for(i=0; i<3; i++) {
      	  temp_py = PyList_GetItem(omega, i);
	        Omega[i] = PyFloat_AsDouble(temp_py);
      	}
	      for(i=0; i<3; i++) {
      	  temp_py = PyList_GetItem(_switch, i);
	        Switch[i] = PyInt_AsLong(temp_py);
      	}
      	if(pM) {
	        pM->set_omega(Omega, Switch);
	        msg += "Success!";
        } else {
	        msg += "Fails in setting omega!";
        }
     }
     return  Py_BuildValue("s", msg.c_str());
%End
     void set_position(double*);
%MemberCode
     int i;
     PyObject *Pos;
     double pos[3];
     PyObject *temp_py;
     std::string msg="";
     OdMarker *pM;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mO", sipThisObj, sipClass_OdMarker, &pM, &Pos))  
     {
	      for(i=0; i<3; i++) {
      	  temp_py = PyList_GetItem(Pos, i);
	        pos[i] = PyFloat_AsDouble(temp_py);
      	}
      	if(pM) {
	        pM->set_position(pos);
	        msg += "Success!";
      	} else {
	        msg += "Fails in setting position!";
      	}
     }
     return  Py_BuildValue("s", msg.c_str());
%End
     void set_angles(double*);
%MemberCode
     int i;
     PyObject *Pos;
     double ang[3];
     PyObject *temp_py;
     std::string msg="";
     OdMarker *pM;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mO", sipThisObj, sipClass_OdMarker, &pM, &Pos))  
     {
	     for(i=0; i<3; i++) {
	      temp_py = PyList_GetItem(Pos, i);
	      ang[i] = PyFloat_AsDouble(temp_py);
	     }
	     if(pM) {
	       pM->set_angles(ang);
	       msg += "Success!";
	     } else 
	       msg += "Fails in seting angles!";
      }
      return  Py_BuildValue("s", msg.c_str());
%End
     void position(double*);
%MemberCode
       int pva;
       double pos[3];
       double *p;
       OdMarker *pB;
       PyObject *XList=PyList_New(0);
       p = pos;
       if (sipParseArgs(&sipArgsParsed,sipArgs,  "mi", sipThisObj, sipClass_OdMarker, &pB, &pva))  
       {
	         p = pB->position(p, pva);
	         for(int i=0; i<3; i++) {
	           PyList_Append(XList, PyFloat_FromDouble(pos[i]));
	         }
       }
       return XList;
%End
    void orientation(double*);
%MemberCode
    double ori[9];
    double *p;
    OdMarker *pB;
    PyObject *XList=PyList_New(0);
    int pva=0;
    int size = 9;
    p = ori;
    if (sipParseArgs(&sipArgsParsed,sipArgs,  "mi", sipThisObj, sipClass_OdMarker, &pB, &pva))  
    {
       size = (pva == 0) ? 9 : 3;
	     p = pB->orientation(p, pva);
	     for(int i=0; i<size; i++) {
	           PyList_Append(XList, PyFloat_FromDouble(ori[i]));
       }
    }
     return XList;
%End
    void position_orientation(double*);
%MemberCode
    double pos[3];
    double ori[9];
    double sum[16];
    double *p;
    OdMarker *pB;
    int i;
    PyObject *XList=PyList_New(16);
    for(i=0; i<16; i++) {
	    sum[i]=0.0;
    }
    sum[15]=1.0;
    if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdMarker, &pB))  
    {
      p = pos;
      p = pB->position(p);
      p = ori;
      p = pB->orientation(p);
      for(i=12; i<15; i++) {
	      sum[i] = pos[i-12];
      }
      for(i=0; i<3; i++) {
	      sum[i*4] = ori[i];
	      sum[i*4+1] = ori[i+3];
	      sum[i*4+2] = ori[i+6];
      }
      for(i=0; i<16; i++) {
         PyList_SetItem(XList, i, PyFloat_FromDouble(sum[i]));
      }
    }
    return XList;
%End
    char* info();
%MemberCode
     //std::string msg;
     char msg[1000];
     OdMarker *pB;
     char* pmsg = msg;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdMarker, &pB))  
     {
	   if (pB) { 	
		pmsg=pB->info(pmsg);
	   } 
     }
     return  Py_BuildValue("s", msg);
%End
};

class OdJoint
{
   private:
   public:
     OdJoint(int, int);
%MemberCode
     int id;
	   PyObject *pystr;
     if (sipParseArgs(&sipArgsParsed,sipArgs, "iO", &id, &pystr)) {
	     char* pstr=PyString_AsString(pystr);
	     Py_BEGIN_ALLOW_THREADS
	     sipCpp = new OdJoint(id, pstr);
      Py_END_ALLOW_THREADS
     }  
%End
    char* info();
%MemberCode
    //std::string msg;
     char msg[1000];
     OdJoint *pB;
     char* pmsg = msg;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdJoint, &pB))  
     {
	     if (pB) { 	
	          pmsg= pB->info(pmsg);
	     } 
     }
     return  Py_BuildValue("s", msg);
%End
  double* disp();
%MemberCode
	double temp_d[6];
  double *pd;
  OdJoint *pB;
  int dof;
  PyObject *XList=PyList_New(0);
  if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdJoint, &pB))  {
     dof = pB->dofs();
     pd = temp_d;
     pd = pB->disp(pd);
     for(int i=0; i<dof; i++) {
	     PyList_Append(XList, PyFloat_FromDouble(temp_d[i]));
     }
  }
  return XList;
%End
  double* vel();
%MemberCode
  double temp_d[6];
  double *pd;
  OdJoint *pB;
  int dof;
  PyObject *XList=PyList_New(0);
  if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdJoint, &pB))  {
     dof = pB->dofs();
     pd = temp_d;
     pd = pB->vel(pd);
     for(int i=0; i<dof; i++) {
	      PyList_Append(XList, PyFloat_FromDouble(temp_d[i]));
     }
  }
  return XList;
%End
  double* acc();
%MemberCode
  double temp_d[6];
  double *pd;
  OdJoint *pB;
  int dof;
  PyObject *XList=PyList_New(0);
  if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdJoint, &pB))  {
     dof = pB->dofs();
     pd = temp_d;
     pd = pB->acc(pd);
     for(int i=0; i<dof; i++) {
	      PyList_Append(XList, PyFloat_FromDouble(temp_d[i]));
     }
  }
  return XList;
%End
  void set_imarker(OdMarker*);
%MemberCode
     std::string msg="";
     OdMarker *pM;
     PyObject *pO;
     OdJoint *pC;
     int flag = 0;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mO", sipThisObj, sipClass_OdJoint, &pC, &pO))  
     {
        int iserrp=0;
	     pM = sipForceConvertTo_OdMarker(pO, &iserrp);
        if(pC && !iserrp) {
	           pC->set_imarker(pM);
	           flag = 1;
	           msg += "I Marker is added!";
        }
     }
     return  Py_BuildValue("s", msg.c_str());
%End
  void set_jmarker(OdMarker*);
%MemberCode
     std::string msg="";
     OdMarker *pM;
     OdJoint *pC;
     PyObject *pO;
     int flag = 0;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mO", sipThisObj, sipClass_OdJoint, &pC, &pO))  
     {
        int iserrp=0;
	     pM = sipForceConvertTo_OdMarker(pO, &iserrp);
        if(pC && !iserrp) {
	           pC->set_jmarker(pM);
	           flag = 1;
	           msg += "J Marker is added!";
        }
     }
     return  Py_BuildValue("s", msg.c_str());
%End
 void set_expression(char*);
%MemberCode
     char* expr;
     std::string msg="";
     PyStringObject *PyStr;
     OdJoint *pC;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mO", sipThisObj, sipClass_OdJoint, &pC, &PyStr))  {
        int iserrp=0;
	     expr = PyString_AsString((PyObject*)PyStr);
	     pC->set_expr(expr);
	     msg += "expression is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
%End
void fixed();
%MemberCode
     OdJoint *pC;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdJoint, &pC))  
     {
	     if(pC) pC->fixed();
     }  
     return  Py_BuildValue("s", "Free Joint");
%End

void free();
%MemberCode
     OdJoint *pC;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdJoint, &pC))  
     {
	     if(pC) pC->free();
     }  
     return  Py_BuildValue("s", "Free Joint");
%End


void revolute();
%MemberCode
     OdJoint *pC;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdJoint, &pC))  
     {
	     if(pC) pC->revolute();
     }  
     return  Py_BuildValue("s", "Revolute Joint");
%End
void translational();
%MemberCode
     OdJoint *pC;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdJoint, &pC))  
     {
	     if(pC) pC->translational();
     }  
     return  Py_BuildValue("s", "Translational Joint");
%End
void txyrz();
%MemberCode
     OdJoint *pC;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdJoint, &pC))  
     {
	     if(pC) pC->txyrz();
     }  
     return  Py_BuildValue("s", "TxyRz Joint");
%End
void spherical();
%MemberCode
     OdJoint *pC;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdJoint, &pC))  
     {
	     if(pC) pC->spherical();
     }  
     return  Py_BuildValue("s", "Spherical Joint");
%End
void universal();
%MemberCode
     OdJoint *pC;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdJoint, &pC))  
     {
	     if(pC) pC->universal();
     }  
     return  Py_BuildValue("s", "Universal Joint");
%End
};

class OdJointSPDP
{
   private:
   public:
     OdJointSPDP(int, int);
%MemberCode
     int id;
     PyObject *pystr;
     if (sipParseArgs(&sipArgsParsed,sipArgs, "iO", &id, &pystr)) {
        char* pstr=PyString_AsString(pystr);
        Py_BEGIN_ALLOW_THREADS
        sipCpp = new OdJointSPDP(id, pstr);
        Py_END_ALLOW_THREADS
     }  
%End

  void set_joint(OdJoint*);
%MemberCode
     int ith;
     std::string msg="";
     OdJoint *pM;
     PyObject *pO;
     OdJointSPDP *pC;
     int flag = 0;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mOi", sipThisObj, sipClass_OdJointSPDP, &pC, &pO, &ith))  
     {
        int iserrp=0;
	     pM = sipForceConvertTo_OdJoint(pO, &iserrp);
        if(pC && !iserrp) {
	           pC->setJoint(pM, ith);
	           flag = 1;
	           msg += "I Marker is added!";
        }
     }
     return  Py_BuildValue("s", msg.c_str());
%End
 void set_damping(double);
%MemberCode
     float val=0.0;
     std::string msg="";
     OdJointSPDP *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdJointSPDP, &pF, &val))  {
	     pF->set_damping((float)val);
	     msg += "damping is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
%End
 void set_stiffness(double);
%MemberCode
     float val=0.0;
     std::string msg="";
     OdJointSPDP *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdJointSPDP, &pF, &val))  {
	     pF->set_stiffness((float)val);
	     msg += "stiffness is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
%End
 void set_distance(double);
%MemberCode
     float val=0.0;
     std::string msg="";
     OdJointSPDP *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdJointSPDP, &pF, &val))  {
	     pF->set_distance((float)val);
	     msg += "distance is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
%End
 void set_force(double);
%MemberCode
     float val=0.0;
     std::string msg="";
     OdJointSPDP *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdJointSPDP, &pF, &val))  {
	     pF->set_force((float)val);
	     msg += "Force is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
%End

};

class OdJointForce 
{
   private:
   public:
     OdJointForce(int, int);
%MemberCode
     int id;
     PyObject *pystr;
     if (sipParseArgs(&sipArgsParsed,sipArgs, "iO", &id, &pystr)) {
        char* pstr=PyString_AsString(pystr);
        Py_BEGIN_ALLOW_THREADS
        sipCpp = new OdJointForce(id, pstr);
        Py_END_ALLOW_THREADS
     }  
%End

 void set_expression(char*);
%MemberCode
     char* expr;
     std::string msg="";
     PyStringObject *PyStr;
     OdJointForce *pC;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mO", sipThisObj, sipClass_OdJointForce, &pC, &PyStr))
     {
        int iserrp=0;
	     expr = PyString_AsString((PyObject*)PyStr);
	     pC->set_expr(expr);
	     msg += "expression is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
%End

  void set_joint(OdJoint*);
%MemberCode
     std::string msg="";
     OdJoint *pM;
     PyObject *pO;
     OdJointForce *pC;
     int flag = 0;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mO", sipThisObj, sipClass_OdJointForce, &pC, &pO))  
     {
        int iserrp=0;
	     pM = sipForceConvertTo_OdJoint(pO, &iserrp);
        if(pC && !iserrp) {
	           pC->setJoint(pM);
	           flag = 1;
	           msg += "I Marker is added!";
        }
     }
     return  Py_BuildValue("s", msg.c_str());
%End
};

class OdForce 
{
   private:
   public:
     OdForce(int, int);
%MemberCode
     int id;
     PyObject *pystr;
     if (sipParseArgs(&sipArgsParsed,sipArgs, "iO", &id, &pystr)) {
        char* pstr=PyString_AsString(pystr);
        Py_BEGIN_ALLOW_THREADS
        sipCpp = new OdForce(id, pstr);
        Py_END_ALLOW_THREADS
     }  
%End
    char* info();
%MemberCode
    //std::string msg;
     char msg[1000];
     OdForce *pB;
     char* pmsg = msg;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdForce, &pB))  
     {
	     if (pB) { 	
	          pmsg= pB->info(pmsg);
	     } 
     }
     return  Py_BuildValue("s", msg);
%End
  void set_imarker(OdMarker*);
%MemberCode
     std::string msg="";
     OdMarker *pM;
     PyObject *pO;
     OdForce *pC;
     int flag = 0;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mO", sipThisObj, sipClass_OdForce, &pC, &pO))  
     {
        int iserrp=0;
	      pM = sipForceConvertTo_OdMarker(pO, &iserrp);
        if(pC && !iserrp) {
      	 pC->set_imarker(pM);
	       flag = 1;
	       msg += "I Marker is added!";
        }
     }
     return  Py_BuildValue("s", msg.c_str());
%End
  void set_jmarker(OdMarker*);
%MemberCode
     std::string msg="";
     OdMarker *pM;
     OdForce *pC;
     PyObject *pO;
     int flag = 0;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mO", sipThisObj, sipClass_OdForce, &pC, &pO))  
     {
        int iserrp=0;
	      pM = sipForceConvertTo_OdMarker(pO, &iserrp);
        if(pC && !iserrp) {
	        pC->set_jmarker(pM);
	        flag = 1;
	        msg += "J Marker is added!";
        }
     }
     return  Py_BuildValue("s", msg.c_str());
%End
 void set_damping(double);
%MemberCode
     float val=0.0;
     std::string msg="";
     OdForce *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdForce, &pF, &val))  {
	     pF->set_damping((float)val);
	     msg += "damping is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
%End
 void set_stiffness(double);
%MemberCode
     float val=0.0;
     std::string msg="";
     OdForce *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdForce, &pF, &val))  {
	     pF->set_stiffness((float)val);
	     msg += "stiffness is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
%End
 void set_distance(double);
%MemberCode
     float val=0.0;
     std::string msg="";
     OdForce *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdForce, &pF, &val))  {
	     pF->set_distance((float)val);
	     msg += "distance is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
%End

 void set_ixx(double);
%MemberCode
     float val=0.0;
     std::string msg="";
     OdForce *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdForce, &pF, &val))  {
	     pF->set_ixx((float)val);
	     msg += "distance is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
%End

 void set_iyy(double);
%MemberCode
     float val=0.0;
     std::string msg="";
     OdForce *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdForce, &pF, &val))  {
	     pF->set_iyy((float)val);
	     msg += "distance is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
%End

 void set_izz(double);
%MemberCode
     float val=0.0;
     std::string msg="";
     OdForce *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdForce, &pF, &val))  {
	     pF->set_izz((float)val);
	     msg += "distance is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
%End

 void set_area(double);
%MemberCode
     float val=0.0;
     std::string msg="";
     OdForce *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdForce, &pF, &val))  {
	     pF->set_area((float)val);
	     msg += "distance is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
%End

 void set_asy(double);
%MemberCode
     float val=0.0;
     std::string msg="";
     OdForce *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdForce, &pF, &val))  {
	     pF->set_asy((float)val);
	     msg += "distance is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
%End

 void set_asz(double);
%MemberCode
     float val=0.0;
     std::string msg="";
     OdForce *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdForce, &pF, &val))  {
	     pF->set_asz((float)val);
	     msg += "distance is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
%End

 void set_ratio(double);
%MemberCode
     float val=0.0;
     std::string msg="";
     OdForce *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdForce, &pF, &val))  {
	     pF->set_ratio((float)val);
	     msg += "distance is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
%End

 void set_emodulus(double);
%MemberCode
     float val=0.0;
     std::string msg="";
     OdForce *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdForce, &pF, &val))  {
	     pF->set_emodulus((float)val);
	     msg += "distance is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
%End

 void set_Kt(double*);
%MemberCode
     std::string msg="";
     double k[3];
     PyObject *K;
     PyObject *temp_py;
     OdForce *pM;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mO", sipThisObj, sipClass_OdForce, &pM, &K))  
     {
        for(int i=0; i<3; i++) {
	        temp_py = PyList_GetItem(K, i);
	        k[i] = PyFloat_AsDouble(temp_py);
        }
        pM->set_Kt(k);
	      msg += "Success!";
     } else {
	      msg += "Fails in seting K!";
     }
      return  Py_BuildValue("s", msg.c_str());
%End
 void set_Kr(double*);
%MemberCode
     std::string msg="";
     double k[3];
     PyObject *K;
     PyObject *temp_py;
     OdForce *pM;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mO", sipThisObj, sipClass_OdForce, &pM, &K))  
     {
        for(int i=0; i<3; i++) {
	        temp_py = PyList_GetItem(K, i);
	        k[i] = PyFloat_AsDouble(temp_py);
        }
        pM->set_Kr(k);
	      msg += "Success!";
     } else {
	      msg += "Fails in seting K!";
     }
      return  Py_BuildValue("s", msg.c_str());
%End
 void set_Ct(double*);
%MemberCode
     std::string msg="";
     double k[3];
     PyObject *K;
     PyObject *temp_py;
     OdForce *pM;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mO", sipThisObj, sipClass_OdForce, &pM, &K))  
     {
        for(int i=0; i<3; i++) {
	        temp_py = PyList_GetItem(K, i);
	        k[i] = PyFloat_AsDouble(temp_py);
        }
        pM->set_Ct(k);
	      msg += "Success!";
     } else {
	      msg += "Fails in seting C!";
     }
      return  Py_BuildValue("s", msg.c_str());
%End
 void set_Cr(double*);
%MemberCode
     std::string msg="";
     double k[3];
     PyObject *K;
     PyObject *temp_py;
     OdForce *pM;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mO", sipThisObj, sipClass_OdForce, &pM, &K))  
     {
        for(int i=0; i<3; i++) {
	        temp_py = PyList_GetItem(K, i);
	        k[i] = PyFloat_AsDouble(temp_py);
        }
        pM->set_Cr(k);
	      msg += "Success!";
     } else {
	      msg += "Fails in seting C!";
     }
      return  Py_BuildValue("s", msg.c_str());
%End
 void set_F(double*);
%MemberCode
     std::string msg="";
     double k[3];
     PyObject *K;
     PyObject *temp_py;
     OdForce *pM;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mO", sipThisObj, sipClass_OdForce, &pM, &K))  
     {
        for(int i=0; i<3; i++) {
	        temp_py = PyList_GetItem(K, i);
	        k[i] = PyFloat_AsDouble(temp_py);
        }
        pM->set_F(k);
	      msg += "Success!";
     } else {
	      msg += "Fails in seting F!";
     }
      return  Py_BuildValue("s", msg.c_str());
%End
 void set_T(double*);
%MemberCode
     std::string msg="";
     double k[3];
     PyObject *K;
     PyObject *temp_py;
     OdForce *pM;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mO", sipThisObj, sipClass_OdForce, &pM, &K))  
     {
        for(int i=0; i<3; i++) {
	        temp_py = PyList_GetItem(K, i);
	        k[i] = PyFloat_AsDouble(temp_py);
        }
        pM->set_T(k);
	      msg += "Success!";
     } else {
	      msg += "Fails in seting F!";
     }
      return  Py_BuildValue("s", msg.c_str());
%End
 void set_gmodulus(double);
%MemberCode
     float val=0.0;
     std::string msg="";
     OdForce *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdForce, &pF, &val))  {
	     pF->set_gmodulus((float)val);
	     msg += "distance is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
%End

 void set_force(double);
%MemberCode
     float val=0.0;
     std::string msg="";
     OdForce *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdForce, &pF, &val))  {
	     pF->set_force((float)val);
	     msg += "force is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
%End
 void set_expression(char*);
%MemberCode
     char* expr;
     std::string msg="";
     PyStringObject *PyStr;
     OdForce *pC;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mO", sipThisObj, sipClass_OdForce, &pC, &PyStr))       {
        int iserrp=0;
	     expr = PyString_AsString((PyObject*)PyStr);
	     pC->set_expr(expr);
	     msg += "expression is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
%End
void sforce();
%MemberCode
     OdForce *pC;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdForce, &pC))  
     {
	     if(pC) pC->sforce();
     }  
     return  Py_BuildValue("s", "Sforce");
%End
void storque();
%MemberCode
     OdForce *pC;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdForce, &pC))  
     {
	     if(pC) pC->storque();
     }  
     return  Py_BuildValue("s", "Storque");
%End
void spdpt();
%MemberCode
     OdForce *pC;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdForce, &pC))  
     {
	     if(pC) pC->spdpt();
     }  
     return  Py_BuildValue("s", "Translational Spring-Damper");
%End

void beam();
%MemberCode
     OdForce *pC;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdForce, &pC))  
     {
	     if(pC) pC->beam();
     }  
     return  Py_BuildValue("s", "Beam");
%End
void bushing();
%MemberCode
     OdForce *pC;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdForce, &pC))  
     {
	     if(pC) pC->bushing();
     }  
     return  Py_BuildValue("s", "Bushing");
%End
};

class OdSystem 
{
  public:
  OdSystem(char*); 
%MemberCode
     char* pname;
     PyObject *PyStr;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "O", &PyStr)) {
	     pname = PyString_AsString((PyObject*)PyStr);
	     Py_BEGIN_ALLOW_THREADS
	     sipCpp = new OdSystem(pname, 1);
       Py_END_ALLOW_THREADS
     }  
%End


void setName(char*);
%MemberCode
     char* pname;
     OdSystem *pS;
     PyObject *PyStr;
	   int flag=0;
     if (sipParseArgs(&sipArgsParsed,sipArgs, "mO", sipThisObj, sipClass_OdSystem, &pS, &PyStr)) {
	     pname = PyString_AsString((PyObject*)PyStr);
       pS->setName(pname);
       flag=1;
     }
     return Py_BuildValue("i", flag);
%End


  void setGravity(double *);
%MemberCode
     int i;
     PyObject *G;
     OdSystem *pS;
     PyObject *temp_py;
     double gra[3];
     int flag=0;
     if (sipParseArgs(&sipArgsParsed,sipArgs, "mO", sipThisObj, sipClass_OdSystem, &pS, &G)) {
       for(i=0; i<3; i++) {
          temp_py =  PyList_GetItem(G, i);
          gra[i] = PyFloat_AsDouble(temp_py);
       }
       if ( pS ) {
        pS->setGravity(gra);
       }
       flag = 1;
     }
     return Py_BuildValue("i", flag);
%End
/*
void add_subsystem(OdSubSystem*);
%MemberCode
     OdSubSystem *pSub;
     OdSystem *pS;
     PyObject *pO;
     int flag=0;
     if (sipParseArgs(&sipArgsParsed,sipArgs, "mO", sipThisObj, sipClass_OdSystem, &pS, &pO)) {
        int iserp = 0;
        pSub = sipForceConvertTo_OdSubSystem(pO, &iserp);
        if(pS && !iserp) {
	       //pS->add_subsystem(pSub);
	       flag = 1;
        }
     } 
     return Py_BuildValue("i", flag);

%End
*/

  void add_body(OdBody*); 
%MemberCode
     OdBody *pb;
     OdSystem *pS;
     PyObject *pO;
     int flag=0;
     if (sipParseArgs(&sipArgsParsed,sipArgs, "mO", sipThisObj, sipClass_OdSystem, &pS, &pO)) {
        int iserp = 0;
        pb = sipForceConvertTo_OdBody(pO, &iserp);
        if(pS && !iserp) {
	       pS->add_body(pb);
	       flag = 1;
        }
     } 
     return Py_BuildValue("i", flag);
%End

  void add_constraint(OdJoint*);
%MemberCode
     OdJoint *pc;
     OdSystem *pS;
     PyObject *pO;
     int flag=0;
     if (sipParseArgs(&sipArgsParsed,sipArgs, "mO", sipThisObj, sipClass_OdSystem, &pS, &pO)) {
    	int iserp = 0;
        pc = sipForceConvertTo_OdJoint(pO, &iserp);
       	if (pS && !iserp) {
	       pS->add_constraint(pc);
	       flag = 1;
       	}
     } 
     return Py_BuildValue("i", flag);
%End
  void add_force(OdForce*);
%MemberCode
     OdForce *pc;
     OdSystem *pS;
     PyObject *pO;
     int flag=0;
     if (sipParseArgs(&sipArgsParsed,sipArgs, "mO", sipThisObj, sipClass_OdSystem, &pS, &pO)) {
    	int iserp = 0;
        pc = sipForceConvertTo_OdForce(pO, &iserp);
       	if (pS && !iserp) {
	       pS->add_force(pc);
	       flag = 1;
       	}
     } 
     return Py_BuildValue("i", flag);
%End
  void add_joint_force(OdJointForce*);
%MemberCode
     OdJointForce *pc;
     OdSystem *pS;
     PyObject *pO;
     int flag=0;
     if (sipParseArgs(&sipArgsParsed,sipArgs, "mO", sipThisObj, sipClass_OdSystem, &pS, &pO)) {
    	int iserp = 0;
        pc = sipForceConvertTo_OdJointForce(pO, &iserp);
       	if (pS && !iserp) {
	       pS->add_joint_force(pc);
	       flag = 1;
       	}
     } 
     return Py_BuildValue("i", flag);
%End

  void add_joint_spdp(OdJointSPDP*);
%MemberCode
     OdJointSPDP *pc;
     OdSystem *pS;
     PyObject *pO;
     int flag=0;
     if (sipParseArgs(&sipArgsParsed,sipArgs, "mO", sipThisObj, sipClass_OdSystem, &pS, &pO)) {
    	int iserp = 0;
        pc = sipForceConvertTo_OdJointSPDP(pO, &iserp);
       	if (pS && !iserp) {
	       pS->add_joint_spdp(pc);
	       flag = 1;
       	}
     } 
     return Py_BuildValue("i", flag);
%End

/*
  void get_incidence(vector<int>&);
%MemberCode
  int len, num_b, num_j;
  vector_int array;
  OdSystem *pS;
  PyObject *XList=PyList_New(0);
     if (sipParseArgs(&sipArgsParsed,sipArgs, "m", sipThisObj, sipClass_OdSystem, &pS)) {
	     pS->create_incidence();
	     num_b = pS->get_num_body();
	     num_j = pS->get_num_constraint();
      array.resize(num_b*num_j);
	     pS->create_incidence(array);
	     len = array.size();
	     num_b = pS->get_num_body();
	     num_j = pS->get_num_constraint();
	     if(len == num_b*num_j) {
	      for (int i=0; i<len ; i++) {
	        PyList_Append(XList, PyInt_FromLong((long)array[i]));
	      }
	     }
  }
  return XList;
%End
  void get_relevence(vector<int>&);
%MemberCode
  int len, num_j;
  vector_int array;
  OdSystem *pS;
  PyObject *XList=PyList_New(0);
  if (sipParseArgs(&sipArgsParsed,sipArgs, "m", sipThisObj, sipClass_OdSystem, &pS)) {
  	num_j = pS->get_num_constraint();
		array.resize(num_j*num_j);
  	pS->create_relevence(array);
  	len = array.size();
  	if(len == num_j*num_j) {
  	    for (int i=0; i<len ; i++) {
		  		PyList_Append(XList, PyInt_FromLong((long)array[i]));
  	    }	
  	}
  }
  return XList;
%End
*/
/*
void topology_analysis(vector<int>&, vector<int>&, vector<int>& );
%MemberCode
  int len_i, i, len;
  int num_b, num_j;
  int len_j, j, temp_int;
  vector_int inci;
  vector_int rele;
  vector_int perm;  
  vector_int b_perm;  
  vector_int j_perm;  
  vector_int top_b;  
  vector_int free_js;  
  vector_int broken_bs;  
  OdSystem *pS;
  PyObject *XList_inci=PyList_New(0);
  PyObject *XList_rele=PyList_New(0);
  PyObject *XList_sort_rele=PyList_New(0);
  PyObject *XList_bperm=PyList_New(0);
  PyObject *XList_jperm=PyList_New(0);
  PyObject *XList_topb=PyList_New(0);
  PyObject *XList_freejs=PyList_New(0);
  PyObject *XList_brokenbs=PyList_New(0);
  if (sipParseArgs(&sipArgsParsed,sipArgs, "m", sipThisObj, sipClass_OdSystem, &pS)) {
	pS->connnect_floating_subsystems();
	pS->break_loops();
	pS->connnect_floating_subsystems();
  pS->topology_analysis();
	num_b = pS->get_num_body();
	num_j = pS->get_num_constraint();
	cout << "body: " << num_b << "  joint:" << num_j << endl;
	perm.resize(num_b);
	inci.resize(num_b*num_j);
	rele.resize(num_j*num_j);
  	pS->topology_analysis(perm, inci, rele );
	len = inci.size();
  	for (i=0; i<len ; i++) { PyList_Append(XList_inci, PyInt_FromLong((long)inci[i])); }	
	len = rele.size();
  	for (i=0; i<len ; i++) { PyList_Append(XList_rele, PyInt_FromLong((long)rele[i])); }	
	b_perm.resize(num_b);
	j_perm.resize(num_j);
	pS->sort_branch(rele, b_perm, j_perm);
	len_i = b_perm.size();
  	for (i=0; i<len_i ; i++) { PyList_Append(XList_bperm, PyInt_FromLong((long)b_perm[i])); }	
		len_j = j_perm.size();
  	for (i=0; i<len_j ; i++) { PyList_Append(XList_jperm, PyInt_FromLong((long)j_perm[i])); }	
  	for (i=0; i<len_i ; i++) { 
	   for(j=0; j< len_j; j++) {
				temp_int = rele[len_j*b_perm[i]+j_perm[j]];
				PyList_Append(XList_sort_rele, PyInt_FromLong((long)temp_int)); 
 	   }
		}	
	top_b.resize(num_j*num_b);
  pS->find_element_on_branches(rele, b_perm, j_perm, top_b);
	len_i = top_b.size();
  for (i=0; i<len_i ; i++) { PyList_Append(XList_topb, PyInt_FromLong((long)top_b[i])); }	
	free_js.resize(num_j*num_b);
	pS->free_joints(free_js);
  for (i=0; i<free_js.size() ; i++) { PyList_Append(XList_freejs, PyInt_FromLong((long)free_js[i])); }	
	broken_bs.resize(num_b);
  pS->broken_bodies(broken_bs);
  for (i=0; i<broken_bs.size() ; i++) { PyList_Append(XList_brokenbs, PyInt_FromLong((long)broken_bs[i])); }	
  return  Py_BuildValue("OOOOOOOO", XList_inci, XList_rele, XList_bperm, XList_jperm, XList_sort_rele, XList_topb, XList_freejs, XList_brokenbs);
  }
 
%End
*/
 
    char* info();
%MemberCode
     char msg[1000];
     OdSystem *pS;
     char* pmsg = msg;
     if (sipParseArgs(&sipArgsParsed,sipArgs, "m", sipThisObj, sipClass_OdSystem, &pS)) {
			if(pS) {
	   		pmsg = pS->info(pmsg);
      }
     }
     return  Py_BuildValue("s", msg);
%End
int get_number_of_bodies();
%MemberCode
    OdSystem *pS;
    int num=0;
    if (sipParseArgs(&sipArgsParsed,sipArgs, "m", sipThisObj, sipClass_OdSystem, &pS)) {
			if(pS) {
	  		num = pS->get_num_body();
			}
    }
     return  Py_BuildValue("i", num);
%End
int get_number_of_joints();
%MemberCode
    int num=0;
    OdSystem *pS;
    if (sipParseArgs(&sipArgsParsed,sipArgs, "m", sipThisObj, sipClass_OdSystem, &pS)) {
			if(pS) {
	  		num = pS->get_num_constraint();
			}
    }
     return  Py_BuildValue("i", num);
%End
int displacement_ic();
%MemberCode
    int error_code=0;
    OdSystem *pS;
    std::string _msg;
    if (sipParseArgs(&sipArgsParsed,sipArgs, "m", sipThisObj, sipClass_OdSystem, &pS)) {
	 if(pS) {
	    try { error_code=pS->displacement_ic(); }
	    catch (std::string msg) {
	       _msg = msg;
	       error_code = 1;
        }
	    }
    }
     return  Py_BuildValue("is", error_code, _msg.c_str());
%End

int velocity_ic();
%MemberCode
    int error_code=0;
    OdSystem *pS;
    if (sipParseArgs(&sipArgsParsed,sipArgs, "m", sipThisObj, sipClass_OdSystem, &pS)) {
        if(pS) {
	   error_code=pS->velocity_ic();
        }
    }
     return  Py_BuildValue("i", error_code);
%End

int acceleration_and_force_ic();
%MemberCode
    int error_code=0;
    OdSystem *pS;
    if (sipParseArgs(&sipArgsParsed,sipArgs, "m", sipThisObj, sipClass_OdSystem, &pS)) {
        if(pS) {
	   error_code=pS->acceleration_and_force_ic();
        }
    }
     return  Py_BuildValue("i", error_code);
%End

double cpu_time(); 
%MemberCode
    double val;
    OdSystem *pS;
    PyObject *XList=PyList_New(0);
    val=0.0;
    if (sipParseArgs(&sipArgsParsed,sipArgs, "m", sipThisObj, sipClass_OdSystem, &pS)) {
	  if(pS) {
        val = pS->cpuTime();
	      PyList_Append(XList, PyFloat_FromDouble(val));
        val = pS->jacTime();
	      PyList_Append(XList, PyFloat_FromDouble(val));
        val = pS->rhsTime();
	      PyList_Append(XList, PyFloat_FromDouble(val));
        val = pS->solTime();
	      PyList_Append(XList, PyFloat_FromDouble(val));
      }
    }  
    return XList;
%End

int dynamic_analysis_hht(double, double, int, double, double, double, int);
%MemberCode
    int error_code;
    OdSystem *pS=0;
    double end, tol, minh, maxh, inith;
    int  iters, __debug; 
    std::string _msg;
    int iserrp = 0;
    _msg = "Untouched"; error_code =0;
    if (sipParseArgs(&sipArgsParsed,sipArgs, "mddidddi", 
    	sipThisObj, sipClass_OdSystem, &pS, &end,  &tol, &iters, &maxh, &minh, &inith, &__debug)) {
    	if(pS)  {
        try {
          pS->dynamic_analysis_hht(end, tol, iters, maxh, minh, inith, __debug);
          _msg = "Success";
        } catch (std::string msg) {
					_msg = msg;
	        error_code = 1;
        }
      } else {
          _msg = "zero PS";
      }
     }
     return  Py_BuildValue("is", error_code, _msg.c_str());
%End

int dynamic_analysis_bdf(double, double, int, double, double, double, int);
%MemberCode
    int error_code;
    OdSystem *pS=0;
    double end, tol, minh, maxh, inith;
    int  iters, __debug; 
    std::string _msg;
    int iserrp = 0;
    _msg = "Untouched"; error_code =0;
    if (sipParseArgs(&sipArgsParsed,sipArgs, "mddidddi", 
    	sipThisObj, sipClass_OdSystem, &pS, &end,  &tol, &iters, &maxh, &minh, &inith, &__debug)) {
    	if(pS)  {
        try {
          pS->dynamic_analysis_bdf(end, tol, iters, maxh, minh, inith, __debug);
          _msg = "Success";
        } catch (std::string msg) {
					_msg = msg;
	        error_code = 1;
        }
      } else {
          _msg = "zero PS";
      }
     }
     return  Py_BuildValue("is", error_code, _msg.c_str());
%End

int kinematic_analysis(double, int, double, int);
%MemberCode
    int error_code=0;
    OdSystem *pS;
    double end, tol;
    int num, iters; 
    std::string _msg;
    if (sipParseArgs(&sipArgsParsed,sipArgs, "mdidi", 
    	sipThisObj, sipClass_OdSystem, &pS, &end, &num, &tol, &iters)) {
    	if(pS)  {
	       try{ 	error_code=pS->kinematic_analysis(end, num, tol, iters);}
         catch (std::string msg) {
					_msg = msg;
	        error_code = 1;
         }
  		}
    }
     return  Py_BuildValue("is", error_code, _msg.c_str());
%End
int static_analysis(int, double );
%MemberCode
    int error_code=0;
    OdSystem *pS;
    double tol;
    int num; 
    std::string _msg;
    if (sipParseArgs(&sipArgsParsed,sipArgs, "mid", 
    	sipThisObj, sipClass_OdSystem, &pS, &num, &tol)) {
    	if(pS)  {
	       try{ 	error_code=pS->static_analysis(num, tol);}
         catch (std::string msg) {
					_msg = msg;
	        error_code = 1;
         }
  		}
    }
     return  Py_BuildValue("is", error_code, _msg.c_str());
%End
int numdif();
%MemberCode
    int error_code=0;
    OdSystem *pS;
    if (sipParseArgs(&sipArgsParsed,sipArgs, "m", sipThisObj, sipClass_OdSystem, &pS)) {
	  if(pS) {
          pS->numdif();
      }
    }
     return  Py_BuildValue("i", error_code);
%End
};

