// C++ interface wrapper code.
//
// Generated by SIP 3.10.2 (3.10.2-205) on Wed Mar 27 22:12:12 2019

#include "sipodsystemDeclodsystem.h"
#include "sipodsystemOdForce.h"


PyObject *sipClass_OdForce;

static void sipDealloc_OdForce(sipThisType *);

static PyTypeObject sipType_OdForce = {
	PyObject_HEAD_INIT(&PyType_Type)
	0,
	sipWrapperTypeName,
	sizeof (sipThisType),
	0,
	(destructor)sipDealloc_OdForce,
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

static PyObject *sipDo_OdForce_info(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 813 "python\od_system.sip"
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
#line 55 "sipodsystemOdForce.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdForce,sipName_info);

	return NULL;
}

static PyObject *sipDo_OdForce_set_imarker(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 827 "python\od_system.sip"
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
#line 87 "sipodsystemOdForce.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdForce,sipName_set_imarker);

	return NULL;
}

static PyObject *sipDo_OdForce_set_jmarker(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 846 "python\od_system.sip"
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
#line 119 "sipodsystemOdForce.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdForce,sipName_set_jmarker);

	return NULL;
}

static PyObject *sipDo_OdForce_set_damping(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 865 "python\od_system.sip"
     float val=0.0;
     std::string msg="";
     OdForce *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdForce, &pF, &val))  {
	     pF->set_damping((float)val);
	     msg += "damping is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
#line 143 "sipodsystemOdForce.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdForce,sipName_set_damping);

	return NULL;
}

static PyObject *sipDo_OdForce_set_stiffness(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 876 "python\od_system.sip"
     float val=0.0;
     std::string msg="";
     OdForce *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdForce, &pF, &val))  {
	     pF->set_stiffness((float)val);
	     msg += "stiffness is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
#line 167 "sipodsystemOdForce.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdForce,sipName_set_stiffness);

	return NULL;
}

static PyObject *sipDo_OdForce_set_distance(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 887 "python\od_system.sip"
     float val=0.0;
     std::string msg="";
     OdForce *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdForce, &pF, &val))  {
	     pF->set_distance((float)val);
	     msg += "distance is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
#line 191 "sipodsystemOdForce.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdForce,sipName_set_distance);

	return NULL;
}

static PyObject *sipDo_OdForce_set_ixx(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 899 "python\od_system.sip"
     float val=0.0;
     std::string msg="";
     OdForce *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdForce, &pF, &val))  {
	     pF->set_ixx((float)val);
	     msg += "distance is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
#line 215 "sipodsystemOdForce.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdForce,sipName_set_ixx);

	return NULL;
}

static PyObject *sipDo_OdForce_set_iyy(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 911 "python\od_system.sip"
     float val=0.0;
     std::string msg="";
     OdForce *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdForce, &pF, &val))  {
	     pF->set_iyy((float)val);
	     msg += "distance is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
#line 239 "sipodsystemOdForce.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdForce,sipName_set_iyy);

	return NULL;
}

static PyObject *sipDo_OdForce_set_izz(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 923 "python\od_system.sip"
     float val=0.0;
     std::string msg="";
     OdForce *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdForce, &pF, &val))  {
	     pF->set_izz((float)val);
	     msg += "distance is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
#line 263 "sipodsystemOdForce.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdForce,sipName_set_izz);

	return NULL;
}

static PyObject *sipDo_OdForce_set_area(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 935 "python\od_system.sip"
     float val=0.0;
     std::string msg="";
     OdForce *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdForce, &pF, &val))  {
	     pF->set_area((float)val);
	     msg += "distance is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
#line 287 "sipodsystemOdForce.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdForce,sipName_set_area);

	return NULL;
}

static PyObject *sipDo_OdForce_set_asy(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 947 "python\od_system.sip"
     float val=0.0;
     std::string msg="";
     OdForce *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdForce, &pF, &val))  {
	     pF->set_asy((float)val);
	     msg += "distance is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
#line 311 "sipodsystemOdForce.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdForce,sipName_set_asy);

	return NULL;
}

