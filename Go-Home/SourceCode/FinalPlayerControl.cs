using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FinalPlayerControl : MonoBehaviour {

	public float player_Speed;
	public float player_SpeedAdd;
	public float player_SpeedSub;
	public float player_MaxSpeed;

	public float playerHorDir;

	public bool is_EndMoving;

	public Vector3 velocity;

	public AudioClip sound_walk;
	public AudioClip sound_fall;

	public AudioSource audio_walk;
	public AudioSource audio_fall;

	public GameObject position_EndMoving;

    Animator animator;

	// Use this for initialization
	void Start () {
		is_EndMoving = false;

		player_Speed = 0.0f;

        animator = GetComponent<Animator>();

		player_MaxSpeed = 1.5f;
		player_SpeedAdd = 4.0f;
		player_SpeedSub = 4.0f;

		audio_walk = this.gameObject.AddComponent<AudioSource> ();
		audio_fall = this.gameObject.AddComponent<AudioSource> ();

		audio_fall.clip = this.sound_fall;
		audio_walk.clip = this.sound_walk;
	}
	
	// Update is called once per frame
	void Update () {
		if (is_EndMoving) {

			if(audio_walk.isPlaying == true)
				audio_walk.Stop ();

			if (Mathf.RoundToInt(player_Speed) == 0) {
				player_Speed = 0;
			}

			if (player_Speed > 0) {
				player_Speed -= player_SpeedSub * Time.deltaTime;
			} else if (player_Speed < 0) {
				player_Speed += player_SpeedSub * Time.deltaTime;
			}

            animator.SetBool("isWalking", false);

            return;
		}

		playerHorDir = Input.GetAxisRaw ("Horizontal");

		switch ((int)playerHorDir) {
		case -1:
			{
                    if (transform.localScale.x > 0)
                    {
                        transform.localScale = new Vector3(-transform.localScale.x, transform.localScale.y, transform.localScale.z);
                    }
                    animator.SetBool("isWalking", true);

                    player_Speed -= player_SpeedAdd * Time.deltaTime;
				player_Speed = Mathf.Clamp (player_Speed, -player_MaxSpeed, player_MaxSpeed);


				if(audio_walk.isPlaying == false)
					audio_walk.Play ();

				break;
			}
		case 0:
			{

                    animator.SetBool("isWalking", false);

                    if (Mathf.RoundToInt(player_Speed) == 0) {
					player_Speed = 0;
					break;
				}

				if (player_Speed > 0) {
					player_Speed -= player_SpeedSub * Time.deltaTime;
				} else if (player_Speed < 0) {
					player_Speed += player_SpeedSub * Time.deltaTime;
				}

				if(audio_walk.isPlaying == true)
					audio_walk.Stop ();

				break;
			}
		case 1:
			{
                    if (transform.localScale.x < 0)
                    {
                        transform.localScale = new Vector3(-transform.localScale.x, transform.localScale.y, transform.localScale.z);
                    }
                    animator.SetBool("isWalking", true);

                    player_Speed += player_SpeedAdd * Time.deltaTime;
				player_Speed = Mathf.Clamp (player_Speed, -player_MaxSpeed, player_MaxSpeed);

				if(audio_walk.isPlaying == false)
					audio_walk.Play ();

				break;
			}
		}

		this.transform.Translate (player_Speed * Time.deltaTime,0,0);
	}

	void OnCollisionEnter(Collision collision){
		if (collision.gameObject.tag == "Floor"){
			this.audio_fall.Play ();
		}
	}

	void OnTriggerEnter2D(Collider2D collider){
       
        

		if (collider.gameObject == this.position_EndMoving) {
			is_EndMoving = true;

            collider.gameObject.GetComponent<LightOut>().startToFade = true;
        }
	}
}
