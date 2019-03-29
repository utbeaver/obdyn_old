#ifndef OD_TOPO_H
#define OD_TOPO_H


#include <vector>
#include <stdlib.h>
using namespace std;
#ifdef _WINDOWS
#define LINKUTILSDLL __declspec(dllexport)
//	template class LINKUTILSDLL std::vector<int>;
//from 
//http://support.microsoft.com/default.aspx?scid=http://support.microsoft.com:80/support/kb/articles/Q168/9/58.asp&NoWebContent=1
#endif
class  obdyn_topology {
private: 
  int num_of_joint;
  int num_of_body;
  vector<int> incidence;
  vector<int> relevence;
  vector<int> perm;
  void process(vector<int>&, vector<int>&, int m, int n, vector<int>&);
public:
  obdyn_topology(void) {
    num_of_joint=0;
    num_of_body=0;
  }
  ~obdyn_topology(void) {
  }
  void set_num_joint(int nj) {num_of_joint = nj;}
  void set_num_body(int nb) { num_of_body = nb; }
  
  void set_incidence(vector<int>& inp) {
    int temp_int = num_of_joint * num_of_body;
    perm.resize(num_of_body);
    incidence.resize(temp_int);
    for(int i=0; i<temp_int; i++) { incidence[i]=inp[i];}
  }
  void set_relevence(vector<int>& rel) {
    int temp_int = num_of_joint * num_of_joint;
    relevence.resize(temp_int);
    for(int i=0; i<temp_int; i++) relevence[i]=rel[i];
  }
  void process_topo() {
    perm.resize((num_of_joint>num_of_body)?num_of_joint:num_of_body);
    process(incidence, relevence, num_of_joint, num_of_body, perm );
  }
  
  void process_for_branch(vector<int>& h, vector<int>& b_perm, vector<int>& j_perm, int m );
  
  int get_num_body() { return num_of_body;}
  int get_num_joint() { return num_of_joint;}
  void get_incidence(vector<int>& outp) {
    int temp_int = 0;
    for(int i=0; i< num_of_joint; i++) {
      for(int j=0; j<num_of_body; j++) {
        outp[temp_int++] = incidence[i*num_of_body + j];
      }
    }
  } 
  void get_relevence(vector<int>& out) {
    //   int body_diff;
    int temp_int = 0;
    for(int i=0; i<num_of_joint; i++) {
      for(int j=0; j<num_of_joint; j++) {
        out[temp_int++] = relevence[i*num_of_joint+j];
      }
      
    }
  }
  
  void get_permutation_vector(vector<int>& out) { 
    for (unsigned i=0; i< perm.size(); i++) {
      out[i] = perm[i];
    }
  }
};

class tuple_ {
private:
  int _sum;
  int _index;
public:
  tuple_() {
    _sum = 0;
    _index = 0;
  }
  inline int init(void) {
    _sum = 0;
    _index = 0;
    return 1;
  }
  inline void set(int s, int i) {
    _sum = s;
    _index = i;
  }
  inline int index(void) const {return _index;}
  inline int sum(void) const {return _sum;}
  inline bool operator<(const tuple_& right) const {
    return _sum < right._sum;
  }
};
/*
extern "C" {
void od_set_num_joint(int nj);
void od_set_num_body(int nb);
void od_set_incidence(vector<int>& inp);
void od_set_relevence(vector<int>& inp);
void od_process_topo();
int od_get_num_body();
int od_get_num_joint();
void od_get_incidence(vector<int>& x);
void od_get_relevence(vector<int>& out); 
void od_get_permutation_vec(vector<int>& out); 
}
*/

#endif
