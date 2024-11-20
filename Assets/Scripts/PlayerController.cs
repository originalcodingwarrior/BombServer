using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerController : MonoBehaviour
{
    public Player player;
    public bool canOpenDoor;
    public DoorSide currentDoor;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        if(canOpenDoor && Input.GetKeyDown(KeyCode.K))
        {
            StartCoroutine(OpenDoor());
        }

        float horizontal = Input.GetAxis("Horizontal"); //A,D 입력
        float vertical = Input.GetAxis("Vertical"); //W,S 입력

        if(horizontal != 0 || vertical != 0) {

            Vector3 moveDirection = new Vector3(horizontal, 0, vertical);

            transform.Translate(moveDirection * player.speed * Time.deltaTime);

            NetworkManager.Instance.SendData();

        }


        }

    void OnTriggerEnter(Collider other)
    {
        if (other.CompareTag("DoorSide"))
        {
            canOpenDoor = true;
            currentDoor = other.GetComponent<DoorSide>();
        }
    }

    void OnTriggerExit(Collider other)
    {
        if (other.CompareTag("DoorSide"))
        {
            canOpenDoor = false;
            currentDoor = null;
        }
    }

    IEnumerator OpenDoor()
    {
        yield return new WaitForSeconds(0.3f);

        transform.position = currentDoor.GetOppositeSidePosition();
    }
}
