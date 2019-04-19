#include <algorithm>
#include <math.h>
#include <ctype.h>
#include "expr.h"
#include "od_system.h"
#include "od_measure.h"

Expression::Expression(const char* expr, od_systemGeneric* psys, Expr_Type type) :  od_object("") {
	e_type = type;// pSys = psys;
	initialized = 0;
	this->set_system(psys);
	for (int i = 0; i < strlen(expr); i++) {
		if (!isspace(expr[i]))
			_expr += toupper(expr[i]);
	}
	evaled = 0; I = J = K = 0;
	devaled = 0; ddevaled = 0; Measure = 0; jointMeasure = 0;
	for (int i = 0; i < 3; i++) {
		columns[i].resize(0);
		partial[i].resize(0);
		columns_[i] = 0;
		partial_[i] = 0;
	}
	parent = 0; x0y1z2 = 0;
	sub_expr_ = 0; config_expr_ = 0;
	pi = 3.1415926535897931;

}

Expression::Expression(string& expr, od_systemGeneric* psys, Expr_Type type) :  od_object("") {
	e_type = type; pSys = psys; initialized = 0;
	evaled = 0; I = J = K = 0;
	devaled = 0; ddevaled = 0; Measure = 0; jointMeasure = 0;
	for (int i = 0; i < 3; i++) {
		columns[i].resize(0);
		partial[i].resize(0);
		columns_[i] = 0;
		partial_[i] = 0;
	}
	parent = 0; x0y1z2 = 0;
	sub_expr_ = 0; config_expr_ = 0;
	for (char c : expr) {
		if (!isspace(c))
			_expr += toupper(c);
	}
	pi = 3.1415926535897931;
}


Expression::~Expression() {
	unsigned i;
	for (i = 0; i < sub_expr.size(); i++) delete sub_expr[i];
	for (i = 0; i < 3; i++) {
		DELARY(columns_[i]); DELARY(partial_[i])

	}
	DELARY(sub_expr_) DELARY(config_expr_)

}

int Expression::unevaluate() {
	evaled = 0; devaled = 0; ddevaled = 0;
	for (unsigned i = 0; i < sub_expr.size(); i++) {
		sub_expr_[i]->unevaluate();
	}
	if (Measure) Measure->unevaluate();
	return 1;
}

