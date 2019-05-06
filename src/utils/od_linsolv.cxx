#include <assert.h>
#include "od_linsolv.h"
#include "mat33.h"

void od_matrix::equalsub(int m1, int n1, double** ms, double scale) {
	for (int i = 0; i < m1; i++) {
		for (int j = 0; j < n1; j++) {
			mat[i][j] = ms[i][j] * scale;
		}
	}
	is_zero = 0; _repar = 1;
}
void od_matrix::addsub(int m1, int n1, double** ms, double val) {
	for (int i = 0; i < m1; i++) {
		for (int j = 0; j < n1; j++) {
			mat[i][j] += ms[i][j] * val;
		}
	}
	is_zero = 0; _repar = 1;
}
double* od_matrix::BX(double* b, double *t, double val) {
	double* temp;
	if (!t) {
		temp = new double[dim];
		fill(temp, temp + dim, 0.0);
	}
	else temp = t;
	for (int i = start_idx; i < dim_n; i++) {
		for (int j = 0; j < dim; j++) {
			temp[j] += mat[i][j] * b[i] * val;
		}
	}
	if (!t) {
		copy(temp, temp + dim, b);
		delete[] temp;
	}
	return t+dim;
}
double* od_matrix::CX(double* b, double *t, double val) {
	double *temp;
	if (!t) {
		temp = new double[dim];
		fill(temp, temp + dim, 0.0);
	}
	else temp = t;

	for (int i = start_idx; i < dim; i++) {
		//if (_zeros_[i] == 0) continue;
		for (int j = 0; j < dim_n; j++) {
			temp[i] += mat[i][j] * b[j] * val;
		}
	}
	if (!t) {
		copy(temp, temp + dim, b);
		delete[] temp;
	}
	return b;
}
void od_matrix::BX(od_matrix* pA, int basei, int basej) {
	double *pc;
	int i, ii;
	if (pA->rowwise) {
		for (i = pA->start_idx; i < pA->rows(); i++) {
			//if (pA->_zeros_[i] == 0) continue;
			ii = i;// pA->pVec[i] + basei;
			pc = pA->row(ii) + basej;
			this->CX(pc);// , 0)
		}
	}
	else {
		for (i = pA->start_idx; i < pA->cols(); i++) {
			//if (pA->_zeros_[i] == 0) continue;
			ii = i;// pA->pVec[i] + basej;
			pc = pA->col(ii) + basei;
			this->CX(pc);// , 0)
		}
	}
}
int od_matrix_dense::ludcmp(double **a, int n, int *indx, double *d)
//
// Given a matrix a[0,...n-1][0,...n-1], this routine replaces it by the LU decomposition
// of a rowwise permutation of itself.  a and n are input.  a is output, arranged as
// in equation (2.3.14 (Numerical Recipes in C 2nd Edition)).  indx[0,...n-1] is an
// output vector that records the row permutation effected by the partial pivoting.
//   d is output as +1.0 or -1.0 depending on whether the number of row interchanges
// was even or odd, respectively.  This routine is used in combination with lubksb to
// solve linear equations or invert a matrix.
//
{
	//const double SMALL_VALUE = 1.e-9;
	int error_code = 0;            //  Clear the error_code flag
	int i, imax, j, k;
	double big, sum, temp;
	double *vv;                       //  vv stores the implicit scaling of each row.
//	for (i = 0; i < n; i++)for (j = 0; j < n; j++) _mat[i][j] = mat[i][j];
	vv = new double[n];               //  Allocate vv.
	if (d) *d = 1.0;                         //  No row interchanges yet.
	for (i = 0; i < n; i++) {   //  Loop over rows to get the implicit scaling information.
		big = 0.0;
		for (j = 0; j < n; j++)
			if ((temp = fabs(a[pVec[i]][pVec[j]])) > big)  big = temp;
		if (fabs(big) < SMALL_VALUE) {  // No nonzero element in this row
		  //error_code = 1;                // Set the error_code flag
			error_code = pVec[i] + 1;
			DELARY(vv);    // Release work space memory
			throw equ_exception(i);
			///         cout << endl << " Singular Matrix:  Row " << i << " is a zero row" << endl;
			return (error_code);            // Return the error_code flag
		}
		vv[i] = 1.0 / big;             //  Save the scaling.
	}

	for (j = 0; j < n; j++) {    //  This is the loop over columns of Crout's method.
		imax = j;
		for (i = 0; i < j; i++) { //  This is equation (2.3.12) except for  i = j.
			sum = a[pVec[i]][pVec[j]];
			for (k = 0; k < i; k++)
				sum -= a[pVec[i]][pVec[k]] * a[pVec[k]][pVec[j]];
			a[pVec[i]][pVec[j]] = sum;
		}
		big = 0.0;                      //  Initialize for the search for largest pivot element.
		for (i = j; i < n; i++) { //  This is  i = j  of equation (2.3.12) and  i = j+1...N
			sum = a[pVec[i]][pVec[j]];               //    of equation (2.3.13).
			for (k = 0; k < j; k++)
				sum -= a[pVec[i]][pVec[k]] * a[pVec[k]][pVec[j]];
			a[pVec[i]][pVec[j]] = sum;
			if ((temp = vv[i] * fabs(sum)) >= big && d) {  // Is the figure of merit for the pivot
				big = temp;                               //   better than the best so far?
				imax = i;
			}
		}
		if (j != imax && d) {                 //  do we need to interchange rows?
			for (k = 0; k < n; k++) {   //  Yes, do so...
				temp = a[pVec[imax]][pVec[k]];
				a[pVec[imax]][pVec[k]] = a[pVec[j]][pVec[k]];
				a[pVec[j]][pVec[k]] = temp;
			}
			*d = -(*d);                       //  ...and change the parity of d.
			vv[imax] = vv[j];                 //  Also interchange the scale factor.
		}
		indx[j] = imax;
		if (fabs(a[pVec[j]][pVec[j]]) < SMALL_VALUE) {  // Zero pivot, the matrix is singular
		  //error_code = 1;                      // Set the error_code flag
			error_code = j + 1;
			DELARY(vv);
			throw equ_exception(i);
			//        cout << endl << " Singular Matrix:  pivot " << j << ", " << j << " row(" << indx[j]
			//            << ") is " << a[j][j] << endl;
			return (error_code);                  // Return the error_code flag
		}
		if (j != n - 1) {            //  Now, finally, divide by the pivot element.
			temp = 1.0 / (a[pVec[j]][pVec[j]]);
			for (i = j + 1; i < n; i++)
				a[pVec[i]][pVec[j]] *= temp;
		}
	}                                       //  Go back for the next column in the reduction.
	DELARY(vv);
	return (error_code);
}
void od_matrix_dense::lbksb(double **a, int n, int *indx, double* b) {
	int i, ip, j;
	int ii = -1;
	double sum;

	for (i = 0; i < n; i++) {     //  When ii is set to a positive value, it will become the
		ip = indx[i];                //    index of the first nonvanishing element of b.  We now
		sum = b[pVec[ip]];                  //    do the forward substitution, equation (2.3.6).  The
		b[pVec[ip]] = b[pVec[i]];                   //    only new wrinkle is to unscramble the permutation
		//    as we go.
		if (ii >= 0)
			for (j = ii; j <= i - 1; j++)
				sum -= a[pVec[i]][pVec[j]] * b[pVec[j]];
		else if (sum)
			ii = i;                      //  A nonzero element was encountered, so from now on we
		  //    will have to do the sums in the loop above.
		b[pVec[i]] = sum;
	}
}
void od_matrix_dense::ubksb(double **a, int n, int *indx, double* b) {
	int i, j;
	int ii = -1;
	double sum;
	for (i = n - 1; i >= 0; i--) {    //  Now we do the backsubstitution, equation (2.3.7).
		sum = b[pVec[i]];
		for (j = i + 1; j < n; j++)
			sum -= a[pVec[i]][pVec[j]] * b[pVec[j]];
		b[pVec[i]] = sum / a[pVec[i]][pVec[i]];           //  Store a component of the solution vextor X.
	}
}
void od_matrix_dense::lubksb(double **a, int n, int *indx, double* b)
//
//  Solves the set of n linear equations  A * X = B.  Here a[0,...n-1][0,...n-1] is input, not as
//  the matrix A but rather as its LU decomposition, determined by the routine ludcmp.
//    indx[0..n-1] is input as the permutation vector returned by ludcmp.  b[0..n-1] is input as
//  the right-hand side vector B, and returns with the solution vector X.  a, n, and indx are
//  not modified by this routine and can be left in place for successive calls with different
//  right-hand sides b.  This routine takes into account the possibility that b will begin with
//  many zero elements, so it is efficient for use in matrix inversion.
//
{
	int i, ip, j;
	int ii = -1;
	double sum;

	for (i = 0; i < n; i++) {     //  When ii is set to a positive value, it will become the
		ip = indx[i];                //    index of the first nonvanishing element of b.  We now
		sum = b[pVec[ip]];                  //    do the forward substitution, equation (2.3.6).  The
		b[pVec[ip]] = b[pVec[i]];                   //    only new wrinkle is to unscramble the permutation
		//    as we go.
		if (ii >= 0)
			for (j = ii; j <= i - 1; j++)
				sum -= a[pVec[i]][pVec[j]] * b[pVec[j]];
		else if (sum)
			ii = i;                      //  A nonzero element was encountered, so from now on we
		  //    will have to do the sums in the loop above.
		b[pVec[i]] = sum;
	}

	for (i = n - 1; i >= 0; i--) {    //  Now we do the backsubstitution, equation (2.3.7).
		sum = b[pVec[i]];
		for (j = i + 1; j < n; j++)
			sum -= a[pVec[i]][pVec[j]] * b[pVec[j]];
		b[pVec[i]] = sum / a[pVec[i]][pVec[i]];           //  Store a component of the solution vextor X.
	}
}
void od_matrix_dense::lubksbT(double **a, int n, int *indx, double* b)
{
	int i, j;
	int ii = -1;
	double sum;
	//U^tY = b
	for (i = 0; i < n; i++) {
		sum = b[pVec[i]];
		for (j = 0; j < i; j++) {
			sum -= a[pVec[j]][pVec[i]] * b[pVec[j]];
		}
		b[pVec[i]] = sum / a[pVec[i]][pVec[i]];
	}

	for (i = n - 1; i >= 0; i--) {
		sum = b[pVec[i]];
		for (j = i + 1; j < n; j++) {
			sum -= a[pVec[j]][pVec[i]] * b[pVec[j]];
		}
		b[pVec[i]] = sum;
	}
}
od_matrix_dense::od_matrix_dense(int dim_m, int dim_c, int _init) : od_matrix(dim_m, dim_c) {
	//eff_dim = dim_m;
	//if (_init) init();
}

