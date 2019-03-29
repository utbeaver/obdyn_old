#include "mat33.h"


void Mat33::update(double *q) {
  double c1, c2, c3, s1, s2, s3, c, s;
  c = cos(*q);
  s = sin(*q);
  switch (mtype) {
  case ZAXIS:
    x[0] = c;   y[0] = -s;    z[0]=0.0; 
    x[1] = s;   y[1] = c;     z[1]=0.0; 
   x[2] = 0.0;   y[2] = 0.0;  z[2]=1.0; 
    break;
  case YAXIS:
    x[0] = c;     y[1]=0.0;   z[0] = s; 
    x[1] = 0.0;   y[1]=1.0;   z[1] = 0.0; 
    x[2] = -s;    y[2]=0.0;   z[2] = c; 
    break;
  case XAXIS:
    x[0]=1.0; y[0] =0.0;      z[0]=0.0;
    x[1]=0.0;  y[1] = c;       z[1] = -s; 
    x[2]=0.0;  y[2] = s;       z[2] = c; 
    break;
  case BRYANT:
    c1 = c; c2 = cos(q[1]); c3 = cos(q[2]);
    s1 = s; s2 = sin(q[1]); s3 = sin(q[2]);
    x[0]=c2*c3;             y[0]=-c2*s3;            z[0]=s2;
    x[1]=s3*c1+s3*c2*s1;    y[1]=c3*c1-s3*s2*s1;    z[1]=-c2*s1;
    x[2]=s3*s1-c3*c1*s2;    y[2]=c3*s1+s3*s2*c1;    z[2]=c2*c1;
    break;
  case EULER:
    c3 = c; c2 = cos(q[1]); c1 = cos(q[2]);
    s3 = s; s2 = sin(q[1]); s1 = sin(q[2]);
    x[0]=c3*c1-s3*c2*s1;    y[0]=-c3*s1-s3*c2*c1;   z[0]=s3*s2;
    x[1]=s3*c1+c3*c2*s1;    y[1]=-s3*s1+c3*c2*c1;   z[1]=-c3*s2;
    x[2]=s2*s1;             y[2]=s2*c1;             z[2]=c2;
    break;
  default:
	  x[0] = q[0];   y[0] = q[1]; z[0] = q[2];
	  x[1] = q[4];   y[1] = q[5]; z[1] = q[6];
	  x[2] = q[8];   y[2] = q[9]; z[2] = q[10];
    break;
  }
}




Mat33& Mat33::operator*(Mat33& m2) 
{ 
  double *pf1, *pf2, *pf3;
  double _x[3], _y[3], _z[3];
  pf1 = m2.x; pf2 = pf1+1; pf3 = pf2+1; 
  *(_x)   = *(x)  * *(pf1) + *(y)  * *(pf2) + *(z)  * *(pf3);
  *(_x+1) = *(x+1)* *(pf1) + *(y+1)* *(pf2) + *(z+1)* *(pf3);
  *(_x+2) = *(x+2)* *(pf1) + *(y+2)* *(pf2) + *(z+2)* *(pf3);
  *pf1 = *(_x); *pf2=*(_x+1);*pf3=*(_x+2);
  pf1 = m2.y; pf2 = pf1+1; pf3 = pf2+1; 
  *(_y)   = *(x)  * *(pf1) + *(y)  * *(pf2) + *(z)  * *(pf3);
  *(_y+1) = *(x+1)* *(pf1) + *(y+1)* *(pf2) + *(z+1)* *(pf3);
  *(_y+2) = *(x+2)* *(pf1) + *(y+2)* *(pf2) + *(z+2)* *(pf3);
  *pf1 = *(_y); *pf2=*(_y+1);*pf3=*(_y+2);
  pf1 = m2.z; pf2 = pf1+1; pf3 = pf2+1; 
  *(_z)   = *(x)  * *(pf1) + *(y)  * *(pf2) + *(z)  * *(pf3);
  *(_z+1) = *(x+1)* *(pf1) + *(y+1)* *(pf2) + *(z+1)* *(pf3);
  *(_z+2) = *(x+2)* *(pf1) + *(y+2)* *(pf2) + *(z+2)* *(pf3);
  *pf1 = *(_z); *pf2=*(_z+1);*pf3=*(_z+2);
  return m2;
}

