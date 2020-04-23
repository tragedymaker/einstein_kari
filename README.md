# Theory of Computer Games/2019 HW2
##  Einstein_kari
利用蒙第卡羅樹搜尋法找出恰當的步數  
另有助教提供random/conservative/greedy三種不同實力的agent供測試

## Rule
### How to move
紅方：右，下，右下   
藍方：左，上，左上

### How to win
吃光對手所有棋子 or 雙方皆有棋子抵達終點時，棋子編號較小者獲勝

### Note
1.任何棋子皆可吃其他棋子（無子力大小之分）  
2.若已無合法步（5死1終點）則可選擇pass，若有則強迫必須走

## Compile
make in baseline folder and game folder respectively  
get game,greedy,conservative,random,r07522316 

## Start to play
### AI v.s. myAI
./game -p0 ./[agentname] -p1 ./r07522316
### Human v.s. myAI
./game -p0 ./r07522316

以下影片為與greedy AI對戰情況，設定思考時間為10秒，影片為8倍速，我的AI是藍色player1  
![image](https://github.com/tragedymaker/einstein_kari/blob/master/einstein_kari.gif)
