import React from 'react';
import Popup from 'react-popup';
import { MarketplaceDownloadURL } from '../../common/constants';
import { Button, DropdownButton, Dropdown, Form, InputGroup, FormControl } from 'react-bootstrap';

const defaultSymbol = "BTC-USD";
const defaultExchange = "COINBASE";
const defaultInterval = "1m";

class MarketplaceDataDownload extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            symbol: "BTC-USD",
            exchange: "",
            interval: "1m",
            startDate: "",
            endDate: "",
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
        this.state.start_ts = event.target.value;
    }
    updateEndTs = (event) => {
        this.state.end_ts = event.target.value;
    }

    download = () => {
        let url = MarketplaceDownloadURL;
        // console.log("====================");
        // console.log(this.state);
        let reqBody = {
            "symbol": this.state.symbol,
            "exchange": this.state.exchange,
            "interval": this.state.interval,
            "start_ts": this.state.start_ts,
            "end_ts": this.state.end_ts,
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
                    <FormControl onChange={this.updateSymbol} />
                </InputGroup>
                <InputGroup className="mb-3" controlId="formBasicExchange" defaultValue={defaultSymbol}>
                    <InputGroup.Text>Exchange</InputGroup.Text>

                    <FormControl onChange={this.updateExchange} />
                </InputGroup>
                <InputGroup className="mb-3" controlId="formBasicInterval">
                    <InputGroup.Text>Interval</InputGroup.Text>
                    <FormControl onChange={this.updatePassword} />
                </InputGroup>
                <Button variant="primary" type="submit" onClick={this.connect}>
                    Connect
                </Button>
            </div>
        );
    }
}

export default MarketplaceDataDownload