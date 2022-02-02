using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AutoRotate : MonoBehaviour
{
    public float rotSpeed;
    public Vector3 axis;


    void FixedUpdate()
    {
        transform.Rotate(axis * rotSpeed * Time.fixedDeltaTime);        
    }
}
