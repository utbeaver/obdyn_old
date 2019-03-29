#include "od_equset.h"
#include "od_gstiff.h"
#include "od_system.h"

od_integrator::od_integrator(double initS) {
	h = initS;
	initH = initS;
	evalJac = 1;
	maxCorrectNum = 5;
	hMin = 1.0e-6; hMax = 1.0e-2; numFuncEvals = 0; numJacEvals = 0; numSteps = 0;
	rhsTol = 1.e-12;
	success = 0;
}

od_integrator::~od_integrator(void)
{
	int i;
	for (i = 0; i < numVar; i++) {
		DELARY(yy[i]);
		DELARY(yySave[i]);
	}
	DELARY(yy);
	DELARY(yySave);
	DELARY(yyErr);
	DELARY(yyErrSave);
	DELARY(yyMax);
}

void od_integrator::allocate(void)
{
	yy = new double*[numVar];
	yySave = new double*[numVar];
	for (int i = 0; i < numVar; i++) {
		yy[i] = new double[7];
		yySave[i] = new double[7];
		fill(yy[i], yy[i] + 7, 0.0);
		fill(yySave[i], yySave[i] + 7, 0.0);
	}
	yyErr = new double[numVar];
	yyErrSave = new double[numVar];
	yyMax = new double[numVar];
	fill(yyErr, yyErr + numVar, 0.0);
	fill(yyErrSave, yyErrSave + numVar, 0.0);
	fill(yyMax, yyMax + numVar, 1.0);
}

od_gstiff::od_gstiff(od_equation_bdf_I* pequ, double _initStep) : od_integrator(_initStep){
	int i;
	pEqu = pequ;
	numVar = pEqu->num_rows();
	//h = _initStep;
	//initH = _initStep;
	pEqu = pequ;
	_X = pEqu->X();
	_Xdot = pEqu->Xdot();
	_Xddot = pEqu->Xddot();

	allocate();
	maxCorrectNum = 5;
	EPS = .001;
	init(0.0, _initStep);
	for (i = 0; i < numVar; i++) {
		yyMax[i] = (fabs(_X[i]) < 1.0) ? 1.0 : fabs(_X[i]);
	}
	//evalJac = 1;
	//  integrationStepFail=0;
	// integrationStepSuccessful=0;
	integratorOrderMax = 6;
	//hMin = 1.0e-6; hMax = 1.0e-2; numFuncEvals = 0; numJacEvals = 0; numSteps = 0;
	ofstream* pmsg = pEqu->msgFile();
}

od_gstiff::~od_gstiff() {
	/*int i;
	for (i = 0; i < numVar; i++) {
		DELARY(yy[i]);
		DELARY(yySave[i]);
	}
	DELARY(yy);
	DELARY(yySave);
	DELARY(yyErr);
	DELARY(yyErrSave);
	DELARY(yyMax);*/
}

void od_gstiff::restartIntegrator() {

	init(time, initH);
	evalJac = 1;
}

void od_gstiff::init(double startTime, double _initStep)
{
	time = startTime;
	h = _initStep;
	initH = _initStep;
	invH = 1.0 / h;
	setOrder(1);
	pEqu->get_states();
	for (int i = 0; i < numVar; i++) {
		yy[i][0] = _X[i];
		if (pEqu->Var(i)) yy[i][1] = 0.0;
		//if (pEqu->Var(i))  yy[i][1] = 0.0;
		else	yy[i][1] = _Xdot[i] * h;
		yyMax[i] = fabs(_X[i]);
		if (yyMax[i] < 1.0) yyMax[i] = 1.0;
	}
	reSetSi2();
}

void od_hhti3::init(double startTime, double _initStep)
{
	time = startTime;
	h = _initStep;
	initH = _initStep;
	invH = 1.0 / h;
	pEqu->get_states();
	for (int i = 0; i < numVar; i++) {
		yy[i][0] = _Xddot[i];
		yy[i][1] = 0.0;
/*		if (pEqu->Var(i)) yy[i][1] = 0.0;
		else	yy[i][1] = _Xdot[i] * h;
		yyMax[i] = fabs(_X[i]);
		if (yyMax[i] < 1.0) yyMax[i] = 1.0;*/
	}
}

