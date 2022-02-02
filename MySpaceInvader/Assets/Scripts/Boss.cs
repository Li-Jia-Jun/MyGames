using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Boss : MonoBehaviour
{
    [Header("Ref")]
    [SerializeField] private Player player;
    [SerializeField] private GameObject laser;
    [SerializeField] private GameObject beam;

    [Header("Cofig")]
    [SerializeField] private float fireGap;
    [SerializeField] private float aimTime;
    [SerializeField] private float laserTime;
    [SerializeField] private float recoveryTime;

    [Header("Audio")]
    [SerializeField] private AudioClip aimClip;
    [SerializeField] private AudioClip shootClip;

    private AudioSource audioSource;
    private GameManager gameManager;

    private float lastFire;

    private bool startFight;

    private bool shooting;


    void Awake()
    {
        audioSource = GetComponent<AudioSource>();    
    }

    void Start()
    {
        gameManager = GameManager.GetInstance();
        ResetState();
    }

    void OnTriggerEnter(Collider other)
    {
        if (other.CompareTag("PlayerBullet"))
        {
            gameManager.OnBossHit();
        }
    }

    void FixedUpdate()
    {
        if (startFight && gameManager.IsGameplayState())
        {
            if (!shooting && Time.time - lastFire > fireGap)
            {
                StartCoroutine(Shoot());
            }
            
            if(!shooting)
            {
                LookAtPlayer(1);
            }
        }
    }

    public void ResetState()
    {
        lastFire = Time.time;
        shooting = false;

        laser.SetActive(false);
        beam.SetActive(false);
    }

    public void StartFight()
    {
        startFight = true;
    }

    private void LookAtPlayer(float t)
    {
        Quaternion target = Quaternion.LookRotation(player.transform.position - transform.position, new Vector3(0, 0, 1));
        transform.rotation = Quaternion.Slerp(transform.rotation, target, t);
    }

    private IEnumerator Shoot()
    {
        shooting = true;

        beam.SetActive(true);

        audioSource.clip = aimClip;
        audioSource.time = 0;
        audioSource.Play();

        yield return new WaitForSeconds(aimTime + Random.Range(0, aimTime * 0.5f));

        if (!gameManager.IsGameplayState())
            goto END;

        audioSource.Pause();
        audioSource.clip = shootClip;
        audioSource.time = 0;
        audioSource.Play();

        laser.SetActive(true);

        yield return new WaitForSeconds(laserTime);

        if (!gameManager.IsGameplayState())
            goto END;

        laser.SetActive(false);

        beam.SetActive(false);

        float time = Time.time;
        while (Time.time < time + recoveryTime)
        {
            LookAtPlayer((Time.time - time) / recoveryTime);
            yield return 0;
        }       

        shooting = false;
        lastFire = Time.time;

    END:
        ;
    }
}