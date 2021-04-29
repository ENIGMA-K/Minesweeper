//
//  MineSys.h
//  MineSweeper
//
//  Created by Kevin Liu on 2021/4/28.
//

#ifndef MineSys_h
#define MineSys_h
#include <vector>
#include "math.h"
#include <time.h>
#include <regex>
using namespace std;
//MARK:base methods
//延时函数
void sleep(double _duration){
    struct timespec ts, ts1;
    ts.tv_nsec =(_duration-floor(_duration))*1000000000;
    ts.tv_sec = floor(_duration);
    nanosleep(&ts, &ts1);
}
//随机数(0 to uper)
int rnd(int uper)
{
    srand((int)time(NULL)*rand());
    return (rand()%uper);
};
//安全释放指针(被逼无奈)
template <class T>
inline void SafeDelete(T*& pVal)
{
    if (pVal != NULL)
    {
        delete pVal;
        pVal = NULL;
    }
}
template <class T>
inline void SafeDeleteArray(T*& pVal)
{
    if (pVal != NULL)
    {
        delete[] pVal;
        pVal = NULL;
    }
}
//生成随机序列/*n个里面取m个，其中第 non 个不取，non共有k个*/
//这里没有做合理性检验，基本要求：non中不出现重复，且范围属于0~n-1
bool* select(int m,int n,vector<int> non){
    bool *_opt=new bool[n];
    //初始化输出，false为空；
    for (int _i=0;_i<n;_i++){
        *(_opt+_i)=false;
    }
    //slc为选中的雷的位置（从左到右，从上到下）
    int sel[m];
    //第一：生成每一个雷所在的位置顺序，其中不包括non的位置
    for (int _i=0;_i<m;_i++){
        sel[_i]=rnd(n-m+_i-int(non.size())+1);
    }
    for (int _i=0;_i<m;_i++){
        for (int _j=0;_j<_i;_j++){
            if (sel[_j]>=sel[_i]){
                sel[_j]++;
            }
        }
    }
    //第二：对于所有的雷点，如果其前有需要隔离的选择点，则将其加上
    for (int _i=0;_i<m;_i++){
        for (int _j=0;_j<non.size();_j++){
            if (sel[_i]>=non[_j]){
                sel[_i]++;
            }
        }
    }
    //第三：雷顺序值转换为bool列表
    for (int _i=0;_i<m;_i++){
        *(_opt+sel[_i])=true;
    }
//    for (int _i=0;_i<n;_i++){
//        cout<<*(_opt+_i)<<"\t";
//    }
//    cout<<endl;
    return _opt;
}

//MARK:MineSite（雷点，即局面上的每一个小格）
class MINESITE{
public:
    int mark=0;//标记的数值或雷（雷记为-1）
    bool isReveal=false;//是否开过
    bool isFlaged=false;//是否插旗
    //特别注意，Reveal和Flagged最多只有一个是真值
public:
    MINESITE();//初始化
    MINESITE(int UseMark);//初始化（按照数值初始化）
    char show();//揭露
};
MINESITE::MINESITE(){
    mark=0;
    isReveal=false;
    isFlaged=false;
}
MINESITE::MINESITE(int UseMark){
    if (UseMark>=-1&&UseMark<=8){
        mark=UseMark;
    }else{
        mark=0;
    }
    isReveal=false;
    isFlaged=false;
}
char MINESITE::show(){
    char _opt=' ';//输出字段
    if (isFlaged){
        return '!';
    }
    if (mark==-1/*是雷*/){
        _opt='*';
    } else if (mark==0/*为空*/){
        _opt=' ';
    } else{
        _opt=mark+48;
    }
    return _opt;
}

//MARK:type Pos（定义的是每一个雷的坐标）
typedef struct POS{
    int __x=0;
    int __y=0;
}POS;
//雷的坐标点与POS转换应当放在雷场大小确定以后，mark一下
//操作
typedef struct OPR{
    POS _pos={-1,-1};
    int _oprType=-1;//1=Click,3=DoubleClik,5=RightClick
}OPR;

// MARK:一般游戏设置
POS default_size={16,30};
int default_num_of_mines=99;
bool default_NF=false;

