import React from 'react';
import logo from './logo.svg';
import './App.css';
import BasicStockCharts from './component/BasicStockCharts'
import TDXTestStockCharts from './component/TDXTestStockCharts'
import StockCharts from './component/vnpy/stockCharts';
import CoinbaseAccountConnector from './component/vnpy/coinbaseAccountConnector';

import 'bootstrap/dist/css/bootstrap.min.css'
import BackTesterCharts from './component/vnpy/backtesterCharts';

function App() {
    return (
        <div className="App">
            {/* <BasicStockCharts /> */}
            {/* <TDXTestStockCharts />
            <StockCharts /> */}
            <CoinbaseAccountConnector />
            <BackTesterCharts />
        </div>
    );
}

export default App;
