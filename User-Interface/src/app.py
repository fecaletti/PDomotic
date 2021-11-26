from sys import path
import flask
from flask_restful import Api, Resource, reqparse
from flask import Flask, json, g, request
from flask_cors import CORS
#from werkzeug import http #comment this on deployment
#from handlers.LightCommanderHandler import LightCommanderHandler
from lib.jApi import jApiClient

japi = jApiClient("/home/pi/PDomotic/PDomotic/CPP-Service/data/input.json", "/home/pi/PDomotic/PDomotic/CPP-Service/data/output.json")
app = Flask(__name__)
CORS(app) #comment this on deployment
api = Api(app)

class LightCommander(Resource):
    def get(self):
        jsonObject = {}
        jsonObject["serviceOutput"] = japi.GetServiceOutput()
        jsonObject["serviceInput"] = japi.GetServiceInput()
        return json.dumps(jsonObject)

    def post(self):
        reqData = json.loads(request.data)
        if((reqData["outId"] != None) and (reqData["targetValue"] != None)):
            japi.SendExecutionRequest(1, int(reqData["outId"]), int(reqData["targetValue"]))
            return flask.Response(status=200)
        
        return flask.Response(status=401)
    
    def put(self):
        reqData = json.loads(request.data)
        ret = japi.SetServiceConfig(reqData)
        if ret == True:
            return flask.Response(status=200)
        
        return flask.Response(status=401)

api.add_resource(LightCommander, "/")

if __name__ == '__main__':
    japi.StartThread()
    app.run(host="0.0.0.0", debug=True, port=5000)