void od_gstiff::setOrder(int newOrder) {
	integratorOrder = newOrder;
	b0inv = -1.0 / A_values[integratorOrder - 1][0];
	K = integratorOrder + 1;
	IDOUB = K;
	ENQ1 = 0.5 / (double)integratorOrder;
	ENQ2 = .5 / (double)(integratorOrder + 1);
	ENQ3 = .5 / (double)(integratorOrder + 2);
	double PEPSH = EPS;
	IDOUB_MAX = 8;
	E = PERTST[integratorOrder][0] * PEPSH;
	E *= E;

	EUP = PERTST[integratorOrder][1] * PEPSH;
	EUP *= EUP;

	EDOWN = PERTST[integratorOrder][2] * PEPSH;
	EDOWN *= EDOWN;
	deltaTol = (PEPSH/1000.0 < PEPSH*ENQ3/numVar) ?PEPSH/1000.0 :PEPSH*ENQ3/numVar ;
	//deltaTol = PEPSH * ENQ3 / numVar;
}

void od_gstiff::predict() {
	int i, j, k;
	for (i = 0; i < numVar; i++) {
		if (pEqu->Var(i))  continue;
		for (j = 0; j < integratorOrder; j++) {
			for (k = integratorOrder; k > j; k--) {
					yy[i][k - 1] += yy[i][k];
			}
		}
	}
	reSetSi2();
}
void od_gstiff::setInitStep(double val) { initH = val; init(0.0, initH); }
void od_gstiff::reSetSi2() {
	//return;
	for (int j = 0; j < numVar; j++) {
		if (pEqu->Var(j) == 2)
			for (int i = 0; i <= integratorOrder; i++) yy[j][i] = 0.0;
	}
}

void od_integrator::saveOld() {
	int i, j;
	timeOld = time;
	hOld = h;
	integratorOrderOld = integratorOrder;
	for (i = 0; i < numVar; i++) {
		for (j = 0; j <= integratorOrderOld; j++) {
			yySave[i][j] = yy[i][j];
		}
	}
}

void od_integrator::restoreOld() {
	int i, j;
	time = timeOld;
	h = hOld;
	integratorOrder = integratorOrderOld;
	for (i = 0; i < numVar; i++) {
		//if (pEqu->Var(i))  continue;
		for (j = 0; j <= integratorOrderOld; j++) {
			yy[i][j] = yySave[i][j];
		}
	}
}

void od_gstiff::scaleStep(double ratio) {
	int i, j;
	if (ratio*h > hMax) {
		ratio = hMax / h;
	}
	else if (ratio*h < hMin) {
		ratio = hMin / h;
	}
	if (fabs(ratio - 1.0) < 1.0e-2) return;
	double r = 1.0;
	for (j = 1; j <= integratorOrder; j++) {
		r *= ratio;
		for (i = 0; i < numVar; i++) {
			if (pEqu->Var(i))  continue;
			yy[i][j] *= r;
		}
	}
	h *= ratio;
	invH = 1.0 / h;
	evalJac = 1;
}

void od_gstiff::interpolate(double _end) {
	int i, j;
	double rr;
	double r = 1.0;
	invH = 1.0 / h;
	double endStep = _end - time;
	double ratio = endStep * invH;
	fromYYtoX(invH);
	r = ratio;
	for (j = 1; j <= integratorOrder; j++) {
		for (i = 0; i < numVar; i++) {
			if (pEqu->Var(i)==0) _X[i] += yy[i][j] * r;
		}
		r *= ratio;
	}
	r = ratio * invH;
	for (j = 2; j <= integratorOrder; j++) {
		rr = r * j;
		for (i = 0; i < numVar; i++) {
			if (pEqu->Var(i) == 0) _Xdot[i] += yy[i][j] * rr;
		}
		r *= ratio;
	}
	pEqu->set_states();
}

