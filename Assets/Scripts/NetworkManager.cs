using System;
using System.Net.Sockets;
using System.Threading.Tasks;
using UnityEngine;

public class NetworkManager : MonoBehaviour
{
    public static NetworkManager Instance;

    TcpClient client;
    NetworkStream stream;

    public Player player;
    public OtherPlayer[] otherPlayer;

    public const int MAX_CLIENTS = 6;

    private void Awake()
    {
        if(Instance == null)
        {
            Instance = this;
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

    public void SendData()
    {
        try
        {
            float x = player.transform.position.x;
            float y = player.transform.position.y;
            float z = player.transform.position.z;

            byte[] buffer = new byte[13];
            System.Buffer.BlockCopy(new float[] { x, y, z }, 0, buffer, 0, 12);
            buffer[12] = (byte)(player.HasBomb() ? 1 : 0);

            stream.Write(buffer, 0, buffer.Length);
        }
        catch(Exception e) 
        {
            Debug.LogError("데이터 전송 실패 " + e.Message);
        }
        
    }

    private async void ReceiveData()
    {
        byte[] buffer = new byte[17];
        try
        {
            int bytesRead = await stream.ReadAsync(buffer, 0, buffer.Length);

            if (bytesRead > 0)
            {
                ProcessData(buffer);
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
        int clientID = BitConverter.ToInt32(buffer, 0);
        float x = BitConverter.ToSingle(buffer, 4);
        float y = BitConverter.ToSingle(buffer, 8);
        float z = BitConverter.ToSingle(buffer, 12);
        bool hasBomb = BitConverter.ToBoolean(buffer, 16);

        Debug.Log($"ID : {clientID}, Position: ({x}, {y}, {z}), hasBomb: {hasBomb}");

        UpdateOtherPlayer(clientID, new Vector3(x,y,z), hasBomb);
    }

    void UpdateOtherPlayer(int id, Vector3 position, bool hasBomb)
    {
        if(otherPlayer[id - 1] != null)
        {
            otherPlayer[id - 1].UpdateData(position, hasBomb);
        }
        else
        {
            Debug.LogWarning(id + "번 플레이어가 없음");
        }
    }


    private void OnApplicationQuit()
    {
        stream.Close();
        client.Close();
    }
}
