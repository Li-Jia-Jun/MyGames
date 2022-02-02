using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UFO : MonoBehaviour
{
    [Header("Config")]
    [SerializeField] private float speed;
    [SerializeField] private float appearChance;
    [SerializeField] private float appearGap;
    public int hitPoint; 

    private float lastAppear;
    private bool appear;
    private Vector3 originPos;

    private AudioSource audioSource;
    private GameManager gameManager;


    void Awake()
    {
        audioSource = GetComponent<AudioSource>();

        originPos = transform.position;
        gameObject.SetActive(false);
    }

    void Start()
    {
        gameManager = GameManager.GetInstance();
    }

    void OnTriggerEnter(Collider other)
    {
        if (!other.CompareTag("PlayerBullet"))
            return;

        audioSource.Play();

        gameManager.AddPoint(hitPoint);
        lastAppear = Time.time;
        appear = false;
        transform.position = originPos;
    }

    void FixedUpdate()
    {
        if (!gameManager.IsGameplayState())
            return;

        if (!appear && Time.time - lastAppear > appearGap)
        {
            lastAppear = Time.time;
            if (Random.Range(0, 1) <= appearChance)
            {                
                appear = true;
            }
        }

        if (appear)
        {
            transform.position += new Vector3(speed * Time.fixedDeltaTime, 0, 0);
            if (transform.position.x > gameManager.boundMax + 5.0f)
            {
                lastAppear = Time.time;
                transform.position = originPos;
                appear = false;
            }
        }
    }

    public void ResetState()
    {
        gameObject.SetActive(true);
        transform.position = originPos;
        lastAppear = appearGap + Time.time;
    }
}
