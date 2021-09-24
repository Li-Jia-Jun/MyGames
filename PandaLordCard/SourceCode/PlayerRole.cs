using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using PokerNameSpace;
using UnityEngine.UI;

namespace RoleNameSpace
{
    public class PlayerRole : Role
    {
        private void Awake()
        {
            init();
            isRush = false;
            cantRuleText.SetActive(false);
            handPlacesSelected = new List<int>();
            orderSelected = new List<int>();
            QuietBt();

            handPlacesVec3 = new Vector3[20];
            for (int i = 0; i < 20; i++) {
                handPlacesVec3[i] = new Vector3(handPlaces[i].transform.position.x, handPlaces[i].transform.position.y, handPlaces[i].transform.position.z);
                handPlaces[i].gameObject.SetActive(false);
            }

            sp_zhengChang = Resources.Load("emoji/emmm", typeof(Sprite)) as Sprite;
            sp_moJi = Resources.Load("emoji/磨叽", typeof(Sprite)) as Sprite;
        }

        protected override void discard()
        {
            isRush = false;
            base.discard();
            //启用按钮
            ActivateBt();
            //是否有可能管上（连同更新hints）
            canRule = checkCanRule();

            if (canRule) 
                Bt_hints.GetComponent<Button>().enabled = true;
            else
                cantRuleText.SetActive(true); 
            //开始计时
            startCounting();                                
        }

