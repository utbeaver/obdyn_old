
#ifndef OD_BODY_API_H
#define OD_BODY_API_H
#ifdef _WINDOWS
 #define LINKELEMENTSDLL  __declspec(dllexport)
#else
 #define LINKELEMENTSDLL
#endif
class od_body;
class OdMarker;
class LINKELEMENTSDLL OdBody {
 private:
  od_body *pB;
  char _name[80];
 public:
  OdBody(int id, char* name_=0 );
	~OdBody(); 
  void setMass(double);
  void setI(double*);
  char* info(char* msg); 
  void add_cm_marker(OdMarker *pM);
  void add_marker(OdMarker *pM, int=0);
  od_body* core();
};
#endif