#ifndef OD_LINSOLV_H
#define OD_LINSOLV_H

#include <math.h>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include "od_topo.h"
#include "od_element.h"
using namespace std;
class od_matrix {
public:
	int dim, dim_n, rowwise;
	double** mat;
	int* pVec;
	int _repar;
	int eff_dim;
	int start_idx;
	int is_zero;
	int* perm;
	//int *_zeros_;
public:
	od_matrix(int m, int n, int _init = 1) {
		dim = m; dim_n = n; rowwise = 1; _repar = 1; start_idx = 0; is_zero = 1;
		pVec = new int[max(dim, dim_n)];
		perm = new int[dim]; fill(perm, perm + dim, 0);
		for (int i = 0; i < max(dim, dim_n); i++) { pVec[i] = i; }
		eff_dim = dim;
		if (_init) init();
	}
	~od_matrix() { DELARY(pVec);  DELARY(perm); }

	void zeros() {
		if (rowwise == 1)  for (int i = 0; i < dim; i++) fill(mat[i], mat[i] + dim_n, 0.0);
		else for (int i = 0; i < dim_n; i++) fill(mat[i], mat[i] + dim, 0.0);
	}
	void zerocol(int j) {
		  for (int i = 0; i < dim; i++) mat[i][j] = 0.0; 
	}
	inline  int zero() const { return is_zero; }
	void init();
	void startIdx(int v = 0) { start_idx = v; }
	double** values() const { return mat; }
	/*virtual double* solve(double*, int repar = 0, int = 0, int* d = 0) = 0;
	virtual double* solveT(double*, int repar = 0, int = 0, int* d = 0) = 0;
	virtual void solve(od_matrix*, int = 0, int = 0) = 0;
	virtual void solveT(od_matrix*, int = 0, int = 0) = 0;
	virtual int LU() = 0;
	virtual void print_out(int* pVec = 0) = 0;*/

