using System.Collections;
using System.Collections.Generic;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

using RoleNameSpace;
using PokerNameSpace;

public class GameController : MonoBehaviour {

    private void Start ()
    {
        Time.timeScale = 1.0f;
        init();
	}

    private void init()
    {
        whoisrule = 0;
        whoseturn = 0;
        lastGroup = new PokerGroup(PKType.PK_NAT, PokerGroup.NAT_VALUE, null);
        restartBt.gameObject.SetActive(false);
        quitBt.gameObject.SetActive(false);
        pedal.SetActive(false);
        resultImage.gameObject.SetActive(false);
    }
    //
    //游戏开始
    private void run(object[] objs)
    {
        //地主拿到额外三张牌
        lord = (int)objs[0];
        whoisrule = whoseturn = lord;
        boor1 = (lord + 1) % 3;
        boor2 = (lord + 2) % 3;
        roles[lord].isLandLord = true; 
        roles[lord].getExtra3((int)objs[1],(int)objs[2],(int)objs[3]);
        pedal.SetActive(false);
        //地主出牌
        roles[whoseturn].SendMessage("discard");
    }
    //
    //先选地主，后发牌，发牌过程中整理牌
    private void dealCards()
    {
        int[] cards = new int[54];
        for (int i = 0; i < 54; i++)       //初始化
            cards[i] = i;

        int x, y;
        System.Random r = new System.Random();
        for (int i = 0; i < 54; i++) {       //洗牌
            do {
                x = r.Next(0, 54);
                y = r.Next(0, 54);
            } while (x == y);
            int temp;
            temp = cards[x];
            cards[x] = cards[y];
            cards[y] = temp;
        }

        int[] playerCards = new int[17];   //分牌
        int[] ai1Cards = new int[17];
        int[] ai2Cards = new int[17];
        for (int i = 0; i < 51; i++) {
            if (i < 17)
                ai1Cards[i] = cards[i];
            else if (i >= 17 && i < 34)
                ai2Cards[i - 17] = cards[i];
            else
                playerCards[i - 34] = cards[i];
        }
        roles[0].getCards(playerCards);
        roles[1].getCards(ai1Cards);
        roles[2].getCards(ai2Cards);

        //用最后三张牌选地主
        pedal.SendMessage("ShowPedal", new object[] { cards[51],cards[52], cards[53] });
    }
    //
    //管上回调函数
    private void ruleCall()
    {
        whoisrule = whoseturn;

        if (roles[whoisrule].cardNum == 0) {
            endGame();
            return;
        }

        //下一个玩家开始决策
        whoseturn = (whoseturn + 1) % 3;
        roles[whoseturn].SendMessage("discard");
    }
    //
    //不叫回调函数
    private void dontRuleCall()
    {
        whoseturn = (whoseturn + 1) % 3;
        //下一个玩家开始决策
        roles[whoseturn].SendMessage("discard");
    }
    //
    //结束游戏
    private void endGame()
    {
        Time.timeScale = 0;
        quitBt.gameObject.SetActive(true);
        restartBt.gameObject.SetActive(true);
        //停止背景音
        GetComponent<AudioSource>().Stop();

        if (roles[0].isLandLord) {
            if(roles[1].cardNum == 0 || roles[2].cardNum == 0) {//玩家失败
                resultImage.sprite= Resources.Load("emoji/失败嘲讽", typeof(Sprite)) as Sprite;
                AudioSource.PlayClipAtPoint(loseMusic, new Vector3(0, 0, 0));
            }
            else {//玩家胜利
                resultImage.sprite = Resources.Load("emoji/胜利", typeof(Sprite)) as Sprite;
                AudioSource.PlayClipAtPoint(winMusic, new Vector3(0, 0, 0));
            }
        }
        else {
            if(roles[lord].cardNum == 0) {//玩家失败
                resultImage.sprite = Resources.Load("emoji/失败嘲讽", typeof(Sprite)) as Sprite;
                AudioSource.PlayClipAtPoint(loseMusic, new Vector3(0, 0, 0));
            }
            else {//玩家胜利
                resultImage.sprite = Resources.Load("emoji/胜利", typeof(Sprite)) as Sprite;
                AudioSource.PlayClipAtPoint(winMusic, new Vector3(0, 0, 0));
            }
        }

        resultImage.gameObject.SetActive(true);

    }
    //
    //开始、重新开始、退出按钮回调
    public void startBtCall()
    {
        AudioSource.PlayClipAtPoint(click, new Vector3(0, 0, 0));
        startBt.gameObject.SetActive(false);
        //开始选地主
        pedal.SetActive(true);
        dealCards();
    }
    public void restartBtCall()
    {
        AudioSource.PlayClipAtPoint(click, new Vector3(0, 0, 0));
        SceneManager.LoadScene("Scenes/gameScene");
    }
    public void quitBtCall()
    {
        AudioSource.PlayClipAtPoint(click, new Vector3(0, 0, 0));
        Application.Quit();
    }

    private void updateLastGroup(PokerGroup newGroup)
    {
        lastGroup = newGroup;
        roles[whoisrule].SendMessage("clearOut");
    }

    public Role[] roles;
    public int lord,boor1, boor2;                      //地主、农民1、农民2
    public int whoseturn;                                 //轮到谁
    public int whoisrule;                                   //谁的牌没人管
    public PokerGroup lastGroup;                   //上一轮打出的牌

    public GameObject pedal;                         //选地主界面
    public Button startBt;                                 //开始按钮
    public Button restartBt;                             //重新开始按钮
    public Button quitBt;                                 //离开游戏按钮          
    public Image resultImage;                         //结果图像

    public AudioClip winMusic, loseMusic;      //胜利音效，失败音效   
    public AudioClip click;              
}
