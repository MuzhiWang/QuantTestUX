import React from 'react';
import CanvasJSReact from '../lib/canvasjs.react';
import Select from 'react-select';
import {DateType, StockType} from '../common/Enums';
import Popup from 'react-popup';
import './BasicStockCharts.css';
// import 'bootstrap/dist/css/bootstrap.min.css'
import {Button, DropdownButton, Dropdown} from 'react-bootstrap'


var CanvasJS = CanvasJSReact.CanvasJS;
var CanvasJSChart = CanvasJSReact.CanvasJSChart;

var intervalConfig = {
    '1min': 1200,
    '5min': 240,
    'day': 5
}

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
            startDate: "2018-07-25",
            endDate: "2019-11-10",
            dateType: DateType.ONE_DAY,
            stockType: StockType.INDEX,
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
        
        this.dateTypeOptions = [
            { value: DateType.ONE_MIN, label: '1 min', color: '#00B8D9', isFixed: true },
            { value: DateType.FIVE_MIN, label: '5 min', color: '#0052CC', isFixed: true },
            { value: DateType.ONE_DAY, label: '1 day', color: '#5243AA', isFixed: true },
        ];

        this.stockTypeOptions = [
            { value: StockType.STOCK, label: 'stock', color: '#00B8D9', isFixed: true },
            { value: StockType.INDEX, label: 'index', color: '#0052CC', isFixed: true },
        ]

        this.selectedDateTypeOption = selectedOption => {
            this.setState({
                dateType: selectedOption.value
            });
            // console.log(`date type: ${this.state.dateType}`)
        };

        this.selectedStockTypeOption = selectedOption => {
            this.setState({
                stockType: selectedOption.value
            })
            // console.log(`stock type: ${this.state.stockType}`)
        }
    }

    componentDidMount() {
        this.getStock();
    }

    getStock = () => {
        let options2  = {
			theme: "light1", // "light1", "light2", "dark1", "dark2"
			animationEnabled: true,
            exportEnabled: true,
            zoomEnabled: true,
			title:{
				text: `${this.state.stockId} ${this.state.stockType} price`
			},
			axisX: {
                interval: intervalConfig[this.state.dateType],
			},
			axisY: {
				includeZero:false,
				prefix: "￥",
				title: "Price (in RMB)"
			}
        }
        var dataSeries = { 
            type: "candlestick",
            showInLegend: true,
            name: this.state.stockId,
            yValueFormatString: "$###0.00",
            // xValueFormatString: "MMM-D YYYY HH:mm",
         };
        let dataPoints = [];
        dataSeries.dataPoints = dataPoints;
        let url = `http://127.0.0.1:5000/get_stock_df?stock_id=${this.state.stockId}&&start_date=${this.state.startDate}&&end_date=${this.state.endDate}&&date_type=${this.state.dateType}`
        if (this.state.stockType == StockType.INDEX) {
            url += '&&index=True'
        }
        fetch(url)
        .then(res => res.json())
        .then(res => {
            console.log("log res: \n\n\n");
            console.log(res);
            var stockData = [];
            let stockDataPoints = [];
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
            options2.data = stockData;

            return options2;
        })
        .then(options => {
            this.setState({
                options: options
            });
            console.log("here it is")
            console.log(this.state);
        })
        .catch(err => {
            Popup.create({
                title: 'Request failed',
                content: `Please retry to connect to the server. \n\n Error msg: ${err}`,
                className: 'alert',
                buttons: {
                    right: ['ok']
                }
            }, true);    
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
                    {this.state.options != undefined &&
                    <CanvasJSChart options = {this.state.options}/>}
                    <div style={{display: 'inline-block'}}>
                        <Button
                            variant="success"
                            onClick={this.getStock}>
                                Get Stock
                        </Button>
                        <Select
                            defaultValue={this.dateTypeOptions[2]}
                            options={this.dateTypeOptions}
                            // options={groupedOptions}
                            // formatGroupLabel={formatGroupLabel}
                            onChange={this.selectedDateTypeOption}
                        />
                        <Select
                            defaultValue={this.stockTypeOptions[1]}
                            options={this.stockTypeOptions}
                            // options={groupedOptions}
                            // formatGroupLabel={formatGroupLabel}
                            onChange={this.selectedStockTypeOption}
                        />
                        {/* <DropdownButton
                            title={this.state.dateType}>
                            <Dropdown.Item eventKey="1" onClick={this.selectedOption}>1 min</Dropdown.Item>
                            <Dropdown.Item eventKey="2" onClick={this.selectedOption}>5 min</Dropdown.Item>
                            <Dropdown.Item eventKey="3" onClick={this.selectedOption}>1 day</Dropdown.Item>
                        </DropdownButton> */}
                        <input type="text" name="stockId" value={this.state.stockId} 
                        onChange={this.changeStockId.bind(this)} />
                        <input type="text" name="startDate" value={this.state.startDate} 
                        onChange={this.changeStartDate.bind(this)} />
                        <input type="text" name="endDate" value={this.state.endDate} 
                        onChange={this.changeEndDate.bind(this)} />
                    </div>
                    <Popup 
                        className="mm-popup"
                        btnClass="mm-popup__btn"
                        closeBtn={true}
                        defaultOk="Ok" />
              </div>
          );
    }
}

export default BasicStockCharts