int Expression::init() {
	int not_matched, operand, index;
	int e_index, i, j;
	int k;
	struct token_property {
		Expr_Type type;
		string str;
	} property;
	if (initialized) return 1;
	if (e_type == CONSTANT || e_type == PI || e_type == TIME) { initialized = 1; return 1; }
	if (e_type == JOINTD || e_type == JOINTV || e_type == JOINTA) {
		vector<int> ids;
		int last_idx = 0;
		for (i = 0; i < _expr.size(); i++) {
			if (_expr[i] == ',' || i == _expr.size() - 1) {
				int ii = (i == (int)_expr.size() - 1) ? i + 1 : i;
				string val_str(_expr, last_idx, ii - last_idx);
				ids.push_back(atoi(val_str.c_str()));
				last_idx = i + 1;
			}
		}
		int id1 = 0;
		int id2 = 0;
		id1 = ids[0];
		if (ids.size() > 1) {
			id2 = ids[1];
		}
		jointMeasure = new od_joint_measure(id1, (char*)"", pSys, (int)(e_type - JOINTD), id2);
		jointMeasure->init();
		//    sumColumn.resize(0);
		for (int jj = 0; jj < 3; jj++) {
			//int temp_col = jointMeasure->get_partial_col();
			partial[jj].push_back(0.0);
		}
		Expression* _parent = this->parent;
		Expression* _offspring = this;
		while (_parent) {
			_parent->add_config_expr(_offspring);
			_offspring = _parent;
			_parent = _parent->parent;
		}
	}
	if (e_type == DX || e_type == DY || e_type == DZ ||
		e_type == RX || e_type == RY || e_type == RZ) {
		vector<int> ids;
		int last_idx = 0;
		for (i = 0; i < _expr.size(); i++) {
			if (_expr[i] == ',' || i == _expr.size() - 1) {
				int ii = (i == _expr.size() - 1) ? i + 1 : i;
				string val_str(_expr, last_idx, ii - last_idx);
				ids.push_back(atoi(val_str.c_str()));
				last_idx = i + 1;
			}
		}
		if (ids.size() == 1) { I = ids[0]; J = 0;      K = 0; }
		if (ids.size() == 2) { I = ids[0]; J = ids[1]; K = 0; }
		if (ids.size() == 3) { I = ids[0]; J = ids[1]; K = ids[2]; }
		if (e_type == DX || e_type == DY || e_type == DZ || e_type == DM) {
			Measure = new od_measure(pSys, od_measure::DT, I, J, K);
		}
		else if (e_type == RX || e_type == RY || e_type == RZ) {
			Measure = new od_measure(pSys, od_measure::DR, I, J, K);
		}
		if (e_type == DX || e_type == RX) x0y1z2 = 0;
		else if (e_type == DY || e_type == RY) x0y1z2 = 1;
		else if (e_type == DZ || e_type == RZ) x0y1z2 = 2;
		else x0y1z2 = 3;

		//    sumColumn.resize(0);
		Measure->init();
		for (int jj = 0; jj < 3; jj++) {
			int _size = (int)Measure->get_partial_size(jj);
			for (j = 0; j < _size; j++) {
				//int temp_col = (int)Measure->get_partial_col(jj, j);
				partial[jj].push_back(0.0);
			}
		}
		Expression* _parent = this->parent;
		Expression* _offspring = this;
		while (_parent) {
			_parent->add_config_expr(_offspring);
			_offspring = _parent;
			_parent = _parent->parent;
		}
		return 1;
	}

	//init the op_priority
	if (op_priority.size() == 0) {
		op_priority['+'] = 10000;
		op_priority['-'] = 10000;
		op_priority['*'] = 20000;
		op_priority['/'] = 20000;
		op_priority['^'] = 30000;
	}
	//check if ( and ) are matched
	not_matched = 0;
	for (i = 0; i < _expr.length(); i++) {
		if (_expr[i] == '(') not_matched++;
		if (_expr[i] == ')') not_matched--;
	}
	if (not_matched) {
		//exception
	}
	for (i = 0; i < _expr.size(); i++) {
		property.str.erase();
		operand = 0;

		//numbers
		if ((isdigit(_expr[i]) || _expr[i] == '.')) {
			property.type = CONSTANT;
			property.str += _expr[i]; i++;
			while (isdigit(_expr[i]) || _expr[i] == '.') {
				property.str += _expr[i]; i++;
			}
			//i--;
			operand = 1;
		}
		//TIME

		if (_expr.substr(i, 4) == "TIME") {
			property.type = TIME;
			property.str += "TIME";
			operand = 1;
			i = i + 4;
		}
		//PI
		if (_expr.substr(i, 2) == "PI") {
			property.type = PI;
			property.str += "PI";
			operand = 1;
			i = i + 2;
		}
		//JOINT
		if (_expr.substr(i, 7) == "JOINTD(" || _expr.substr(i, 7) == "JOINTV(" || _expr.substr(i, 7) == "JOINTA(") {
			if (_expr.substr(i, 7) == "JOINTD(") property.type = JOINTD;
			if (_expr.substr(i, 7) == "JOINTV(") property.type = JOINTV;
			if (_expr.substr(i, 7) == "JOINTA(") property.type = JOINTA;
			e_index = (int)fmp(i + 7);
			for (j = i + 7; j < e_index; j++) property.str += _expr[j];
			operand = 1;
			i = e_index + 1;
		}

		//sin fucntion
		if (_expr.substr(i, 4) == "SIN(") {
			e_index = (int)fmp(i + 4);
			property.type = SIN;
			for (j = i + 4; j < e_index; j++) property.str += _expr[j];
			operand = 1;
			i = e_index + 1;
		}
		//cos function
		if (_expr.substr(i, 4) == "COS(") {
			e_index = (int)fmp(i + 4);
			property.type = COS;
			for (j = i + 4; j < e_index; j++) property.str += _expr[j];
			operand = 1;
			i = e_index + 1;
		}
		//tan function
		if (_expr.substr(i, 4) == "TAN(") {
			e_index = (int)fmp(i + 4);
			property.type = TAN;
			for (j = i + 4; j < e_index; j++) property.str += _expr[j];
			operand = 1;
			i = e_index + 1;
		}
		//exp function
		if (_expr.substr(i, 4) == "EXP(") {
			e_index = (int)fmp(i + 4);
			property.type = EXP;
			for (j = i + 4; j < e_index; j++) property.str += _expr[j];
			operand = 1;
			i = e_index + 1;
		}
		//log function
		if (_expr.substr(i, 4) == "LOG(") {
			e_index = (int)fmp(i + 4);
			property.type = LOG;
			for (j = i + 4; j < e_index; j++) property.str += _expr[j];
			operand = 1;
			i = e_index + 1;
		}
		//sqrt function

		if (_expr.substr(i, 5) == "SQRT(") {
			e_index = (int)fmp(i + 5);
			property.type = SQRT;
			for (j = i + 5; j < e_index; j++) property.str += _expr[j];
			operand = 1;
			i = e_index + 1;
		}
		//DX, DY, DZ

		if (_expr.substr(i, 3) == "DX(" || _expr.substr(i, 3) == "DY(" || _expr.substr(i, 3) == "DZ(") {
			e_index = (int)(i + 3);
			if (_expr[i + 1] == 'X') property.type = DX;
			if (_expr[i + 1] == 'Y') property.type = DY;
			if (_expr[i + 1] == 'Z') property.type = DZ;
			if (_expr[i + 1] == 'M') property.type = DM;
			for (j = i + 3; i < e_index; i++) {
				property.str += _expr[j];
			}
			operand = 1;
			i = e_index + 1;
		}
		//RX, RY, RZ

		if (_expr.substr(i, 3) == "RX(" ||
			_expr.substr(i, 3) == "RY(" ||
			_expr.substr(i, 3) == "RZ(") {

			e_index = (int)fmp(i + 3);
			if (_expr[i + 1] == 'X') property.type = RX;
			if (_expr[i + 1] == 'Y') property.type = RY;
			if (_expr[i + 1] == 'Z') property.type = RZ;
			for (j = i + 3; i < e_index; i++) {
				property.str += _expr[j];
			}
			operand = 1;
			i = e_index + 1;
		}
		// LP
		if (_expr[i] == '(') {
			e_index = (int)fmp(i + 1);
			property.type = PAREN;
			for (j = i + 1; j < e_index; j++) property.str += _expr[j];
			operand = 1;
			i = e_index + 1;
		}

		if (operand) {
			Expression* temp_obj =
				new Expression(property.str, pSys, property.type);
			index = (int)sub_expr.size();
			sub_expr.push_back(temp_obj);
			op_stack.push_back(index);
			temp_obj->set_parent(this);
			temp_obj->init();
			operand = 0;
		}
		// +-*/^
		if (_expr[i] == '+' || _expr[i] == '-' ||
			_expr[i] == '*' || _expr[i] == '/' || _expr[i] == '^') {
			int tempInt = _expr[i] + 10000;
			if (op_erator.size() > 0) {
				if (op_priority[op_erator.top() - 10000] >= op_priority[_expr[i]]) {
					op_stack.push_back(op_erator.top());
					op_erator.pop();
				}
			}
			op_erator.push(tempInt);
		}
		/*if(operand) {
		  Expression* temp_obj =
			new Expression(property.str, pSys, property.type);
		  index=sub_expr.size();
		  sub_expr.push_back(temp_obj);
		  op_stack.push_back(index);
		  temp_obj->set_parent(this);
		  temp_obj->init();
		  operand = 0;
		}*/
	}

	while (!op_erator.empty()) {
		op_stack.push_back(op_erator.top()); op_erator.pop();
	}
	int _size;
	for (i = 0; i < config_expr.size(); i++) {
		Expression* pExpr = config_expr[i];
		for (j = 0; j < 3; j++) {
			_size = (int)pExpr->get_partial_size(j);
			for (k = 0; k < _size; k++) {
				columns[j].push_back((int)pExpr->get_partial_col(j, k));
				partial[j].push_back(0.0);
			}
		}
	}
	if (jointMeasure) {
		for (j = 0; j < 3; j++) {
			columns[j].push_back(jointMeasure->get_partial_col());
			partial[j].push_back(0.0);
		}
	}
	for (i = 0; i < 3; i++) {
		_size = (int)columns[i].size();
		if (_size) {
			columns_[i] = new int[_size];
			partial_[i] = new double[_size];
			for (j = 0; j < (int)_size; j++) {
				columns_[i][j] = columns[i][j];
				partial_[i][j] = partial[i][j];
			}
		}
	}

	// for_each(sub_expr.begin(), sub_expr.end(), mem_fun(&Expression::init));
	_size = (int)sub_expr.size();
	sub_expr_ = new Expression*[_size];
	for (i = 0; i < (int)_size; i++) {
		sub_expr_[i] = sub_expr[i];
	}

	_size = (int)config_expr.size();
	config_expr_ = new Expression*[_size];
	for (i = 0; i < (int)_size; i++) {
		config_expr_[i] = config_expr[i];
	}
	initialized = 1;
	return 1;
}

