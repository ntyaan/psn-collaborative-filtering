# psn-collaborative-filtering

## Qiitaから移し

## 目標

ゲームの実績やトロフィーはプレイしたことがあるかどうかやどの程度プレイしたのかの指標となり，これらを使って推薦システムっぽいものを作れないかとずっと思っていました．

今回はプレイステーションのゲームを対象とします．

PSN非公式サイト([https://psnprofiles.com](https://psnprofiles.com/))から登録されたゲーム情報とトロフィー取得情報をユーザごとにスクレイピングし，強調フィルタリングで自分にとってのおすすめのゲームを表示することが目標です．

ちなみこのサイト一回登録されると自分のゲームプレイ情報が晒されることになるので注意が必要です．

今回は自分の情報以外にユーザのサイトをクロールして2000人の情報をスクレイピングしました．

<!--
過程が長くなるので先に結果のtop10を出しておきます．
僕の[アカウント](https://psnprofiles.com/mmilkkr)を対象のユーザとし，プレイ済みのゲームもあえて表示します．
予測値は実際自分がこのゲームをプレイしていたと仮定したときのトロフィー取得率と考えます．
この予測値が高い順に，予測値と平均取得率の差が20以上で，2000人中30人以上にプレイされたゲームを並べました\
<<<<<<< HEAD
ps3でゲームをしないのでプラットフォームがps3のみのゲームを省いています．
=======
ps3でゲームをしないのでプラットフォームがps3のみのゲームを省いています．-->

<!--
>>>>>>> refs/remotes/origin/master
| ranking | 予測値 | 平均取得率 |所持数| (ps4, vita) | 画像 | ゲーム名 |
|:---|:---|:----|:---|:----|:---|:---|
|**1**| 88.5148|61|55|(ps4)| ![nba-2k17-the-prelude](https://i.psnprofiles.com/games/bd0437/S1ef2a1.png)|[nba-2k17-the-prelude](https://psnprofiles.com/trophies/5278-nba-2k17-the-prelude)|
|**2**| 82.4907|16|227|(ps4, ps3, vita)| ![the-swapper](https://i.psnprofiles.com/games/2623d7/Se7d099.png)|[the-swapper](https://psnprofiles.com/trophies/2667-the-swapper)|
|**3**| 75.8524|49|34|(ps4)| ![riptide-gp2](https://i.psnprofiles.com/games/c58521/S3cf88e.png)|[riptide-gp2](https://psnprofiles.com/trophies/3639-riptide-gp2)|
|**4**| 75.375|55|51|(ps4)| ![monopoly-plus](https://i.psnprofiles.com/games/e54d9b/S7d9b9c.png)|[monopoly-plus](https://psnprofiles.com/trophies/3211-monopoly-plus)|
|**5**| 74.1724|50|156|(ps4)| ![child-of-light](https://i.psnprofiles.com/games/8e2ab8/S276fb5.png)|[child-of-light](https://psnprofiles.com/trophies/2578-child-of-light)|
|**6**| 73.5575|51|305|(ps4)| ![life-is-strange](https://i.psnprofiles.com/games/ed93d6/S1628ef.png)|[life-is-strange](https://psnprofiles.com/trophies/3313-life-is-strange)|
|**7**| 65.684|38|45|(ps4)| ![kitten-squad](https://i.psnprofiles.com/games/31ff91/Sfb6049.png)|[kitten-squad](https://psnprofiles.com/trophies/3864-kitten-squad)|
|**8**| 65.4886|40|43|(ps4)| ![adventure-capitalist](https://i.psnprofiles.com/games/84235d/S5350b0.png)|[adventure-capitalist](https://psnprofiles.com/trophies/5181-adventure-capitalist)|
|**9**| 64.4804|35|137|(ps4, ps3, vita)| ![teslagrad](https://i.psnprofiles.com/games/7e9125/S0de3be.png)|[teslagrad](https://psnprofiles.com/trophies/3178-teslagrad)|
|**10**| 62.3582|39|159|(ps4)| ![final-fantasy-type-0-hd](https://i.psnprofiles.com/games/77c651/S96fd71.png)|[final-fantasy-type-0-hd](https://psnprofiles.com/trophies/3416-final-fantasy-type-0-hd)|
2位，5位，6位，9位，10位は既にプレイしたことあり，偶然にも全てトロコンしているのでそれっぽい結果(?)な気がします．
それ以外のゲームはあまり...~~バスケゲームとか興味ねぇ~~->

## スクレイピング

[https://psnprofiles.com](https://psnprofiles.com/)のユーザページはjavascriptが効いていてページの最下部までスクロールする必要があります．

そこでseleniumとPhantomJSを使いました．

ubuntu16.04でAnaconda3を入れてjupyter notebookでpythonのコーディングをします．

seleniumのインストール:

```
pip install selenium
```

ubuntu環境でphantomJS入れるのはこちら:

[https://gist.github.com/julionc/7476620](https://gist.github.com/julionc/7476620)

```python
#ページスクロール soupを返す
def get_page_scroll(url):
    #PhantomJs
    driver = webdriver.PhantomJS()
    driver.get(url)
    #スクロールダウン
    #スクロールされてるか判断する部分
    lastHeight = driver.execute_script("return document.body.scrollHeight")
    soup = []
    while True:
        #スクロールダウン
        driver.execute_script("window.scrollTo(0, document.body.scrollHeight);")  
        #読み込まれるのを待つ
        time.sleep(5)
        newHeight = driver.execute_script("return document.body.scrollHeight")
        page_html = driver.page_source.encode('utf-8')
        soup = bs4.BeautifulSoup(page_html,'lxml')
        #スクロールされてるか判断する部分
        if newHeight == lastHeight:
            break
        lastHeight = newHeight
    return soup
```

ユーザのクローリングは[leaderboard](https://psnprofiles.com/leaderboard)からページを指定して行います．

1ページあたり50人(private userがいなければ)得られます．

```python
#ページをクロール
def get_userlist(start, pages):
    #ユーザをリストに
    id_list = []
    for i in range(start, start+pages):
        #leaderboadから取得する
        list_url = LIST_URL + str(i)
        try:
            page_html = urllib.request.urlopen(list_url).read()
        except Exception as e:
                estr = str(i) + " is 404"
                print(estr)
                continue
        soup = bs4.BeautifulSoup(page_html,'lxml')
        table = soup.find('table', id ='leaderboard')
        user_list = table.find_all('a', class_ = 'title')
        for href in user_list:
            id = href.get('href')
            #既にとったIDならリストに加えない
            if user_id_list.count(id.strip('/'))==0:
                #ユーザをリストに加える
                id_list.append(id)
    return id_list
```

全体のコードはgithubにぶち込んでおきます．
[https://github.com/ntyaan/psn-collaborative-filtering/blob/master/scraping.ipynb](https://github.com/ntyaan/psn-collaborative-filtering/blob/master/scraping.ipynb)

### データ
今回は2000ユーザ4803ゲームをスクレイピングしました．
整形したデータはこちら:

[ゲームタイトル](https://github.com/ntyaan/psn-collaborative-filtering/blob/master/data/games.txt)

[ゲームサムネイル](https://github.com/ntyaan/psn-collaborative-filtering/blob/master/data/images.txt)

[トロフィー平均取得率](https://github.com/ntyaan/psn-collaborative-filtering/blob/master/data/average.txt)

[プラットフォーム](https://github.com/ntyaan/psn-collaborative-filtering/blob/master/data/platform.txt) (ps4,ps3,vita)

[プレイ数](https://github.com/ntyaan/psn-collaborative-filtering/blob/master/data/count.txt) (2000人中)

[データ](https://raw.githubusercontent.com/ntyaan/psn-collaborative-filtering/master/data/data.txt) (ユーザ番号，ゲーム番号，トロフィー取得率)

## 強調フィルタリング

自分と似ているユーザが高評価とするアイテムを自分がまだ未評価であったときそのアイテムを推薦するというものです．

### ユーザ間の類似度

自分と他のユーザとの類似度としてピアソン相関係数を用います．

相関係数は-1から1の値をとり，1に近いほど類似性が高いです．

$M$をユーザ$k$と対象のユーザ$k'$が互いに評価しているアイテム数とします．

ここでは対象ユーザ(自分)のやったことのあるゲーム数が$M$です．

$x_{k',\ell}$はユーザ$k'$が評価したアイテム$\ell$の評価値となり，$\bar{x_{k',.}}$はユーザ$k'$と対象ユーザ$k$が互いにが評価したアイテムの平均評価値となります．

```math
sim(k, k')=\frac{\sum_{\ell=1}^M (x_{k,\ell}-\bar{x_{k,.}})(x_{k',\ell}-\bar{x_{k',.}})}{\sqrt{\sum_{\ell=1}^M(x_{k,\ell}-\bar{x_{k,.}})^2}\sqrt{\sum_{\ell=1}^M(x_{k',\ell}-\bar{x_{k',.}})^2}}\tag{1}
```

ここで計算を簡単にするために式変形を行います．

まず式(1)の分母を展開:

```math
\sqrt{\sum_{\ell=1}^M(x_{k,\ell}-\bar{x_{k,.}})^2}=\sqrt{\sum_{\ell=1}^M x_{k,\ell}^2-2\bar{x_{k,.}}\sum_{\ell=1}^Mx_{k,\ell}+ \bar{x_{k,.}}^2\sum_{\ell=1}^M }
```
ここで$\bar{x_{k,.}}=\frac{1}{M}\sum_{\ell=1}^M x_{k,\ell}$なので:

```math
\sqrt{\sum_{\ell=1}^M x_{k,\ell}^2-2\bar{x_{k,.}}\sum_{\ell=1}^Mx_{k,\ell}+ \bar{x_{k,.}}^2\sum_{\ell=1}^M }　= \sqrt{\sum_{\ell=1}^M x_{k,\ell}^2-2\bar{x_{k,.}} \times M \bar{x_{k,.}}+ \bar{x_{k,.}}^2\times M }　\\
= \sqrt{\sum_{\ell=1}^M x_{k,\ell}^2- M\bar{x_{k,.}}^2 } 
```

次に(1)の分子を展開:

```math
\sum_{\ell=1}^M (x_{k,\ell}-\bar{x_{k,.}})(x_{k',\ell}-\bar{x_{k',.}})=\sum_{\ell=1}^M x_{k,\ell}x_{k',\ell}-\sum_{\ell=1}^M x_{k,\ell}\bar{x_{k',.}}-\sum_{\ell=1}^M x_{k',\ell}\bar{x_{k,.}}+\sum_{\ell=1}^M \bar{x_{k,.}}\bar{x_{k',.}}\\
=\sum_{\ell=1}^M x_{k,\ell}x_{k',\ell}-M\bar{x_{k,.}}\bar{x_{k',.}}-M\bar{x_{k',.}}\bar{x_{k,.}}+M\bar{x_{k,.}}\bar{x_{k',.}}\\
=\sum_{\ell=1}^M x_{k,\ell}x_{k',\ell}-M\bar{x_{k,.}}\bar{x_{k',.}}
```

式(1)に代入しなおすと:

```math
sim(k, k')=\frac{\sum_{\ell=1}^M x_{k,\ell}x_{k',\ell}-M\bar{x_{k,.}}\bar{x_{k',.}}}{\sqrt{\sum_{\ell=1}^M x_{k,\ell}^2- M\bar{x_{k,.}}^2 } \sqrt{\sum_{\ell=1}^M x_{k',\ell}^2- M\bar{x_{k',.}}^2 }}
```

```math
sim(k, k')=\frac{\sum_{\ell=1}^M x_{k,\ell}x_{k',\ell}-\frac{1}{M}\sum_{\ell=1}^M x_{k,\ell}\sum_{\ell=1}^M x_{k',\ell}}{\sqrt{\sum_{\ell=1}^M x_{k,\ell}^2- \frac{1}{M} (\sum_{\ell=1}^M x_{k,\ell})^2 } \sqrt{\sum_{\ell=1}^M x_{k',\ell}^2- \frac{1}{M}}(\sum_{\ell=1}^M x_{k',\ell})^2}\tag{2}
```

これについてはオライリー出版の集合知プログラミングを参考にしました．

サンプルコードが公開されていてリンクはこちら:

[https://resources.oreilly.com/examples/9780596529321/blob/master/PCI_Code%20Folder/chapter2/recommendations.py](https://resources.oreilly.com/examples/9780596529321/blob/master/PCI_Code%20Folder/chapter2/recommendations.py)

41行目，関数sim_pearsonがこの式にあたる．

C++で書いたもの:

```cpp
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
```

### 評価値を求める
対象ユーザ$k$のアイテム$\ell$の予測値:

```math
\hat{x_{k,\ell}}=\bar{x_{k,.}}+\frac{\sum_{k':sim(k,k'>0)} sim(k,k')(x_{k,\ell}-\bar{x_{k',.}})}{\sum_{k':sim(k,k'>0)} sim(k,k')}
```
$\sum_{k':sim(k,k'>0)}$は類似度が0より大きいユーザに対してのみ計算するというものです．

C++で書いたもの:

```cpp
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
```

### コード
C++行列ライブラリであるEigen3の疎行列クラスを練習兼ねて使いました．

- [recom.h](https://github.com/ntyaan/psn-collaborative-filtering/blob/master/recom.h)
- [recom.cxx](https://github.com/ntyaan/psn-collaborative-filtering/blob/master/recom.cxx)
- [recom_main.cxx](https://github.com/ntyaan/psn-collaborative-filtering/blob/master/recom_main.cxx)

## 今後

- より多くのユーザを得る
とりあえず2000人としましたが疎行列クラスを使ったおかげか意外と処理が速かったのでユーザ数増やしてもいけそう．
ただスクレイピングの時間がかかる(2000人でまる1日くらい)．

- 先にユーザのグループ分けをする
予めクラスタリングでユーザ分けしておいて対象ユーザと同じグループに所属するユーザとの類似度のみ計算して予測したい．
クラスタリングの初期値によって結果が変わって面白い．
ただデータが大きいと処理時間がかる．


リンク:
[https://github.com/ntyaan/psn-collaborative-filtering](https://github.com/ntyaan/psn-collaborative-filtering)


# 2000ユーザ，4803ゲーム

[mmilkkr](https://psnprofiles.com/mmilkkr)のおすすめゲーム

| ranking | 予測値 | 平均取得率 |所持数| (ps4, vita) | 画像 | ゲーム名 |
|:---|:---|:----|:---|:----|:---|:---|
|**1**| 88.5148|61|55|(ps4)| ![nba-2k17-the-prelude](https://i.psnprofiles.com/games/bd0437/S1ef2a1.png)|[nba-2k17-the-prelude](https://psnprofiles.com/trophies/5278-nba-2k17-the-prelude)|
|**2**| 82.4907|16|227|(ps4, ps3, vita)| ![the-swapper](https://i.psnprofiles.com/games/2623d7/Se7d099.png)|[the-swapper](https://psnprofiles.com/trophies/2667-the-swapper)|
|**3**| 75.8524|49|34|(ps4)| ![riptide-gp2](https://i.psnprofiles.com/games/c58521/S3cf88e.png)|[riptide-gp2](https://psnprofiles.com/trophies/3639-riptide-gp2)|
|**4**| 75.375|55|51|(ps4)| ![monopoly-plus](https://i.psnprofiles.com/games/e54d9b/S7d9b9c.png)|[monopoly-plus](https://psnprofiles.com/trophies/3211-monopoly-plus)|
|**5**| 74.1724|50|156|(ps4)| ![child-of-light](https://i.psnprofiles.com/games/8e2ab8/S276fb5.png)|[child-of-light](https://psnprofiles.com/trophies/2578-child-of-light)|
|**6**| 73.5575|51|305|(ps4)| ![life-is-strange](https://i.psnprofiles.com/games/ed93d6/S1628ef.png)|[life-is-strange](https://psnprofiles.com/trophies/3313-life-is-strange)|
|**7**| 65.684|38|45|(ps4)| ![kitten-squad](https://i.psnprofiles.com/games/31ff91/Sfb6049.png)|[kitten-squad](https://psnprofiles.com/trophies/3864-kitten-squad)|
|**8**| 65.4886|40|43|(ps4)| ![adventure-capitalist](https://i.psnprofiles.com/games/84235d/S5350b0.png)|[adventure-capitalist](https://psnprofiles.com/trophies/5181-adventure-capitalist)|
|**9**| 64.4804|35|137|(ps4, ps3, vita)| ![teslagrad](https://i.psnprofiles.com/games/7e9125/S0de3be.png)|[teslagrad](https://psnprofiles.com/trophies/3178-teslagrad)|
|**10**| 62.3582|39|159|(ps4)| ![final-fantasy-type-0-hd](https://i.psnprofiles.com/games/77c651/S96fd71.png)|[final-fantasy-type-0-hd](https://psnprofiles.com/trophies/3416-final-fantasy-type-0-hd)|
|**11**| 60.9607|40|508|(ps4)| ![bloodborne](https://i.psnprofiles.com/games/b9aa79/S9484fb.png)|[bloodborne](https://psnprofiles.com/trophies/3431-bloodborne)|
|**12**| 60.6831|37|185|(ps4, ps3, vita)| ![thomas-was-alone](https://i.psnprofiles.com/games/f9c5d6/S7f8330.png)|[thomas-was-alone](https://psnprofiles.com/trophies/1932-thomas-was-alone)|
|**13**| 59.7288|38|107|(ps4, ps3)| ![sherlock-holmes-crimes-and-punishments](https://i.psnprofiles.com/games/e6c2c4/Scf336e.png)|[sherlock-holmes-crimes-and-punishments](https://psnprofiles.com/trophies/2932-sherlock-holmes-crimes-and-punishments)|
|**14**| 58.2081|23|171|(ps4)| ![stick-it-to-the-man](https://i.psnprofiles.com/games/112f7b/S3a4ba2.png)|[stick-it-to-the-man](https://psnprofiles.com/trophies/2587-stick-it-to-the-man)|
|**15**| 57.1273|36|52|(ps4)| ![bully](https://i.psnprofiles.com/games/145b47/Sa2c683.png)|[bully](https://psnprofiles.com/trophies/4475-bully)|
|**16**| 56.5735|7|39|(vita)| ![chronovolt](https://i.psnprofiles.com/games/c97a0e/Scc76d6.png)|[chronovolt](https://psnprofiles.com/trophies/1728-chronovolt)|
|**17**| 55.9457|30|119|(ps4)| ![the-elder-scrolls-v-skyrim](https://i.psnprofiles.com/games/850ee4/S8958f3.png)|[the-elder-scrolls-v-skyrim](https://psnprofiles.com/trophies/5507-the-elder-scrolls-v-skyrim)|
|**18**| 55.5923|33|32|(ps4)| ![kingdom-hearts-ii](https://i.psnprofiles.com/games/ce78b5/S1b8a08.png)|[kingdom-hearts-ii](https://psnprofiles.com/trophies/5913-kingdom-hearts-ii)|
|**19**| 55.277|4|62|(ps4, ps3, vita)| ![pumped-bmx](https://i.psnprofiles.com/games/9461b6/Sc817b2.png)|[pumped-bmx](https://psnprofiles.com/trophies/3897-pumped-bmx)|
|**20**| 55.1843|17|92|(ps4)| ![smite](https://i.psnprofiles.com/games/6519bb/S3f597d.png)|[smite](https://psnprofiles.com/trophies/4802-smite)|
|**21**| 53.6665|29|101|(ps4)| ![sound-shapes](https://i.psnprofiles.com/games/891961/S1f7c1c.png)|[sound-shapes](https://psnprofiles.com/trophies/2286-sound-shapes)|
|**22**| 53.3833|25|32|(ps4)| ![disney-infinity-20](https://i.psnprofiles.com/games/6b2215/S5a95cb.png)|[disney-infinity-20](https://psnprofiles.com/trophies/2895-disney-infinity-20)|
|**23**| 51.9432|30|164|(ps4, ps3, vita)| ![titan-attacks](https://i.psnprofiles.com/games/d3b5d6/S1050d8.png)|[titan-attacks](https://psnprofiles.com/trophies/2580-titan-attacks)|
|**24**| 50.6589|24|178|(ps4)| ![call-of-duty-modern-warfare-remastered](https://i.psnprofiles.com/games/594415/Seca8e6.png)|[call-of-duty-modern-warfare-remastered](https://psnprofiles.com/trophies/5379-call-of-duty-modern-warfare-remastered)|
|**25**| 50.6176|29|44|(ps4)| ![bound-by-flame](https://i.psnprofiles.com/games/528e12/S72a920.png)|[bound-by-flame](https://psnprofiles.com/trophies/2595-bound-by-flame)|
|**26**| 50.356|28|475|(ps4)| ![the-witcher-3-wild-hunt](https://i.psnprofiles.com/games/a426c2/S59313c.png)|[the-witcher-3-wild-hunt](https://psnprofiles.com/trophies/3573-the-witcher-3-wild-hunt)|
|**27**| 50.3372|1|49|(ps4, vita)| ![switch-galaxy-ultra](https://i.psnprofiles.com/games/fac288/S1c3928.png)|[switch-galaxy-ultra](https://psnprofiles.com/trophies/3269-switch-galaxy-ultra)|
|**28**| 46.7298|24|153|(ps4)| ![crash-bandicoot-warped](https://i.psnprofiles.com/games/880a06/Saacf5e.png)|[crash-bandicoot-warped](https://psnprofiles.com/trophies/6247-crash-bandicoot-warped)|
|**29**| 46.345|10|661|(ps4)| ![the-playroom](https://i.psnprofiles.com/games/f86427/S8b192a.png)|[the-playroom](https://psnprofiles.com/trophies/2166-the-playroom)|
|**30**| 45.6094|24|86|(vita)| ![destiny-of-spirits](https://i.psnprofiles.com/games/700f0b/Sa3c641.png)|[destiny-of-spirits](https://psnprofiles.com/trophies/2496-destiny-of-spirits)|
|**31**| 45.5833|17|33|(vita)| ![get-off-my-lawn](https://i.psnprofiles.com/games/45ee8b/Sf77343.png)|[get-off-my-lawn](https://psnprofiles.com/trophies/3121-get-off-my-lawn)|
|**32**| 44.9585|24|91|(vita)| ![metal-gear-solid-3](https://i.psnprofiles.com/games/266caf/S893a8f.png)|[metal-gear-solid-3](https://psnprofiles.com/trophies/1486-metal-gear-solid-3)|
|**33**| 44.3991|23|132|(vita)| ![wake-up-club](https://i.psnprofiles.com/games/d7458c/S14ad42.png)|[wake-up-club](https://psnprofiles.com/trophies/1424-wake-up-club)|
|**34**| 43.396|16|40|(ps4)| ![valkyria-chronicles](https://i.psnprofiles.com/games/aa4cd5/Sd11d6f.png)|[valkyria-chronicles](https://psnprofiles.com/trophies/4335-valkyria-chronicles)|
|**35**| 43.308|14|101|(ps3, vita)| ![proteus](https://i.psnprofiles.com/games/bbde20/S79eea4.png)|[proteus](https://psnprofiles.com/trophies/2213-proteus)|
|**36**| 43.2624|8|137|(ps4, ps3)| ![sportsfriends](https://i.psnprofiles.com/games/da7e4e/Sd603e3.png)|[sportsfriends](https://psnprofiles.com/trophies/2593-sportsfriends)|
|**37**| 43.1092|20|35|(ps4)| ![rogue-galaxy](https://i.psnprofiles.com/games/503a54/Saac432.png)|[rogue-galaxy](https://psnprofiles.com/trophies/4127-rogue-galaxy)|
|**38**| 42.4636|21|105|(ps3, vita)| ![luftrausers](https://i.psnprofiles.com/games/1b0c44/Sd78531.png)|[luftrausers](https://psnprofiles.com/trophies/2470-luftrausers)|
|**39**| 41.9075|18|87|(ps4)| ![doki-doki-universe](https://i.psnprofiles.com/games/abfd10/Sab6f60.png)|[doki-doki-universe](https://psnprofiles.com/trophies/2314-doki-doki-universe)|
|**40**| 41.3916|17|49|(ps4)| ![nhl-15](https://i.psnprofiles.com/games/4900ad/S92650f.png)|[nhl-15](https://psnprofiles.com/trophies/2873-nhl-15)|
|**41**| 41.1953|4|58|(ps4)| ![fortnite](https://i.psnprofiles.com/games/a78131/Sca8ea1.png)|[fortnite](https://psnprofiles.com/trophies/6391-fortnite)|
|**42**| 39.988|7|34|(ps4, ps3, vita)| ![hohokum](https://i.psnprofiles.com/games/95bc0f/Se50b9f.png)|[hohokum](https://psnprofiles.com/trophies/2806-hohokum)|
|**43**| 39.6472|3|68|(ps4)| ![aarus-awakening](https://i.psnprofiles.com/games/c43a0e/S5234d9.png)|[aarus-awakening](https://psnprofiles.com/trophies/3353-aarus-awakening)|
|**44**| 34.2532|9|80|(ps4)| ![airmech-arena](https://i.psnprofiles.com/games/58ccf8/Sa348c8.png)|[airmech-arena](https://psnprofiles.com/trophies/3550-airmech-arena)|
|**45**| 33.7892|11|231|(ps4)| ![dont-starve-console-edition](https://i.psnprofiles.com/games/c5a0a1/S13417e.png)|[dont-starve-console-edition](https://psnprofiles.com/trophies/2363-dont-starve-console-edition)|
|**46**| 33.5369|2|103|(ps4)| ![dc-universe-online](https://i.psnprofiles.com/games/179bc8/S55cfb5.png)|[dc-universe-online](https://psnprofiles.com/trophies/2306-dc-universe-online)|
|**47**| 33.467|7|159|(ps4, ps3, vita)| ![lone-survivor](https://i.psnprofiles.com/games/99455c/Sf9b767.png)|[lone-survivor](https://psnprofiles.com/trophies/2136-lone-survivor)|
|**48**| 32.834|6|52|(ps4)| ![the-four-kings-casino-and-slots](https://i.psnprofiles.com/games/cf1c44/Seb1b6e.png)|[the-four-kings-casino-and-slots](https://psnprofiles.com/trophies/4135-the-four-kings-casino-and-slots)|
|**49**| 32.2725|11|47|(ps4)| ![dynasty-warriors-8-xtreme-legends](https://i.psnprofiles.com/games/461b31/S94cd5d.png)|[dynasty-warriors-8-xtreme-legends](https://psnprofiles.com/trophies/2503-dynasty-warriors-8-xtreme-legends)|
|**50**| 30.3674|1|31|(ps4, vita)| ![ninja-senki-dx](https://i.psnprofiles.com/games/75c4c5/S167429.png)|[ninja-senki-dx](https://psnprofiles.com/trophies/4305-ninja-senki-dx)|
|**51**| 28.6019|8|126|(ps3, vita)| ![duke-nukem-3d-megaton-edition](https://i.psnprofiles.com/games/9fdcc2/S196357.png)|[duke-nukem-3d-megaton-edition](https://psnprofiles.com/trophies/3045-duke-nukem-3d-megaton-edition)|
|**52**| 28.4072|3|132|(ps4)| ![dc-universe-online](https://i.psnprofiles.com/games/a50e1f/S223bfb.png)|[dc-universe-online](https://psnprofiles.com/trophies/2259-dc-universe-online)|
|**53**| 25.799|2|89|(ps4)| ![secret-ponchos](https://i.psnprofiles.com/games/5e54fd/Se619da.png)|[secret-ponchos](https://psnprofiles.com/trophies/3209-secret-ponchos)|
|**54**| 23.9|3|63|(ps4)| ![planetside-2](https://i.psnprofiles.com/games/f4413b/S2fcdec.png)|[planetside-2](https://psnprofiles.com/trophies/3643-planetside-2)|

[mmilkkr](https://psnprofiles.com/mmilkkr)に似たユーザ一覧

| ranking | ユーザ名 | 類似度 | 
|:---|:---|:----|
|**1**|[KorvenDalas](https://psnprofiles.com/KorvenDalas)|1|
|**2**|[bappz](https://psnprofiles.com/bappz)|1|
|**3**|[Chop2Screw](https://psnprofiles.com/Chop2Screw)|1|
|**4**|[KINGBLOODAXE](https://psnprofiles.com/KINGBLOODAXE)|1|
|**5**|[neith83](https://psnprofiles.com/neith83)|1|
|**6**|[therd](https://psnprofiles.com/therd)|1|
|**7**|[plegos](https://psnprofiles.com/plegos)|1|
|**8**|[OG-Kush-BR](https://psnprofiles.com/OG-Kush-BR)|1|
|**9**|[bollaro](https://psnprofiles.com/bollaro)|1|
|**10**|[SNAKE_0-5](https://psnprofiles.com/SNAKE_0-5)|1|
|**11**|[WidgetPL](https://psnprofiles.com/WidgetPL)|1|
|**12**|[RonSun](https://psnprofiles.com/RonSun)|1|
|**13**|[brutal_grutle](https://psnprofiles.com/brutal_grutle)|1|
|**14**|[EZIOisBACK](https://psnprofiles.com/EZIOisBACK)|1|
|**15**|[penguins_fan87](https://psnprofiles.com/penguins_fan87)|1|
|**16**|[Oblivian_92](https://psnprofiles.com/Oblivian_92)|1|
|**17**|[AWesker70](https://psnprofiles.com/AWesker70)|1|
|**18**|[BATLESTAR](https://psnprofiles.com/BATLESTAR)|1|
|**19**|[Djidane1](https://psnprofiles.com/Djidane1)|1|
|**20**|[basch2](https://psnprofiles.com/basch2)|1|
|**21**|[Papa_Lazarou](https://psnprofiles.com/Papa_Lazarou)|1|
|**22**|[MORTAL_INFERNO](https://psnprofiles.com/MORTAL_INFERNO)|1|
|**23**|[piconjo666](https://psnprofiles.com/piconjo666)|1|
|**24**|[Tsukishi-D](https://psnprofiles.com/Tsukishi-D)|1|
|**25**|[K1dW17hGun5](https://psnprofiles.com/K1dW17hGun5)|1|
|**26**|[Hitman1986](https://psnprofiles.com/Hitman1986)|1|
|**27**|[Patrik-SVK2](https://psnprofiles.com/Patrik-SVK2)|1|
|**28**|[nakara](https://psnprofiles.com/nakara)|1|
|**29**|[MoScHiN_4rozzo](https://psnprofiles.com/MoScHiN_4rozzo)|1|
|**30**|[Alberto-C](https://psnprofiles.com/Alberto-C)|1|
|**31**|[Madore_](https://psnprofiles.com/Madore_)|1|
|**32**|[AoNoExorcist17](https://psnprofiles.com/AoNoExorcist17)|1|
|**33**|[cholopower93](https://psnprofiles.com/cholopower93)|1|
|**34**|[Wigglertron](https://psnprofiles.com/Wigglertron)|1|
|**35**|[actnthi](https://psnprofiles.com/actnthi)|1|
|**36**|[blechkopp](https://psnprofiles.com/blechkopp)|1|
|**37**|[JayCrizzle69](https://psnprofiles.com/JayCrizzle69)|1|
|**38**|[j-paul-AK13](https://psnprofiles.com/j-paul-AK13)|1|
|**39**|[Dantezin](https://psnprofiles.com/Dantezin)|1|
|**40**|[xQUIET-KILLAx](https://psnprofiles.com/xQUIET-KILLAx)|1|
|**41**|[IronWarZ](https://psnprofiles.com/IronWarZ)|1|
|**42**|[bicbic23](https://psnprofiles.com/bicbic23)|1|
|**43**|[DaNnyRoCkEr5769](https://psnprofiles.com/DaNnyRoCkEr5769)|1|
|**44**|[atlasatlas](https://psnprofiles.com/atlasatlas)|1|
|**45**|[blue-white-fire](https://psnprofiles.com/blue-white-fire)|1|
|**46**|[b-boy410](https://psnprofiles.com/b-boy410)|1|
|**47**|[ithigo](https://psnprofiles.com/ithigo)|1|
|**48**|[QUEEN_TATSU_MA](https://psnprofiles.com/QUEEN_TATSU_MA)|1|
|**49**|[MrSmith88](https://psnprofiles.com/MrSmith88)|1|
|**50**|[EvilPandaPirate](https://psnprofiles.com/EvilPandaPirate)|1|
|**51**|[MikeLeRoi-](https://psnprofiles.com/MikeLeRoi-)|1|
|**52**|[Kongari](https://psnprofiles.com/Kongari)|1|
|**53**|[Hazelraine](https://psnprofiles.com/Hazelraine)|1|
|**54**|[katapultz](https://psnprofiles.com/katapultz)|1|
|**55**|[eMiLeM](https://psnprofiles.com/eMiLeM)|1|
|**56**|[SpeIIs](https://psnprofiles.com/SpeIIs)|1|
|**57**|[Paindriven13](https://psnprofiles.com/Paindriven13)|1|
|**58**|[Goldnheart](https://psnprofiles.com/Goldnheart)|1|
|**59**|[Narmesa](https://psnprofiles.com/Narmesa)|1|
|**60**|[RainbowCheese123](https://psnprofiles.com/RainbowCheese123)|1|
|**61**|[FeDo-Berlin](https://psnprofiles.com/FeDo-Berlin)|1|
|**62**|[jacksepticey](https://psnprofiles.com/jacksepticey)|1|
|**63**|[saxplyr70](https://psnprofiles.com/saxplyr70)|1|
|**64**|[Willy21](https://psnprofiles.com/Willy21)|1|
|**65**|[Ninja-Tree-RH](https://psnprofiles.com/Ninja-Tree-RH)|1|
|**66**|[v123moto45](https://psnprofiles.com/v123moto45)|1|
|**67**|[plexx713](https://psnprofiles.com/plexx713)|1|
|**68**|[indica38](https://psnprofiles.com/indica38)|1|
|**69**|[HaitianDiplomat](https://psnprofiles.com/HaitianDiplomat)|1|
|**70**|[H72276992](https://psnprofiles.com/H72276992)|1|
|**71**|[SoloWingKnight](https://psnprofiles.com/SoloWingKnight)|1|
|**72**|[Phil32](https://psnprofiles.com/Phil32)|1|
|**73**|[Pennywise822](https://psnprofiles.com/Pennywise822)|1|
|**74**|[PINO325](https://psnprofiles.com/PINO325)|1|
|**75**|[LegendarySaint](https://psnprofiles.com/LegendarySaint)|1|
|**76**|[cerko1](https://psnprofiles.com/cerko1)|1|
|**77**|[Shkloo](https://psnprofiles.com/Shkloo)|1|
|**78**|[SpiritusPL](https://psnprofiles.com/SpiritusPL)|1|
|**79**|[pon4ik297](https://psnprofiles.com/pon4ik297)|1|
|**80**|[Thea_Agapi](https://psnprofiles.com/Thea_Agapi)|1|
|**81**|[b38669](https://psnprofiles.com/b38669)|1|
|**82**|[turikamn](https://psnprofiles.com/turikamn)|1|
|**83**|[StievlWare](https://psnprofiles.com/StievlWare)|1|
|**84**|[Alex1602_](https://psnprofiles.com/Alex1602_)|1|
|**85**|[Rimworld](https://psnprofiles.com/Rimworld)|1|
|**86**|[Chao_Ken](https://psnprofiles.com/Chao_Ken)|1|
|**87**|[xAltellic_21](https://psnprofiles.com/xAltellic_21)|1|
|**88**|[ZakkStar](https://psnprofiles.com/ZakkStar)|1|
|**89**|[BlindJudgment](https://psnprofiles.com/BlindJudgment)|1|
|**90**|[LordStark101](https://psnprofiles.com/LordStark101)|1|
|**91**|[Azzrael072](https://psnprofiles.com/Azzrael072)|0.999594|
|**92**|[RFK](https://psnprofiles.com/RFK)|0.998874|
|**93**|[Blackout8118](https://psnprofiles.com/Blackout8118)|0.998269|
|**94**|[darkghost227](https://psnprofiles.com/darkghost227)|0.996602|
|**95**|[gryle_pt](https://psnprofiles.com/gryle_pt)|0.995765|
|**96**|[Jormasaurus](https://psnprofiles.com/Jormasaurus)|0.995196|
|**97**|[bossarmy123](https://psnprofiles.com/bossarmy123)|0.993622|
|**98**|[roshankalro](https://psnprofiles.com/roshankalro)|0.991788|
|**99**|[Mackincheezz](https://psnprofiles.com/Mackincheezz)|0.991434|
|**100**|[Mister_Mane](https://psnprofiles.com/Mister_Mane)|0.989394|
|**101**|[Tony_Sooprano](https://psnprofiles.com/Tony_Sooprano)|0.988954|
|**102**|[fifa456n](https://psnprofiles.com/fifa456n)|0.98626|
|**103**|[Night2Fire](https://psnprofiles.com/Night2Fire)|0.9821|
|**104**|[EVO469](https://psnprofiles.com/EVO469)|0.978929|
|**105**|[king_haider123](https://psnprofiles.com/king_haider123)|0.978057|
|**106**|[shaqattack91](https://psnprofiles.com/shaqattack91)|0.973374|
|**107**|[lynnlong](https://psnprofiles.com/lynnlong)|0.972273|
|**108**|[CapitaoxBR1](https://psnprofiles.com/CapitaoxBR1)|0.970653|
|**109**|[Andrewsarchus77](https://psnprofiles.com/Andrewsarchus77)|0.970653|
|**110**|[appreciateit](https://psnprofiles.com/appreciateit)|0.967545|
|**111**|[joljo13](https://psnprofiles.com/joljo13)|0.956895|
|**112**|[SUPER_SONIC_86](https://psnprofiles.com/SUPER_SONIC_86)|0.956363|
|**113**|[kazoo](https://psnprofiles.com/kazoo)|0.95294|
|**114**|[ZEO_BR](https://psnprofiles.com/ZEO_BR)|0.949857|
|**115**|[Kevin-Jun](https://psnprofiles.com/Kevin-Jun)|0.948757|
|**116**|[vaoboo](https://psnprofiles.com/vaoboo)|0.943864|
|**117**|[Andr3wzz24](https://psnprofiles.com/Andr3wzz24)|0.940697|
|**118**|[cjyar](https://psnprofiles.com/cjyar)|0.940479|
|**119**|[xGJystic](https://psnprofiles.com/xGJystic)|0.914919|
|**120**|[Doe69](https://psnprofiles.com/Doe69)|0.914807|
|**121**|[animus_ex_fides](https://psnprofiles.com/animus_ex_fides)|0.910854|
|**122**|[barteejeremie](https://psnprofiles.com/barteejeremie)|0.910598|
|**123**|[shinenick](https://psnprofiles.com/shinenick)|0.910243|
|**124**|[baronholbach](https://psnprofiles.com/baronholbach)|0.904437|
|**125**|[Fire_Resist_71](https://psnprofiles.com/Fire_Resist_71)|0.903617|
|**126**|[the_dixa](https://psnprofiles.com/the_dixa)|0.900845|
|**127**|[doradorey](https://psnprofiles.com/doradorey)|0.891392|
|**128**|[mu-kun_](https://psnprofiles.com/mu-kun_)|0.890823|
|**129**|[likemiii](https://psnprofiles.com/likemiii)|0.887068|
|**130**|[horndevil69](https://psnprofiles.com/horndevil69)|0.876448|
|**131**|[fairygoddess](https://psnprofiles.com/fairygoddess)|0.876032|
|**132**|[Oiracul](https://psnprofiles.com/Oiracul)|0.873164|
|**133**|[CZEBullyZ](https://psnprofiles.com/CZEBullyZ)|0.863327|
|**134**|[Rurouni1560](https://psnprofiles.com/Rurouni1560)|0.858708|
|**135**|[WorstOfAllTime](https://psnprofiles.com/WorstOfAllTime)|0.858449|
|**136**|[cpt_duncan](https://psnprofiles.com/cpt_duncan)|0.852446|
|**137**|[Aisids](https://psnprofiles.com/Aisids)|0.851844|
|**138**|[sezkee](https://psnprofiles.com/sezkee)|0.849839|
|**139**|[W33DnSNIPE](https://psnprofiles.com/W33DnSNIPE)|0.848555|
|**140**|[huttchins](https://psnprofiles.com/huttchins)|0.847719|
|**141**|[StylePinkPink](https://psnprofiles.com/StylePinkPink)|0.846392|
|**142**|[VERY_EVIL_ASH](https://psnprofiles.com/VERY_EVIL_ASH)|0.845181|
|**143**|[MightyDizorder](https://psnprofiles.com/MightyDizorder)|0.842166|
|**144**|[kubikkubik](https://psnprofiles.com/kubikkubik)|0.84031|
|**145**|[tegienio](https://psnprofiles.com/tegienio)|0.839035|
|**146**|[Animu_Dragon](https://psnprofiles.com/Animu_Dragon)|0.837796|
|**147**|[CrazyAchiasa](https://psnprofiles.com/CrazyAchiasa)|0.836697|
|**148**|[RachelAvatar](https://psnprofiles.com/RachelAvatar)|0.834071|
|**149**|[WeakerYouth](https://psnprofiles.com/WeakerYouth)|0.82966|
|**150**|[chumba24](https://psnprofiles.com/chumba24)|0.827989|
|**151**|[profe](https://psnprofiles.com/profe)|0.826722|
|**152**|[Seachicken32](https://psnprofiles.com/Seachicken32)|0.823309|
|**153**|[Techsoly](https://psnprofiles.com/Techsoly)|0.818567|
|**154**|[DrDeath813](https://psnprofiles.com/DrDeath813)|0.812873|
|**155**|[pi_wi](https://psnprofiles.com/pi_wi)|0.812703|
|**156**|[Yuriku6](https://psnprofiles.com/Yuriku6)|0.811078|
|**157**|[SkyeBurner](https://psnprofiles.com/SkyeBurner)|0.810807|
|**158**|[MRdubbynr1](https://psnprofiles.com/MRdubbynr1)|0.802229|
|**159**|[sxk](https://psnprofiles.com/sxk)|0.799469|
|**160**|[Kaasutii](https://psnprofiles.com/Kaasutii)|0.797777|
|**161**|[devils7advocate](https://psnprofiles.com/devils7advocate)|0.796633|
|**162**|[PatrykKOT](https://psnprofiles.com/PatrykKOT)|0.795965|
|**163**|[closebutfar](https://psnprofiles.com/closebutfar)|0.790703|
|**164**|[yb4ever](https://psnprofiles.com/yb4ever)|0.787239|
|**165**|[frost345677](https://psnprofiles.com/frost345677)|0.786895|
|**166**|[luchito1080p](https://psnprofiles.com/luchito1080p)|0.782659|
|**167**|[WarZone56](https://psnprofiles.com/WarZone56)|0.782356|
|**168**|[mr_jackkas](https://psnprofiles.com/mr_jackkas)|0.778855|
|**169**|[suny228](https://psnprofiles.com/suny228)|0.776194|
|**170**|[SuperTempDark](https://psnprofiles.com/SuperTempDark)|0.775458|
|**171**|[Clowerdean](https://psnprofiles.com/Clowerdean)|0.760662|
|**172**|[DavidxTidus](https://psnprofiles.com/DavidxTidus)|0.75869|
|**173**|[DJ_Chris101](https://psnprofiles.com/DJ_Chris101)|0.757517|
|**174**|[nekor4fite](https://psnprofiles.com/nekor4fite)|0.757287|
|**175**|[Gunner63095](https://psnprofiles.com/Gunner63095)|0.754976|
|**176**|[chicken_360](https://psnprofiles.com/chicken_360)|0.754757|
|**177**|[fatiga_cronica](https://psnprofiles.com/fatiga_cronica)|0.744708|
|**178**|[Cuban_BEEFCAKE](https://psnprofiles.com/Cuban_BEEFCAKE)|0.743509|
|**179**|[Denver413](https://psnprofiles.com/Denver413)|0.739883|
|**180**|[RyutheStarHUN](https://psnprofiles.com/RyutheStarHUN)|0.738574|
|**181**|[Robsvafan](https://psnprofiles.com/Robsvafan)|0.735413|
|**182**|[Bronbron89](https://psnprofiles.com/Bronbron89)|0.730672|
|**183**|[AaaaaaaaronH](https://psnprofiles.com/AaaaaaaaronH)|0.729777|
|**184**|[X-SakuraUchiha-X](https://psnprofiles.com/X-SakuraUchiha-X)|0.725797|
|**185**|[KozuBlue](https://psnprofiles.com/KozuBlue)|0.722869|
|**186**|[DisaHC](https://psnprofiles.com/DisaHC)|0.720896|
|**187**|[SeefPanda](https://psnprofiles.com/SeefPanda)|0.720735|
|**188**|[ArmchairSoldier](https://psnprofiles.com/ArmchairSoldier)|0.715347|
|**189**|[theory980](https://psnprofiles.com/theory980)|0.713959|
|**190**|[MaulRx](https://psnprofiles.com/MaulRx)|0.710065|
|**191**|[aimar53](https://psnprofiles.com/aimar53)|0.70968|
|**192**|[cacxalot](https://psnprofiles.com/cacxalot)|0.702959|
|**193**|[jotajota94](https://psnprofiles.com/jotajota94)|0.701928|
|**194**|[ChaosX3](https://psnprofiles.com/ChaosX3)|0.696581|
|**195**|[raving-rabbit23](https://psnprofiles.com/raving-rabbit23)|0.690469|
|**196**|[orga_gilchrist71](https://psnprofiles.com/orga_gilchrist71)|0.685925|
|**197**|[TL1000SVSS](https://psnprofiles.com/TL1000SVSS)|0.685571|
|**198**|[Xenos18](https://psnprofiles.com/Xenos18)|0.681092|
|**199**|[Arlem_ITA](https://psnprofiles.com/Arlem_ITA)|0.680204|
|**200**|[MunizPB](https://psnprofiles.com/MunizPB)|0.67415|
|**201**|[Enigmatical9](https://psnprofiles.com/Enigmatical9)|0.6694|
|**202**|[Darth_T84](https://psnprofiles.com/Darth_T84)|0.6666|
|**203**|[ChiIIing-](https://psnprofiles.com/ChiIIing-)|0.665771|
|**204**|[seanodinihio](https://psnprofiles.com/seanodinihio)|0.664551|
|**205**|[Dake79](https://psnprofiles.com/Dake79)|0.664267|
|**206**|[darkwave](https://psnprofiles.com/darkwave)|0.663352|
|**207**|[ST-N310](https://psnprofiles.com/ST-N310)|0.660415|
|**208**|[Thauma-Paidi](https://psnprofiles.com/Thauma-Paidi)|0.658629|
|**209**|[LiBF](https://psnprofiles.com/LiBF)|0.658219|
|**210**|[shinta-09](https://psnprofiles.com/shinta-09)|0.656291|
|**211**|[GSylar](https://psnprofiles.com/GSylar)|0.655936|
|**212**|[madao765](https://psnprofiles.com/madao765)|0.65544|
|**213**|[T3ngo_M8ngo](https://psnprofiles.com/T3ngo_M8ngo)|0.655426|
|**214**|[Daniand83](https://psnprofiles.com/Daniand83)|0.649912|
|**215**|[Mr_Pingouin_78](https://psnprofiles.com/Mr_Pingouin_78)|0.649018|
|**216**|[Power-gamer1-2-3](https://psnprofiles.com/Power-gamer1-2-3)|0.646862|
|**217**|[Idriszade](https://psnprofiles.com/Idriszade)|0.645187|
|**218**|[Weow](https://psnprofiles.com/Weow)|0.642761|
|**219**|[kenlight777](https://psnprofiles.com/kenlight777)|0.642522|
|**220**|[Dangaiares](https://psnprofiles.com/Dangaiares)|0.641767|
|**221**|[Comray002](https://psnprofiles.com/Comray002)|0.635472|
|**222**|[MoeJoe666](https://psnprofiles.com/MoeJoe666)|0.633352|
|**223**|[duobleray](https://psnprofiles.com/duobleray)|0.633188|
|**224**|[The_OG_Corpral](https://psnprofiles.com/The_OG_Corpral)|0.631842|
|**225**|[Be-cry](https://psnprofiles.com/Be-cry)|0.631507|
|**226**|[Loger25](https://psnprofiles.com/Loger25)|0.631248|
|**227**|[ElGiulio](https://psnprofiles.com/ElGiulio)|0.631079|
|**228**|[GreazyMeat](https://psnprofiles.com/GreazyMeat)|0.628092|
|**229**|[agro64](https://psnprofiles.com/agro64)|0.627295|
|**230**|[kevdawg2003](https://psnprofiles.com/kevdawg2003)|0.626766|
|**231**|[Pat-Man-Levy](https://psnprofiles.com/Pat-Man-Levy)|0.624917|
|**232**|[Chrysoprase](https://psnprofiles.com/Chrysoprase)|0.62314|
|**233**|[CaptainCochran](https://psnprofiles.com/CaptainCochran)|0.621261|
|**234**|[brunomartins_10](https://psnprofiles.com/brunomartins_10)|0.620988|
|**235**|[cronqvist](https://psnprofiles.com/cronqvist)|0.620581|
|**236**|[ToKillTime](https://psnprofiles.com/ToKillTime)|0.620303|
|**237**|[FaVo_2008](https://psnprofiles.com/FaVo_2008)|0.619663|
|**238**|[Fixermonster](https://psnprofiles.com/Fixermonster)|0.617458|
|**239**|[blackie_13](https://psnprofiles.com/blackie_13)|0.617167|
|**240**|[TTH-X](https://psnprofiles.com/TTH-X)|0.612271|
|**241**|[goldweiss](https://psnprofiles.com/goldweiss)|0.612261|
|**242**|[Zardigan](https://psnprofiles.com/Zardigan)|0.609131|
|**243**|[Genocide22](https://psnprofiles.com/Genocide22)|0.607957|
|**244**|[QueenAmano](https://psnprofiles.com/QueenAmano)|0.604028|
|**245**|[Jax_Johnson](https://psnprofiles.com/Jax_Johnson)|0.602754|
|**246**|[T7easy1](https://psnprofiles.com/T7easy1)|0.601414|
|**247**|[marc1laur](https://psnprofiles.com/marc1laur)|0.600939|
|**248**|[yamatokyu](https://psnprofiles.com/yamatokyu)|0.600051|
|**249**|[victorst](https://psnprofiles.com/victorst)|0.599678|
|**250**|[Churrk](https://psnprofiles.com/Churrk)|0.599068|
|**251**|[amiynbizness](https://psnprofiles.com/amiynbizness)|0.59688|
|**252**|[SkipWhiddle](https://psnprofiles.com/SkipWhiddle)|0.595958|
|**253**|[Midnite--Maniac](https://psnprofiles.com/Midnite--Maniac)|0.595875|
|**254**|[hayabusabauer](https://psnprofiles.com/hayabusabauer)|0.594333|
|**255**|[Solid_Snake-MGS](https://psnprofiles.com/Solid_Snake-MGS)|0.589163|
|**256**|[DarKnight01](https://psnprofiles.com/DarKnight01)|0.587442|
|**257**|[makomba](https://psnprofiles.com/makomba)|0.585492|
|**258**|[launcher_no8](https://psnprofiles.com/launcher_no8)|0.584296|
|**259**|[Gerchi-san](https://psnprofiles.com/Gerchi-san)|0.583213|
|**260**|[Horo_Cat](https://psnprofiles.com/Horo_Cat)|0.58012|
|**261**|[Big_Borsuk](https://psnprofiles.com/Big_Borsuk)|0.57399|
|**262**|[N5FTL](https://psnprofiles.com/N5FTL)|0.570911|
|**263**|[Anakyne](https://psnprofiles.com/Anakyne)|0.57009|
|**264**|[jeben](https://psnprofiles.com/jeben)|0.564773|
|**265**|[sid08199](https://psnprofiles.com/sid08199)|0.563096|
|**266**|[Franki558nho007](https://psnprofiles.com/Franki558nho007)|0.562124|
|**267**|[BANKAI-9999](https://psnprofiles.com/BANKAI-9999)|0.560526|
|**268**|[BreezyNate](https://psnprofiles.com/BreezyNate)|0.558255|
|**269**|[Chevy9](https://psnprofiles.com/Chevy9)|0.55772|
|**270**|[Guthak](https://psnprofiles.com/Guthak)|0.556697|
|**271**|[Sleeze](https://psnprofiles.com/Sleeze)|0.55077|
|**272**|[stefval](https://psnprofiles.com/stefval)|0.54809|
|**273**|[Jennylein-87](https://psnprofiles.com/Jennylein-87)|0.547872|
|**274**|[portland_Benjai](https://psnprofiles.com/portland_Benjai)|0.547324|
|**275**|[piedra103](https://psnprofiles.com/piedra103)|0.545817|
|**276**|[kenchiba](https://psnprofiles.com/kenchiba)|0.545205|
|**277**|[javadb76](https://psnprofiles.com/javadb76)|0.544571|
|**278**|[schormi](https://psnprofiles.com/schormi)|0.538008|
|**279**|[ShinRaikdou](https://psnprofiles.com/ShinRaikdou)|0.530313|
|**280**|[induce](https://psnprofiles.com/induce)|0.52932|
|**281**|[larysa1](https://psnprofiles.com/larysa1)|0.528679|
|**282**|[skrh_KSA](https://psnprofiles.com/skrh_KSA)|0.52845|
|**283**|[iggoolrob](https://psnprofiles.com/iggoolrob)|0.526073|
|**284**|[littlesaber](https://psnprofiles.com/littlesaber)|0.525541|
|**285**|[Spyable](https://psnprofiles.com/Spyable)|0.521078|
|**286**|[kittyxiii](https://psnprofiles.com/kittyxiii)|0.520401|
|**287**|[MattmaN27](https://psnprofiles.com/MattmaN27)|0.507726|
|**288**|[Jerry_B98](https://psnprofiles.com/Jerry_B98)|0.506606|
|**289**|[PowerPhil](https://psnprofiles.com/PowerPhil)|0.50616|
|**290**|[Zai_phon](https://psnprofiles.com/Zai_phon)|0.505948|
|**291**|[DeadStarXI](https://psnprofiles.com/DeadStarXI)|0.504483|
|**292**|[Diokaza](https://psnprofiles.com/Diokaza)|0.504036|
|**293**|[BlackoutStreamx](https://psnprofiles.com/BlackoutStreamx)|0.502956|
|**294**|[Zz-Vanguard-zZ](https://psnprofiles.com/Zz-Vanguard-zZ)|0.502298|
|**295**|[Bitberger](https://psnprofiles.com/Bitberger)|0.502273|
|**296**|[Rinxin](https://psnprofiles.com/Rinxin)|0.5|
|**297**|[Xenophase](https://psnprofiles.com/Xenophase)|0.5|
|**298**|[Ruffy254](https://psnprofiles.com/Ruffy254)|0.499268|
|**299**|[doubleA4200](https://psnprofiles.com/doubleA4200)|0.495733|
|**300**|[vw12vv](https://psnprofiles.com/vw12vv)|0.492333|
|**301**|[ehPom](https://psnprofiles.com/ehPom)|0.491958|
|**302**|[Alex-Yeh-](https://psnprofiles.com/Alex-Yeh-)|0.48914|
|**303**|[Akustikrausch](https://psnprofiles.com/Akustikrausch)|0.488622|
|**304**|[Kaesesauce](https://psnprofiles.com/Kaesesauce)|0.487172|
|**305**|[I_Guardiola_I](https://psnprofiles.com/I_Guardiola_I)|0.485926|
|**306**|[LO_3PRIN](https://psnprofiles.com/LO_3PRIN)|0.481514|
|**307**|[Doctor-Disco-357](https://psnprofiles.com/Doctor-Disco-357)|0.481477|
|**308**|[Ninja_Dave789](https://psnprofiles.com/Ninja_Dave789)|0.481071|
|**309**|[llLeonhart](https://psnprofiles.com/llLeonhart)|0.479996|
|**310**|[Ishimondo](https://psnprofiles.com/Ishimondo)|0.478895|
|**311**|[forseti_pl_](https://psnprofiles.com/forseti_pl_)|0.476853|
|**312**|[Cyber24PL](https://psnprofiles.com/Cyber24PL)|0.47642|
|**313**|[RoZiPL](https://psnprofiles.com/RoZiPL)|0.476266|
|**314**|[Mischo_HH](https://psnprofiles.com/Mischo_HH)|0.474963|
|**315**|[the-ghostrider74](https://psnprofiles.com/the-ghostrider74)|0.474404|
|**316**|[kayeliz](https://psnprofiles.com/kayeliz)|0.473837|
|**317**|[ZELLO18](https://psnprofiles.com/ZELLO18)|0.473615|
|**318**|[RavenXRW](https://psnprofiles.com/RavenXRW)|0.472262|
|**319**|[Migglestyx](https://psnprofiles.com/Migglestyx)|0.470848|
|**320**|[Gonsazes](https://psnprofiles.com/Gonsazes)|0.470474|
|**321**|[P1_Game_Freak](https://psnprofiles.com/P1_Game_Freak)|0.469173|
|**322**|[oleycon](https://psnprofiles.com/oleycon)|0.464576|
|**323**|[RenS_GepkenS](https://psnprofiles.com/RenS_GepkenS)|0.455471|
|**324**|[snlffon](https://psnprofiles.com/snlffon)|0.45453|
|**325**|[cysio25](https://psnprofiles.com/cysio25)|0.452522|
|**326**|[ikura](https://psnprofiles.com/ikura)|0.450768|
|**327**|[NightHawk4399](https://psnprofiles.com/NightHawk4399)|0.45029|
|**328**|[leysiydavid](https://psnprofiles.com/leysiydavid)|0.449817|
|**329**|[Warhero32](https://psnprofiles.com/Warhero32)|0.448836|
|**330**|[Luciferr](https://psnprofiles.com/Luciferr)|0.444376|
|**331**|[EiRi_](https://psnprofiles.com/EiRi_)|0.441315|
|**332**|[sappyday](https://psnprofiles.com/sappyday)|0.439971|
|**333**|[Jhael](https://psnprofiles.com/Jhael)|0.437407|
|**334**|[Kinkearth456](https://psnprofiles.com/Kinkearth456)|0.434004|
|**335**|[dam-witek_1](https://psnprofiles.com/dam-witek_1)|0.432426|
|**336**|[IAmLeb-](https://psnprofiles.com/IAmLeb-)|0.430261|
|**337**|[EricP23](https://psnprofiles.com/EricP23)|0.430079|
|**338**|[Ray_and_Ray](https://psnprofiles.com/Ray_and_Ray)|0.429393|
|**339**|[mrSpriggi](https://psnprofiles.com/mrSpriggi)|0.429093|
|**340**|[Brutananadlewski](https://psnprofiles.com/Brutananadlewski)|0.428259|
|**341**|[SOURINYOMOUTH](https://psnprofiles.com/SOURINYOMOUTH)|0.424425|
|**342**|[Thinkverse](https://psnprofiles.com/Thinkverse)|0.423806|
|**343**|[Aye_Jae504](https://psnprofiles.com/Aye_Jae504)|0.42197|
|**344**|[marlee123](https://psnprofiles.com/marlee123)|0.421052|
|**345**|[jacketGT](https://psnprofiles.com/jacketGT)|0.420933|
|**346**|[ipwn-noobs14](https://psnprofiles.com/ipwn-noobs14)|0.420169|
|**347**|[nansy413](https://psnprofiles.com/nansy413)|0.417231|
|**348**|[Ch0c0l4t3_B0ii7](https://psnprofiles.com/Ch0c0l4t3_B0ii7)|0.414493|
|**349**|[Massive-Attack1](https://psnprofiles.com/Massive-Attack1)|0.41289|
|**350**|[TheProSnakez](https://psnprofiles.com/TheProSnakez)|0.412835|
|**351**|[iame1102](https://psnprofiles.com/iame1102)|0.411664|
|**352**|[Niy](https://psnprofiles.com/Niy)|0.410163|
|**353**|[kit_justin](https://psnprofiles.com/kit_justin)|0.408394|
|**354**|[Crimson_Water](https://psnprofiles.com/Crimson_Water)|0.407947|
|**355**|[Lunitron](https://psnprofiles.com/Lunitron)|0.407486|
|**356**|[Dateiji](https://psnprofiles.com/Dateiji)|0.407233|
|**357**|[NBAKING1985](https://psnprofiles.com/NBAKING1985)|0.405735|
|**358**|[chyck23](https://psnprofiles.com/chyck23)|0.403811|
|**359**|[ed_great](https://psnprofiles.com/ed_great)|0.400725|
|**360**|[Lelouch-sos](https://psnprofiles.com/Lelouch-sos)|0.399475|
|**361**|[Oreye03](https://psnprofiles.com/Oreye03)|0.399265|
|**362**|[FlameAdell](https://psnprofiles.com/FlameAdell)|0.395656|
|**363**|[Flaash91](https://psnprofiles.com/Flaash91)|0.392882|
|**364**|[zansshi](https://psnprofiles.com/zansshi)|0.387988|
|**365**|[GdzieLaszlo](https://psnprofiles.com/GdzieLaszlo)|0.384922|
|**366**|[Jaky82](https://psnprofiles.com/Jaky82)|0.383155|
|**367**|[dingle1897](https://psnprofiles.com/dingle1897)|0.382366|
|**368**|[tankist161](https://psnprofiles.com/tankist161)|0.381945|
|**369**|[Jared_Helsin](https://psnprofiles.com/Jared_Helsin)|0.38154|
|**370**|[Manteca85](https://psnprofiles.com/Manteca85)|0.3804|
|**371**|[Qiulian](https://psnprofiles.com/Qiulian)|0.380135|
|**372**|[Croxx89](https://psnprofiles.com/Croxx89)|0.378869|
|**373**|[jivit](https://psnprofiles.com/jivit)|0.377151|
|**374**|[Iron-tom](https://psnprofiles.com/Iron-tom)|0.376021|
|**375**|[zombiekill56](https://psnprofiles.com/zombiekill56)|0.374709|
|**376**|[DrBryan_lzh](https://psnprofiles.com/DrBryan_lzh)|0.372777|
|**377**|[Lord_Pecho](https://psnprofiles.com/Lord_Pecho)|0.37262|
|**378**|[eYeRsmErT](https://psnprofiles.com/eYeRsmErT)|0.371383|
|**379**|[HomelessMilkman](https://psnprofiles.com/HomelessMilkman)|0.370628|
|**380**|[Ruy1990](https://psnprofiles.com/Ruy1990)|0.370556|
|**381**|[malmarius](https://psnprofiles.com/malmarius)|0.369993|
|**382**|[vicejef22](https://psnprofiles.com/vicejef22)|0.368901|
|**383**|[killerblackbird2](https://psnprofiles.com/killerblackbird2)|0.368161|
|**384**|[piff167](https://psnprofiles.com/piff167)|0.363966|
|**385**|[Cipriextref](https://psnprofiles.com/Cipriextref)|0.363233|
|**386**|[Krobyte](https://psnprofiles.com/Krobyte)|0.359754|
|**387**|[Seryoga90](https://psnprofiles.com/Seryoga90)|0.35872|
|**388**|[CHRISTMAStheRICH](https://psnprofiles.com/CHRISTMAStheRICH)|0.358632|
|**389**|[Silverlml](https://psnprofiles.com/Silverlml)|0.358395|
|**390**|[MATIDA4649](https://psnprofiles.com/MATIDA4649)|0.35773|
|**391**|[sty22](https://psnprofiles.com/sty22)|0.357112|
|**392**|[Tremor76](https://psnprofiles.com/Tremor76)|0.356984|
|**393**|[graham20](https://psnprofiles.com/graham20)|0.356646|
|**394**|[Thorazer](https://psnprofiles.com/Thorazer)|0.356141|
|**395**|[Lightningnitro](https://psnprofiles.com/Lightningnitro)|0.351412|
|**396**|[OgamiSakura](https://psnprofiles.com/OgamiSakura)|0.351351|
|**397**|[ShimattaShishou](https://psnprofiles.com/ShimattaShishou)|0.350459|
|**398**|[SamuraiKnott](https://psnprofiles.com/SamuraiKnott)|0.34828|
|**399**|[Noob_Slayer862](https://psnprofiles.com/Noob_Slayer862)|0.347275|
|**400**|[chenli2011](https://psnprofiles.com/chenli2011)|0.346896|
|**401**|[dev789](https://psnprofiles.com/dev789)|0.346485|
|**402**|[REDEYES123](https://psnprofiles.com/REDEYES123)|0.346478|
|**403**|[Scarface21291](https://psnprofiles.com/Scarface21291)|0.345563|
|**404**|[imperial_dragon9](https://psnprofiles.com/imperial_dragon9)|0.345059|
|**405**|[Boudrix](https://psnprofiles.com/Boudrix)|0.343791|
|**406**|[Kemikalz](https://psnprofiles.com/Kemikalz)|0.342903|
|**407**|[Gabrek](https://psnprofiles.com/Gabrek)|0.342344|
|**408**|[simplyJaeMar68](https://psnprofiles.com/simplyJaeMar68)|0.339926|
|**409**|[Niko7190](https://psnprofiles.com/Niko7190)|0.337975|
|**410**|[adi_1987bvb](https://psnprofiles.com/adi_1987bvb)|0.332872|
|**411**|[Rixa](https://psnprofiles.com/Rixa)|0.332301|
|**412**|[kratosdom](https://psnprofiles.com/kratosdom)|0.33217|
|**413**|[Caged-Izm](https://psnprofiles.com/Caged-Izm)|0.33168|
|**414**|[TheReal_Seal](https://psnprofiles.com/TheReal_Seal)|0.331388|
|**415**|[wayne30463](https://psnprofiles.com/wayne30463)|0.330365|
|**416**|[ArnoJr](https://psnprofiles.com/ArnoJr)|0.328284|
|**417**|[nishiyan_0214](https://psnprofiles.com/nishiyan_0214)|0.327311|
|**418**|[sonicwolf14](https://psnprofiles.com/sonicwolf14)|0.325957|
|**419**|[Rijckaert](https://psnprofiles.com/Rijckaert)|0.325108|
|**420**|[TheRaptor59](https://psnprofiles.com/TheRaptor59)|0.324546|
|**421**|[GrimGecko2](https://psnprofiles.com/GrimGecko2)|0.321246|
|**422**|[ozzycat](https://psnprofiles.com/ozzycat)|0.319761|
|**423**|[Makepu](https://psnprofiles.com/Makepu)|0.317588|
|**424**|[Keivan-K1](https://psnprofiles.com/Keivan-K1)|0.317001|
|**425**|[Zanchos](https://psnprofiles.com/Zanchos)|0.316656|
|**426**|[BrHoMZ](https://psnprofiles.com/BrHoMZ)|0.315582|
|**427**|[MorbiousVampire](https://psnprofiles.com/MorbiousVampire)|0.31405|
|**428**|[ELeMeNTs_Heart](https://psnprofiles.com/ELeMeNTs_Heart)|0.310225|
|**429**|[xHALIL](https://psnprofiles.com/xHALIL)|0.309861|
|**430**|[SN0ORz](https://psnprofiles.com/SN0ORz)|0.309549|
|**431**|[yaoyao_1222](https://psnprofiles.com/yaoyao_1222)|0.309253|
|**432**|[Sam_link](https://psnprofiles.com/Sam_link)|0.30894|
|**433**|[Vosen111](https://psnprofiles.com/Vosen111)|0.308699|
|**434**|[kegking2012](https://psnprofiles.com/kegking2012)|0.308081|
|**435**|[xMiZLe](https://psnprofiles.com/xMiZLe)|0.307758|
|**436**|[KeBrOcK_RaGe](https://psnprofiles.com/KeBrOcK_RaGe)|0.30357|
|**437**|[syam22](https://psnprofiles.com/syam22)|0.302093|
|**438**|[fndmntl](https://psnprofiles.com/fndmntl)|0.299365|
|**439**|[sylv514](https://psnprofiles.com/sylv514)|0.298569|
|**440**|[eggacton1982](https://psnprofiles.com/eggacton1982)|0.298202|
|**441**|[micky2344](https://psnprofiles.com/micky2344)|0.296026|
|**442**|[Syrupman](https://psnprofiles.com/Syrupman)|0.295542|
|**443**|[yaichi1282](https://psnprofiles.com/yaichi1282)|0.295382|
|**444**|[philgeburt](https://psnprofiles.com/philgeburt)|0.292185|
|**445**|[azmatik01](https://psnprofiles.com/azmatik01)|0.290856|
|**446**|[NightmareNoob79](https://psnprofiles.com/NightmareNoob79)|0.289509|
|**447**|[Skylar-Trey](https://psnprofiles.com/Skylar-Trey)|0.288885|
|**448**|[atsu228](https://psnprofiles.com/atsu228)|0.288595|
|**449**|[Enter_The_Kraken](https://psnprofiles.com/Enter_The_Kraken)|0.287274|
|**450**|[kyo_das](https://psnprofiles.com/kyo_das)|0.287096|
|**451**|[mocke](https://psnprofiles.com/mocke)|0.286|
|**452**|[Elbowdan](https://psnprofiles.com/Elbowdan)|0.285217|
|**453**|[Deja_Fox](https://psnprofiles.com/Deja_Fox)|0.284886|
|**454**|[marc581](https://psnprofiles.com/marc581)|0.283372|
|**455**|[MATANEKO](https://psnprofiles.com/MATANEKO)|0.282481|
|**456**|[BK201kuro](https://psnprofiles.com/BK201kuro)|0.281928|
|**457**|[mo-ti](https://psnprofiles.com/mo-ti)|0.280996|
|**458**|[unk2001](https://psnprofiles.com/unk2001)|0.280705|
|**459**|[kumaaggai](https://psnprofiles.com/kumaaggai)|0.279828|
|**460**|[killfreak421](https://psnprofiles.com/killfreak421)|0.273486|
|**461**|[EkinTR](https://psnprofiles.com/EkinTR)|0.273434|
|**462**|[HZAZI](https://psnprofiles.com/HZAZI)|0.273084|
|**463**|[Squall98](https://psnprofiles.com/Squall98)|0.270871|
|**464**|[iburst5](https://psnprofiles.com/iburst5)|0.269706|
|**465**|[george_13](https://psnprofiles.com/george_13)|0.268636|
|**466**|[TommersBomberz](https://psnprofiles.com/TommersBomberz)|0.268188|
|**467**|[ColonelHamFist](https://psnprofiles.com/ColonelHamFist)|0.267775|
|**468**|[MakoMW](https://psnprofiles.com/MakoMW)|0.267635|
|**469**|[amanotCZ](https://psnprofiles.com/amanotCZ)|0.267078|
|**470**|[miss1204](https://psnprofiles.com/miss1204)|0.26688|
|**471**|[scras9](https://psnprofiles.com/scras9)|0.265433|
|**472**|[spongebelt](https://psnprofiles.com/spongebelt)|0.265031|
|**473**|[X_Emperios_X](https://psnprofiles.com/X_Emperios_X)|0.265031|
|**474**|[er1234gio](https://psnprofiles.com/er1234gio)|0.263139|
|**475**|[D3viLSoh](https://psnprofiles.com/D3viLSoh)|0.263047|
|**476**|[RAZIEL_XRS](https://psnprofiles.com/RAZIEL_XRS)|0.26254|
|**477**|[The0End](https://psnprofiles.com/The0End)|0.259653|
|**478**|[DropeRj](https://psnprofiles.com/DropeRj)|0.257241|
|**479**|[DaRkWiLd92](https://psnprofiles.com/DaRkWiLd92)|0.256576|
|**480**|[G_R__M](https://psnprofiles.com/G_R__M)|0.256333|
|**481**|[figus87](https://psnprofiles.com/figus87)|0.255743|
|**482**|[mikeci591](https://psnprofiles.com/mikeci591)|0.255637|
|**483**|[DjRazielx](https://psnprofiles.com/DjRazielx)|0.255512|
|**484**|[itsmykeeeyo](https://psnprofiles.com/itsmykeeeyo)|0.253933|
|**485**|[super_turbo_pig](https://psnprofiles.com/super_turbo_pig)|0.253717|
|**486**|[kq22](https://psnprofiles.com/kq22)|0.253562|
|**487**|[bokan0124](https://psnprofiles.com/bokan0124)|0.253041|
|**488**|[Narak_93](https://psnprofiles.com/Narak_93)|0.252957|
|**489**|[NearJ](https://psnprofiles.com/NearJ)|0.252379|
|**490**|[hama-puro](https://psnprofiles.com/hama-puro)|0.250047|
|**491**|[Vegi85](https://psnprofiles.com/Vegi85)|0.248935|
|**492**|[Thiagobharaujo](https://psnprofiles.com/Thiagobharaujo)|0.247473|
|**493**|[cgv2010](https://psnprofiles.com/cgv2010)|0.246214|
|**494**|[khimey](https://psnprofiles.com/khimey)|0.246001|
|**495**|[satu001](https://psnprofiles.com/satu001)|0.245207|
|**496**|[highlord](https://psnprofiles.com/highlord)|0.243645|
|**497**|[AvatarWan777](https://psnprofiles.com/AvatarWan777)|0.24364|
|**498**|[hammy78](https://psnprofiles.com/hammy78)|0.24252|
|**499**|[JohnWalker12](https://psnprofiles.com/JohnWalker12)|0.241128|
|**500**|[daffhiro](https://psnprofiles.com/daffhiro)|0.240256|
|**501**|[razzael79](https://psnprofiles.com/razzael79)|0.23549|
|**502**|[aIlast0r](https://psnprofiles.com/aIlast0r)|0.232779|
|**503**|[Geoff26194](https://psnprofiles.com/Geoff26194)|0.232539|
|**504**|[Scrafttech](https://psnprofiles.com/Scrafttech)|0.231699|
|**505**|[gildardogomez](https://psnprofiles.com/gildardogomez)|0.228977|
|**506**|[DragonSenSei](https://psnprofiles.com/DragonSenSei)|0.227719|
|**507**|[Dead_Pizza](https://psnprofiles.com/Dead_Pizza)|0.226013|
|**508**|[WonderCrump](https://psnprofiles.com/WonderCrump)|0.225688|
|**509**|[MikeJones7788](https://psnprofiles.com/MikeJones7788)|0.225557|
|**510**|[Wax743](https://psnprofiles.com/Wax743)|0.224999|
|**511**|[Uzineo](https://psnprofiles.com/Uzineo)|0.222819|
|**512**|[BlackkMagik91](https://psnprofiles.com/BlackkMagik91)|0.221474|
|**513**|[AtomicNishikado](https://psnprofiles.com/AtomicNishikado)|0.221453|
|**514**|[kainerd](https://psnprofiles.com/kainerd)|0.22127|
|**515**|[FinaCrash96](https://psnprofiles.com/FinaCrash96)|0.220361|
|**516**|[Marcellous66](https://psnprofiles.com/Marcellous66)|0.220032|
|**517**|[Lazygamer3000](https://psnprofiles.com/Lazygamer3000)|0.219898|
|**518**|[nicmustwin](https://psnprofiles.com/nicmustwin)|0.219489|
|**519**|[spikeymikey999](https://psnprofiles.com/spikeymikey999)|0.218957|
|**520**|[diablostallion85](https://psnprofiles.com/diablostallion85)|0.217973|
|**521**|[Mr-AboWaLeeD-Q8](https://psnprofiles.com/Mr-AboWaLeeD-Q8)|0.217201|
|**522**|[yaya_ko](https://psnprofiles.com/yaya_ko)|0.216375|
|**523**|[a2334ypanda](https://psnprofiles.com/a2334ypanda)|0.213483|
|**524**|[Vishael](https://psnprofiles.com/Vishael)|0.212284|
|**525**|[Pilote29](https://psnprofiles.com/Pilote29)|0.212199|
|**526**|[Hurri105](https://psnprofiles.com/Hurri105)|0.208575|
|**527**|[Rafel87](https://psnprofiles.com/Rafel87)|0.205804|
|**528**|[alexgrig](https://psnprofiles.com/alexgrig)|0.205693|
|**529**|[The_Vespillo](https://psnprofiles.com/The_Vespillo)|0.204636|
|**530**|[RDsix3](https://psnprofiles.com/RDsix3)|0.201202|
|**531**|[jhoon2760](https://psnprofiles.com/jhoon2760)|0.197388|
|**532**|[SammiiDoogles](https://psnprofiles.com/SammiiDoogles)|0.196725|
|**533**|[CtrlADelce](https://psnprofiles.com/CtrlADelce)|0.196583|
|**534**|[ELENTIRR](https://psnprofiles.com/ELENTIRR)|0.195294|
|**535**|[bibiffx](https://psnprofiles.com/bibiffx)|0.193863|
|**536**|[ernie237](https://psnprofiles.com/ernie237)|0.19049|
|**537**|[mnvb4](https://psnprofiles.com/mnvb4)|0.189544|
|**538**|[heroofpokke](https://psnprofiles.com/heroofpokke)|0.188613|
|**539**|[Geddoe](https://psnprofiles.com/Geddoe)|0.187248|
|**540**|[January39](https://psnprofiles.com/January39)|0.186653|
|**541**|[GAMENlAC](https://psnprofiles.com/GAMENlAC)|0.184643|
|**542**|[HitokagePan](https://psnprofiles.com/HitokagePan)|0.182314|
|**543**|[Firefox73](https://psnprofiles.com/Firefox73)|0.181343|
|**544**|[theoreme77](https://psnprofiles.com/theoreme77)|0.180027|
|**545**|[xthemusic](https://psnprofiles.com/xthemusic)|0.179513|
|**546**|[DocCSmith](https://psnprofiles.com/DocCSmith)|0.178419|
|**547**|[ThothBeyond](https://psnprofiles.com/ThothBeyond)|0.17592|
|**548**|[wargonefx](https://psnprofiles.com/wargonefx)|0.174974|
|**549**|[gramzik](https://psnprofiles.com/gramzik)|0.174867|
|**550**|[Simo-ita-rav82](https://psnprofiles.com/Simo-ita-rav82)|0.174233|
|**551**|[Ravenask](https://psnprofiles.com/Ravenask)|0.173548|
|**552**|[DuckSwimmer](https://psnprofiles.com/DuckSwimmer)|0.170264|
|**553**|[ClockworkTango](https://psnprofiles.com/ClockworkTango)|0.170258|
|**554**|[BeardedWizard69](https://psnprofiles.com/BeardedWizard69)|0.168552|
|**555**|[shinsincostan](https://psnprofiles.com/shinsincostan)|0.168328|
|**556**|[chris1029](https://psnprofiles.com/chris1029)|0.168231|
|**557**|[Atriedi](https://psnprofiles.com/Atriedi)|0.164898|
|**558**|[MilitantRacoons](https://psnprofiles.com/MilitantRacoons)|0.161229|
|**559**|[shivabboy](https://psnprofiles.com/shivabboy)|0.159565|
|**560**|[REJIOSTAR](https://psnprofiles.com/REJIOSTAR)|0.158291|
|**561**|[Urika_Nemrick](https://psnprofiles.com/Urika_Nemrick)|0.156971|
|**562**|[SvieriOnNaamat](https://psnprofiles.com/SvieriOnNaamat)|0.156407|
|**563**|[Kingdom](https://psnprofiles.com/Kingdom)|0.153429|
|**564**|[scrotiecraptaco](https://psnprofiles.com/scrotiecraptaco)|0.152443|
|**565**|[HavaGess](https://psnprofiles.com/HavaGess)|0.151705|
|**566**|[K_A_O_S_9_0_0](https://psnprofiles.com/K_A_O_S_9_0_0)|0.15169|
|**567**|[palmu124](https://psnprofiles.com/palmu124)|0.150776|
|**568**|[BackFabric](https://psnprofiles.com/BackFabric)|0.150697|
|**569**|[anime21](https://psnprofiles.com/anime21)|0.149549|
|**570**|[Hojny](https://psnprofiles.com/Hojny)|0.148754|
|**571**|[elSpaniard](https://psnprofiles.com/elSpaniard)|0.148603|
|**572**|[Coach_Black](https://psnprofiles.com/Coach_Black)|0.148418|
|**573**|[Pa1ny3](https://psnprofiles.com/Pa1ny3)|0.145434|
|**574**|[samuraijock](https://psnprofiles.com/samuraijock)|0.145418|
|**575**|[BiiG_RiiG_nZ](https://psnprofiles.com/BiiG_RiiG_nZ)|0.144372|
|**576**|[bany_rock](https://psnprofiles.com/bany_rock)|0.14327|
|**577**|[bloodclross](https://psnprofiles.com/bloodclross)|0.13877|
|**578**|[SmoQ1988_PL](https://psnprofiles.com/SmoQ1988_PL)|0.137559|
|**579**|[nstarkey13](https://psnprofiles.com/nstarkey13)|0.136766|
|**580**|[Digital_Plissken](https://psnprofiles.com/Digital_Plissken)|0.136388|
|**581**|[gujeps](https://psnprofiles.com/gujeps)|0.135958|
|**582**|[PyroVanHuk](https://psnprofiles.com/PyroVanHuk)|0.134382|
|**583**|[finash](https://psnprofiles.com/finash)|0.134231|
|**584**|[Andrashi](https://psnprofiles.com/Andrashi)|0.130194|
|**585**|[Sarago](https://psnprofiles.com/Sarago)|0.129278|
|**586**|[Aesculap](https://psnprofiles.com/Aesculap)|0.128839|
|**587**|[RykkChii](https://psnprofiles.com/RykkChii)|0.128832|
|**588**|[Mental-Guy](https://psnprofiles.com/Mental-Guy)|0.128576|
|**589**|[greencivico](https://psnprofiles.com/greencivico)|0.12836|
|**590**|[deadburger101](https://psnprofiles.com/deadburger101)|0.127878|
|**591**|[Leek_Chain](https://psnprofiles.com/Leek_Chain)|0.124637|
|**592**|[kamenriderzx2](https://psnprofiles.com/kamenriderzx2)|0.124351|
|**593**|[Rartastic](https://psnprofiles.com/Rartastic)|0.12425|
|**594**|[ACES122](https://psnprofiles.com/ACES122)|0.124131|
|**595**|[Doyasa](https://psnprofiles.com/Doyasa)|0.122731|
|**596**|[leaf_lettuce](https://psnprofiles.com/leaf_lettuce)|0.12251|
|**597**|[BeAsTxGeNEs](https://psnprofiles.com/BeAsTxGeNEs)|0.121724|
|**598**|[ARIOS520](https://psnprofiles.com/ARIOS520)|0.118729|
|**599**|[HTMLlRulezDOOd](https://psnprofiles.com/HTMLlRulezDOOd)|0.117545|
|**600**|[Narucist](https://psnprofiles.com/Narucist)|0.117381|
|**601**|[zkani](https://psnprofiles.com/zkani)|0.117317|
|**602**|[Nineduce12](https://psnprofiles.com/Nineduce12)|0.117145|
|**603**|[aliasbob](https://psnprofiles.com/aliasbob)|0.115678|
|**604**|[MultiBorstel](https://psnprofiles.com/MultiBorstel)|0.115392|
|**605**|[Francescoc](https://psnprofiles.com/Francescoc)|0.114438|
|**606**|[millerna](https://psnprofiles.com/millerna)|0.113928|
|**607**|[JDM_4EVA](https://psnprofiles.com/JDM_4EVA)|0.11324|
|**608**|[Nordsjelesagn](https://psnprofiles.com/Nordsjelesagn)|0.11159|
|**609**|[hamateaonline](https://psnprofiles.com/hamateaonline)|0.110642|
|**610**|[Paolobaran88](https://psnprofiles.com/Paolobaran88)|0.110599|
|**611**|[RobsonCbjr013](https://psnprofiles.com/RobsonCbjr013)|0.110208|
|**612**|[WolfgangTesla](https://psnprofiles.com/WolfgangTesla)|0.109628|
|**613**|[Cerebus519](https://psnprofiles.com/Cerebus519)|0.106374|
|**614**|[deadshot01](https://psnprofiles.com/deadshot01)|0.104438|
|**615**|[dbigfeet](https://psnprofiles.com/dbigfeet)|0.104256|
|**616**|[RIOGA6](https://psnprofiles.com/RIOGA6)|0.0990152|
|**617**|[GonzaDF](https://psnprofiles.com/GonzaDF)|0.0989485|
|**618**|[OMGaPandaXD](https://psnprofiles.com/OMGaPandaXD)|0.0984678|
|**619**|[luluxiong](https://psnprofiles.com/luluxiong)|0.0963021|
|**620**|[StickPersonXIII](https://psnprofiles.com/StickPersonXIII)|0.0949428|
|**621**|[HaTtoRi-HanZo-o](https://psnprofiles.com/HaTtoRi-HanZo-o)|0.0939843|
|**622**|[ZeroSuitSamus774](https://psnprofiles.com/ZeroSuitSamus774)|0.0917376|
|**623**|[man_hunt_](https://psnprofiles.com/man_hunt_)|0.0910265|
|**624**|[TORETO999](https://psnprofiles.com/TORETO999)|0.090854|
|**625**|[xgordox](https://psnprofiles.com/xgordox)|0.0889232|
|**626**|[BBQJimRoss](https://psnprofiles.com/BBQJimRoss)|0.0877793|
|**627**|[djsuperjim](https://psnprofiles.com/djsuperjim)|0.0801642|
|**628**|[fdirksen](https://psnprofiles.com/fdirksen)|0.079893|
|**629**|[NutJockey](https://psnprofiles.com/NutJockey)|0.0771788|
|**630**|[Takdan](https://psnprofiles.com/Takdan)|0.0713305|
|**631**|[iiihataiii](https://psnprofiles.com/iiihataiii)|0.0713088|
|**632**|[codchamp987](https://psnprofiles.com/codchamp987)|0.0705767|
|**633**|[zabek87](https://psnprofiles.com/zabek87)|0.0677254|
|**634**|[SangoKun](https://psnprofiles.com/SangoKun)|0.0641607|
|**635**|[vapor7skill](https://psnprofiles.com/vapor7skill)|0.0640184|
|**636**|[dejhro](https://psnprofiles.com/dejhro)|0.0622147|
|**637**|[mata4133](https://psnprofiles.com/mata4133)|0.0620947|
|**638**|[Steeve85000](https://psnprofiles.com/Steeve85000)|0.0619822|
|**639**|[Blade133](https://psnprofiles.com/Blade133)|0.0608763|
|**640**|[jloudermilk](https://psnprofiles.com/jloudermilk)|0.058309|
|**641**|[Cenarius_BR](https://psnprofiles.com/Cenarius_BR)|0.0577302|
|**642**|[pos_b](https://psnprofiles.com/pos_b)|0.0561508|
|**643**|[Hyper_Sonic618](https://psnprofiles.com/Hyper_Sonic618)|0.0526212|
|**644**|[kylito627](https://psnprofiles.com/kylito627)|0.0520464|
|**645**|[Arnos_NL](https://psnprofiles.com/Arnos_NL)|0.0517371|
|**646**|[Mephis](https://psnprofiles.com/Mephis)|0.0507108|
|**647**|[kerrak](https://psnprofiles.com/kerrak)|0.0495425|
|**648**|[klovers1104](https://psnprofiles.com/klovers1104)|0.0471682|
|**649**|[SleepTime96](https://psnprofiles.com/SleepTime96)|0.0466694|
|**650**|[jack03039](https://psnprofiles.com/jack03039)|0.0465287|
|**651**|[mayhemiza80sbaby](https://psnprofiles.com/mayhemiza80sbaby)|0.0440051|
|**652**|[Warlord13179](https://psnprofiles.com/Warlord13179)|0.0431127|
|**653**|[nataku601](https://psnprofiles.com/nataku601)|0.041965|
|**654**|[Dbl-Tap45](https://psnprofiles.com/Dbl-Tap45)|0.0416107|
|**655**|[ZhouJieLun](https://psnprofiles.com/ZhouJieLun)|0.0399596|
|**656**|[XxPeRaLo_98xX](https://psnprofiles.com/XxPeRaLo_98xX)|0.037044|
|**657**|[jossi71](https://psnprofiles.com/jossi71)|0.035159|
|**658**|[Sumoomoo](https://psnprofiles.com/Sumoomoo)|0.0341896|
|**659**|[Luisoz690](https://psnprofiles.com/Luisoz690)|0.0341769|
|**660**|[RyuDragon1](https://psnprofiles.com/RyuDragon1)|0.0302458|
|**661**|[MikeDaElder](https://psnprofiles.com/MikeDaElder)|0.0288706|
|**662**|[KIKTRA-MEX](https://psnprofiles.com/KIKTRA-MEX)|0.0279797|
|**663**|[gundam-uc](https://psnprofiles.com/gundam-uc)|0.0262448|
|**664**|[brownleaf](https://psnprofiles.com/brownleaf)|0.0251116|
|**665**|[hard-pulse3](https://psnprofiles.com/hard-pulse3)|0.0231001|
|**666**|[Prince1994](https://psnprofiles.com/Prince1994)|0.022565|
|**667**|[mclovin182567](https://psnprofiles.com/mclovin182567)|0.0169469|
|**668**|[kittkatzz0](https://psnprofiles.com/kittkatzz0)|0.0147491|
|**669**|[Jamezu](https://psnprofiles.com/Jamezu)|0.0123662|
|**670**|[killerwampa97](https://psnprofiles.com/killerwampa97)|0.00943843|
|**671**|[KenyonMcDohl](https://psnprofiles.com/KenyonMcDohl)|0.00898914|
|**672**|[GoodrichGuy](https://psnprofiles.com/GoodrichGuy)|0.00840968|
|**673**|[cracer369](https://psnprofiles.com/cracer369)|0.00572137|
|**674**|[nomi1998](https://psnprofiles.com/nomi1998)|0.00474048|
|**675**|[Cash3785](https://psnprofiles.com/Cash3785)|0.00254487|
