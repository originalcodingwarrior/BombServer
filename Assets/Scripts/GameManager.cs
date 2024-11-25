using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class GameManager : MonoBehaviour
{
    public static GameManager Instance;

    public GameObject bomb;

    public Vector3[] bombPosition;
    /* 0번이 로컬플레이어 자리
           2
        3     1
           0
      이렇게 반시계방향으로 돌아감 */

    public Button passButton;
    public bool isTimerRunning;

    private void Awake()
    {
        if (Instance == null)
        {
            Instance = this;
        }
        else
        {
            Destroy(gameObject);
        }
    }

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    void GameEnd()
    {
        bomb.GetComponent<ParticleSystem>().Play();
        Debug.Log("펑~");
    }

    public void UpdateGameState(int bomb_owner, float timer)
    {
        if(timer <= 0)
        {
            GameEnd();
        }

        bomb.transform.position = bombPosition[bomb_owner];

        if (bomb_owner == 0) //본인 차례였으면 타이머 시작
        {
            passButton.interactable = true;
            StartCoroutine(StartTimer(timer));
        }
        else
        {
            passButton.interactable = false;
        }
    }

    IEnumerator StartTimer(float timer)
    {
        isTimerRunning = true;
        while(timer > 0 && isTimerRunning)
        {
            timer -= Time.deltaTime;
            yield return null;
        }

        NetworkManager.Instance.SendData(timer);
    }

    public void StopTimer() //버튼 누르면 실행
    {
        isTimerRunning = false;
    }
}
