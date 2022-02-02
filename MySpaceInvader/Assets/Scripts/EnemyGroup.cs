using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EnemyGroup : MonoBehaviour
{
    [Header("Config")]
    [SerializeField] private float[] speeds = { 1, 3, 5, 7, 10 };
    [SerializeField] private float downDistance;

    [SerializeField] private int row;
    [SerializeField] private int col;

    [SerializeField] private float formationStartX;
    [SerializeField] private float formationXGap;
    [SerializeField] private float formationStartY;
    [SerializeField] private float formationYGap;

    [Header("Ref")]   
    [SerializeField] private ObjectPooler bulletPooler;

    [Header("Prefabs")]
    [SerializeField] private GameObject enemy1;
    [SerializeField] private GameObject enemy2;
    [SerializeField] private GameObject enemy3;

    [HideInInspector] public int enemyCount;

    private GameManager gameManager;

    private Enemy[,] group;
    private List<Enemy> fireList;   // Enemies that will fire 

    private int phase;
    private bool isLeft;
    private float lastFireTime;


    private void Awake()
    {
        SpawnEnemies();
        ResetState();
    }

    void Start()
    {
        gameManager = GameManager.GetInstance();
    }

    void FixedUpdate()
    {
        if (!gameManager.IsGameplayState())
            return;

        UpdateGroupMovement();

        Fire();
    }

    public void ResetState()
    {
        // Reset state
        enemyCount = row * col;
        phase = 0;

        // Reset each enemy
        for (int i = 0; i < row; i++)
        {
            for (int j = 0; j < col; j++)
            {
                group[i, j].ResetState();
                group[i, j].transform.localPosition = new Vector3(formationStartX + j * formationXGap, formationStartY + i * formationYGap, 0);
            }
        }

        // Reset fireList
        fireList = new List<Enemy>();
        for (int j = 0; j < col; j++)
        {
            fireList.Add(group[0, j]);
        }
    }

    public void OnEnemyHit(Enemy enemy)
    {
        enemyCount--;

        gameManager.AddPoint(enemy.hitPoint);

        UpdatePhase();

        UpdateFireList(enemy);

        if (enemyCount == 0)
        {
            // We don't want the last enemy to interfere with cutscene
            enemy.dontCollide = true;

            gameManager.EndGame();
        }
    }

    private void SpawnEnemies()
    {
        group = new Enemy[row, col];
        for (int i = 0; i < row; i++)
        {
            for (int j = 0; j < col; j++)
            {
                GameObject go = null;
                if (i < 2) go = enemy1;
                else if (i < 4) go = enemy2;
                else go = enemy3;

                GameObject newGo = Instantiate(go, this.transform);

                Enemy e = newGo.GetComponent<Enemy>();
                e.group = this;
                e.coord = new Vector2Int(i, j);
                e.bulletPooler = bulletPooler;
                group[i, j] = e;
            }
        }
    }

    private void Fire()
    {
        if (Time.time - lastFireTime < 0.1f)
            return;

        lastFireTime = Time.time;
        foreach (Enemy e in fireList)
        {
            e.Fire();
        }
    }

    private void UpdateGroupMovement()
    {
        float speed = speeds[phase];
        speed *= isLeft ? -1 : 1;

        float down = 0;
        foreach (Enemy e in group)
        {
            if (e.isDead)
                continue;

            if ((e.transform.position.x > gameManager.boundMax && !isLeft) ||
                (e.transform.position.x < gameManager.boundMin && isLeft))
            {
                down = -downDistance;
                isLeft = !isLeft;
                break;
            }
        }

        foreach (Enemy e in group)
        {
            if (e.isDead)
                continue;

            e.transform.position += new Vector3(speed * Time.fixedDeltaTime, down, 0);

            if (e.transform.position.y < 0)
            {
                gameManager.EndGame();
                return;
            }
        }
    }

    private void UpdatePhase()
    {     
        if (enemyCount <= 1)
        {
            phase = 4;
        }
        else if (enemyCount <= 10)
        {
            phase = 3;
        }
        else if (enemyCount <= row * col - 15)
        {
            phase = 2;
        }
        else if (enemyCount <= row * col - 5)
        {
            phase = 1;
        }
        else
        {
            phase = 0;
        }
    }

    private void UpdateFireList(Enemy enemy)
    {
        if (!fireList.Contains(enemy))
            return;

        fireList.Remove(enemy);

        int x = enemy.coord.x;
        int y = enemy.coord.y;
        if (x + 1 < row && !group[x + 1, y].isDead)
        {
            fireList.Add(group[x + 1, y]);
        }
    }
}
