import React from 'react';
import { Button, DropdownButton, Dropdown, Form, InputGroup, FormControl } from 'react-bootstrap';
const { default: PopupReact } = require("react-popup/dist/Popup.react");
const { RunBacktestingURL } = require("../../common/constants");
const { DateToTimestamp } = require("../../utils/utils");


const defaultClassName = "DoubleMaStrategy";
const defaultSymbol = "BTC-USD";
const defaultExchange = "COINBASE";
const defaultInterval = "1m";
const defaultStartDate = "2021-11-01";
const defaultEndDate = "2021-12-01";
const defaultRate = 2.5e-5;
const defaultSlippage = 0.2;
const defaultSize = 300;
const defaultPricetick = 0.2;
const defaultCapital = 1000000;
const defaultInverse = false;
const fastWindow = "fast_window";
const slowWindow = "slow_window";
var defaultSettings = {};
defaultSettings[fastWindow] = 600;
defaultSettings[slowWindow] = 1200;

class RunBacktesting extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            className: defaultClassName,
            symbol: defaultSymbol,
            exchange: defaultExchange,
            interval: defaultInterval,
            startTs: DateToTimestamp(defaultStartDate),
            endTs: DateToTimestamp(defaultEndDate),
            rate: defaultRate,
            splippage: defaultSlippage,
            size: defaultSize,
            pricetick: defaultPricetick,
            capital: defaultCapital,
            inverse: defaultInverse,
            settings: defaultSettings,
        }
    }

    runBacktesting = () => {
        let url = RunBacktestingURL;
        let reqBody = {
            "class_name": this.state.className,
            "symbol": this.state.symbol,
            "exchange": this.state.exchange,
            "interval": this.state.interval,
            "start_ts": this.state.startTs,
            "end_ts": this.state.endTs,
            "rate": this.state.rate,
            "slippage": this.state.splippage,
            "size": this.state.size,
            "pricetick": this.state.pricetick,
            "capital": this.state.capital,
            "inverse": this.state.inverse,
            "setting": this.state.settings,
        };
        console.log(reqBody);

        fetch(url, {
            method: "POST",
            body: JSON.stringify(reqBody),
            headers: {
                'Content-Type': 'application/json',
                'Access-Control-Allow-Origin': '*',
                'x-requested-with': '*'
            },
        }).then(res => {

        }).catch(err => {
            PopupReact.create({
                title: 'Trigger running backtesting failed',
                content: `Please retry to connect to the server. \n\n Error msg: ${err}`,
                className: 'alert',
                buttons: {
                    right: ['ok']
                }
            }, true);
        })
    }

    updateClassName = (event) => {
        this.state.className = event.target.value;
    }
    updateSymbol = (event) => {
        this.state.symbol = event.target.value;
    }
    updateExchange = (event) => {
        this.state.exchange = event.target.value;
    }
    updateInterval = (event) => {
        this.state.interval = event.target.value;
    }
    updateStartTs = (event) => {
        this.state.startTs = DateToTimestamp(event.target.value);
    }
    updateEndTs = (event) => {
        this.state.endTs = DateToTimestamp(event.target.value);
    }
    updateRate = (event) => {
        this.state.rate = event.target.value;
    }
    updateSlippage = (event) => {
        this.state.splippage = event.target.value;
    }
    updateSize = (event) => {
        this.state.size = event.target.value;
    }
    updatePricetick = (event) => {
        this.state.pricetick = event.target.value;
    }
    updateCapital = (event) => {
        this.state.capital = event.target.value;
    }
    updateInverse = (event) => {
        this.state.inverse = event.target.value;
    }
    updateFastWindow = (event) => {
        this.state.settings[fastWindow] = event.target.value;
    }
    updateSlowWindow = (event) => {
        this.state.settings[slowWindow] = event.target.value;
    }

    render() {
        return (
            <div> 
                <InputGroup className="mb-3">
                    <InputGroup.Text>交易策略</InputGroup.Text>
                    <FormControl placeholder={defaultClassName}  onChange={this.updateClassName} />
                </InputGroup>
                <InputGroup className="mb-3">
                    <InputGroup.Text>股票代码</InputGroup.Text>
                    <FormControl placeholder={defaultSymbol}  onChange={this.updateSymbol} />
                </InputGroup>
                <InputGroup className="mb-3">
                    <InputGroup.Text>交易所代码</InputGroup.Text>
                    <FormControl placeholder={defaultExchange}  onChange={this.updateExchange} />
                </InputGroup>
                <InputGroup className="mb-3">
                    <InputGroup.Text>开始日期</InputGroup.Text>
                    <FormControl placeholder={defaultStartDate}  onChange={this.updateStartTs} />
                </InputGroup>
                <InputGroup className="mb-3">
                    <InputGroup.Text>结束日期</InputGroup.Text>
                    <FormControl placeholder={defaultEndDate}  onChange={this.updateEndTs} />
                </InputGroup>
                <InputGroup className="mb-3">
                    <InputGroup.Text>手续费率</InputGroup.Text>
                    <FormControl placeholder={defaultRate}  onChange={this.updateRate} />
                </InputGroup>
                <InputGroup className="mb-3">
                    <InputGroup.Text>交易滑点</InputGroup.Text>
                    <FormControl placeholder={defaultSlippage}  onChange={this.updateSlippage} />
                </InputGroup>
                <InputGroup className="mb-3">
                    <InputGroup.Text>合约乘数</InputGroup.Text>
                    <FormControl placeholder={defaultSize}  onChange={this.updateSize} />
                </InputGroup>
                <InputGroup className="mb-3">
                    <InputGroup.Text>价格跳动</InputGroup.Text>
                    <FormControl placeholder={defaultPricetick}  onChange={this.updatePricetick} />
                </InputGroup>
                <InputGroup className="mb-3">
                    <InputGroup.Text>回测资金</InputGroup.Text>
                    <FormControl placeholder={defaultCapital}  onChange={this.updateCapital} />
                </InputGroup>
                <InputGroup className="mb-3">
                    <InputGroup.Text>合约模式</InputGroup.Text>
                    <FormControl placeholder={defaultInverse.toString()}  onChange={this.updateInverse} />
                </InputGroup>
                <InputGroup className="mb-3">
                    <InputGroup.Text>快周期</InputGroup.Text>
                    <FormControl placeholder={defaultSettings[fastWindow]}  onChange={this.updateFastWindow} />
                </InputGroup>
                <InputGroup className="mb-3">
                    <InputGroup.Text>慢周期</InputGroup.Text>
                    <FormControl placeholder={defaultSettings[slowWindow]}  onChange={this.updateSlowWindow} />
                </InputGroup>
                <Button variant="primary" type="submit" onClick={this.runBacktesting}>
                    开始回测
                </Button>
            </div>
        )
    }
}

export default RunBacktesting