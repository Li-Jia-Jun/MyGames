using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BunkerPart : MonoBehaviour
{
    private Bunker bunker;

    private void Start()
    {
        bunker = transform.parent.GetComponent<Bunker>();
    }

    void OnTriggerEnter(Collider other)
    {
        if (other.CompareTag("PlayerBullet") || other.CompareTag("EnemyBullet") || other.CompareTag("Enemy") || other.CompareTag("Laser"))
        {
            bunker.OnBunkerPartHit();
            gameObject.SetActive(false);
        }
    }
}
