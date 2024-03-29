import React from 'react';
import Select from 'react-select';
import { DateType, StockType } from '../../common/Enums';
import Popup from 'react-popup';
import { Button, DropdownButton, Dropdown, Table } from 'react-bootstrap';
import Highcharts from 'highcharts/highstock';
import HighchartsReact from 'highcharts-react-official';
require("highcharts/indicators/indicators-all")(Highcharts)

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
            statisticRes: {},
            balanceOptions: {}
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
        // this.getStock();
    }

    getStock = () => {
        var historyDataArr = []
        var volumeDataArr = []
        var tradesShortArr = []
        var tradesLongArr = []
        let options2 = {
            title: {
                text: 'My BTC-USD chart'
            },
            chart: {
                height: 900,
                zoomBySingleTouch: true,
                zoomType: 'x',
                // backgroundColor: "#B7C3C1",
                // borderColor: '#EBBA95',
                // plotBackgroundColor: "#B7C3C1",
                // plotBorderColor: '#346691',
                plotBorderWidth: 2
            },
            series: [],
            plotOptions: {
                series: {
                    showInLegend: true
                },
                candlestick: {
                    // shared options for all candlestick series
                }
            },
            legend: {
                enabled: false
            },
            // rangeSelector: {
            //     selected: 2
            // },
            yAxis: [{
                height: '60%',
                opposite: false,
                gridLineDashStyle: 'longdashdotdot'
            }, {
                top: '60%',
                height: '20%',
                // opposite: false,
                gridLineWidth: 0
            }, {
                top: '80%',
                height: '20%',
                // opposite: false,
                gridLineWidth: 0
            }],

            tooltip: {
                split: true
            },
        }

        let url = `http://localhost:9082/backtester/result`
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
                let historyDataRes = res["history_data"];
                let tradesDataRes = res["trades_data"];
                let statisticRes = res["statistic_result"];
                let dailyRes = res["daily_result"];
                let dailyResObj = JSON.parse(dailyRes);
                let dailyTsArr = [];
                console.log(dailyResObj);

                // Set statistic res
                this.setState({
                    statisticRes: statisticRes
                })

                // Set balance chart
                let balanceDataArr = [];
                let pnlDataArr = [];
                let pnlPointArr = [];
                for (let tsStr in dailyResObj.balance) {
                    let ts = parseInt(tsStr);
                    balanceDataArr.push(
                        dailyResObj.balance[tsStr]
                    );
                    dailyTsArr.push(new Date(ts).toDateString());
                }
                // console.log("balance data arr");
                // console.log(balanceDataArr);
                
                // Net P&L
                for (let tsStr in dailyResObj.net_pnl) {
                    let pnl = dailyResObj.net_pnl[tsStr];
                    let color = "red";
                    if (pnl < 0) {
                        color = "green"
                    }
                    pnlDataArr.push(
                        {
                            y: pnl,
                            color: color
                        }
                    );
                }
                let balanceOpts = {
                    title: {
                        text: "账户净值"
                    },
                    xAxis: {
                        categories: dailyTsArr
                    },
                    chart: {
                        height: 600,
                        zoomBySingleTouch: true,
                        zoomType: 'x',
                        // backgroundColor: "#B7C3C1",
                        // borderColor: '#EBBA95',
                        // plotBackgroundColor: "#B7C3C1",
                        // plotBorderColor: '#346691',
                        plotBorderWidth: 2
                    },
                    series: [
                        {
                            id: "balance",
                            type: "line",
                            data: balanceDataArr,
                        },
                        {
                            type: "column",
                            data: pnlDataArr,
                            onSeries: 'balance',
                            yAxis: 1,
                            // points: pnlPointArr
                        }
                    ],
                    yAxis: [
                        {
                            height: '60%',
                            opposite: false,
                            // gridLineDashStyle: 'longdashdotdot'
                        }, {
                            top: '60%',
                            height: '40%',
                            opposite: true,
                            gridLineDashStyle: 'longdashdotdot'
                        }
                    ],
                }
                this.setState({
                    balanceOptions: balanceOpts
                });
                console.log(this.state);


                // Set main chart, buy/sell & history data
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
                for (let i = 0; i < tradesDataRes.length; ++i) {
                    let curTrade = tradesDataRes[i]
                    let curDateTime = curTrade["ts"] * 1000
                    if (curTrade["dir"] === "SHORT") {
                        tradesShortArr.push(
                            {
                                x: curDateTime,
                                title: "Sell"
                            }
                        )
                    } else if (curTrade["dir"] === "LONG") {
                        tradesLongArr.push(
                            {
                                x: curDateTime,
                                title: "Buy"
                            }
                        )
                    }
                }
                options2.series.push(
                    {
                        id: 'history',
                        name: 'History',
                        data: historyDataArr,
                        type: "candlestick",
                        showInNavigator: true
                    }
                )
                options2.series.push(
                    {
                        type: 'flags',
                        name: 'Buy',
                        data: tradesLongArr,
                        onSeries: 'history',
                        shape: 'squarepin',
                        color: Highcharts.getOptions().colors[0], // same as onSeries
                        fillColor: Highcharts.getOptions().colors[0],
                        width: 16,
                        style: { // text style
                            color: 'white'
                        },
                        states: {
                            hover: {
                                fillColor: '#395C84' // darker
                            }
                        }
                    }
                )
                options2.series.push(
                    {
                        type: 'flags',
                        name: 'Sell',
                        data: tradesShortArr,
                        onSeries: 'history',
                        shape: 'squarepin',
                        color: Highcharts.getOptions().colors[1], // same as onSeries
                        fillColor: Highcharts.getOptions().colors[1],
                        width: 16,
                        style: { // text style
                            color: 'white'
                        },
                        states: {
                            hover: {
                                fillColor: '#395C84' // darker
                            }
                        }
                    }
                )
                options2.series.push(
                    {
                        id: "volume",
                        data: volumeDataArr,
                        type: "column",
                        name: "Volumn",
                        yAxis: 1
                    }
                )
                options2.series.push(
                    {
                        type: 'sma',
                        id: 'sma1',
                        linkedTo: 'history',
                        yAxis: 0,
                        marker: {
                            lineWidth: 0,
                            radius: 2
                        },
                        params: {
                            period: 600
                        }
                    }
                )
                options2.series.push(
                    {
                        type: 'sma',
                        id: 'sma2',
                        linkedTo: 'history',
                        yAxis: 0,
                        marker: {
                            lineWidth: 0,
                            radius: 2
                        },
                        params: {
                            period: 1200
                        }
                    }
                )
                options2.series.push(
                    {
                        type: 'macd',
                        id: 'oscillator',
                        linkedTo: 'history',
                        yAxis: 2,
                        params: {
                            shortPeriod: 12,
                            longPeriod: 26,
                            signalPeriod: 9,
                            period: 26
                        }
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

    render() {
        return (
            <div>
                {this.state.options != undefined &&
                    <HighchartsReact
                        highcharts={Highcharts}
                        constructorType={'stockChart'}
                        options={this.state.options}
                    />}
                <div>{this.state.balanceOptions != undefined &&
                    <HighchartsReact
                        highcharts={Highcharts}
                        constructorType={'chart'}
                        options={this.state.balanceOptions}
                    />}
                </div>
                <div>
                    <Table size='sm' bordered hover striped>
                        <tbody>
                            {
                                this.state.statisticRes != undefined && Object.keys(this.state.statisticRes).map((key) => {
                                    let val = this.state.statisticRes[key];
                                    return <tr>
                                        <th>
                                            {key}
                                        </th>
                                        <th>
                                            {val}
                                        </th>
                                    </tr>
                                })
                            }
                        </tbody>
                    </Table>
                </div>
                <div style={{ display: 'inline-block' }}>
                    <Button
                        variant="success"
                        onClick={this.getStock}>
                        Get Results
                    </Button>
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