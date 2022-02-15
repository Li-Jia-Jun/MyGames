using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LightBegin : MonoBehaviour
{
    SpriteRenderer blackSpr;
    public float speed = 0.5f;
    
    void Start ()
    {
        blackSpr = GetComponent<SpriteRenderer>();
    }
	
	void Update ()
    {
        if(blackSpr.color.a > 0.1)
            blackSpr.color = new Color(blackSpr.color.r, blackSpr.color.g, blackSpr.color.b, blackSpr.color.a - Time.deltaTime * speed);
    }
}
