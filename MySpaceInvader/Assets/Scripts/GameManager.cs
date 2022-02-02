using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.Playables;

public class GameManager : MonoBehaviour
{
    [Header("UI")]
    [SerializeField] private GameObject mainMenuUIRoot;
    [SerializeField] private GameObject gameplayUIRoot;
    [SerializeField] private Text pointUI;
    [SerializeField] private Text playerLifeUI;
    [SerializeField] private GameObject winUI;
    [SerializeField] private GameObject loseUI;
    [SerializeField] private Slider bossLifeUI;

    [Header("Andirod UI")]
    [SerializeField] private GameObject startMenu_andriod;
    [SerializeField] private GameObject gameplay_andriod;

    [Header("Ref")]
    [SerializeField] private Player player;
    [SerializeField] private EnemyGroup enemyGroup;
    [SerializeField] private UFO ufo;
    [SerializeField] private Bunker[] bunkers;
    [SerializeField] private Boss boss;

    [SerializeField] private AudioSource audio;

    [SerializeField] private AudioSource ambient;

    [Header("Config")]
    public int playerMaxLife = 10;
    public int bossMaxLife = 25;
    public int boundMax = 12;
    public int boundMin = -12;

    [Header("Audio")]
    [SerializeField] private AudioClip bgm;
    [SerializeField] private AudioClip bgmBoss;

    [Header("Timeline")]
    [SerializeField] private PlayableDirector gameStartTimeline;
    [SerializeField] private PlayableDirector bossFightTimeline;

    GameState _state;
    bool bossFight;

    // Data
    private int point;
    private int playerLife;
    private int bossLife;

    // Singlton
    private static GameManager _instance;
    public static GameManager GetInstance()
    {
        return _instance;
    }


    void Awake()
    {
        _instance = this;
    }

    void Start()
    {
        MainMenu();
    }

    void Update()
    {
        switch (_state)
        {
            case GameState.StartMenu:
                if (Input.GetKeyDown(KeyCode.R))
                {
                    StartGameTimeline();
                }
                break;

            case GameState.Gameplay:
                break;

            case GameState.Cutscene:
                break;

            case GameState.Gameover:
                if (Input.GetKeyDown(KeyCode.R))
                {
                    if (loseUI.activeSelf)
                    {
                        if (bossFight)
                        {
                            StartBossFight();
                        }
                        else
                        {
                            StartGame();
                        }
                    }
                    else if (winUI.activeSelf)
                    {
                        MainMenu();
                    }
                }
                break;
        }
    }

    public bool IsGameplayState()
    {
        return _state == GameState.Gameplay;
    }

    public void AddPoint(int add)
    {      
        point += add;
        pointUI.text = point.ToString();
    }

    public void PlayerMinusLife(int minus = 1)
    {
        playerLife -= minus;

        playerLifeUI.text = playerLife.ToString();

        if (playerLife <= 0)
        {
            EndGame();
            return;
        }     
    }

    public void StartGameTimeline()
    {
        gameStartTimeline.Play();
        mainMenuUIRoot.SetActive(false);
    }

    public void EndGame()
    {
        audio.Pause();

        if (bossFight)
        {
            _state = GameState.Gameover;
            if (bossLife <= 0)
            {
                winUI.SetActive(true);
            }
            else
            {
                loseUI.SetActive(true);
            }
        }

        if (playerLife > 0 && enemyGroup.enemyCount <= 0)
        {
            if (!bossFight)
            {
                StartCoroutine(StartBossfightTimeline());
            }
            else
            {
                _state = GameState.Gameover;
                winUI.SetActive(true);
            }          
        }
        else
        {
            _state = GameState.Gameover;
            loseUI.SetActive(true);
        }
    }

    public void StartGame()
    {
        _state = GameState.Gameplay;

        ambient.volume = 0.2f;

        audio.clip = bgm;
        audio.time = 0f;
        audio.Play();
     
        gameplayUIRoot.SetActive(true);
        winUI.SetActive(false);
        loseUI.SetActive(false);

        point = 0;
        pointUI.text = point.ToString();

        playerLife = playerMaxLife;
        playerLifeUI.text = playerLife.ToString();

        player.ResetState();
        enemyGroup.ResetState();
        ufo.ResetState();
        for (int i = 0; i < bunkers.Length; i++)
        {
            bunkers[i].ResetState();
        }

        boss.gameObject.SetActive(false);

#if UNITY_ANDROID
        gameplay_andriod.SetActive(true);
#endif
    }

    public void MainMenu()
    {
        _state = GameState.StartMenu;

        bossFight = false;

        ambient.volume = 0.4f;

        mainMenuUIRoot.SetActive(true);
        gameplayUIRoot.SetActive(false);
        winUI.SetActive(false);
        loseUI.SetActive(false);
        bossLifeUI.gameObject.SetActive(false);
        pointUI.text = "";
        playerLifeUI.text = "";
   
        gameStartTimeline.time = 0;
        gameStartTimeline.Stop();

        bossFightTimeline.time = 0;
        bossFightTimeline.Stop();

        player.ResetState();
        enemyGroup.ResetState();
        ufo.ResetState();
        for (int i = 0; i < bunkers.Length; i++)
        {
            bunkers[i].ResetState();
        }

        boss.gameObject.SetActive(false);

#if UNITY_ANDROID
        startMenu_andriod.SetActive(true);
#endif
    }

    public IEnumerator StartBossfightTimeline()
    {
        yield return new WaitForSeconds(2f);

        ambient.volume = 0.4f;

        _state = GameState.Cutscene;

        ufo.gameObject.SetActive(false);       

        player.ResetState();

        gameplayUIRoot.SetActive(false);

        bossFightTimeline.Play();
    }

    public void StartBossFight()
    {
        _state = GameState.Gameplay;

        ambient.volume = 0.2f;

        bossLife = bossMaxLife;
        bossLifeUI.gameObject.SetActive(true);
        bossLifeUI.value = 1;

        audio.clip = bgmBoss;
        audio.volume = 0.35f;
        audio.time = 0f;
        audio.Play();

        winUI.SetActive(false);
        loseUI.SetActive(false);

        playerLife = playerMaxLife;
        playerLifeUI.text = playerLife.ToString();

        bossFight = true;

        boss.gameObject.SetActive(true);
        boss.ResetState();
        boss.StartFight();

        gameplayUIRoot.SetActive(true);

#if UNITY_ANDROID
        gameplay_andriod.SetActive(true);
#endif
    }

    public void OnBossHit()
    {
        bossLife--;
        bossLifeUI.value = (bossLife * 1.0f) / bossMaxLife;

        AddPoint(10);

        if (bossLife <= 0)
        {
            EndGame();
        }
    }
}

enum GameState
{ 
    StartMenu, Cutscene, Gameplay, Gameover,
}