<?xml version="1.0" encoding="UTF-8"?>
<qapl resolution="16" theme="0">
    <chart font="Sans Serif,12,-1,5,400,0,0,0,0,0,0,0,0,0,0,1" colour="#ffaaff00">
        <axes colour="#ff55ff00">
            <label font="Source Code Pro ExtraLight,12,-1,5,400,0,0,0,0,0,0,0,0,0,0,1" colour="#ffff5500"/>
            <title font="PT Sans Caption,12,-1,5,400,0,0,0,0,0,0,0,0,0,0,1" colour="#ff0055ff"/>
        </axes>
    </chart>
    <title>Lissajous</title>
    <index></index>
    <xlabel>θ</xlabel>
    <ylabel>val</ylabel>
    <range>
        <initial>0j0</initial>
        <final>6.28319j0</final>
    </range>
    <active aspect="0" mode="1" marker="15">
        <Xexpression>cos %1</Xexpression>
        <Yexpression>sin 2.7×%1</Yexpression>
        <label>cos θ by sin 2.7θ</label>
        <pen colour="#ff000000" style="1" width="1"/>
    </active>
</qapl>