// MARK:field（整个局面，称为雷场）
class FIELD{
public:
    int mine_remain;//剩余雷数
    int size_x;//行数
    int size_y;//列数
    vector<vector<MINESITE>> field;//使用vector的意义主要是稳定，避免指针出现不可控的异常
public:
    FIELD();
    FIELD(int _x,int _y,int _mine);//初始化（按照大小）
    //此处为雷的坐标点与POS转换
    int Trans(POS _Ppos);
    POS Trans(int _Ipos);
    void Create(int NumOfMines,vector<POS> non);//以雷数生成游戏,其中所有的non保证无雷
    POS Click(POS _click);//点击无雷区域(返回炸雷的点/或-1,-1）（该值不能作为ai算法中的中间步骤，迭代回传除外）
    bool FlagIt(POS _mine);//标记雷点(标错为false）（该值不能作为ai算法中的中间步骤，迭代回传除外）
    bool UnFlagIt(POS _mine);//移出雷点(拔错为FALSE）（该值不能作为ai算法中的中间步骤，迭代回传除外）
    POS DoubleClick(POS _click);//开一圈(返回炸雷的点）（该值不能作为ai算法中的中间步骤，迭代回传除外）
    void Show(bool solve=true);//true为解题视角，false 为全局视角
    bool Operation(OPR _opr);//每一步操作，合理返回true,错误返回FALSE
    void EndGameShow();//有结束时的输出
    bool isWin();//判断是否获胜
};
FIELD::FIELD(){
    mine_remain=default_num_of_mines;
    size_x=default_size.__x;
    size_y=default_size.__y;
    field.clear();
    for (int _i=0;_i<size_x;_i++){
        vector<MINESITE> use_to_push;
        for (int _j=0;_j<size_y;_j++){
            MINESITE UTP;
            use_to_push.push_back(UTP);
        }
        field.push_back(use_to_push);
    }
}
FIELD::FIELD(int _x,int _y,int _mine){
    mine_remain=_mine;
    size_x=_x;
    size_y=_y;
    field.clear();
    for (int _i=0;_i<size_x;_i++){
        vector<MINESITE> use_to_push;
        for (int _j=0;_j<size_y;_j++){
            MINESITE UTP;
            use_to_push.push_back(UTP);
        }
        field.push_back(use_to_push);
    }
    for (int _i=0;_i<_x;_i++){
        vector<MINESITE> _k;
        _k.clear();
        for (int _j=0;_j<_y;_j++){
            MINESITE _l=MINESITE();
            _k.push_back(_l);
        }
        field.push_back(_k);
    }
}
//此处为雷的坐标点与POS转换
int FIELD::Trans(POS _Ppos){
    int _Ipos=0;
    _Ipos=_Ppos.__x*size_y+_Ppos.__y;
    return _Ipos;
}
POS FIELD::Trans(int _Ipos){
    POS _Ppos;
    _Ppos.__x=_Ipos/size_y;
    _Ppos.__y=_Ipos%size_y;
    return _Ppos;
}
//以雷数生成游戏,其中所有的non保证无雷
void FIELD::Create(int NumOfMines,vector<POS> non){
    bool *MineId=new bool[size_x*size_y];
    vector<int> NonIds;
    NonIds.clear();
    for (int _i=0;_i<non.size();_i++){
        NonIds.push_back(Trans(non[_i]));
    }
    //获得所有雷的位置
    MineId=select(NumOfMines, size_x*size_y,NonIds);
    //field填入所有的雷
//    for (int _i=0;_i<size_x*size_y;_i++){
//        cout<<*(MineId+_i)<<"\t";
//    }
//    cout<<endl;
    for (int _i=0;_i<size_x;_i++){
        for (int _j=0;_j<size_y;_j++){
            if (*(MineId+_i*size_y+_j)/*该点是雷*/){
                field[_i][_j].mark=-1;
            }else{
                field[_i][_j].mark=0;
            }
        }
    }
//    for (int _i=0;_i<size_x*size_y;_i++){
//        cout<<*(MineId+_i)<<"\t";
//    }
//    cout<<endl;
    SafeDeleteArray(MineId);
    //field计算其他的雷点数值
    for (int _i=0;_i<size_x;_i++){
        for (int _j=0;_j<size_y;_j++){
            if (field[_i][_j].mark!=-1/*如果点非雷*/){
                int count=0;
                for (int _m=-1;_m<2;_m++){
                    for (int _n=-1;_n<2;_n++){
                        if (((_i+_m)>=0)&&((_i+_m)<size_x)&&((_j+_n)>=0)&&((_j+_n)<size_y)){
                            if (field[_i+_m][_j+_n].mark==-1){
                                count++;
                            }
                        }
                    }
                }
                field[_i][_j].mark=count;
            }
        }
    }
    //完毕
}
//展示雷场（此命令为测试专用，UI需要直接打印FIELD对象//true为解题视角，false 为全局视角
void FIELD::Show(bool solve){
    if (solve/*解题视角下*/){
        for (int _i=0;_i<size_x;_i++){
            for (int _j=0;_j<size_y;_j++){
                if (field[_i][_j].isReveal){
                    cout<<field[_i][_j].mark<<"\t";
                }else if (field[_i][_j].isFlaged){
                    cout<<"!"<<"\t";
                }else{
                    cout<<"#"<<"\t";
                }
            }
            cout<<endl;
        }
    }else/*非解题视角*/{
        for (int _i=0;_i<size_x;_i++){
            for (int _j=0;_j<size_y;_j++){
                if (field[_i][_j].mark==-1){
                    cout<<"*"<<"\t";
                }else{
                    cout<<field[_i][_j].mark<<"\t";
                }
                
            }
            cout<<endl;
        }
    }
    cout<<"Num of mines remain:"<<mine_remain<<endl;
}
//单击操作
POS FIELD::Click(POS _click){
    POS _true={-1,-1};
    if (field[_click.__x][_click.__y].isFlaged||field[_click.__x][_click.__y].isReveal){
        return _true;//已经点击过或者已经插旗
    }
    if (field[_click.__x][_click.__y].mark==-1/*踩雷了*/){
        cout<<"oops!"<<endl;
        field[_click.__x][_click.__y].isReveal=true;
        return _click;
    }
    if (field[_click.__x][_click.__y].mark!=0/*点击点非空*/){
        field[_click.__x][_click.__y].isReveal=true;
        return _true;
    }
    if (field[_click.__x][_click.__y].mark==0/*点击点为空*/){
        field[_click.__x][_click.__y].isReveal=true;
        vector<POS> todoList;
        todoList.clear();
        //寻找周围的 nonReveal 点
        for (int perix=-1;perix<2;perix++){
            for (int periy=-1;periy<2;periy++){
                if (((perix+_click.__x)>=0)&&((perix+_click.__x)<size_x)&&((periy+_click.__y)>=0)&&((periy+_click.__y)<size_y)/*在范围内*/){
                    if (!field[perix+_click.__x][periy+_click.__y].isReveal/*未被揭示*/){
                        POS node;
                        node.__x=perix+_click.__x;
                        node.__y=periy+_click.__y;
                        todoList.push_back(node);
                    }
                }
            }
        }
        //迭代进入下一层
        for (int _i=0;_i<todoList.size();_i++){
            POS nextLevel=Click({todoList[_i].__x, todoList[_i].__y});
            if (nextLevel.__x!=_true.__x||nextLevel.__y!=_true.__y){
                return nextLevel;
            }
        }
        return _true;
    }
    return _true;
}
//双击操作
POS FIELD::DoubleClick(POS _click){
    POS _true={-1,-1};
    vector<POS> todoList;
    todoList.clear();
    //寻找周围的 nonReveal 点
    for (int perix=-1;perix<2;perix++){
        for (int periy=-1;periy<2;periy++){
            if (((perix+_click.__x)>=0)&&((perix+_click.__x)<size_x)&&((periy+_click.__y)>=0)&&((periy+_click.__y)<size_y)/*在范围内*/){
                if (!field[perix+_click.__x][periy+_click.__y].isReveal/*未被揭示*/){
                    POS node;
                    node.__x=perix+_click.__x;
                    node.__y=periy+_click.__y;
                    todoList.push_back(node);
                }
            }
        }
    }
    //迭代进入下一层
    for (int _i=0;_i<todoList.size();_i++){
        POS nextLevel=Click({todoList[_i].__x, todoList[_i].__y});
        if (nextLevel.__x!=_true.__x||nextLevel.__y!=_true.__y){
            return nextLevel;
        }
    }
    return _true;
}
//插旗
bool FIELD::FlagIt(POS _mine){
    if (field[_mine.__x][_mine.__y].isFlaged){
        return true;//已经插过旗
    }
    if (field[_mine.__x][_mine.__y].isReveal){
        return false;//已经揭示并且插旗错误
    }
    if (field[_mine.__x][_mine.__y].mark==-1){
        field[_mine.__x][_mine.__y].isFlaged=true;
        mine_remain-=1;
        return true;
    }else{
        field[_mine.__x][_mine.__y].isFlaged=true;
        mine_remain-=1;
        return false;
        
    }
}
//拔旗
bool FIELD::UnFlagIt(POS _mine){
    if (!field[_mine.__x][_mine.__y].isFlaged){
        return true;//并未插过旗
    }
    if (field[_mine.__x][_mine.__y].mark==-1){
        field[_mine.__x][_mine.__y].isFlaged=false;
        mine_remain++;
        return false;
    }else{
        field[_mine.__x][_mine.__y].isFlaged=false;
        mine_remain++;
        return true;
    }
}
//操作(注：不区分最后一个错误的点）
bool FIELD::Operation(OPR _opr){
    POS _true={-1,-1};
    POS _Presult;
    bool _Bresult=true;
    switch (_opr._oprType) {
        case 1:
            _Presult=Click(_opr._pos);
            if (_Presult.__x!=_true.__x||_Presult.__y!=_true.__y){
                _Bresult=false;
            }
            break;
            
        case 3:
            _Presult=DoubleClick(_opr._pos);
            if (_Presult.__x!=_true.__x||_Presult.__y!=_true.__y){
                _Bresult=false;
            }
            break;
            
        case 5:
            _Bresult=FlagIt(_opr._pos);
            break;
            
        case 7:
            _Bresult=UnFlagIt(_opr._pos);
            break;
            
        default:
            break;
    }
    return _Bresult;
}
//有结束时的输出
void FIELD::EndGameShow(){
    for (int _i=0;_i<size_x;_i++){
        for (int _j=0;_j<size_y;_j++){
            if (field[_i][_j].isReveal){
                if (field[_i][_j].mark==-1){
                    cout<<"X"<<"\t";
                }else{
                    cout<<field[_i][_j].mark<<"\t";
                }
            }else if (field[_i][_j].isFlaged){
                if (field[_i][_j].mark==-1){
                    cout<<"!"<<"\t";
                }else{
                    cout<<"N"<<"\t";
                }
            }else{
                cout<<"#"<<"\t";
            }
        }
        cout<<endl;
    }
    cout<<"Num of mines remain:"<<mine_remain<<endl;
}
//判断是否获胜
bool FIELD::isWin(){
    bool _isWin=true;
    int countTruePoint=0;
    for (int _i=0;_i<size_x;_i++){
        for (int _j=0;_j<size_y;_j++){
            if ((field[_i][_j].isReveal&&field[_i][_j].mark!=-1)||(field[_i][_j].isFlaged&&field[_i][_j].mark==-1)){
                countTruePoint++;
            }else{
                _isWin=false;
            }
        }
    }
    if (_isWin==false||countTruePoint!=size_x*size_y){
        return false;
    }else{
        return true;
    }
}



