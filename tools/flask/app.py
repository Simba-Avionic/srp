from flask import Flask, jsonify, request
import socket
import json

app = Flask(__name__)

actions = []
requirements = {}




@app.route('/actions', methods=['GET', 'DELETE'])
def home():
    if request.method == 'GET':
        return jsonify(actions)  # Zwraca listę actions w formacie JSON

    elif request.method == 'DELETE':
        actions.clear()
        return jsonify({"message": "All actions deleted"}), 200


@app.route('/hardware/gpio', methods=['POST'])
def post_data():
    # Sprawdzanie, czy dane zostały przesłane jako JSON
    if request.is_json:
        data = request.get_json()  # Pobieranie danych JSON
        json_data = json.dumps(data)
        actions.append(json_data)
        return {"message": "Dane zostały odebrane"}, 200
    else:
        return {"error": "Dane muszą być w formacie JSON"}, 400

@app.route('/hardware/gpio/value/<string:value>/<int:pin_number>', methods=['GET'])
def get_gpio_value(value, pin_number):
    if str(pin_number) not in requirements:
        return {}, 500
    data = requirements.get(str(pin_number))
    response = {
        "value": data.get(value)
    }
    actions.append({"action": "Get", "pinNumber": pin_number, "value": data.get(value)})
    return jsonify(response), 200

if __name__ == '__main__':
    app.run(host="0.0.0.0", port=5000)

    