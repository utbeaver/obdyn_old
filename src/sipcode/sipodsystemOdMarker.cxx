// C++ interface wrapper code.
//
// Generated by SIP 3.10.2 (3.10.2-205) on Wed Mar 27 22:12:12 2019

#include "sipodsystemDeclodsystem.h"
#include "sipodsystemOdMarker.h"


PyObject *sipClass_OdMarker;

static void sipDealloc_OdMarker(sipThisType *);

static PyTypeObject sipType_OdMarker = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	sipWrapperTypeName,
	sizeof (sipThisType),
	0,
	(destructor)sipDealloc_OdMarker,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	Py_TPFLAGS_DEFAULT,
};

static PyObject *sipDo_OdMarker_set_velocity(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 276 "python\od_system.sip"
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
#line 69 "sipodsystemOdMarker.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdMarker,sipName_set_velocity);

	return NULL;
}

static PyObject *sipDo_OdMarker_set_omega(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 304 "python\od_system.sip"
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
#line 110 "sipodsystemOdMarker.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdMarker,sipName_set_omega);

	return NULL;
}

static PyObject *sipDo_OdMarker_set_position(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 332 "python\od_system.sip"
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
#line 146 "sipodsystemOdMarker.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdMarker,sipName_set_position);

	return NULL;
}

static PyObject *sipDo_OdMarker_set_angles(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 355 "python\od_system.sip"
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
#line 181 "sipodsystemOdMarker.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdMarker,sipName_set_angles);

	return NULL;
}

static PyObject *sipDo_OdMarker_position(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 377 "python\od_system.sip"
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
#line 211 "sipodsystemOdMarker.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdMarker,sipName_position);

	return NULL;
}

static PyObject *sipDo_OdMarker_orientation(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 394 "python\od_system.sip"
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
#line 243 "sipodsystemOdMarker.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdMarker,sipName_orientation);

	return NULL;
}

static PyObject *sipDo_OdMarker_position_orientation(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 413 "python\od_system.sip"
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
#line 289 "sipodsystemOdMarker.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdMarker,sipName_position_orientation);

	return NULL;
}

static PyObject *sipDo_OdMarker_info(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 446 "python\od_system.sip"
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
#line 316 "sipodsystemOdMarker.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdMarker,sipName_info);

	return NULL;
}

// Cast a pointer to a type somewhere in its superclass hierarchy.

const void *sipCast_OdMarker(const void *ptr,PyObject *targetClass)
{
	if (targetClass == sipClass_OdMarker)
		return ptr;

	return NULL;
}

static void sipDealloc_OdMarker(sipThisType *sipThis)
{
	if (sipThis -> u.cppPtr != NULL)
	{

		if (sipIsPyOwned(sipThis))
		{
			Py_BEGIN_ALLOW_THREADS

			delete (OdMarker *)sipThis -> u.cppPtr;

			Py_END_ALLOW_THREADS
		}
	}

	sipDeleteThis(sipThis);
}

PyObject *sipNew_OdMarker(PyObject *sipSelf,PyObject *sipArgs)
{
	static sipExtraType et = {
		sipCast_OdMarker
	};

	int sipFlags = SIP_PY_OWNED;
	int sipArgsParsed = 0;
	sipThisType *sipThis = NULL;
	OdMarker *sipCpp = NULL;

	// See if there is something pending.

	sipCpp = (OdMarker *)sipGetPending(&sipFlags);

	if (sipCpp == NULL)
	{
#line 212 "python\od_system.sip"
     int id, real;
     PyObject *pystr;
     if (sipParseArgs(&sipArgsParsed,sipArgs, "iO", &id, &pystr)) {
	char* pstr = PyString_AsString(pystr);
	Py_BEGIN_ALLOW_THREADS
	sipCpp = new OdMarker(id, pstr);
        Py_END_ALLOW_THREADS
     }
#line 380 "sipodsystemOdMarker.cxx"
	}

	if (sipCpp == NULL)
	{
#line 223 "python\od_system.sip"
     int id, real;
     PyObject *pystr;
     OdBody *pB;
     if (sipParseArgs(&sipArgsParsed,sipArgs, "imOO", &id, sipClass_OdBody, &pB, &pystr )) {
       char* pstr = PyString_AsString(pystr);
       Py_BEGIN_ALLOW_THREADS
       sipCpp = new OdMarker(id, pB, pstr);
       Py_END_ALLOW_THREADS
     }
#line 395 "sipodsystemOdMarker.cxx"
	}

	if (sipCpp == NULL)
	{
#line 235 "python\od_system.sip"
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
#line 421 "sipodsystemOdMarker.cxx"
	}

	if (sipCpp == NULL)
	{
#line 258 "python\od_system.sip"
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
#line 442 "sipodsystemOdMarker.cxx"
	}

	if (sipCpp == NULL)
	{
		OdMarker * a0;

		if (sipParseArgs(&sipArgsParsed,sipArgs,"J1",sipClass_OdMarker,&a0))
		{
			Py_BEGIN_ALLOW_THREADS
			sipCpp = new OdMarker(*a0);
			Py_END_ALLOW_THREADS
		}
	}

	if (sipCpp == NULL)
	{
		sipNoCtor(sipArgsParsed,sipName_OdMarker);
		return NULL;
	}

	// Wrap the object.

	if ((sipThis = sipCreateThis(sipSelf,sipCpp,&sipType_OdMarker,sipFlags,&et)) == NULL)
	{
		if (sipFlags & SIP_PY_OWNED)
		{
			Py_BEGIN_ALLOW_THREADS

			delete (OdMarker *)sipCpp;

			Py_END_ALLOW_THREADS
		}

		return NULL;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyMethodDef lazyMethods_OdMarker[] = {
	{sipName_info, sipDo_OdMarker_info, METH_VARARGS, NULL},
	{sipName_orientation, sipDo_OdMarker_orientation, METH_VARARGS, NULL},
	{sipName_position, sipDo_OdMarker_position, METH_VARARGS, NULL},
	{sipName_position_orientation, sipDo_OdMarker_position_orientation, METH_VARARGS, NULL},
	{sipName_set_angles, sipDo_OdMarker_set_angles, METH_VARARGS, NULL},
	{sipName_set_omega, sipDo_OdMarker_set_omega, METH_VARARGS, NULL},
	{sipName_set_position, sipDo_OdMarker_set_position, METH_VARARGS, NULL},
	{sipName_set_velocity, sipDo_OdMarker_set_velocity, METH_VARARGS, NULL}
};

sipLazyAttrDef sipClassAttrTab_OdMarker = {
	0, NULL,
	sizeof (lazyMethods_OdMarker) / sizeof (PyMethodDef), lazyMethods_OdMarker,
	0, NULL
};

OdMarker *sipForceConvertTo_OdMarker(PyObject *valobj,int *iserrp)
{
	if (*iserrp || valobj == NULL)
		return NULL;

	if (valobj == Py_None || sipIsSubClassInstance(valobj,sipClass_OdMarker))
		return (OdMarker *)sipConvertToCpp(valobj,sipClass_OdMarker,iserrp);

	sipBadClass(sipName_OdMarker);

	*iserrp = 1;

	return NULL;
}
