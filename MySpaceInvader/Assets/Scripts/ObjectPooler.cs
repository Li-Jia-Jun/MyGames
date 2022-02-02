using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ObjectPooler : MonoBehaviour
{
    [SerializeField] private GameObject prefab;

    [SerializeField] private int maxNum;
    private List<GameObject> pool;
   

    void Start()
    {
        gameObject.transform.position = new Vector3(0, 0, 0);

        pool = new List<GameObject>();
        for (int i = 0; i < maxNum; i++)
        {
            GameObject go = Instantiate<GameObject>(prefab, gameObject.transform);
            go.SetActive(false);
            pool.Add(go);
        }
    }

    public GameObject getObject()
    {
        if (pool.Count > 0)
        {
            GameObject go = pool[pool.Count - 1];
            go.SetActive(true);
            pool.RemoveAt(pool.Count - 1);
            return go;
        }
        else
        {
            Debug.LogError("Try to get objects from empty pool:" + gameObject.name);
            return null;
        }    
    }

    public void backToPool(GameObject go)
    {
        if (pool.Count < maxNum)
        {
            pool.Add(go);
            go.SetActive(false);
        }
        else 
        {
            Debug.LogError("Try to add too many objects to pool:" + gameObject.name);
        }
    }
}
