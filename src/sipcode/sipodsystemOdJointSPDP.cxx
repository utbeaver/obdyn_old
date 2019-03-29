// C++ interface wrapper code.
//
// Generated by SIP 3.10.2 (3.10.2-205) on Wed Mar 27 22:12:12 2019

#include "sipodsystemDeclodsystem.h"
#include "sipodsystemOdJointSPDP.h"


PyObject *sipClass_OdJointSPDP;

static void sipDealloc_OdJointSPDP(sipThisType *);

static PyTypeObject sipType_OdJointSPDP = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	sipWrapperTypeName,
	sizeof (sipThisType),
	0,
	(destructor)sipDealloc_OdJointSPDP,
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

static PyObject *sipDo_OdJointSPDP_set_joint(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 678 "python\od_system.sip"
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
#line 61 "sipodsystemOdJointSPDP.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdJointSPDP,sipName_set_joint);

	return NULL;
}

static PyObject *sipDo_OdJointSPDP_set_damping(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 698 "python\od_system.sip"
     float val=0.0;
     std::string msg="";
     OdJointSPDP *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdJointSPDP, &pF, &val))  {
	     pF->set_damping((float)val);
	     msg += "damping is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
#line 85 "sipodsystemOdJointSPDP.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdJointSPDP,sipName_set_damping);

	return NULL;
}

static PyObject *sipDo_OdJointSPDP_set_stiffness(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 709 "python\od_system.sip"
     float val=0.0;
     std::string msg="";
     OdJointSPDP *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdJointSPDP, &pF, &val))  {
	     pF->set_stiffness((float)val);
	     msg += "stiffness is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
#line 109 "sipodsystemOdJointSPDP.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdJointSPDP,sipName_set_stiffness);

	return NULL;
}

static PyObject *sipDo_OdJointSPDP_set_distance(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 720 "python\od_system.sip"
     float val=0.0;
     std::string msg="";
     OdJointSPDP *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdJointSPDP, &pF, &val))  {
	     pF->set_distance((float)val);
	     msg += "distance is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
#line 133 "sipodsystemOdJointSPDP.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdJointSPDP,sipName_set_distance);

	return NULL;
}

static PyObject *sipDo_OdJointSPDP_set_force(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 731 "python\od_system.sip"
     float val=0.0;
     std::string msg="";
     OdJointSPDP *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdJointSPDP, &pF, &val))  {
	     pF->set_force((float)val);
	     msg += "Force is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
#line 157 "sipodsystemOdJointSPDP.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdJointSPDP,sipName_set_force);

	return NULL;
}

// Cast a pointer to a type somewhere in its superclass hierarchy.

const void *sipCast_OdJointSPDP(const void *ptr,PyObject *targetClass)
{
	if (targetClass == sipClass_OdJointSPDP)
		return ptr;

	return NULL;
}

static void sipDealloc_OdJointSPDP(sipThisType *sipThis)
{
	if (sipThis -> u.cppPtr != NULL)
	{

		if (sipIsPyOwned(sipThis))
		{
			Py_BEGIN_ALLOW_THREADS

			delete (OdJointSPDP *)sipThis -> u.cppPtr;

			Py_END_ALLOW_THREADS
		}
	}

	sipDeleteThis(sipThis);
}

PyObject *sipNew_OdJointSPDP(PyObject *sipSelf,PyObject *sipArgs)
{
	static sipExtraType et = {
		sipCast_OdJointSPDP
	};

	int sipFlags = SIP_PY_OWNED;
	int sipArgsParsed = 0;
	sipThisType *sipThis = NULL;
	OdJointSPDP *sipCpp = NULL;

	// See if there is something pending.

	sipCpp = (OdJointSPDP *)sipGetPending(&sipFlags);

	if (sipCpp == NULL)
	{
#line 666 "python\od_system.sip"
     int id;
     PyObject *pystr;
     if (sipParseArgs(&sipArgsParsed,sipArgs, "iO", &id, &pystr)) {
        char* pstr=PyString_AsString(pystr);
        Py_BEGIN_ALLOW_THREADS
        sipCpp = new OdJointSPDP(id, pstr);
        Py_END_ALLOW_THREADS
     }  
#line 221 "sipodsystemOdJointSPDP.cxx"
	}

	if (sipCpp == NULL)
	{
		OdJointSPDP * a0;

		if (sipParseArgs(&sipArgsParsed,sipArgs,"J1",sipClass_OdJointSPDP,&a0))
		{
			Py_BEGIN_ALLOW_THREADS
			sipCpp = new OdJointSPDP(*a0);
			Py_END_ALLOW_THREADS
		}
	}

	if (sipCpp == NULL)
	{
		sipNoCtor(sipArgsParsed,sipName_OdJointSPDP);
		return NULL;
	}

	// Wrap the object.

	if ((sipThis = sipCreateThis(sipSelf,sipCpp,&sipType_OdJointSPDP,sipFlags,&et)) == NULL)
	{
		if (sipFlags & SIP_PY_OWNED)
		{
			Py_BEGIN_ALLOW_THREADS

			delete (OdJointSPDP *)sipCpp;

			Py_END_ALLOW_THREADS
		}

		return NULL;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyMethodDef lazyMethods_OdJointSPDP[] = {
	{sipName_set_damping, sipDo_OdJointSPDP_set_damping, METH_VARARGS, NULL},
	{sipName_set_distance, sipDo_OdJointSPDP_set_distance, METH_VARARGS, NULL},
	{sipName_set_force, sipDo_OdJointSPDP_set_force, METH_VARARGS, NULL},
	{sipName_set_joint, sipDo_OdJointSPDP_set_joint, METH_VARARGS, NULL},
	{sipName_set_stiffness, sipDo_OdJointSPDP_set_stiffness, METH_VARARGS, NULL}
};

sipLazyAttrDef sipClassAttrTab_OdJointSPDP = {
	0, NULL,
	sizeof (lazyMethods_OdJointSPDP) / sizeof (PyMethodDef), lazyMethods_OdJointSPDP,
	0, NULL
};

OdJointSPDP *sipForceConvertTo_OdJointSPDP(PyObject *valobj,int *iserrp)
{
	if (*iserrp || valobj == NULL)
		return NULL;

	if (valobj == Py_None || sipIsSubClassInstance(valobj,sipClass_OdJointSPDP))
		return (OdJointSPDP *)sipConvertToCpp(valobj,sipClass_OdJointSPDP,iserrp);

	sipBadClass(sipName_OdJointSPDP);

	*iserrp = 1;

	return NULL;
}