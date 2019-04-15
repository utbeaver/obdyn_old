
#ifndef OD_MARKER_API_H
#define OD_MARKER_API_H

#ifdef _WINDOWS
#define LINKELEMENTSDLL __declspec(dllexport)
#else
#define LINKELEMENTSDLL
#endif

class OdBody;
class od_marker;

class V3 {
private:
	double v[3];
	double *p;
public:
	V3() { for (int i=0; i < 3; i++) v[i] = 0.0; p = v; }
	V3(double x, double y, double z) { v[0] = x; v[1] = y; v[2] = z; p = v; }
	int size() { return 3; }
	double get(int i) { return v[i];}
	double* ptr() { return p; }
	double x() { return v[0]; }
	double y() { return v[1]; }
	double z() { return v[2]; }
};

class V6 {
private:
	double v[6];
	double *p;
public:
	V6() { for (int i = 0; i < 6; i++) v[i] = 0.0; p = v; }
	V6(double x, double y, double z, double xy, double xz, double yz) {
		v[0] = x; v[1] = y; v[2] = z; v[3] = xy; v[4] = xz; v[5] = yz; p = v;
	}
	void set(double* val, int n=6) { for (int i = 0; i < n; i++) v[i] = val[i]; }
	double get(int i) { return v[i]; }
	double* ptr() { return p; }
};
class V9 {
private:
	double v[9];
	double *p;
public:
	V9() { for (int i = 0; i < 9; i++) v[i] = 0.0; p = v; }
	void set(double* val, int n=9) { for (int i = 0; i < n; i++) v[i] = val[i]; }
	void set(int i, double vv) { v[i] = vv; }
	double get(int i) { return v[i]; }
	double* ptr() { return p; }
};

class V16 {
public:
	double v[16];
	V16() { for (int i = 0; i < 16; i++) v[i] = 0.0; v[0] = v[5] = v[10] = v[15] = 1.0; }
	void set(int i, double vv) { v[i] = vv; }
	double* ptr() { return &v[0]; }
};

class LINKELEMENTSDLL OdMarker {
private:
	od_marker *pM;
	char* _name[80];
public:
	OdMarker(int id, char* name_ = 0);
	OdMarker(int id, OdBody* pB, char *name_);
	OdMarker(int id, double*, double*, char* name_ );
	OdMarker(int id, double*, char* name_ = 0);
	OdMarker(int id, V3* P, V3* Q, char* name_);
	OdMarker(int id, V16* P, char* name_);
	OdMarker(int id, OdMarker* ref, V3* P, V3* ang, char* name_);
	~OdMarker();
	od_marker* core();
	char* info(char*);
	//void set_position(double* pos);
	//void set_angles(double* ang);
	double* orientation(int pva);
	double* position(int pva);
	V3 P() { double* p = position(0); return V3(p[0], p[1], p[2]); }
	V3 V() {
		double* p = position(1);  return V3(p[0], p[1], p[2]);
	}
	V3 A() {
		double* p = position(2);  return V3(p[0], p[1], p[2]);
	}
	V9 Q() { double* p = orientation(0); V9 v; v.set(p); return v; }
	V3 W() {
		double* p = orientation(1); return V3(p[0], p[1], p[2]);
	}
	V3 DW() {
		double* p = orientation(2);  return V3(p[0], p[1], p[2]);
	}
	//void name(char *name_);
	//void set_omega(double*, int*);
	//void set_velocity(double*, int*);
};
#endif
