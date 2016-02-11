[Domain]
Description=Domain UniqueID "country_boundaries.mps"
Time=1319278384
Version=3.1
Class=Domain UniqueID
Width=13
Type=DomainUniqueID
[Ilwis]
ContainsDom=True
Description=All countires boundaries of the world (2011)
Time=1319278458
Version=3.1
Class=ILWIS::Segment Map
Type=BaseMap
[DomainSort]
Sorting=AlphaNumeric
Prefix=Boundary
[DomainIdentifier]
Nr=1618
[BaseMap]
CoordSystem=LatlonWGS84.csy
CoordBounds=-180 -89.99889902136 180 83.599600328293
Domain=country_boundaries.mps
DomainInfo=country_boundaries.mps;Long;UniqueID;0;;
HistogramSize=0
Type=SegmentMap
Proximity=0.867992496748
[SegmentMap]
Snap Distance=3.600000000000
Tunnel Width=0.360000000000
Type=SegmentMapStore
[SegmentMapStore]
Status=-32767
Format=2
Segments=1629
DeletedSegments=0
Coordinates=0
[Table]
Description=
Time=1319296612
Version=3.1
Class=Table
Domain=None.dom
DomainInfo=none.dom;Byte;none;0;;
Columns=5
Records=1629
Type=TableStore
[TableStore]
Data=country_boundaries.mps#
UseAs=No
Col0=MinCoords
Col1=MaxCoords
Col2=Coords
Col3=Deleted
Col4=SegmentValue
Type=TableBinary
[Col:MinCoords]
Description=
Time=1319296610
Version=3.1
Class=Column
Domain=LatlonWGS84.csy
DomainInfo=LatlonWGS84.csy;Coord;coord;0;;
ReadOnly=No
OwnedByTable=Yes
StoreType=Coord
Stored=Yes
Type=ColumnStore
[Col:MaxCoords]
Description=
Time=1319296610
Version=3.1
Class=Column
Domain=LatlonWGS84.csy
DomainInfo=LatlonWGS84.csy;Coord;coord;0;;
ReadOnly=No
OwnedByTable=Yes
StoreType=Coord
Stored=Yes
Type=ColumnStore
[Col:Coords]
Description=
Time=1319296610
Version=3.1
Class=Column
Domain=CoordBuf.dom
DomainInfo=CoordBuf.dom;?;coordbuf;0;;
ReadOnly=No
OwnedByTable=No
StoreType=CoordBuf
Stored=Yes
Type=ColumnStore
[Col:Deleted]
Description=
Time=1319296610
Version=3.1
Class=Column
Domain=bool.dom
DomainInfo=bool.dom;Byte;bool;0;;
Range=0:1:offset=-1
ReadOnly=No
OwnedByTable=No
Sum=?
Mean=?
StdDev=?
StoreType=Long
Stored=Yes
Type=ColumnStore
[Col:SegmentValue]
Description=
Time=1319296610
Version=3.1
Class=Column
Domain=country_boundaries.mps
DomainInfo=country_boundaries.mps;Long;UniqueID;0;;
ReadOnly=No
OwnedByTable=No
StoreType=Long
Stored=Yes
Type=ColumnStore
