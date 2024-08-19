import sys
import glob
import threading
import time
import binascii
from tools.diag_app.src.lib.db import DB
from tools.diag_app.src.lib.query import QueryMenager
from flask import Flask, send_from_directory
from flask_socketio import emit,send, SocketIO
from tools.diag_app.src.lib.uds_menager import UdsMenager, UdsClient, UdsDecoder
db_ = DB()
uds_menager = UdsMenager(DB())
app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app,cors_allowed_origins="*")

def UdsRx(data):
    out = {}
    out["ecu_id"] = data["ecu_id"]
    res = data["res"]
    if(res[0] == 0x7f and res[2] == 0x78):
        out["req"] = ""+binascii.hexlify(res).decode("utf-8")+" -> Response pending "
    else:
        out["req"]= ""+binascii.hexlify(res).decode("utf-8")
    print(out)
    socketio.emit('console_rx',out)
    
def NewDtcStatus(data):
    res = data["res"]
    if(res[0] == 0x7f and res[2] != 0x78):
        print("Error !")
        return
    elif res[0] == 0x59:
        dtc_list = UdsDecoder.DecodeDtcList(res)
        for dtc_uds in dtc_list:
            db_.UpdateDtcStatus(str(data["ecu_id"]),int(dtc_uds["id"]),dtc_uds["status"])
        socketio.emit('dtc_status_update',QueryMenager.GetDtcList(db_))
def ClearStatus(data):
    out = {}
    out["ecu_id"] = data["ecu_id"]
    out["desc"] = "DTC cleaning"
    res = data["res"]
    if(res[0] == 0x7f and res[2] == 0x78):
        return
    elif res[0] == 0x54:
        out["status"] = "Ok"
    else:
        out["status"] = "Error: "+binascii.hexlify(res).decode("utf-8")
    
    socketio.emit('alert_all',out)
    
@app.route('/<path:path>', methods=['GET'])
def static_proxy(path):
  print(path)
  return send_from_directory('../front', path)


@app.route('/')
def root():
  return send_from_directory('../front', 'index.html')

@socketio.on('connect_ecu')
def handle_my_custom_event(*args):
    print("connecting")
    res = {}
    res["status"] = True
    emit("status_ecu",res)
    emit("ecu_status_list",QueryMenager.GetEcuList(DB()))
    emit("console_rx","connecting")

@socketio.on('dtc_clear_all')
def dtc_status(data):
    uds_menager.SendRequest(str(data["ecu_id"]),"14FFFFFFFF",ClearStatus)

@socketio.on('dtc_status')
def dtc_status(data):
    print("Dtc_status: ")
    print(data)
    uds_menager.SendRequest(str(data["ecu_id"]),"190A",NewDtcStatus)
    
@socketio.on('uds_tx')
def TransmitUDS(data):
    uds_menager.SendRequest(data["ecu_id"],data["req"],UdsRx)
    emit("console_tx",data)
    
@socketio.on('update')
def Update(data):
    emit("ecu_status_list",QueryMenager.GetEcuList(DB()))

@socketio.on('connect')
def test_connect(auth):
    emit("ecu_status_list",QueryMenager.GetEcuList(DB()))

@app.errorhandler(500)
def server_error(e):
  return 'An internal error occurred [main.py] %s' % e, 500
@app.errorhandler(404)
def server_error(e):
  return send_from_directory('../front', 'index.html')


if __name__ == '__main__':

    for i in range(1,len(sys.argv)):
        sys.argv[i] = sys.argv[i].replace("//","")
        sys.argv[i] = sys.argv[i].split(":")[0]

    for i in range(1,len(sys.argv)):
        for path in glob.glob(sys.argv[i]+"/*.json"):
            db_.load(path)
    for ecu in db_.ecu_list:
        uds_menager.AddClient(UdsClient(ecu,DB()))
    socketio.run(app,port=8080)
    # app.run(host='127.0.0.1', port=8080, debug=True)