string::size_type Expression::fmp(string::size_type index)
{
	int not_matched = 1;
	for (string::size_type i = index; i < _expr.length(); i++) {
		if (_expr[i] == '(') not_matched++;
		if (_expr[i] == ')') not_matched--;
		if (!not_matched) return i;
	}
	return string::npos;
}


int Expression::eval_partial() {
	int i, j, ret_code=0;
	Vec3 *pV = 0;

	if (e_type == DX || e_type == DY || e_type == DZ || e_type == DM ||
		e_type == RX || e_type == RY || e_type == RZ) {
		Measure->eval(1);
		for (i = 0; i < 1; i++) { // i: 0 for pos, 1 for vel 2 for acc
			int _size = Measure->get_partial_size(i);
			if (x0y1z2 < 3) {
				for (j = 0; j < _size; j++) {
					pV = Measure->get_partial(i, j);
					if (pV) partial_[i][j] = (pV->v)[x0y1z2];
				}
			}
			else {
				for (j = 0; j < _size; j++) {
					partial_[i][j] = Measure->get_DM_partial(i, j);
				}
			}
		}
		ret_code = 1;
	}
	else if (e_type == DVX || e_type == DVY || e_type == DVZ || e_type == DVM ||
		e_type == RVX || e_type == RVY || e_type == RVZ) {
		Measure->eval(1);
		for (i = 0; i < 2; i++) { // i: 0 for pos, 1 for vel 2 for acc
			int _size = Measure->get_partial_size(i);
			if (x0y1z2 < 3) {
				for (j = 0; j < _size; j++) {
					pV = Measure->get_partial(i, j);
					if (pV) partial_[i][j] = (pV->v)[x0y1z2];
				}
			}
			else {
				for (j = 0; j < _size; j++) {
					partial_[i][j] = Measure->get_DM_partial(i, j);
				}
			}
		}
		ret_code = 1;
	}
	else if (e_type == JOINTD || e_type == JOINTV || e_type == JOINTA) {
		for (j = 0; j < 3; j++) {
			double tempd = jointMeasure->get_partial(j);
			partial_[j][0] = tempd;
		}
	}
	else if (config_expr.size() == 0) {
		ret_code = 0;
	}
	else {
		int jj, _size, kk, tempInt;
		int counter[3];
		unsigned ii;
		for (ii = 0; ii < config_expr.size(); ii++) {
			config_expr_[ii]->eval_partial();
		}
		counter[0] = counter[1] = counter[2] = 0;
		for (ii = 0; ii < config_expr.size(); ii++) {
			Expression* this_expr;
			Expression* that_expr = config_expr_[ii];
			double that_value = that_expr->eval();
			for (jj = 0; jj < 2; jj++) {
				_size = (int)that_expr->get_partial_size(jj);
				for (kk = 0; kk < _size; kk++) {
					stack<double> temp_stack;
					stack<int> temp_flags;
					int iop1, iop2;
					double value_, op1, op2, value__, str_value__;
					vector<int>::iterator iter = op_stack.begin();
					vector<int>::iterator _end = op_stack.end();
					while (iter != _end) {
						int flag = 1;
						tempInt = *iter;
						//an operand 
						if (tempInt < 10000) {
							this_expr = sub_expr_[tempInt];
							if (this_expr != that_expr) {
								value_ = this_expr->eval(); flag = 0;
							}
							else {
								value_ = this_expr->get_partial(jj, kk); flag = 1;
							}
							temp_stack.push(value_);
							temp_flags.push(flag);
						}
						else if (tempInt > 10000) {
							//an operator
							op1 = temp_stack.top();
							iop1 = temp_flags.top();
							temp_stack.pop();
							temp_flags.pop();
							if (temp_stack.empty() || temp_flags.empty()) {
								op2 = 0; iop2 = 0;
							}
							else {
								op2 = temp_stack.top();
								iop2 = temp_flags.top();
								temp_flags.pop();
								temp_stack.pop();
							}
							if ((tempInt - 10000) == '+') {
								value_ = op2 * (double)iop2 + op1 * (double)iop1;
							}
							else if ((tempInt - 10000) == '-') {
								value_ = op2 * (double)iop2 - op1 * (double)iop1;
							}
							else if ((tempInt - 10000) == '*') {
								value_ = op2 * op1;
							}
							else if ((tempInt - 10000) == '/') {
								if (iop1 == 0 && iop2 == 0) {
									value_ = op2 / op1;
								}
								else if (iop1 == 0 && iop2 == 1) {
									value_ = op2 / op1;
								}
								else if (iop1 == 1) {
									value_ = op2 * op1 / (that_value*that_value);
								}
							}
							else if ((tempInt - 10000) == '^') {
								if (iop1 == 1) {
									value_ = pow(op2, that_value)*op1;
								}
								else {
									value_ = pow(that_value, op1 - 1.0)*op2;
								}
							}
							temp_stack.push(value_);
							temp_flags.push(1);
						}
						iter++;
					}
					str_value__ = temp_stack.top();
					switch (e_type)
					{
					case SIN:
						value__ = cos(str_value)*str_value__; break;
					case COS:
						value__ = -sin(str_value)*str_value__; break;
					case EXP:
						value__ = exp(str_value - 1)*str_value__; break;
					case LOG:
						value__ = 1.0 / str_value * str_value__; break;
					case TAN:
						value__ = 1.0 / (cos(str_value)*cos(str_value))*str_value__; break;
					case SQRT:
						value__ = .5 / sqrt(str_value)*str_value__; break;
					default:
						value__ = str_value__;
						break;
					}
					partial_[jj][counter[jj]++] = value__;
				}
			}
		}
		ret_code = 1;
	}
	return ret_code;
}