int od_hhti3::correct() {
	int i, j, maxRhsIdx, errorCode = 1;
	int rhsConverged = 0;
	int deltaConverged = 0;
	double dblTemp, left, right, ratio, bhh;
	double *pRhs = 0;
	double maxRhs = 0.0;
	double xi = 1.0;
	double c = 0.001;
	double scale = 1.0;
	ofstream* pmsg = pEqu->msgFile();
	int debug = pEqu->debug();
	pEqu->time(time);
	if (pmsg && debug) {
		*pmsg << setw(10) << "Count" << setw(20) << "Maximum RHS" << setw(20) << "Maximum Delta" << endl;
	}
	fill(yyErr, yyErr + numVar, 0.0);
	Dx1 = 1.0;
	right = c * c * phiN / (h*h*h*h);
	bhh = this->beta*h*h;
	for (numCorrect = 0; numCorrect < maxCorrectNum; numCorrect++) {
		Dx = Dx1;
		this->fromYYtoX();
		pRhs= pEqu->evalRhs(); numFuncEvals++;
		if (numCorrect % 3 == 0) evalJac = 1;
		if (evalJac) {
			numJacEvals++;
			pEqu->evalJac();
			pEqu->evalJacHHT();
			evalJac = 0;
		}
		for (j = 0; j < numVar; j++) {
			dblTemp = fabs(pRhs[j]);
			if (dblTemp >= maxRhs) {
				maxRhs = dblTemp;
				maxRhsIdx = j;
			}
		}
		maxRhs = pRhs[maxRhsIdx];
		rhsConverged = 1;

		if (fabs(maxRhs) > rhsTol) {
			rhsConverged = 0;
		}
	
		pEqu->solve(bhh);
		Dx1 = getNorm(pRhs, _Xddot, treedofs);
		for (i = 0; i < numVar; i++) {
			_Xddot[i] += pRhs[i]*scale;
		}
		scale *= 0.99;
		ratio = Dx1 / Dx;
		ratio = ratio / (1 - ratio);
		//ratio = 1.0;
		left = ratio * ratio*Dx1*Dx1;
		deltaConverged = 1;
		if (left >= right) deltaConverged = 0;
		for (i = 0; i < numVar; i++) {
			for (j = integratorOrder-1; j >= 1; j--) {
				yy[i][j] = yy[i][j-1];
			}
			yy[i][0] = _Xddot[i];
			yyErr[i] += pRhs[i];
		}
		if (deltaConverged /*&& numCorrect >=1 && rhsConverged*/) {
			pEqu->set_states();
			return 0;
		}
	}
	return errorCode;
}

int od_gstiff::correct() {
	int j, errorCode = 0;
	double dblTemp;
	invH = 1.0 / h;
	int maxRhsIdx, maxDeltaIdx;
	double maxRhs = 0.0;
	double maxDelta = 0.0;
	double tinu = b0inv * invH;
	double *pRhs = 0;
	double *pprhs = 0;
	int deltaConverged = 1;
	int rhsConverged = 1;
	ofstream* pmsg = pEqu->msgFile();
	int debug = pEqu->debug();
	fill(yyErr, yyErr + numVar, 0.0);
	pEqu->time(time);
	if (pmsg && debug) {
		*pmsg << setw(10) << "Count" << setw(20) << "Maximum RHS" << setw(20) << "Maximum Delta" << endl;
	}
	for (numCorrect = 0; numCorrect < maxCorrectNum; numCorrect++) {
		maxRhs = 0.0;
		maxDelta = 0.0;
		maxRhsIdx = 0; maxDeltaIdx = 0;
		fromYYtoX(invH);
		pRhs = pEqu->evalRhs(); numFuncEvals++;
		pprhs = pEqu->pprhs;
		if (numCorrect%3 ==0) evalJac = 1;
		if (evalJac ) {
			numJacEvals++;
			pEqu->evalJac(tinu);
			pEqu->evalJacBdf(tinu);
			evalJac = 0;
		}
		maxRhsIdx = getMax(pRhs, numVar, maxRhs);
		//maxRhs = pRhs[maxRhsIdx];
		rhsConverged = 1;
		if (fabs(maxRhs) > rhsTol) {
			rhsConverged = 0;
		}
		double s = pEqu->startRecord();
		errorCode = pEqu->solve(tinu);
		pEqu->solveBDF(tinu);
		pRhs = pprhs;
		pEqu->stopRecord(s, 2);
		if (errorCode) {
			return 2; //singular Jacobian
		}
		deltaConverged = 1;
		maxDeltaIdx = getMax(pRhs, numVar, maxDelta, pEqu->Var());
		for (j = 0; j < numVar; j++) {
			if (!pEqu->Var(j)) {
				if (deltaTol*yyMax[j] < fabs(pRhs[j]))
					deltaConverged = 0;
			}
		}
		maxDelta = pRhs[maxDeltaIdx];
		//update YY
		double deltaB0inv;
		for (j = 0; j < numVar; j++) {
			yy[j][0] += pRhs[j];
			deltaB0inv = pRhs[j] * b0inv;
			//if (pEqu->Var(j))deltaB0inv = 0.0;
			yy[j][1] += deltaB0inv;
			yyErr[j] += deltaB0inv;
		}
		if (pmsg && debug) {
			*pmsg << scientific << setprecision(8);
			*pmsg << setw(10) << numCorrect << setw(20) << maxRhs << setw(20) << maxDelta << endl;
		}
		if (numCorrect == 0) deltaConverged = 0;
		if (deltaConverged /*&& rhsConverged*/) {
			return 0;
		}
	}
	return 1;
}

