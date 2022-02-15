using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class PlayerControl : MonoBehaviour {


    public GameObject actionTag;
    public GameObject startTag;
    public GameObject nullTag;

    public AudioClip sound_walk;
    public AudioClip sound_fall;
    public AudioClip sound_breath;

    public AudioSource audio_walk;
    public AudioSource audio_fall;
    public AudioSource audio_breath;

    public bool operationNum;
    public bool can_ClimpUP;
    public bool is_Climping;
    public int climpUp_Frames;

    public float player_Speed;
	public float player_SpeedAdd;
	public float player_SpeedSub;
	public float player_MaxSpeed;

	public float playerHorDir;
	public float playerVerDir;

	public bool can_PlayerMove;
	public bool is_StartSavingActionData;
	public bool is_StartImitate;

	public GameObject subPlayer;
	public Rigidbody playerRigidbody;
	public Vector3 velocity;

    public Animator animator;

	public string subPlayer_Name = "son";

	public enum STEP {
		none = -2,
		moveLeft = -1,
		stand = 0,
		moveRight,
		climb
	};

	public enum ACTIONSTATE{
		none = -1,
		start = 0,
		action
	}

	public STEP step = STEP.stand;
	public STEP laststep = STEP.none;

	public ACTIONSTATE actionState = ACTIONSTATE.none;   

	//ActionData ------------------------------------------------------------

	Queue<ACTIONPAIR> ActionQueue;

	public struct ACTIONPAIR
	{
		public int key_frame;
		public char value_action;
	};

	ACTIONPAIR actionData = new ACTIONPAIR();
	//ActionData End------------------------------------------------------------

	// Use this for initialization
	void Start () {
        operationNum = false;

        nullTag.SetActive(true);
        startTag.SetActive(false);
        actionTag.SetActive(false);

        audio_breath = this.gameObject.AddComponent<AudioSource>();
        audio_walk = this.gameObject.AddComponent<AudioSource>();
        audio_fall = this.gameObject.AddComponent<AudioSource>();

        audio_breath.clip = this.sound_breath;
        audio_fall.clip = this.sound_fall;
        audio_walk.clip = this.sound_walk;

        climpUp_Frames = 60;

        animator = GetComponent<Animator>();

		ActionQueue = new Queue<ACTIONPAIR> ();
		actionData.key_frame = 1;
		actionData.value_action = 'S';

		can_PlayerMove = false;
		is_StartImitate = false;
		is_StartSavingActionData = false;

		player_Speed = 0.0f;

		player_MaxSpeed = 1.5f;
		player_SpeedAdd = 4.0f;
		player_SpeedSub = 4.0f;

		subPlayer = GameObject.Find (subPlayer_Name);
		playerRigidbody = this.GetComponent<Rigidbody> ();

        animator.SetBool("isWalking", false);
    }
	
	// Update is called once per frame
	void FixedUpdate () {

        if (Input.GetKeyDown(KeyCode.P))
            SceneManager.LoadScene("jumpLevel", LoadSceneMode.Single);


        if (is_StartImitate) {
			return;
		}

        if (Input.GetKeyDown(KeyCode.W))
        {

            StartClimping();

        }

        if (is_Climping)
        {
            if (climpUp_Frames-- <= 0)
            {
                is_Climping = false;
            }
            else
            {
                return;
            }
        }

        if (can_PlayerMove) {
			playerHorDir = Input.GetAxisRaw ("Horizontal");

           


            switch ((int)playerHorDir) {
			case -1:
				{
					step = STEP.moveLeft;
                        if(transform.localScale.x > 0)
                        {
                            transform.localScale = new Vector3(-transform.localScale.x, transform.localScale.y, transform.localScale.z);
                        }            
                        animator.SetBool("isWalking", true);


                    player_Speed -= player_SpeedAdd * Time.deltaTime;
					player_Speed = Mathf.Clamp (player_Speed, -player_MaxSpeed, player_MaxSpeed);

					if (laststep != step) {
						laststep = step;
						if(is_StartSavingActionData)
							ActionEnqueue ();
					}

                        if (audio_walk.isPlaying == false)
                            audio_walk.Play();

                        actionData.value_action = 'L';

					break;
				}
			case 0:
				{
					step = STEP.stand;
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

					if (laststep != step) {
						laststep = step;
						if(is_StartSavingActionData)
							ActionEnqueue ();
					}

                        if (audio_walk.isPlaying == true)
                            audio_walk.Stop();

                        actionData.value_action = 'S';

					break;
				}
			case 1:
				{
					step = STEP.moveRight;
                        if (transform.localScale.x < 0)
                        {
                            transform.localScale = new Vector3(-transform.localScale.x, transform.localScale.y, transform.localScale.z);
                        }
                        animator.SetBool("isWalking", true);

					player_Speed += player_SpeedAdd * Time.deltaTime;
					player_Speed = Mathf.Clamp (player_Speed, -player_MaxSpeed, player_MaxSpeed);

					if (laststep != step) {
						laststep = step;
						if(is_StartSavingActionData)
							ActionEnqueue ();
					}

                        if (audio_walk.isPlaying == false)
                            audio_walk.Play();

                        actionData.value_action = 'R';

					break;
				}
			}

			if(is_StartSavingActionData)
				actionData.key_frame++;

			this.transform.Translate (player_Speed * Time.deltaTime,0,0);
		}


        if (Input.GetKeyDown(KeyCode.Space))
        {

            // 1 --> move 0 --> stop

            operationNum = !operationNum;

            switch (operationNum)
            {
                case false:
                    {
                        if (actionState == ACTIONSTATE.start)
                        {
                            //velocity forced reset
                            player_Speed = 0;

                            animator.SetBool("isWalking", false);

                            //Audio forced reset
                            if (audio_walk.isPlaying == true)
                                audio_walk.Stop();

                            can_PlayerMove = false;

                            is_StartSavingActionData = false;
                            ActionEnqueue();

                            nullTag.SetActive(false);
                            startTag.SetActive(false);
                            actionTag.SetActive(true);

                            is_StartImitate = true;
                            subPlayer.GetComponent<SubPlayerControl>().ImitateAction();

                            actionState = ACTIONSTATE.action;
                        }
                        break;
                    }
                case true:
                    {
                        if (actionState != ACTIONSTATE.start)
                        {
                            can_PlayerMove = true;

                            actionState = ACTIONSTATE.start;

                            //改成Start
                            nullTag.SetActive(false);
                            startTag.SetActive(true);
                            actionTag.SetActive(false);

                            is_StartSavingActionData = true;
                        }
                        break;
                    }
            }

        }
    }

	void ActionEnqueue(){
		ActionQueue.Enqueue (actionData);

		//Debug.Log (actionData.value_action + "------>" + actionData.key_frame);

		actionData.key_frame = 1;
	}

	public int ReturnActionData(ref int key,ref char value){

		if (ActionQueue.Count == 0) {
			return 0;
		}
			
		ACTIONPAIR tempActionPair = ActionQueue.Dequeue ();

		key = tempActionPair.key_frame;
		value = tempActionPair.value_action;
		return 1;
	}

	public void StopImitate(){
		ActionQueue.Clear ();

		actionState = ACTIONSTATE.none;
		actionData.key_frame = 1;
		actionData.value_action = 'S';

        nullTag.SetActive(true);
        startTag.SetActive(false);
        actionTag.SetActive(false);

        is_StartImitate = false;
	}





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

    void StartClimping()
    {
        climpUp_Frames = 100;

        if (can_ClimpUP)
        {
            //Animator
            animator.SetTrigger("Climb");
        }

        //Son
        subPlayer.GetComponent<SubPlayerControl>().SubPlayerStartClimping(can_ClimpUP);

        is_Climping = true;

    }
}
