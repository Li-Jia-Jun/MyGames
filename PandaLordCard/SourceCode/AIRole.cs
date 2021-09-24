using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;

using PokerNameSpace;

namespace RoleNameSpace
{
    public class AIRole : Role
    {
        private void Awake()
        {
            init();
            delayTime = outA = out2 = outWang = out7 = out10 = 0;
            //预先加载图片资源
            sp_lengJing = Resources.Load("emoji/冷静分析", typeof(Sprite)) as Sprite;
            sp_reDong = Resources.Load("emoji/热动分析", typeof(Sprite)) as Sprite;
            sp_sheHui = Resources.Load("emoji/社会", typeof(Sprite)) as Sprite;
            sp_shiBai = Resources.Load("emoji/分析失败", typeof(Sprite)) as Sprite;
            sp_buNeng = Resources.Load("emoji/不能分析", typeof(Sprite)) as Sprite;
            sp_guanShang1 = Resources.Load("emoji/管上1", typeof(Sprite)) as Sprite;
            sp_guanShang2 = Resources.Load("emoji/管上2", typeof(Sprite)) as Sprite;
            sp_guanShang3 = Resources.Load("emoji/管上3", typeof(Sprite)) as Sprite;
            sp_zhengChang = Resources.Load("emoji/emmm", typeof(Sprite)) as Sprite;
            sp_ziYou = Resources.Load("emoji/自由出牌", typeof(Sprite)) as Sprite;
            sp_ziYou2 = Resources.Load("emoji/自由出牌2", typeof(Sprite)) as Sprite;
            sp_ziYou3 = Resources.Load("emoji/自由出牌3", typeof(Sprite)) as Sprite;
        }

        protected override void discard()
        {
            canRule = checkCanRule();

            base.discard();
            noteText.GetComponent<Text>().text = "";
            //记牌
            markCards();
            //开始计时
            startCounting();
            //随机停一段时间
            System.Random r = new System.Random();
            if(canRule)
                delayTime = r.Next(1, 3);
            else
                delayTime = r.Next(2, 4);

            //随机加载一张“正在思考图片”
            if (gameController.whoisrule == gameController.whoseturn)
                GetComponent<SpriteRenderer>().sprite = sp_lengJing;
            else
                GetComponent<SpriteRenderer>().sprite = sp_reDong;
        }
 
