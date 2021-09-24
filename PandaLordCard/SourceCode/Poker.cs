using System.Collections.Generic;
using UnityEngine;
using System.Linq;
using System;

namespace PokerNameSpace
{
    /*
        sturct Poker                    扑克牌类
        sturct PokerGroup          组扑克牌类，存放这组扑克牌的类型、牌数等信息
        enum PKType                 一组扑克牌的类型
        class PokerManager       扑克牌管理类，主要用于将一组牌包装为PokerGroup
        struct Pos                        二维坐标类
    */

    public enum PKType
    {
        PK_DAN,                                                                         //1张

        PK_DUI, PK_WANG_ZHA,                                                //2张

        PK_SAN,                                                                           //3张

        PK_BOMB, PK_3_DAN,                                                      //4张

        PK_3_DUI,                                                                         //5张

        PK_4_DAN_DAN, PK_4_DUI,                                              //6张

        PK_33_DAN_DAN, PK_4_DUI_DUI,                                    //8张

        PK_33_DUI_DUI,                                                                //10张

        PK_333_DAN_DAN_DAN,                                                 //12张

        PK_333_DUI_DUI_DUI,                                                      //15张

        PK_3333_DAN_DAN_DAN_DAN,                                      //16张

        PK_33333_DAN_DAN_DAN_DAN_DAN, PK_3333_DUI_DUI_DUI_DUI,  //20张

        PK_SHUN, //单顺（5~12张）

        PK_SHUN2,//双顺（6~20张）

        PK_SHUN3,//三顺（9~18张）

        PK_NAT//非类（Not A Type）
    };

    public struct Poker : IComparable,IEquatable<Poker>
    {
        public static readonly int WANG_VALUE_XIAO = 100;
        public static readonly int WANG_VALUE_DA = 200;
        public static readonly int WRONG_VALUE = -1;

        public Poker(int order)
        {
            sp = Resources.Load("pokers2/" + order, typeof(Sprite)) as Sprite;
            Id = order / 4 + 1; Suit = order % 4;
            switch (Id) {
                case 3: Value = 0; break;
                case 4: Value = 1; break;
                case 5: Value = 2; break;
                case 6: Value = 3; break;
                case 7: Value = 4; break;
                case 8: Value = 5; break;
                case 9: Value = 6; break;
                case 10: Value = 7; break;  // J
                case 11: Value = 8; break;  // Q
                case 12: Value = 9; break;  // K
                case 13: Value = 10; break;
                case 1: Value = 11; break;
                case 2: Value = 12; break;
                case 14:
                    if(Suit == 0)
                        Value = WANG_VALUE_XIAO;
                    else
                        Value = WANG_VALUE_DA;
                    break;
                default: Value = WRONG_VALUE; break;
            }
        }
        //
        //获得牌的编号（1~54）
        public int getOrder()
        {
            return (Id-1) * 4 + Suit;
        }
        //
        //根据权值的升序排列
        public int CompareTo(object obj)
        {
            int v = ((Poker)obj).Value;
            if (this.Value > v)
                return 1;      //小的排在前，大的排在后
            if (this.Value < v)
                return -1;
            else
                return 0;
        }

        public bool Equals(Poker other)
        {
            return other.getOrder() == getOrder();
        }

        public int Id;          //牌号（1~14）
        public int Suit;       //花色（0~3）
        public int Value;    //权值（3最小，王最大）
        public Sprite sp;    
    }

    public struct Pos : IEquatable<Pos>
    {
        public Pos(int x, int y)
        {
            this.x = x;
            this.y = y;
        }

        public bool Equals(Pos other)
        {
            return (other.x == x && other.y == y);
        }

        public int x;
        public int y;
    }

    public struct PokerGroup : IComparable
    {
        public static readonly int NAT_VALUE = -1;                      //非类的权值
        public static readonly int NOT_NUM = -1;                        //无意义的数量

        public PokerGroup(PKType t, int v, Poker[] pks)
        {
            type = t; value = v; pokers = pks;
        }

        public int CompareTo(object obj)
        {
            int v = ((PokerGroup)obj).value;
            if (this.value > v)
                return 1;      //小的排在前，大的排在后
            if (this.value < v)
                return -1;
            else
                return 0;
        }//根据权值升序排序

        public Poker[] pokers;
        public PKType type;
        public int value;
    }

