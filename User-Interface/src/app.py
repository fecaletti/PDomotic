from sys import path
import flask
from flask_restful import Api, Resource, reqparse
from flask import Flask, json, g, request
from flask_cors import CORS
from werkzeug import http #comment this on deployment
#from handlers.LightCommanderHandler import LightCommanderHandler
from lib.jApi import jApiClient

japi = jApiClient("..\\..\\CPP-Service\\data\\input.json", "..\\..\\CPP-Service\\data\\output.json")
app = Flask(__name__)
CORS(app) #comment this on deployment
api = Api(app)

class LightCommander(Resource):
    def get(self):
        return japi.GetServiceOutput()

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
    app.run(debug=True)