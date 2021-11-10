<?xml version="1.0" encoding="UTF-8"?>
<qapl resolution="128" theme="3" polar="0">
    <chart font="Padauk,12,-1,5,400,0,0,0,0,0,0,0,0,0,0,1" colour="#ffff00ff">
        <axes colour="#ffffff7f">
            <label font="Source Code Pro ExtraLight,12,-1,5,400,0,0,0,0,0,0,0,0,0,0,1" colour="#ff000000"/>
            <title font="PT Sans Caption,12,-1,5,400,0,0,0,0,0,0,0,0,0,0,1" colour="#ffff55ff"/>
        </axes>
    </chart>
    <title>Euler helix, *0j1×ω, flat projections, 0-0j3π</title>
    <index></index>
    <xlabel>ω</xlabel>
    <ylabel>value</ylabel>
    <range>
        <initial>0j0</initial>
        <final>9.42478j0</final>
    </range>
    <active aspect="0" mode="1" marker="7">
        <Xexpression></Xexpression>
        <Yexpression>* 0j1×%1</Yexpression>
        <label>Real</label>
        <pen colour="#ffaa5500" style="1" width="2"/>
    </active>
    <stack index="0" aspect="3" mode="1">
        <Xexpression></Xexpression>
        <Yexpression>* 0j1×%1</Yexpression>
        <label>Phase</label>
        <pen colour="#ffff0000" style="1" width="2"/>
    </stack>
    <stack index="1" aspect="1" mode="1">
        <Xexpression></Xexpression>
        <Yexpression>* 0j1×%1</Yexpression>
        <label>Imaginary</label>
        <pen colour="#ff00aa00" style="1" width="2"/>
    </stack>
    <stack index="2" aspect="2" mode="1">
        <Xexpression></Xexpression>
        <Yexpression>* 0j1×%1</Yexpression>
        <label>Magnitude</label>
        <pen colour="#ff5555ff" style="1" width="2"/>
    </stack>
</qapl>