void od_gstiff::fromYYtoX(double factor) {
	for (int i = 0; i < numVar; i++) {
		_X[i] = yy[i][0];
		//if (pEqu->Var(i)) _Xdot[i] = 0.0;
		//else
		_Xdot[i] = yy[i][1] * factor;
	}
}

void od_gstiff::getNewStepAndOrder(int fail) {
	int newIntegratorOrder;
	int i;
	double bx, dblTemp;
	if (!fail) {
		if (numCorrect < 2 && h >= hMax) {
			return;
		}
		else if (IDOUB >= 1) {
			IDOUB--;
			if (IDOUB == 0) {
				for (i = 0; i < numVar; i++) yyErrSave[i] = yyErr[i];
			}
			return;
		}
	}
	//  Factor for order kept the same
	double factorSame = 1.2;
	PR2_goNowhere = pow((errorSum / E), ENQ2)*factorSame;
	if (!fail && integratorOrder < integratorOrderMax) {
		errorSum = 0.0;
		for (i = 0; i < pEqu->num_cols(); i++) {
			if (pEqu->Var(i)) continue;
			dblTemp = fabs(yy[i][0]);
			dblTemp = (dblTemp > 1.0) ? dblTemp : 1.0;
			bx = (yyErr[i] - yyErrSave[i]) / dblTemp;
			errorSum += bx * bx;
		}
		double factorUp = 1.4;
		PR3_goUp = pow((errorSum / EUP), ENQ3)*factorUp;
	}
	else {
		PR3_goUp = 1.0e17;
	}
	//  Factor for order reduced by one
	if (integratorOrder > 1) {
		errorSum = 0.0;
		for (i = 0; i < pEqu->num_cols(); i++) {
			if (pEqu->Var(i)) continue;
			//     bx = yy[i][integratorOrder]/yyMax[i];
			dblTemp = fabs(yy[i][0]);
			dblTemp = (dblTemp > 1.0) ? dblTemp : 1.0;
			bx = yy[i][integratorOrder] / dblTemp;
			errorSum += bx * bx;
		}
		double factorDown = 1.30;
		PR1_goDown = pow((errorSum / EDOWN), ENQ1)*factorDown;
	}
	else {
		PR1_goDown = 1.0e17;
	}
	//Get min. dividing factor
	if (PR3_goUp < PR2_goNowhere) {
		newIntegratorOrder = integratorOrder + 1;
		dividMin = PR3_goUp;
	}
	else {
		newIntegratorOrder = integratorOrder;
		dividMin = PR2_goNowhere;
	}
	if (dividMin > .1 && PR1_goDown < dividMin) {
		newIntegratorOrder = integratorOrder - 1;
		dividMin = PR1_goDown;
	}
	if (dividMin < .1) dividMin = .1;
	double scale = 1.0 / dividMin;
	if (!fail && scale < 1.1) {
		IDOUB = IDOUB_MAX;
	}
	else {
		IDOUB = K;
		if (newIntegratorOrder > integratorOrder) {
			double minusNewIntOrdInv = -1.0 / newIntegratorOrder;
			for (i = 0; i < numVar; i++) {
				if (pEqu->Var(i)==0)
					yy[i][newIntegratorOrder] = yyErr[i] * A_values[newIntegratorOrder - 1][newIntegratorOrder] * minusNewIntOrdInv;
			}
		}
		if (newIntegratorOrder != integratorOrder) {
			if (scale > 1.1) scale = 1.1;
			setOrder(newIntegratorOrder);
			IDOUB = IDOUB_MAX;
		}
		scaleStep(scale);
		IDOUB = IDOUB_MAX;
	}
	return;
}

