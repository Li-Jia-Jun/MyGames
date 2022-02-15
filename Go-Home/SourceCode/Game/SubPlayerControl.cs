using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SubPlayerControl : MonoBehaviour {

    public AudioClip sound_walk;
    public AudioClip sound_fall;

    public AudioSource audio_walk;
    public AudioSource audio_fall;

    public Animator animator;

	public float player_Speed;
	public float player_SpeedAdd;
	public float player_SpeedSub;
	public float player_MaxSpeed;

	public int imitateFrame;
	public int targetFrame;

	public char imitateAction;

	public bool is_StartImitate;

	public string mainPlayer_Name = "father";

	public GameObject mainPlayer;

	// Use this for initialization
	void Start () {

        this.audio_walk = this.gameObject.AddComponent<AudioSource>();
        this.audio_fall = this.gameObject.AddComponent<AudioSource>();

        this.audio_walk.clip = sound_walk;
        this.audio_fall.clip = sound_fall;

        animator = GetComponent<Animator>();

		mainPlayer = GameObject.Find (mainPlayer_Name);

		player_Speed = 0.0f;

		player_MaxSpeed = 1.5f;
		player_SpeedAdd = 4.0f;
		player_SpeedSub = 4.0f;

		is_StartImitate = false;
		imitateFrame = 1;
	}
	
	// Update is called once per frame
	void FixedUpdate () {
		if (is_StartImitate) {
			if (imitateFrame >= targetFrame) {

				//Get Action Data
				if (mainPlayer.GetComponent<PlayerControl> ().ReturnActionData (ref targetFrame, ref imitateAction) == 1) {
					imitateFrame = 1;

				//Debug.Log (targetFrame + "//////" + imitateAction);

				} else {
					mainPlayer.GetComponent<PlayerControl> ().StopImitate ();

					//velocity forced reset
					player_Speed = 0;

                    if(this.audio_walk.isPlaying == true){
                        audio_walk.Stop();
                    }

                    animator.SetBool("isWalking", false);

                    is_StartImitate = false;
					return;
				}

			} else {
				switch (imitateAction) {
				case 'S':
					{
                            animator.SetBool("isWalking", false);

                            if (Mathf.RoundToInt (player_Speed) == 0) {
							player_Speed = 0;

                                if (audio_walk.isPlaying == true)
                                    audio_walk.Stop();

                                break;
						}
				
						if (player_Speed > 0) {
							player_Speed -= player_SpeedSub * Time.deltaTime;
						} else if (player_Speed < 0) {
							player_Speed += player_SpeedSub * Time.deltaTime;
						}

						break;
					}
				case 'R':
					{
                            if (transform.localScale.x < 0)
                            {
                                transform.localScale = new Vector3(-transform.localScale.x, transform.localScale.y, transform.localScale.z);
                            }
                            animator.SetBool("isWalking", true);
                            player_Speed += player_SpeedAdd * Time.deltaTime;
						player_Speed = Mathf.Clamp (player_Speed, -player_MaxSpeed, player_MaxSpeed);

                            if (audio_walk.isPlaying == false)
                                audio_walk.Play();

                            break;
					}
				case 'L':
					{
                            if (transform.localScale.x > 0)
                            {
                                transform.localScale = new Vector3(-transform.localScale.x, transform.localScale.y, transform.localScale.z);
                            }
                            animator.SetBool("isWalking", true);
                            player_Speed -= player_SpeedAdd * Time.deltaTime;
						player_Speed = Mathf.Clamp (player_Speed, -player_MaxSpeed, player_MaxSpeed);

                            if (audio_walk.isPlaying == false)
                                audio_walk.Play();

                            break;
					}
				}
			}
				
			imitateFrame++;
			this.transform.Translate (player_Speed * Time.deltaTime, 0, 0);

		} else {
			return;
		}
	}

	public void ImitateAction(){
		is_StartImitate = true;
	}

    public bool can_ClimpUP = false;

    void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.gameObject.tag == "Wall")
        {
            can_ClimpUP = true;
        }
        else if (collision.gameObject.tag == "Floor")
        {
            this.audio_fall.Play();
        }
    }

    void OnTriggerExit2D(Collider2D collision)
    {
        if (collision.gameObject.tag == "Wall")
        {
            can_ClimpUP = false;
        }
    }

    public void SubPlayerStartClimping(bool can_FatherClimpUp)
    {
        if (can_ClimpUP == false)
        {
            return;
        }
        else if(can_ClimpUP && can_FatherClimpUp)
        {

            //Animator
            animator.SetTrigger("Climb");
            //*********************
        }
    }


}