double Expression::eval(int _partial)
{
	//if(is_evaluated() && !_partial) return value;
	int tempInt;
	stack<double> temp_stack;
	double value_, op1, op2;
	if (initialized == 0)
		init();

	if (e_type == PI) {
		value = pi;
		return pi;
	}
	else if (e_type == TIME) {
		value = pSys->time();
		return value;
	}
	else if (e_type == CONSTANT) {
		value = atof(_expr.c_str());
		return value;
	}
	else if (e_type == DX || e_type == DY || e_type == DZ || e_type == DM ||
		e_type == RX || e_type == RY || e_type == RZ) {
		switch (e_type) {
		case DX:
		case RX: tempInt = 0; break;
		case DY:
		case RY: tempInt = 1; break;
		case DZ:
		case RZ: tempInt = 2; break;
		default: tempInt = 3; break;
		}
		Measure->eval(_partial);
		value = Measure->get_val()[tempInt];
		return value;
	}
	else if (e_type == JOINTD) {
		jointMeasure->eval();
		value = jointMeasure->value();
		return value;

	}
	else if (e_type == JOINTV) {
		jointMeasure->eval();
		value = jointMeasure->value(1);
		return value;
	}
	else if (e_type == JOINTA) {
		jointMeasure->eval();
		value = jointMeasure->value(2);
		return value;

	}
	//for(unsigned i=0; i<op_stack.size(); i++) {
	vector<int>::iterator iter = op_stack.begin();
	vector<int>::iterator _end = op_stack.end();
	while (iter != _end) {
		//an operand 
		tempInt = *iter;
		if (tempInt < 10000) {
			value_ = sub_expr_[tempInt]->eval(_partial);
			temp_stack.push(value_);
		}
		else {
			//an operator
			//if(tempInt > 10000) {
			op1 = temp_stack.top();
			temp_stack.pop();
			if (temp_stack.empty()) {
				op2 = 0.0;
			}
			else {
				op2 = temp_stack.top();
				temp_stack.pop();
			}
			if ((tempInt - 10000) == '+') {
				value_ = op2 + op1;
			}
			else if ((tempInt - 10000) == '-') {
				value_ = op2 - op1;
			}
			else if ((tempInt - 10000) == '*') {
				value_ = op2 * op1;
			}
			else if ((tempInt - 10000) == '/') {
				value_ = op2 / op1;
			} if ((tempInt - 10000) == '^')
				value_ = pow(op2, op1);
			temp_stack.push(value_);
		}
		iter++;
	}
	str_value = temp_stack.top();
	switch (e_type)
	{
	case SIN:
		value = sin(str_value); break;
	case COS:
		value = cos(str_value); break;
	case EXP:
		value = exp(str_value); break;
	case LOG:
		value = log(str_value); break;
	case TAN:
		value = tan(str_value); break;
	case SQRT:
		value = sqrt(str_value); break;
	default:
		value = str_value;
		break;
	}
	evaled = 1;
	if (_partial) {
		eval_partial();
	}
	return value;
}

