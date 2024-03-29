// C++ interface wrapper code.
//
// Generated by SIP 3.10.2 (3.10.2-205) on Wed Mar 27 22:12:12 2019

#include "sipodsystemDeclodsystem.h"
#include "sipodsystemOdJoint.h"


PyObject *sipClass_OdJoint;

static void sipDealloc_OdJoint(sipThisType *);

static PyTypeObject sipType_OdJoint = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	sipWrapperTypeName,
	sizeof (sipThisType),
	0,
	(destructor)sipDealloc_OdJoint,
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

static PyObject *sipDo_OdJoint_info(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 477 "python\od_system.sip"
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
#line 55 "sipodsystemOdJoint.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdJoint,sipName_info);

	return NULL;
}

static PyObject *sipDo_OdJoint_disp(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 491 "python\od_system.sip"
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
#line 85 "sipodsystemOdJoint.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdJoint,sipName_disp);

	return NULL;
}

static PyObject *sipDo_OdJoint_vel(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 508 "python\od_system.sip"
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
#line 115 "sipodsystemOdJoint.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdJoint,sipName_vel);

	return NULL;
}

static PyObject *sipDo_OdJoint_acc(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 525 "python\od_system.sip"
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
#line 145 "sipodsystemOdJoint.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdJoint,sipName_acc);

	return NULL;
}

static PyObject *sipDo_OdJoint_set_imarker(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 542 "python\od_system.sip"
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
#line 177 "sipodsystemOdJoint.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdJoint,sipName_set_imarker);

	return NULL;
}

static PyObject *sipDo_OdJoint_set_jmarker(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 561 "python\od_system.sip"
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
#line 209 "sipodsystemOdJoint.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdJoint,sipName_set_jmarker);

	return NULL;
}

static PyObject *sipDo_OdJoint_set_expression(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 580 "python\od_system.sip"
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
#line 236 "sipodsystemOdJoint.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdJoint,sipName_set_expression);

	return NULL;
}

static PyObject *sipDo_OdJoint_fixed(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 594 "python\od_system.sip"
     OdJoint *pC;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdJoint, &pC))  
     {
	     if(pC) pC->fixed();
     }  
     return  Py_BuildValue("s", "Free Joint");
#line 258 "sipodsystemOdJoint.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdJoint,sipName_fixed);

	return NULL;
}

static PyObject *sipDo_OdJoint_free(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 604 "python\od_system.sip"
     OdJoint *pC;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdJoint, &pC))  
     {
	     if(pC) pC->free();
     }  
     return  Py_BuildValue("s", "Free Joint");
#line 280 "sipodsystemOdJoint.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdJoint,sipName_free);

	return NULL;
}

static PyObject *sipDo_OdJoint_revolute(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 615 "python\od_system.sip"
     OdJoint *pC;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdJoint, &pC))  
     {
	     if(pC) pC->revolute();
     }  
     return  Py_BuildValue("s", "Revolute Joint");
#line 302 "sipodsystemOdJoint.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdJoint,sipName_revolute);

	return NULL;
}

static PyObject *sipDo_OdJoint_translational(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 624 "python\od_system.sip"
     OdJoint *pC;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdJoint, &pC))  
     {
	     if(pC) pC->translational();
     }  
     return  Py_BuildValue("s", "Translational Joint");
#line 324 "sipodsystemOdJoint.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdJoint,sipName_translational);

	return NULL;
}

static PyObject *sipDo_OdJoint_txyrz(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 633 "python\od_system.sip"
     OdJoint *pC;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdJoint, &pC))  
     {
	     if(pC) pC->txyrz();
     }  
     return  Py_BuildValue("s", "TxyRz Joint");
#line 346 "sipodsystemOdJoint.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdJoint,sipName_txyrz);

	return NULL;
}

static PyObject *sipDo_OdJoint_spherical(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 642 "python\od_system.sip"
     OdJoint *pC;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdJoint, &pC))  
     {
	     if(pC) pC->spherical();
     }  
     return  Py_BuildValue("s", "Spherical Joint");
#line 368 "sipodsystemOdJoint.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdJoint,sipName_spherical);

	return NULL;
}

static PyObject *sipDo_OdJoint_universal(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 651 "python\od_system.sip"
     OdJoint *pC;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdJoint, &pC))  
     {
	     if(pC) pC->universal();
     }  
     return  Py_BuildValue("s", "Universal Joint");
#line 390 "sipodsystemOdJoint.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdJoint,sipName_universal);

	return NULL;
}

// Cast a pointer to a type somewhere in its superclass hierarchy.

