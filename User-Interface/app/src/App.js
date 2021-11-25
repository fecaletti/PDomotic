import logo from './logo.svg';
import './App.css';
import React, { useEffect, useRef, useState } from 'react';
import Table from 'react-bootstrap/Table'
import Card from 'react-bootstrap/Card'
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

  function SetConfigs() {
    alert('teste');
    let configs = {
      "key": "automaticLightCommand",
      "value": ConfigAutoCommand.current.value
    };
    axios.put(MAIN_API_URL, configs)
    .then(response => {
      LoadComponent();
    }).catch(error => {
      console.log(error);
    })
  }
  
  function ToggleOutput(outId) {
    let configs = {
      "outId": outId,
      "value": !parseInt(Output0Ref.current.value)
    };
    axios.post(MAIN_API_URL, configs)
    .then(response => {
      LoadComponent();
    }).catch(error => {
      console.log(error);
    })
  }

  function LoadComponent() {
    axios.get(MAIN_API_URL).then(response => {
      console.log("SUCCESS", response);
      setGetMessage(response);
      
      let jsonLoaded = JSON.parse(response.data);
      setHardwareOutputData(JSON.parse(jsonLoaded["serviceOutput"]));
      setHardwareInputData(JSON.parse(jsonLoaded["serviceInput"]));
      // console.log("TARGET", getHardwareOutputData["automaticLightCommand"]);
    }).catch(error => {
      console.log(error);
    })
  }

  useEffect(()=>{
    LoadComponent();
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
            <h3>LOADING</h3>}</div>
        </header>
        <body className="App-body">
            <div className="App-body-row">
              <div className="App-body-outputs">
                <label className="App-table-header">OUTPUTS</label>
                {getHardwareOutputData !== undefined? 
                <div><span>OUT0: </span><span ref={Output0Ref}>{getHardwareOutputData["out0"]}</span></div>
                :
                <h4>...</h4>}
                <button className="App-btn" onClick={ToggleOutput()} ref={ToggleBtnRef}>Toggle</button>
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
                        <select value={getHardwareOutputData["automaticLightCommand"]} ref={ConfigAutoCommand}>
                          <option value="0">0</option>
                          <option value="1">1</option>
                        </select>
                      </td>
                    </tr>
                    <tr>
                      <td>Executed Requests:</td>
                      <td>{getHardwareOutputData["execRequestCounter"]}</td>
                    </tr>
                  </tbody>
                </Table>
                : 
                <h4>...</h4>}
                <br/>
                <button className="App-btn" onClick={SetConfigs()} ref={ConfigBtnRef}>Send</button>
              </div>
            </div>
        </body>
      </div>

    </div>
  );
}

export default App;