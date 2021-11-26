import logo from './logo.svg';
import './App.css';
import React, { useEffect, useRef, useState } from 'react';
import Table from 'react-bootstrap/Table'
import Card from 'react-bootstrap/Card'
import Button from 'react-bootstrap/Button'
import axios from 'axios';

const MAIN_API_URL = 'http://localhost:4433/';

function App() {
  const [getMessage, setGetMessage] = useState({});
  const [getHardwareOutputData, setHardwareOutputData] = useState({});
  const [getHardwareInputData, setHardwareInputData] = useState({});
  const Output0Ref = useRef(null);
  const ToggleBtnRef = useRef(null);
  const ConfigBtnRef = useRef(null);
  const ConfigAutoCommand = useRef(null);
  let lastValueConfig1 = 0;
  let actualValueConfig1 = 0;

  function SetConfigs() {
    let configs = {
      "key": "automaticLightCommand",
      "value": parseInt(document.getElementById('selectConfig0').value)
    };
    axios.put(MAIN_API_URL, configs)
    .then(response => {
      console.log(`PUT SENT RESP == ${response}`);
    }).catch(error => {
      console.log(error);
    })
  }
  
  function ToggleOutput(outId) {
    console.log(`POSTING -> ${!parseInt(getHardwareOutputData["out0"])}`);
    let configs = {
      "outId": outId,
      "targetValue": !parseInt(getHardwareOutputData["out0"])
    };
    axios.post(MAIN_API_URL, configs)
    .then(response => {
      console.log(`POST SENT! RESP -> ${response}`);
    }).catch(error => {
      console.log(error);
    })
  }

  function LoadComponent() {
    axios.get(MAIN_API_URL).then(response => {
      //console.log("SUCCESS", response);
      setGetMessage(response);
      
      let jsonLoaded = JSON.parse(response.data);
      setHardwareOutputData(JSON.parse(jsonLoaded["serviceOutput"]));
      setHardwareInputData(JSON.parse(jsonLoaded["serviceInput"]));

      let outParsed = JSON.parse(jsonLoaded["serviceOutput"]);
      if(lastValueConfig1 != outParsed["automaticLightCommand"]) {
        actualValueConfig1 = outParsed["automaticLightCommand"];
        lastValueConfig1 = outParsed["automaticLightCommand"];
        document.getElementById('selectConfig0').value = actualValueConfig1;
        //console.log("value set " + actualValueConfig1);
      }
      // console.log("TARGET", getHardwareOutputData["automaticLightCommand"]);
    }).catch(error => {
      setGetMessage(error);
      console.log(error);
    })
  }

  useEffect(()=>{
    const interval = setInterval(() => {
      LoadComponent();
    }, 300);
    return () => clearInterval(interval);
  }, [])
  return (
    <div className="App">
      <div className="App-wrapper">
        <header className="App-header">
          <img src={logo} className="App-logo" alt="logo" />
          <p>Light commander</p>
          <div>{getMessage.status === 200 ? 
            <h3>CONNECTED</h3>
            :
            <h3>NOT CONNECTED</h3>}</div>
        </header>
        <body className="App-body">
            <div className="App-body-row">
              <div className="App-body-outputs">
                <label className="App-table-header">OUTPUTS</label>
                {getHardwareOutputData !== undefined? 
                <div><span>OUT0: </span><span ref={Output0Ref}>{getHardwareOutputData["out0"]}</span></div>
                :
                <h4>...</h4>}
                <Button variant="outlined" className="App-btn" onClick={() => ToggleOutput(0)} ref={ToggleBtnRef}>Toggle</Button>
              </div>
              <div className="App-body-inputs">
                <label className="App-table-header">INPUTS</label>
                {getHardwareOutputData !== undefined?
                <Table striped bordered hover>
                  <thead>
                    <tr>
                      <th>Input</th>
                      <th>Value</th>
                    </tr>
                  </thead>
                  <tbody>
                    <tr>
                      <td>IN0:</td>
                      <td>{getHardwareOutputData["input0"]}</td>
                    </tr>
                    <tr> 
                      <td>Sens. 1:</td>
                      <td>{getHardwareOutputData["input1"]}</td>
                    </tr>
                    <tr>
                      <td>Sens. 2:</td>
                      <td>{getHardwareOutputData["input2"]}</td>
                    </tr>
                  </tbody>
                </Table>
                : 
                <h4>...</h4>}
              </div>
              <div className="App-body-settings">
                <label className="App-table-header">SETTINGS</label>
                {getHardwareOutputData !== undefined? 
                <Table striped bordered hover>
                  <thead>
                    <tr>
                      <th>Config</th>
                      <th>Value</th>
                    </tr>
                  </thead>
                  <tbody>
                    <tr>
                      <td>Automatic Command:</td>
                      <td>
                        <select id="selectConfig0" defaultValue="0" onChange={() => SetConfigs()}>
                          <option value="0">off</option>
                          <option value="1">on</option>
                        </select>
                      </td>
                    </tr>
                    <tr>
                      <td>People Counter:</td>
                      <td>{getHardwareOutputData["peopleCounter"]}</td>
                    </tr>
                  </tbody>
                </Table>
                : 
                <h4>...</h4>}
                <br/>
                {/* <Button className="App-btn" onClick={() => SetConfigs()} ref={ConfigBtnRef}>Send</Button> */}
              </div>
            </div>
        </body>
      </div>

    </div>
  );
}

export default App;