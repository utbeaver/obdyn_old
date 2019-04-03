
%module odsystem 

%{
#include "od_marker_api.h"
#include "od_constraint_api.h"
#include "od_body_api.h"
#include "od_force_api.h"
#include "od_system_api.h"
#include "od_subsystem_api.h"
%}
%ignore OdMarker::OdMarker(int id, OdBody* pB, char *name_);
%ignore OdMarker::OdMarker(int id, double PVA[3], double ANG[3], char* name_);
%ignore OdMarker::OdMarker(int id, double MAT[16], char* name_ );
%ignore OdMarker::orientation(int pva);
%ignore OdMarker::position(int pva);
%ignore OdJoint::explicitFixed(int im);
%ignore OdJoint::explicitFixed();
%ignore OdSystem::setGravity(double*);
%ignore OdSystem::OdSystem(char*,int);
%ignore V3::set(double*, int);
%ignore V6::set(double*, int);
%ignore V9::set(double*, int);
/*
%{
static int convert_darray(PyObject *input, double *temp, int size) {     
  int i;
  if (!PySequence_Check(input)) {
    PyErr_SetString(PyExc_ValueError, "Expected a sequence");
    return NULL;
  }
  if (PySequence_Length(input) != size) {
    PyErr_SetString(PyExc_ValueError, "Size mismatch. Expected $1_dim0 elements");
    return NULL;
  }
  for (i = 0; i < size; i++) {
    PyObject *o_oo = PySequence_GetItem(input, i);
    if (!PyNumber_Check(o_oo)) {
      PyErr_SetString(PyExc_ValueError, "Sequence elements must be numbers");      
      return NULL;
    }
    temp[i] = PyFloat_AsDouble(o_oo);
  }
  return 1;
}
%}
%typemap(in) double PVA[3] (double temp[$1_dim0]) {
    if (convert_darray($input, temp, $1_dim0)==NULL) {
        return NULL;
    }
    $1=temp;
}
%typemap(in) double ANG[3] (double temp[$1_dim0]) {
    if (convert_darray($input, temp, $1_dim0)==NULL) {
        return NULL;
    }
    $1=temp;
}
%typemap(in) double MAT[16] (double temp[$1_dim0]) {
    if (convert_darray($input, temp, $1_dim0)==NULL) {
        return NULL;
    }
    $1=temp;
}
*/

%typemap(argout) double DOFPVA[6] {
    PyObject *o_o=PyList_New(6);
    for(int i=0; i<6; i++) {
        PyList_SetItem(o_o, i, PyFloat_FromDouble($1[i]));
    }
    $result=o_o;
}
/*
%typemap(argout) double PVA_[6] {
    PyObject *o_o=PyList_New(6);
    for(int i=0; i<6; i++) {
        PyList_SetItem(o_o, i, PyFloat_FromDouble($1[i]));
    }
    $result=o_o;
}
*/



%include "od_marker_api.h"
%include "od_body_api.h"
%include "od_system_api.h"
%include "od_constraint_api.h"
%include "od_force_api.h"
