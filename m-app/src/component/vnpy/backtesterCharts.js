import React from 'react';
import Select from 'react-select';
import { DateType, StockType } from '../../common/Enums';
import Popup from 'react-popup';
import { Button, DropdownButton, Dropdown } from 'react-bootstrap';
import Highcharts from 'highcharts/highstock';
import HighchartsReact from 'highcharts-react-official';

var intervalConfig = {
    '1min': 360,
    '5min': 60,
    'day': 5
}

class BackTesterCharts extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            options: {
                title: {
                    text: 'My stock chart'
                },
                series: []
            },
            stockId: "000001",
            startDate: "2018-07-25",
            endDate: "2019-11-10",
            dateType: DateType.ONE_DAY,
            stockType: StockType.INDEX,
        };

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
        var historyDataArr = []
        var volumeDataArr = []
        var tradesDataArr = []
        let options2 = {
            title: {
                text: 'My stock chart'
            },
            series: [],
            plotOptions: {
                series: {
                    // general options for all series
                },
                candlestick: {
                    // shared options for all candlestick series
                }
            },
            // rangeSelector: {
            //     selected: 1
            // },
            yAxis: [{
                labels: {
                    align: 'right',
                    x: -3
                },
                title: {
                    text: 'OHLC'
                },
                height: '60%',
                lineWidth: 2,
                resize: {
                    enabled: true
                }
            }, {
                labels: {
                    align: 'right',
                    x: -3
                },
                title: {
                    text: 'Volume'
                },
                top: '65%',
                height: '35%',
                offset: 0,
                lineWidth: 2
            }],

            tooltip: {
                split: true
            },
        }
        // set the allowed units for data grouping
        let groupingUnits = [[
            'week',             // unit name
            [1]               // allowed multiples
        ], [
            'month',
            [1, 2, 3, 4, 6]
        ]]

        let url = `http://localhost:9082/backtester`
        fetch(url, {
            method: 'GET',
            headers: {
                'Content-Type': 'application/json',
                'Access-Control-Allow-Origin': '*',
                'x-requested-with': '*'
            },
        })
            .then(res => res.json())
            .then(res => {
                console.log("log res: \n\n\n");
                console.log(res);
                let stockCount = Object.keys(res["history_data"]).length;
                let historyDataRes = res["history_data"]
                let tradesDataRes = res["trades_data"]

                for (let i = 0; i < stockCount; ++i) {
                    let curBarData = historyDataRes[i]
                    let curDateTime = curBarData["ts"] * 1000
                    historyDataArr.push(
                        [curDateTime, curBarData["O"], curBarData["H"], curBarData["L"], curBarData["C"]]
                    )
                    volumeDataArr.push(
                        [curDateTime, curBarData["V"]]
                    )
                }
                options2.series.push(
                    {
                        data: historyDataArr,
                        type: "candlestick",
                        // dataGrouping: {
                        //     units: groupingUnits
                        // }
                    }
                )
                options2.series.push(
                    {
                        data: volumeDataArr,
                        type: "column",
                        name: "Volumn",
                        // dataGrouping: {
                        //     units: groupingUnits
                        // }
                    }
                )

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
        this.setState({ stockId: event.target.value });
    }

    changeStartDate(event) {
        this.setState({ startDate: event.target.value });
    }

    changeEndDate(event) {
        this.setState({ endDate: event.target.value });
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
                    <HighchartsReact
                        highcharts={Highcharts}
                        constructorType={'stockChart'}
                        options={this.state.options}
                    />}
                <div style={{ display: 'inline-block' }}>
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

export default BackTesterCharts