// od_parJ_parqi.cpp: implementation of the od_parJ_parqi class.
//
//////////////////////////////////////////////////////////////////////

#include "od_parJ_parqi.h"
#include "Mat33.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*

od_parJ_parqi::od_parJ_parqi(int rows, int cols)
{
  height=rows; 
  width=cols; 
  initialized=0;
  num_nz=0;
  Rii_idx.resize(height);
  fill(Rii_idx.begin(), Rii_idx.end(), -1);
  start_index_of_row.resize(width);
  fill(start_index_of_row.begin(), start_index_of_row.end(), -1);
  start_index_of_col.resize(height);
  fill(start_index_of_col.begin(), start_index_of_col.end(), -1);
  zero = 0;
}

od_parJ_parqi::~od_parJ_parqi()
{
  
}


void od_parJ_parqi::init(int lc, int rc, int up, int br, vector<int>& depends, 
                         vector<unsigned>& if_rotation, vector<int>& _prev)
{
  if (initialized) return;
  if (lc == 0 && rc == 0 && up == 0 && br == 0) {
    zero = 1;
  }
  int i,j;
  left_col =lc; right_col=rc; up_row=up; bottom_row=br;
  num_nz = 0;
  if_rot.resize(0);
  copy(if_rotation.begin(), if_rotation.end(), back_inserter(if_rot));
  prev.resize(0);
  copy(_prev.begin(), _prev.end(), back_inserter(prev));
  for(i=up_row; i<bottom_row; i++) {
    start_index_of_row[i]=(num_nz);
    for(j=left_col; j<right_col; j++) {
      if(depends[i*width+j] != 0) {
        if (depends[i*width+j] > 0) { 
          col_index.push_back(j);
        } else {
          if(Rii_idx[i] == -1) Rii_idx[i] = num_nz; 
          col_index.push_back(j);
        }
        num_nz++;
      }
    }
  }
  start_index_of_row[i]=(num_nz);
  num_nz = 0;
  for(j=left_col; j<right_col; j++) {
    start_index_of_col[j]=(num_nz);
    for(i=up_row; i<bottom_row; i++) {
      if(depends[i*width+j] != 0) {
        num_nz++;
        row_index.push_back(i);        
      }      
    }
  }
  start_index_of_col[j]=(num_nz);
  initialized=1;
}

Vec3* od_parJ_parqi::multiply(Vec3* output, double* q, Vec3* Ej, Vec3* Ri, Vec3* Rii)
{
  if(zero) return output;
  int i,j, col;
  double temp_d[9];
  double* pd = temp_d;
  double* pd1 = temp_d+3;
  double* pd2 = temp_d+6;
  int start_col_index;
  int next_start_col_index;
  int counter=0;
  vector<Vec3*> sum_ej;
  Vec3* pV;
  
  
  for(i=up_row; i<bottom_row; i++) {
    pV = new Vec3(); pV->init();
    sum_ej.push_back(pV);
    start_col_index = start_index_of_row[i];
    next_start_col_index=start_index_of_row[i+1];
    fill(pd, pd+9, 0.0);
    for(j=start_col_index; j<Rii_idx[i]; j++) {
      col = col_index[j];
      if(if_rot[col]) {
        pd = Ej[col].multiply_by(pd, q[col]);
      } else { 
        pd2 = Ej[col].multiply_by(pd2, q[col]);
      }
    }
    pd = Ri[i]^pd;
    for(j=Rii_idx[i]; j<next_start_col_index; j++) {
      col = col_index[j];
      if(if_rot[col]) pd1 = Ej[col].multiply_by(pd1, q[col]);
      else pd2 = Ej[col].multiply_by(pd2, q[col]);
    }
    pd1 = Rii[i]^pd1;
    output[i] += pd2;
    output[i] -= pd1;
    output[i] -= pd;
    
  }
  for(i=0; i<(int)sum_ej.size(); i++) delete sum_ej[i];
  return output;
}

double* od_parJ_parqi::multiply(double* output, Vec3* q, Vec3* Ej, Vec3* Ri, Vec3* Rii) {
  if (zero) return output;
  int i, j, k, row;
  int _height = bottom_row-up_row;
  Vec3* RiXqi=new Vec3[_height];
  Vec3* RiiXqi = new Vec3[_height]; 
  Vec3 vecTemp3;
  double _val;
  if(!Rii) Rii = Ri;
  for(i=0; i<_height; i++)  {
    j=up_row+i;
    RiXqi[i]= q[j];
    RiXqi[i] = Ri[j]^RiXqi[i];
    RiiXqi[i]=q[j];
    RiiXqi[i] = Rii[j]^RiiXqi[i];
  }
  for(j=left_col; j<right_col; j++) {
    vecTemp3.init();
    int start_row_index = start_index_of_col[j];
    int next_start_row_index = start_index_of_col[j+1];
    row = row_index[start_row_index];
    if(if_rot[j]) vecTemp3 += RiiXqi[row-up_row];
    else vecTemp3 += q[j];
    start_row_index++;
    for(k=start_row_index; k<next_start_row_index; k++) {
      row = row_index[k];
      vecTemp3 += RiXqi[row-up_row];
    }
    _val = Ej[j]*vecTemp3;
    output[j] += _val;
  }
  DELARY(RiXqi)
  DELARY(RiiXqi)
  return output;
}

Vec3* od_parJ_parqi::multiply(Vec3* output, double* q, Vec3* Ej) {
  if (zero) return output;
  double temp_d[3];
  double* pd=temp_d;
  int start_row_index, next_start_row_index, k, j, row;
  for( j=left_col; j<right_col; j++) {
    start_row_index = start_index_of_col[j];
    next_start_row_index = start_index_of_col[j+1];
    for( k=start_row_index; k<next_start_row_index; k++) {
      row = row_index[k];
      pd = Ej[row].multiply_by(pd, q[j]);
      output[row] += pd;
    }
  }
  return output;
}

double* od_parJ_parqi::multiply(double* output, Vec3* q, Vec3* Ej, int constraint_idx) {
  if (zero) return output;
  Vec3 vecTemp;
  double temp_d;
  int j, k, start_row_index, next_start_row_index, row;
  for (j=left_col; j<right_col;  j++) {
    if(!if_rot[j]) continue; //No contribution from translation joint
    vecTemp.init();
    if(constraint_idx != -1) {
      start_row_index = constraint_idx;
      next_start_row_index = constraint_idx+1;
    } else {
      start_row_index = start_index_of_col[j];
      next_start_row_index = start_index_of_col[j+1];
    }
    for(k=start_row_index; k<next_start_row_index; k++) {
      row = row_index[k];
      vecTemp += q[row];
    }
    temp_d = vecTemp*Ej[j];
    output[j] += temp_d;
  }
  return output;
}
*/