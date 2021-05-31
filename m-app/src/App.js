import React from 'react';
import logo from './logo.svg';
import './App.css';
import BasicStockCharts from './component/BasicStockCharts'
import TDXTestStockCharts from './component/TDXTestStockCharts'
import VNPYStockCharts from './component/vnpy/vnpyStockCharts';
import 'bootstrap/dist/css/bootstrap.min.css'

function App() {
    return (
        <div className="App">
            {/* <BasicStockCharts /> */}
            <TDXTestStockCharts />
            <VNPYStockCharts />
        </div>
    );
}

export default App;
