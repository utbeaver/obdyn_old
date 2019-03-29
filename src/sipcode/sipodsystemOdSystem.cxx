// C++ interface wrapper code.
//
// Generated by SIP 3.10.2 (3.10.2-205) on Wed Mar 27 22:12:12 2019

#include "sipodsystemDeclodsystem.h"
#include "sipodsystemOdSystem.h"


PyObject *sipClass_OdSystem;

static void sipDealloc_OdSystem(sipThisType *);

static PyTypeObject sipType_OdSystem = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	sipWrapperTypeName,
	sizeof (sipThisType),
	0,
	(destructor)sipDealloc_OdSystem,
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

static PyObject *sipDo_OdSystem_setName(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1216 "python\od_system.sip"
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
#line 54 "sipodsystemOdSystem.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdSystem,sipName_setName);

	return NULL;
}

static PyObject *sipDo_OdSystem_setGravity(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1231 "python\od_system.sip"
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
#line 87 "sipodsystemOdSystem.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdSystem,sipName_setGravity);

	return NULL;
}

static PyObject *sipDo_OdSystem_add_body(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1271 "python\od_system.sip"
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
#line 116 "sipodsystemOdSystem.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdSystem,sipName_add_body);

	return NULL;
}

static PyObject *sipDo_OdSystem_add_constraint(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1288 "python\od_system.sip"
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
#line 145 "sipodsystemOdSystem.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdSystem,sipName_add_constraint);

	return NULL;
}

static PyObject *sipDo_OdSystem_add_force(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1304 "python\od_system.sip"
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
#line 174 "sipodsystemOdSystem.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdSystem,sipName_add_force);

	return NULL;
}

static PyObject *sipDo_OdSystem_add_joint_force(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1320 "python\od_system.sip"
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
#line 203 "sipodsystemOdSystem.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdSystem,sipName_add_joint_force);

	return NULL;
}

static PyObject *sipDo_OdSystem_add_joint_spdp(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1337 "python\od_system.sip"
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
#line 232 "sipodsystemOdSystem.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdSystem,sipName_add_joint_spdp);

	return NULL;
}

static PyObject *sipDo_OdSystem_info(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1466 "python\od_system.sip"
     char msg[1000];
     OdSystem *pS;
     char* pmsg = msg;
     if (sipParseArgs(&sipArgsParsed,sipArgs, "m", sipThisObj, sipClass_OdSystem, &pS)) {
			if(pS) {
	   		pmsg = pS->info(pmsg);
      }
     }
     return  Py_BuildValue("s", msg);
#line 257 "sipodsystemOdSystem.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdSystem,sipName_info);

	return NULL;
}

static PyObject *sipDo_OdSystem_get_number_of_bodies(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1478 "python\od_system.sip"
    OdSystem *pS;
    int num=0;
    if (sipParseArgs(&sipArgsParsed,sipArgs, "m", sipThisObj, sipClass_OdSystem, &pS)) {
			if(pS) {
	  		num = pS->get_num_body();
			}
    }
     return  Py_BuildValue("i", num);
#line 281 "sipodsystemOdSystem.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdSystem,sipName_get_number_of_bodies);

	return NULL;
}

static PyObject *sipDo_OdSystem_get_number_of_joints(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1489 "python\od_system.sip"
    int num=0;
    OdSystem *pS;
    if (sipParseArgs(&sipArgsParsed,sipArgs, "m", sipThisObj, sipClass_OdSystem, &pS)) {
			if(pS) {
	  		num = pS->get_num_constraint();
			}
    }
     return  Py_BuildValue("i", num);
#line 305 "sipodsystemOdSystem.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdSystem,sipName_get_number_of_joints);

	return NULL;
}

static PyObject *sipDo_OdSystem_displacement_ic(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1500 "python\od_system.sip"
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
#line 334 "sipodsystemOdSystem.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdSystem,sipName_displacement_ic);

	return NULL;
}

