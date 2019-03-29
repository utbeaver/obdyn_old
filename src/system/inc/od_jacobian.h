#ifndef OD_JACOBIAN_H
#define OD_JACOBIAN_H
#include <vector>
#ifndef SMALL_VALUE_T
#define SMALL_VALUE_T 1.0e-6
#endif

using namespace std;
#ifdef _WINDOWS
#define LINKODJACDLL __declspec(dllexport)
#else
#define LINKODJACDLL
#endif

template<class T>
class BlockLU {
private:
	int m, n, i;
	//T *M00, *M11, *M10, *M01;
	T *T11, *T00, *T01, *T10;
	double *tempd;
public:
	BlockLU(T* t00, T* t11, T* t10, T* t01) {
		//M00 = t00; M11 = t11; M10 = t10; M01 = t01;
		T11 = t00; T00 = t11; T10 = t01; T01 = t10;
		m = T11.rows(); n = T00.rows();
		tempd=new double[m>n?m:n]
	}
	BlockLU() {
		DELARY(tempd);
	}
	void LU() {
		T00.LU();
		T00.solveT(T10);
		T11.MinusCaB(T10, T01);
		T11.LU();
		T00.solve(T01);
	}
	double* solve(double *vals, int repar, int effn, int d=0) {
		double* v1 = vals;
		double* v0 = vals + m;
		if (repar) LU();
		// LU1
		copy(v0, v0 + n, tempd);
		tempd = T01 * tempd;
		for (i = 0; i < n; i++) v0[i] -= tempd[i];
		//Lu2
		T00.solve(v0);
		T11.solve(v1);
		//L3
		copy(v1, v1 + m, tempd);
		temp = T10 * temp;
		for (i = 0; i < m; i++) v1[0] -= tempd[i];
		return vals + m + n;
	}
};

template<class T>
class LINKODJACDLL od_jacobian {
private:
	T** objArrayInCol;
	T** objList;
	int *startIndex, *numNZinCol;
	int num_rows, num_cols, _size;
	int created;
	int _no_instance;
	int size_;
public:
	od_jacobian() {
		num_rows = 0; num_cols = 0; _no_instance = 0;
		created = 0;
		objArrayInCol = 0;
		startIndex = 0;
		numNZinCol = 0;
		_size = 0;
		size_ = 0;
		objList = 0;
	}
	~od_jacobian() {
		if (objArrayInCol) {
			DELARY(objArrayInCol);
		}
		if (objList) {
			for (int i = 0; i < size_; i++) DELARY(objList[i]);
			DELARY(objList);
		}
		DELARY(startIndex);
		DELARY(numNZinCol);
	}
	void init();
	inline void no_instance() { _no_instance = 1; }
	void create_jacobian(int r, int c, int *topolevel);
	inline int number_rows() const { return num_rows; }
	inline int number_cols() const { return num_cols; }
	inline T** column(int, int*, int*) const;
	inline T* pObject(int index) const {
		return objList[index];
	}
	inline int size() const {
		return size_;
	}
	inline  T* element(int i, int j) const;
	void toFile(ofstream*, const char* pName);
	od_jacobian<T>& operator=(od_jacobian<T>&);
	od_jacobian<T>& operator+=(od_jacobian<T>&);
	od_jacobian<T>& operator-=(od_jacobian<T>&);
};

template<class T>
void od_jacobian<T>::init() {
	T** tempObj = objList;
	for (int i = 0; i < size_; i++) { (*tempObj)->init(); tempObj++; }
}

template<class T>
void od_jacobian<T>::create_jacobian(int r, int c, int *topolevel)
{
	int i;
	T** tempObj = objList;
	for (i = 0; i < size_; i++) { (*tempObj)->init(); tempObj++; }
	if (created) return;
	int  j, temp_int, start_index, nz_in_col;
	num_rows = r;
	num_cols = c;
	_size = r * c;
	objArrayInCol = new T*[_size];
	for (i = 0; i < _size; i++) objArrayInCol[i] = 0;
	startIndex = new int[num_cols];
	numNZinCol = new int[num_cols];
	for (j = 0; j < num_cols; j++) {
		nz_in_col = 0;
		start_index = -1;
		for (i = 0; i < num_rows; i++) {
			temp_int = topolevel[i*num_cols + j];
			if (temp_int != 0) {
				if (start_index == -1)
					start_index = i;
				nz_in_col++;
			}
		}
		if (start_index == -1)
			start_index = 0;
		startIndex[j] = start_index;
		numNZinCol[j] = nz_in_col;
	}
	vector<T*> _objList;
	if (!_no_instance) {
		for (j = 0; j < num_cols; j++) {
			int startindex = startIndex[j] + num_rows * j;
			for (i = 0; i < numNZinCol[j]; i++) {
				T* pT = new T;
				*(objArrayInCol + startindex + i) = pT;
				_objList.push_back(pT);
			}
		}
		size_ = _objList.size();
		objList = new T*[size_];
		for (i = 0; i < size_; i++) objList[i] = _objList[i];
	}

	created = 1;
}


