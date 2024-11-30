using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerController : MonoBehaviour
{

    // Start is called before the first frame update
    void Start()
    {
        
    }

    public void Explosion()
    {
        Rigidbody rb = GetComponent<Rigidbody>();

        rb.AddTorque(Vector3.back * 1500f, ForceMode.Impulse);
        rb.AddForce(Vector3.up * 10f, ForceMode.Impulse);
    }
}
