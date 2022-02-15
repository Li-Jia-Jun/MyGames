using System.Collections;
using System.Collections.Generic;
using UnityEngine.SceneManagement;
using UnityEngine;

public class LightOut : MonoBehaviour
{
    public SpriteRenderer blackSpr;
    SpriteRenderer lightSpr;
    

    public float speed = 0.5f;

    public bool startToFade = false;
    

	void Start ()
    {
		lightSpr = GetComponent<SpriteRenderer>();
	}
	
	void Update ()
    {
		if(startToFade)
        {
            lightSpr.color = new Color(lightSpr.color.r, lightSpr.color.g, lightSpr.color.b, lightSpr.color.a - Time.deltaTime * speed);
            blackSpr.color = new Color(blackSpr.color.r, blackSpr.color.g, blackSpr.color.b, blackSpr.color.a + Time.deltaTime * speed);
        }

        if(blackSpr.color.a >= 0.9f)
        {
            SceneManager.LoadScene("Finish");
        }
	}

}