double Expression::eval_wrt_time() {
	//if (devaled) return dvalue;
	stack<double> temp_stack;
	double dvalue_, op1, op2, dop1, dop2;
	double value_;
	//double time = pSys->time();
	if (initialized == 0)
		init();

	if (e_type == TIME) {
		dvalue = 1.0;
		return 1.0;
	}
	else if (e_type == DX || e_type == DY || e_type == DZ ||
		e_type == RX || e_type == RY || e_type == RZ || e_type == PI || e_type == CONSTANT) {
		dvalue = 0.0;
		return 0.0;
	}
	vector<int>::iterator iter = op_stack.begin();
	vector<int>::iterator _end = op_stack.end();
	int tempInt;
	while (iter != _end) {
		tempInt = *iter;
		if (tempInt < 10000) {
			value_ = sub_expr_[tempInt]->val();
			dvalue_ = sub_expr_[tempInt]->eval_wrt_time();
			temp_stack.push(value_);
			temp_stack.push(dvalue_);
		}
		else {
			//if(tempInt > 10000) {
			dop1 = temp_stack.top();
			temp_stack.pop();
			op1 = temp_stack.top();
			temp_stack.pop();
			if (temp_stack.empty()) {
				op2 = 0.0;
				dop2 = 0.0;
			}
			else {
				dop2 = temp_stack.top();
				temp_stack.pop();
				op2 = temp_stack.top();
				temp_stack.pop();
			}
			if ((tempInt - 10000) == '+') {
				dvalue_ = dop1 + dop2;
				value_ = op1 + op2;
			}
			else if ((tempInt - 10000) == '-') {
				dvalue_ = dop2 - dop1;
				value_ = op2 - op1;
			}
			else if ((tempInt - 10000) == '*') {
				dvalue_ = dop2 * op1 + op2 * dop1;
				value_ = op2 * op1;
			}
			else if ((tempInt - 10000) == '/') {
				dvalue_ = (dop2*op1 - dop1 * op2) / (op1*op1);
				value_ = op2 / op1;
			}
			else if ((tempInt - 10000) == '^') {
				dvalue_ = op1 * pow(op2, op1 - 1)*dop2;// +pow(op2, op1)*log(op2)*dop1;
				value_ = pow(op2, op1);
			}
			temp_stack.push(value_);
			temp_stack.push(dvalue_);
		}
		iter++;
	}
	str_dvalue = temp_stack.top();
	temp_stack.pop();
	str_value = temp_stack.top();
	switch (e_type)
	{
	case SIN:
		dvalue = cos(str_value)*str_dvalue; break;
	case COS:
		dvalue = -sin(str_value)*str_dvalue; break;
	case EXP:
		dvalue = exp(str_value - 1)*str_dvalue; break;
	case LOG:
		dvalue = 1.0 / str_value * str_dvalue; break;
	case TAN:
		dvalue = 1.0 / (cos(str_value)*cos(str_value))*str_dvalue; break;
	case SQRT:
		dvalue = .5 / sqrt(str_value)*str_dvalue; break;
	default:
		dvalue = str_dvalue;
		break;
	}
	devaled = 1;
	return dvalue;
}

