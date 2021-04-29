//
//  main.cpp
//  MineSweeper
//
//  Created by Kevin Liu on 2021/4/28.
//

#include <iostream>
#include <vector>
#include "MineSys.h" //扫雷系统
#include "OLSys.h"  //联机系统
#include <time.h>
using namespace std;
//目前main主要用于测试代码
int main() {
    string inpcmd;
    vector<OPR> stepOPRs;
    //主循环
    while (true){
        //开始新游戏
        cout<<"NewGame!"<<endl;
        stepOPRs=getUsrOprs();//保证了stepOPR>0
        //注意，应当保证第一步所有的操作为1
        vector<POS> startPOS;
        startPOS.clear();
        for (int i=0;i<stepOPRs.size();i++){
            startPOS.push_back(stepOPRs[i]._pos);
        }
        FIELD baseField=FIELD();
        baseField.Create(default_num_of_mines, startPOS);//此步创建了新的局面
        for (int i=0;i<stepOPRs.size();i++){
            baseField.Click(startPOS[i]);
        }
        //以下进入各步的循环
        bool is_game_continue=true;
        bool is_win=false;
        while (is_game_continue){
            baseField.Show();
            stepOPRs=getUsrOprs();//获取所有的操作（目前仅支持xx-yy-oprType）
            for (int i=0;i<stepOPRs.size();i++){
                is_game_continue=baseField.Operation(stepOPRs[i]);
            }
            //mineRemain==0时，判断是否获胜
            if (baseField.mine_remain==0){
                if (is_win==baseField.isWin()){
                    is_game_continue=false;
                }
            }
        }
        if (is_win){
            //获胜
            cout<<"WIN!"<<endl;
        }
        baseField.EndGameShow();
        cout<<endl<<endl;
        sleep(5000);
        
    }

    
    
    
    
    return 0;
}

