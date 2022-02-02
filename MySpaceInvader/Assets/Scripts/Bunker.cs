using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Bunker : MonoBehaviour
{
    private List<GameObject> parts;
    private AudioSource audioSource;

    private void Awake()
    {
        audioSource = GetComponent<AudioSource>();
    }

    public void OnBunkerPartHit()
    {
        audioSource.Play();
    }

    public void ResetState()
    {
        if (parts == null)
        {
            parts = new List<GameObject>();
            for (int i = 0; i < transform.childCount; i++)
            {
                parts.Add(transform.GetChild(i).gameObject);
            }
        }

        for (int i = 0; i < parts.Count; i++)
        {
            parts[i].SetActive(true);
        }
    }
}