int od_gstiff::toTime(double timeEnd) {
	if (time >= timeEnd) return 0;
	int returnCode = 0;
	int errorCode = 0;
	int correctFail = 0;
	int numTries = 0;
	ofstream* pmsg = pEqu->msgFile();
	int debug = pEqu->debug();
	if (pmsg && debug) *pmsg << "#Attemp to simulate from " << time << endl;
	evalJac = 1;
	while (time < timeEnd) {
		saveOld();
		time += h;
		predict();
		errorCode = correct();
		if (errorCode == 0) {
			correctFail = 0;
			errorCode = checkIntegrationError();
			if (errorCode) {
				restoreOld();
				numTries++;
				evalJac = 1;
				if (numTries > 10) {
					returnCode = 1;
					return returnCode;
					break;
				}
				else {
					getNewStepAndOrder(errorCode);
				}
			}
			else {
				numTries = 0;
				if (pEqu->checkEulerBryant()) {
					restartIntegrator();
				}
				else {
					numSteps++;
					getNewStepAndOrder(errorCode);
				}
			}
		}
		else {
			correctFail++;
			IDOUB = IDOUB_MAX;
			restoreOld();
			numTries++;
			if (correctFail > 5 || h <= hMin) {
				int flag = pEqu->checkRedundancy();
				if (flag) {
					od_equation_acc_ic acc_ic(pEqu->model());
					acc_ic.initialize();
					acc_ic.solve();
					restartIntegrator();
					correctFail = 0;
				}
			}
			if (correctFail > 10 || h <= hMin) {
				//throw exception
				returnCode = 1;
				return returnCode;
				break;
			}
			//if covergence failure
			if (correctFail != 1) {
				if (errorCode == 1) {
					scaleStep(.25);
				}
				else if (errorCode == 2) {
					scaleStep(3.0);
				}
			}
			if (correctFail >= 1) { evalJac = 1; }
		}
	}
	reSetSi2();
	fromYYtoX(invH);
	for (int i = 0; i < numVar; i++) {
		yyMax[i] = (fabs(_X[i]) < yyMax[i]) ? yyMax[i] : fabs(_X[i]);
	}
	if (pmsg) {
		*pmsg << "%Steps taken " << numSteps << endl;
		*pmsg << setw(12) << "%Time" << setw(12) << "StepSize" << setw(6) << "Order";
		*pmsg << setw(10) << "rhs" << setw(10) << "jac" << setw(12) << "KPE";
		*pmsg << setw(12) << "Cpu" << setw(12) << "JacCpu" << setw(12) << "RhsCPU" << endl;
	}
	if (pmsg) {
		*pmsg << scientific << setprecision(4);
		*pmsg << setw(12) << time << setw(12) << h << setw(6) << integratorOrder;
		*pmsg << setw(10) << numFuncEvals << setw(10) << numJacEvals << setw(12) << pEqu->getKPE();
		od_system* pM = pEqu->model();
		*pmsg << setw(12) << pM->cpuTime() << setw(12) << pM->jacTime() << setw(12) << pM->rhsTime() << ";" << endl;
	}
	return 0;

}

void od_hhti3::getNewStepAndOrder(int fail) {
	double deltaX = 0.0;
	if (fail) {
		restoreOld();
		h = max(hMin, h*0.67);
		integratorOrder = 1;
	}
	else {


	}

}

