#ifndef OD_LINSOLV_H
#define OD_LINSOLV_H

#include <math.h>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include "od_topo.h"
#include "od_element.h"
using namespace std;
class od_matrix {
public:
	int dim, dim_n, rowwise;
	double** mat;
//	double** _mat;
	int* pVec;
	int _repar;
	int eff_dim;
public:
	od_matrix(int m, int n) {
		dim = m; dim_n = n; rowwise = 1; _repar= 1;
	}
	~od_matrix() { DELARY(pVec); }
	void zeros() {
		if (rowwise == 1)  for (int i = 0; i < dim; i++) fill(mat[i], mat[i] + dim_n, 0.0);
		else for (int i = 0; i < dim_n; i++) fill(mat[i], mat[i] + dim, 0.0);
	}
	virtual void init() = 0;
	double** values() const { return mat; }
	virtual double* solve(double*, int repar = 0, int = 0, int* d = 0) = 0;
	virtual double* solveT(double*, int repar = 0, int = 0, int* d = 0) = 0;
	virtual void solve(od_matrix*, int = 0, int = 0) = 0;
	virtual void solveT(od_matrix*, int = 0, int = 0) = 0;
	virtual int LU() = 0;
	virtual void print_out(int* pVec = 0) = 0;

	virtual void setPvecN(int *pved, int effn) = 0;
	double* row(int i) const { return (rowwise == 1) ? mat[i] : 0; }
	double* col(int i) const { return (rowwise == 1) ? 0 : mat[i]; }
	//virtual void setCol(int i) = 0;
	virtual void MinusCaB(od_matrix* m1, od_matrix* m2) = 0;
	inline int rows() const { return (rowwise == 1) ? dim : dim_n; }
	inline int cols() const { return (rowwise == 1) ? dim_n : dim; }
	void D_LF(od_matrix *L, od_matrix *F, int = 0, int = 0);
	void D_diagLF(od_matrix *L, od_matrix *F, int = 0, int = 0);
	double* CX(double* = 0, double* =0, double = 1.0);
	double* BX(double*, double* = 0, double = 1.0);
	void BX(od_matrix* pA, int basei = 0, int basej = 0);
	double element(int i, int j) const { return  (rowwise == 1) ? mat[i][j] : mat[j][i]; }
	void  element(int i, int j, double val) {
		if (rowwise) mat[i][j] = val; else mat[j][i] = val;
	}
	inline void elementr(int i, int j, double val) { mat[i][j] = val; }
	inline void elementc(int i, int j, double val) { mat[j][i] = val; }
	inline void add(int i, int j, double val) { if (rowwise) mat[i][j] += val; else mat[j][i] += val; }
	inline void addr(int i, int j, double val) { 
		mat[i][j] += val;
	}
	inline void addc(int i, int j, double val) { mat[j][i] += val; }
	void equalsub(int m1, int n1, double** ms, double = 1.0);
	void addsub(int m1, int n1, double** ms, double = 1.0);
	void equal(od_matrix* pA, int basex = 0, int basey = 0, double val = 1.0) {
		for (int i = 0; i < pA->dim; i++) 
			for (int j = 0; j < pA->dim_n; j++) 
				mat[i + basex][j + basey] = pA->mat[i][j] * val;
	}
	void equalT(od_matrix* pA,  int basex = 0, int basey = 0, double val = 1.0) {
		for (int i = 0; i < pA->dim; i++)
			for (int j = 0; j < pA->dim_n; j++)
				mat[i + basey][j + basex] = pA->mat[i][j] * val;
	}
	void neg(int si = 0, int sj = 0, int ei = 0, int ej = 0) {
		ei = (ei == 0) ? dim : ei;
		ej = (ej == 0) ? dim_n : ej;
		if (rowwise) { for (int i = si; i < ei; i++) for (int j = sj; j < ej; j++) mat[i][j] = -mat[i][j]; }
		else { for (int i = si; i < ei; i++) for (int j = sj; j < ej; j++) mat[j][i] = -mat[j][i]; }
	}

