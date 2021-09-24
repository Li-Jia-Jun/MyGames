using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using PokerNameSpace;

namespace RoleNameSpace
{
    //参与者类
    public abstract class Role : MonoBehaviour
    {
        private void Awake()
        {
            init();
        }
        //
        //初始化
        protected void init()
        {
            //初始化策略
            hints = new List<Pos>();
            //初始化手牌容器
            pokers = new Dictionary<int, List<Poker>>();
            for (int i = 1; i <= 14; i++)
                pokers.Add(i, new List<Poker>());

            isLandLord = false;
            isCounting = false;
            deltaTime = 0.0f;
            totalTime = 0;
            isLandLord = false;
            canRule = false;

            pokerManager = new PokerManager();
        }
        //
        //拿手牌，将cards[]转换为Dictionary<int,List<Poker>>的存储方式
        public virtual void getCards(int[] cards)
        {
            cardNum = cards.Length;
            for (int i = 0; i < cardNum; i++)
                pokers[cards[i] / 4 + 1].Add(new Poker(cards[i]));                     
        }
        //
        //获取地主的三张牌
        public virtual void getExtra3(int p1, int p2, int p3)
        {
            pokers[p1/ 4 + 1].Add(new Poker(p1));
            pokers[p2 / 4 + 1].Add(new Poker(p2));
            pokers[p3 / 4 + 1].Add(new Poker(p3));
            cardNum += 3;
        }
        //
        //出牌
        //可以则更改gameController的 lastGroup，将牌显示，删除牌，cardNum减小，return true
        //否则return false
        protected virtual void discard()
        {
            //首先清除上一次出牌
            for (int i = 0; i < outPlaces.Length; i++)
                outPlaces[i].GetComponent<SpriteRenderer>().sprite = null;
        }
        //
        //获取List<Pos>表示的PokerGroup
        protected PokerGroup getPokerGroup(List<Pos> list)
        {
            int count = list.Count;
            Poker[] p = new Poker[count];
            for (int i = 0; i < count; i++)
                p[i] = pokers[list[i].x][list[i].y];

            return pokerManager.getPokerType(p);
        }
        protected PokerGroup getPokerGroup(List<int> list)
        {
            int count = list.Count;
            Poker[] p = new Poker[count];
            for (int i = 0; i < count; i++)
                p[i] = new Poker(list[i]);

            return pokerManager.getPokerType(p);
        }
        //
        //开始计时
        protected void startCounting()
        {
            isCounting = true;

            deltaTime = 0;
            if (isLandLord) totalTime = 20;//地主出牌时间为20秒
            else                  totalTime = 15;

            timeText.GetComponent<Text>().text = totalTime.ToString();
        }
        //
        //停止计时
        protected void endCounting()
        {
            isCounting = false;
            timeText.GetComponent<Text>().text = "";
        }
        //
        //能不能管上
        protected bool checkCanRule()
        {
            //第一次出牌时
            if(gameController.lastGroup.type == PKType.PK_NAT) 
                return true;
           
            bool can = false;
            switch (gameController.lastGroup.type) {
                case PKType.PK_DAN:              can =  findRuleDAN();break;
                case PKType.PK_DUI:                can = findRuleDUI(); break;
                case PKType.PK_SAN:               can = findRuleSAN(); break;
                case PKType.PK_3_DAN:           can = findRule3_DAN(); break;
                case PKType.PK_3_DUI:             can = findRule3_DUI(); break;
                case PKType.PK_4_DAN_DAN:  can = findRule4_DAN_DAN(); break;
                case PKType.PK_4_DUI:             can = findRule4_DUI(); break;
                case PKType.PK_SHUN:             can = findRuleSHUN(); break;
                case PKType.PK_SHUN2:           can = findRuleSHUN2(); break;
                case PKType.PK_SHUN3:           can = findRuleSHUN3(); break;
                default: break;
                    //在没想到更好的算法之前，先暂时不做进一步判断
            }
            if (can) return true;

            //炸弹和王炸最后考虑
            if (findRuleBOMB() || findRuleWANG_ZHA()) return true;
            if (gameController.whoseturn == gameController.whoisrule) return true;
            return false;
        }
        //
        //清除打出的手牌图片（由下一个出牌玩家调用）
        private void clearOut()
        {
            for (int i = 0; i < outPlaces.Length; i++)
                outPlaces[i].GetComponent<SpriteRenderer>().sprite = null;
        }

