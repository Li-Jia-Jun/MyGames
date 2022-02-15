using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class StartGame : MonoBehaviour
{
    public GameController gameController;

    public Text text;

    public float timer = 0;

	void Start ()
    {
        text = GetComponent<Text>();
    }
	
	void Update ()
    {
        if(Input.GetKeyDown(KeyCode.Space))
        {            
            gameController.GoNextScene();
        }

        timer += Time.deltaTime;
        text.color =  new Color(text.color.r, text.color.g, text.color.b, 0.5f * (Mathf.Sin(timer)+1));
    }
}
