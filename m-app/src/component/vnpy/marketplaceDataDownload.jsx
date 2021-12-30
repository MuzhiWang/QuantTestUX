import React from 'react';
import Popup from 'react-popup';
import { MarketplaceDownloadURL } from '../../common/constants';
import { Button, DropdownButton, Dropdown, Form, InputGroup, FormControl } from 'react-bootstrap';
import { DateToTimestamp } from '../../utils/utils';

const defaultSymbol = "BTC-USD";
const defaultExchange = "COINBASE";
const defaultInterval = "1m";
const defaultStartDate = "2021-11-01"
const defaultEndDate = "2021-12-01"

class MarketplaceDataDownload extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            symbol: defaultSymbol,
            exchange: defaultExchange,
            interval: defaultInterval,
            startTs: DateToTimestamp(defaultStartDate),
            endTs: DateToTimestamp(defaultEndDate),
        }
    }

    componentDidMount() {}

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

    download = () => {
        let url = MarketplaceDownloadURL;
        // console.log("====================");
        console.log(this.state);
        let reqBody = {
            "symbol": this.state.symbol,
            "exchange": this.state.exchange,
            "interval": this.state.interval,
            "start_ts": this.state.startTs,
            "end_ts": this.state.endTs,
        };

        fetch(url, {
            method: "POST",
            body: JSON.stringify(reqBody),
            headers: {
                'Content-Type': 'application/json',
                'Access-Control-Allow-Origin': '*',
                'x-requested-with': '*'
            },
        }).then(res => {
            console.log("download process started successfully.")
        }).catch(err => {
            Popup.create({
                title: 'download data failed',
                content: `Please retry to connect to the server. \n\n Error msg: ${err}`,
                className: 'alert',
                buttons: {
                    right: ['ok']
                }
            }, true);
        })
    }

    render() {
        return (
            <div>
                <InputGroup className="mb-3" controlId="formBasicSymbol">
                    <InputGroup.Text>Symbol</InputGroup.Text>
                    <FormControl placeholder={defaultSymbol} onChange={this.updateSymbol} />
                </InputGroup>
                <InputGroup className="mb-3" controlId="formBasicExchange">
                    <InputGroup.Text>Exchange</InputGroup.Text>
                    <FormControl placeholder={defaultExchange} onChange={this.updateExchange} />
                </InputGroup>
                <InputGroup className="mb-3" controlId="formBasicInterval">
                    <InputGroup.Text>Interval</InputGroup.Text>
                    <FormControl placeholder={defaultInterval} onChange={this.updateInterval} />
                </InputGroup>
                <InputGroup className="mb-3" controlId="formBasicStartDate">
                    <InputGroup.Text>Start Date</InputGroup.Text>
                    <FormControl placeholder={defaultStartDate} onChange={this.updateStartTs} />
                </InputGroup>
                <InputGroup className="mb-3" controlId="formBasicEndDate">
                    <InputGroup.Text>End Date</InputGroup.Text>
                    <FormControl placeholder={defaultEndDate} onChange={this.updateEndTs} />
                </InputGroup>
                <Button variant="primary" type="submit" onClick={this.download}>
                    下载数据
                </Button>
            </div>
        );
    }
}

export default MarketplaceDataDownload