	//virtual void setPvecN(int *pved, int effn) = 0;
	double* row(int i) const { return (rowwise == 1) ? mat[i] : 0; }
	double* col(int i) const { return (rowwise == 1) ? 0 : mat[i]; }
	inline int rows() const { return (rowwise == 1) ? dim : dim_n; }
	inline int cols() const { return (rowwise == 1) ? dim_n : dim; }
	void D_diagLF(od_matrix *L, od_matrix *F, int = 0, int = 0);
	double* CX(double* = 0, double* = 0, double = 1.0);
	double* BX(double*, double* = 0, double = 1.0);
	void BX(od_matrix* pA, int basei = 0, int basej = 0);
	double element(int i, int j) const { return  (rowwise == 1) ? mat[i][j] : mat[j][i]; }
	void  element(int i, int j, double val) {
		if (rowwise) {
			mat[i][j] = val; //_zeros_[i] = 1;
		}
		else {
			mat[j][i] = val;// _zeros_[j] = 1;
		}
	}
	inline void add(int i, int j, double val) {
		if (rowwise) {
			mat[i][j] += val;// _zeros_[i] = 1;
		}
		else {
			mat[j][i] += val;// _zeros_[j] = 1;
		}
		is_zero = 0;
	}
	void equalsub(int m1, int n1, double** ms, double = 1.0);
	void addsub(int m1, int n1, double** ms, double = 1.0);
	void equal(od_matrix* pA, int basex = 0, int basey = 0, double val = 1.0) {
		if (pA->is_zero) return;
		is_zero = 0; _repar = 1;
		for (int i = 0; i < pA->dim; i++)
			for (int j = 0; j < pA->dim_n; j++)
				mat[i + basex][j + basey] = pA->mat[i][j] * val;
	}
	void equalT(od_matrix* pA, int basex = 0, int basey = 0, double val = 1.0) {
		if (pA->is_zero) return;
		double t;
		for (int i = 0; i < pA->dim; i++)
			for (int j = 0; j < pA->dim_n; j++) {
				t = pA->mat[i][j];
				mat[i + basey][j + basex] = t * val;
			}
		is_zero = 0; _repar = 1;
	}
	void neg(int si = 0, int sj = 0, int ei = 0, int ej = 0) {
		ei = (ei == 0) ? dim : ei;
		ej = (ej == 0) ? dim_n : ej;
		if (rowwise) { for (int i = si; i < ei; i++) for (int j = sj; j < ej; j++) mat[i][j] = -mat[i][j]; }
		else { for (int i = si; i < ei; i++) for (int j = sj; j < ej; j++) mat[j][i] = -mat[j][i]; }
	}
	void add(od_matrix* pA, double val = 1.0) {
		if (pA->is_zero) return;
		int i, j;
		is_zero = 0; _repar = 1;
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
		//is_zero = 0;
	}
	inline void LUreq() { _repar = 1; }
	void setPvecN(int *pved, int effn);
};
class od_matrix_dense : public od_matrix {
private:
	
public:
	od_matrix_dense(int dim_s=3, int dim_c=3,  int = 1);
	~od_matrix_dense() {
		for (int i = 0; i < ((rowwise == 1) ? dim : dim_n); i++) DELARY(mat[i]); DELARY(mat);
	}
	virtual double* solve(double*,  int = 0, int* d = 0);
	double* solveL(double*, int repar = 0, int = 0, int* d = 0);
	double* solveU(double*, int repar = 0, int = 0, int* d = 0);
	virtual double* solveT(double*, int repar = 0, int = 0, int* d = 0);
	virtual void solve(od_matrix*, int = 0, int = 0);
	virtual void solveT(od_matrix*, int = 0, int = 0);
	virtual void print_out(int* flag = 0);
	virtual int LU();
	void lbksb(double **a, int n, int *indx, double* b = 0);
	void ubksb(double **a, int n, int *indx, double* b = 0);
	void lubksb(double **a, int n, int *indx, double* b = 0);
	void lubksbT(double **a, int n, int *indx, double* b = 0);
	int ludcmp(double **a, int n, int *indx, double *d = 0);
	//virtual void setPvecN(int *pved, int effn);
	
};
class od_matrix_dense3B : public od_matrix {
public:
	od_matrix_dense3B(int m = 3, int n = 3) :od_matrix(m, n, 0) { rowwise = 0; init(); }
};
class od_matrix_dense3C : public od_matrix {
public:
	od_matrix_dense3C(int m = 3, int n = 3) : od_matrix(m, n) {}
};
class od_matrix_denseB : public od_matrix {
public:
	od_matrix_denseB(int m, int n) :od_matrix(m, n, 0) { rowwise = 0; init(); }
};
class od_matrix_denseC : public od_matrix {
public:
	od_matrix_denseC(int m, int n) :od_matrix(m, n) {}
};

class od_matrix_denseBs {
private:
	std::vector<od_matrix_denseB*> pMs;
public:
	od_matrix_denseBs() {}
	~od_matrix_denseBs() {
		for (int i = 0; i < (int)pMs.size(); i++) delete pMs[i]; pMs.clear();
	}
	void addMat(int m, int n) {
		pMs.push_back(new od_matrix_denseB(m, n));
	}
	od_matrix_denseB* B(int i) { return pMs[i]; }
	int size() const { return (int)pMs.size(); }
	double* BX(double *b, double *t) {
		double *p = t;
		for (int i = 0; i < (int)pMs.size(); i++) {
			p = pMs[i]->BX(b, p);
		}
		return p;
	}
};
	
class od_matrix_denseCs {
private:
	std::vector<od_matrix_denseC*> pMs;
public:
	od_matrix_denseCs() {}
	~od_matrix_denseCs() {
		for (int i = 0; i < (int)pMs.size(); i++) delete pMs[i]; pMs.clear();
	}
	void addMat(int m, int n) {
		pMs.push_back(new od_matrix_denseC(m, n));
	}
	od_matrix_denseC* C(int i) { return pMs[i]; }
	int size() const { return (int)pMs.size(); }
	double* CX(double *b, double *t) {
		double* p = b;
		for (int i = 0; i < (int)pMs.size(); i++) {
			p = pMs[i]->CX(p, t);
		}
		return p;
	}
};