        protected Dictionary<int, List<Poker>> pokers;               //手牌（int 表示List<Poker>的牌值，14表示王）                                              
        protected List<Pos> hints;                                                 //出牌策略（记录可以出牌的位置）
        protected bool isCounting;                                                 //是否倒计时
        protected float deltaTime;                                                  //每帧流逝的时间
        protected int totalTime;                                                      //计时开始后流逝的时间     
        public int cardNum;                                                            //手牌总数
        public bool isLandLord;                                                      //是否为地主
        protected bool canRule;                                                     //有没有可能管上
        public GameObject[] outPlaces;                                         //20个出牌位置

        public GameController gameController;
        protected PokerManager pokerManager;

        public GameObject timeText;                                              //倒计时标志

        /* 
            从手牌中搜寻某一种大于上一次出牌的牌型（兼容自由出牌）
            如果有则用hints记录，返回true没有则返回false
            采用最简单的贪心算法                                    
        */
        protected bool findRuleDAN()
        {
            int value = gameController.lastGroup.value;
            if (gameController.whoisrule == gameController.whoseturn) value = -2;
            //先找3~K
            for (int i = 3; i <= 13; i++) {
                if (pokers[i].Count >= 1 && pokers[i][0].Value > value) {
                    hints.Clear();                          //更新策略
                    hints.Add(new Pos(i, 0));
                    return true;
                }
            }
            //再找1~2
            for (int i = 1; i <= 2; i++) {
                if (pokers[i].Count >= 1 && pokers[i][0].Value > value) {
                    hints.Clear();                          //更新策略
                    hints.Add(new Pos(i, 0));
                    return true;
                }
            }
            //最后考虑王
            if (pokers[14].Count >= 1) {
                hints.Clear();                          //更新策略
                hints.Add(new Pos(14, 0));
                return true;
            }
            return false;
        }//找出 单张
        protected bool findRuleDUI()
        {
            int value = gameController.lastGroup.value;
            if (gameController.whoisrule == gameController.whoseturn) value =  -2;
            //先找3~K
            for (int i = 3; i <= 13; i++) {
                if (pokers[i].Count >= 2 && pokers[i][0].Value > value) {
                    hints.Clear();
                    hints.Add(new Pos(i, 0)); hints.Add(new Pos(i, 1));
                    return true;
                }
            }
            //再找1~2
            for (int i = 1; i <= 2; i++) {
                if (pokers[i].Count >= 2 && pokers[i][0].Value > value) {
                    hints.Clear();
                    hints.Add(new Pos(i, 0));
                    hints.Add(new Pos(i, 1));
                    return true;
                }
            }
            return false;
        }//找出 对子
        protected bool findRuleWANG_ZHA()
        {
            if (pokers[14].Count == 2) {
                hints.Clear();
                hints.Add(new Pos(14, 0)); hints.Add(new Pos(14, 1));
                return true;
            }
            return false;
        }//找出 王炸
        protected bool findRuleSAN()
        {
            int value = gameController.lastGroup.value;
            if (gameController.whoisrule == gameController.whoseturn) value = -2;
            //先找3~K
            for (int i = 3; i <= 13; i++) {
                if (pokers[i].Count >= 3 && pokers[i][0].Value > value) {
                    hints.Clear();
                    hints.Add(new Pos(i, 0)); hints.Add(new Pos(i, 1)); hints.Add(new Pos(i, 2));
                    return true;
                }
            }
            //再找1~2
            for (int i = 1; i <= 2; i++) {
                if (pokers[i].Count >= 3 && pokers[i][0].Value > value) {
                    hints.Clear();
                    hints.Add(new Pos(i, 0)); hints.Add(new Pos(i, 1)); hints.Add(new Pos(i, 2));
                    return true;
                }
            }
            return false;
        }//找出 三张
        protected bool findRuleBOMB()
        {
            if (gameController.lastGroup.type == PKType.PK_WANG_ZHA)
                return false;

            int value = gameController.lastGroup.value;
            if (gameController.whoisrule == gameController.whoseturn || gameController.lastGroup.type != PKType.PK_BOMB) value = -2;

            for (int i = 1; i <= 14; i++) {
                if (pokers[i].Count == 4 && pokers[i][0].Value > value) {
                    hints.Clear();
                    hints.Add(new Pos(i, 0)); hints.Add(new Pos(i, 1));
                    hints.Add(new Pos(i, 2)); hints.Add(new Pos(i, 3));
                    return true;
                }
            }
            return false;
        }//找出 炸弹
        protected bool findRule3_DAN()
        {
            int value = gameController.lastGroup.value;
            if (gameController.whoisrule == gameController.whoseturn) value = -2;
            int pos3 = -1; int pos1 = -1;//记录三张和一张的位置

            for (int i = 1; i <= 13; i++) {

                if (pokers[i].Count >= 3) {
                    //优先选取最小的作为三张
                    if ((pos3 == -1 && pokers[i][0].Value > value) || (pos3 != -1 && pokers[pos3][0].Value > pokers[i][0].Value && pokers[i][0].Value > value)) {
                        pos3 = i;
                        continue;
                    }
                }

                if (pokers[i].Count >= 1) {
                    //优先选取最小的作为单张
                    if ((pos1 == -1) || (pos1 != -1 && pokers[pos1][0].Value > pokers[i][0].Value))
                        pos1 = i;
                }
            }

            if (pos3 != -1 && pos1 != -1) {
                hints.Clear();
                hints.Add(new Pos(pos3, 0)); hints.Add(new Pos(pos3, 1));
                hints.Add(new Pos(pos3, 2)); hints.Add(new Pos(pos1, 0));
                return true;
            }
            else {
                return false;
            }
        }//找出 3带1
        protected bool findRule3_DUI()
        {
            int value = gameController.lastGroup.value;
            if (gameController.whoisrule == gameController.whoseturn) value = -2;
            int pos3 = -1; int pos2 = -1;//记录三张和二张的位置

            for (int i = 1; i <= 14; i++) {

                if (pokers[i].Count >= 3) {
                    //优先选取最小的作为三张
                    if ((pos3 == -1 && pokers[i][0].Value > value) || (pos3 != -1 && pokers[pos3][0].Value > pokers[i][0].Value && pokers[i][0].Value > value)) {
                        pos3 = i;
                        continue;
                    }
                }

                if (pokers[i].Count >= 2) {
                    //优先选取最小的作为二张
                    if ((pos2 == -1) || (pos2 != -1 && pokers[pos2][0].Value > pokers[i][0].Value))
                        pos2 = i;
                }
            }

            if (pos3 != -1 && pos2 != -1) {
                hints.Clear();
                hints.Add(new Pos(pos3, 0)); hints.Add(new Pos(pos3, 1));
                hints.Add(new Pos(pos3, 2)); hints.Add(new Pos(pos2, 0));
                hints.Add(new Pos(pos2, 1));
                return true;
            }
            else {
                return false;
            }
        }//找出 3带对
        protected bool findRule4_DAN_DAN()
        {
            int value = gameController.lastGroup.value;
            if (gameController.whoisrule == gameController.whoseturn) value = -2;
            int pos4 = -1; int pos11 = -1; int pos12 = -1;//记录三张和一张的位置

            for (int i = 1; i <= 14; i++) {

                if (pokers[i].Count == 4) {
                    //优先选取最小的作为四张
                    if ((pos4 == -1 && pokers[i][0].Value > value) || (pos4 != -1 && pokers[pos4][0].Value > pokers[i][0].Value && pokers[i][0].Value > value)) {
                        pos4 = i;
                        continue;
                    }
                }

                if (pokers[i].Count >= 1) {
                    if (pos11 == -1) {
                        pos11 = i;
                        continue;
                    }

                    if (pos12 == -1) {
                        pos12 = i;
                        continue;
                    }
                }
            }

            if (pos4 != -1 && pos11 != -1 && pos12 != -1) {
                hints.Clear();
                hints.Add(new Pos(pos4, 0)); hints.Add(new Pos(pos4, 1));
                hints.Add(new Pos(pos4, 2)); hints.Add(new Pos(pos4, 3));
                hints.Add(new Pos(pos11, 0)); hints.Add(new Pos(pos12, 0));
                return true;
            }
            else {
                return false;
            }
        }//找到 4带2单
        protected bool findRule4_DUI()
        {
            int value = gameController.lastGroup.value;
            if (gameController.whoisrule == gameController.whoseturn) value = -2;
            int pos4 = -1; int pos2 = -1;//记录三张和二张的位置

            for (int i = 1; i <= 14; i++) {

                if (pokers[i].Count == 4) {
                    //优先选取最小的作为三张
                    if ((pos4 == -1 && pokers[i][0].Value > value) || (pos4 != -1 && pokers[pos4][0].Value > pokers[i][0].Value && pokers[i][0].Value > value)) {
                        pos4 = i;
                        continue;
                    }
                }

                if (pokers[i].Count >= 2) {
                    //优先选取最小的作为二张
                    if ((pos2 == -1) || (pos2 != -1 && pokers[pos2][0].Value > pokers[i][0].Value))
                        pos2 = i;
                }
            }

            if (pos4 != -1 && pos2 != -1) {
                hints.Clear();
                hints.Add(new Pos(pos4, 0)); hints.Add(new Pos(pos4, 1));
                hints.Add(new Pos(pos4, 2)); hints.Add(new Pos(pos4, 3));
                hints.Add(new Pos(pos2, 0)); hints.Add(new Pos(pos2, 1));
                return true;
            }
            else {
                return false;
            }
        }//找到 4带对
        protected bool findRuleSHUN()
        {
            if (gameController.whoisrule == gameController.whoseturn) { //自由出牌
                for(int i = 3; i <=  9; i++) {
                    int j = i; for (; j <= 13; j++)  if (pokers[j].Count == 0) break; 
                    if( j - i >= 5) {
                        hints.Clear();
                        for (; i < j; i++) hints.Add(new Pos(i,0));
                        return true;
                    }
                }
            }
            else {//管上出牌
                int num = gameController.lastGroup.pokers.Length;//lastGroup顺子牌数
                int value = gameController.lastGroup.value ;//lastGroup顺子的最大牌值

                if (value == 11)//带A的顺子是最大的
                    return false;
             
                for (int i = value + 3 - num + 2 ; i <= 9; i++) {// (value+3)表示顺子的最大牌型，-num+2表示从顺子的第二张牌开始找
                    int j = i; for(; j < i+num; j++) if (pokers[j].Count == 0 || j == 14) break;
                    if ( j - i == num ) {
                        hints.Clear();
                        for (; i < j; i++) hints.Add(new Pos(i, 0));
                        return true;
                    }
                }
            }
            return false;
        }//找到单顺
        protected bool findRuleSHUN2()
        {
            if (gameController.whoisrule == gameController.whoseturn) { //自由出牌
                for (int i = 3; i <= 11; i++) {
                    int j = i; for (; j <= 13; j++) if (pokers[j].Count < 2) break;
                    if (j - i >= 3) {
                        hints.Clear();
                        for (; i < j; i++) { hints.Add(new Pos(i, 0)); hints.Add(new Pos(i, 1)); }
                        return true;
                    }
                }
            }
            else {//管上出牌
                int num = gameController.lastGroup.pokers.Length/2;//lastGroup双顺对数
                int value = gameController.lastGroup.value;//lastGroup顺子的最大牌值

                if (value == 11)//带A的顺子是最大的
                    return false;

                for (int i = value + 3 - num + 2; i <= 11; i++) {
                    int j = i; for (; j < i + num; j++) if ( pokers[j].Count < 2 || j == 14) break;
                    if (j - i  == num) {
                        hints.Clear();
                        for (; i < j; i++) { hints.Add(new Pos(i, 0)); hints.Add(new Pos(i, 1));}
                        return true;
                    }
                }
            }
            return false;
        }//找到双顺
        protected bool findRuleSHUN3()
        {
            if (gameController.whoisrule == gameController.whoseturn) { //自由出牌
                for (int i = 3; i <= 11; i++) {
                    int j = i; for (; j <= 13; j++) if (pokers[j].Count < 3) break;
                    if (j - i >= 3) {
                        hints.Clear();
                        for (; i < j; i++) { hints.Add(new Pos(i, 0)); hints.Add(new Pos(i, 1)); hints.Add(new Pos(i, 2));}
                        return true;
                    }
                }
            }
            else {//管上出牌
                int num = gameController.lastGroup.pokers.Length / 3;//lastGroup三顺对数
                int value = gameController.lastGroup.value;//lastGroup顺子的最大牌值

                if (value == 11)//带A的顺子是最大的
                    return false;

                for (int i = value + 3 - num + 2; i <= 11; i++) {
                    int j = i; for (; j < i + num; j++) if (pokers[j].Count < 3 ||  j == 13) break;
                    if (j - i  == num) {
                        hints.Clear();
                        for (; i < j; i++) { hints.Add(new Pos(i, 0)); hints.Add(new Pos(i, 1)); hints.Add(new Pos(i, 2)); }
                        return true;
                    }
                }
            }
            return false;
        }//找到三顺
    }
}
