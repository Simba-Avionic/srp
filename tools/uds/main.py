from doipclient import DoIPClient
import binascii
# ip = '192.168.10.101'
# logical_address = 0x000A
# client = DoIPClient(ip, logical_address,client_ip_address='192.168.10.4')
# client.send_diagnostic(bytearray.fromhex('221133'))
# print(binascii.hexlify(bytearray(client.receive_diagnostic())).decode("utf-8"))
def receive_loop(client):
    res = bytearray(client.receive_diagnostic())
    if(res[0] == 0x7f and res[2] == 0x78):
        print("    "+binascii.hexlify(res).decode("utf-8")+" -> Response pending ")
        receive_loop(client)
    else:
        print("    "+binascii.hexlify(res).decode("utf-8"))
        
def send(client,data):
    client.send_diagnostic(bytearray.fromhex(data))
    receive_loop(client)

def main():
    print("Znane adresy:")
    print("192.168.10.101 - EC")
    print("192.168.10.102 - FC")
    ip = input("Połącz się z ECU przez podanie adresu IP: ")
    if ip == "exit":
        exit(0)
    print("Łaczenie z: "+ip)
    try:
        client = DoIPClient(ip,0x0A)
        print("połączono")
        
        while True:
            data = input("> ")
            if(data == "exit"):
                break
            else:
                send(client,data)
    except:
        print("Comunication error !!!")
        main()
    
if (__name__ == "__main__"):
    main()