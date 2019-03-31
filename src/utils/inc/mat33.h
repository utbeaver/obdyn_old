#ifndef MAT33_H
#define MAT33_H
#include <math.h>
#include <assert.h>
#include <string.h>
#include "od_element.h"

#define SMALL_VALUE 1.0e-12
#define SMALL_VALUE1 1.0e-10
#define EQ3(a,b)  (a)[0]=(b)[0];(a)[1]=(b)[1];(a)[2]=(b)[2];
#define U_ADD3(a, b) (a)[0]+=(b)[0];(a)[1]+=(b)[1];(a)[2]+=(b)[2]; 
#define U_SUB3(a, b) (a)[0]-=(b)[0];(a)[1]-=(b)[1];(a)[2]-=(b)[2]; 
#define DOT_X(a, b) ((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define ADD3(a, b, c) c[0]=(a)[0]+(b)[0];c[1]=(a)[1]+(b)[1];c[2]=(a)[2]+(b)[2]; 
#define SUB3(a, b, c) c[0]=(a)[0]-(b)[0];c[1]=(a)[1]-(b)[1];c[2]=(a)[2]-(b)[2]; 
#define CROSS_X(a, b, c) c[0]=(a)[1]*(b)[2]-(a)[2]*(b)[1];c[1]=(a)[2]*(b)[0]-(a)[0]*(b)[2];c[2]=(a)[0]*(b)[1]-(a)[1]*(b)[0];
#define ZERO3(a) (a)[0]=(a)[1]=(a)[2]=0.0;
#define MAG(v) sqrt((v)[0]*(v)[0]+(v)[1]*(v)[1]+(v)[2]*(v)[2])
#define NEG(v) v[0]=-v[0];v[1]=-v[1];v[2]=-v[2];
#define MULT_S(a, b) a[0]*=(b);a[1]*=(b);a[2]*=(b);
#define TRI_CROSS_X(a, b, c, d) {double _x1=DOT_X(a,c); double _x2=DOT_X(a,b); d[0]=_x1*b[0]-_x2*c[0];d[1]=_x1*b[1]-_x2*c[1];d[2]=_x1*b[2]-_x2*c[2];}
#define TRI_CROSS_XN(a, b, c, d) {double _x1=DOT_X(b,c); double _x2=DOT_X(a,c); d[0]=-_x1*a[0]+_x2*b[0];d[1]=-_x1*a[1]+_x2*b[1];d[2]=-_x1*a[2]+_x2*b[2];}
class Vec3;
class Mat33;

extern "C" void cross_product_with_doubles(const double* const f, const double* const s, Vec3& vec);
extern "C" void cross_product(Vec3& f, Vec3& s, Vec3& product);
extern "C" void cross_product_with_double(Vec3& f, const double* const pd, Vec3& product);

extern "C" void mat_vec(Mat33&  m, Vec3&  v, Vec3& out);
extern "C" void mat_mat3(Mat33& f, Mat33& s, Mat33& out);
extern "C" void mat_double(Mat33&  m, double*  const pd, Vec3& out);
extern "C" void mat_doubles(Mat33&  m, double*  const pd, double*);
extern "C" void matXvec(double **I1, double *I2, double *Out, int m, int n);
extern "C" void matTXvec(double **I1, double *I2, double *Out, int m, int n);
extern "C" void matXmatT(double **I1, double **I2, double **Out, int m, int n, int k);

#define LINKUTILSDLL __declspec(dllexport)
class Vec3 {
public:
	double v[3];
	int idx; //debug the position in sparseMat
	Vec3() {v[0] = v[1] = v[2] = 0.0; idx = -1;}
	Vec3(double *pd) { v[0] = pd[0]; v[1] = pd[1]; v[2] = pd[2];}
	inline void set(double a, double b, double c) { v[0] = a; v[1] = b; v[2] = c;}
	inline int numRows() const { return 3; }
	inline int numCols() const { return 1; }
	int const operator!=(const Vec3& another) const {
		Vec3 temp;
		int flag = 1;
		temp = another;
		temp = (*this) ^ (temp);
		return 1;
	}