static PyObject *sipDo_OdSystem_velocity_ic(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1517 "python\od_system.sip"
    int error_code=0;
    OdSystem *pS;
    if (sipParseArgs(&sipArgsParsed,sipArgs, "m", sipThisObj, sipClass_OdSystem, &pS)) {
        if(pS) {
	   error_code=pS->velocity_ic();
        }
    }
     return  Py_BuildValue("i", error_code);
#line 358 "sipodsystemOdSystem.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdSystem,sipName_velocity_ic);

	return NULL;
}

static PyObject *sipDo_OdSystem_acceleration_and_force_ic(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1529 "python\od_system.sip"
    int error_code=0;
    OdSystem *pS;
    if (sipParseArgs(&sipArgsParsed,sipArgs, "m", sipThisObj, sipClass_OdSystem, &pS)) {
        if(pS) {
	   error_code=pS->acceleration_and_force_ic();
        }
    }
     return  Py_BuildValue("i", error_code);
#line 382 "sipodsystemOdSystem.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdSystem,sipName_acceleration_and_force_ic);

	return NULL;
}

static PyObject *sipDo_OdSystem_cpu_time(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1541 "python\od_system.sip"
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
#line 415 "sipodsystemOdSystem.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdSystem,sipName_cpu_time);

	return NULL;
}

static PyObject *sipDo_OdSystem_dynamic_analysis_hht(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1562 "python\od_system.sip"
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
#line 453 "sipodsystemOdSystem.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdSystem,sipName_dynamic_analysis_hht);

	return NULL;
}

static PyObject *sipDo_OdSystem_dynamic_analysis_bdf(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1588 "python\od_system.sip"
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
#line 491 "sipodsystemOdSystem.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdSystem,sipName_dynamic_analysis_bdf);

	return NULL;
}

static PyObject *sipDo_OdSystem_kinematic_analysis(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1614 "python\od_system.sip"
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
#line 523 "sipodsystemOdSystem.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdSystem,sipName_kinematic_analysis);

	return NULL;
}

static PyObject *sipDo_OdSystem_static_analysis(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1633 "python\od_system.sip"
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
#line 555 "sipodsystemOdSystem.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdSystem,sipName_static_analysis);

	return NULL;
}

static PyObject *sipDo_OdSystem_numdif(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1652 "python\od_system.sip"
    int error_code=0;
    OdSystem *pS;
    if (sipParseArgs(&sipArgsParsed,sipArgs, "m", sipThisObj, sipClass_OdSystem, &pS)) {
	  if(pS) {
          pS->numdif();
      }
    }
     return  Py_BuildValue("i", error_code);
#line 579 "sipodsystemOdSystem.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdSystem,sipName_numdif);

	return NULL;
}

// Cast a pointer to a type somewhere in its superclass hierarchy.

const void *sipCast_OdSystem(const void *ptr,PyObject *targetClass)
{
	if (targetClass == sipClass_OdSystem)
		return ptr;

	return NULL;
}

static void sipDealloc_OdSystem(sipThisType *sipThis)
{
	if (sipThis -> u.cppPtr != NULL)
	{

		if (sipIsPyOwned(sipThis))
		{
			Py_BEGIN_ALLOW_THREADS

			delete (OdSystem *)sipThis -> u.cppPtr;

			Py_END_ALLOW_THREADS
		}
	}

	sipDeleteThis(sipThis);
}

PyObject *sipNew_OdSystem(PyObject *sipSelf,PyObject *sipArgs)
{
	static sipExtraType et = {
		sipCast_OdSystem
	};

	int sipFlags = SIP_PY_OWNED;
	int sipArgsParsed = 0;
	sipThisType *sipThis = NULL;
	OdSystem *sipCpp = NULL;

	// See if there is something pending.

	sipCpp = (OdSystem *)sipGetPending(&sipFlags);

	if (sipCpp == NULL)
	{
#line 1203 "python\od_system.sip"
     char* pname;
     PyObject *PyStr;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "O", &PyStr)) {
	     pname = PyString_AsString((PyObject*)PyStr);
	     Py_BEGIN_ALLOW_THREADS
	     sipCpp = new OdSystem(pname, 1);
       Py_END_ALLOW_THREADS
     }  
