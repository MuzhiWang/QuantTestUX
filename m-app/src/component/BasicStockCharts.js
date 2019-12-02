import React from 'react';
import CanvasJSReact from '../lib/canvasjs.react'

var CanvasJS = CanvasJSReact.CanvasJS;
var CanvasJSChart = CanvasJSReact.CanvasJSChart;

class BasicStockCharts extends React.Component {
    constructor(props) {
        super(props);

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
            data: data  // random data
        }

        this.state = {
            options: options
        };
    }

    getStock = () => {
        var options = this.state.options
        var dataSeries = { type: "line" };
        var dataPoints = [];
        dataSeries.dataPoints = dataPoints;
        // const request = async () => { 
        const response = fetch('http://127.0.0.1:5000/get_stock_df?stock_id=000001&&start_date=2019-07-25&&end_date=2019-09-01')
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
            console.log("here it is")
            console.log(options);
            return res;
        });
      
        console.log("after fetch");
        console.log(options);

        this.setState({
            options: options
        });

        console.log("check state options");
        console.log(this.state.options);
    }

    render() {
        return (
              <div>
                  <CanvasJSChart options = {this.state.options}/>
                  <button
                    type="button"
                    onClick={this.getStock}
                    >Get stock</button>
              </div>
          );
    }
}

export default BasicStockCharts