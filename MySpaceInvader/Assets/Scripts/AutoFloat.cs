using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AutoFloat : MonoBehaviour
{
    public float speed;

    public float xScale;
    public float yScale;


    void FixedUpdate()
    {
        transform.position += new Vector3(Mathf.Sin(speed * Time.time) * xScale, Mathf.Cos(speed * Time.time) * yScale, 0);
    }
}