    public class PokerManager
    {
        public PokerManager()
        {
            getPKTypeArray = new getPKType[20];
            //getPKTypeArray[0]代表牌数为一张的getPKType，以此类推
            getPKTypeArray[0] = new getPKType(getDAN);
            getPKTypeArray[1]  = new getPKType(getDUI);
            getPKTypeArray[2] = new getPKType(getSAN);
            getPKTypeArray[3]  = new getPKType(getBOMB);
            getPKTypeArray[3] += getPK_3_DAI_DAN;
            getPKTypeArray[4]  = new getPKType(getSHUN);
            getPKTypeArray[4] += getPK_3_DAI_DUI;
            getPKTypeArray[5]  = new getPKType(getSHUN);
            getPKTypeArray[5] += getSHUN2;
            getPKTypeArray[5] += getPK_4_2;
            getPKTypeArray[6]  = new getPKType(getSHUN);
            getPKTypeArray[7]  = new getPKType(getSHUN);
            getPKTypeArray[7] += getSHUN2;
            getPKTypeArray[7] += getPK_3_DAI_DAN;
            getPKTypeArray[7] += getPK_4_DUI_DUI;
            getPKTypeArray[8]  = new getPKType(getSHUN);
            getPKTypeArray[8] += getSHUN3;
            getPKTypeArray[9]  = new getPKType(getSHUN);
            getPKTypeArray[9] += getSHUN2;
            getPKTypeArray[9] += getPK_3_DAI_DUI;
            getPKTypeArray[10]  = new getPKType(getSHUN);
            getPKTypeArray[11]  = new getPKType(getSHUN);
            getPKTypeArray[11] += getSHUN2;
            getPKTypeArray[11] += getSHUN3;
            getPKTypeArray[11] += getPK_3_DAI_DAN;
            getPKTypeArray[12]  = new getPKType(getPK_NAT);
            getPKTypeArray[13]  = new getPKType(getSHUN2);
            getPKTypeArray[14] = new getPKType(getSHUN3);
            getPKTypeArray[14] += getPK_3_DAI_DUI;
            getPKTypeArray[15] = new getPKType(getSHUN2);
            getPKTypeArray[15] += getPK_3_DAI_DAN;
            getPKTypeArray[16]  = new getPKType(getPK_NAT);
            getPKTypeArray[17]  = new getPKType(getSHUN2);
            getPKTypeArray[17] += getSHUN3;
            getPKTypeArray[18]  = new getPKType(getPK_NAT);
            getPKTypeArray[19] = new getPKType(getSHUN2);
            getPKTypeArray[19] += getPK_3_DAI_DAN;
            getPKTypeArray[19] += getPK_3_DAI_DUI;
        }

        delegate PokerGroup getPKType(Poker[] p);
        getPKType[] getPKTypeArray;

        PokerGroup getDAN(Poker[] p)
        {
            if (p.Length != 1)
                return new PokerGroup(PKType.PK_NAT, PokerGroup.NAT_VALUE,null);
            else
                return new PokerGroup(PKType.PK_DAN, p[0].Value,p);
        }//获取 单张
        PokerGroup getDUI(Poker[] p)
        {
            if (p.Length != 2)
                return new PokerGroup(PKType.PK_NAT,  PokerGroup.NAT_VALUE,null);

            else if (p[0].Id == p[1].Id) {
                if (p[0].Id == 14)
                    return new PokerGroup(PKType.PK_WANG_ZHA, p[0].Value,p);
                else
                    return new PokerGroup(PKType.PK_DUI,  p[0].Value,p);
            }
            else {
                return new PokerGroup(PKType.PK_NAT, PokerGroup.NAT_VALUE,null);
            }
        }//获取 对子、王炸
        PokerGroup getSAN(Poker[] p)
        {
            if (p.Length != 3)
                return new PokerGroup(PKType.PK_NAT, PokerGroup.NAT_VALUE,null);
            if (p[0].Id == p[1].Id && p[0].Id == p[2].Id)
                return new PokerGroup(PKType.PK_SAN, p[0].Value,p);
            else
                return new PokerGroup(PKType.PK_NAT, PokerGroup.NAT_VALUE,null);
        }//获取 三张
        PokerGroup getBOMB(Poker[] p)
        {
            if (p.Length != 4)
                return new PokerGroup(PKType.PK_NAT,  PokerGroup.NAT_VALUE,null);
            else if (p[0].Id == p[1].Id && p[0].Id == p[2].Id && p[0].Id == p[3].Id)
                return new PokerGroup(PKType.PK_BOMB, p[0].Value,p);
            else
                return new PokerGroup(PKType.PK_NAT, PokerGroup.NAT_VALUE,null);
        }//获取 炸弹

