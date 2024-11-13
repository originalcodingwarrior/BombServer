using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player : MonoBehaviour
{
    private bool hasBomb = false;
    public float speed = 7f;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public bool HasBomb()
    {
        return hasBomb;
    }

    protected void SetPlayerHasBomb(bool hasBomb)
    {
        this.hasBomb = hasBomb;

        if(hasBomb) //폭탄 있음
        {
            GetComponent<Renderer>().material.color = Color.red;
            speed = 8f;
        }
        else //폭탄 없음
        {
            GetComponent<Renderer>().material.color = Color.white;
            speed = 7f;
        }
    }

}
