using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class GameManager : MonoBehaviour
{
    public static GameManager Instance;

    public GameObject bomb;
    public GameObject loadingImage;
    public GameObject gameOverText;

    public ParticleSystem explosion;

    public Vector3[] bombPosition;
    /* 0���� �����÷��̾� �ڸ�
           2
        3     1
           0
      �̷��� �ݽð�������� ���ư� */
    private int bombOwner;

    public Button passButton;
    public GameObject replayButton;
    private bool isTimerRunning;

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
        NetworkManager.Instance.OnGameReady += GameIsReady;
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    void GameEnd()
    {
        explosion.transform.position = bombPosition[bombOwner];
        explosion.Play();
        Debug.Log("��~");
        Destroy(bomb);

        GameObject Loser = GameObject.Find("Player " + bombOwner);
        Loser.GetComponent<PlayerController>().Explosion();

        if(bombOwner == 0)
        {
            gameOverText.SetActive(true);
        }

        replayButton.SetActive(true);

    }

    public void UpdateGameState(int bomb_owner, float timer)
    {
        if(timer <= 0)
        {
            GameEnd();
        }

        bombOwner = bomb_owner;
        bomb.transform.position = bombPosition[bombOwner];

        if (bombOwner == 0) //���� ���ʿ����� Ÿ�̸� ����
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

    public void StopTimer() //��ư ������ ����
    {
        isTimerRunning = false;
    }

    private void GameIsReady()
    {
        loadingImage.SetActive(false);
        Debug.Log("��� Ŭ���̾�Ʈ ���� �Ϸ�. ���� ����");

        NetworkManager.Instance.OnGameReady -= GameIsReady;
    }
}
