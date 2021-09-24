using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

using PokerNameSpace;
using RoleNameSpace;

public class PedalController : MonoBehaviour {

    private void Start()
    {
        lord = -1;
        ai1Time = 3; ai2Time = 6; lastTime = 2;
        startAI = AI_1Done = AI_2Done = isAllDecided =  false;
    }
    //
    //显示选地主
    private void ShowPedal(object[] objs)
    {
        choices = new int[3];
        for (int i = 0; i < 3; i++) choices[i] = -1;

        //p[0]表示不叫，p[1]、p[2]、p[3]表示三张牌的order
        p = new int[4];
        p[0] = -1;  p[1] = (int)objs[0]; p[2]= (int)objs[1]; p[3] = (int)objs[2];
        //三张牌背
        Sprite sp1 = Instantiate(Resources.Load("pokers2/-1", typeof(Sprite)) as Sprite, out1.GetComponent<Transform>().position, out1.GetComponent<Transform>().rotation, out1.GetComponent<Transform>());
        out1.GetComponent<Image>().sprite = sp1;
        Sprite sp2 = Instantiate(Resources.Load("pokers2/-1", typeof(Sprite)) as Sprite, out2.GetComponent<Transform>().position, out2.GetComponent<Transform>().rotation, out2.GetComponent<Transform>());
        out2.GetComponent<Image>().sprite = sp2;
        Sprite sp3 = Instantiate(Resources.Load("pokers2/-1", typeof(Sprite)) as Sprite, out3.GetComponent<Transform>().position, out3.GetComponent<Transform>().rotation, out3.GetComponent<Transform>());
        out3.GetComponent<Image>().sprite = sp3;
    }
    //
    //四个按钮回调
    public void BuJiaoCall()
    {
        AudioSource.PlayClipAtPoint(click, new Vector3(0, 0, 0));
        choices[0] = 0;  bt_buJiao.gameObject.SetActive(false); startAI = true; 
    }
    public void YiFenCall()
    {
        AudioSource.PlayClipAtPoint(click, new Vector3(0, 0, 0));
        choices[0] = 1; bt_1Fen.gameObject.SetActive(false); startAI = true;
    }
    public void ErFenCall()
    {
        AudioSource.PlayClipAtPoint(click, new Vector3(0, 0, 0));
        choices[0] = 2; bt_2Fen.gameObject.SetActive(false); startAI = true;
    }
    public void SanFenCall()
    {
        AudioSource.PlayClipAtPoint(click, new Vector3(0, 0, 0));
        choices[0] = 3; bt_3Fen.gameObject.SetActive(false); startAI = true;
    }
    //
    //决定谁是地主
    private void Decide()
    {
        //隐藏按钮
        bt_1Fen.gameObject.SetActive(false); bt_2Fen.gameObject.SetActive(false); bt_3Fen.gameObject.SetActive(false);
        bt_buJiao.gameObject.SetActive(false);

        //显示三张牌
        out1.GetComponent<Image>().sprite = new Poker(p[1]).sp;
        out2.GetComponent<Image>().sprite = new Poker(p[2]).sp;
        out3.GetComponent<Image>().sprite = new Poker(p[3]).sp;

        int playerValue = new Poker(choices[0]).Value;
        int ai1Value = new Poker(choices[1]).Value;
        int ai2Value = new Poker(choices[2]).Value;

        if (choices[0] == 0 && choices[1] == 0 && choices[2] == 0)  //都不叫则玩家为地主
            lord = 0;
        else if ((playerValue > ai1Value || (playerValue == ai1Value && p[choices[0]] > p[choices[1]]))  && (playerValue > ai2Value || (playerValue == ai2Value && p[choices[0]] > p[choices[2]])))
            lord = 0;
        else if ((ai1Value > playerValue || (ai1Value == playerValue && p[choices[1]] > p[choices[0]])) && (ai1Value > ai2Value || ( ai1Value == ai2Value && p[choices[1]] > p[choices[2]])))
            lord = 1;
        else
            lord = 2;

        switch (lord)
        {
            case 0: resultText.GetComponent<Text>().text = "你是地主！"; break;
            case 1: resultText.GetComponent<Text>().text = "熊猫一是地主！"; break;
            case 2: resultText.GetComponent<Text>().text = "熊猫二是地主！"; break;
        }
        isAllDecided = true; 
    }