Mat33& Mat33::operator^(Mat33& m2){
  double *pf1, *pf2, *pf3;
  double _x[3], _y[3], _z[3];
  pf1 = m2.x; pf2 = pf1+1; pf3 = pf2+1; 
  *(_x)   = *(x)* *(pf1) + *(x+1)* *(pf2) + *(x+2)* *(pf3);
  *(_x+1) = *(y)* *(pf1) + *(y+1)* *(pf2) + *(y+2)* *(pf3);
  *(_x+2) = *(z)* *(pf1) + *(z+1)* *(pf2) + *(z+2)* *(pf3);
  *pf1 = *(_x); *pf2=*(_x+1);*pf3=*(_x+2);
  pf1 = m2.y; pf2 = pf1+1; pf3 = pf2+1; 
  *(_y)   = *(x)* *(pf1) + *(x+1)* *(pf2) + *(x+2)* *(pf3);
  *(_y+1) = *(y)* *(pf1) + *(y+1)* *(pf2) + *(y+2)* *(pf3);
  *(_y+2) = *(z)* *(pf1) + *(z+1)* *(pf2) + *(z+2)* *(pf3);
  *pf1 = *(_y); *pf2=*(_y+1);*pf3=*(_y+2);
  pf1 = m2.z; pf2 = pf1+1; pf3 = pf2+1; 
  *(_z)   = *(x)* *(pf1) + *(x+1)* *(pf2) + *(x+2)* *(pf3);
  *(_z+1) = *(y)* *(pf1) + *(y+1)* *(pf2) + *(y+2)* *(pf3);
  *(_z+2) = *(z)* *(pf1) + *(z+1)* *(pf2) + *(z+2)* *(pf3);
  *pf1 = *(_z); *pf2=*(_z+1);*pf3=*(_z+2);
  return m2;
}

void Mat33::transpose(Mat33& m1) {
  m1.x[0]=x[0]; m1.y[0]=x[1]; m1.z[0]=x[2]; 
  m1.x[1]=y[0]; m1.y[1]=y[1]; m1.z[1]=y[2]; 
  m1.x[2]=z[0]; m1.y[2]=z[1]; m1.z[2]=z[2]; 
}



Mat33& Mat33::TMTt(Mat33& T) {
  //I am M!!!!
  Mat33 T_temp;
  /*T_temp = */T.transpose(T_temp);
  T_temp = (*this)*T_temp;
  T_temp = T*T_temp;
  T = T_temp;
  return T;
}



void cross_product_with_doubles(const double* const v, const double* const vec, Vec3& temp) {
  if (!v || !vec) {
    temp.v[0]=0.0; temp.v[1]=0.0; temp.v[2]=0.0;
    //  temp.not_zero=0;
  } else {
    temp.v[0] = v[1]*vec[2]-v[2]*vec[1];
    temp.v[1] = v[2]*vec[0]-v[0]*vec[2];
    temp.v[2] = v[0]*vec[1]-v[1]*vec[0];
  }
}

void cross_product( Vec3& v,  Vec3& vec, Vec3& temp) {
  
  temp.v[0] = v.v[1]*vec.v[2]-v.v[2]*vec.v[1];
  temp.v[1] = v.v[2]*vec.v[0]-v.v[0]*vec.v[2];
  temp.v[2] = v.v[0]*vec.v[1]-v.v[1]*vec.v[0];
  
}

void cross_product_with_double(Vec3& v, const double* const vec, Vec3& temp) {
  
  double* vv = v.v;
  temp.v[0] = vv[1]*vec[2]-vv[2]*vec[1];
  temp.v[1] = vv[2]*vec[0]-vv[0]*vec[2];
  temp.v[2] = vv[0]*vec[1]-vv[1]*vec[0];
  
}

/*void cross_product_all_doubles(const double* const v1, const double* const v2,  double*  v3) {
  v3[0] = v1[1]*v2[2]-v1[2]*v2[1];
  v3[1] = v1[2]*v2[0]-v1[0]*v2[2];
  v3[2] = v1[0]*v2[1]-v1[1]*v2[0];
}*/