        public override void getCards(int[] cards)
        {
            base.getCards(cards);
            cardNumText.GetComponent<Text>().text = "牌数：" + cardNum;
            lordText.GetComponent<Text>().text = "农民";
        }
        public override void getExtra3(int p1, int p2, int p3)
        {
            base.getExtra3(p1, p2, p3);
            cardNumText.GetComponent<Text>().text = "牌数：" + cardNum;
            lordText.GetComponent<Text>().text = "地主";
        }
        //
        //通用自由出牌模式
        private void freeDiscard()
        {
            /* 策略是：优先出组合牌型，再考虑单张牌 */

            //外面没有顺子时，考虑顺序：顺子、3张、2张、单张、炸弹、4张、王炸
            if (pokers[7].Count + out7 == 4 && pokers[10].Count + out10 == 4) 
            {
                if ( findRuleSHUN3() || findRuleSHUN2() || findRuleSHUN()
                   || findRule3_DUI() || findRule3_DAN() || findRuleDUI() || findRuleDAN()
                   || findRule4_DAN_DAN() || findRule4_DUI()
                   || findRuleWANG_ZHA() || findRuleBOMB() ) {
                    //如果手牌太大则调整
                    if (getPokerGroup(hints).value >= 10 && cardNum >= 8) {
                        if (findRuleDUI() && findRuleDAN()) discardImp();   
                    }
                    else
                        discardImp();
                }
            }
            else //否则考虑顺序：3张、顺子、2张、单张、4张
            {
                if ( findRule3_DUI() || findRule3_DAN()
                   || findRuleSHUN3() || findRuleSHUN2() || findRuleSHUN()
                   || findRuleDUI() || findRuleDAN()
                    || findRule4_DAN_DAN() || findRule4_DUI()) {
                    //如果手牌太大则调整
                    if (getPokerGroup(hints).value >= 10 && cardNum >= 8) {
                        if (findRuleDUI() && findRuleDAN()) discardImp();
                    }
                    else
                        discardImp();
                }
            }

            hints.Clear(); endCounting();
            gameController.SendMessage("ruleCall");
        }
        //
        //农民自由出牌
        private void boorFreeDiscard()
        {
            /* 策略是：如果下一轮是另一个农民出牌且他手牌比自己少时，优先考虑出单张或对子给他过
                               下一轮是地主出牌则优先考虑多张  */

            //更换表情
            System.Random r = new System.Random();
            switch(r.Next(0,3))
            {
                case 0: GetComponent<SpriteRenderer>().sprite = sp_ziYou; break;
                case 1: GetComponent<SpriteRenderer>().sprite = sp_ziYou2; break;
                case 2: GetComponent<SpriteRenderer>().sprite = sp_ziYou3; break;
            }

            int next = (gameController.whoseturn + 1) % 3;
            //下个是地主
            if (next == gameController.lord) 
                freeDiscard();
            //下个是农民     
            else
            {
                if (gameController.roles[next].cardNum < cardNum - 5) 
                {
                    if (findRuleDAN() || findRuleDUI()
                        || findRule3_DUI() || findRule3_DAN()
                        || findRule4_DAN_DAN() || findRule4_DUI()
                        || findRuleSHUN() || findRuleSHUN2() || findRuleSHUN3()
                        || findRuleWANG_ZHA() || findRuleBOMB()) {
                        discardImp();
                        hints.Clear(); endCounting();
                        gameController.SendMessage("ruleCall");
                    }
                }
                else 
                {
                    freeDiscard();
                }
            }        
        }
        //
        //地主自由出牌
        private void lordFreeDiscard()
        {
            //更换表情
            System.Random r = new System.Random();
            switch (r.Next(0, 3)) {
                case 0: GetComponent<SpriteRenderer>().sprite = sp_ziYou; break;
                case 1: GetComponent<SpriteRenderer>().sprite = sp_ziYou2; break;
                case 2: GetComponent<SpriteRenderer>().sprite = sp_ziYou3; break;
            }
            //直接采用自由出牌模式
            freeDiscard();
        }
        //
        //农民管上出牌
        private void boorRuleDiscard()
        {
            /* 策略是：如果上家是地主且地主手牌较多时适当打压，否则尽量打压
             *                是农民则与自己的手牌数比较，相差小于5张或是可以出较小牌则出牌 */

            int lastCardNum = gameController.roles[gameController.whoisrule].cardNum;
            PokerGroup pkg = getPokerGroup(hints);
            if (gameController.whoisrule == gameController.lord)//上家是地主 
            {
                if ( lastCardNum >= 15 ) 
                 {           
                    if (pkg.type == PKType.PK_WANG_ZHA || pkg.type == PKType.PK_BOMB) { dontDiscard(); return; }
                    if ((pkg.type == PKType.PK_DAN || pkg.type == PKType.PK_DUI || pkg.type == PKType.PK_SAN) && (pkg.pokers[0].Id == 1 || pkg.pokers[0].Id == 2)) { dontDiscard(); return; }
                }
                discardImp();
            }
            else//上家是农民
            {
                if(pkg.type == PKType.PK_BOMB || pkg.type == PKType.PK_WANG_ZHA) { dontDiscard(); return; }
                //牌数相差小于5张以内则出牌
                else if (cardNum - lastCardNum < 5) discardImp();
                else if( (pkg.type == PKType.PK_DAN || pkg.type == PKType.PK_DUI) && pkg.value < 8 ) discardImp();
                else { dontDiscard(); return; }
            }
            //更换表情
            System.Random r = new System.Random();
            switch (r.Next(0, 3)) {
                case 0: GetComponent<SpriteRenderer>().sprite = sp_guanShang1; break;
                case 1: GetComponent<SpriteRenderer>().sprite = sp_guanShang2; break;
                case 2: GetComponent<SpriteRenderer>().sprite = sp_guanShang3; break;
            }
            hints.Clear(); endCounting();
            gameController.SendMessage("ruleCall");
        }
        //
        //地主管上出牌
        private void lordRuleDiscard()
        {
            /*  
                策略是： 只在农民手牌10张以内才用大牌，其他情况则能管就管，管不上就过
             */
      
            if (gameController.roles[gameController.boor1].cardNum > 10 && gameController.roles[gameController.boor2].cardNum > 10) {
                PokerGroup pkg = getPokerGroup(hints);
                if (pkg.type == PKType.PK_WANG_ZHA || pkg.type == PKType.PK_BOMB) { dontDiscard(); return; }
                if (pkg.type == PKType.PK_DAN || pkg.type == PKType.PK_DUI || pkg.type == PKType.PK_SAN && (pkg.pokers[0].Id == 1 || pkg.pokers[0].Id == 2)) { dontDiscard(); return; }
                discardImp();
            }
            else {
                //hints已经在checkCanRule()时更新了
                discardImp();
            }
            //更换表情
            System.Random r = new System.Random();
            switch (r.Next(0, 3)) {
                case 0: GetComponent<SpriteRenderer>().sprite = sp_guanShang1; break;
                case 1: GetComponent<SpriteRenderer>().sprite = sp_guanShang2; break;
                case 2: GetComponent<SpriteRenderer>().sprite = sp_guanShang3; break;
            }
            endCounting(); hints.Clear();
            gameController.SendMessage("ruleCall");
        }
        //
        //不叫
        private void dontDiscard()
        {
            AudioSource.PlayClipAtPoint(click4, new Vector3(0, 0, 0));
            noteText.GetComponent<Text>().text = "不叫";
            //更好表情
            System.Random r = new System.Random();
            switch(r.Next(0, 3))
            {
                case 1: GetComponent<SpriteRenderer>().sprite = sp_shiBai; break;
                case 0: GetComponent<SpriteRenderer>().sprite = sp_sheHui; break;
                case 2: GetComponent<SpriteRenderer>().sprite = sp_buNeng; break;
            }    

            endCounting();
            hints.Clear();
            gameController.SendMessage("dontRuleCall");
        }
        //
        //记牌
        private void markCards()
        {
            //记牌内容：
            //7、10（判断外面是否可能有顺子）
            //A、2（判断外面的大牌）
            //王（判断是否可能有王炸）
            if (gameController.lastGroup.type == PKType.PK_NAT)  return;

            foreach (Poker poker in gameController.lastGroup.pokers) {
                switch (poker.Id){
                    case 1: outA++; break;
                    case 2: out2++; break;
                    case 7: out7++; break;
                    case 10: out10++; break;
                    case 14: outWang++; break;
                }
            }
        }
        //
        //实际出牌操作
        private void discardImp()
        {
            AudioSource.PlayClipAtPoint(click4, new Vector3(0, 0, 0));
            noteText.GetComponent<Text>().text = "出牌";
            gameController.SendMessage("updateLastGroup", getPokerGroup(hints));

            List<Poker> temp = new List<Poker>();
            List<int> temp2 = new List<int>();

            for (int i = 0; i < hints.Count; i++) {
                int x = hints[i].x, y = hints[i].y;
                outPlaces[i].GetComponent<SpriteRenderer>().sprite = Instantiate(pokers[x][y].sp, outPlaces[i].GetComponent<Transform>().position, outPlaces[i].GetComponent<Transform>().rotation, outPlaces[i].GetComponent<Transform>());
                temp.Add(pokers[x][y]); temp2.Add(x);
            }

            for (int i = 0; i < temp.Count; i++)  pokers[temp2[i]].Remove(temp[i]);

            cardNum -= hints.Count;
            cardNumText.GetComponent<Text>().text = "牌数：" + cardNum;
            temp.Clear(); temp2.Clear();
        }