    private void Update()
    {
        if(startAI) {
            deltaTime += Time.deltaTime;
            if(deltaTime >= 1.0f) {
                ai1Time--;
                ai2Time--;
                deltaTime = 0;

                //AI_1决策
                if(ai1Time == 0) {
                    AudioSource.PlayClipAtPoint(click, new Vector3(0, 0, 0));
                    System.Random r = new System.Random();
                    int temp = r.Next(0, 4);
                    while (temp == choices[0] && temp != 0) temp = r.Next(0, 4);
                    choices[1] = temp; AI_1Done = true;
                    //显示结果
                    switch (temp) {
                        case 0: ai1Text.GetComponent<Text>().text = "不叫"; bt_buJiao.gameObject.SetActive(false); break;
                        case 1: ai1Text.GetComponent<Text>().text = "一分"; bt_1Fen.gameObject.SetActive(false); break;
                        case 2: ai1Text.GetComponent<Text>().text = "二分"; bt_2Fen.gameObject.SetActive(false); break;
                        case 3: ai1Text.GetComponent<Text>().text = "三分"; bt_3Fen.gameObject.SetActive(false); break;
                    }
                }

                //AI_2决策
                if (ai2Time == 0) {
                    AudioSource.PlayClipAtPoint(click, new Vector3(0, 0, 0));
                    System.Random r = new System.Random();
                    int temp = r.Next(0, 4);
                    while ((temp == choices[0] || temp == choices[1]) && temp != 0) temp = r.Next(0, 4);
                    choices[2] = temp; AI_2Done = true;
                    //显示结果
                    switch(temp) {
                        case 0: ai2Text.GetComponent<Text>().text = "不叫"; bt_buJiao.gameObject.SetActive(false); break;
                        case 1: ai2Text.GetComponent<Text>().text = "一分"; bt_1Fen.gameObject.SetActive(false); break;
                        case 2: ai2Text.GetComponent<Text>().text = "二分"; bt_2Fen.gameObject.SetActive(false); break;
                        case 3: ai2Text.GetComponent<Text>().text = "三分"; bt_3Fen.gameObject.SetActive(false); break;      
                    }      
                }
            }
        }
        //AI都已经决策完
        if(AI_1Done && AI_2Done) {
            startAI = AI_1Done = AI_2Done = false;
            Decide();
        }
        
        if(isAllDecided) {
            deltaTime += Time.deltaTime;
            if(deltaTime >= 1.0f) {
                lastTime--;
                deltaTime = 0;

                if(lastTime == 0)  gameController.SendMessage("run", new object[] { lord, p[1], p[2], p[3] });
            }
        }       
    }

    public GameObject out1, out2, out3;                            //牌位置
    int[] p;                                                                            //对应参与者四种选择的结果
    public int[] choices;                                                       //三位参与者决策结果:123表示叫一、二、三分，0表示不叫
    int lord;                                                                          //标志地主
    int ai1Time, ai2Time;                                                      //AI决策的时间
    int lastTime;                                                                   //最后结果显示时间
    float deltaTime;                                                              //流逝的时间
    bool startAI, AI_1Done, AI_2Done;                                 //AI开始决策标志位
    bool isAllDecided;                                                           //Decide完成标志位

    public GameController gameController;
    public Button bt_buJiao, bt_1Fen, bt_2Fen, bt_3Fen;
    public Text resultText;
    public GameObject ai1Text, ai2Text;

    public AudioClip click;                                                     //点击音效                                                                        
}