// MARK:Usr opra（用户操作）（正则表达式）
//mode 1: xx-yy-oprType
//Mode 2: xx-yy1+yy2+yy3-oprType
//Mode 3: xx1+xx2+xx3-yy-oprType
//
//获取多个操作表达式(自带获取输入,默认为cin途径)(作为应用时需要更改)
vector<OPR> getUsrOprs(){
    string _input;
    getline(cin,_input);
    vector<OPR> _opr;
    _opr.clear();
    regex pattern("\\d+-\\d+-\\d+");//默认操作数只取最后一位（1，3，5，7），但需要用多数字占位
    vector<string> matchBank;//原始匹配集
    matchBank.clear();
    for (sregex_iterator it(_input.begin(),_input.end(), pattern), end;     //end是尾后迭代器，regex_iterator是regex_iterator的string类型的版本
            it != end;++it){
            matchBank.push_back(it->str());
        }
    //查找合理opr
    for (int _i=0;_i<matchBank.size();_i++){
        OPR newOpr;
        newOpr._pos.__x=stoi(matchBank[_i].substr(0,matchBank[_i].find_first_of("-")));
        newOpr._pos.__y=stoi(matchBank[_i].substr(matchBank[_i].find_first_of("-")+1,matchBank[_i].find_last_of("-")-matchBank[_i].find_first_of("-")-1));
        newOpr._oprType=stoi(matchBank[_i].substr(matchBank[_i].find_last_of("-")+1,1));
        if (newOpr._oprType%2==1) _opr.push_back(newOpr);
    }
    if (_opr.size()<=0){
        cout<<"false input! enter again!"<<endl;
        _opr=getUsrOprs();
    }
    return _opr;
}





#endif /* MineSys_h */
