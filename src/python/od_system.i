
%module odsystem 

%{
#include "od_marker_api.h"
#include "od_constraint_api.h"
#include "od_body_api.h"
#include "od_force_api.h"
#include "od_system_api.h"
#include "od_subsystem_api.h"
//#include "expr.h"
%}

%include "od_system_api.h"
%include "od_marker_api.h"
%include "od_constraint_api.h"
%include "od_body_api.h"
%include "od_force_api.h"