        PokerGroup getSHUN(Poker[] p)
        {
            Array.Sort(p);
            int length = p.Length;
            //顺子至少从3开始
            if (p[0].Id < 3)
                return new PokerGroup(PKType.PK_NAT, PokerGroup.NAT_VALUE,null);

            //顺子必须连续且不能有
            for (int i = 0; i < length - 1; i++)
                if ((p[i].Id + 1 != p[i + 1].Id) || p[i+1].Value > 11)
                    return new PokerGroup(PKType.PK_NAT, PokerGroup.NAT_VALUE,null);

            return new PokerGroup(PKType.PK_SHUN, p[length - 1].Value,p);

        }//获取 单顺（5张~12张）
        PokerGroup getSHUN2(Poker[] p)
        {
            Array.Sort(p);
            int length = p.Length;
            //顺子至少从3开始
            if (p[0].Id < 3)
                return new PokerGroup(PKType.PK_NAT, PokerGroup.NAT_VALUE,null);

            for (int i = 0; i < length - 2; i += 2)
                if ((p[i].Id + 1 != p[i + 2].Id) || p[i + 2].Value > 11)
                    return new PokerGroup(PKType.PK_NAT, PokerGroup.NAT_VALUE,null);

            return new PokerGroup(PKType.PK_SHUN2, p[length - 1].Value,p);

        }//获取 双顺（6~20张）
        PokerGroup getSHUN3(Poker[] p)
        {
            Array.Sort(p);
            int length = p.Length;
            //顺子至少从3开始
            if (p[0].Id < 3)
                return new PokerGroup(PKType.PK_NAT, PokerGroup.NAT_VALUE,null);

            for (int i = 0; i < length - 3; i += 3)
                if ((p[i].Id + 1 != p[i + 3].Id) || p[i + 3].Value > 11)
                    return new PokerGroup(PKType.PK_NAT, PokerGroup.NAT_VALUE,null);

            return new PokerGroup(PKType.PK_SHUN3, p[length - 1].Value,p);

        }//获取 三顺（9~18张）