	const int operator==(Vec3& an) const {
		double a;
		for (int i = 0; i < 3; i++) {
			a = (fabs(v[i]) > 1.0) ? v[i] : 1.0;
			a = (v[i] - an.v[i]) / a;
			if (a > SMALL_VALUE1) return 1;
		}
		return 0;
	}
	
	inline double* VxD(double *V, double d, int reset = 0) const {
		if (reset) ZERO3(V);
		V[0] += v[0] * d;
		V[1] += v[1] * d;
		V[2] += v[2] * d;
		return V;
	}

	/*inline void divided_by(double a) {
		if (fabs(a) > SMALL_VALUE) {
			a = 1.0 / a;
			v[0] *= a; v[1] *= a; v[2] *= a;
		}
	}*/
	inline void multiplied_by(double a) { v[0] *= a; v[1] *= a; v[2] *= a; }
	inline void negate() { NEG(v) }
	//inline  double* v  const { return (double*)v; }
	inline double* to_double(double* temp) const { EQ3(temp, v) return temp; }
	inline  Vec3& operator= (const Vec3& another) { EQ3(v, another.v); return *this; }
	inline Vec3& operator=(const Vec3* another) { if (another) { EQ3(v, another->v) } else { ZERO3(v); } return *this; }
	inline void init() { ZERO3(v); }
	inline double mag() const { return MAG(v); }
	inline Vec3& operator+=(const Vec3& vec) { U_ADD3(v, vec.v) return *this; }
	inline Vec3& operator+=(const Vec3* an) { if (an) U_ADD3(v, an->v) return *this; }
	inline Vec3& operator-=(Vec3& vec) { U_SUB3(v, vec.v) return *this; }
	inline Vec3& operator-=(const Vec3* an) { if (an) U_SUB3(v, an->v) return *this; }
	inline Vec3& operator=(double *pd) { EQ3(v, pd) return *this; }
	inline Vec3& operator+=(double* pd) { U_ADD3(v, pd) return *this; }
	inline Vec3& operator-=(double* pd) { U_SUB3(v, pd) return *this; }
	inline Vec3& operator^(Vec3& vec) const {
		double temp[3];
		CROSS_X(v, vec.v, temp);
		EQ3(vec.v, temp);
		return vec;
	}
	inline double* operator^(double* vec) const {
		double temp[3];
		CROSS_X(v, vec, temp);
		EQ3(vec, temp);
		return vec;
	}
	inline double operator*(Vec3& vec) { return DOT_X(v, vec.v); }
	inline double operator*(double* vec) { return  DOT_X(v, vec); }
};
class Mat33/* : public od_object*/ {
public:
	double x[3], y[3], z[3];
	double* v[3];
public:
	enum TYPE { GENERAL, XAXIS, YAXIS, ZAXIS, EULER, BRYANT, EU_PARA, IDENTITY, DIAG };
	TYPE mtype;
	Mat33()/* : od_object()*/ {
		v[0] = x; v[1] = y; v[2] = z;
		ZERO3(x); ZERO3(y); ZERO3(z);
		x[0] = 1.0; y[1] = 1.0; z[2] = 1.0;
		mtype = IDENTITY;
	}

	Mat33(double const val) /*: od_object()*/ {
		v[0] = x; v[1] = y; v[2] = z;
		ZERO3(x) ZERO3(y) ZERO3(z);
		x[0] = val; y[1] = val; z[2] = val;
		mtype = DIAG;
	}

	Mat33(double *q, TYPE type) /*: od_object() */ {
		v[0] = x; v[1] = y; v[2] = z;
		mtype = type;
		ZERO3(x) ZERO3(y) ZERO3(z);
		x[0] = 1.0; y[1] = 1.0; z[2] = 1.0;
		update(q);
	}

	inline void init() {
		ZERO3(x); ZERO3(y); ZERO3(z);
		x[0] = 1.0; y[1] = 1.0; z[2] = 1.0;
	}
	inline double* to_double(double *vals) const {
		vals[0] = x[0];
		vals[1] = y[0];
		vals[2] = z[0];

		vals[3] = x[1];
		vals[4] = y[1];
		vals[5] = z[1];

		vals[6] = x[2];
		vals[7] = y[2];
		vals[8] = z[2];
		return vals;
	}