int od_hhti3::toTime(double timeEnd)
{
	if (time >= timeEnd) return 0;
	int returnCode = 0;
	double PHI, deltaX = 0.0;
	int errorCode = 0;
	int correctFail = 0;
	int numTries = 0;
	ofstream* pmsg = pEqu->msgFile();
	int debug = pEqu->debug();
	int i;// , success = 0;
	if (pmsg && debug) *pmsg << "#Attemp to simulate from " << time << endl;
	evalJac = 1;
	phiN = phi*treedofs;
//	pEqu->get_states();
	while (time < timeEnd) {
		correctFail = 1;
		saveOld();
		//pEqu->updatepQ();
		pEqu->calMa(h);
		this->predict();
		time += h;
		errorCode = correct();
		if (errorCode) { //does not converge, reduce h
			restoreOld();
			h = max(hMin, h*0.67);
			integratorOrder = 1;
			//getNewStepAndOrder(errorCode);
			success = 0;
		}
		else {
			deltaX = getNorm(yyErr, _Xddot, treedofs);
			PHI = deltaX * deltaX*h*h*h*h / phiN;
			if (deltaX < 1.0e-18) { //SMALL_VALUE
				h = hMax;
				success = 0;
				correctFail = 0;
			} else if (PHI > 1.0) { //reduce h
				restoreOld();
				h = max(0.9*h / pow(PHI, 1.0 / 6.0), hMin);
				integratorOrder = 1;
				success = 0;
			} else {
				correctFail = 0;
				success++;
				if (integratorOrder < integratorOrderMax) integratorOrder++;
				if (success > 10) {
					h = 0.9*h / pow(PHI, 1.0 / 6.0);
					h = (h > hMax) ? hMax : h;
					integratorOrder = 1;
					success = 0;
				}
			}
		}
		if (correctFail == 0) {
			for (i = 0; i < numVar; i++) {
				yyErrSave[i] = yyErr[i];
				pX[i] = _X[i];
				pXd[i] = _Xdot[i];
				pXdd[i] = _Xddot[i];
			}
			pEqu->updatepQ();
		}
	}
	return 0;
}


void od_hhti3::interpolate(double _end)
{
}

void od_gstiff::output(double expectedTime) {
	saveOld();
	interpolate(expectedTime);
	restoreOld();

}

int od_gstiff::checkIntegrationError() {
	int i, errorCode = 0;
	double bx, dblTemp;
	errorSum = 0.0;
	for (i = 0; i < pEqu->num_cols(); i++) {
		if (pEqu->Var(i)) continue;
		dblTemp = fabs(yy[i][0]);
		dblTemp = (dblTemp > 1.0) ? dblTemp : 1.0;
		bx = yyErr[i] / dblTemp;
		errorSum += bx * bx;
	}
	if (errorSum > E) {
		return 1;
	}
	for (int j = 2; j <= integratorOrder; j++) {
		for (i = 0; i < numVar; i++) {
			if (pEqu->Var(i))  continue;
			yy[i][j] -= A_values[integratorOrder - 1][j] * yyErr[i];
		}
	}
	return 0;
}

od_hhti3::od_hhti3(od_equation_hhti3* pequ, double _initStep, double _alpha) : od_integrator(_initStep) {
	int i;
	pEqu = pequ;
	alpha = _alpha;
	beta_gamma(alpha);
	numVar = pEqu->num_rows();
	treedofs = pEqu->treeDofs();
	_X = pEqu->X();
	_Xdot = pEqu->Xdot();
	_Xddot = pEqu->Xddot();
	pX = new double[numVar];
	pXd = new double[numVar];
	pXdd = new double[numVar];

	for (i = 0; i < numVar; i++) {
		pX[i] = _X[i];
		pXd[i] = _Xdot[i];
		pXdd[i] = _Xddot[i];
	}
	allocate();
	init(0.0, _initStep);
	pEqu->calMa(_initStep);
	for (i = 0; i < numVar; i++) {
		yy[i][0] = _Xddot[i];
	}
	Dx = Dx1 = 0.0;
	integratorOrder = 1;
	integratorOrderMax = 3;
}

void od_hhti3::fromYYtoX(double factor) {
	for (int i = 0; i < numVar; i++) {
		_X[i] = pX[i] +h * pXd[i] + 0.5*h*h*((1.0 - 2.0*beta)*pXdd[i] + 2.0*beta*_Xddot[i]);
		_Xdot[i] = pXd[i] +h * ((1.0 - gamma)*pXdd[i] + gamma * _Xddot[i]);
	}
}


void od_hhti3::predict()
{
	for (int i = 0; i < numVar; i++) _Xddot[i] = pXdd[i];
	//this->fromYYtoX();
}


