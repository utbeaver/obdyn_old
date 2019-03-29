
#ifndef OD_MARKER_API_H
#define OD_MARKER_API_H

#ifdef _WINDOWS
#define LINKELEMENTSDLL __declspec(dllexport)
#else
#define LINKELEMENTSDLL
#endif

class OdBody;
class od_marker;

class LINKELEMENTSDLL OdMarker {
private:
	od_marker *pM;
	char* _name[80];
public:
	OdMarker(int id, char* name_ = 0);
	OdMarker(int id, OdBody* pB, char *name_ = 0);
	OdMarker(int id, double*, double*, char* name_ = 0);
	OdMarker(int id, double*, char* name_ = 0);
	~OdMarker();
	od_marker* core();
	char* info(char*);
	void set_position(double* pos);
	void set_angles(double* ang);
	double* position(double*, int = 0);
	double* orientation(double*, int = 0);
	//void name(char *name_);
	void set_omega(double*, int*);
	void set_velocity(double*, int*);
};
#endif