	Mat33& operator=(Mat33& another) {
		x[0] = another.x[0];
		x[1] = another.x[1];
		x[2] = another.x[2];
		y[0] = another.y[0];
		y[1] = another.y[1];
		y[2] = another.y[2];
		z[0] = another.z[0];
		z[1] = another.z[1];
		z[2] = another.z[2];
		return *this;
	}

	inline void operator=(double *vals) {
		x[0] = vals[0];
		y[0] = vals[1];
		z[0] = vals[2];

		x[1] = vals[3];
		y[1] = vals[4];
		z[1] = vals[5];

		x[2] = vals[6];
		y[2] = vals[7];
		z[2] = vals[8];
		mtype = GENERAL;
	}

	inline void operator=(double const val) {
		x[0] = y[1] = z[2] = val;
		mtype = DIAG;
	}
	inline void multiplied_by(double const val) {
		x[0] *= val;
		x[1] *= val;
		x[2] *= val;
		y[0] *= val;
		y[1] *= val;
		y[2] *= val;
		z[0] *= val;
		z[1] *= val;
		z[2] *= val;
	}
	inline void set_type(TYPE ty) { mtype = ty; }
	inline TYPE get_type() const { return mtype; }

	//inline int numRows() const {return 3;}
	//inline int numCols() const {return 3;}
	inline Vec3& operator*(Vec3& vec) const {
		double _x = vec.v[0];
		double _y = vec.v[1];
		double _z = vec.v[2];
		vec.v[0] = x[0] * _x + y[0] * _y + z[0] * _z;
		vec.v[1] = x[1] * _x + y[1] * _y + z[1] * _z;
		vec.v[2] = x[2] * _x + y[2] * _y + z[2] * _z;
		return vec;
	}
	inline Vec3& operator^(Vec3& vec) const {
		double _x = vec.v[0];
		double _y = vec.v[1];
		double _z = vec.v[2];
		vec.v[0] = x[0] * _x + x[1] * _y + x[2] * _z;
		vec.v[1] = y[0] * _x + y[1] * _y + y[2] * _z;
		vec.v[2] = z[0] * _x + z[1] * _y + z[2] * _z;
		return vec;
	}
	inline double* operator*(double* pvec) const {
		double _x = pvec[0];
		double _y = pvec[1];
		double _z = pvec[2];
		pvec[0] = x[0] * _x + y[0] * _y + z[0] * _z;
		pvec[1] = x[1] * _x + y[1] * _y + z[1] * _z;
		pvec[2] = x[2] * _x + y[2] * _y + z[2] * _z;
		return pvec;
	}
	inline double* operator^(double* vec) const
	{
		double _x = vec[0];
		double _y = vec[1];
		double _z = vec[2];
		vec[0] = x[0] * _x + x[1] * _y + x[2] * _z;
		vec[1] = y[0] * _x + y[1] * _y + y[2] * _z;
		vec[2] = z[0] * _x + z[1] * _y + z[2] * _z;
		return vec;
	}
	double *toBryant(double *p) {
		double sy = sqrt(y[2] * y[2] + z[2] * z[2]);
		p[0] = atan2(x[1], x[0]);
		p[1] = atan2(-x[2], sy);
		p[2] = atan2(y[2], z[2]);
		return p;
	}
	//transposed multiplification
	Mat33& operator^(Mat33&);
	inline Mat33& operator*(Mat33&);
	void transpose(Mat33&);
	void update(double *q);
	Mat33& TMTt(Mat33 &T);
};

class  vector_int {
private:
	int *vals;
	int size1;
public:
	vector_int() { vals = 0; size1 = 0; }
	~vector_int() {
		DELARY(vals)
	}
	inline void resize(int s) {
		if (s > size1) {
			DELARY(vals)
				vals = new int[s];
		}
		size1 = s;
	}
	inline int& operator[](int index) {
		return vals[index];
	}
	inline int size() {
		return size1;
	}
};


#endif
