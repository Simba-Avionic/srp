router_schema = {
    "routers": {
        "type": "list",
        "schema": {
            "type": "dict",
            "schema": {
                "name": {"type": "string", "required": True},
                "interfaces": {
                    "type": "list",
                    "schema": {"type": "string", "allowed": ["app1", "app2", "app3"]}
                },
                "interface": {"type": "string", "allowed": ["rut1", "rut2"]}
            }
        }
    }
}


interface_schema ={
    "interfaces": {
        "type": "list",
        "schema": {
            "type": "dict",
            "schema": {
                "name": {"type": "string", "required": True},
                "ip": {"type": "string", "required": True},
                "port": {"type": "integer", "required": True, "min": 0, "max": 99999}
            }
        }
    }
}
services_schema={
    "services": {
        "type": "list",
        "schema": {
            "type": "dict",
            "schema": {
                "name": {"type": "string", "required": True},
                "service_id": {"type": "integer", "required": True},
                "interface": {"type": "string", "required": True},
                "pub_methods": {
                    "type": "list",
                    "schema": {
                        "type": "dict",
                        "schema": {
                            "method_id": {"type": "integer", "required": True},
                            "name": {"type": "string", "required": True}
                        }
                    }
                },
                "pub_events": {
                    "type": "list",
                    "schema": {
                        "type": "dict",
                        "schema": {
                            "event_id": {"type": "integer", "required": True},
                            "name": {"type": "string", "required": True}
                        }
                    }
                },
                "req_methods": {
                    "type": "list",
                    "schema": {"type": "string"}
                },
                "req_events": {
                    "type": "list",
                    "schema": {"type": "string"}
                },
                "conf": {
                    "type": "dict"
                }
            }
        }
    }
}