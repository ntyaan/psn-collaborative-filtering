#include"recom.h"

Recom::Recom(int user, int item){
  Data.resize(user, item);
  Users.resize(user);
  Games.resize(item);
  Games_url.resize(item);
  Images.resize(item);
  Average.resize(item);
  Count.resize(item);
  Platform.resize(item, 3);
  Similarities.resize(user);
  Prediction.resize(item);
  for(int ell=0;ell<item;ell++)
    Prediction[ell]=0.0;
  Activeuser.resize(item);
}

Recom::~Recom(){
}

int Recom::user_number(){
  return Users.size();
}

int Recom::item_number(){
  return Games.size();
}

void Recom::input_data(){
  std::string filename = "data/data.txt";
  int row,col,value,size;
  std::ifstream ifs(filename);
  if(ifs.fail()){
    std::cerr << "can't find " << filename <<std::endl;
    std::exit(1);
  }
  ifs>>size;
  std::vector<Triplet<double> > t(size);
  for(int i=0;i<size;i++){
    ifs>>row>>col>>value;
    t[i]=Triplet<double>(row, col, value);
  }
  Data.setFromTriplets(t.begin(),t.end());
  ifs.close();
  return;
}

void Recom::input_users(){
  std::string filename = "data/users.txt";
  std::string str;
  std::ifstream ifs(filename);
  if(ifs.fail()){
    std::cerr << "can't find " << filename <<std::endl;
    std::exit(1);
  }
  for(int i=0;i<user_number();i++){
    ifs>>str;
    Users[i]=str;
  }
  return;
}

void Recom::input_games(){
  std::string filename = "data/games.txt";
  std::string str;
  std::ifstream ifs(filename);
  if(ifs.fail()){
    std::cerr << "can't find " << filename <<std::endl;
    std::exit(1);
  }
  for(int i=0;i<item_number();i++){
    getline(ifs, str);
    Games[i]=str;
  }
  return;
}

void Recom::input_games_url(){
  std::string filename = "data/games_url.txt";
  std::string str;
  std::ifstream ifs(filename);
  if(ifs.fail()){
    std::cerr << "can't find " << filename <<std::endl;
    std::exit(1);
  }
  for(int i=0;i<item_number();i++){
    getline(ifs, str);
    Games_url[i]=str;
  }
  return;
}

void Recom::input_images(){
  std::string filename = "data/images.txt";
  std::string str;
  std::ifstream ifs(filename);
  if(ifs.fail()){
    std::cerr << "can't find " << filename <<std::endl;
    std::exit(1);
  }
  for(int i=0;i<item_number();i++){
    getline(ifs, str);
    Images[i]=str;
  }
  return;
}

void Recom::input_average(){
  std::string filename = "data/average.txt";
  double value;
  std::ifstream ifs(filename);
  if(ifs.fail()){
    std::cerr << "can't find " << filename <<std::endl;
    std::exit(1);
  }
  for(int i=0;i<item_number();i++){
    ifs>>value;
    Average[i]=value;
  }
  return;
}

void Recom::input_platform(){
  std::string filename = "data/platform.txt";
  int value;
  std::ifstream ifs(filename);
  if(ifs.fail()){
    std::cerr << "can't find " << filename <<std::endl;
    std::exit(1);
  }
  for(int i=0;i<item_number();i++){
    for(int j=0;j<3;j++){
      ifs>>value;
      Platform(i,j)=value;
    }
  }
  return;
}

void Recom::input_count(){
  std::string filename = "data/count.txt";
  int key;
  std::ifstream ifs(filename);
  if(ifs.fail()){
    std::cerr << "can't find " << filename <<std::endl;
    std::exit(1);
  }
  for(int i=0;i<item_number();i++){
    ifs>>key;
    Count[i]=key;
  }
  return;
}

void Recom::input_active_user(std::string actuser){
  std::string filename = "data/"+actuser+".txt";
  std::ifstream ifs(filename);
  if(ifs.fail()){
    std::cerr << "can't find " << filename <<std::endl;
    std::exit(1);
  }
  int col,value;
  while(ifs){
    ifs>>col>>value;
    Activeuser.coeffRef(col)=value;
  }
  return;
}

void Recom::pearsonsim(){
  for(int i=0;i<user_number();i++){
    double psum=0.0,sum1=0.0,sum2=0.0;
    double sum1sq=0.0,sum2sq=0.0;
    double hyokasu=0.0;
    //Eigen3のsparseVectorを使ってる
    for(SparseVector<double>::InnerIterator it(Activeuser);it;++it){
      //Eigen3のsparseMatrixを使ってる
      double user_value=Data.coeff(i,it.index());
      if(user_value>0 && it.value()>0){
	hyokasu+=1.0;
	psum+=user_value*it.value();
	sum1+=user_value;
	sum2+=it.value();
	sum1sq+=pow(user_value,2.0);
	sum2sq+=pow(it.value(),2.0);
      }
    }
    if(hyokasu>0){
      //分子の計算
      double numerator=psum-(sum1*sum2/hyokasu);
      //分母の計算
      double denominator=sqrt((sum1sq-pow(sum1,2.0)/hyokasu)
			      *(sum2sq-pow(sum2,2.0)/hyokasu));
      //分母が0のときの処理
      if(denominator==0)
	Similarities[i]=-1.0;
      else
	Similarities[i]=numerator/denominator;
    }
    //対象ユーザとアイテムが一つも被らなかったときの処理(hyokasu=0)
    else
      Similarities[i]=-1.0;
  }
  return;
}