static PyObject *sipDo_OdForce_set_asz(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 959 "python\od_system.sip"
     float val=0.0;
     std::string msg="";
     OdForce *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdForce, &pF, &val))  {
	     pF->set_asz((float)val);
	     msg += "distance is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
#line 335 "sipodsystemOdForce.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdForce,sipName_set_asz);

	return NULL;
}

static PyObject *sipDo_OdForce_set_ratio(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 971 "python\od_system.sip"
     float val=0.0;
     std::string msg="";
     OdForce *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdForce, &pF, &val))  {
	     pF->set_ratio((float)val);
	     msg += "distance is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
#line 359 "sipodsystemOdForce.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdForce,sipName_set_ratio);

	return NULL;
}

static PyObject *sipDo_OdForce_set_emodulus(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 983 "python\od_system.sip"
     float val=0.0;
     std::string msg="";
     OdForce *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdForce, &pF, &val))  {
	     pF->set_emodulus((float)val);
	     msg += "distance is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
#line 383 "sipodsystemOdForce.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdForce,sipName_set_emodulus);

	return NULL;
}

static PyObject *sipDo_OdForce_set_Kt(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 995 "python\od_system.sip"
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
#line 416 "sipodsystemOdForce.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdForce,sipName_set_Kt);

	return NULL;
}

static PyObject *sipDo_OdForce_set_Kr(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1015 "python\od_system.sip"
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
#line 449 "sipodsystemOdForce.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdForce,sipName_set_Kr);

	return NULL;
}

static PyObject *sipDo_OdForce_set_Ct(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1035 "python\od_system.sip"
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
#line 482 "sipodsystemOdForce.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdForce,sipName_set_Ct);

	return NULL;
}

static PyObject *sipDo_OdForce_set_Cr(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1055 "python\od_system.sip"
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
#line 515 "sipodsystemOdForce.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdForce,sipName_set_Cr);

	return NULL;
}

static PyObject *sipDo_OdForce_set_F(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1075 "python\od_system.sip"
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
#line 548 "sipodsystemOdForce.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdForce,sipName_set_F);

	return NULL;
}

static PyObject *sipDo_OdForce_set_T(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1095 "python\od_system.sip"
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
#line 581 "sipodsystemOdForce.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdForce,sipName_set_T);

	return NULL;
}

static PyObject *sipDo_OdForce_set_gmodulus(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1115 "python\od_system.sip"
     float val=0.0;
     std::string msg="";
     OdForce *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdForce, &pF, &val))  {
	     pF->set_gmodulus((float)val);
	     msg += "distance is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
#line 605 "sipodsystemOdForce.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdForce,sipName_set_gmodulus);

	return NULL;
}

static PyObject *sipDo_OdForce_set_force(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1127 "python\od_system.sip"
     float val=0.0;
     std::string msg="";
     OdForce *pF;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "mf", sipThisObj, sipClass_OdForce, &pF, &val))  {
	     pF->set_force((float)val);
	     msg += "force is set!";
     }  
     return  Py_BuildValue("s", msg.c_str());
#line 629 "sipodsystemOdForce.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdForce,sipName_set_force);

	return NULL;
}

static PyObject *sipDo_OdForce_set_expression(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1138 "python\od_system.sip"
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
#line 656 "sipodsystemOdForce.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdForce,sipName_set_expression);

	return NULL;
}

static PyObject *sipDo_OdForce_sforce(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1152 "python\od_system.sip"
     OdForce *pC;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdForce, &pC))  
     {
	     if(pC) pC->sforce();
     }  
     return  Py_BuildValue("s", "Sforce");
#line 678 "sipodsystemOdForce.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdForce,sipName_sforce);

	return NULL;
}

