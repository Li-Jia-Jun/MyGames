using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using RoleNameSpace;
using PokerNameSpace;

public class HandPokerController : MonoBehaviour
{
    public static readonly int offset = 10;   //牌的上下偏移量
    //
    //记录该牌在玩家手牌<int,List<Poker>>中的位置
    public void getOrder(int order)
    {
        Order = order;
    }
    //
    //记录该牌在玩家手牌<Button>中的位置
    public void getHandPos(int hp)
    {
        HandPos = hp;
    }
    //
    //点击该牌回调事件
    public void OnClick()
    {
        Vector3 p = GetComponent<Transform>().position;
        if (IsUp) {
            player.GetComponent<PlayerRole>().SendMessage("disselectCard", new object[] { Order,HandPos} );
            //下移
            GetComponent<Transform>().position = new Vector3(p.x, p.y - offset, p.z);      
        }
        else {
            player.GetComponent<PlayerRole>().SendMessage("selectCard", new object[] { Order, HandPos });
            //上移
            GetComponent<Transform>().position = new Vector3(p.x, p.y + offset, p.z);
        }  
        IsUp = ! IsUp;
    }

    public int Order { set; get; }                    //此牌的order
    public int HandPos { set; get; }              //此牌在玩家手牌handplaces中的位置
    public bool IsUp { set; get; }                  //是否被选中（选中时牌稍微上移）
    public GameObject player;
}
