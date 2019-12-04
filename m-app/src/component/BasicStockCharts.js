import React from 'react';
import CanvasJSReact from '../lib/canvasjs.react'
import Select from 'react-select'
import {DateType} from '../common/Enums'

var CanvasJS = CanvasJSReact.CanvasJS;
var CanvasJSChart = CanvasJSReact.CanvasJSChart;

class BasicStockCharts extends React.Component {
    constructor(props) {
        super(props);

        // var limit = 50000;
        // var y = 100;    
        // var data = [];
        // var dataSeries = { type: "line" };
        // var dataPoints = [];
    
        // for (var i = 0; i < limit; i += 1) {
        //     y += Math.round(Math.random() * 10 - 5);
        //     dataPoints.push({
        //         x: i,
        //         y: y
        //     });
        // }
        // dataSeries.dataPoints = dataPoints;
        // data.push(dataSeries);
    
        // const spanStyle = {
        //     position:'absolute', 
        //     top: '10px',
        //     fontSize: '20px', 
        //     fontWeight: 'bold', 
        //     backgroundColor: '#d85757',
        //     padding: '0px 4px',
        //     color: '#ffffff'
        // }

        // var options = {
        //     zoomEnabled: true,
        //     animationEnabled: true,
        //     title: {
        //         text: "Try Zooming - Panning"
        //     },
        //     axisY: {
        //         includeZero: false
        //     },
        //     // data: data  // random data
        // }

        
        this.state = {
            options: {},
            stockId: "000001",
            startDate: "2019-07-25",
            endDate: "2019-11-10",
            dateType: DateType.ONE_MIN
        };

        this.options = {
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

        this.options2  = {
			theme: "light2", // "light1", "light2", "dark1", "dark2"
			animationEnabled: true,
            exportEnabled: true,
            zoomEnabled: true,
			title:{
				text: `${this.state.stockId} Stock Price`
			},
			axisX: {
                interval: 3600,
			},
			axisY: {
				includeZero:false,
				prefix: "￥",
				title: "Price (in RMB)"
			}
        }
        
        this.dateTypeOptions = [
            { value: DateType.ONE_MIN, label: '1 min', color: '#00B8D9', isFixed: true },
            { value: DateType.FIVE_MIN, label: '5 min', color: '#0052CC', isFixed: true },
            { value: DateType.ONE_DAY, label: '1 day', color: '#5243AA', isFixed: true },
        ];

        this.selectedOption = selectedOption => {
            this.setState({
                dateType: selectedOption.value
            });
            console.log(`date type: ${this.state.dateType}`)
        };
    }

    componentDidMount() {
        this.getStock();
    }

    getStock = () => {
        var dataSeries = { 
            type: "candlestick",
            showInLegend: true,
            name: this.state.stockId,
            yValueFormatString: "$###0.00",
            // xValueFormatString: "MMM-D YYYY HH:mm",
         };
        let dataPoints = [];
        dataSeries.dataPoints = dataPoints;
        // const request = async () => { 
        fetch(`http://127.0.0.1:5000/get_stock_df?stock_id=${this.state.stockId}&&start_date=${this.state.startDate}&&end_date=${this.state.endDate}`)
        .then(res => res.json())
        .then(res => {
            console.log("log res: \n\n\n");
            console.log(res);
            var stockData = [];
            let stockDataPoints = [];
            // console.log(res["date"])
            let stockCount = Object.keys(res["date"]).length;
            let date = res["date"];
            let open = res["open"];
            let high = res["high"];
            let low = res["low"];
            let close = res["close"];

            for (let i = 0; i < stockCount; ++i) {
                stockDataPoints.push({
                    // label: new Date(date[i]),
                    label: date[i],
                    y: [open[i], high[i], low[i], close[i]]
                });
            }
            dataSeries.dataPoints = stockDataPoints;
            stockData.push(dataSeries);
            this.options2.data = stockData;

            return this.options2;
        })
        .then(options => {
            this.setState({
                options: options
            });
            console.log("here it is")
            console.log(this.state);
        });
    }

    changeStockId(event) {
        this.setState({stockId: event.target.value});
    }
    
    changeStartDate(event) {
        this.setState({startDate: event.target.value});
    }

    changeEndDate(event) {
        this.setState({endDate: event.target.value});
    }

    changeDateType(selectedOption) {
        
        console.log(`select option: ${selectedOption}`);
        this.setState({
            dateType: selectedOption.value
        });
    }

    render() {
        return (
              <div>
                    <Select
                        defaultValue={this.dateTypeOptions[0]}
                        options={this.dateTypeOptions}
                        // options={groupedOptions}
                        // formatGroupLabel={formatGroupLabel}
                        onChange={this.selectedOption}
                    />
                    {this.state.options != undefined &&
                    <CanvasJSChart options = {this.state.options}/>}
                    <button
                        type="button"
                        onClick={this.getStock}
                        >Get stock</button>
                    <input type="text" name="stockId" value={this.state.stockId} 
                    onChange={this.changeStockId.bind(this)} />
                    <input type="text" name="startDate" value={this.state.startDate} 
                    onChange={this.changeStartDate.bind(this)} />
                    <input type="text" name="endDate" value={this.state.endDate} 
                    onChange={this.changeEndDate.bind(this)} />
              </div>
          );
    }
}

export default BasicStockCharts