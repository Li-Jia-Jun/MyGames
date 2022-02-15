using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LightBreath : MonoBehaviour
{
    private SpriteRenderer sr;
    private float i = 0;
    private bool isOne = false;

    public float speed = 0.5f;
    public float lowerLight = 0.2f;

    void Start()
    {
        sr = GetComponent<SpriteRenderer>();
    }

    void Update()
    {
        if (!isOne)
        {
            i += Time.deltaTime * speed;
            sr.color = new Color(sr.color.r, sr.color.g, sr.color.b, i);
            if (i >= 1)
                isOne = true;
        }
        else
        {
            i -= Time.deltaTime * speed;
            sr.color = new Color(sr.color.r, sr.color.g, sr.color.b, i);
            if (i <= lowerLight)
                isOne = false;
        }

        //i += speed;
        //float a = (Mathf.Sin(i)+1)* 0.5f;
        //if (a < lowerLight)
        //    a = lowerLight;
        //sr.color = new Color(sr.color.r, sr.color.g, sr.color.b, a);
    }
}
