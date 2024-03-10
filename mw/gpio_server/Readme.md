# GPIO MW Service

## Frame REQ
| name | size | desc |
| --- | --- | --- |
| service_id | uint16_t | client id |
| pin_id | uint16_t | nadane id konkretnego pinu |
| value | uint8_t | wartość do ustawienia pinu (0/1)(LOW/HIGH) |

## Frame RES
| name | size | desc |
| --- | --- | --- |
| pin_id | uint16_t | |
| value | uint8_t | |

## Config

{
    "pins":{
        <pin_id>:{
            "pin":33,
            "direction":"out",
        }
    }
}