        PokerGroup getPK_3_DAI_DAN(Poker[] p)
        {
            int length = p.Length;
            //dictionary保存各面值与其数量的键值对，markValue标记了数量为3的牌的最大权值
            //num1和num3分别表示数量为1和数量为3的牌的次数
            Dictionary<int, int> d = new Dictionary<int, int>(); int markValue = -1;
            int num3 = 0, num1 = 0;

            foreach (Poker poker in p) {
                if (d.ContainsKey(poker.Id)) {
                    if(++d[poker.Id] == 3) {
                        num3++;
                        num1--;
                        if (markValue < poker.Value)
                            markValue = poker.Value;
                    }            
                }
                else {
                    num1++;
                    d.Add(poker.Id, 1);
                }
            }

            if (num3 == 1 && num1 == 1 && length == 4) {
                return new PokerGroup(PKType.PK_3_DAN, markValue,p);
            }
            else if (num3 == 2 && num1 == 2 && length == 8) {
                return new PokerGroup(PKType.PK_33_DAN_DAN, markValue,p);
            }
            else if (num3 == 3 && num1 == 3 && length == 12) {
                return new PokerGroup(PKType.PK_333_DAN_DAN_DAN,markValue,p);
            }
            else if (num3 == 4 && num1 == 4 && length == 16) {
                return new PokerGroup(PKType.PK_3333_DAN_DAN_DAN_DAN, markValue,p);
            }
            else if (num3 == 5 && num1 == 5 && length == 20) {//能拿到这种牌你也是6啊
                return new PokerGroup(PKType.PK_33333_DAN_DAN_DAN_DAN_DAN,markValue,p);
            }
            else {
                return new PokerGroup(PKType.PK_NAT, PokerGroup.NAT_VALUE,null);
            }
        }//获取 三带单、三三带单单、三三三带单单单……
        PokerGroup getPK_3_DAI_DUI(Poker[] p)
        {
            int length = p.Length;
            //dictionary保存各面值与其数量的键值对，markValue标记了数量为3的牌的最大权值
            //num2和num3分别表示数量为2和数量为3的牌的次数
            Dictionary<int, int> d = new Dictionary<int, int>(); int markValue = -1;
            int num3 = 0; int num2 = 0;

            foreach (Poker poker in p) {
                if (d.ContainsKey(poker.Id)) {
                    d[poker.Id]++;
                    if (d[poker.Id] == 2)
                        num2++;
                    if(d[poker.Id] == 3) {
                        num3++;
                        num2--;
                        if (markValue < poker.Value)
                            markValue = poker.Value;
                    }
                }
                else {
                    d.Add(poker.Id, 1);
                }
            }

            if (num3 == 1 && num2 == 1 && length == 5) {
                return new PokerGroup(PKType.PK_3_DUI, markValue, p );
            }
            else if (num3 == 2 && num2 == 2 && length == 10) {
                return new PokerGroup(PKType.PK_33_DUI_DUI,markValue,p);
            }
            else if (num3 == 3 && num2 == 3 && length == 15) {
                return new PokerGroup(PKType.PK_333_DUI_DUI_DUI, markValue, p);
            }
            else if (num3 == 4 && num2 == 4 && length == 20) {//能拿到这种牌你也是6啊
                return new PokerGroup(PKType.PK_3333_DUI_DUI_DUI_DUI, markValue,p);
            }
            else {
                return new PokerGroup(PKType.PK_NAT, PokerGroup.NAT_VALUE,null);
            }
        }//获取 三带对、三三带对对、三三三带对对对……
        PokerGroup getPK_4_2(Poker[] p)
        {
            if (p.Length != 6)
                return new PokerGroup(PKType.PK_NAT, PokerGroup.NAT_VALUE, null);

            //dictionary保存各面值与其数量的键值对，markValue标记了数量为4的牌的权值
            Dictionary<int, int> d = new Dictionary<int, int>(); int markValue = -1;

            foreach (Poker poker in p) {
                if (d.ContainsKey(poker.Id)) {
                    d[poker.Id]++;
                    if (d[poker.Id] == 4)
                        markValue = poker.Value;
                }
                else
                    d.Add(poker.Id, 1);
            }

            if (d.Values.Contains(4) && d.Values.Contains(1))
                return new PokerGroup(PKType.PK_4_DAN_DAN, markValue, p );
            else if (d.Values.Contains(4) && d.Values.Contains(2))
                return new PokerGroup(PKType.PK_4_DUI, markValue, p);
            else
                return new PokerGroup(PKType.PK_NAT,PokerGroup.NAT_VALUE,null);
        }//获取 四带对、四带二单
        PokerGroup getPK_4_DUI_DUI(Poker[] p)
        {
            int length = p.Length;
            if (length != 8)
                return new PokerGroup(PKType.PK_NAT, PokerGroup.NAT_VALUE, null);
            //dictionary保存各面值与其数量的键值对，markValue标记了数量为3的牌的最大权值
            //num2和num3分别表示数量为2和数量为3的牌的次数
            Dictionary<int, int> d = new Dictionary<int, int>(); int markValue = -1;
            int num4 = 0; int num2 = 0;

            foreach (Poker poker in p) {
                if (d.ContainsKey(poker.Id)) {
                    ++d[poker.Id];
                    if (d[poker.Id] == 2)
                        num2++;
                    if (d[poker.Id] == 4) {
                        num4++;
                        num2--;
                        markValue = poker.Value;
                    }
                }
                else {
                    d.Add(poker.Id, 1);
                }
            }

            if (num4 == 1 && num2 == 2)
                return new PokerGroup(PKType.PK_4_DUI_DUI,markValue, p);
            else
                return new PokerGroup(PKType.PK_NAT, PokerGroup.NAT_VALUE, null);
        }//获取 四带对

        PokerGroup getPK_NAT(Poker[] p)
        {
            return new PokerGroup(PKType.PK_NAT, PokerGroup.NAT_VALUE, null);
        }//获取 非类

        //把p归纳成某种类型的牌型并将其返回
        public PokerGroup getPokerType(Poker[] pokers)
        {
            int length = pokers.Length;
            if (length <= 0 || length >= 20)
                return new PokerGroup(PKType.PK_NAT, PokerGroup.NAT_VALUE, null);

            foreach (getPKType d in getPKTypeArray[length-1].GetInvocationList()) {
                PokerGroup pg = d(pokers);
                if (pg.type != PKType.PK_NAT)
                    return pg;
            }
            return new PokerGroup(PKType.PK_NAT, PokerGroup.NAT_VALUE, null);
        }
    }
}