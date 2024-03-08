# GPIO MW Service

## Frame
| name | size | desc |
| --- | --- | --- |
| service_id | uint16_t | client id |
| pin_id | uint16_t | nadane id konkretnego pinu |
| value | uint8_t | wartość do ustawienia pinu (0/1)(LOW/HIGH) |


## Config

{
    "pins":{
        <pin_id>:{
            "pin":33,
            "direction":"out",
        }
    }
}