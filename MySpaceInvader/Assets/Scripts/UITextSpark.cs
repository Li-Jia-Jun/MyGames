using UnityEngine;
using UnityEngine.UI;

public class UITextSpark : MonoBehaviour
{
    [SerializeField] private float speed;

    private Text text;


    void Start()
    {
        text = GetComponent<Text>();
    }

    void Update()
    {
        text.color = new Color(text.color.r, text.color.g, text.color.b, (Mathf.Sin(Time.time * speed) + 1) * 0.5f);
    }
}