void Recom::pearsonpred2(){
  double num=0.0, den=0.0;
  //Eigen3のSparseVectorを使ってる
  for(SparseVector<double>::InnerIterator it(Activeuser);it;++it){
    if(it.value()>0){
      num+=it.value();
      den++;
    }
  }
  //対象ユーザの既評価平均値
  double actuseraverage=num/den;
  for(int ell=0;ell<item_number();ell++){
    //分子，分母
    double numerator=0.0,denominator=0.0;
    for(int k=0;k<user_number();k++){
      //Eigen3のsparseMatrixを使ってる
      double uservalue=Data.coeff(k,ell);
      if(Similarities[k]>0.0 && uservalue>0.0){
	numerator+=Similarities[k]
	  *(uservalue-user_average(k));
	denominator+=Similarities[k];
      }
    }
    //分母が0のときの処理
    if(denominator==0)
      Prediction[ell]+=actuseraverage;
    else {
      Prediction[ell]+=actuseraverage
	+numerator/denominator;
    }
  }
  return;
}

//ユーザの既評価平均値を返す
double Recom::user_average(int index){
  double result=0.0;
  double hyoka=0.0;
  for(SparseMatrix<double, RowMajor>
	::InnerIterator it(Data,index);it;++it){
    result+=it.value();
    hyoka++;
  }
  return result/hyoka;
}

void Recom::print_result(int arg1, double arg2){
  double premax=100;
  int top=1;
  std::cout<<"| ranking | 予測値 | 平均取得率 |"
	   <<"所持数| (ps4, vita) | 画像 | ゲーム名 |"
	   <<std::endl;
  std::cout<<"|:---|:---|:----|:---|:----|:---|:---|"<<std::endl;
  for(int i=0;i<item_number();i++){
    int index=0;
    double max=0;
    for(int ell=0;ell<item_number();ell++){
      if(Prediction[ell]<premax&&Prediction[ell]>max){
	max=Prediction[ell];
	index=ell;
      }
    }
    premax=max;
    double sub = Prediction[index]-Average[index];
    if(!(!Platform(index,0)&&Platform(index,1)&&!Platform(index,2))
       &&Count[index]>=arg1&&sub>=arg2){
      std::cout<<"|**"<<top<<"**| "<<Prediction[index]
	       <<"|"<<Average[index]<<"|";
      std::cout<<Count[index]<<"|";
      if(Platform(index,0) && Platform(index,1) && Platform(index,2) )
	std::cout<<"(ps4, ps3, vita)| ";
      else if(Platform(index,0) && Platform(index,1) && !Platform(index,2) )
	std::cout<<"(ps4, ps3)| ";
      else if(Platform(index,0) && !Platform(index,1) && Platform(index,2) )
	std::cout<<"(ps4, vita)| ";
      else if(!Platform(index,0) && Platform(index,1) && Platform(index,2) )
	std::cout<<"(ps3, vita)| ";
      else if(Platform(index,0) && !Platform(index,1) && !Platform(index,2) )
	std::cout<<"(ps4)| ";
      else if(!Platform(index,0) && !Platform(index,1) && Platform(index,2) )
	std::cout<<"(vita)| ";
      else
	std::cout<<"()| ";
      std::cout<<"!["<<Games[index]<<"]("<<Images[index]<<")|"
	       <<"["<<Games[index]<<"]("<<Games_url[index]<<")|"
	       <<std::endl;
      top++;
    }
    if(Prediction[index]<0.0)
      break;
  }
  return;
}

void Recom::print_simuser(){
  std::vector<int> index(user_number());
  for(int i=0;i<user_number();i++)
    index[i]=i;
  //降順にソート
  int tmp;
  double value;
  for(int i=0;i<user_number();++i){
    for(int j=i+1;j<user_number();++j){
      if(Similarities[i]<Similarities[j]){
	value=Similarities[i];
	Similarities[i]=Similarities[j];
	Similarities[j]=value;
	tmp=index[i];
	index[i]=index[j];
	index[j]=tmp;
      }
    }
  }
  std::cout<<"| ranking | ユーザ名 | 類似度 | "<<std::endl;
  std::cout<<"|:---|:---|:----|"<<std::endl;
  for(int i=0;i<user_number();i++){
    if(Similarities[i]>0)
      std::cout<<"|**"<<i+1<<"**|["
	       <<Users[index[i]]<<"](https://psnprofiles.com/"
	       <<Users[index[i]]<<")|"<<Similarities[i]
	       <<"|"<<std::endl;
  }
  return;
}
