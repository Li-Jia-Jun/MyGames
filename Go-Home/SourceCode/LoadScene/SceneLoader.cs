using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class SceneLoader : MonoBehaviour
{
    public Text processText;
    public Text processTag;

    private AsyncOperation async;
    private uint _nowprocess;
    
    public int level = -1;
    public bool isReady = false;

    void Start()
    {
        _nowprocess = 0;

        if (level != -1)
            StartCoroutine(loadScene());
        else
            Debug.Log("level is -1!!!!");
    }

    IEnumerator loadScene()
    {
        async = SceneManager.LoadSceneAsync("Level_" + level);
        async.allowSceneActivation = false;
        yield return async;
    }

    void Update()
    {
        if (async == null)
        {
            return;
        }
        
        
        if(isReady)
        {
            if(Input.anyKeyDown)
            {
                async.allowSceneActivation = true;
            }
        }

        uint toProcess;
        if (async.progress < 0.9f)
        {
            toProcess = (uint)(async.progress * 100);
        }
        else
        {
            toProcess = 100;
            processTag.text = "按任意键进入游戏";
        }

        if (_nowprocess < toProcess)
        {
            _nowprocess++;

            if(_nowprocess > 95)
                isReady = true;
        }

        processText.text = _nowprocess.ToString() + "%";
    }
}