void od_matrix::init() {
	int i;
	//perm = new int[dim];

	if (rowwise == 1) {
		mat = new double*[dim];
		for (i = 0; i < dim; i++) {
			mat[i] = new double[dim_n];
		}
	}
	else {
		mat = new double*[dim_n];
		for (i = 0; i < dim_n; i++) {
			mat[i] = new double[dim]; //fill(mat[i], mat[i] + dim, 0.0);
		}
	}
	zeros();
}
double* od_matrix_dense::solve(double* X, int effn, int* pvec) {
	if (effn != 0 && pvec != 0) {
		setPvecN(pvec, effn);
	}
	if (_repar) {
		LU();
		_repar = 0;
	}
	for (int i = eff_dim; i < dim; i++) X[pVec[i]] = 0.0;
	lubksb(mat, eff_dim, perm, X);
	return X;
}
double* od_matrix_dense::solveL(double* X, int repar, int effn, int* pvec) {
	for (int i = eff_dim; i < dim; i++) X[pVec[i]] = 0.0;
	lbksb(mat, eff_dim, perm, X);
	return X;
}
double* od_matrix_dense::solveU(double* X, int repar, int effn, int* pvec) {
	//for (int i = eff_dim; i < dim; i++) X[pVec[i]] = 0.0;
	ubksb(mat, eff_dim, perm, X);
	return X;
}
void od_matrix_dense::solveT(od_matrix* m, int basei, int basej) {
	int i, j, mr = m->eff_dim;// rows();
	int mc = this->cols();
	double* pr;
	for (i = m->start_idx; i < mr; i++) {
		pr = m->row(m->pVec[i] + basei) + basej;
		for (j = eff_dim; j < mc; j++) pr[pVec[j]] = 0.0;
		lubksbT(mat, eff_dim, perm, pr);
	}
}
void od_matrix_dense::solve(od_matrix* m, int basei, int basej) {
	int i, j, mc = m->eff_dim;// cols();
	int mr = this->cols();
	double* pc;
	for (i = m->start_idx; i < mc; i++) {
		//if (_zeros_[m->pVec[i] + basej] == 0) continue;
		pc = m->col(m->pVec[i] + basej) + basei;
		for (j = eff_dim; j < mr; j++) pc[pVec[j]] = 0.0;
		lubksb(mat, eff_dim, perm, pc);
	}
}
void od_matrix::D_diagLF(od_matrix* L, od_matrix* F, int basei, int basej) {
	//	assert(L->cols() == F->rows());
	int i, j, k, ii, jj, kk;
	double *pr, *pc, sum;
	for (i = L->start_idx; i < L->eff_dim; i++) {
		ii = L->pVec[i];
		pr = L->row(ii);
		for (j = F->start_idx; j < F->eff_dim; j++) {
			jj = F->pVec[j];
			pc = F->col(jj);
			sum = 0;
			for (k = 0; k < L->cols(); k++) {
				kk = k;
				sum += pr[kk] * pc[kk];
			}
			mat[ii][jj] -= sum;
		}
	}
	_repar = 1;
}
double* od_matrix_dense::solveT(double* X, int repar, int effn, int* pvec) {
	setPvecN(pvec, effn);
	if (repar || _repar) {
		LU();
		repar = 0;
		_repar = 0;
	}
	for (int i = eff_dim; i < dim; i++) X[pVec[i]] = 0.0;
	lubksbT(mat, eff_dim, perm, X);
	return X;
}
void od_matrix::setPvecN(int *pvec, int effn) {
	int temp = (rowwise == 1) ? dim : dim_n;

	int ii = 0;
	for (int i = 0; i < temp; i++) {
		if (pvec[i] == 1) pVec[ii++] = i;
	}
	if (ii != eff_dim) {
		eff_dim = ii; _repar = 1;
	}
	for (int i = 0; i < temp; i++) {
		if (pvec[i] != 1) pVec[ii++] = i;
	}
	
}
int od_matrix_dense::LU() {
	double d;
	int error_code = 0;
	//	print_out();
	error_code = ludcmp(mat, eff_dim, perm, &d);
	if (!error_code) _repar = 0;
	return error_code;
}
void od_matrix_dense::print_out(int* pVec) {
	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < dim; j++) {
			if (pVec == 0) {
				cout << setw(8) << setfill(' ') << setprecision(1) << mat[i][j] << ' ';
			}
			else {
				cout << setw(8) << setfill(' ') << setprecision(1) << mat[pVec[i]][pVec[j]] << ' ';
			}
		}
		cout << endl;
	}
	cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
}
od_matrix_blockTriDiagonal::od_matrix_blockTriDiagonal(int _n, int _ndofs) {
	//int i;
	n = _n;
	ndofs = _ndofs;
	_varNum = ndofs * n;
	E_ = 0;
	F_ = 0;
	D_ = 0;
	if (n > 1) {
		E_ = new od_matrix_dense3B[n - 1]; //upper
		D_ = new od_matrix_dense[n];
		F_ = new od_matrix_dense3C[n - 1];  //lower
	}
	_repar = 1;
}
int od_matrix_blockTriDiagonal::LU() {
	if (!_repar) return 1;
	for (int i = 0; i < n - 1; i++) {
		D_[i].LU();
		D_[i].solveT(E_ + i);
		D_[i + 1].D_diagLF(E_ + i, F_ + i);
	}
	D_[n - 1].LU();
	_repar = 0;
	return 1;
}
void od_matrix_blockTriDiagonal::init() {
	for (int i = 0; i < n - 1; i++) {
		D_[i].init();
		E_[i].init();
		F_[i].init();
	}
	D_[n - 1].init();
}
void od_matrix_blockTriDiagonal::zeros() {
	for (int i = 0; i < n - 1; i++) {
		D_[i].zeros(); E_[i].zeros(); F_[i].zeros();
	}
	D_[n - 1].zeros();
}
int od_matrix_blockTriDiagonal::ludcmp() {
	this->LU();
	return 0;
}
double* od_matrix_blockTriDiagonal::lubksb(double* b) {
	int i;
	double *pb = 0;
	double *pbn = 0;
	for (i = 1; i < n; i++) {
		pb = b + 3 * (i - 1);
		pbn = pb + 3;
		pb = E_[i - 1].CX(pb);
		U_SUB3(pbn, pb);
	}
	///////
	D_[n - 1].solve(pbn);
	for (i = n - 2; i >= 0; i--) {
		pb = b + i * 3;
		pbn = F_[i].BX(pbn);
		U_SUB3(pb, pbn);
		D_[i].solve(pb);
		pbn = pb;
	}
	return b;
}
/*
double* od_matrix_blockTriDiagonal ::lubksbT(double* b) {
	int i;
	double *pb, *pbn=0;

	for (i = 0; i < n - 1; i++) {
		pb = b + 3 * i;
		D_[i].solveT(pb);
		pbn = pb + 3;
		pb = F_[i].CX(pb);
		U_SUB3(pbn, pb);
	}
	D_[n - 1].solveT(pbn);
	for (i = n - 2; i >= 0; i++) {
		E_[i].BX(pbn);
		pb = b + i * 3;
		U_SUB3(pb, pbn);
		pbn = pb + 3;
	}
	return b;
}
*/
double* od_matrix_blockTriDiagonal::solve(double* b) {
	if ( _repar) LU();
	lubksb(b);
	return 0;
}
/*
double* od_matrix_blockTriDiagonal ::solveT(double* b, int repar) {
	if (repar) LU();
	lubksbT(b);
	return 0;
}
*/
od_matrix_mechanism::od_matrix_mechanism(int m_, int a_m) {
	aux_m = a_m;
	m = m_;
	_repar = 1;
	pVec = new int[m + aux_m]; //perm = new int[m + aux_m];
	for (int i = 0; i < m + aux_m; i++) {
		pVec[i] = i; //perm[i] = i;
	}
	eff_dim = m + aux_m;
	_repar = 1;
	motiondof = new int[m]; fill(motiondof, motiondof + m, 0);
}

