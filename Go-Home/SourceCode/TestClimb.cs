using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TestClimb : MonoBehaviour {

    Animator animator;

    int count = 1;
    bool start = false;

	void Start ()
    {
		animator = GetComponent<Animator>();
	}
	

    void climbOffSet()
    {
        float scale = transform.localScale.x;

        transform.position = new Vector3(transform.position.x + 1.5f * scale, 
            transform.position.y + 3f * Mathf.Abs(scale), transform.position.z);    
    }

    void printCount()
    {
        start = false;
      
        Debug.Log("count = "+ count);

        count = 0;
    }

	void FixedUpdate ()
    {
        if(Input.GetKeyDown(KeyCode.G))
        {
            animator.SetTrigger("Climb");
            start = true;
        }

        if(start)
        {
            count++;
        }
	}
}