template<class T>
T*	od_jacobian<T>::element(int i, int j) const {
	return *(objArrayInCol + j * num_rows + i);
}

template<class T>
T** od_jacobian<T>::column(int i, int* idx, int* len) const {
	*idx = startIndex[i];
	*len = numNZinCol[i];
	return (objArrayInCol + num_rows * i);
}

template<class T> int compare_Jac(od_jacobian<T>& Jac, od_jacobian<T>& Jac1) {
	int diff = 0;
	unsigned size1, size2, i;
	T *T1, *T2;
	size1 = Jac.size();
	size2 = Jac1.size();
	if (size1 != size2) {
		diff = 1;
	}
	else {
		for (i = 0; i < size1; i++) {
			T1 = Jac.pObject(i);
			T2 = Jac1.pObject(i);
			if (*T1 == *T2) {
				diff = 1;
			}
		}
	}
	return  diff;
}

template<class T, class Vec>
void multiplyMat_Assembly(od_jacobian<T>& Jac,
	od_jacobian<Vec>& Input,
	od_jacobian<Vec>& Output,
	od_constraint** cons
)
{
	int i, ii, j, idx, _len;
	int jac_num_cols = Jac.number_cols();
	int num_rows = Jac.number_rows();
	int num_cols = Input.number_cols();
	Vec3 **input = 0;
	Vec3 **output = 0;
	double *pd1, *pd;
	int intTemp = 3 * num_rows;
	double* Sum = new double[intTemp];

	for (ii = 0; ii < num_cols; ii++) {
		//for(j=0; j<intTemp; j++) Sum[j]=0.0;
		fill(Sum, Sum + intTemp, 0.0);
		input = Input.column(ii, &idx, &_len);
		output = Output.column(ii, &idx, &_len);
		for (j = idx; j < idx + _len; j++) {
			if (output[j]) {
				pd1 = Sum + 3 * j;
				i = ((od_element*)cons[j])->get_prev_idx();
				if (j != idx) {
					pd = Sum + 3 * i;
					EQ3(pd1, pd);
				}
				if(input[j]) { //if output[j]==0, input[j] should be 0
					U_ADD3(pd1, input[j]->v);
				}
				EQ3(output[j]->v, pd1);
			}
		}
	}
	DELARY(Sum)
}

template<class T>
void od_jacobian<T>::toFile(ofstream*  pOutFile, const char* pName) {
	int i, j, ii, jj;
	T* pObj;
	vector<double> xyz[3];
	T Zero; Zero.init();
	*pOutFile << pName << "=[" << endl;
	for (i = 0; i < num_rows; i++) {
		for (j = 0; j < 3; j++) xyz[j].resize(0);
		for (j = 0; j < num_cols; j++) {
			pObj = element(i, j);
			if (!pObj) pObj = &Zero;
			for (ii = 0; ii < pObj->numRows(); ii++) {
				int cols = pObj->numCols();
				for (jj = 0; jj < cols; jj++) {
					xyz[ii].push_back((pObj->v)[ii*cols + jj]);
				}
			}
		}
		for (j = 0; j < 3; j++) {
			for (ii = 0; ii < (int)xyz[j].size(); ii++) {
				*pOutFile << scientific << setprecision(6);
				if (fabs(xyz[j][ii]) < SMALL_VALUE_T) xyz[j][ii] = 0.0;
				*pOutFile << setw(9) << xyz[j][ii] << " ";
			}
			*pOutFile << ";" << endl;
		}
	}
	*pOutFile << "];" << endl;
}

template<class T>
od_jacobian<T>& od_jacobian<T>::operator=(od_jacobian<T>& odj) {
	for (int i = 0; i < size_; i++) { (*(objList[i])) = (*(odj.objList[i])); }
	return *this;
}
template<class T>
od_jacobian<T>& od_jacobian<T>::operator+=(od_jacobian<T>& odj) {
	for (int i = 0; i < size_; i++) { (*(objList[i])) += (*(odj.objList[i])); }
	return *this;
}
template<class T>
od_jacobian<T>& od_jacobian<T>::operator-=(od_jacobian<T>& odj) {
	for (int i = 0; i < size_; i++) { (*(objList[i])) -= (*(odj.objList[i])); }
	return *this;
}
#endif
