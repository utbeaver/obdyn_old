
#define a(i,j) a[(i)*(n)+(j)]
#define h(i,j) h[(i)*m+(j)]
#include <algorithm>
#include <functional>
#include <iostream>
#include <stack>
#include "od_topo.h"
#include "od_element.h"

void   obdyn_topology::process(vector<int>& a,
	vector<int>& h, int m, int n, vector<int>& unconnected)
{
	// m -- the number of joints
	//n -- the number of body
	int i, j, k, jmax, temp;
	int* permu = new int[(m > n) ? m : n];
	int no_dangling = 1;

	for (i = 0; i < ((m > n) ? m : n); i++) {permu[i] = i;}
	for (i = 0; i < n; i++) {unconnected[i] = 1;}
	for (i = 0; i < n; i++) {
		if (i >= m) {
			unconnected[i] = -1;
			no_dangling = 0;
			continue;
		}
		/*for each column, find its first non-zero element*/
		jmax = i;
		if (!(a(i, i))) {
			jmax = -1;
			for (j = i + 1; j < m; j++) {
				if (a(j, i)) {
					jmax = j;
					break;
				}
			}
		}
		/* if there is no non-zero entries down the column, put this column to the end*/
		/*this element is a independent body of a dangling loop*/
		if (jmax == -1) {
			unconnected[i] = -1;
			no_dangling = 0;
			continue;
		}
		/*switch lines if necesary*/
		if (jmax != i) {
			temp = permu[i];
			permu[i] = permu[jmax];
			permu[jmax] = temp;
			for (j = 0; j < n; j++) {
				temp = a(i, j);
				a(i, j) = a(jmax, j);
				a(jmax, j) = temp;
			}
			for (j = 0; j < m; j++) {
				temp = h(i, j);
				h(i, j) = h(jmax, j);
				h(jmax, j) = temp;
			}
		}
		/*remove the elements in ith column except the a(i,i)*/
		for (j = 0; j < m; j++) {
			if (j == i) continue;
			if (a(j, i) != 0) {
				temp = a(i, i)*a(j, i);
				for (k = 0; k < n; k++) {
					a(j, k) -= temp * a(i, k);
				}
				for (k = 0; k < m; k++) {
					h(j, k) -= temp * h(i, k);
				}
			}
		}
	}
	for (i = 0; i < n; i++) {
		if (a(i, i) == -1) {
			a(i, i) = 1;
			for (j = 0; j < m; j++) h(i, j) = -h(i, j);
		}
	}
	if (no_dangling) {
		for (i = 0; i < ((m > n) ? m : n); i++) {
			unconnected[i] = permu[i];
		}
	}
	DELARY(permu)
}

void   obdyn_topology::process_for_branch
(vector<int>& h, vector<int>& b_perm, vector<int>& j_perm, int m)
{
	int i, j, sum, index, temp_int;
	int current_nzs, last_nzs;
	vector<tuple_> vec_tu;
	stack<int> branch_base;

	last_nzs = m + 1;
	branch_base.push(m);
	vec_tu.resize(m);
	if (b_perm.size() != (unsigned)m) { b_perm.resize(m); }
	if (j_perm.size() != (unsigned)m) { j_perm.resize(m); }
	for (i = 0; i < m; i++) { b_perm[i] = i; j_perm[i] = i; }
	for (index = 0; index < m; index++) {
		current_nzs = 0;
		if (last_nzs == 1) {
			branch_base.pop();
		}
		//couning the NZs of current column;
		temp_int = branch_base.top();
		for (i = 0; i < temp_int; i++) {
			if (h(b_perm[i], j_perm[index])) current_nzs++;
		}
		if (current_nzs < last_nzs - 1 || last_nzs == 1) {
			//in case of sub-branch, push the current base deeper in the stack and 
			//find new one by conducting the col permutation
			//find the col with most NZs and move it to the left
			for (j = index; j < m; j++) {
				sum = 0;
				for (i = index; i < branch_base.top(); i++) { sum -= abs(h(b_perm[i], j_perm[j])); }
				vec_tu[j].set(sum, j_perm[j]);
			}
			sort(vec_tu.begin() + index, vec_tu.end());
			for (i = index; i < m; i++) { j_perm[i] = vec_tu[i].index(); }
			current_nzs = 0;
			for_each(vec_tu.begin() + index, vec_tu.end(), mem_fun_ref(&tuple_::init));
			//moving the NZ row upward
			for (i = index; i < m; i++) vec_tu[i].set(m + 2, b_perm[i]);
			for (i = index; i < branch_base.top(); i++) {
				sum = abs(h(b_perm[i], j_perm[index]));
				if (!sum) sum = m + 1;
				else current_nzs++;
				vec_tu[i].set(sum, b_perm[i]);
			}
			sort(vec_tu.begin() + index, vec_tu.end());
			for (i = index; i < m; i++) { b_perm[i] = vec_tu[i].index(); }
			temp_int = index + current_nzs;
			if (temp_int < branch_base.top()) {
				branch_base.push(index + current_nzs);
			}
		}
		//conduct  col and row permutations
		for (j = index; j < m; j++) {
			sum = 0;
			for (i = index; i < branch_base.top(); i++) {
				sum += abs(h(b_perm[i], j_perm[j]));
			}
			sum = -sum; vec_tu[j].set(sum, j_perm[j]);
		}
		sort(vec_tu.begin() + index, vec_tu.end());
		for (j = index; j < m; j++) { j_perm[j] = vec_tu[j].index(); }
		for_each(vec_tu.begin() + index, vec_tu.end(), mem_fun_ref(&tuple_::init));
		for (i = index; i < m; i++) vec_tu[i].set(m + 2, i);
		for (i = index; i < branch_base.top(); i++) {
			sum = 0;
			for (j = index; j < branch_base.top(); j++) { sum += abs(h(b_perm[i], j_perm[j])); }
			if (!sum) sum = m + 1;
			vec_tu[i].set(sum, b_perm[i]);
		}
		sort(vec_tu.begin() + index, vec_tu.end());
		for (i = index; i < branch_base.top(); i++) { b_perm[i] = vec_tu[i].index(); }
		last_nzs = current_nzs;
	}
}


/*
extern "C" {
static obdyn_topology Topo;
void od_set_num_joint(int nj) {Topo.set_num_joint(nj);}
void od_set_num_body(int nb) {Topo.set_num_body(nb);}
void od_set_incidence(vector<int>& inp){Topo.set_incidence(inp);}
void od_set_relevence(vector<int>& inp){Topo.set_relevence(inp);}
void od_process_topo(){Topo.process_topo();}
int od_get_num_body() { return Topo.get_num_body();}
int od_get_num_joint() {return Topo.get_num_joint();}
void od_get_incidence(vector<int>& x) {Topo.get_incidence(x);}
void od_get_relevence(vector<int>& out) {Topo.get_relevence(out);}
void od_get_permutation_vec(vector<int>& out) {Topo.get_permutation_vector(out);}
}

*/