	void add(od_matrix* pA, double val = 1.0) {
		int i, j;
		if (rowwise) {
			for (i = 0; i < dim; i++) 
				for (j = 0; j < dim_n; j++) 
					mat[i][j] += pA->mat[i][j] * val;
		}
		else {
			for (i = 0; i < dim; i++) 
				for (j = 0; j < dim_n; j++)
					mat[j][i] += pA->mat[j][i] * val;
		}
	}
	void addB(int fromrow, int numrow, int shiftrow, double val = 1.0) {
		for (int j = 0; j < dim_n; j++) {
			double *p = mat[j];
			for (int i = fromrow; i < numrow + fromrow; i++) {
				p[i + shiftrow] += p[i] * val;
			}
		}
	}
	/*void addC(int colidx, int cols_, int delta, double val = 1.0) {
		for (int i = 0; i < dim; i++) {
			double* p = mat[i];
			for (int j = colidx; j < cols_ + colidx; j++) {
				p[j + delta] += p[j] * val;
			}
		}
	}*/
};
class od_matrix_dense : public od_matrix {
private:
	int* perm;
	double* pCol;
public:
	od_matrix_dense(int dim_s, int dim_c, int* = 0, int = 1);
	~od_matrix_dense() {
		int i;
		DELARY(perm);  DELARY(pCol);
		int temp = (rowwise == 1) ? dim : dim_n;
		for ( i = 0; i < temp; i++) DELARY(mat[i]); DELARY(mat);
//		for ( i = 0; i < temp; i++) DELARY(_mat[i]); DELARY(_mat);
	}
	virtual void init();
	virtual double* solve(double*, int repar = 0, int = 0, int* d = 0);
	double* solveL(double*, int repar = 0, int = 0, int* d = 0);
	double* solveU(double*, int repar = 0, int = 0, int* d = 0);
	virtual double* solveT(double*, int repar = 0, int = 0, int* d = 0);
	virtual void solve(od_matrix*, int = 0, int = 0);
	virtual void solveT(od_matrix*, int = 0, int = 0);
	virtual void print_out(int* flag = 0);
	//void _print_out(int* flag = 0);
	virtual int LU();
	void lbksb(double **a, int n, int *indx, double* b = 0);
	void ubksb(double **a, int n, int *indx, double* b = 0);
	void lubksb(double **a, int n, int *indx, double* b = 0);
	void lubksbT(double **a, int n, int *indx, double* b = 0);
	int ludcmp(double **a, int n, int *indx, double *d = 0);
	virtual void setPvecN(int *pved, int effn);
	virtual void MinusCaB(od_matrix* m1, od_matrix* m2);
	//virtual void setCol(int i);
};
class od_matrix_dense3B : public od_matrix_dense {
public:
	od_matrix_dense3B(int m = 3, int n = 3) :od_matrix_dense(m, n, 0, 0) { rowwise = 0; init(); }
};
class od_matrix_dense3C : public od_matrix_dense {
public:
	od_matrix_dense3C(int m = 3, int n = 3) : od_matrix_dense(m, n) {}
};
class od_matrix_denseB : public od_matrix_dense {
public:
	od_matrix_denseB(int m, int n) :od_matrix_dense(m, n, 0, 0) { rowwise = 0; init(); }
};

class od_blockTriDiagonal {
protected:
	int n;
	int _varNum;
	od_matrix *D_;
	od_matrix *F_, *E_;

public:
	od_blockTriDiagonal(int _n);
	inline int blockSize() const { return n; }
	inline int size() const { return _varNum; }
	void init();
	void zeros();
	~od_blockTriDiagonal() { DELARY(E_); DELARY(D_); DELARY(F_); }
	int LU();
	double* lubksb(double* b);
	double* lubksbT(double* b);
	double* solve(double*, int repar = 0);
	double* solveT(double*, int repar = 0);
	inline  od_matrix* D(int i) const { return D_ + i; }
	inline  od_matrix* E(int i) const { return E_ + i; }
	inline  od_matrix* F(int i) const { return F_ + i; }
	int ludcmp();
};

class od_matrix_composite {
protected:
	int  m, aux_m;
	double mu;
	od_matrix_dense *pM, *pMv, *pMd;
	od_matrix_dense *pA,  *pD;
	od_matrix_dense3C *pJ;
	od_matrix_dense *pB, *pC;
	int* pVec;
	int* perm;
	int eff_dim;
	int _repar;
public:
	od_matrix_composite(int _m, int a_m, int*);
	~od_matrix_composite() {
		delete pD; delete pM; delete pMv; delete pMd; delete pA;
		delete pC; delete pB; delete pVec; delete perm;
	}
	od_matrix_dense* M() const { pM->zeros();  return pM; }
	od_matrix_dense* Mv() const { pMv->zeros(); return pMv; }
	od_matrix_dense* Md() const { pMd->zeros(); return pMd; }
	od_matrix_dense3C* J() const { pJ->zeros();  return pJ; }
	void setPvecN(int *pvec, int effn);

};


class od_matrix_bdf : public od_matrix_composite {
private:
	int  h_m;
	int  h_am;
	od_matrix_dense3C *pJv;
	
public:
	od_matrix_bdf(int m_, int a_m, int* = 0);
	~od_matrix_bdf() {
		delete pJv; 
	}
	od_matrix_dense3C* Jv() const { pJv->zeros(); return pJv; }
	void update(double tinu, int *, int);
	void LU();
	double* solve(double*, int repar = 0, int = 0, int* d = 0);
};

class od_matrix_hht : public od_matrix_composite {
public:
	od_matrix_hht(int m_, int a_m, int* = 0);
	void update(double tinu, int *, int);
	void LU();
	double* solve(double*, int repar = 0, int = 0, int* d = 0);
};

#endif