class od_matrix_mechanism {
protected:
	int  m, aux_m;
	double mu;
	od_matrix_dense *pM, *pMv, *pMd;
	od_matrix_dense *pA, *pD;
	od_matrix_dense3C *pJ;
	od_matrix_dense3B *pB;
	od_matrix_dense3C *pC;
	int* pVec;
	//int* perm;
	int eff_dim;
	int _repar;
	int *motiondof;
public:
	od_matrix_mechanism(int _m, int a_m);// , int* = 0);
	~od_matrix_mechanism() {
		delete pD; delete pM; delete pMv; delete pMd; delete pA;
		delete pC; delete pB; delete pVec; delete motiondof;
	}
	inline void LUreq() { _repar = 1; pA->LUreq(); pD->LUreq(); }
	od_matrix_dense* M() const { pM->zeros();  return pM; }
	od_matrix_dense* Mv() const { pMv->zeros(); return pMv; }
	od_matrix_dense* Md() const { pMd->zeros(); return pMd; }
	od_matrix_dense3C* J() const { pJ->zeros();  return pJ; }
	void setPvecN(int *pvec, int effn);
	void LU();
	void D_diagLF(od_matrix_denseCs *Cs, od_matrix_denseBs *Bs) {
		int iii, i, j, k, ii, jj;
		double temp, *pr, *pc;
		od_matrix_denseB* pB;
		od_matrix_denseC* pC;
		for (iii = 0; iii < Cs->size(); iii++) {
			pB = Bs->B(iii); if (pB->is_zero) continue;
			pC = Cs->C(iii); if (pC->is_zero) continue;
			for (ii = 0; ii < pA->eff_dim; ii++) {
				i = pA->pVec[ii];
				//if (pC->_zeros_[i] == 0) continue;
				pr = pC->row(i);
				for (jj = 0; jj < pA->eff_dim; jj++) {
					j = pA->pVec[jj];
					//if (pB->_zeros_[i] == 0) continue;
					pc = pB->col(j);
					temp = 0.0;
					for (k = 0; k < pB->rows(); i++)
						temp += pr[k] * pc[k];
					pA->add(i, j, temp);
				}
			}
		}

	}
	inline od_matrix_dense* A() { pA->zeros(); return pA; }
	inline od_matrix_dense3B* B() { pB->zeros(); return pB; }
	inline od_matrix_dense3C* C() { pC->zeros(); return pC; }
	inline od_matrix_dense* D() { pD->zeros(); return pD; }
	void update(double tinu, int* pvec, int effn) {
		setPvecN(pvec, effn);
		for (int i = 0; i < m; i++) {
			if (motiondof[i]!= 0) pJ->zerocol(i);
		}
		pD->zeros();
		pC->equal(pJ);
		pB->equalT(pJ, 0, 0, tinu*1.0);
		pA->setPvecN(pVec, m);
		pC->setPvecN(pVec + m, eff_dim - m);
		pB->setPvecN(pVec + m, eff_dim - m);
		pD->setPvecN(pVec + m, eff_dim - m);
		_repar = 1;
	}
	int dims() { return m + aux_m; }
};
class od_matrix_bdf : public od_matrix_mechanism {
private:
	int  h_m;
	int  h_am;
	od_matrix_dense3C *pJv;

public:
	od_matrix_bdf(int m_, int a_m);
	~od_matrix_bdf() {
		delete pJv;
	}
	od_matrix_dense3C* Jv() const { pJv->zeros(); return pJv; }
	void update(double tinu, int *, int);
	void LU();
	double* solve(double*);
};
class od_matrix_hht : public od_matrix_mechanism {
public:
	od_matrix_hht(int m_, int a_m);
	void update(double tinu, int *, int);
	double* solve(double*);
};
class od_matrix_blockTriDiagonal {
protected:
	int n, ndofs;
	int _varNum;
	od_matrix_dense *D_;
	od_matrix_dense3C *F_;
	od_matrix_dense3B *E_;
	int _repar;

public:
	od_matrix_blockTriDiagonal(int _n, int _ndofs = 3);
	inline int dims() const { return n; }
	inline int size() const { return _varNum; }
	void init();
	void zeros();
	~od_matrix_blockTriDiagonal() { DELARY(E_); DELARY(D_); DELARY(F_); }
	int LU();
	double* lubksb(double* b);
	double* solve(double*);
	void solve(od_matrix* pB) {
		double *pb;
		for (int i = pB->start_idx; i < pB->cols(); i++) {
			pb = pB->col(i);
			this->solve(pb);
		}
	}
	//double* solveT(double*, int repar = 0);
	inline  od_matrix* D(int i) const { return D_ + i; }
	inline  od_matrix* E(int i) const { return E_ + i; }
	inline  od_matrix* F(int i) const { return F_ + i; }
	int ludcmp();
	void LUreq() { _repar = 1; for (int i = 0; i < n - 1; i++) D_[i].LUreq(); }
	void equal(od_matrix_blockTriDiagonal *pM, double val = 1.0) {
		for (int i = 0; i < n - 1; i++) {
			D_[i].equal(pM->D_ + i, 0, 0, val); E_[i].equal(pM->E_ + i, 0, 0, val); F_[i].equal(pM->F_ + i, 0, 0, val);
		}
		D_[n - 1].equal(pM->D_ + n - 1, 0, 0, val);
	}
	void add(od_matrix_blockTriDiagonal *pM, double val = 1.0) {
		for (int i = 0; i < n - 1; i++) {
			D_[i].add(pM->D_ + i, val); E_[i].add(pM->E_ + i, val); F_[i].add(pM->F_ + i, val);
		}
		D_[n - 1].add(pM->D_ + n - 1, val);
	}
};
class od_matrix_subsystem {
protected:
	od_matrix_dense* A;
	od_matrix_denseB *B;
	od_matrix_dense *C;
	od_matrix_blockTriDiagonal *D;
	int n;
	int ndofs, varnum;;
	int _repar;
public:
	od_matrix_subsystem(int _n, int _dofs=0) {
		n = _n; ndofs = _dofs;
		varnum = n * ndofs;
		B = new od_matrix_denseB(varnum, 12); B->startIdx(6);
		C = new od_matrix_dense(12, varnum); C->startIdx(6);
		D = new od_matrix_blockTriDiagonal(n, ndofs);
		A = new od_matrix_dense(12, 12);
		_repar = 1;
	}
	~od_matrix_subsystem() {
		delete A; delete B; delete C; delete A;
	}
	void LUreq() { _repar = 1; A->LUreq(); D->LUreq(); }
	int dims() {return 12 + varnum;}
	int LU() {
		if (!_repar) return 1;
		D->LU();
		D->solve(B);
		A->D_diagLF(C, B);
		A->LU();
		_repar = 0;
		return 1;
	}
	void init() {
		A->init(); B->init(); C->init(); D->init(); _repar = 1;
	}
	double* solve(double* b) {
		double *tempd = new double[varnum];
		double *pb = b + 12;
		if (_repar) LU();
		D->solve(pb); fill(tempd, tempd + 6, 0.0);
		C->CX(pb, tempd);
		for (int i = 0; i < 12; i++) b[i] -= tempd[i];
		A->solve(b);
		fill(tempd, tempd + varnum, 0.0);
		B->BX(b, tempd);
		for (int i = 0; i < varnum; i++) { pb[i] -= tempd[i]; }
		delete[] tempd;
		
		return b + varnum + 12;
	}
	void solve(od_matrix_denseB* pB) {
		double *pb;
		for (int i = pB->start_idx; i < pB->cols(); i++) {
			//if (pB->_zeros_[i] == 0) continue;
			pb = pB->col(i);
			this->solve(pb);
		}
	}
};
class od_matrix_subsystems {
private:
	int dim;
	std::vector< od_matrix_subsystem*> pMat;
	int _repar;
public:
	od_matrix_subsystems() {
		dim = 0; _repar= 1; 
	}
	~od_matrix_subsystems() {
		for (int i = 0; i < (int)pMat.size(); i++) delete pMat[i]; pMat.clear();
	}
	void LUreq() {
		_repar = 1; for (int i = 0; i < (int)pMat.size(); i++) pMat[i]->LUreq();
	}
	void addMat(int n, int _dofs) {
		od_matrix_subsystem* p = new od_matrix_subsystem(n, _dofs);
		pMat.push_back(p);
		dim += p->dims(); _repar = 1;
	}
	void LU() {
		if (_repar == 0) return;
		for_each(pMat.begin(), pMat.end(), mem_fun(&od_matrix_subsystem::LU)); 
		_repar = 0;
	}
	double* solve(double* b) {
		if (_repar ) LU();
		for (int i = 0; i < (int)pMat.size(); i++)
			b = pMat[i]->solve(b);
		return b;
	}
	void solve(od_matrix_denseBs* pBs) {
		for (int i = 0; i < (int)pMat.size(); i++) pMat[i]->solve(pBs->B(i));
	}
	od_matrix_subsystem* M(int i) { return pMat[i]; }
};
class od_matrix_system {
private:
	od_matrix_hht *_A;
	od_matrix_denseBs *_B;
	od_matrix_denseCs *_C;
	od_matrix_subsystems *_D;
	int dim, numlam, nsys, _dims;
	int _repar;
public:
	od_matrix_system(int _dim, int _nlambda) {
		nsys = 0;
		dim = _dim;
		numlam = _nlambda;
		_A = new od_matrix_hht(dim, numlam);

		_C = new od_matrix_denseCs();
		_B = new od_matrix_denseBs();
		_D = new od_matrix_subsystems();
		dim += _nlambda;
		_dims = dim;
	}
	void update(double tinu, int* pvec, int effn) {
		_A->update(tinu, pvec, effn);
		//if (nsys) {
		// zero motion degree in B and C	
		//}
		LUreq();
	}
	void LUreq() { _repar = 1; _A->LUreq(); _D->LUreq(); }
	~od_matrix_system() { delete _A; if (nsys) { delete _B; delete _C; delete _D; } }
	void add_submatrix(int n, int dofs_) {
		_D->addMat(n, dofs_);
		_B->addMat(n*dofs_ + 12, dim);
		_C->addMat(dim, n*dofs_ + 12);
		nsys++;
		_dims += n * dofs_ + 12;
	}
	int LU() {
		if (_repar == 0)  return 1;
		if (nsys) {
			_D->LU();
			_D->solve(_B);
			_A->D_diagLF(_C, _B);
		}
		_A->LU();
		_repar = 0;
		return 1;
	}
	inline od_matrix_hht* A() { return _A; }
	inline od_matrix_subsystems* D() { return _D; }
	inline od_matrix_denseCs* C() { return _C; }
	inline od_matrix_denseBs* B() { return _B; }
	double* solve(double* b) {
		int varnum = _dims;
		double *tempd=0;
		double *pb = b + dim;
		if(_repar) LU(); 
		if (nsys) {
			tempd = new double[varnum];
			_D->solve(pb); fill(tempd, tempd + dim, 0.0);
			_C->CX(pb, tempd);
			for (int i = 0; i < dim; i++) b[i] -= tempd[i];
		}
		_A->solve(b);
		if(nsys){
			fill(tempd, tempd + varnum, 0.0);
			_B->BX(b, tempd);
			for (int i = 0; i < varnum; i++) { pb[i] -= tempd[i]; }
			delete[] tempd;
		}
		return b;
	}
};
#endif
