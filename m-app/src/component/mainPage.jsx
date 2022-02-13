import React from 'react';
import { Layout, Menu, Breadcrumb } from 'antd';
import {
    UpCircleTwoTone,
    DesktopOutlined,
    PieChartOutlined,
    FileOutlined,
    TeamOutlined,
    UserOutlined,
  } from '@ant-design/icons';
import CoinbaseAccountConnector from './vnpy/coinbaseAccountConnector';
import MarketplaceDataDownload from './vnpy/marketplaceDataDownload';
import BackTesterCharts from './vnpy/backtesterCharts';
import RunBacktesting from './vnpy/runBacktesting';
import EventLog from './vnpy/eventLog';



const { Header, Content, Footer, Sider } = Layout;
const { SubMenu } = Menu;

class MainPage extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            collapsed: false,
            tabKey: 1,
        }; 
    }

    onCollapse = collapsed => {
        console.log(collapsed);
        this.setState({ collapsed });
    };

    tabClick = click => {
        console.log(click.key);
        this.setState({
            tabKey: click.key
        });
    }

    render() { 
        const { collapsed } = this.state; 
        return (
        <Layout style={{ minHeight: '100vh' }}>
            <Sider collapsible collapsed={collapsed} onCollapse={this.onCollapse}>
                <div className="logo" />
            <Menu theme="dark" defaultSelectedKeys={['1']} mode="inline">
                <Menu.Item key="1" icon={<UpCircleTwoTone />} onClick={this.tabClick}>
                    Connector
                </Menu.Item>
                <Menu.Item key="2" icon={<DesktopOutlined />} onClick={this.tabClick}>
                    Backtesting
                </Menu.Item>
                <Menu.Item key="3" icon={<DesktopOutlined />} onClick={this.tabClick}>
                    Backtesting Result
                </Menu.Item>
            </Menu>
            </Sider>
            <Layout className="site-layout">
                <Header className="site-layout-background" style={{ padding: 0 }} />
                <Content style={{ margin: '0 16px', display: this.state.tabKey == 1 ? "block" : "none" }}>
                    <CoinbaseAccountConnector />
                    <MarketplaceDataDownload />
                </Content>
                <Content style={{ margin: '0 16px', display: this.state.tabKey == 2 ? "block" : "none"  }}>
                    <RunBacktesting />
                </Content>
                <Content style={{ margin: '0 16px', display: this.state.tabKey == 3 ? "block" : "none"  }}>
                    <BackTesterCharts />
                </Content>
                <Content style={{ margin: '0 16px'}}>
                    <EventLog />
                </Content>
            </Layout>
        </Layout>
        )
    }
}

export default MainPage;