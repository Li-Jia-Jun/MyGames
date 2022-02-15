using System.Collections;
using System.Collections.Generic;
using UnityEngine.SceneManagement;
using UnityEngine;



public class FinishPoint : MonoBehaviour
{	
    public static int count = 0;    // Count of players arriving at finish point

    public GameController gameController;
    
    void OnTriggerEnter2D(Collider2D collider2D)
    {
        if(collider2D.tag == "Player")
        {
            count++;
            if(count == 2)
            {
                count = 0;
                //gameController.GoNextScene();   // End game only if both players arrive at finish point

                //跳回选关
                SceneManager.LoadScene("jumpLevel", LoadSceneMode.Single);
            }
        }
    }

    void OnTriggerExit2D(Collider2D collider2D)
    {
        if (collider2D.tag == "Player")
        {
            count--;
        }
    }

    void Update()
    {
    }
}
