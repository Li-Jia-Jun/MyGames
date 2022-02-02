using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class Bullet : MonoBehaviour
{
    [Header("Prefab")]
    [SerializeField] private GameObject explosion;

    private Rigidbody rigidBody;
    private ObjectPooler pooler;
    private GameManager gameManager;

    [HideInInspector] public string dontCollideTag;


    void Awake()
    {
        rigidBody = GetComponent<Rigidbody>();
        pooler = transform.parent.GetComponent<ObjectPooler>(); // Bullets will be spawned by pooler so we can safely request pooler at Awake()
    }

    void Start()
    {
        gameManager = GameManager.GetInstance();
    }

    void OnTriggerEnter(Collider other)
    {
        if (!other.CompareTag(dontCollideTag))
        {
            if (!other.CompareTag("Wall"))
            {
                Instantiate(explosion, transform.position, transform.rotation);
            }
            
            pooler.backToPool(gameObject);
        }
    }

    void FixedUpdate()
    {
        if (!gameManager.IsGameplayState())
        {
            pooler.backToPool(gameObject);
        }
    }

    public void Fire(Vector3 pos, Vector3 vel)
    {
        transform.position = pos;
        rigidBody.velocity = vel;
    }
}