static PyObject *sipDo_OdForce_storque(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1161 "python\od_system.sip"
     OdForce *pC;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdForce, &pC))  
     {
	     if(pC) pC->storque();
     }  
     return  Py_BuildValue("s", "Storque");
#line 700 "sipodsystemOdForce.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdForce,sipName_storque);

	return NULL;
}

static PyObject *sipDo_OdForce_spdpt(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1170 "python\od_system.sip"
     OdForce *pC;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdForce, &pC))  
     {
	     if(pC) pC->spdpt();
     }  
     return  Py_BuildValue("s", "Translational Spring-Damper");
#line 722 "sipodsystemOdForce.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdForce,sipName_spdpt);

	return NULL;
}

static PyObject *sipDo_OdForce_beam(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1180 "python\od_system.sip"
     OdForce *pC;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdForce, &pC))  
     {
	     if(pC) pC->beam();
     }  
     return  Py_BuildValue("s", "Beam");
#line 744 "sipodsystemOdForce.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdForce,sipName_beam);

	return NULL;
}

static PyObject *sipDo_OdForce_bushing(PyObject *sipThisObj,PyObject *sipArgs)
{
	int sipArgsParsed = 0;

	{
#line 1189 "python\od_system.sip"
     OdForce *pC;
     if (sipParseArgs(&sipArgsParsed,sipArgs,  "m", sipThisObj, sipClass_OdForce, &pC))  
     {
	     if(pC) pC->bushing();
     }  
     return  Py_BuildValue("s", "Bushing");
#line 766 "sipodsystemOdForce.cxx"
	}

	// Report an error if the arguments couldn't be parsed.

	sipNoMethod(sipArgsParsed,sipName_OdForce,sipName_bushing);

	return NULL;
}

// Cast a pointer to a type somewhere in its superclass hierarchy.

const void *sipCast_OdForce(const void *ptr,PyObject *targetClass)
{
	if (targetClass == sipClass_OdForce)
		return ptr;

	return NULL;
}

static void sipDealloc_OdForce(sipThisType *sipThis)
{
	if (sipThis -> u.cppPtr != NULL)
	{

		if (sipIsPyOwned(sipThis))
		{
			Py_BEGIN_ALLOW_THREADS

			delete (OdForce *)sipThis -> u.cppPtr;

			Py_END_ALLOW_THREADS
		}
	}

	sipDeleteThis(sipThis);
}

