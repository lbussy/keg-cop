<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="9.5.2">
<drawing>
<settings>
<setting alwaysvectorfont="yes"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
<layer number="2" name="Route2" color="1" fill="3" visible="no" active="no"/>
<layer number="3" name="Route3" color="4" fill="3" visible="no" active="no"/>
<layer number="4" name="Route4" color="1" fill="4" visible="no" active="no"/>
<layer number="5" name="Route5" color="4" fill="4" visible="no" active="no"/>
<layer number="6" name="Route6" color="1" fill="8" visible="no" active="no"/>
<layer number="7" name="Route7" color="4" fill="8" visible="no" active="no"/>
<layer number="8" name="Route8" color="1" fill="2" visible="no" active="no"/>
<layer number="9" name="Route9" color="4" fill="2" visible="no" active="no"/>
<layer number="10" name="Route10" color="1" fill="7" visible="no" active="no"/>
<layer number="11" name="Route11" color="4" fill="7" visible="no" active="no"/>
<layer number="12" name="Route12" color="1" fill="5" visible="no" active="no"/>
<layer number="13" name="Route13" color="4" fill="5" visible="no" active="no"/>
<layer number="14" name="Route14" color="1" fill="6" visible="no" active="no"/>
<layer number="15" name="Route15" color="4" fill="6" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="15" fill="1" visible="no" active="no"/>
<layer number="21" name="tPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="22" name="bPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="23" name="tOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="24" name="bOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="25" name="tNames" color="7" fill="1" visible="no" active="no"/>
<layer number="26" name="bNames" color="7" fill="1" visible="no" active="no"/>
<layer number="27" name="tValues" color="7" fill="1" visible="no" active="no"/>
<layer number="28" name="bValues" color="7" fill="1" visible="no" active="no"/>
<layer number="29" name="tStop" color="7" fill="3" visible="no" active="no"/>
<layer number="30" name="bStop" color="7" fill="6" visible="no" active="no"/>
<layer number="31" name="tCream" color="7" fill="4" visible="no" active="no"/>
<layer number="32" name="bCream" color="7" fill="5" visible="no" active="no"/>
<layer number="33" name="tFinish" color="6" fill="3" visible="no" active="no"/>
<layer number="34" name="bFinish" color="6" fill="6" visible="no" active="no"/>
<layer number="35" name="tGlue" color="7" fill="4" visible="no" active="no"/>
<layer number="36" name="bGlue" color="7" fill="5" visible="no" active="no"/>
<layer number="37" name="tTest" color="7" fill="1" visible="no" active="no"/>
<layer number="38" name="bTest" color="7" fill="1" visible="no" active="no"/>
<layer number="39" name="tKeepout" color="4" fill="11" visible="no" active="no"/>
<layer number="40" name="bKeepout" color="1" fill="11" visible="no" active="no"/>
<layer number="41" name="tRestrict" color="4" fill="10" visible="no" active="no"/>
<layer number="42" name="bRestrict" color="1" fill="10" visible="no" active="no"/>
<layer number="43" name="vRestrict" color="2" fill="10" visible="no" active="no"/>
<layer number="44" name="Drills" color="7" fill="1" visible="no" active="no"/>
<layer number="45" name="Holes" color="7" fill="1" visible="no" active="no"/>
<layer number="46" name="Milling" color="3" fill="1" visible="no" active="no"/>
<layer number="47" name="Measures" color="7" fill="1" visible="no" active="no"/>
<layer number="48" name="Document" color="7" fill="1" visible="no" active="no"/>
<layer number="49" name="Reference" color="7" fill="1" visible="no" active="no"/>
<layer number="51" name="tDocu" color="6" fill="1" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="88" name="SimResults" color="9" fill="1" visible="yes" active="yes"/>
<layer number="89" name="SimProbes" color="9" fill="1" visible="yes" active="yes"/>
<layer number="90" name="Modules" color="5" fill="1" visible="yes" active="yes"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="con-tycoelectronics" urn="urn:adsk.eagle:library:193">
<description>&lt;b&gt;Tyco Electronics Connector&lt;/b&gt;&lt;p&gt;
http://catalog.tycoelectronics.com&lt;br&gt;
&lt;author&gt;Created by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
<package name="RJ45-NO-SHIELD" urn="urn:adsk.eagle:footprint:10651/1" library_version="2">
<description>&lt;b&gt;RJ45 Low Profile&lt;/b&gt; No Shield&lt;p&gt;
For all RJ45 N and Z Series Models&lt;br&gt;
Source: www.tycoelectronics.com .. ENG_DS_1654001_1099_RJ_L_0507.pdf</description>
<wire x1="-7.527" y1="10.819" x2="7.527" y2="10.819" width="0.2032" layer="21"/>
<wire x1="8.4455" y1="-5.5118" x2="-8.4455" y2="-5.5118" width="0.01" layer="20"/>
<wire x1="7.527" y1="-7.782" x2="-7.527" y2="-7.782" width="0.2032" layer="21"/>
<wire x1="-7.527" y1="-7.782" x2="-7.527" y2="10.819" width="0.2032" layer="21"/>
<wire x1="7.527" y1="10.819" x2="7.527" y2="-7.782" width="0.2032" layer="21"/>
<pad name="4" x="-0.635" y="8.89" drill="0.9" diameter="1.4"/>
<pad name="3" x="-1.905" y="6.35" drill="0.9" diameter="1.4"/>
<pad name="2" x="-3.175" y="8.89" drill="0.9" diameter="1.4"/>
<pad name="5" x="0.635" y="6.35" drill="0.9" diameter="1.4"/>
<pad name="1" x="-4.445" y="6.35" drill="0.9" diameter="1.4"/>
<pad name="6" x="1.905" y="8.89" drill="0.9" diameter="1.4"/>
<pad name="7" x="3.175" y="6.35" drill="0.9" diameter="1.4"/>
<pad name="8" x="4.445" y="8.89" drill="0.9" diameter="1.4"/>
<text x="-9.525" y="-0.635" size="1.778" layer="25" rot="R90">&gt;NAME</text>
<text x="-5.715" y="2.54" size="1.778" layer="27">&gt;VALUE</text>
<rectangle x1="7.6" y1="-5.485" x2="8.875" y2="-4.342" layer="21"/>
<rectangle x1="-8.875" y1="-5.485" x2="-7.625" y2="-4.342" layer="21"/>
<hole x="-5.715" y="0" drill="3.2512"/>
<hole x="5.715" y="0" drill="3.2512"/>
</package>
</packages>
<packages3d>
<package3d name="RJ45-NO-SHIELD" urn="urn:adsk.eagle:package:10660/1" type="box" library_version="2">
<description>RJ45 Low Profile No Shield
For all RJ45 N and Z Series Models
Source: www.tycoelectronics.com .. ENG_DS_1654001_1099_RJ_L_0507.pdf</description>
<packageinstances>
<packageinstance name="RJ45-NO-SHIELD"/>
</packageinstances>
</package3d>
</packages3d>
<symbols>
<symbol name="JACK8" urn="urn:adsk.eagle:symbol:10650/1" library_version="2">
<wire x1="1.524" y1="8.128" x2="0" y2="8.128" width="0.254" layer="94"/>
<wire x1="0" y1="8.128" x2="0" y2="7.112" width="0.254" layer="94"/>
<wire x1="0" y1="7.112" x2="1.524" y2="7.112" width="0.254" layer="94"/>
<wire x1="1.524" y1="5.588" x2="0" y2="5.588" width="0.254" layer="94"/>
<wire x1="0" y1="5.588" x2="0" y2="4.572" width="0.254" layer="94"/>
<wire x1="0" y1="4.572" x2="1.524" y2="4.572" width="0.254" layer="94"/>
<wire x1="1.524" y1="3.048" x2="0" y2="3.048" width="0.254" layer="94"/>
<wire x1="0" y1="3.048" x2="0" y2="2.032" width="0.254" layer="94"/>
<wire x1="0" y1="2.032" x2="1.524" y2="2.032" width="0.254" layer="94"/>
<wire x1="1.524" y1="0.508" x2="0" y2="0.508" width="0.254" layer="94"/>
<wire x1="0" y1="0.508" x2="0" y2="-0.508" width="0.254" layer="94"/>
<wire x1="0" y1="-0.508" x2="1.524" y2="-0.508" width="0.254" layer="94"/>
<wire x1="1.524" y1="-2.032" x2="0" y2="-2.032" width="0.254" layer="94"/>
<wire x1="0" y1="-2.032" x2="0" y2="-3.048" width="0.254" layer="94"/>
<wire x1="0" y1="-3.048" x2="1.524" y2="-3.048" width="0.254" layer="94"/>
<wire x1="1.524" y1="-4.572" x2="0" y2="-4.572" width="0.254" layer="94"/>
<wire x1="0" y1="-4.572" x2="0" y2="-5.588" width="0.254" layer="94"/>
<wire x1="0" y1="-5.588" x2="1.524" y2="-5.588" width="0.254" layer="94"/>
<wire x1="3.048" y1="-2.032" x2="5.588" y2="-2.032" width="0.1998" layer="94"/>
<wire x1="5.588" y1="-2.032" x2="5.588" y2="-0.254" width="0.1998" layer="94"/>
<wire x1="5.588" y1="-0.254" x2="6.604" y2="-0.254" width="0.1998" layer="94"/>
<wire x1="6.604" y1="-0.254" x2="6.604" y2="1.778" width="0.1998" layer="94"/>
<wire x1="6.604" y1="1.778" x2="5.588" y2="1.778" width="0.1998" layer="94"/>
<wire x1="5.588" y1="1.778" x2="5.588" y2="3.556" width="0.1998" layer="94"/>
<wire x1="5.588" y1="3.556" x2="3.048" y2="3.556" width="0.1998" layer="94"/>
<wire x1="3.048" y1="3.556" x2="3.048" y2="2.54" width="0.1998" layer="94"/>
<wire x1="3.048" y1="2.54" x2="3.048" y2="2.032" width="0.1998" layer="94"/>
<wire x1="3.048" y1="2.032" x2="3.048" y2="1.524" width="0.1998" layer="94"/>
<wire x1="3.048" y1="1.524" x2="3.048" y2="1.016" width="0.1998" layer="94"/>
<wire x1="3.048" y1="1.016" x2="3.048" y2="0.508" width="0.1998" layer="94"/>
<wire x1="3.048" y1="0.508" x2="3.048" y2="0" width="0.1998" layer="94"/>
<wire x1="3.048" y1="0" x2="3.048" y2="-0.508" width="0.1998" layer="94"/>
<wire x1="3.048" y1="-0.508" x2="3.048" y2="-1.016" width="0.1998" layer="94"/>
<wire x1="3.048" y1="-1.016" x2="3.048" y2="-2.032" width="0.1998" layer="94"/>
<wire x1="3.048" y1="2.54" x2="3.81" y2="2.54" width="0.1998" layer="94"/>
<wire x1="3.048" y1="2.032" x2="3.81" y2="2.032" width="0.1998" layer="94"/>
<wire x1="3.048" y1="1.524" x2="3.81" y2="1.524" width="0.1998" layer="94"/>
<wire x1="3.048" y1="1.016" x2="3.81" y2="1.016" width="0.1998" layer="94"/>
<wire x1="3.048" y1="0.508" x2="3.81" y2="0.508" width="0.1998" layer="94"/>
<wire x1="3.048" y1="0" x2="3.81" y2="0" width="0.1998" layer="94"/>
<wire x1="1.524" y1="-7.112" x2="0" y2="-7.112" width="0.254" layer="94"/>
<wire x1="0" y1="-7.112" x2="0" y2="-8.128" width="0.254" layer="94"/>
<wire x1="0" y1="-8.128" x2="1.524" y2="-8.128" width="0.254" layer="94"/>
<wire x1="1.524" y1="-9.652" x2="0" y2="-9.652" width="0.254" layer="94"/>
<wire x1="0" y1="-9.652" x2="0" y2="-10.668" width="0.254" layer="94"/>
<wire x1="0" y1="-10.668" x2="1.524" y2="-10.668" width="0.254" layer="94"/>
<wire x1="3.048" y1="-0.508" x2="3.81" y2="-0.508" width="0.1998" layer="94"/>
<wire x1="3.048" y1="-1.016" x2="3.81" y2="-1.016" width="0.1998" layer="94"/>
<text x="-2.54" y="10.16" size="1.778" layer="95">&gt;NAME</text>
<text x="-2.54" y="-13.208" size="1.778" layer="96">&gt;VALUE</text>
<pin name="1" x="-2.54" y="7.62" visible="pad" length="short" direction="pas" swaplevel="1"/>
<pin name="2" x="-2.54" y="5.08" visible="pad" length="short" direction="pas" swaplevel="1"/>
<pin name="3" x="-2.54" y="2.54" visible="pad" length="short" direction="pas" swaplevel="1"/>
<pin name="4" x="-2.54" y="0" visible="pad" length="short" direction="pas" swaplevel="1"/>
<pin name="5" x="-2.54" y="-2.54" visible="pad" length="short" direction="pas" swaplevel="1"/>
<pin name="6" x="-2.54" y="-5.08" visible="pad" length="short" direction="pas" swaplevel="1"/>
<pin name="7" x="-2.54" y="-7.62" visible="pad" length="short" direction="pas" swaplevel="1"/>
<pin name="8" x="-2.54" y="-10.16" visible="pad" length="short" direction="pas" swaplevel="1"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="RJ45" urn="urn:adsk.eagle:component:10669/2" prefix="X" library_version="2">
<description>&lt;b&gt;CORCOM Modular RJ Jacks&lt;/b&gt; No Shield&lt;p&gt;
Source: www.tycoelectronics.com .. ENG_DS_1654001_1099_RJ_L_0507.pdf</description>
<gates>
<gate name="G$1" symbol="JACK8" x="0" y="0"/>
</gates>
<devices>
<device name="" package="RJ45-NO-SHIELD">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
<connect gate="G$1" pin="3" pad="3"/>
<connect gate="G$1" pin="4" pad="4"/>
<connect gate="G$1" pin="5" pad="5"/>
<connect gate="G$1" pin="6" pad="6"/>
<connect gate="G$1" pin="7" pad="7"/>
<connect gate="G$1" pin="8" pad="8"/>
</connects>
<package3dinstances>
<package3dinstance package3d_urn="urn:adsk.eagle:package:10660/1"/>
</package3dinstances>
<technologies>
<technology name="-6L-B">
<attribute name="MF" value="" constant="no"/>
<attribute name="MPN" value="RJ45-6L-B" constant="no"/>
<attribute name="OC_FARNELL" value="unknown" constant="no"/>
<attribute name="OC_NEWARK" value="16R6090" constant="no"/>
<attribute name="POPULARITY" value="0" constant="no"/>
</technology>
<technology name="-6L-S">
<attribute name="MF" value="" constant="no"/>
<attribute name="MPN" value="RJ45-6L-S" constant="no"/>
<attribute name="OC_FARNELL" value="unknown" constant="no"/>
<attribute name="OC_NEWARK" value="16R6091" constant="no"/>
<attribute name="POPULARITY" value="0" constant="no"/>
</technology>
<technology name="-6X">
<attribute name="MF" value="" constant="no"/>
<attribute name="MPN" value="RJ45-6X" constant="no"/>
<attribute name="OC_FARNELL" value="unknown" constant="no"/>
<attribute name="OC_NEWARK" value="01P1604" constant="no"/>
<attribute name="POPULARITY" value="0" constant="no"/>
</technology>
<technology name="-8L-B">
<attribute name="MF" value="TYCO ELECTRONICS" constant="no"/>
<attribute name="MPN" value="RJ45-8L-B" constant="no"/>
<attribute name="OC_FARNELL" value="1279843" constant="no"/>
<attribute name="OC_NEWARK" value="52K4445" constant="no"/>
<attribute name="POPULARITY" value="0" constant="no"/>
</technology>
<technology name="-8L-S">
<attribute name="MF" value="TYCO ELECTRONICS" constant="no"/>
<attribute name="MPN" value="RJ45-8L-S" constant="no"/>
<attribute name="OC_FARNELL" value="unknown" constant="no"/>
<attribute name="OC_NEWARK" value="80K9067" constant="no"/>
<attribute name="POPULARITY" value="0" constant="no"/>
</technology>
<technology name="-8X">
<attribute name="MF" value="" constant="no"/>
<attribute name="MPN" value="RJ45-8X" constant="no"/>
<attribute name="OC_FARNELL" value="unknown" constant="no"/>
<attribute name="OC_NEWARK" value="16R6101" constant="no"/>
<attribute name="POPULARITY" value="0" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="Donnie">
<packages>
<package name="HDR-3_VERT">
<wire x1="1.27" y1="0.635" x2="1.905" y2="1.27" width="0.1524" layer="21"/>
<wire x1="1.905" y1="1.27" x2="3.175" y2="1.27" width="0.1524" layer="21"/>
<wire x1="3.175" y1="1.27" x2="3.81" y2="0.635" width="0.1524" layer="21"/>
<wire x1="3.81" y1="-0.635" x2="3.175" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="3.175" y1="-1.27" x2="1.905" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="1.905" y1="-1.27" x2="1.27" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="3.81" y1="0.635" x2="4.445" y2="1.27" width="0.1524" layer="21"/>
<wire x1="4.445" y1="1.27" x2="5.715" y2="1.27" width="0.1524" layer="21"/>
<wire x1="5.715" y1="1.27" x2="6.35" y2="0.635" width="0.1524" layer="21"/>
<wire x1="6.35" y1="-0.635" x2="5.715" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="5.715" y1="-1.27" x2="4.445" y2="-1.27" width="0.1524" layer="21"/>
<wire x1="4.445" y1="-1.27" x2="3.81" y2="-0.635" width="0.1524" layer="21"/>
<wire x1="6.35" y1="0.635" x2="6.35" y2="-0.635" width="0.1524" layer="21"/>
<pad name="1" x="0" y="0" drill="1.016" shape="long" rot="R90"/>
<pad name="2" x="2.54" y="0" drill="1.016" shape="long" rot="R90"/>
<pad name="3" x="5.08" y="0" drill="1.016" shape="long" rot="R90"/>
<text x="-1.27" y="1.905" size="1.27" layer="25" ratio="10">&gt;NAME</text>
<text x="-1.27" y="-1.905" size="1.27" layer="27" ratio="10" align="top-left">&gt;VALUE</text>
<rectangle x1="2.286" y1="-0.254" x2="2.794" y2="0.254" layer="51"/>
<rectangle x1="-0.254" y1="-0.254" x2="0.254" y2="0.254" layer="51"/>
<rectangle x1="4.826" y1="-0.254" x2="5.334" y2="0.254" layer="51"/>
<rectangle x1="-1.27" y1="-1.27" x2="1.27" y2="1.27" layer="21"/>
</package>
</packages>
<symbols>
<symbol name="HDR-3">
<wire x1="3.81" y1="-2.54" x2="-1.27" y2="-2.54" width="0.4064" layer="94"/>
<wire x1="1.27" y1="5.08" x2="2.54" y2="5.08" width="0.6096" layer="94"/>
<wire x1="1.27" y1="2.54" x2="2.54" y2="2.54" width="0.6096" layer="94"/>
<wire x1="1.27" y1="0" x2="2.54" y2="0" width="0.6096" layer="94"/>
<wire x1="-1.27" y1="7.62" x2="-1.27" y2="-2.54" width="0.4064" layer="94"/>
<wire x1="3.81" y1="-2.54" x2="3.81" y2="7.62" width="0.4064" layer="94"/>
<wire x1="-1.27" y1="7.62" x2="3.81" y2="7.62" width="0.4064" layer="94"/>
<text x="-1.27" y="-5.08" size="1.778" layer="96">&gt;VALUE</text>
<text x="-1.27" y="8.382" size="1.778" layer="95">&gt;NAME</text>
<pin name="1" x="7.62" y="0" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="2" x="7.62" y="2.54" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="3" x="7.62" y="5.08" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="HDR-3" prefix="P" uservalue="yes">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt;</description>
<gates>
<gate name="G$1" symbol="HDR-3" x="0" y="0"/>
</gates>
<devices>
<device name="" package="HDR-3_VERT">
<connects>
<connect gate="G$1" pin="1" pad="1"/>
<connect gate="G$1" pin="2" pad="2"/>
<connect gate="G$1" pin="3" pad="3"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="frames" urn="urn:adsk.eagle:library:229">
<description>&lt;b&gt;Frames for Sheet and Layout&lt;/b&gt;</description>
<packages>
</packages>
<symbols>
<symbol name="LETTER_L" urn="urn:adsk.eagle:symbol:13870/1" library_version="1">
<frame x1="0" y1="0" x2="248.92" y2="185.42" columns="12" rows="17" layer="94" border-left="no" border-top="no" border-right="no" border-bottom="no"/>
</symbol>
<symbol name="DOCFIELD" urn="urn:adsk.eagle:symbol:13864/1" library_version="1">
<wire x1="0" y1="0" x2="71.12" y2="0" width="0.1016" layer="94"/>
<wire x1="101.6" y1="15.24" x2="87.63" y2="15.24" width="0.1016" layer="94"/>
<wire x1="0" y1="0" x2="0" y2="5.08" width="0.1016" layer="94"/>
<wire x1="0" y1="5.08" x2="71.12" y2="5.08" width="0.1016" layer="94"/>
<wire x1="0" y1="5.08" x2="0" y2="15.24" width="0.1016" layer="94"/>
<wire x1="101.6" y1="15.24" x2="101.6" y2="5.08" width="0.1016" layer="94"/>
<wire x1="71.12" y1="5.08" x2="71.12" y2="0" width="0.1016" layer="94"/>
<wire x1="71.12" y1="5.08" x2="87.63" y2="5.08" width="0.1016" layer="94"/>
<wire x1="71.12" y1="0" x2="101.6" y2="0" width="0.1016" layer="94"/>
<wire x1="87.63" y1="15.24" x2="87.63" y2="5.08" width="0.1016" layer="94"/>
<wire x1="87.63" y1="15.24" x2="0" y2="15.24" width="0.1016" layer="94"/>
<wire x1="87.63" y1="5.08" x2="101.6" y2="5.08" width="0.1016" layer="94"/>
<wire x1="101.6" y1="5.08" x2="101.6" y2="0" width="0.1016" layer="94"/>
<wire x1="0" y1="15.24" x2="0" y2="22.86" width="0.1016" layer="94"/>
<wire x1="101.6" y1="35.56" x2="0" y2="35.56" width="0.1016" layer="94"/>
<wire x1="101.6" y1="35.56" x2="101.6" y2="22.86" width="0.1016" layer="94"/>
<wire x1="0" y1="22.86" x2="101.6" y2="22.86" width="0.1016" layer="94"/>
<wire x1="0" y1="22.86" x2="0" y2="35.56" width="0.1016" layer="94"/>
<wire x1="101.6" y1="22.86" x2="101.6" y2="15.24" width="0.1016" layer="94"/>
<text x="1.27" y="1.27" size="2.54" layer="94">Date:</text>
<text x="12.7" y="1.27" size="2.54" layer="94">&gt;LAST_DATE_TIME</text>
<text x="72.39" y="1.27" size="2.54" layer="94">Sheet:</text>
<text x="86.36" y="1.27" size="2.54" layer="94">&gt;SHEET</text>
<text x="88.9" y="11.43" size="2.54" layer="94">REV:</text>
<text x="1.27" y="19.05" size="2.54" layer="94">TITLE:</text>
<text x="1.27" y="11.43" size="2.54" layer="94">Document Number:</text>
<text x="17.78" y="19.05" size="2.54" layer="94">&gt;DRAWING_NAME</text>
</symbol>
</symbols>
<devicesets>
<deviceset name="LETTER_L" urn="urn:adsk.eagle:component:13920/1" prefix="FRAME" uservalue="yes" library_version="1">
<description>&lt;b&gt;FRAME&lt;/b&gt;&lt;p&gt;
LETTER landscape</description>
<gates>
<gate name="G$1" symbol="LETTER_L" x="0" y="0"/>
<gate name="G$2" symbol="DOCFIELD" x="147.32" y="0" addlevel="must"/>
</gates>
<devices>
<device name="">
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
<part name="SENSORS" library="con-tycoelectronics" library_urn="urn:adsk.eagle:library:193" deviceset="RJ45" device="" package3d_urn="urn:adsk.eagle:package:10660/1" technology="-8X"/>
<part name="FLOW" library="con-tycoelectronics" library_urn="urn:adsk.eagle:library:193" deviceset="RJ45" device="" package3d_urn="urn:adsk.eagle:package:10660/1" technology="-8X"/>
<part name="FIVE_EIGHT" library="con-tycoelectronics" library_urn="urn:adsk.eagle:library:193" deviceset="RJ45" device="" package3d_urn="urn:adsk.eagle:package:10660/1" technology="-8X"/>
<part name="ONE_FOUR" library="con-tycoelectronics" library_urn="urn:adsk.eagle:library:193" deviceset="RJ45" device="" package3d_urn="urn:adsk.eagle:package:10660/1" technology="-8X"/>
<part name="ROOM" library="Donnie" deviceset="HDR-3" device=""/>
<part name="TOWER" library="Donnie" deviceset="HDR-3" device=""/>
<part name="UPPER" library="Donnie" deviceset="HDR-3" device=""/>
<part name="LOWER" library="Donnie" deviceset="HDR-3" device=""/>
<part name="KEG" library="Donnie" deviceset="HDR-3" device=""/>
<part name="FRAME1" library="frames" library_urn="urn:adsk.eagle:library:229" deviceset="LETTER_L" device="" value="Frame"/>
</parts>
<sheets>
<sheet>
<plain>
</plain>
<instances>
<instance part="SENSORS" gate="G$1" x="55.88" y="119.38" smashed="yes" rot="R90">
<attribute name="NAME" x="45.72" y="116.84" size="1.778" layer="95" rot="R90"/>
<attribute name="VALUE" x="69.088" y="116.84" size="1.778" layer="96" rot="R90"/>
</instance>
<instance part="FLOW" gate="G$1" x="99.06" y="119.38" smashed="yes" rot="R90">
<attribute name="NAME" x="88.9" y="116.84" size="1.778" layer="95" rot="R90"/>
<attribute name="VALUE" x="112.268" y="116.84" size="1.778" layer="96" rot="R90"/>
</instance>
<instance part="FIVE_EIGHT" gate="G$1" x="129.54" y="22.86" smashed="yes">
<attribute name="NAME" x="127" y="33.02" size="1.778" layer="95"/>
<attribute name="VALUE" x="127" y="9.652" size="1.778" layer="96"/>
</instance>
<instance part="ONE_FOUR" gate="G$1" x="0" y="20.32" smashed="yes" rot="R180">
<attribute name="NAME" x="2.54" y="10.16" size="1.778" layer="95" rot="R180"/>
<attribute name="VALUE" x="2.54" y="33.528" size="1.778" layer="96" rot="R180"/>
</instance>
<instance part="ROOM" gate="G$1" x="-2.54" y="104.14" smashed="yes">
<attribute name="VALUE" x="-3.81" y="99.06" size="1.778" layer="96"/>
<attribute name="NAME" x="-3.81" y="112.522" size="1.778" layer="95"/>
</instance>
<instance part="TOWER" gate="G$1" x="-2.54" y="88.9" smashed="yes">
<attribute name="VALUE" x="-3.81" y="83.82" size="1.778" layer="96"/>
<attribute name="NAME" x="-3.81" y="97.282" size="1.778" layer="95"/>
</instance>
<instance part="UPPER" gate="G$1" x="-2.54" y="73.66" smashed="yes">
<attribute name="VALUE" x="-3.81" y="68.58" size="1.778" layer="96"/>
<attribute name="NAME" x="-3.81" y="82.042" size="1.778" layer="95"/>
</instance>
<instance part="LOWER" gate="G$1" x="-2.54" y="58.42" smashed="yes">
<attribute name="VALUE" x="-3.81" y="53.34" size="1.778" layer="96"/>
<attribute name="NAME" x="-3.81" y="66.802" size="1.778" layer="95"/>
</instance>
<instance part="KEG" gate="G$1" x="-2.54" y="43.18" smashed="yes">
<attribute name="VALUE" x="-3.81" y="38.1" size="1.778" layer="96"/>
<attribute name="NAME" x="-3.81" y="51.562" size="1.778" layer="95"/>
</instance>
<instance part="FRAME1" gate="G$1" x="-60.96" y="-38.1" smashed="yes"/>
<instance part="FRAME1" gate="G$2" x="86.36" y="-38.1" smashed="yes">
<attribute name="LAST_DATE_TIME" x="99.06" y="-36.83" size="2.54" layer="94"/>
<attribute name="SHEET" x="172.72" y="-36.83" size="2.54" layer="94"/>
<attribute name="DRAWING_NAME" x="104.14" y="-19.05" size="2.54" layer="94"/>
</instance>
</instances>
<busses>
</busses>
<nets>
<net name="KEG1" class="0">
<segment>
<pinref part="ONE_FOUR" gate="G$1" pin="1"/>
<label x="2.54" y="12.7" size="1.778" layer="95"/>
<wire x1="2.54" y1="12.7" x2="91.44" y2="12.7" width="0.1524" layer="91"/>
<pinref part="FLOW" gate="G$1" pin="1"/>
<label x="91.44" y="111.76" size="1.778" layer="95" rot="R90"/>
<wire x1="91.44" y1="12.7" x2="91.44" y2="116.84" width="0.1524" layer="91"/>
</segment>
</net>
<net name="KEG2" class="0">
<segment>
<pinref part="ONE_FOUR" gate="G$1" pin="2"/>
<label x="2.54" y="15.24" size="1.778" layer="95"/>
<wire x1="2.54" y1="15.24" x2="76.2" y2="15.24" width="0.1524" layer="91"/>
<wire x1="76.2" y1="15.24" x2="76.2" y2="25.4" width="0.1524" layer="91"/>
<wire x1="76.2" y1="25.4" x2="93.98" y2="25.4" width="0.1524" layer="91"/>
<pinref part="FLOW" gate="G$1" pin="2"/>
<label x="93.98" y="111.76" size="1.778" layer="95" rot="R90"/>
<wire x1="93.98" y1="25.4" x2="93.98" y2="116.84" width="0.1524" layer="91"/>
</segment>
</net>
<net name="KEG3" class="0">
<segment>
<pinref part="ONE_FOUR" gate="G$1" pin="3"/>
<label x="2.54" y="17.78" size="1.778" layer="95"/>
<wire x1="2.54" y1="17.78" x2="78.74" y2="17.78" width="0.1524" layer="91"/>
<wire x1="78.74" y1="17.78" x2="78.74" y2="22.86" width="0.1524" layer="91"/>
<wire x1="78.74" y1="22.86" x2="96.52" y2="22.86" width="0.1524" layer="91"/>
<pinref part="FLOW" gate="G$1" pin="3"/>
<label x="96.52" y="111.76" size="1.778" layer="95" rot="R90"/>
<wire x1="96.52" y1="22.86" x2="96.52" y2="116.84" width="0.1524" layer="91"/>
</segment>
</net>
<net name="KEG4" class="0">
<segment>
<pinref part="ONE_FOUR" gate="G$1" pin="4"/>
<label x="2.54" y="20.32" size="1.778" layer="95"/>
<wire x1="2.54" y1="20.32" x2="10.16" y2="20.32" width="0.1524" layer="91"/>
<pinref part="FLOW" gate="G$1" pin="4"/>
<label x="99.06" y="111.76" size="1.778" layer="95" rot="R90"/>
<wire x1="10.16" y1="20.32" x2="99.06" y2="20.32" width="0.1524" layer="91"/>
<wire x1="99.06" y1="20.32" x2="99.06" y2="116.84" width="0.1524" layer="91"/>
<pinref part="ONE_FOUR" gate="G$1" pin="5"/>
<wire x1="2.54" y1="22.86" x2="10.16" y2="22.86" width="0.1524" layer="91"/>
<wire x1="10.16" y1="22.86" x2="10.16" y2="20.32" width="0.1524" layer="91"/>
<junction x="10.16" y="20.32"/>
</segment>
</net>
<net name="KEG5" class="0">
<segment>
<pinref part="FLOW" gate="G$1" pin="5"/>
<label x="101.6" y="111.76" size="1.778" layer="95" rot="R90"/>
<wire x1="101.6" y1="30.48" x2="101.6" y2="116.84" width="0.1524" layer="91"/>
<pinref part="FIVE_EIGHT" gate="G$1" pin="1"/>
<wire x1="101.6" y1="30.48" x2="127" y2="30.48" width="0.1524" layer="91"/>
<label x="121.92" y="30.48" size="1.778" layer="95"/>
</segment>
</net>
<net name="KEG6" class="0">
<segment>
<pinref part="FLOW" gate="G$1" pin="6"/>
<label x="104.14" y="111.76" size="1.778" layer="95" rot="R90"/>
<wire x1="104.14" y1="27.94" x2="104.14" y2="116.84" width="0.1524" layer="91"/>
<pinref part="FIVE_EIGHT" gate="G$1" pin="2"/>
<wire x1="104.14" y1="27.94" x2="127" y2="27.94" width="0.1524" layer="91"/>
<label x="121.92" y="27.94" size="1.778" layer="95"/>
</segment>
</net>
<net name="KEG7" class="0">
<segment>
<pinref part="FLOW" gate="G$1" pin="7"/>
<label x="106.68" y="111.76" size="1.778" layer="95" rot="R90"/>
<wire x1="106.68" y1="25.4" x2="106.68" y2="116.84" width="0.1524" layer="91"/>
<pinref part="FIVE_EIGHT" gate="G$1" pin="3"/>
<wire x1="106.68" y1="25.4" x2="127" y2="25.4" width="0.1524" layer="91"/>
<label x="121.92" y="25.4" size="1.778" layer="95"/>
</segment>
</net>
<net name="KEG8" class="0">
<segment>
<pinref part="FLOW" gate="G$1" pin="8"/>
<label x="109.22" y="111.76" size="1.778" layer="95" rot="R90"/>
<wire x1="109.22" y1="22.86" x2="109.22" y2="116.84" width="0.1524" layer="91"/>
<pinref part="FIVE_EIGHT" gate="G$1" pin="4"/>
<wire x1="109.22" y1="22.86" x2="127" y2="22.86" width="0.1524" layer="91"/>
<label x="121.92" y="22.86" size="1.778" layer="95"/>
</segment>
</net>
<net name="5V" class="0">
<segment>
<pinref part="SENSORS" gate="G$1" pin="8"/>
<label x="66.04" y="111.76" size="1.778" layer="95" rot="R90"/>
<wire x1="66.04" y1="116.84" x2="66.04" y2="30.48" width="0.1524" layer="91"/>
<pinref part="ONE_FOUR" gate="G$1" pin="8"/>
<wire x1="2.54" y1="30.48" x2="15.24" y2="30.48" width="0.1524" layer="91"/>
<pinref part="FIVE_EIGHT" gate="G$1" pin="8"/>
<wire x1="15.24" y1="30.48" x2="66.04" y2="30.48" width="0.1524" layer="91"/>
<wire x1="127" y1="12.7" x2="119.38" y2="12.7" width="0.1524" layer="91"/>
<wire x1="119.38" y1="12.7" x2="93.98" y2="12.7" width="0.1524" layer="91"/>
<wire x1="93.98" y1="12.7" x2="93.98" y2="10.16" width="0.1524" layer="91"/>
<wire x1="93.98" y1="10.16" x2="66.04" y2="10.16" width="0.1524" layer="91"/>
<wire x1="66.04" y1="10.16" x2="66.04" y2="30.48" width="0.1524" layer="91"/>
<junction x="66.04" y="30.48"/>
<pinref part="KEG" gate="G$1" pin="2"/>
<wire x1="5.08" y1="45.72" x2="15.24" y2="45.72" width="0.1524" layer="91"/>
<wire x1="15.24" y1="45.72" x2="15.24" y2="60.96" width="0.1524" layer="91"/>
<pinref part="LOWER" gate="G$1" pin="2"/>
<wire x1="15.24" y1="60.96" x2="5.08" y2="60.96" width="0.1524" layer="91"/>
<wire x1="15.24" y1="60.96" x2="15.24" y2="76.2" width="0.1524" layer="91"/>
<junction x="15.24" y="60.96"/>
<pinref part="UPPER" gate="G$1" pin="2"/>
<wire x1="15.24" y1="76.2" x2="5.08" y2="76.2" width="0.1524" layer="91"/>
<wire x1="15.24" y1="76.2" x2="15.24" y2="91.44" width="0.1524" layer="91"/>
<junction x="15.24" y="76.2"/>
<pinref part="TOWER" gate="G$1" pin="2"/>
<wire x1="15.24" y1="91.44" x2="5.08" y2="91.44" width="0.1524" layer="91"/>
<wire x1="15.24" y1="91.44" x2="15.24" y2="106.68" width="0.1524" layer="91"/>
<junction x="15.24" y="91.44"/>
<pinref part="ROOM" gate="G$1" pin="2"/>
<wire x1="15.24" y1="106.68" x2="5.08" y2="106.68" width="0.1524" layer="91"/>
<wire x1="15.24" y1="30.48" x2="15.24" y2="45.72" width="0.1524" layer="91"/>
<junction x="15.24" y="30.48"/>
<junction x="15.24" y="45.72"/>
<label x="5.08" y="106.68" size="1.778" layer="95"/>
<label x="5.08" y="91.44" size="1.778" layer="95"/>
<label x="5.08" y="76.2" size="1.778" layer="95"/>
<label x="5.08" y="60.96" size="1.778" layer="95"/>
<label x="5.08" y="45.72" size="1.778" layer="95"/>
<label x="121.92" y="12.7" size="1.778" layer="95"/>
<pinref part="FIVE_EIGHT" gate="G$1" pin="6"/>
<wire x1="127" y1="17.78" x2="119.38" y2="17.78" width="0.1524" layer="91"/>
<wire x1="119.38" y1="17.78" x2="119.38" y2="12.7" width="0.1524" layer="91"/>
<junction x="119.38" y="12.7"/>
</segment>
</net>
<net name="GND" class="0">
<segment>
<pinref part="SENSORS" gate="G$1" pin="7"/>
<label x="63.5" y="111.76" size="1.778" layer="95" rot="R90"/>
<wire x1="63.5" y1="116.84" x2="63.5" y2="27.94" width="0.1524" layer="91"/>
<pinref part="ONE_FOUR" gate="G$1" pin="7"/>
<wire x1="63.5" y1="27.94" x2="12.7" y2="27.94" width="0.1524" layer="91"/>
<pinref part="FIVE_EIGHT" gate="G$1" pin="7"/>
<wire x1="12.7" y1="27.94" x2="10.16" y2="27.94" width="0.1524" layer="91"/>
<wire x1="10.16" y1="27.94" x2="2.54" y2="27.94" width="0.1524" layer="91"/>
<wire x1="127" y1="15.24" x2="81.28" y2="15.24" width="0.1524" layer="91"/>
<wire x1="81.28" y1="15.24" x2="81.28" y2="27.94" width="0.1524" layer="91"/>
<wire x1="81.28" y1="27.94" x2="63.5" y2="27.94" width="0.1524" layer="91"/>
<junction x="63.5" y="27.94"/>
<pinref part="KEG" gate="G$1" pin="1"/>
<wire x1="5.08" y1="43.18" x2="12.7" y2="43.18" width="0.1524" layer="91"/>
<wire x1="12.7" y1="43.18" x2="12.7" y2="58.42" width="0.1524" layer="91"/>
<pinref part="LOWER" gate="G$1" pin="1"/>
<wire x1="12.7" y1="58.42" x2="5.08" y2="58.42" width="0.1524" layer="91"/>
<wire x1="12.7" y1="58.42" x2="12.7" y2="73.66" width="0.1524" layer="91"/>
<junction x="12.7" y="58.42"/>
<pinref part="UPPER" gate="G$1" pin="1"/>
<wire x1="12.7" y1="73.66" x2="5.08" y2="73.66" width="0.1524" layer="91"/>
<wire x1="12.7" y1="73.66" x2="12.7" y2="88.9" width="0.1524" layer="91"/>
<junction x="12.7" y="73.66"/>
<pinref part="TOWER" gate="G$1" pin="1"/>
<wire x1="12.7" y1="88.9" x2="5.08" y2="88.9" width="0.1524" layer="91"/>
<wire x1="12.7" y1="88.9" x2="12.7" y2="104.14" width="0.1524" layer="91"/>
<junction x="12.7" y="88.9"/>
<pinref part="ROOM" gate="G$1" pin="1"/>
<wire x1="12.7" y1="104.14" x2="5.08" y2="104.14" width="0.1524" layer="91"/>
<wire x1="12.7" y1="27.94" x2="12.7" y2="43.18" width="0.1524" layer="91"/>
<junction x="12.7" y="27.94"/>
<junction x="12.7" y="43.18"/>
<label x="5.08" y="43.18" size="1.778" layer="95"/>
<label x="5.08" y="73.66" size="1.778" layer="95"/>
<label x="5.08" y="58.42" size="1.778" layer="95"/>
<label x="5.08" y="88.9" size="1.778" layer="95"/>
<label x="5.08" y="104.14" size="1.778" layer="95"/>
<label x="121.92" y="15.24" size="1.778" layer="95"/>
<pinref part="ONE_FOUR" gate="G$1" pin="6"/>
<wire x1="2.54" y1="25.4" x2="10.16" y2="25.4" width="0.1524" layer="91"/>
<wire x1="10.16" y1="25.4" x2="10.16" y2="27.94" width="0.1524" layer="91"/>
<junction x="10.16" y="27.94"/>
</segment>
</net>
<net name="KDAT" class="0">
<segment>
<pinref part="SENSORS" gate="G$1" pin="5"/>
<label x="58.42" y="111.76" size="1.778" layer="95" rot="R90"/>
<pinref part="KEG" gate="G$1" pin="3"/>
<wire x1="5.08" y1="48.26" x2="58.42" y2="48.26" width="0.1524" layer="91"/>
<wire x1="58.42" y1="48.26" x2="58.42" y2="116.84" width="0.1524" layer="91"/>
<label x="5.08" y="48.26" size="1.778" layer="95"/>
</segment>
</net>
<net name="LDAT" class="0">
<segment>
<pinref part="SENSORS" gate="G$1" pin="4"/>
<label x="55.88" y="111.76" size="1.778" layer="95" rot="R90"/>
<pinref part="LOWER" gate="G$1" pin="3"/>
<wire x1="5.08" y1="63.5" x2="55.88" y2="63.5" width="0.1524" layer="91"/>
<wire x1="55.88" y1="63.5" x2="55.88" y2="116.84" width="0.1524" layer="91"/>
<label x="5.08" y="63.5" size="1.778" layer="95"/>
</segment>
</net>
<net name="UDAT" class="0">
<segment>
<pinref part="SENSORS" gate="G$1" pin="3"/>
<label x="53.34" y="111.76" size="1.778" layer="95" rot="R90"/>
<pinref part="UPPER" gate="G$1" pin="3"/>
<wire x1="5.08" y1="78.74" x2="53.34" y2="78.74" width="0.1524" layer="91"/>
<wire x1="53.34" y1="78.74" x2="53.34" y2="116.84" width="0.1524" layer="91"/>
<label x="5.08" y="78.74" size="1.778" layer="95"/>
</segment>
</net>
<net name="TDAT" class="0">
<segment>
<pinref part="SENSORS" gate="G$1" pin="2"/>
<label x="50.8" y="111.76" size="1.778" layer="95" rot="R90"/>
<pinref part="TOWER" gate="G$1" pin="3"/>
<wire x1="5.08" y1="93.98" x2="50.8" y2="93.98" width="0.1524" layer="91"/>
<wire x1="50.8" y1="93.98" x2="50.8" y2="116.84" width="0.1524" layer="91"/>
<label x="5.08" y="93.98" size="1.778" layer="95"/>
</segment>
</net>
<net name="RDAT" class="0">
<segment>
<pinref part="SENSORS" gate="G$1" pin="1"/>
<wire x1="48.26" y1="116.84" x2="48.26" y2="109.22" width="0.1524" layer="91"/>
<label x="48.26" y="111.76" size="1.778" layer="95" rot="R90"/>
<pinref part="ROOM" gate="G$1" pin="3"/>
<wire x1="5.08" y1="109.22" x2="48.26" y2="109.22" width="0.1524" layer="91"/>
<label x="5.08" y="109.22" size="1.778" layer="95"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
<compatibility>
<note version="8.2" severity="warning">
Since Version 8.2, EAGLE supports online libraries. The ids
of those online libraries will not be understood (or retained)
with this version.
</note>
<note version="8.3" severity="warning">
Since Version 8.3, EAGLE supports URNs for individual library
assets (packages, symbols, and devices). The URNs of those assets
will not be understood (or retained) with this version.
</note>
<note version="8.3" severity="warning">
Since Version 8.3, EAGLE supports the association of 3D packages
with devices in libraries, schematics, and board files. Those 3D
packages will not be understood (or retained) with this version.
</note>
</compatibility>
</eagle>
