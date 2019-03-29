// od_parJ_parqi.h: interface for the od_parJ_parqi class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OD_PARJ_PARQI_H__4007C904_821D_4F8C_93B7_B6DDEE6C18D2__INCLUDED_)
#define AFX_OD_PARJ_PARQI_H__4007C904_821D_4F8C_93B7_B6DDEE6C18D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>
using namespace std;
class Vec3;

/*class od_parJ_parqi  
{
private:
  int left_col, right_col, up_row, bottom_row;
  int num_nz;
  int height, width;
  vector<int> start_index_of_row;
  vector<int> start_index_of_col;
  vector<int> col_index;
  vector<int> row_index;
  int initialized;
  vector<int> Rii_idx;
  vector<int> if_rot;
  vector<int> prev;
  int zero;
public:
  od_parJ_parqi(int rows, int cols);
  void init(int lc, int rc, int up, int br, vector<int>& , vector<unsigned>&, vector<int>&  );
  void init(vector<int>&);
	virtual ~od_parJ_parqi();
  //for Jt q 
  Vec3* multiply(Vec3*, double*, Vec3*, Vec3*, Vec3*);
  //for Jt^t V
  double* multiply(double*, Vec3*, Vec3*, Vec3*, Vec3*);
  //for Jr q
  Vec3* multiply(Vec3*, double*, Vec3*);
  //for Jr^t V  -- if int !- -1, this function is used for Largangian multiplication
  double* multiply(double*, Vec3*, Vec3*, int=-1);
};*/

#endif // !defined(AFX_OD_PARJ_PARQI_H__4007C904_821D_4F8C_93B7_B6DDEE6C18D2__INCLUDED_)