PyObject *sipNew_OdForce(PyObject *sipSelf,PyObject *sipArgs)
{
	static sipExtraType et = {
		sipCast_OdForce
	};

	int sipFlags = SIP_PY_OWNED;
	int sipArgsParsed = 0;
	sipThisType *sipThis = NULL;
	OdForce *sipCpp = NULL;

	// See if there is something pending.

	sipCpp = (OdForce *)sipGetPending(&sipFlags);

	if (sipCpp == NULL)
	{
#line 802 "python\od_system.sip"
     int id;
     PyObject *pystr;
     if (sipParseArgs(&sipArgsParsed,sipArgs, "iO", &id, &pystr)) {
        char* pstr=PyString_AsString(pystr);
        Py_BEGIN_ALLOW_THREADS
        sipCpp = new OdForce(id, pstr);
        Py_END_ALLOW_THREADS
     }  
#line 830 "sipodsystemOdForce.cxx"
	}

	if (sipCpp == NULL)
	{
		OdForce * a0;

		if (sipParseArgs(&sipArgsParsed,sipArgs,"J1",sipClass_OdForce,&a0))
		{
			Py_BEGIN_ALLOW_THREADS
			sipCpp = new OdForce(*a0);
			Py_END_ALLOW_THREADS
		}
	}

	if (sipCpp == NULL)
	{
		sipNoCtor(sipArgsParsed,sipName_OdForce);
		return NULL;
	}

	// Wrap the object.

	if ((sipThis = sipCreateThis(sipSelf,sipCpp,&sipType_OdForce,sipFlags,&et)) == NULL)
	{
		if (sipFlags & SIP_PY_OWNED)
		{
			Py_BEGIN_ALLOW_THREADS

			delete (OdForce *)sipCpp;

			Py_END_ALLOW_THREADS
		}

		return NULL;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyMethodDef lazyMethods_OdForce[] = {
	{sipName_beam, sipDo_OdForce_beam, METH_VARARGS, NULL},
	{sipName_bushing, sipDo_OdForce_bushing, METH_VARARGS, NULL},
	{sipName_info, sipDo_OdForce_info, METH_VARARGS, NULL},
	{sipName_set_Cr, sipDo_OdForce_set_Cr, METH_VARARGS, NULL},
	{sipName_set_Ct, sipDo_OdForce_set_Ct, METH_VARARGS, NULL},
	{sipName_set_F, sipDo_OdForce_set_F, METH_VARARGS, NULL},
	{sipName_set_Kr, sipDo_OdForce_set_Kr, METH_VARARGS, NULL},
	{sipName_set_Kt, sipDo_OdForce_set_Kt, METH_VARARGS, NULL},
	{sipName_set_T, sipDo_OdForce_set_T, METH_VARARGS, NULL},
	{sipName_set_area, sipDo_OdForce_set_area, METH_VARARGS, NULL},
	{sipName_set_asy, sipDo_OdForce_set_asy, METH_VARARGS, NULL},
	{sipName_set_asz, sipDo_OdForce_set_asz, METH_VARARGS, NULL},
	{sipName_set_damping, sipDo_OdForce_set_damping, METH_VARARGS, NULL},
	{sipName_set_distance, sipDo_OdForce_set_distance, METH_VARARGS, NULL},
	{sipName_set_emodulus, sipDo_OdForce_set_emodulus, METH_VARARGS, NULL},
	{sipName_set_expression, sipDo_OdForce_set_expression, METH_VARARGS, NULL},
	{sipName_set_force, sipDo_OdForce_set_force, METH_VARARGS, NULL},
	{sipName_set_gmodulus, sipDo_OdForce_set_gmodulus, METH_VARARGS, NULL},
	{sipName_set_imarker, sipDo_OdForce_set_imarker, METH_VARARGS, NULL},
	{sipName_set_ixx, sipDo_OdForce_set_ixx, METH_VARARGS, NULL},
	{sipName_set_iyy, sipDo_OdForce_set_iyy, METH_VARARGS, NULL},
	{sipName_set_izz, sipDo_OdForce_set_izz, METH_VARARGS, NULL},
	{sipName_set_jmarker, sipDo_OdForce_set_jmarker, METH_VARARGS, NULL},
	{sipName_set_ratio, sipDo_OdForce_set_ratio, METH_VARARGS, NULL},
	{sipName_set_stiffness, sipDo_OdForce_set_stiffness, METH_VARARGS, NULL},
	{sipName_sforce, sipDo_OdForce_sforce, METH_VARARGS, NULL},
	{sipName_spdpt, sipDo_OdForce_spdpt, METH_VARARGS, NULL},
	{sipName_storque, sipDo_OdForce_storque, METH_VARARGS, NULL}
};

sipLazyAttrDef sipClassAttrTab_OdForce = {
	0, NULL,
	sizeof (lazyMethods_OdForce) / sizeof (PyMethodDef), lazyMethods_OdForce,
	0, NULL
};

OdForce *sipForceConvertTo_OdForce(PyObject *valobj,int *iserrp)
{
	if (*iserrp || valobj == NULL)
		return NULL;

	if (valobj == Py_None || sipIsSubClassInstance(valobj,sipClass_OdForce))
		return (OdForce *)sipConvertToCpp(valobj,sipClass_OdForce,iserrp);

	sipBadClass(sipName_OdForce);

	*iserrp = 1;

	return NULL;
}
