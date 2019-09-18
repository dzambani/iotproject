#!/usr/bin/python
from flask import Flask, request, render_template
import json
import time
import datetime
from .QueryClass import QueryClass
#from .SensorClass import SensorClass

app = Flask(__name__)
sensor_dict = []
mdbsql = QueryClass();

def checkSensor(id):
    for s in sensor_dict:
        if id == s['id']:
            return false
    return true

def getTime():
    currtime = time.time()
    return datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')

@app.route('/post', methods = ['GET', 'POST'])
def doTheIOT():
    if request.method == 'POST':
        sensor = request.get_json()
        if checkSensor(sensor['id']):
            mdbsql.addSensor(sensor['id'], sensor['type'], sensor['location'])
            sensor_dict.append(sensor)
        mdbsql.addData(sensor['id'], getTime(), sensor['value'], sensor['status'])
    if request.method == 'GET':
        for s in sensor_dict:
            s['value'] = getLastValue(s['id']) #makes value and status a tuple
            s['status'] = getLastStatus(s['id'])
        return render_template('index.html', sensor_dict=sensor_dict)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=80, threaded=True)
