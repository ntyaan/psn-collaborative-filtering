#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cfloat>
#include <cmath>
#include <Eigen/Dense>
#include <Eigen/LU>
#include <Eigen/Sparse>
#include <vector>

using namespace Eigen;
constexpr int users = 2000;
constexpr int items = 4803;
int main() {
  MatrixXi mat=MatrixXi::Zero(users, items);
  int col, value;
  for(int i=0;i<users;i++){
    std::string file="psn_game/user/"+std::to_string(i)+".txt";
    std::ifstream ifs(file);
    while(ifs){
      ifs>>col>>value;
      mat(i,col)=value;
    }
    ifs.close();
  }
  std::ofstream ofs_("psn_game/data/data.txt");
  int x=0;
  for(int i=0;i<users;i++){
    for(int j=0;j<items;j++){
      if(mat(i,j)>0){
	ofs_<<i<<"\t"<<j<<"\t"<<mat(i,j)<<std::endl;
	x++;
      }
    }
  }
  std::string str ="sed -i '1s/^/"+std::to_string(x)+"\\n/' psn_game/data/data.txt";
  system(str.c_str());
  return 0;
}