const void *sipCast_OdJoint(const void *ptr,PyObject *targetClass)
{
	if (targetClass == sipClass_OdJoint)
		return ptr;

	return NULL;
}

static void sipDealloc_OdJoint(sipThisType *sipThis)
{
	if (sipThis -> u.cppPtr != NULL)
	{

		if (sipIsPyOwned(sipThis))
		{
			Py_BEGIN_ALLOW_THREADS

			delete (OdJoint *)sipThis -> u.cppPtr;

			Py_END_ALLOW_THREADS
		}
	}

	sipDeleteThis(sipThis);
}

PyObject *sipNew_OdJoint(PyObject *sipSelf,PyObject *sipArgs)
{
	static sipExtraType et = {
		sipCast_OdJoint
	};

	int sipFlags = SIP_PY_OWNED;
	int sipArgsParsed = 0;
	sipThisType *sipThis = NULL;
	OdJoint *sipCpp = NULL;

	// See if there is something pending.

	sipCpp = (OdJoint *)sipGetPending(&sipFlags);

	if (sipCpp == NULL)
	{
#line 466 "python\od_system.sip"
     int id;
	   PyObject *pystr;
     if (sipParseArgs(&sipArgsParsed,sipArgs, "iO", &id, &pystr)) {
	     char* pstr=PyString_AsString(pystr);
	     Py_BEGIN_ALLOW_THREADS
	     sipCpp = new OdJoint(id, pstr);
      Py_END_ALLOW_THREADS
     }  
#line 454 "sipodsystemOdJoint.cxx"
	}

	if (sipCpp == NULL)
	{
		OdJoint * a0;

		if (sipParseArgs(&sipArgsParsed,sipArgs,"J1",sipClass_OdJoint,&a0))
		{
			Py_BEGIN_ALLOW_THREADS
			sipCpp = new OdJoint(*a0);
			Py_END_ALLOW_THREADS
		}
	}

	if (sipCpp == NULL)
	{
		sipNoCtor(sipArgsParsed,sipName_OdJoint);
		return NULL;
	}

	// Wrap the object.

	if ((sipThis = sipCreateThis(sipSelf,sipCpp,&sipType_OdJoint,sipFlags,&et)) == NULL)
	{
		if (sipFlags & SIP_PY_OWNED)
		{
			Py_BEGIN_ALLOW_THREADS

			delete (OdJoint *)sipCpp;

			Py_END_ALLOW_THREADS
		}

		return NULL;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyMethodDef lazyMethods_OdJoint[] = {
	{sipName_acc, sipDo_OdJoint_acc, METH_VARARGS, NULL},
	{sipName_disp, sipDo_OdJoint_disp, METH_VARARGS, NULL},
	{sipName_fixed, sipDo_OdJoint_fixed, METH_VARARGS, NULL},
	{sipName_free, sipDo_OdJoint_free, METH_VARARGS, NULL},
	{sipName_info, sipDo_OdJoint_info, METH_VARARGS, NULL},
	{sipName_revolute, sipDo_OdJoint_revolute, METH_VARARGS, NULL},
	{sipName_set_expression, sipDo_OdJoint_set_expression, METH_VARARGS, NULL},
	{sipName_set_imarker, sipDo_OdJoint_set_imarker, METH_VARARGS, NULL},
	{sipName_set_jmarker, sipDo_OdJoint_set_jmarker, METH_VARARGS, NULL},
	{sipName_spherical, sipDo_OdJoint_spherical, METH_VARARGS, NULL},
	{sipName_translational, sipDo_OdJoint_translational, METH_VARARGS, NULL},
	{sipName_txyrz, sipDo_OdJoint_txyrz, METH_VARARGS, NULL},
	{sipName_universal, sipDo_OdJoint_universal, METH_VARARGS, NULL},
	{sipName_vel, sipDo_OdJoint_vel, METH_VARARGS, NULL}
};

sipLazyAttrDef sipClassAttrTab_OdJoint = {
	0, NULL,
	sizeof (lazyMethods_OdJoint) / sizeof (PyMethodDef), lazyMethods_OdJoint,
	0, NULL
};

OdJoint *sipForceConvertTo_OdJoint(PyObject *valobj,int *iserrp)
{
	if (*iserrp || valobj == NULL)
		return NULL;

	if (valobj == Py_None || sipIsSubClassInstance(valobj,sipClass_OdJoint))
		return (OdJoint *)sipConvertToCpp(valobj,sipClass_OdJoint,iserrp);

	sipBadClass(sipName_OdJoint);

	*iserrp = 1;

	return NULL;
}