#line 643 "sipodsystemOdSystem.cxx"
	}

	if (sipCpp == NULL)
	{
		OdSystem * a0;

		if (sipParseArgs(&sipArgsParsed,sipArgs,"J1",sipClass_OdSystem,&a0))
		{
			Py_BEGIN_ALLOW_THREADS
			sipCpp = new OdSystem(*a0);
			Py_END_ALLOW_THREADS
		}
	}

	if (sipCpp == NULL)
	{
		sipNoCtor(sipArgsParsed,sipName_OdSystem);
		return NULL;
	}

	// Wrap the object.

	if ((sipThis = sipCreateThis(sipSelf,sipCpp,&sipType_OdSystem,sipFlags,&et)) == NULL)
	{
		if (sipFlags & SIP_PY_OWNED)
		{
			Py_BEGIN_ALLOW_THREADS

			delete (OdSystem *)sipCpp;

			Py_END_ALLOW_THREADS
		}

		return NULL;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyMethodDef lazyMethods_OdSystem[] = {
	{sipName_acceleration_and_force_ic, sipDo_OdSystem_acceleration_and_force_ic, METH_VARARGS, NULL},
	{sipName_add_body, sipDo_OdSystem_add_body, METH_VARARGS, NULL},
	{sipName_add_constraint, sipDo_OdSystem_add_constraint, METH_VARARGS, NULL},
	{sipName_add_force, sipDo_OdSystem_add_force, METH_VARARGS, NULL},
	{sipName_add_joint_force, sipDo_OdSystem_add_joint_force, METH_VARARGS, NULL},
	{sipName_add_joint_spdp, sipDo_OdSystem_add_joint_spdp, METH_VARARGS, NULL},
	{sipName_cpu_time, sipDo_OdSystem_cpu_time, METH_VARARGS, NULL},
	{sipName_displacement_ic, sipDo_OdSystem_displacement_ic, METH_VARARGS, NULL},
	{sipName_dynamic_analysis_bdf, sipDo_OdSystem_dynamic_analysis_bdf, METH_VARARGS, NULL},
	{sipName_dynamic_analysis_hht, sipDo_OdSystem_dynamic_analysis_hht, METH_VARARGS, NULL},
	{sipName_get_number_of_bodies, sipDo_OdSystem_get_number_of_bodies, METH_VARARGS, NULL},
	{sipName_get_number_of_joints, sipDo_OdSystem_get_number_of_joints, METH_VARARGS, NULL},
	{sipName_info, sipDo_OdSystem_info, METH_VARARGS, NULL},
	{sipName_kinematic_analysis, sipDo_OdSystem_kinematic_analysis, METH_VARARGS, NULL},
	{sipName_numdif, sipDo_OdSystem_numdif, METH_VARARGS, NULL},
	{sipName_setGravity, sipDo_OdSystem_setGravity, METH_VARARGS, NULL},
	{sipName_setName, sipDo_OdSystem_setName, METH_VARARGS, NULL},
	{sipName_static_analysis, sipDo_OdSystem_static_analysis, METH_VARARGS, NULL},
	{sipName_velocity_ic, sipDo_OdSystem_velocity_ic, METH_VARARGS, NULL}
};

sipLazyAttrDef sipClassAttrTab_OdSystem = {
	0, NULL,
	sizeof (lazyMethods_OdSystem) / sizeof (PyMethodDef), lazyMethods_OdSystem,
	0, NULL
};

OdSystem *sipForceConvertTo_OdSystem(PyObject *valobj,int *iserrp)
{
	if (*iserrp || valobj == NULL)
		return NULL;

	if (valobj == Py_None || sipIsSubClassInstance(valobj,sipClass_OdSystem))
		return (OdSystem *)sipConvertToCpp(valobj,sipClass_OdSystem,iserrp);

	sipBadClass(sipName_OdSystem);

	*iserrp = 1;

	return NULL;
}
