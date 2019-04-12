#ifndef OD_GSTIFF_H
#define OD_GSTIFF_H

class od_equation_dynamic;
class od_equation_bdf_I;
class od_equation_hhti3;

class od_integrator {
protected:
	int numVar, evalJac, treedofs;
	double time, h, hOld;
	double hMin, hMax, invH, initH;
	double *_X, *_Xdot, *_Xddot;
	double deltaTol, EPS, rhsTol;
	double timeOld;
	int maxCorrectNum, numCorrect;
	double errorSum;
	int numFuncEvals, numJacEvals, numSteps;
	int maxIterCorrect;
	double **yy, **yySave;
	double *yyErr, *yyErrSave, *yyMax;
	int integratorOrder, K;
	int integratorOrderOld;
	int integratorOrderMax;
	int success;
	int modifiedLU;

public:
	od_integrator(double initStep);
	~od_integrator(void);
	void setMaxStepSize(double val) { hMax = val; }
	void setMinStepSize(double val) { hMin = val; }
	void setMaxCorrectNum(int val) { maxCorrectNum = val; }
	inline double getH() const {return h;}
	void allocate(void);
	void saveOld();
	void restoreOld();
	
	int getMax(double* v, int n, double& maxv, int* vars=0) {
		double tempd = 0;
		int idx=0;
		maxv = 0.0;
		for (int i = 0; i < n; i++) {
			tempd = fabs(v[i]);
			if (vars && *(vars+i)) tempd = 0.0;
			if (tempd > maxv) {
				maxv = tempd;
				idx = i;
			}
		}
		return idx;
	}
	double getNorm(double* dv, double* v, int n) {
		int i;
		double tempd, tempv;
		tempd = 0.0;
		for (i = 0; i < n; i++) {
			tempv =fabs(dv[i])/max(fabs(v[i]), 1.0);
			tempd += tempv * tempv;
			//tempd = (tempv > tempd) ? tempv : tempd;
		}
		//tempd /= (double)n;
		return sqrt(tempd/(double)n);
		//return tempd;
	}
};
class od_gstiff : public od_integrator {
private:
	od_equation_bdf_I *pEqu;
	double ENQ1, ENQ2, ENQ3, E, EUP, EDOWN;
	double PR1_goDown, PR2_goNowhere, PR3_goUp, dividMin;
	// int integrationStepFail;
	// int integrationStepSuccessful;
	int IDOUB, IDOUB_MAX;
	double b0inv;
	//int numFuncEvals, numJacEvals, numSteps;
	void reSetSi2();
public:
	od_gstiff(od_equation_bdf_I* pequ, double initStep);
	~od_gstiff();
	void init(double start_time, double initStep);
	void setInitStep(double val);
	void setLocalErrTol(double val) { EPS = val; setOrder(1);; }
	
	void restartIntegrator();
	//void setErrorBound(double val);
	void setOrder(int val);
	int toTime(double end, int=0);
	void predict();
	void interpolate(double _end);
	//void saveOld();
	//void restoreOld();
	void scaleStep(double scale);
	int checkIntegrationError();
	int correct();
	void getNewStepAndOrder(int);
	void fromYYtoX(double factor);
	void output(double);
};
class od_hhti3 : public od_integrator {
private:
	double alpha;
	double beta;
	double gamma;
	od_equation_hhti3 *pEqu;
	void beta_gamma(double a) {
		beta = (1.0 - a); beta = beta * beta / 4.0;  gamma = 0.5 - a;
	}
	double phi, phiN;
	double *pX, *pXd, *pXdd;
	void Phi(void) {
		double tempd =beta -  1.0 / ((1.0 + alpha) * 6.0);
		tempd *= tempd;
		phi = EPS * EPS / tempd;
	}
	double Dx, Dx1;

public:
	od_hhti3(od_equation_hhti3* pequ, double _initStep, double _alpha = -1.0 / 3.0);
	~od_hhti3() {
		DELARY(pX); DELARY(pXd); DELARY(pXdd);
	}
	void setInitStep(double val) { h=initH = val; }
	void setLocalErrTol(double val) {
		EPS = val; Phi();
	}
	
	int toTime(double, int=0);
	void predict();
	void interpolate(double _end);

	void init(double start_time, double initStep);

	int correct();
	void getNewStepAndOrder(int);
	void fromYYtoX(double factor=0);
	//void output(double);
	//inline double getAlpha(void) const { return alpha; }
	inline double getBeta(void) const { return beta; }
	inline double getGamma(void) const { return gamma; }
};

const double PERTST[8][3] = {
	{  0.000000,  0.000000, 0.000000 },
	{  2.000000,  3.000000, 1.000000 },
	{  4.500000,  6.000000, 1.000000 },
	{  7.333000,  9.167000, 0.500000 },
	{ 10.420000, 12.500000, 0.166700 },
	{ 13.700000, 15.980000, 0.041330 },
	{ 17.150000,  1.000000, 0.008267 },
	{  1.000000,  1.000000, 1.000000 }, };


const double A_values[6][7] = {
	{ -1.0      ,    -1.0  },
	{ -2. / 3.    ,    -1.0,    -1. / 3. },
	{ -6. / 11.   ,    -1.0,    -6. / 11.   ,    -1. / 11. },
	{ -0.480    ,    -1.0,    -0.700    ,    -0.200   ,    -0.020 },
	{ -120. / 274.,    -1.0,    -225. / 274.,    -85. / 274.,    -15. / 274.,    -1. / 274.},
	{ -180. / 441.,    -1.0,    -58. / 63.  ,    -15. / 36. ,    -25. / 252.,    -3. / 252.,    -1. / 1764.}
};
#endif
