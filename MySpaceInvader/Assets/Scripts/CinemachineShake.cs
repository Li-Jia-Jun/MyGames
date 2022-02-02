using UnityEngine;
using Cinemachine;

public class CinemachineShake : MonoBehaviour
{
    public float intensity;
    public float time;

    private CinemachineBasicMultiChannelPerlin perlin;
      
    private float timer = 0;


    void Start()
    {
        perlin = GetComponent<CinemachineVirtualCamera>().GetCinemachineComponent<CinemachineBasicMultiChannelPerlin>();
    }

    void Update()
    {
        if (timer > 0)
        {
            perlin.m_AmplitudeGain = (timer / time) * intensity;
            timer -= Time.deltaTime;
        }
        else
        {
            if (perlin.m_AmplitudeGain > 0)
                perlin.m_AmplitudeGain = 0;
        }
    }

    public void Shake()
    {
        timer = time;
    }
}
