using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class AplhaChanger : MonoBehaviour
{
    private Text text;

    void Start()
    {
        text = GetComponent<Text>();
    }

	void Update ()
    {
		if(text.color.a < 1)
        {
            text.color =  new Color(text.color.r, text.color.g, text.color.b, text.color.a + 0.005f);
        }
	}
}
