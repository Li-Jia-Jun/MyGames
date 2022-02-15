using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DoorController : MonoBehaviour
{
    public GameObject doorOpen;
    public GameObject doorClose;
    public GameObject buttomUp;
    public GameObject buttomDown;

    void Start()
    {
        doorClose.SetActive(true);
        buttomUp.SetActive(true);

        doorOpen.SetActive(false);
        buttomDown.SetActive(false);
    }


    void OnTriggerEnter2D(Collider2D collider2D)
    {
        doorOpen.SetActive(true);
        buttomDown.SetActive(true);

        doorClose.SetActive(false);
        buttomUp.SetActive(false);
    }

    void OnTriggerExit2D(Collider2D collider2D)
    {
        doorClose.SetActive(true);
        buttomUp.SetActive(true);

        doorOpen.SetActive(false);
        buttomDown.SetActive(false);
    }

    void Update ()
    {

    }
}
