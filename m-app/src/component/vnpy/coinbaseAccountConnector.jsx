import React from 'react';
import Select from 'react-select';
import { DateType, StockType } from '../../common/Enums';
import Popup from 'react-popup';
import { Button, DropdownButton, Dropdown, Form, InputGroup, FormControl } from 'react-bootstrap';
import { AccountConnectURL } from '../../common/constants';

class CoinbaseAccountConnector extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            id: "",
            serect: "",
            passphrase: "",
            server: "REAL"
        };
    }

    componentDidMount() {
        // this.connect();
    }

    connect = () => {
        let url = AccountConnectURL;
        // console.log("====================");
        // console.log(this.state);
        let reqBody = {
            "id": this.state.id,
            "secret": this.state.serect,
            "server": "REAL",
            "passphrase": this.state.passphrase
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
            console.log("coinbase account connected successfully.")
        }).catch(err => {
            Popup.create({
                title: 'Coinbase account connection failed',
                content: `Please retry to connect to the server. \n\n Error msg: ${err}`,
                className: 'alert',
                buttons: {
                    right: ['ok']
                }
            }, true);
        })
    }

    updateID = (event) => {
        this.state.id = event.target.value;
    }
    updateSecret = (event) => {
        this.state.serect = event.target.value;
    }
    updatePassword = (event) => {
        this.state.passphrase = event.target.value;
    }

    render() {
        return (
            <div> 
                <InputGroup className="mb-3" controlId="formBasicID">
                    <InputGroup.Text>ID</InputGroup.Text>
                    <FormControl onChange={this.updateID} />
                </InputGroup>
                <InputGroup className="mb-3" controlId="formBasicSecret">
                    <InputGroup.Text>Secret</InputGroup.Text>
                    <FormControl onChange={this.updateSecret} />
                </InputGroup>
                <InputGroup className="mb-3" controlId="formBasicPassword">
                    <InputGroup.Text>Password</InputGroup.Text>
                    <FormControl onChange={this.updatePassword} />
                </InputGroup>
                <Button variant="primary" type="submit" onClick={this.connect}>
                    Connect
                </Button>
            </div>
        )
    };
}

export default CoinbaseAccountConnector