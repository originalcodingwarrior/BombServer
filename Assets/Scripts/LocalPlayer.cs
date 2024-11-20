using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LocalPlayer : Player
{
    //서버에 자신의 데이터를 전달.

    // Start is called before the first frame update
    void Start()
    {
        SetPlayerHasBomb(true);
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    void OnTriggerEnter(Collider collider) //다른 플레이어와 충돌시
    {
        OtherPlayer other = collider.gameObject.GetComponent<OtherPlayer>();
        //충돌한 플레이어의 OtherPlayer 클래스 받아오기

        if(other == null) {
            return;
        }

        if (this.HasBomb()) //나한테 폭탄 있었음
        {
            Debug.Log("우하핫");
            SetPlayerHasBomb(false); //폭탄 사라짐
        }
        else if (other.HasBomb()) //상대한테 폭탄 있었음
        {
            Debug.Log("이런젠장");
            SetPlayerHasBomb(true); //폭탄 생김
        }

        NetworkManager.Instance.SendData();
    }
}