double Expression::eval_wrt_time_2() {
	//if(ddevaled) return ddvalue;
	stack<double> temp_stack;
	double dvalue_, op1, op2, dop1, dop2, ddop1, ddop2;
	double value_;
	double ddvalue_;
	//double time = pSys->time();
	if (initialized == 0)
		init();

	if (e_type == DX || e_type == DY || e_type == DZ || e_type == CONSTANT ||
		e_type == RX || e_type == RY || e_type == RZ || e_type == PI || e_type == TIME) {
		ddvalue = 0.0;
		return 0.0;
	}
	int tempInt;
	vector<int>::iterator iter = op_stack.begin();
	vector<int>::iterator _end = op_stack.end();
	while (iter != _end) {
		tempInt = *iter;
		if (tempInt < 10000) {
			value_ = sub_expr_[tempInt]->val();
			dvalue_ = sub_expr_[tempInt]->dval();
			ddvalue_ = sub_expr_[tempInt]->eval_wrt_time_2();
			temp_stack.push(value_);
			temp_stack.push(dvalue_);
			temp_stack.push(ddvalue_);
		}
		else {
			//if(tempInt > 10000) {
			ddop1 = temp_stack.top();
			temp_stack.pop();
			dop1 = temp_stack.top();
			temp_stack.pop();
			op1 = temp_stack.top();
			temp_stack.pop();
			if (temp_stack.empty()) {
				op2 = 0.0;
				dop2 = 0.0;
				ddop2 = 0.0;
			}
			else {
				ddop2 = temp_stack.top();
				temp_stack.pop();
				dop2 = temp_stack.top();
				temp_stack.pop();
				op2 = temp_stack.top();
				temp_stack.pop();
			}
			if ((tempInt - 10000) == '+') {
				dvalue_ = dop1 + dop2;
				value_ = op1 + op2;
				ddvalue_ = ddop1 + ddop2;
			}
			else if ((tempInt - 10000) == '-') {
				dvalue_ = dop2 - dop1;
				value_ = op2 - op1;
				ddvalue_ = ddop2 - ddop1;
			}
			else if ((tempInt - 10000) == '*') {
				dvalue_ = dop2 * op1 + op2 * dop1;
				value_ = op2 * op1;
				ddvalue_ = ddop2 * op1 + dop2 * dop1*2.0 + op2 * ddop1;
			}
			else if ((tempInt - 10000) == '/') {
				dvalue_ = (dop2*op1 - dop1 * op2) / (op1*op1);
				value_ = op2 / op1;
				ddvalue_ = ((ddop2*op1 - op2 * ddop1)*op1 - 2 * dop1*(dop2*op1 - dop1 * op2)) / (op1*op1*op1);
			}
			else if ((tempInt - 10000) == '^') {
				dvalue_ = op1 * pow(op2, op1 - 1)*dop2/*+pow(op2,op1)*log(op2)*dop1*/;
				value_ = pow(op2, op1);
				ddvalue = op1 * pow(op2, op1 - 2)*dop2*dop2 + op1 * pow(op2, op1 - 1)*ddop2;
			}
			temp_stack.push(value_);
			temp_stack.push(dvalue_);
			temp_stack.push(ddvalue_);
		}
		iter++;
	}
	str_ddvalue = temp_stack.top();
	temp_stack.pop();
	str_dvalue = temp_stack.top();
	temp_stack.pop();
	str_value = temp_stack.top();

	switch (e_type)
	{
	case SIN:
		ddvalue = cos(str_value)*str_ddvalue - sin(str_value)*str_dvalue*str_dvalue; break;
	case COS:
		ddvalue = -sin(str_value)*str_ddvalue - cos(str_dvalue)*str_dvalue*str_dvalue; break;
	case EXP:
		ddvalue = exp(str_value - 1)*str_ddvalue + exp(str_value - 2)*str_dvalue*str_dvalue; break;
	case LOG:
		ddvalue = 1.0 / str_value * str_ddvalue - 1.0 / (str_value*str_value)*str_dvalue*str_dvalue; break;
	case TAN:
		value_ = cos(str_value)*cos(str_value);
		ddvalue = 1.0 / (value_)*str_ddvalue - 2.0 / (value_*cos(str_value))*str_dvalue*str_dvalue; break;
	case SQRT:
		ddvalue = .5 / sqrt(str_value)*str_ddvalue - .25 / pow(str_value, 1.5)*str_dvalue*str_dvalue; break;
	default:
		ddvalue = str_ddvalue;
		break;
	}
	ddevaled = 1;
	return ddvalue;
}
