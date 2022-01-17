import React from 'react';
import { Table, Tag, Space } from 'antd';

// var ws = new WebSocket('ws://localhost:9082/event_log');

const eventLogRegEx = /".*?"/g

class EventLog extends React.Component {
    eventQueueSize = 99;

    sorter = (a, b) => (isNaN(a) && isNaN(b) ? (a || '').localeCompare(b || '') : a - b);
    eventColumns = [
        {
            title: 'Timestamp',
            dataIndex: 'ts',
            key: 'ts',
            sorter: (a, b) => this.sorter(b.ts, a.ts),
        },
        {
            title: 'Time',
            dataIndex: 'time',
            key: 'time',
        },
        {
            title: 'Info',
            dataIndex: 'info',
            key: 'info',
        }
    ];

    ws = new WebSocket('ws://localhost:9082/event_log');

    constructor(props) {
        super(props);

        this.state = {
            eventList: []
        }

        var eventLog = this;
        this.ws.onopen = function(event) {
            console.log("event log ws open");
        }

        this.ws.onmessage = function(event) {
            console.log("%%%%%%%%%%%%%%%%%%%%");
            console.log(event.data);
            eventLog.setState(function(prevs) {
                while (prevs.eventList.length > this.eventQueueSize) {
                    prevs.eventList.shift();
                }
                let d = event.data;

                try {
                    let [ts, date, info] = this.parseEventLog(d);

                    prevs.eventList.unshift({
                        key: ts,
                        ts: ts,
                        time: date,
                        info: info,
                    });
                } catch (err) {
                    console.error(err);      
                }
                return {
                    eventList: prevs.eventList,
                };
            });
            console.log(eventLog.state);
        }

        this.ws.onclose = function(event) {
            console.log("event log ws close");
        }

        this.ws.onerror = function(err) {
            console.log(err.message)
        }

    }

    parseEventLog(str) {
        let splitArr = str.split(",");
        console.log(splitArr);
        let ts = parseInt(splitArr[3].replace(/\s+/g, ""));
        console.log(ts);
        let date = new Date(ts);
        let dateStr = date.toLocaleString("en-US");

        let matches = str.match(eventLogRegEx);
        console.log(matches);
        let msg = matches[1];
        console.log(msg);

        msg = this.toStr(msg);

        return [ts, dateStr, msg];
    }

    // Turn to the Unicode codeacharcodeat() method to return the Unicode encoding of the character at the specified location. The return value is an integer between 0 and 65535.
    toUnicode(str) {
        var res = [];
        for ( var i=0; i<str.length; i++ ) {
            res[i] = ( "00" + str.charCodeAt(i).toString(16) ).slice(-4);
        }
        return "\\u" + res.join("\\u");
    }

    // The unicode escape() function encodes strings
    toStr(str){
        str = str.replace(/(\\u)(\w{1,4})/gi,function(v){
            return (String.fromCharCode(parseInt((escape(v).replace(/(%5Cu)(\w{1,4})/g,"$2")),16)));
        });
        str = str.replace(/(&#x)(\w{1,4});/gi,function(v){
            return String.fromCharCode(parseInt(escape(v).replace(/(%26%23x)(\w{1,4})(%3B)/g,"$2"),16));
        });
        str = str.replace(/(&#)(\d{1,6});/gi,function(v){
            return String.fromCharCode(parseInt(escape(v).replace(/(%26%23)(\d{1,6})(%3B)/g,"$2")));
        });

        return str;
    }

    render() {
        return <div>
            <Table 
                dataSource={this.state.eventList}
                columns={this.eventColumns} 
            />
        </div>
    }
}

export default EventLog