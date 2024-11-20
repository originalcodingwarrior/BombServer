using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class OtherPlayer : Player
{
    //서버로부터 데이터를 받아서 갱신.

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void UpdateData(Vector3 position, bool hasBomb)
    {
        this.SetPosition(position);
        this.SetPlayerHasBomb(hasBomb);
      
    }

}
