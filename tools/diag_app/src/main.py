import sys
import glob
import threading
import time
from tools.diag_app.src.lib.db import DB
from tools.diag_app.src.lib.query import QueryMenager
from flask import Flask, send_from_directory
from flask_socketio import emit,send, SocketIO
from tools.diag_app.src.lib.uds_menager import UdsMenager, UdsClient
db_ = DB()
uds_menager = UdsMenager(DB())
app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app,cors_allowed_origins="*")

def UdsRx(data):
    print("RX: ")
    print(data)
    socketio.emit('console_rx',data)

@app.route('/<path:path>', methods=['GET'])
def static_proxy(path):
  return send_from_directory('tools/diag_app/front', path)


@app.route('/')
def root():
  return send_from_directory('tools/diag_app/front', 'index.html')

@socketio.on('connect_ecu')
def handle_my_custom_event(*args):
    print("connecting")
    res = {}
    res["status"] = True
    emit("status_ecu",res)
    emit("ecu_status_list",QueryMenager.GetEcuList(DB()))
    emit("console_rx","connecting")
    
    
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


@app.errorhandler(500)
def server_error(e):
  return 'An internal error occurred [main.py] %s' % e, 500
