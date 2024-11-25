using System;
using System.Net.Sockets;
using System.Threading.Tasks;
using UnityEngine;

public class NetworkManager : MonoBehaviour
{
    public static NetworkManager Instance;

    TcpClient client;
    NetworkStream stream;

    public const int MAX_CLIENTS = 4;

    private void Awake()
    {
        if(Instance == null)
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
        ConnectToServer("117.16.44.109", 8080);
        ReceiveData();
    }

    void ConnectToServer(string ip, int port)
    {
        try
        {
            client = new TcpClient(ip, port);
            stream = client.GetStream();
            Debug.Log("서버에 연결됨 ");
        }
        catch (Exception e)
        {
            Debug.LogError("서버 연결 실패 " + e.Message);

        }
    }

    public void SendData(float timer)
    {
        //GameManager측에서 timer 전달
        try
        {
            byte[] data = BitConverter.GetBytes(timer);
            stream.Write(data, 0, data.Length);
        }
        catch (Exception e)
        {
            Debug.Log("데이터 전송 실패 " + e.Message);
        }

    }

    private async void ReceiveData()
    {
        byte[] data = new byte[12];
        try
        {
            int bytesRead = await stream.ReadAsync(data, 0, data.Length);

            if (bytesRead > 0)
            {
                ProcessData(data);
                ReceiveData();
            }
        }
        catch (Exception e)
        {
            Debug.Log("데이터 수신 실패 " + e.Message);
        }
    }

    //데이터 직렬화
    private void ProcessData(byte[] buffer)
    {
        int bomb_owner = BitConverter.ToInt32(buffer, 0);
        float timer = BitConverter.ToSingle(buffer, 4);

        Debug.Log($"현재 턴 : {bomb_owner}, 남은 카운트 : {timer}");

        GameManager.Instance.UpdateGameState(bomb_owner, timer);
    }


    private void OnApplicationQuit()
    {
        stream.Close();
        client.Close();
    }
}
