using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DoorSide : MonoBehaviour
{
    public DoorSide oppositeSide;

    public Vector3 GetOppositeSidePosition()
    {
        return oppositeSide.transform.position;
    }
}
