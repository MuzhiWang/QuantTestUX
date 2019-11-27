import React from 'react';
import logo from './logo.svg';
import './App.css';
import CanvasJSReact from './lib/canvasjs.react'
// import Canvas from 'react-canvas-js'

var CanvasJS = CanvasJSReact.CanvasJS;
var CanvasJSChart = CanvasJSReact.CanvasJSChart;

function App() {
  var limit = 50000;
	var y = 100;    
	var data = [];
	var dataSeries = { type: "line" };
	var dataPoints = [];

	for (var i = 0; i < limit; i += 1) {
		y += Math.round(Math.random() * 10 - 5);
		dataPoints.push({
			x: i,
			y: y
		});
	}
	dataSeries.dataPoints = dataPoints;
	data.push(dataSeries);

	const spanStyle = {
		position:'absolute', 
		top: '10px',
		fontSize: '20px', 
		fontWeight: 'bold', 
		backgroundColor: '#d85757',
		padding: '0px 4px',
		color: '#ffffff'
	}

	var options = {
		zoomEnabled: true,
		animationEnabled: true,
		title: {
			text: "Try Zooming - Panning"
		},
		axisY: {
			includeZero: false
		},
		// data: data  // random data
	}


	const request = async () => { 
		const response = await fetch('http://127.0.0.1:5000/get_stock_df?stock_id=000001&&start_date=2019-07-25&&end_date=2019-09-01')
		.then(res => res.json())
		.then(res => {
			console.log("log res: \n\n\n");
			console.log(res);
			var stockData = []
			var stockDataPoints = []
			console.log(res["date"])
			var stockCount = Object.keys(res["date"]).length
			for (var i = 0; i < stockCount; ++i) {
				stockDataPoints.push({
					x: i,
					y: res["close"][i]
				});
			}
			dataSeries.dataPoints = stockDataPoints;
			stockData.push(dataSeries);
			options.data = stockData;
			console.log(options);
			return res;
		});
		console.log("await end...")
		console.log(response)
	}

	request();
  
	console.log("after fetch");
	console.log(options);

  return (
    <div className="App">
      <header className="App-header">
        <img src={logo} className="App-logo" alt="logo" />
        <p>
          Edit <code>src/App.js</code> and save to reload. tes test test test
        </p>
        <a
          className="App-link"
          href="https://reactjs.org"
          target="_blank"
          rel="noopener noreferrer"
        >
          Learn React
        </a>
      </header>
      <div>
          <CanvasJSChart options = {options}/>
      </div>
    </div>
  );
}

export default App;
