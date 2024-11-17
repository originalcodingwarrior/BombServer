using System;
using System.Net;
using System.Net.Sockets;
using System.IO;
using UnityEngine;
using System.Collections.Generic;

public class NetworkManager : MonoBehaviour
{
    private Socket clientSocket;
    private byte[] buffer = new byte[16];
    public Player player;

    public static NetworkManager Instance;

    private void Awake()
    {
        if(Instance == null)
        {
            Instance = this;

            DontDestroyOnLoad(this.gameObject);
        }
        else
        {
            Destroy(this.gameObject);
        }
    }

    // Start is called before the first frame update
    void Start()
    {
        ConnectToServer("117.16.44.109", 8080);
    }

    // Update is called once per frame
    void Update()
    {

    }

    void OnApplicationQuit()
    {
        if (IsSocketConnected())
            clientSocket.Close();
    }

    void ConnectToServer(string ip, int port)
    {
        try
        {
            clientSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            clientSocket.Connect(new IPEndPoint(IPAddress.Parse(ip), port));
            
            Debug.Log("서버와 연결");

        }
        catch (Exception e)
        {
            Debug.Log("서버와 연결 실패 :" + e.Message);
        }
    }

    public void SendHasBombStatus(bool hasBomb)
    {
        if (!IsSocketConnected()) return;

        try
        {
            byte[] data = new byte[1];
            data[0] = hasBomb ? (byte)1 : (byte)0;

            clientSocket.Send(data);

        }
        catch (Exception e)
        {
            Debug.Log("데이터 전송 실패 :" + e.Message);
        }
    }
    public void SendPlayerPosition(Vector3 vector3)
    {
        if (!IsSocketConnected()) return;

        try
        {
            List<byte> dataList = new List<byte>();

            dataList.AddRange(BitConverter.GetBytes(vector3.x));
            dataList.AddRange(BitConverter.GetBytes(vector3.y));
            dataList.AddRange(BitConverter.GetBytes(vector3.z));

            byte[] data = dataList.ToArray();
            clientSocket.Send(data);

        }
        catch (Exception e)
        {
            Debug.Log("데이터 전송 실패 :" + e.Message);
        }
    }

    bool IsSocketConnected()
    {
        return clientSocket != null && clientSocket.Connected;
    }
}
