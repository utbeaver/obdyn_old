#ifndef OD_CONSTRAINT_API_H
#define OD_CONSTRAINT_API_H

#include "expr.h"
class OdSystem;

#ifdef _WINDOWS
  #define LINKELEMENTSDLL  __declspec(dllexport)
#else
  #define LINKELEMENTSDLL
#endif

class LINKELEMENTSDLL OdExpression {
 private:
  Expression *pExpr;
 public:
  OdExpression(char*, OdSystem*);
  ~OdExpression() {if (pExpr) delete pExpr;}
  Expression* core() {return pExpr;}
};
#endif