        public override void getCards(int[] cards)
        {
            base.getCards(cards);
            pokers2Hands();
        }
        public override void getExtra3(int p1, int p2, int p3)
        {
            base.getExtra3(p1,p2,p3);
            pokers2Hands();
        }
        //
        //将pokers映射到20个手牌位置
        private void pokers2Hands()
        {
            for(int i = 0; i < cardNum; i++) 
                handPlaces[i].gameObject.SetActive(true);

            int count = 0;
            //先将王、2、A放在最左侧
            for (int i = 0; i < pokers[14].Count; i++) {
                Button hand = handPlaces[count];
                hand.image.overrideSprite = pokers[14][i].sp;
                hand.GetComponent<HandPokerController>().SendMessage("getOrder", pokers[14][i].getOrder());
                hand.GetComponent<HandPokerController>().SendMessage("getHandPos", count);
                count++;
            }
            for (int i = 0; i < pokers[2].Count; i++) {
                Button hand = handPlaces[count];
                hand.image.overrideSprite = pokers[2][i].sp;
                hand.GetComponent<HandPokerController>().SendMessage("getOrder", pokers[2][i].getOrder());
                hand.GetComponent<HandPokerController>().SendMessage("getHandPos", count);
                count++;
            }
            for (int i = 0; i < pokers[1].Count; i++) {
                Button hand = handPlaces[count];
                hand.image.overrideSprite = pokers[1][i].sp;
                hand.GetComponent<HandPokerController>().SendMessage("getOrder", pokers[1][i].getOrder());
                hand.GetComponent<HandPokerController>().SendMessage("getHandPos", count);
                count++;
            }
            //再依次放入K、Q、J、10……4、3
            for (int i = 13; i >= 3; i--)
                for (int j = 0; j < pokers[i].Count; j++) {
                    Button hand = handPlaces[count];
                    hand.image.overrideSprite = pokers[i][j].sp;
                    hand.GetComponent<HandPokerController>().SendMessage("getOrder", pokers[i][j].getOrder());
                    hand.GetComponent<HandPokerController>().SendMessage("getHandPos", count);
                    count++;
                }
        }
        //
        //选中一张手牌（点击手牌回调事件）
        private void selectCard(object[] objs)
        {
            AudioSource.PlayClipAtPoint(click4, new Vector3(0,0,0));

            //记录选中的牌在pokers和handPlaces的位置
            //规定第一个参数为Pos，第二个参数为int
            orderSelected.Add((int)objs[0]);
            handPlacesSelected.Add((int)objs[1]);
            //有可能管上或没人大过自己时才检测牌型
            if (canRule) checkSelectedCards();    
        }
        //
        //撤销选择一张手牌（点击手牌回调事件）
        private void disselectCard(object[] objs)
        {
            AudioSource.PlayClipAtPoint(click4, transform.localPosition);

            //规定第一个参数为Pos，第二个参数为int
            orderSelected.Remove((int)objs[0]);
            handPlacesSelected.Remove((int)objs[1]);
            //有可能管上或没人大过自己时才检测牌型
            if (canRule)  checkSelectedCards();
        }
        //
        //调整手牌位置
        private void adjustHandPos()
        {
            int start = (20 - cardNum) / 2;
            for (int i = 0; i < 20; i++)
                if(handPlaces[i].gameObject.activeInHierarchy) 
                    handPlaces[i].gameObject.transform.position = handPlacesVec3[start++];
        }            
        //
        //检测玩家选中的牌型是否满足条件，开启按钮
        private void checkSelectedCards()
        {
            //获取PokerGroup
            PokerGroup pkgSelected = getPokerGroup(orderSelected);

            //没人能管或第一次出牌
            if(gameController.whoisrule == gameController.whoseturn) {
                if (pkgSelected.type != PKType.PK_NAT) 
                    Bt_doRule.GetComponent<Button>().enabled = true;       
                else
                    Bt_doRule.GetComponent<Button>().enabled = false;
                return;
            }
            //可以管上
            if(pkgSelected.type == PKType.PK_WANG_ZHA)
                Bt_doRule.GetComponent<Button>().enabled = true;
            else if (pkgSelected.type == gameController.lastGroup.type && pkgSelected.pokers.Length == gameController.lastGroup.pokers.Length && pkgSelected.value > gameController.lastGroup.value)
                Bt_doRule.GetComponent<Button>().enabled = true;
            else if(pkgSelected.type == PKType.PK_BOMB && gameController.lastGroup.type != PKType.PK_WANG_ZHA)
                Bt_doRule.GetComponent<Button>().enabled = true;
            else
                Bt_doRule.GetComponent<Button>().enabled = false;
        }
        //
        //管上（由玩家触发）
        private void rule()
        {
            AudioSource.PlayClipAtPoint(click3, transform.localPosition);

            //更新lastGroup
            gameController.SendMessage("updateLastGroup", getPokerGroup(orderSelected));
            //删除handPlaces
            for (int i = 0; i < handPlacesSelected.Count; i++)  handPlaces[handPlacesSelected[i]].gameObject.SetActive(false);
            //显示打出的手牌
            for(int i = 0; i < orderSelected.Count; i++)
                outPlaces[i].GetComponent<SpriteRenderer>().sprite = outPlaces[i].GetComponent<SpriteRenderer>().sprite = Instantiate(new Poker(orderSelected[i]).sp, outPlaces[i].GetComponent<Transform>().position, outPlaces[i].GetComponent<Transform>().rotation, outPlaces[i].GetComponent<Transform>());
            //删除pokers
            foreach (int order in orderSelected)
                foreach (KeyValuePair<int,List<Poker>> pair in pokers)
                    pair.Value.Remove(new Poker(order));
            //牌数减少
            cardNum -= orderSelected.Count;
            //清理容器
            orderSelected.Clear(); hints.Clear(); handPlacesSelected.Clear();
            //关闭按钮和提示
            QuietBt();
            //停止计时
            endCounting();
            //调整手牌位置
            adjustHandPos();
            //更换AI表情
            if (isLandLord) {
                gameController.roles[gameController.boor1].GetComponent<SpriteRenderer>().sprite = sp_zhengChang;
                gameController.roles[gameController.boor2].GetComponent<SpriteRenderer>().sprite = sp_zhengChang;
            }
            else {
                gameController.roles[gameController.lord].GetComponent<SpriteRenderer>().sprite = sp_zhengChang;
            }
            //轮转
            gameController.SendMessage("ruleCall");
        }
        //
        //不叫
        private void dontRule()
        {
            AudioSource.PlayClipAtPoint(click3, transform.localPosition);

            //所有选中的牌复位
            foreach (Button b in handPlaces) 
                if (b && b.GetComponent<HandPokerController>().IsUp) {
                    b.GetComponent<Transform>().position = new Vector3(b.GetComponent<Transform>().position.x, b.GetComponent<Transform>().position.y - HandPokerController.offset, b.GetComponent<Transform>().position.z);
                    b.GetComponent<HandPokerController>().IsUp = false;
                }            
            //清理容器
            hints.Clear(); orderSelected.Clear(); handPlacesSelected.Clear();
            //关闭按钮和提示
            QuietBt(); 
            //停止计时
            endCounting();
            //更换AI表情
            if (isLandLord) {
                gameController.roles[gameController.boor1].GetComponent<SpriteRenderer>().sprite = sp_zhengChang;
                gameController.roles[gameController.boor2].GetComponent<SpriteRenderer>().sprite = sp_zhengChang;
            }
            else {
                gameController.roles[gameController.lord].GetComponent<SpriteRenderer>().sprite = sp_zhengChang;
            }
            //反馈给gameController
            gameController.SendMessage("dontRuleCall");
        }
        //
        //提示
        private void showHints()
        {
            AudioSource.PlayClipAtPoint(click3, transform.localPosition);

            //先将玩家选中的牌放回
            foreach (Button b in handPlaces)
                if (b.IsActive() &&b.GetComponent<HandPokerController>().IsUp)
                    b.GetComponent<HandPokerController>().SendMessage("OnClick");

            foreach (Pos p in hints) 
                foreach(Button b in handPlaces) 
                    if(b.IsActive() && b.GetComponent<HandPokerController>().Order == pokers[p.x][p.y].getOrder()) {
                        b.GetComponent<HandPokerController>().SendMessage("OnClick");    
                        break;
                    }        
        }
        //
        //停用按钮和提示
        private void QuietBt()
        {
            Bt_dontRule.GetComponent<Button>().enabled = false;
            Bt_dontRule.SetActive(false);
            Bt_doRule.GetComponent<Button>().enabled = false;
            Bt_doRule.SetActive(false);
            Bt_hints.GetComponent<Button>().enabled = false;
            Bt_hints.SetActive(false);

            foreach (Button b in handPlaces)
                b.enabled = false;

            cantRuleText.SetActive(false);
        }
        //
        //启用按钮
        private void ActivateBt()
        {
            Bt_dontRule.SetActive(true);
            Bt_dontRule.GetComponent<Button>().enabled = true;
            Bt_doRule.SetActive(true);
            Bt_hints.SetActive(true);

            foreach (Button b in handPlaces) 
                b.enabled = true;          
        }