void od_matrix_mechanism::setPvecN(int *pvec, int effn) {
	for (int i = 0; i < m + aux_m; i++) {
		pVec[i] = pvec[i];
	}
	for (int i = 0; i < m; i++) {
		motiondof[i] = (pvec[i] == 1) ? 0 : 1;
	}
}
od_matrix_bdf::od_matrix_bdf(int m_, int a_m) : od_matrix_mechanism(m_, a_m) {
	aux_m = a_m;
	m = m_;
	h_m = m / 2;
	h_am = aux_m / 2;
	pM = new od_matrix_dense(h_m, h_m);
	pMv = new od_matrix_dense(h_m, h_m);
	pMd = new od_matrix_dense(h_m, h_m);
	pD = new od_matrix_dense(aux_m, aux_m);
	pA = new od_matrix_dense(h_m, h_m);
	pJ = new od_matrix_dense3C(h_am, h_m);
	pJv = new od_matrix_dense3C(h_am, h_m);
	pB = new od_matrix_dense3B(m, aux_m);
	pC = new od_matrix_dense3C(aux_m, m);
}
void od_matrix_bdf::update(double tinu, int* pvec, int effn) {
	/*//
			A      muMd      0       J^t
			muI     -I      J^t       0
			J        Jv       0      0
			0        J        0      0

			A+mu^2Md   0      muMdJ^t  J^t
			muI     -I       J^t      0
			J        Jv       0      0
			0        J        0      0
	//*/
	//int i;// , tempint;
	setPvecN(pvec, effn);
	pD->zeros(); pC->zeros(); pB->zeros(); pA->zeros();
	mu = 1.0 / tinu;
	pA->equal((od_matrix*)pM, 0, 0, -1.0);
	pA->add((od_matrix*)pMv, mu);
	pA->add((od_matrix*)pMd, mu*mu);

	pC->equal(pJ);
	pC->equal(pJv, 0, h_m);
	pC->equal(pJ, h_am, h_m);

	pB->equalT(pJ, 0, h_am);
	pB->equalT(pJ, h_m, 0);
	pMd->BX(pJ);
	pB->equalT(pJ, 0, 0, mu); //muMdJ^t

	//for (i = 0; i < aux_m; i++)
	//	pVec[m + i] -= m;
	pA->setPvecN(pVec, h_m);
	pC->setPvecN(pVec + m, eff_dim - m);
	pB->setPvecN(pVec + m, eff_dim - m);
	pD->setPvecN(pVec + m, eff_dim - m);
	_repar = 1;
}
/*
 M     Mdmu    B1    A    0     B1'    A    0    B1'=B1-MdmuB2         A   0    B1'
 Imu    -I     B2->  muI  -I    B2 --> 0   -I    B2'=B2-muA^{-1}B1'--> 0   I    -B2'
 C1    C2      0     C1   C2    0      0    C2    C1A^{-1}B1'          0   0   C1A^{-1}B1'+C2B2'
*/
void od_matrix_bdf::LU() {
	/*
	[A  B]          [A                     0 ] [I     A{ -1 }B]
				=
	[C   D]         [C          D - CA ^ {-}B]  [ 0        I    ]
	*/
	if (!_repar) return;
	pA->LU();  //A^{-1}
	pA->solve((od_matrix*)pB); //A^{-1}B1'
	pB->addB(0, h_m, h_m, -mu); //B2'=B2-muA^{-1}B1'
	pB->neg(h_m, 0, m, aux_m);   //B2'=-B2'  == inv(-I)

	pD->D_diagLF(pC, pB);
	pD->LU();
	_repar = 0;
}
double* od_matrix_bdf::solve(double *b) {
	double* tempd = new double[max(m, aux_m)];
	//double temp;
	int i;
	//for (i = 0; i < h_m; i++) { temp = b[i]; b[i] = b[i + h_m]; b[h_m + i] = temp; }
	if ( _repar) 	LU();
	
	fill(tempd, tempd + h_m, 0.0);
	pMd->CX(b + h_m, tempd, mu);
	for (i = 0; i < h_m; i++) { b[i] += tempd[i]; }//b1'=b1+Mdmub2
	pA->solve(b);
	for (i = 0; i < h_m; i++) { b[i + h_m] -= b[i] * mu; b[i + h_m] = -b[i + h_m]; }//b2'=b2-muA^{-1}b1   b2=-b2
	fill(tempd, tempd + aux_m, 0.0);
	pC->CX(b, tempd);
	for (i = 0; i < aux_m; i++) b[m + i] -= tempd[i];
	pD->solve(b + m);
	fill(tempd, tempd + m, 0.0);
	pB->BX(b + m, tempd);
	for (i = 0; i < m; i++) { b[i] -= tempd[i]; }

	delete[] tempd;
	return b;
}
od_matrix_hht::od_matrix_hht(int m_, int a_m) : od_matrix_mechanism(m_, a_m) {

	pM = new od_matrix_dense(m, m);
	pMv = new od_matrix_dense(m, m);
	pMd = new od_matrix_dense(m, m);
	pD = new od_matrix_dense(aux_m, aux_m);
	pA = new od_matrix_dense(m, m);
	pJ = new od_matrix_dense3C(a_m, m);
	pB = new od_matrix_dense3B(m, a_m);
	pC = new od_matrix_dense3C(aux_m, m);
	pVec = new int[m + aux_m]; //perm = new int[m + aux_m];
	for (int i = 0; i < m + aux_m; i++) {
		pVec[i] = i; //perm[i] = i;
	}
	eff_dim = m + aux_m;
	_repar = 1;
}