        private void Update()
        {
            if(isCounting) {
                deltaTime += Time.deltaTime;
                if (deltaTime >= 1.0f) {
                    totalTime -= 1; delayTime -= 1;
                    timeText.GetComponent<Text>().text = totalTime.ToString();
                    deltaTime = 0;
                }

                if(delayTime == 0)
                {
                    if (canRule) //管上
                    {
                        if (gameController.lastGroup.type == PKType.PK_NAT) { //地主第一次出牌
                            freeDiscard();
                        }
                        else if (gameController.whoseturn == gameController.whoisrule) {  //没人管上时出牌
                            if (isLandLord) lordFreeDiscard();
                            else boorFreeDiscard();
                        }
                        else { //管上出牌
                            if (isLandLord) lordRuleDiscard();
                            else boorRuleDiscard();
                        }
                    }
                    else//不叫
                    {
                        dontDiscard();
                    }
                }
            }
        }

        int delayTime;                                           //AI每次出牌的延迟时间
        int outA, out2, outWang, out7, out10;     //记录已经打出的A、2、王、7、10

        public GameObject cardNumText;           //显示手牌数Text
        public GameObject noteText;                  //消息Text
        public GameObject lordText;                   //是否是地主Text

        // 表情
        private Sprite sp_sheHui, sp_shiBai, sp_buNeng, sp_lengJing, sp_reDong, sp_guanShang1, sp_guanShang2, sp_guanShang3, sp_zhengChang;
        private Sprite sp_ziYou, sp_ziYou2, sp_ziYou3;
        //音效
        public AudioClip click4;
    }
}
