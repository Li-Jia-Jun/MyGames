using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Enemy : MonoBehaviour
{
    [Header("Ref")]
    [SerializeField] private Transform firePos;
    [SerializeField] private GameObject model;
    [SerializeField] private GameObject explosion;
    [SerializeField] private GameObject explosion2;
    
    [Header("Config")]
    public int hitPoint;
    public float bulletSpeed;
    public float fireGap;

    [HideInInspector] public bool isDead;
    [HideInInspector] public Vector2Int coord;     // Its Coordinate in formation
    [HideInInspector] public EnemyGroup group;
    [HideInInspector] public ObjectPooler bulletPooler;
    [HideInInspector] public bool dontCollide;

    private float lastFire;
    private AudioSource audioSource;
    private Rigidbody rBody;

    private Quaternion oringalRot;


    void Awake()
    {
        audioSource = GetComponent<AudioSource>();
        rBody = GetComponent<Rigidbody>();

        oringalRot = transform.rotation;
    }

    void OnTriggerEnter(Collider other)
    {
        if (other.CompareTag("Wall"))
        {
            model.SetActive(false);
            explosion.SetActive(false);
            rBody.useGravity = false;
            rBody.velocity = Vector3.zero;
            return;
        }

        if (dontCollide)
            return;

        // First hit
        if (!isDead && other.CompareTag("PlayerBullet"))
        {
            isDead = true;

            rBody.useGravity = true;
            rBody.AddTorque(new Vector3(Random.Range(0, 50), 150, 0), ForceMode.Impulse);
            explosion.SetActive(true);

            group.OnEnemyHit(this);
        }
        // Second hit
        else if (isDead && !(other.CompareTag("Enemy") || other.CompareTag("EnemyBullet")))
        {
            explosion2.SetActive(true);
            model.SetActive(false);

            rBody.useGravity = false;
            rBody.velocity = Vector3.zero;
        }
    }

    public void ResetState()
    {
        lastFire = Random.Range(0, 3) + Time.time;
        isDead = false;
        model.SetActive(true);
        explosion.SetActive(false);
        explosion2.SetActive(false);

        dontCollide = false;

        transform.rotation = oringalRot;

        rBody.velocity = Vector3.zero;
        rBody.angularVelocity = Vector3.zero;
        rBody.useGravity = false;
    }

    public void Fire()
    {
        if (Time.time - lastFire < fireGap)
            return;

        if (Random.Range(0.0f, 1.0f) < 0.8f)
            return;

        lastFire = Time.time;

        GameObject go = bulletPooler.getObject();
        if (go != null)
        {
            audioSource.Play();

            Bullet bullet = go.GetComponent<Bullet>();
            bullet.dontCollideTag = "Enemy";
            bullet.Fire(firePos.position, new Vector3(0, -bulletSpeed, 0));
        }
    }
}