void od_matrix_hht::update(double tinu, int* pvec, int effn) {
	setPvecN(pvec, effn);
	pD->zeros(); pC->zeros(); pB->zeros(); pA->zeros();
	pA->equal((od_matrix*)pM);
	pA->add((od_matrix*)pMv);
	pA->add((od_matrix*)pMd);
	pC->equal(pJ);
	pB->equalT(pJ, 0, 0, -1.0);
	pA->setPvecN(pVec, m);
	pC->setPvecN(pVec + m, eff_dim - m);
	pB->setPvecN(pVec + m, eff_dim - m);
	pD->setPvecN(pVec + m, eff_dim - m);
}

void od_matrix_mechanism::LU() {
	if (_repar == 0) return;
	pA->LU();
	pA->solve(pB);
	pD->D_diagLF(pC, pB);
	pD->LU();
	_repar = 0;
}
double* od_matrix_hht::solve(double *b) {
	double* tempd = new double[max(m, aux_m)];
	int i;
	if ( _repar) {
		LU();
		_repar = 0;
	}

	pA->solve(b);
	fill(tempd, tempd + aux_m, 0.0);
	pC->CX(b, tempd);
	for (i = 0; i < aux_m; i++) b[m + i] -= tempd[i];
	pD->solve(b + m);

	fill(tempd, tempd + m, 0.0);
	pB->BX(b + m, tempd);
	for (i = 0; i < m; i++) { b[i] -= tempd[i]; }

	delete[] tempd;
	return b;
}