        private void Update()
        {
            if(isCounting) {
                deltaTime += Time.deltaTime;
                //玩家未在规定时间内决策则自动出牌或不叫
                if(totalTime <= 0) {
                    dontRule();
                }
                //变换AI图片
                if(totalTime == 7 && isRush == false) {
                    isRush = true;
                    AudioSource.PlayClipAtPoint(rush, new Vector3(0, 0, 0));
                    if (isLandLord) {
                        gameController.roles[gameController.boor1].GetComponent<SpriteRenderer>().sprite = sp_moJi;
                        gameController.roles[gameController.boor2].GetComponent<SpriteRenderer>().sprite = sp_moJi;
                    }
                    else {
                        gameController.roles[gameController.lord].GetComponent<SpriteRenderer>().sprite = sp_moJi;
                    }
                }

                if(deltaTime >= 1.0f) {
                    totalTime -= 1;
                    timeText.GetComponent<Text>().text = totalTime.ToString();
                    deltaTime = 0;
                }
            }
        }

        public GameObject Bt_dontRule;                          //不叫按钮
        public GameObject Bt_doRule;                             //出牌按钮
        public GameObject Bt_hints;                                 //提示按钮
        public GameObject cantRuleText;                         //要不起提示
        public Button[] handPlaces;                                  //20个手牌
        private Vector3[] handPlacesVec3;                       //20个手牌的位置
        private List<int> handPlacesSelected;                  //玩家选择的打出手牌在handPlaces中的位置
        private List<int> orderSelected;                           //玩家选中的牌的order 

        Sprite sp_moJi, sp_zhengChang;                              //AI表情
        public AudioClip click3;                                          //点击音效
        public AudioClip click4;
        public AudioClip rush;
        private bool isRush;                                  
    }
}
