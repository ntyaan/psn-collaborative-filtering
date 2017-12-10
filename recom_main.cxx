#include "recom.h"
constexpr int row=2000;
constexpr int col=4803;
constexpr int MAX=1000;

int main(){
  Recom test(row,col);
  test.input_data();
  test.input_users();
  test.input_games();
  test.input_games_url();
  test.input_images();
  test.input_average();
  test.input_count();
  test.input_platform();
  test.input_active_user("mmilkkr");
  test.pearsonsim();
  test.pearsonpred2();
  //結果出力，row人中30人以上,平均取得率の差が20以上
  test.print_result(30,20.0);
  //似たユーザ出力
  test.print_simuser();
  return 0;
}
