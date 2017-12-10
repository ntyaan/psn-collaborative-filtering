#include <iostream>
#include <fstream>
#include <Eigen/Dense>
#include <Eigen/Sparse>

using namespace Eigen;

#ifndef __RECOM__
#define __RECOM__

class Recom{
 private:
  //SparseMatrix でDataを定義
  SparseMatrix<double, RowMajor> Data;
  std::vector<std::string> Users;
  std::vector<std::string> Games;
  std::vector<std::string> Games_url;
  std::vector<std::string> Images;
  std::vector<double> Average;
  std::vector<int> Count;
  MatrixXi Platform;
  std::vector<double> Similarities;
  std::vector<double> Prediction;
  //SparseVectorで自分データを定義
  SparseVector<double> Activeuser;
  std::vector<int> Membership;
 public:
  //ユーザ数，アイテム数
  Recom(int, int);
  //デストラクタ
  ~Recom();
  //メンバシップ読み込み
  void input_membership(int);
  //ユーザ数を返す
  int user_number();
  //ゲーム数を返す
  int item_number();
  //データ読み込み
  void input_data();
  //ユーザ名読み込み
  void input_users();
  //ゲーム名読み込み
  void input_games();
  //ゲームURL読み込み
  void input_games_url();
  //ゲーム画像URL読み込み
  void input_images();
  //トロフィー平均取得率読み込み
  void input_average();
  //ゲーム所持数読み込み
  void input_count();
  //ゲームプラットフォーム読み込み
  void input_platform();
  //対象ユーザ読み込み
  void input_active_user(std::string);
  //ピアソン相関家数計算
  void pearsonsim();
  //予測値計算:GroupLens
  void pearsonpred2();
  //indexのユーザの既評価値平均を計算
  double user_average(int);
  //出力
  void print_result(int, double);
  //似たユーザ出力
  void print_simuser();
};
#endif
