using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class GameController : MonoBehaviour
{
    private static int nextLoadScene = 0;
    private const int maxScene = 5;


	void Start ()
    {
		
	}

    public void GoNextScene()
    {
        nextLoadScene++;

        if (nextLoadScene > maxScene)
        {
            // End scene
        }
        else
        {
            // Next Level
            SceneManager.LoadScene("Level_" + nextLoadScene + "_load");
        }
    }

    public void GoScene(int secne)
    {
        if (nextLoadScene < maxScene)
        {
            SceneManager.LoadScene("Level_" + secne + "_load");
        }
    }



    void Update ()
    {
		if(Input.GetKeyDown(KeyCode.P))
        {
            SceneManager.LoadScene("jumpLevel");
        }
	}
}
