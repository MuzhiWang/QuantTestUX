import React from 'react';

// import WebSocket from 'ws';


const consumerOptions = {
    topic: 'EVENTLOGGG',
    kafkaHost: 'localhost:19092',
    groupId: 'fe_event_log',
    sessionTimeout: 15000,
    protocol: ['roundrobin'],
    asyncPush: false,
    id: 'consumer1',
    fromOffset: 'latest',
    encoding: 'utf8',
};

var ws = new WebSocket('ws://localhost:9082/event_log');

class EventLog extends React.Component {
    constructor(props) {
        super(props);

        this.state = {

        }

        ws.onopen = function(event) {
            console.log("event log ws open");
        }

        ws.onmessage = function(event) {
            console.log("%%%%%%%%%%%%%%%%%%%%");
            console.log(event.data);
        }

        ws.onclose = function(event) {
            console.log("event log ws close");
        }

        ws.onerror = function(err) {
            console.log(err.message)
        }

    }

    render() {
        return <div>

        </div>
    }
}

export default EventLog