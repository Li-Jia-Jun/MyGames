using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Cinemachine;

public class Player : MonoBehaviour
{
    [Header("Ref")]
    [SerializeField] private ParticleSystem explode;
    [SerializeField] private ObjectPooler bulletPooler;
    [SerializeField] private CameraShake cameraShake;
    [SerializeField] private CameraShake bossCameraShake;
    [SerializeField] private AudioClip fireClip;
    [SerializeField] private AudioClip gethitClip;

    [SerializeField] private Transform firePos;

    [Header("Config")]
    [SerializeField] private float speed;
    [SerializeField] private float rotSpeed;
    [SerializeField] private float bulletSpeed;
    [SerializeField] private float fireGap;

    private Rigidbody rigidBody;
    private AudioSource audioSource;
    
    private GameManager gameManager;

    private Vector3 originalPos;
    private Quaternion originalRot;

    private float fireTimer;
    private float lastLaser;


    void Awake()
    {
        rigidBody = GetComponent<Rigidbody>();
        audioSource = GetComponent<AudioSource>();
        originalPos = transform.position;
        originalRot = transform.rotation;
    }

    void Start()
    {
        gameManager = GameManager.GetInstance();
    }

    void OnTriggerEnter(Collider other)
    {
        if (!gameManager.IsGameplayState())
            return;

        if (other.CompareTag("Laser") && Time.time - lastLaser > 4.0f)
        {
            StartCoroutine(bossCameraShake.Shake(1.2f, 0.5f));

            explode.time = 0;
            explode.Play();

            audioSource.PlayOneShot(gethitClip, 1);

            gameManager.PlayerMinusLife(3);

            lastLaser = Time.time;
        }

        if (other.CompareTag("EnemyBullet") || other.CompareTag("Enemy"))
        {
            if (other.CompareTag("Enemy") && other.GetComponentInParent<Enemy>().dontCollide)
                return;

            StartCoroutine(cameraShake.Shake(0.4f, 0.15f));

            audioSource.PlayOneShot(gethitClip);

            gameManager.PlayerMinusLife();
        }    
    }

    void FixedUpdate()
    {
        if (!gameManager.IsGameplayState())
            return;

        UpdateMovement();

        UpdateFire();
    }

    public void ResetState()
    {
        fireTimer = 0;
        rigidBody.velocity = Vector3.zero;
        transform.position = originalPos;
        transform.rotation = originalRot;      
    }

    private void UpdateMovement()
    {
        float h;
#if UNITY_ANDROID
        h = andriodInput;
#else
        h = Input.GetAxis("Horizontal");
#endif

        // Movement
        rigidBody.velocity += new Vector3(h * speed, 0, 0);
        rigidBody.position = new Vector3(Mathf.Clamp(rigidBody.position.x, gameManager.boundMin, gameManager.boundMax), rigidBody.position.y, rigidBody.position.z);

        // Rotation
        rigidBody.rotation = Quaternion.Euler(rigidBody.rotation.eulerAngles.x, -rigidBody.velocity.x * rotSpeed, rigidBody.rotation.eulerAngles.z);
    }
  
    private void UpdateFire()
    {
        fireTimer += Time.fixedDeltaTime;
        if (fireTimer > fireGap)
        {
            if (Input.GetKey(KeyCode.Space))
            {
                Fire();
                fireTimer = 0.0f;
            }
        }
    }

    public void Fire()
    {
        GameObject go = bulletPooler.getObject();
        if (go != null)
        {
            audioSource.PlayOneShot(fireClip);

            Bullet bullet = go.GetComponent<Bullet>();
            bullet.dontCollideTag = "Player";
            bullet.Fire(firePos.position, new Vector3(0, bulletSpeed, 0));          
        }
    }

#region Andriod Input

    private float andriodInput = 0;

    public void OnLeftButtonDown()
    {
        andriodInput = -1;
    }

    public void OnRightButtonDown()
    {
        andriodInput = 1;
    }

    public void OnButtonUp()
    {
        andriodInput = 0;
    }

#endregion
}