void mat_vec(Mat33&  m, Vec3&  _v, Vec3& out) {
  double* x = m.x;
  double* y = m.y;
  double* z = m.z;
  double* v = out.v;
  double* v1= _v.v;; 
  double* v2= v1+1; 
  double* v3= v1+2;
  *v    = *x    * *v1 + *(y  )* *v2+*(z  )* *v3;
  *(v+1)= *(x+1)* *v1 + *(y+1)* *v2+*(z+1)* *v3;
  *(v+2)= *(x+2)* *v1 + *(y+2)* *v2+*(z+2)* *v3;
  
}

void mat_mat3(Mat33&  f, Mat33&  m2, Mat33& o) {
  double *pf1, *pf2, *pf3;
  double* x= f.x;
  double* y= f.y;
  double* z= f.z;
  double* _x= o.x;
  double* _y= o.y;
  double* _z= o.z;
  pf1 = m2.x; pf2 = pf1+1; pf3 = pf2+1; 
  *(_x)   = *(x)  * *(pf1) + *(y)  * *(pf2) + *(z)  * *(pf3);
  *(_x+1) = *(x+1)* *(pf1) + *(y+1)* *(pf2) + *(z+1)* *(pf3);
  *(_x+2) = *(x+2)* *(pf1) + *(y+2)* *(pf2) + *(z+2)* *(pf3);
  pf1 = m2.y; pf2 = pf1+1; pf3 = pf2+1; 
  *(_y)   = *(x)  * *(pf1) + *(y)  * *(pf2) + *(z)  * *(pf3);
  *(_y+1) = *(x+1)* *(pf1) + *(y+1)* *(pf2) + *(z+1)* *(pf3);
  *(_y+2) = *(x+2)* *(pf1) + *(y+2)* *(pf2) + *(z+2)* *(pf3);
  pf1 = m2.z; pf2 = pf1+1; pf3 = pf2+1; 
  *(_z)   = *(x)  * *(pf1) + *(y)  * *(pf2) + *(z)  * *(pf3);
  *(_z+1) = *(x+1)* *(pf1) + *(y+1)* *(pf2) + *(z+1)* *(pf3);
  *(_z+2) = *(x+2)* *(pf1) + *(y+2)* *(pf2) + *(z+2)* *(pf3);
}

void matXvec(double **I1, double *I2, double *Out, int m, int n) {
  for(int i=0; i<m; i++) {
    Out[i] = 0.0;
    for(int k=0; k<n; k++) 
      Out[i] += I1[i][k]*I2[k];
  }
}


void matTXvec(double **I1, double *I2, double *Out, int m, int n) {
  for(int i=0; i<n; i++) { 
    Out[i] = 0.0;
    for(int k=0; k<m; k++) 
      Out[i] += I1[k][i]*I2[k];
  }
}

void matXmatT(double **I1, double **I2, double **Out, int m, int n, int p) {
  for(int i=0; i<m; i++) 
    for(int j=0; j<n; j++) {
      Out[i][j] = 0.0;
      for(int k=0; k<p; k++) 
        Out[i][j] += I1[i][k]*I2[j][k];
    }
}

void mat_double(Mat33& m, double* pd, Vec3& out) {
  double* x = m.x;
  double* y = m.y;
  double* z = m.z;
  double* v = out.v;
  double v1= *pd; 
  double v2= *(pd+1); 
  double v3= *(pd+2);
  *v    = *x    * v1 + *(y  )* v2+*(z  )* v3;
  *(v+1)= *(x+1)* v1 + *(y+1)* v2+*(z+1)* v3;
  *(v+2)= *(x+2)* v1 + *(y+2)* v2+*(z+2)* v3;
}

void mat_doubles(Mat33& m, double* pd, double *out) {
  double* x = m.x;
  double* y = m.y;
  double* z = m.z;
  double v1= *pd; 
  double v2= *(pd+1); 
  double v3= *(pd+2);
  *out    = *x    * v1 + *(y  )* v2+*(z  )* v3;
  *(out+1)= *(x+1)* v1 + *(y+1)* v2+*(z+1)* v3;
  *(out+2)= *(x+2)* v1 + *(y+2)* v2+*(z+2)* v3;
}






