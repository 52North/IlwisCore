#!/usr/bin python
# -*- coding: utf-8 -*-
from concurrent.futures.process import _WorkItem
from unittest.case import skip

try:
    from ilwis import *
    from datetime import *
    from math import *
    import gc

    workingDir = "file:///E:/pytest"
    babyDir = "/baby"
    exampleDir = "/example"
    worldDir = "/world"
    tempDir = "/temp"
    featureDir = "/feature"
    rasterDir = "/raster"
    integrationDir = "/integration"
    spreadDir = "/spread"

    import unittest as ut

    # @ut.skip("temporarily")
    class TestTable(ut.TestCase):
        def setUp(self):
            try:
                disconnectIssueLogger()
                Engine.setWorkingCatalog(workingDir + featureDir)
                connectIssueLogger()
            except IlwisException:
                connectIssueLogger()
                self.skipTest("could not set working directory!")

        def test_AttributeTable(self):
            fc = FeatureCoverage("rainfall.shp")
            t = Table("rainfall.shp")
            self.assertEqual(t.name(), "rainfall.shp")
            self.assertEqual(
                ('RAINFALL', 'JANUARY', 'FEBRUARY', 'MARCH', 'APRIL', 'MAY', 'JUNE', 'JULY', 'AUGUST',
                 'SEPTEMBER', 'OCTOBER', 'NOVEMBER', 'DECEMBER', 'NEWCOL', 'IDENT', 'sum'),
                t.columns()
            )
            self.assertEqual(Const.iUNDEF, t.columnIndex("unknownColumn"))
            self.assertEqual(10, t.columnIndex("OCTOBER"))
            self.assertTupleEqual((48, 46, 86, 89, 44, 40, 44, 85, 89, 0, 0, 0, 0, 12, 42), t.column("OCTOBER"))
            self.assertTupleEqual((48, 46, 86, 89, 44, 40, 44, 85, 89, 0, 0, 0, 0, 12, 42), t.column(10))
            tup = ('Laguna_Santa_Rosa', 175, 165, 160, 78, 54, 35, 16, 4, 20, 86, 173, 181, 340, 2)
            rec = t.record(2)
            self.assertTrue(all((rec[i] == tup[i] for i in range(len(tup)))))
            self.assertEqual(len(rec), len(t.columns()))

        def test_StandaloneIlwis3Table(self):
            t = Table("rainfall.tbt")
            self.assertEqual("rainfall.tbt", t.name())
            self.assertEqual(12, t.recordCount())
            self.assertEqual(15, t.columnCount())
            t.addColumn("newColumn", "value")
            self.assertEqual(16, t.columnCount())
            self.assertEqual(
                ('january', 'february', 'march', 'april', 'may', 'june', 'july', 'august', 'september', 'october',
                 'november', 'december', 'newcol', 'ident', 'rainfall', 'newColumn'),
                t.columns()
            )
            self.assertEqual((4, 5, 6), t.select("march < 100 AND march != 87"))
            self.assertEqual((9, 10, 11), t.select("march == ?"))
            self.assertEqual(13, t.columnIndex("ident"))
            self.assertEqual(81, t.cell("march", 4))
            self.assertEqual(2, t.columnIndex("march"))
            self.assertEqual([81, 76, 79], [t.cell(2, rec) for rec in t.select("march < 100 AND march != 87")])
            self.assertEqual(Const.rUNDEF, t.cell("newColumn", 0))
            t.setCell("newColumn", 0, 32)
            self.assertEqual(32, t.cell("newColumn", 0))
            self.assertTupleEqual((87, 87, 160, 150, 81, 76, 79, 155, 160, -1e+308, -1e+308, -1e+308),
                                  t.column("march"))
            self.assertTupleEqual((87, 87, 160, 150, 81, 76, 79, 155, 160, -1e+308, -1e+308, -1e+308), t.column(2))
            self.assertTupleEqual((175, 165, 160, 78, 54, 35, 16, 4, 20, 86, 173, 181, 340, 2, 2, -1e+308), t.record(2))

        def test_StandaloneGdalTable(self):
            t = Table("rainfall.shp")
            self.assertTrue(bool(t))
            self.assertFalse(t.isInternal(), msg="created a new table object with that name!!")
            self.assertEqual(t.name(), "rainfall.shp")
            expected = ('RAINFALL', 'JANUARYc', 'FEBRUARY', 'MARCHc', 'APRIL', 'MAY', 'JUNE', 'JULY', 'AUGUST',
                        'SEPTEMBER', 'OCTOBER', 'NOVEMBER', 'DECEMBER', 'NEWCOL', 'IDENT', 'sum')
            actual = t.columns()
            self.assertEqual(expected, actual)
            fc = FeatureCoverage("rainfall.shp")
            self.assertTrue(bool(fc))
            self.assertFalse(fc.isInternal(), msg="created a new table object with that name!!")
            self.assertEqual(
                ('RAINFALL', 'JANUARYc', 'FEBRUARY', 'MARCHc', 'APRIL', 'MAY', 'JUNE', 'JULY', 'AUGUST',
                 'SEPTEMBER', 'OCTOBER', 'NOVEMBER', 'DECEMBER', 'NEWCOL', 'IDENT', 'sum'),
                fc.attributeTable().columns()
            )

        def test_ColumnDefinition(self):
            fc = FeatureCoverage("rainfall.shp")
            for feat in fc:
                coldef = feat.attributeDefinition(1)
                coldef2 = feat.attributeDefinition("MARCH")

            tab = fc.attributeTable()
            self.assertEqual(str(tab.columnDefinition(1)), str(coldef))
            self.assertEqual(str(tab.columnDefinition("MARCH")), str(coldef2))

            numRan = NumericRange(0.0, 500.0)
            numDom = NumericDomain()
            numDom.setRange(numRan)
            datDef = DataDefinition(numDom)
            colDef = ColumnDefinition("MARCHc", datDef, 3)

            tab.setColumnDefinition(3, colDef)
            self.assertNotEqual(str(tab.columnDefinition(3)), str(coldef2))

            colDef2 = ColumnDefinition("JANUARYc", datDef, 1)
            tab.setColumnDefinition("JANUARY", colDef2)
            self.assertNotEqual(str(tab.columnDefinition(1)), str(coldef))

        def test_newColumn(self):
            tab = Table("rainfall.shp")
            before = tab.columnCount()

            txtDom = TextDomain()
            datdef = DataDefinition(txtDom)
            coldef = ColumnDefinition("testText", datdef, tab.columnCount())
            #tab.addColumn(coldef)
            #self.assertEqual(tab.columnCount(), before+1)
            #tab.setCell(tab.columnCount()-1, 1, "new Cell")
            #self.assertEqual(tab.cell(tab.columnCount()-1, 1), "new Cell")

    #@ut.skip("temporarily")
    class TestGeometry(ut.TestCase):
        def setUp(self):
            self.csy = CoordinateSystem("code=epsg:5464")

        def tearDown(self):
            del self.csy

        def test_Geometry(self):
            g = Geometry("POINT(5.4 6 9.0)", self.csy)
            self.assertEqual(str(g), "POINT (5.4000000000000004 6.0000000000000000)",
                             "standalone Geometry(fromWKT) failed!")
            self.assertEqual(g.toWKT(), "POINT (5.4000000000000004 6.0000000000000000)",
                             "standalone Geometry(fromWKT) failed!")
            self.assertTrue(bool(g))
            self.assertEqual(g.ilwisType(), 1)
            self.assertEqual(g.coordinateSystem().name(), "Sibun Gorge 1922")
            self.assertEqual(g.coordinateSystem(), self.csy)

        def test_Transform(self):
            pm = CoordinateSystem("code=epsg:3857")
            self.assertEqual("WGS 84 / Pseudo-Mercator", pm.name())
            wgs = CoordinateSystem("code=epsg:4326")
            self.assertEqual("LatLon WGS 84", wgs.name())
            gk2 = CoordinateSystem("code=epsg:31466")
            self.assertEqual("DHDN / 3-degree Gauss-Kruger zone 2", gk2.name())
            gk5 = CoordinateSystem("code=epsg:3329")
            self.assertEqual("Pulkovo 1942(58) / 3-degree Gauss-Kruger zone 5", gk5.name())
            etrs_laea = CoordinateSystem("code=epsg:3035")
            self.assertEqual("ETRS89 / LAEA Europe", etrs_laea.name())
            proj4 = CoordinateSystem("code=proj4:+proj=utm +zone=35 +ellps=intl +towgs84=-87,-98,-121,0,0,0,0 +units=m +no_defs")

            g = Geometry("POINT(766489.647 6840642.671)", pm)
            g1 = g.transform(gk5)
            self.assertEqual("POINT (4945949.0809892630204558 5819418.3890127958729863)", g1.toWKT())
            g2 = g1.transform(wgs)
            self.assertEqual("POINT (6.8854937241464658 52.2235232302096932)", g2.toWKT())
            g1 = g2.transform(gk5)
            self.assertEqual("POINT (4945949.0857109790667892 5819418.3856139155104756)", g1.toWKT())
            g1 = g2.transform(gk2)
            self.assertEqual("POINT (2560502.5977886635810137 5787988.6849722778424621)", g1.toWKT())
            g1 = g2.transform(etrs_laea)
            self.assertEqual("POINT (4108242.5654639988206327 3239420.9621137753129005)", g1.toWKT())
            g3 = g.transform(proj4)
            self.assertEqual("POINT (-866479.8875676055904478 5979085.6121951946988702)", g3.toWKT())

        def test_Envelope(self):
            g = Geometry("POLYGON((1 1,1 10,10 10,10 1,1 1))", self.csy)
            e = g.envelope()
            self.assertEqual("1.000000 1.000000 10.000000 10.000000", str(e))
            g = Geometry("POINT(1 1 1)", self.csy)
            e = g.envelope()
            self.assertEqual("1.000000 1.000000 1.000000 1.000000", str(e),
                             msg="z's are always 0, since boost can only manage 2D geometries until now")
            g = Geometry("POINT(766489.647 6840642.671)", self.csy)
            self.assertEqual("766489.647000 6840642.671000 766489.647000 6840642.671000", str(g.envelope()))
            self.assertEqual("POINT (766489.6469999999972060 6840642.6710000000894070)", str(g))

        def test_BadWKT(self):
            g = True
            with self.assertRaises(SyntaxError):
                g = Geometry("dgjfdkgkjd", self.csy)
            self.assertTrue(bool(g))
            g = Geometry("POINT EMPTY", self.csy)
            self.assertTrue(bool(g))
            g.fromWKT("POINT(5 5)")
            self.assertTrue(bool(g))
            with self.assertRaises(SyntaxError):
                g.fromWKT("fdsfsds")


        def test_SimpleFeatures(self):
            p = Geometry("POLYGON((1 1,1 10,10 10,10 1,1 1))", self.csy)
            self.assertEqual(str(p), "POLYGON ((1.0000000000000000 1.0000000000000000, 1.0000000000000000 \
10.0000000000000000, 10.0000000000000000 10.0000000000000000, 10.0000000000000000 1.0000000000000000, \
1.0000000000000000 1.0000000000000000))")
            self.assertTrue(bool(p))
            pin = Geometry("POINT(5 5)", self.csy)
            self.assertEqual(str(pin), "POINT (5.0000000000000000 5.0000000000000000)")
            self.assertTrue(bool(pin))
            self.assertTrue(p.contains(pin))
            self.assertTrue(pin.within(p))
            geom = Geometry("POLYGON((1 1,1 10,10 10,10 1,1 1))", self.csy)
            self.assertTrue(p.isSimple())
            self.assertTrue(p.within(geom))
            self.assertTrue(p.contains(geom))
            self.assertFalse(p.disjoint(geom))
            self.assertFalse(p.touches(geom))
            self.assertTrue(p.intersects(geom))
            self.assertFalse(p.crosses(geom))
            self.assertFalse(p.overlaps(geom))
            self.assertTrue(p.equals(geom))
            self.assertTrue(p.equalsExact(geom, 0))
            self.assertTrue(p.covers(geom))
            self.assertTrue(p.coveredBy(geom))
            self.assertFalse(p.relate(geom, "T*T***T**"))  # overlaps
            self.assertTrue(p.relate(geom, "T*F**F***"))  # within
            self.assertEqual(p.distance(geom), 0)
            self.assertEqual(p.getArea(), 81)
            self.assertEqual(p.getLength(), 36)
            self.assertTrue(p.isWithinDistance(geom, 0))
            self.assertEqual(
                "POLYGON ((1.0000000000000000 0.0000000000000000, 0.8049096779838661 0.0192147195967707, \
0.6173165676349055 0.0761204674887151, 0.4444297669803942 0.1685303876974572, 0.2928932188134502 0.2928932188134548, \
0.1685303876974533 0.4444297669803999, 0.0761204674887127 0.6173165676349116, 0.0192147195967693 0.8049096779838725, \
0.0000000000000000 1.0000000000000000, 0.0000000000000000 10.0000000000000000, 0.0192147195967696 10.1950903220161280, \
0.0761204674887133 10.3826834323650896, 0.1685303876974547 10.5555702330196013, 0.2928932188134525 10.7071067811865479, \
0.4444297669803980 10.8314696123025449, 0.6173165676349103 10.9238795325112861, 0.8049096779838718 10.9807852804032304, \
1.0000000000000000 11.0000000000000000, 10.0000000000000000 11.0000000000000000, 10.1950903220161280 10.9807852804032304, \
10.3826834323650896 10.9238795325112861, 10.5555702330196031 10.8314696123025449, 10.7071067811865479 10.7071067811865479, \
10.8314696123025449 10.5555702330196013, 10.9238795325112861 10.3826834323650896, 10.9807852804032304 10.1950903220161280, \
11.0000000000000000 10.0000000000000000, 11.0000000000000000 1.0000000000000000, 10.9807852804032304 0.8049096779838718, \
10.9238795325112861 0.6173165676349102, 10.8314696123025449 0.4444297669803978, 10.7071067811865479 0.2928932188134525, \
10.5555702330196031 0.1685303876974548, 10.3826834323650896 0.0761204674887133, 10.1950903220161280 0.0192147195967696, \
10.0000000000000000 0.0000000000000000, 1.0000000000000000 0.0000000000000000))",
                str(p.buffer(1)))
            self.assertEqual("POLYGON ((1.0000000000000000 1.0000000000000000, 1.0000000000000000 \
10.0000000000000000, 10.0000000000000000 10.0000000000000000, 10.0000000000000000 1.0000000000000000, \
1.0000000000000000 1.0000000000000000))", str(p.convexHull()))
            self.assertEqual("POLYGON ((1.0000000000000000 1.0000000000000000, 1.0000000000000000 \
10.0000000000000000, 10.0000000000000000 10.0000000000000000, 10.0000000000000000 1.0000000000000000, \
1.0000000000000000 1.0000000000000000))", str(p.intersection(geom)))
            self.assertEqual("POLYGON ((1.0000000000000000 1.0000000000000000, 1.0000000000000000 \
10.0000000000000000, 10.0000000000000000 10.0000000000000000, 10.0000000000000000 1.0000000000000000, \
1.0000000000000000 1.0000000000000000))", str(p.Union(geom)))
            self.assertEqual("GEOMETRYCOLLECTION EMPTY", str(p.difference(geom)))
            self.assertEqual("GEOMETRYCOLLECTION EMPTY", str(p.symDifference(geom)))

    #@ut.skip("temporarily")
    class TestUtil(ut.TestCase):
        def test_PixelCoordinate(self):
            p = Pixel(4, 5)
            self.assertTrue(bool(p))
            self.assertEqual(str(p), "pixel(4,5)")
            p *= 3
            self.assertTrue(p == Pixel(12, 15))
            self.assertEqual(str(p), "pixel(12,15)")
            p /= 4
            self.assertEqual(str(p), "pixel(3,3)")
            self.assertTrue(p != Pixel(3, 4))
            self.assertFalse(p.is3D())
            v = Pixel(4, 5, 6)
            self.assertEqual(str(v), "pixel(4,5,6)")
            self.assertTrue(v.is3D())
            self.assertEqual(v.x, 4)
            v.x = 32
            self.assertEqual(v.x, 32)
            v.y = 32
            self.assertEqual(v.y, 32)
            v.z = 32
            self.assertEqual(v.z, 32)
            self.assertEqual(str(v), "pixel(32,32,32)")

            c = Coordinate(-1871900.47, 1874237.55)
            self.assertTrue(bool(c))
            self.assertFalse(c.is3D())
            self.assertEqual(str(c), "coordinate(-1871900.470000,1874237.550000)")
            c *= 6
            self.assertTrue(c == Coordinate(-11231402.820000, 11245425.300000))
            self.assertEqual(str(c), "coordinate(-11231402.820000,11245425.300000)")
            c /= 6
            self.assertEqual(str(c), "coordinate(-1871900.470000,1874237.550000)")
            self.assertFalse(c.is3D())
            c = Coordinate(-1871900.47, 1874237.55, 2)
            self.assertTrue(c.is3D())
            self.assertEqual(str(c), "coordinate(-1871900.470000,1874237.550000,2.000000)")
            self.assertEqual(c.x, -1871900.47)
            c.x = 4.323
            self.assertEqual(c.x, 4.323)


        #@ut.skip("temporarily")
        def test_SizeSizeD(self):
            sz = Size(2, 4, 5)
            self.assertTrue(bool(sz))
            self.assertEqual(str(sz), "Size(2, 4, 5)")
            self.assertEqual(sz.linearSize(), 2 * 4 * 5)
            sz *= 2
            self.assertEqual(str(sz), "Size(4, 8, 10)")
            sz += Size(4, 4, 0)
            self.assertTrue(sz == Size(8, 12, 10))
            sz -= Size(1, 1, 0)
            self.assertTrue(sz == Size(7, 11, 10))
            sz.xsize = 3
            self.assertEqual(sz.xsize, 3)
            self.assertTrue(Pixel(1, 1, 1) in sz)
            self.assertFalse(Pixel(5, 5, 5) in sz)
            sz = SizeD(sz)
            self.assertEqual(str(sz), "Size(3, 11, 10)")
            self.assertTrue(sz == SizeD(3., 11., 10.))
            with self.assertRaises(TypeError, msg="Size and SizeD should be incomparable!"):
                sz == Size(3, 11, 10)
            self.assertTrue(Pixel(2, 10, 9) in sz)
            self.assertTrue(PixelD(2., 10., 9.) in sz)
            self.assertTrue(Coordinate(2., 10., 9.) in sz)
            self.assertEqual(str(sz), "Size(3, 11, 10)")
            sz -= SizeD(100., 100.)  # == SizeD(100., 100., 1.)
            self.assertEqual(str(sz), "Size(0, 0, 9)")
            self.assertFalse(bool(sz))
            self.assertFalse(bool(SizeD(10, 10, 0)))
            sz -= SizeD(10, 10, 0)  # calculation with invalid sz == Size(0, 0, 9) results in Size(0, 0, 0)
            self.assertEqual(str(sz), "Size(0, 0, 0)")
            sz = SizeD(2.5, 4.4, 5.1)
            self.assertTrue(sz == SizeD(2.5, 4.4, 5.1))
            self.assertEqual(str(sz), "Size(2.5, 4.4, 5.1)")
            self.assertEqual(sz.linearSize(), int(2.5) * int(4.4) * int(5.1))
            sz *= 2
            self.assertEqual(str(sz), "Size(5, 8.8, 10.2)")
            sz = Size(32, 43)
            self.assertEqual(str(sz), "Size(32, 43, 1)")

        ##@ut.skip("temporarily")
        def test_BoxEnvelope(self):
            b = Box(Pixel(3, 4, 5), Pixel(4, 5, 6, ))
            self.assertTrue(bool(b))
            self.assertTrue(b.is3D())
            self.assertEqual(str(b), "3 4 5 4 5 6")
            self.assertTrue(b == Box("POLYGON(3 4 5,4 5 6)"))
            self.assertEqual(str(b.minCorner()), "pixel(3,4,5)")
            b.minCorner().x = 39
            self.assertEqual(str(b.minCorner()), "pixel(3,4,5)")
            self.assertTrue(b.minCorner() == Pixel(3, 4, 5))
            self.assertEqual(str(b.maxCorner()), "pixel(4,5,6)")
            self.assertEqual(str(b.size()), "Size(2, 2, 2)")
            self.assertTrue(b.size() == Size(2, 2, 2))
            self.assertEqual(b.size().linearSize(), 2 * 2 * 2)
            b = Box(Pixel(2, 3), Pixel(4, 5))
            self.assertEqual(str(b), "2 3 4 5")
            self.assertFalse(b.is3D())
            self.assertTrue(bool(b))

            env = Envelope(Coordinate(3.6111119, 4.7, 5.9), Coordinate(4.7, 5.8, 6.9))
            self.assertEqual(str(env), "3.611112 4.700000 5.900000 4.700000 5.800000 6.900000")
            self.assertEqual(str(env.size()), "Size(2.08889, 2.1, 2)")
            env = Envelope("POLYGON(3.6111119 4.7 5.9,4.7 5.8 6.9)")
            self.assertEqual(str(env), "3.611112 4.700000 5.900000 4.700000 5.800000 6.900000")
            self.assertEqual(str(env.size()), "Size(2.08889, 2.1, 2)")
            self.assertFalse(env.size() == SizeD(2.08889, 2.1, 2.))  # bug on Python float precision
            env = Envelope(env.size())
            self.assertEqual("0.000000 0.000000 0.000000 1.088888 1.100000 1.000000", str(env))
            env = Envelope(Coordinate(3, 4, 5), Coordinate(4, 5, 6))
            self.assertEqual(str(env), "3.000000 4.000000 5.000000 4.000000 5.000000 6.000000")
            env = Envelope(env.size())
            self.assertEqual(str(env), "0.000000 0.000000 0.000000 1.000000 1.000000 1.000000")
            self.assertTrue(Coordinate(.5, .5, .5) in env)
            self.assertFalse(Coordinate(1.5, -0.5, 0.5) in env)
            self.assertTrue(env in env)

    #@ut.skip("temporarily")
    class TestModule(ut.TestCase):
        def test_Exceptions(self):
            with self.assertRaises(IlwisException):
                raise IlwisException("test exception throwing")
            with self.assertRaises(InvalidObjectException):
                raise InvalidObjectException("test exception throwing")
            try:
                raise IlwisException("test exception throwing")
            except IlwisException as ex:
                self.assertEqual(str(ex), "test exception throwing")

        def test_IssueLogger(self):
            disconnectIssueLogger()
            fc = FeatureCoverage(workingDir + "/noneexistentDir/nonexistent.file")
            self.assertFalse(bool(fc))
            #with self.assertRaises(IlwisException, msg="Cann't find suitable factory for nonexistent.file "):
            #    fc = FeatureCoverage(workingDir + "/noneexistentDir/nonexistent.file")
            connectIssueLogger()

        def test_ilwisTypes(self):
            self.assertEqual(7, it.FEATURE)
            self.assertTrue((it.FEATURE & 4) != 0)
            self.assertFalse((it.CATALOG & it.FEATURE) != 0)
            self.assertEqual(4, (it.FEATURE & 4))
            self.assertEqual(524288, it.CATALOG)
            it.CATALOG = 0
            self.assertEqual(0, it.CATALOG)
            it.CATALOG = 524288
            self.assertEqual(1125899906842624, it.CONTINUOUSCOLOR,
                             msg="64 bit integer")

    #@ut.skip("temporarily")
    class TestConst(ut.TestCase):
        def test_UNDEF(self):
            with self.assertRaises(AttributeError, msg="property is not read only!"):
                Const.sUNDEF = "test"
            self.assertEqual(Const.sUNDEF, "?")
            self.assertEqual(Const.shUNDEF, 32767)
            self.assertEqual(Const.iUNDEF, 2147483645)
            self.assertEqual(Const.flUNDEF, 1e38)
            self.assertEqual(Const.rUNDEF, -1e308)
            self.assertEqual(Const.i64UNDEF, 9223372036854775808)

    #@ut.skip("temporarily")
    class TestEngine(ut.TestCase):
        def setUp(self):
            try:
                disconnectIssueLogger()
                Engine.setWorkingCatalog(workingDir+rasterDir)
                connectIssueLogger()
            except IlwisException:
                connectIssueLogger()
                self.skipTest("could not set working directory!")

            self.cs = CoordinateSystem(
                "code=proj4:+proj=utm +zone=35 +ellps=intl +towgs84=-87,-98,-121,0,0,0,0 +units=m +no_defs")

        def tearDown(self):
            del self.cs

        #@ut.skip("temporarily")
        def test_Operations(self):
            e = Engine()
            ops = e.operations()
            oper = ('setsttributetable', 'groupby', 'numbercondition', 'junction', 'createrastercoverage', 'saveas', 'joinattributes', 'testoperation', 'createprojectedcoordinatesystem', 'createcornersgeoreference', 'createpalettedomain', 'createintervaldomain', 'createidentifierdomain', 'createthematicdomain', 'createnumericdomain', 'columnunaryoperation', 'aggregation', 'convertcolumndomain', 'setworkingcatalog', 'contains', 'covers', 'coveredby', 'touches', 'intersects', 'disjoint', 'within', 'equals', 'crosses', 'overlaps', 'setvaluerange', 'binarymathtable', 'selection', 'mastergeoreference', 'binarymathfeatures', 'binarymathraster', 'selection', 'iffeature', 'stringfind', 'stringsub', 'stringreplace', 'rastersize', 'text2output', 'coord2pixel', 'coordinate', 'pixel', 'pixel2coord', 'selection', 'assignment', 'sin', 'cos', 'tan', 'asin', 'acos', 'atan', 'log10', 'ln', 'abs', 'sqrt', 'ceil', 'floor', 'sgn', 'cosh', 'sinh', 'sin', 'cos', 'tan', 'asin', 'acos', 'atan', 'log10', 'ln', 'abs', 'sqrt', 'ceil', 'floor', 'sgn', 'cosh', 'sinh', 'binarylogicalraster', 'iffraster', 'rastervalue', 'resample', 'buffer', 'transformcoordinates', 'pointrastercrossing', 'raster2point', 'gridding', 'httpserver', 'rasterquantile', 'covariance', 'aggregaterasterstatistics', 'correlation', 'relativeaggregaterasterStatistics', 'script', 'histogramhqualization', 'comparehistograms', 'medianblurfilter', 'gaussianblurfilter', 'erodefilter', 'dilatefilter', 'boxfilter', 'adaptivebilateralfilter', 'bilateralfilter', 'scharrfilter', 'laplacefilter', 'sobelfilter', 'postgresqlcatalog', 'distanceraster', 'densifyraster', 'timesat', 'polygon2raster', 'movingaverage', 'point2raster', 'clusterraster', 'mirrorrotateraster', 'linearrasterfilter', 'rankorderrasterfilter', 'areanumbering', 'sliceraster', 'boxclassification', 'aggregateraster', 'cross', 'linearstretch', 'remoteoperation', 'ilwisremotecatalog', 'wfscatalog')
            self.assertTrue(all((op in ops) for op in oper))
            self.assertEqual("gridsize(rastercoverage,dimension=xsize|ysize|zsize)", e.operationMetaData("rastersize"))
            self.assertEqual(
                "gridding(coordinatesyste,top-coordinate,x-cell-size, y-cell-size, horizontal-cells, vertical-cells)",
                e.operationMetaData("gridding"))
            self.assertEqual(("iffraster(rastercoverage,outputchoicetrue, outputchoicefalse)"),
                             e.operationMetaData("iffraster"))

        def test_Gridding(self):
            polygongrid = Engine.do("gridding", self.cs, Coordinate(225358.6605, 3849480.5700), 1000.0, 1000.0, 12, 12)
            self.assertTrue(bool(polygongrid), msg="gridding result is invalid!")
            self.assertEqual(polygongrid.type(), "FeatureCoverage", msg="wrong IlwisObject type")
            self.assertRegex(polygongrid.name(), r"gridding_[0-9]*",
                             msg="generated name should begin with gridding_ and end with its ID")
            self.assertEqual(polygongrid.featureCount(), 144, msg="wrong number of polygons in gridding result!")

        def test_Resample(self):
            resampled = Engine.do("resample", "subkenya.mpr", "alm011nd.grf", "bicubic")
            self.assertTrue(bool(resampled))

    #@ut.skip("temporarily")
    class TestFeature(ut.TestCase):
        def setUp(self):
            try:
                disconnectIssueLogger()
                Engine.setWorkingCatalog(workingDir + featureDir)
                connectIssueLogger()
            except IlwisException:
                connectIssueLogger()
                self.skipTest("could not set working directory!")

        def test_GPXFromFile(self):
            fc = FeatureCoverage(workingDir + featureDir + "/test.gpx/tracks")
            self.assertFalse(fc.isInternal())
            self.assertEqual(fc.featureCount(), 1)
            for f in fc:
                self.assertRegex(str(f.geometry()), r"MULTILINESTRING\s\(\([\s\.\-\,0-9]*\)\)")

        #@ut.skip("temporarily")
        def test_FeatureCoverage(self):
            fc = FeatureCoverage("rainfall.shp")
            self.assertTrue(fc, msg="FeatureCoverage(rainfall.shp) not loaded correctly!")
            self.assertEqual(fc.name(), "rainfall.shp", msg="internal FeatureCoverage name wrong!")
            self.assertEqual(fc.featureCount(), 13, msg="feature count wrong")
            fc.addColumn("sum", "value")
            att = fc.attributeTable().columns()
            self.assertTupleEqual(att, (
                'RAINFALL', 'JANUARY', 'FEBRUARY', 'MARCH', 'APRIL', 'MAY', 'JUNE', 'JULY',
                'AUGUST', 'SEPTEMBER', 'OCTOBER', 'NOVEMBER', 'DECEMBER', 'NEWCOL', 'IDENT', 'sum'
            ), msg="wrong list of attributes!")
            self.assertEqual(len(att), 16, msg="wrong number of attributes")
            g = Geometry("POINT(5.4 6 9.0)", fc.coordinateSystem())
            newfeature = fc.newFeature(g)
            newfeature2 = fc.newFeature("POINT(5.4 6 9.0)", fc.coordinateSystem())
            self.assertTrue(bool(newfeature), msg="newfeature creation failed!")
            for c in att:
                newfeature[c] = "12"
                newfeature2[c] = "42"
                self.assertEqual(int(newfeature[c]), 12, msg="new value of feature attribute not correct!")
                self.assertEqual(int(newfeature2[c]), 42, msg="new value of feature attribute not correct!")
            self.assertEqual(fc.featureCount(), 15, msg="new feature count wrong")

            fc_invalid = FeatureCoverage() # new empty featurecoverage without coordinate system
            fc_invalid.name("newFC")
            self.assertTrue(fc_invalid.isInternal())
            g = Geometry("POINT(5.4 6 9.0)", CoordinateSystem("code=epsg:23035"))
            with self.assertRaises(FeatureCreationError, msg="should raise FeatureCreationError"):
                newfeature = fc_invalid.newFeature(g)
            fc_invalid.setCoordinateSystem(CoordinateSystem("code=epsg:23035"))
            newfeature = fc_invalid.newFeature(g)

        def test_FeatureIterator(self):
            fc = FeatureCoverage("rainfall.shp")
            self.assertTrue(fc, msg="FeatureCoverage(rainfall.shp) not loaded correctly!")
            summ = 0
            for f in fc:
                tempsum = f.attribute("MAY", 0)
                if tempsum != -1e+308:
                    summ = summ + int(tempsum)
                f["sum"] = str(summ)
                self.assertRegex(str(f), r"Feature\([0-9]*\)", msg="wrong feature representation")
                self.assertRegex(str(f.geometry()),
                                 r"POINT\s\(([0-9\.\-]+|\-1e\+308|5\.4)\s([0-9\.\-]+|\-1e\+308|[0-9]+\.[0-9]+e\+[0-9]+)\)",
                                 msg="wrong geometry representation")
            self.assertEqual(fc.attributeTable().column("MAY"), (10, 15, 54, 59, 14, 9, 10, 55, 60, 0, 0, 0, 0, 12, 42))
            if fc.featureCount() == 15:
                self.assertEqual(summ, 340.0, msg="wrong sum over rainfall in MAY!")
            elif fc.featureCount() == 14:
                self.assertEqual(summ, 298.0, msg="wrong sum over rainfall in MAY!")
            else:
                self.assertEqual(summ, 286.0, msg="wrong sum over rainfall in MAY!")
            del summ
            it = iter(fc)
            self.assertTrue(bool(it))
            self.assertEqual(str(it), "FeatureIterator for rainfall.shp")
            it2 = it + 2
            self.assertTrue(it != it2)
            self.assertFalse(it == it2)
            it3 = it2 - 2
            self.assertTrue(it == it3)
            f = it3.current()
            self.assertTrue(str(f), "Feature(0)")
            f = next(it3)
            self.assertTrue(str(f), "Feature(0)")
            f = next(it3)
            self.assertTrue(str(f), "Feature(1)")

        def test_FeatureAttributes(self):
            fc = FeatureCoverage("GDAL_OGR_feature.vrt")
            #self.assertEqual(('String', 'Date', 'Time', 'DateTime', 'Integer', 'Float'), fc.attributes())
            self.assertEqual(100, fc.featureCount(), msg="meta data contains wrong featureCount")
            it = iter(fc)
            f = next(it)
            self.assertEqual(3, fc.featureCount(), msg="after loading binary data featureCount is correct")

            self.assertEqual(f.geometry().toWKT(), "LINESTRING (1.0000000000000000 1.0000000000000000, \
2.0000000000000000 2.0000000000000000)", msg="unsuccessfully altered geometry")
            f.geometry().fromWKT("POINT(5.4 6 9.0)")
            self.assertEqual(f.geometry().toWKT(), "POINT (5.4000000000000004 6.0000000000000000)",
                             msg="not typecheck! butunsuccessfully altered geometry")

            with self.assertRaises(IndexError, msg="IndexError: No attribute 'wrongColum' found"):
                v = f["wrongColum"]
            v = f["String"]
            self.assertTrue(type(v) is str)
            self.assertEqual(str(v), "LINESTRING(1 1, 2 2)", msg="wrong attribute value")
            f["String"] = 12
            self.assertEqual(12, int(f["String"]),
                             msg="no real type check here since it could be converted back and forth")

            self.assertEqual(f["Integer"], 4123045)
            f["Integer"] = -2147483648 # MIN (qint32)
            self.assertEqual(-2147483648, f["Integer"])
            f["Integer"] = -2147483649 # MIN (qint32) - 1
            self.assertEqual(Const.rUNDEF, f["Integer"])
            # f["Integer"] = -9223372036854775809
            # self.assertEqual(-1, f["Integer"])  # overflow(MIN-1)
            f["Integer"] = 2147483647 # MAX (qint32)
            self.assertEqual(2147483647, f["Integer"])
            f["Integer"] = 2147483648 # MAX (qint32) + 1
            self.assertEqual(Const.rUNDEF, f["Integer"])
            # f["Integer"] = 9223372036854775808
            # self.assertEqual(-1, f["Integer"])  # overflow(MAX+1)
            # f["Integer"] = "9223372036854775808"
            # self.assertEqual(Const.rUNDEF, f["Integer"])  # overflow(MAX+1)

            f["Float"] = "9223372036854775808"
            self.assertEqual("9223372036854775808", f["Float"])
            f["Float"] = 9223372036854775808.
            self.assertEqual(9223372036854775808, f["Float"])
            f["Float"] = 0.432
            self.assertEqual(0.432, f["Float"])
            f["Float"] = 23.4e-32
            self.assertEqual(23.4e-32, f["Float"])

            try:
                import datetime
            except ImportError as exc:
                self.fail(msg=str(exc))

            v = f["Date"]
            self.assertEqual("2014-02-17", str(v))
            self.assertTrue(type(v) is datetime.date)
            self.assertEqual(v, datetime.date(2014, 2, 17))
            f["Date"] = datetime.datetime(2014, 2, 27)
            self.assertEqual(f["Date"], datetime.datetime(2014, 2, 27), msg="no type check here!")

            class NotSupportedObject:
                pass

            with self.assertRaises(ValueError, msg="cannot convert instance NotSupportedObject() to Ilwis class"):
                f["Date"] = NotSupportedObject()
            self.assertEqual(f["Date"], datetime.datetime(2014, 2, 27))
            with self.assertRaises(TypeError, msg="cannot convert None to Ilwis class"):
                f["Date"] = None
            self.assertEqual(f["Date"], datetime.datetime(2014, 2, 27))

            v = f["Time"]
            self.assertEqual("12:42:33", str(v))
            self.assertTrue(type(v) is datetime.time)
            self.assertEqual(v, datetime.time(12, 42, 33))
            f["Time"] = datetime.time(12, 42, 33, 120000)
            self.assertEqual(f["Time"], datetime.time(12, 42, 33, 120000))
            v = f["DateTime"]
            self.assertEqual("2014-02-17 12:42:33", str(v))
            self.assertTrue(type(v) is datetime.datetime)
            self.assertEqual(v, datetime.datetime(2014, 2, 17, 12, 42, 33))
            f["DateTime"] = datetime.datetime(2014, 2, 27)
            self.assertEqual(f["DateTime"], datetime.datetime(2014, 2, 27))

            tup = ('12', datetime.datetime(2014, 2, 27, 0, 0), datetime.time(12, 42, 33, 120000),
                   datetime.datetime(2014, 2, 27, 0, 0), Const.rUNDEF, 2.34e-31)
            tab = fc.attributeTable()
            rec = tab.record(0)
            self.assertTrue(all((rec[i] == tup[i] for i in range(len(tup)))))
            self.assertEqual(len(rec), len(fc.attributeTable().columns()))
            tup = ('LINESTRING(1 1, 2 2, 3 3)', datetime.date(2014, 3, 4), datetime.time(12, 42, 33),
                   datetime.datetime(2014, 3, 4, 12, 42, 33), 4123045, 2342451235.5434)
            rec = fc.attributeTable().record(2)
            self.assertTrue(all((rec[i] == tup[i] for i in range(len(tup)))))
            self.assertEqual(len(rec), len(fc.attributeTable().columns()))
            self.assertTupleEqual(
                ('12', 'LINESTRING(1 1, 3 3)', 'LINESTRING(1 1, 2 2, 3 3)'),
                fc.attributeTable().column(0)
            )

        #@ut.skip("temporarily")
        def test_loadGDALstoreGDAL(self):
            # polygons
            world = FeatureCoverage("ne_110m_admin_0_countries.shp")
            self.assertTrue(bool(world))
            self.assertFalse(world.isInternal())
            world.store(workingDir + tempDir + "/countries_fromshp.shp", "ESRI Shapefile", "gdal")
            # points
            world = FeatureCoverage("rainfall.shp")
            self.assertTrue(bool(world))
            self.assertFalse(world.isInternal())
            world.store(workingDir + tempDir + "/rainfall_fromshp.shp", "ESRI Shapefile", "gdal")
            # lines
            world = FeatureCoverage("drainage.shp")
            self.assertTrue(bool(world))
            self.assertFalse(world.isInternal())
            world.store(workingDir + tempDir + "/drainage_fromshp.shp", "ESRI Shapefile", "gdal")

        #@ut.skip("temporarily")
        def test_loadGDALstoreIlwis3(self):
            # polygons
            world = FeatureCoverage("ne_110m_admin_0_countries.shp")
            self.assertTrue(bool(world))
            self.assertFalse(world.isInternal())
            #            world.setCoordinateSystem(CoordinateSystem("countries.csy"))  # TODO use/copy shp files coordinate system instead
            #world.setOutputConnection(workingDir + tempDir + "/countries_fromshp", "vectormap", "ilwis3")
            #world.store()
            # points
            world = FeatureCoverage("rainfall.shp")
            self.assertTrue(bool(world))
            self.assertFalse(world.isInternal())
            world.setCoordinateSystem(CoordinateSystem("countries.csy"))
            world.store(workingDir + tempDir + "/rainfall_fromshp", "vectormap", "ilwis3")
            # lines
            world = FeatureCoverage("drainage.shp")
            self.assertTrue(bool(world))
            self.assertFalse(world.isInternal())
            world.setCoordinateSystem(CoordinateSystem("countries.csy"))
            #world.setOutputConnection(workingDir + tempDir + "/drainage_fromshp", "vectormap", "ilwis3")
            #world.store()  # skiped until Ilwis3Connector can handle datetime format

            # def test_loadIlwis3storeIlwis3(self):
            #     # polygons
            #     world = FeatureCoverage("ne_110m_admin_0_countries.shp")
            #     self.assertTrue(bool(world))
            #     self.assertFalse(world.isInternal())
            #     world.setCoordinateSystem(CoordinateSystem("countries.csy"))
            #     world.setConnection(workingDir+tempDir+"/countries_fromshp", "vectormap", "ilwis3", IlwisObject.cmOUTPUT)
            #     world.store()
            #     # points
            #     world = FeatureCoverage("rainfall.shp")
            #     self.assertTrue(bool(world))
            #     self.assertFalse(world.isInternal())
            #     world.setCoordinateSystem(CoordinateSystem("countries.csy"))
            #     world.setConnection(workingDir+tempDir+"/rainfall_fromshp", "vectormap", "ilwis3", IlwisObject.cmOUTPUT)
            #     world.store()
            #     # lines
            #     world = FeatureCoverage("drainage.shp")
            #     self.assertTrue(bool(world))
            #     self.assertFalse(world.isInternal())
            #     world.setCoordinateSystem(CoordinateSystem("countries.csy"))
            #     world.setConnection(workingDir+tempDir+"/drainage_fromshp", "vectormap", "ilwis3", IlwisObject.cmOUTPUT)
            #     world.store()

    #@ut.skip("temporarily")
    class TestCoordinateSystem(ut.TestCase):
        def setUp(self):
            try:
                disconnectIssueLogger()
                Engine.setWorkingCatalog(workingDir + featureDir)
                connectIssueLogger()
            except IlwisException:
                connectIssueLogger()
                self.skipTest("could not set working directory!")

        #@ut.skip("temporarily")
        def test_FromFile(self):
            csy = CoordinateSystem(workingDir + featureDir + "/Cochabamba.csy")
            self.assertEqual("cochabamba.csy", csy.name())
            fc = FeatureCoverage("Rainfall.mpp")
            rainCsy = fc.coordinateSystem()
            #self.assertEqual(csy.ilwisID(), rainCsy.ilwisID())
            self.assertEqual(str(csy.envelope()), str(rainCsy.envelope()))

        #ut.skip("temporarily")
        def test_Proj4(self):
            cs1 = CoordinateSystem(
                "code=proj4:+proj=utm +zone=35 +ellps=intl +towgs84=-87,-98,-121,0,0,0,0 +units=m +no_defs")
            cs1.name("myProj4CSY")
            self.assertTrue(bool(cs1), msg="invalid CoordinateSystem from Proj4")
            fc = FeatureCoverage(workingDir + featureDir + "/rainfall.shp")
            fc.setCoordinateSystem(cs1)
            self.assertEqual(fc.coordinateSystem().name(), "myProj4CSY",
                             msg="could not alter FeatureCoverage's CoordinateSystem")
            self.assertEqual(cs1.toProj4(),
                             "+proj=utm +ellps=intl +towgs84=-87,-98,-121,0,0,0,0 +zone=35")

        def test_EPSG(self):
            cs2 = CoordinateSystem("code=epsg:23035")
            self.assertTrue(bool(cs2))
            self.assertEqual(cs2.name(), r"ED50 / UTM zone 35N")

        #@ut.skip("temporarily")
        def test_isEqual(self):
            cs1 = CoordinateSystem(
                "code=proj4:+proj=utm +zone=35 +ellps=intl +towgs84=-87,-98,-121,0,0,0,0 +units=m +no_defs")
            cs2 = CoordinateSystem("code=epsg:23035")
            self.assertTrue(cs1 == cs2)
            self.assertFalse(cs1 != cs2)

        def test_envelope(self):
            rc = RasterCoverage("n000302.mpr")
            env = rc.envelope()
            csyNew = CoordinateSystem("code=epsg:2050")
            newEnv = rc.coordinateSystem().convertEnvelope(csyNew, env)

            self.assertNotEqual(str(env), str(newEnv))

    #@ut.skip("temporarily")
    class TestGeoReference(ut.TestCase):
        def setUp(self):
            try:
                disconnectIssueLogger()
                Engine.setWorkingCatalog(workingDir + rasterDir)
                connectIssueLogger()
            except IlwisException:
                connectIssueLogger()
                self.skipTest("could not set working directory!")

        #@ut.skip("temporarily")
        def test_standaloneFile(self):
            gr = GeoReference("n000302.grf")
            self.assertTrue(bool(gr))
            self.assertFalse(gr.isInternal())
            self.assertFalse(gr.centerOfPixel())

        def test_fromRaster(self):
            r = RasterCoverage("n000302.mpr")
            gr = r.geoReference()
            self.assertTrue(bool(gr))
            self.assertFalse(gr.isInternal())
            self.assertTrue(r.coordinateSystem() == gr.coordinateSystem())
            self.assertTrue(r.size() == gr.size())
            box = Box(gr.size())
            env = gr.box2Envelope(box)
            self.assertEqual("0 0 0 1151 1151 0", str(box))
            self.assertEqual("-4611990.248000 -4603967.493500 4604009.752000 4612032.506500", str(env))
            subenv = Envelope(Coordinate(-1e+06, -1e+06), Coordinate(1e+06, 1e+06))
            subbox = gr.envelope2Box(subenv)
            self.assertEqual("-1000000.000000 -1000000.000000 1000000.000000 1000000.000000", str(subenv))
            self.assertEqual("451 451 701 701", str(subbox))
            self.assertEqual("pixel(536.599,478.436)", str(gr.coord2Pixel(Coordinate(-319195.47, 784540.64))))
            self.assertEqual("coordinate(-319198.248000,784544.506500)", str(gr.pixel2Coord(PixelD(536.599, 478.436))))
            self.assertEqual("coordinate(-323990.248000,788032.506500)", str(gr.pixel2Coord(Pixel(536, 478))))
            # self.assertEqual(0, gr.pixelSize())  # TODO possible bug in GeoRefImplementaion  - nan is not a good pixelSize!!
            self.assertTrue(gr.centerOfPixel())
            self.assertEqual("536 478 536 478", str(
                gr.envelope2Box(Envelope(Coordinate(-319195.47, 784540.64), Coordinate(-319990.248000, 784032.506500)))
            ))
            gr.setCenterOfPixel(False)
            self.assertFalse(gr.centerOfPixel())
            self.assertEqual("536 478 536 478", str(
                gr.envelope2Box(Envelope(Coordinate(-319195.47, 784540.64), Coordinate(-319990.248000, 784032.506500)))
            ))
            gr.setCenterOfPixel(True)
            self.assertTrue(gr.centerOfPixel())

    #@ut.skip("temporarily")
    class TestCatalog(ut.TestCase):
        def setUp(self):
            try:
                disconnectIssueLogger()
                Engine.setWorkingCatalog(workingDir + featureDir)
                connectIssueLogger()
            except IlwisException:
                connectIssueLogger()
                self.skipTest("could not set working directory!")

        def test_fromGPX(self):
            cat = Catalog(workingDir + featureDir + "/test.gpx")  # TODO use setWorkingCatalog instead of full Url
            self.assertTupleEqual(('route_points', 'route_points', 'routes', 'routes', 'track_points', 'track_points',
                                   'tracks', 'tracks', 'waypoints', 'waypoints'), cat.items())
            self.assertTrue(bool(cat))
            trks = cat['tracks']
            self.assertTrue(bool(trks))
            it = iter(trks)
            self.assertEqual(1, trks.featureCount())
            trkpts = cat['track_points']
            self.assertTrue(bool(trkpts))
            it = iter(trkpts)
            self.assertEqual(830, trkpts.featureCount())

        @ut.skip("temporarily")
        def test_fromWFS(self):
            cat = Catalog(
                "http://ogi.state.ok.us/geoserver/wfs?acceptVersions=1.1.0&REQUEST=GetCapabilities&SERVICE=WFS")
            self.assertTupleEqual((), cat.items())  # TODO WFS not yet working


    #@ut.skip("temporarily")
    class TestRaster(ut.TestCase):
        def setUp(self):
            try:
                disconnectIssueLogger()
                Engine.setWorkingCatalog(workingDir + rasterDir)
                connectIssueLogger()
                self.small = [
                    3.0, 80.0, 80.0, 80.0, 3.0,
                    15.0, 22.0, 22.0, 130.0, 100.0,
                    15.0, 132.0, 35.0, 21.0, 100.0,
                    15.0, 34.0, 67.0, 179.0, 100.0,
                    3.0, 120.0, 120.0, 120.0, 3.0,

                    2.0, 99.0, 99.0, 99.0, 2.0,
                    20.0, 32.0, 73.0, 44.0, 55.0,
                    20.0, 78.0, 24.0, 44.0, 55.0,
                    20.0, 34.0, 132.0, 86.0, 55.0,
                    2.0, 145.0, 145.0, 145.0, 2.0,

                    1.0, 30.0, 30.0, 30.0, 1.0,
                    70.0, 60.0, 15.0, 32.0, 150.0,
                    70.0, 12.0, 10.0, 101.0, 150.0,
                    70.0, 69.0, 170.0, 99.0, 150.0,
                    1.0, 120.0, 120.0, 120.0, 1.0,

                    34.0, 34.0, 123.0, 123.0, 45.0,
                    34.0, 174.0, 174.0, -1e+308, 45.0,
                    78.0, 78.0, 174.0, -1e+308, 29.0,
                    78.0, 78.0, 1.0, 156.0, 29.0,
                    78.0, 78.0, 7.0, 7.0, 29.0,

                    32.0, 150.0, 150.0, 22.0, 22.0,
                    32.0, 77.0, 88.0, 131.0, 22.0,
                    117.0, 117.0, 88.0, 5.0, 5.0,
                    45.0, 2.0, 88.0, 2.0, 101.0,
                    45.0, 45.0, 150.0, 101.0, 101.0,

                    90.0, 6.0, 135.0, 90.0, 120.0,
                    90.0, 2.0, 117.0, 4.0, 90.0,
                    45.0, 45.0, 135.0, 45.0, 45.0,
                    90.0, 3.0, 78.0, 5.0, 90.0,
                    7.0, 90.0, 135.0, 90.0, 9.0,

                    179.0, 110.0, 179.0, 67.0, 110.0,
                    90.0, 179.0, 23.0, 90.0, 110.0,
                    110.0, 34.0, 67.0, 110.0, 78.0,
                    179.0, 67.0, 90.0, 110.0, 67.0,
                    90.0, 179.0, 77.0, 90.0, 88.0,
                ]
            except IlwisException:
                connectIssueLogger()
                self.skipTest("could not set working directory!")

        #@ut.skip("temporarily")
        def test_RasterCalculation(self):
            rc = RasterCoverage("n000302.mpr")
            rctif = RasterCoverage("n0.mpr")
            self.assertTrue(rc and rctif, msg="couldn't load RasterCoverages")
            self.assertEqual(rc.name(), "n000302.mpr")
            self.assertEqual(rctif.name(), "n0.mpr")
            pix = Pixel(342, 342, 0)
            self.assertTrue(pix in rc.size())
            self.assertEqual(rc.pix2value(pix), 29.0)
            self.assertEqual(rc.coord2value(Coordinate(-1871900.47, 1874237.55, 0)), 29.0)
            self.assertEqual(rctif.pix2value(pix), 29.0)

            aa7 = Engine.do("sin", rc)
            self.assertAlmostEqual(aa7.pix2value(pix), -0.6636, 3)

            aa1 = rc + rctif
            self.assertEqual(aa1.pix2value(pix), 29.0 + 29.0)
            aa2 = rc + 2
            self.assertEqual(aa2.pix2value(pix), 29.0 + 2)
            aa3 = 2 + rc
            self.assertEqual(aa3.pix2value(pix), 2 + 29.0)
            aa4 = rc - rctif
            self.assertEqual(aa4.pix2value(pix), 29.0 - 29.0)
            aa5 = 2 - rc
            self.assertEqual(aa5.pix2value(pix), 2-29.0)
            aa6 = rc - 2
            self.assertEqual(aa6.pix2value(pix), 29.0 - 2)
            aa7 = rc / rctif
            self.assertEqual(aa7.pix2value(pix), 29.0 / 29.0)
            aa8 = 2 / rc
            self.assertAlmostEqual(aa8.pix2value(pix), 2/29.0, 2)
            aa9 = rc / 2
            self.assertAlmostEqual(aa9.pix2value(pix), 29.0 / 2, 1)
            aa10 = rc * rctif
            self.assertEqual(aa10.pix2value(pix), 29.0 * 29.0)
            aa11 = 2 * rc
            self.assertEqual(aa11.pix2value(pix), 2 * 29.0)
            aa12 = rc * 2
            self.assertEqual(aa12.pix2value(pix), 29.0 * 2)
            aa13 = rc + 3 * rctif
            self.assertEqual(aa13.pix2value(pix), 29.0 + 3 * 29.0)
            aa14 = rctif - 3 + rc / 2
            self.assertAlmostEqual(aa14.pix2value(pix), 29.0 - 3 + 29.0 / 2, 1)
            aa15 = (rc + 3) * 5 + 5 / rctif
            self.assertAlmostEqual(aa15.pix2value(pix), (29.0 + 3) * 5 + 5 / 29.0, 1)
            aa16 = Engine.do("sqrt", rc)
            self.assertAlmostEqual(aa16.pix2value(pix), sqrt(29), 2)

            aa1.store(workingDir + tempDir + "/n000302_frommpr", "GTiff", "gdal")
            aa16 = None
            aa15 = None
            aa14 = None
            aa13 = None
            aa12 = None
            aa11 = None
            aa10 = None
            aa9 = None

        #@ut.skip("temporarily")
        def test_RasterCalculationsDifferentGeoref(self):
            rc1 = RasterCoverage("subkenya.mpr")
            rc2 = RasterCoverage("kenya_2009ndvi_cor_22.mpr")
            pix = Pixel(120, 120, 0) # offset is (0, 0, 0), thus in ilwis3 terms this is pixel (121, 121)
            self.assertTrue(pix in rc1.size())
            self.assertEqual(rc1.pix2value(pix), 96.0)
            aa1 = rc1 + rc2 # rc2 is automatically resampled to rc1; the value at Pixel(120,120,0) is 0.201146909738264 (probably the Ilwis4 "resample" shifts the image right-down by one pixel.
            valrc1 = 96.0
            valrc2 = 0.201146909738264
            aa1.store(workingDir + rasterDir + "/kenya_frommpr", "map", "ilwis3")
            self.assertAlmostEqual(aa1.pix2value(pix), valrc1 + valrc2, 4)
            aa2 = rc1 - rc2
            self.assertAlmostEqual(aa2.pix2value(pix), valrc1 - valrc2, 4)
            aa3 = rc1 * rc2
            self.assertAlmostEqual(aa3.pix2value(pix), valrc1 * valrc2, 4)
            aa4 = rc1 / rc2
            self.assertAlmostEqual(aa4.pix2value(pix), valrc1 / valrc2, 4)
            aa5 = rc1 + rc2 / 3
            self.assertAlmostEqual(aa5.pix2value(pix), valrc1 + valrc2 / 3, 4)
            aa5 = rc1 / 3 + rc2
            self.assertAlmostEqual(aa5.pix2value(pix), valrc1 / 3 + valrc2, 4)
            aa6 = 2 * rc1 - rc2
            self.assertAlmostEqual(aa6.pix2value(pix), 2 * valrc1 - valrc2, 4)
            aa7 = rc1 + rc2 - 2
            self.assertAlmostEqual(aa7.pix2value(pix), valrc1 + valrc2 - 2, 4)
            aa8 = rc1 + rc2 + 2
            self.assertAlmostEqual(aa8.pix2value(pix), valrc1 + valrc2 + 2, 4)
            aa8 = None
            aa7 = None
            aa6 = None
            aa5 = None
            aa4 = None
            aa3 = None
            aa2 = None
            aa1 = None
            gc.collect()

        #@ut.skip("temporarily")
        def test_RasterSelectionWKT(self):
            rc = RasterCoverage("n000302.mpr")

            rcSelWKT = rc.select("Polygon((495209 80832,927209 -999367, 1887209 -1282307,2184809 311232,495209 80832))")

            self.assertTrue(bool(rcSelWKT))

            pix1 = Pixel(740, 650, 0)
            pix2 = Pixel(300, 400, 0)

            coord = rc.geoReference().pixel2Coord(pix1)
            pixSel = rcSelWKT.geoReference().coord2Pixel(coord)

            pixSel.setY(int(pixSel.y)+1)

            self.assertEqual(rcSelWKT.pix2value(pixSel), rc.pix2value(pix1))
            self.assertNotEqual(rcSelWKT.pix2value(pix2), rc.pix2value(pix2))

            rcSelWKT.store(workingDir + rasterDir + "/aa_select_n000302", "map", "ilwis3")

        def test_RasterSelectionGeom(self):
            rc = RasterCoverage("n000302.mpr")

            geom = Geometry("Polygon((495209 80832,927209 -999367, 1887209 -1282307,2184809 311232,495209 80832))", rc.coordinateSystem())

            rcSel = rc.select(geom)

            self.assertTrue(bool(rcSel))

            pix1 = Pixel(740, 650, 0)
            pix2 = Pixel(300, 400, 0)

            coord = rc.geoReference().pixel2Coord(pix1)
            pixSel = rcSel.geoReference().coord2Pixel(coord)

            pixSel.setY(int(pixSel.y)+1)

            self.assertEqual(rcSel.pix2value(pixSel), rc.pix2value(pix1))
            self.assertNotEqual(rcSel.pix2value(pix2), rc.pix2value(pix2))

            rcSel.store(workingDir + rasterDir + "/aa_select_n000302", "map", "ilwis3")

        #@ut.skip("temporarily")
        def test_Datadefinition(self):
            rc = RasterCoverage("n000302.mpr")
            rc2 = RasterCoverage("small.mpr")
            rc3 = RasterCoverage("subkenya.mpr")

            dat = rc.datadef()
            self.assertTrue(bool(dat), msg="couldn't load datadefinition")
            dat2 = rc2.datadef()
            dat3 = rc3.datadef()

            self.assertTrue(bool(dat2))
            self.assertTrue(bool(dat3))
            self.assertTrue(dat.isCompatibleWith(dat2), msg="datadefinitions are not compatible")
            self.assertTrue(dat.isCompatibleWith(dat3), msg="datadefinition of submap is not compatible")

            datmerge = DataDefinition.merge(dat, dat2)
            self.assertTrue(bool(datmerge), msg="couldn't merge datadefinitions")

            dat3 = dat
            self.assertTrue(dat3 == dat)

        #@ut.skip("temporarily")
        def test_PixelIterator(self):
            rcl = RasterCoverage("small.mpl")
            self.assertTrue(bool(rcl), msg="couldn't load small.mpl")
            self.assertEqual(rcl.name(), "small.mpl")

            i = 0
            for v in rcl:
                self.assertEqual(v, self.small[i])
                i += 1

            it = iter(rcl)
            self.assertEqual(str(it), "PixelIterator for small.mpl at position pixel(0,0,0)",
                             msg="wrong string representation for PixelIterator")
            self.assertTrue(bool(it), msg="couldn't create PixelIterator from small.mpl")
            lin = rcl.size().linearSize()
            self.assertEqual(lin, 175, msg="wrong linear size of small.mpl")
            for i in range(lin):
                if it.yChanged():
                    self.assertTrue(i % 5 == 0, msg="yChanged not only every 5th step (i=" + str(i) + ")")
                else:
                    self.assertFalse((i % 5 == 0) and (i != 0),
                                     msg="yChanged not only every 2nd step (i=" + str(i) + ")")
                if it.zChanged():
                    self.assertTrue(i % 25 == 0, msg="zChanged not only every 25th step (i=" + str(i) + ")")
                else:
                    self.assertFalse((i % 25 == 0) and (i != 0),
                                     msg="zChanged not only every 2nd step (i=" + str(i) + ")")
                self.assertEqual(next(it), self.small[i])

            rit = iter(rcl)
            self.assertTrue(bool(rit))
            self.assertEqual(str(rit.position()), "pixel(0,0,0)")
            rit += 4
            self.assertEqual(str(rit.position()), "pixel(4,0,0)")
            rit2 = rit + 3
            self.assertEqual(str(rit2.position()), "pixel(2,1,0)")
            rit3 = 13 + rit
            self.assertEqual(str(rit3.position()), "pixel(2,3,0)")
            self.assertEqual(str(rit.position()), "pixel(4,0,0)")
            self.assertTrue(Pixel(1, 1, 1) in rit)
            self.assertFalse(Pixel(5, 1, 1) in rit)
            self.assertFalse(rit3 == rit2)
            self.assertEqual(float(rit2), 22.0)
            self.assertEqual(rit[23], 120.0)
            self.assertEqual(float(rit), 120.0)
            rit[23] = 434
            self.assertEqual(rit[23], 434)
            rit2 = PixelIterator(rit[Pixel(2, 2, 2)])
            self.assertEqual(str(rit.position()), "pixel(2,2,2)")
            self.assertEqual(int(rit), 62)
            self.assertEqual(next(rit), 10.0)
            self.assertTrue(rit.xChanged())
            self.assertFalse(rit.yChanged())
            self.assertFalse(rit.zChanged())
            self.assertEqual(str(rit.position()), "pixel(3,2,2)")
            self.assertEqual(str(rit2.position()), "pixel(2,2,2)")
            self.assertFalse(rit == rit2)
            self.assertTrue(rit != rit2)
            self.assertTrue(rit >= rit2)
            self.assertTrue(rit > rit2)
            self.assertFalse(rit <= rit2)
            self.assertFalse(rit < rit2)
            self.assertEqual(int(rit), 63)
            self.assertEqual(float(rit), 101.0)
            box = Box("(1 1 0,2 2 3)")
            self.assertEqual(str(box), "1 1 0 2 2 3")
            bit = PixelIterator(rcl, box)
            self.assertTrue(bool(bit))
            boxed_small = [
                ("pixel(1,1,0)", 22.0), ("pixel(2,1,0)", 22.0),
                ("pixel(1,2,0)", 132.0), ("pixel(2,2,0)", 35.0),
                ("pixel(1,1,1)", 32.0), ("pixel(2,1,1)", 73.0),
                ("pixel(1,2,1)", 78.0), ("pixel(2,2,1)", 24.0),
                ("pixel(1,1,2)", 60.0), ("pixel(2,1,2)", 15.0),
                ("pixel(1,2,2)", 12.0), ("pixel(2,2,2)", 10.0),
                ("pixel(1,1,3)", 174.0), ("pixel(2,1,3)", 174.0),
                ("pixel(1,2,3)", 78.0), ("pixel(2,2,3)", 174.0),
            ]
            self.assertTrue(Pixel(1, 1, 1) in bit)
            self.assertFalse(Pixel(0, 0, 0) in bit)
            lin = bit.box().size().linearSize()
            self.assertEqual(lin, 16)
            for i in range(lin):
                self.assertEqual(str(bit.position()), boxed_small[i][0])
                if bit.yChanged():
                    self.assertTrue(i % 2 == 0, msg="yChanged not only every 2nd step (i=" + str(i) + ")")
                else:
                    self.assertFalse((i % 2 == 0) and (i != 0),
                                     msg="yChanged not only every 2nd step (i=" + str(i) + ")")
                if bit.zChanged():
                    self.assertTrue(i % 4 == 0, msg="zChanged not only every 4th step (i=" + str(i) + ")")
                else:
                    self.assertFalse((i % 4 == 0) and (i != 0),
                                     msg="zChanged not only every 4th step (i=" + str(i) + ")")
                self.assertEqual(next(bit), boxed_small[i][1])

        #@ut.skip("temporarily")
        def test_NumPy(self):
            try:
                import numpy as np
            except ImportError:
                self.skipTest("numpy not available")

            rcl = RasterCoverage("small.mpl")
            it = iter(rcl)
            a = np.fromiter(it, np.float, it.box().size().linearSize())
            b = np.equal(self.small, a)
            self.assertTrue(all(v for v in b))

            # this is !!experimental!! direct access for NumPy. We don't encourage to use PixelIterator.asBuffer()
            # because so far no consistency check is done on memory access and the blocks of a raster(-list) are not
            # contiguous, so make sure numpy.ndarrays (views) point only to contiguous memory by adjusting the size
            # to the actual block size of the current block (interface still missing)

            rc = RasterCoverage("n000302.mpr")
            it2 = PixelIterator(rc)
            self.assertEqual(1152 * 1152, it2.box().size().linearSize())
            bu = np.frombuffer(it2.asBuffer(), np.float, it2.box().size().linearSize(), 0)
            self.assertTrue(all(0 <= v <= 255 for v in bu))

        def test_band(self):
            rc = RasterCoverage("landuse.mpr")
            pixIt = rc.band(0)

        def test_reprojectRaster(self):
            rc = RasterCoverage("n000302.mpr")

            targetCsy = CoordinateSystem("code=epsg:2050")

            sourceGeoref = rc.geoReference()
            sourceCsy = rc.coordinateSystem()
            sourceEnv = rc.envelope()

            targetEnv = sourceCsy.convertEnvelope(targetCsy, sourceEnv)

            bo = sourceGeoref.envelope2Box(targetEnv)
            sz = bo.size()

            newGeoRefStr = ("code=georef:type=corners,csy=epsg:2050,envelope=" +
                        str(targetEnv) + ",gridsize=" + str(sz.xsize) + " " + str(sz.ysize) +
                        ",name=grf1")

            targetGeoRef = GeoReference(newGeoRefStr)
            rcReproj = Engine.do("resample", rc.name(), targetGeoRef.name(), "bilinear")
            self.assertTrue(bool(rcReproj))

            rcReproj2 = rc.reprojectRaster("newraster", 2050, "bilinear")
            self.assertTrue(bool(rcReproj2))

            rc = None
            sz = None
            bo = None
            targetEnv = None
            sourceCsy = None
            sourceGeoref = None
            newGeoRefStr = None
            targetGeoRef = None
            rcReproj2 = None
            rcReproj = None
            gc.collect()

    #@ut.skip("temporarily")
    class TestExample(ut.TestCase):  # and martins solution proposal <== example code for presentation
        def setUp(self):
            try:
                disconnectIssueLogger()
                Engine.setWorkingCatalog(workingDir + exampleDir)
                connectIssueLogger()
            except IlwisException:
                connectIssueLogger()
                self.skipTest("could not set working directory!")

        #@ut.skip("temporarily")
        def test_claudio(self):
            distribution = FeatureCoverage(workingDir + exampleDir + "/freq.mpp")
            polygongrid = Engine.do("gridding", distribution.coordinateSystem(), Coordinate(26.5, 4.5), 1, 1, 15, 13)
            self.assertRegex(polygongrid.name(), r"gridding_[0-9]*")
            polygongrid.addColumn("maxY", "value")
            for polygon in polygongrid:
                #polygon.setAttribute("maxY", 0)
                for point in distribution:
                    if polygon.geometry().contains(point.geometry()):
                        if polygon.attribute("maxY", 0) != '?':
                            maxval = max(polygon.attribute("maxY", 0), point.attribute("freq_speciesY", 0))
                        else:
                            maxval = 0
                        polygon.setAttribute("maxY", maxval)
                    #print(count)

            polygongrid.store(workingDir + exampleDir + "/polygongrid", "vectormap", "ilwis3")

        @ut.skip("temporarily")
        def test_claudio2(self):
            import numpy as np
            Engine.setWorkingCatalog(workingDir + rasterDir)
            rcl = RasterCoverage("small.mpl")
            it = rcl.band(0)

            a = np.fromiter(it, np.float, it.box().size().linearSize())
            a[3] = 234.1

            subRange = NumericRange(0, 20)
            subDomain = NumericDomain()
            subDomain.setRange(subRange)

            rc = RasterCoverage("storetest")
            rc.setGeoReference(rcl.geoReference())
            rc.setDataDef(rcl.datadef())
            rc.setStackDefinition(subDomain, (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10))

            rc2 = RasterCoverage()
            rc2.setGeoReference(rcl.geoReference())
            rc2.setDataDef(rcl.datadef())
            rc2.setStackDefinition(subDomain, (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10))
            rc2.setBandDefinition(0, rc.datadef())
            rc2.setBandDefinition(1, rc.datadef())
            rc2.setBandDefinition(2, rc.datadef())

            itNew = rc2.begin()
            for y in range(rc.size().linearSize()):
                itNew[y] = a[y]

            rc.addBand(0, rc2.begin())

            it = rcl.band(1)
            b = np.fromiter(it, np.float, it.box().size().linearSize())

            itNew = rc2.begin()

            for y in range(rc2.size().linearSize()):
                itNew[y] = b[y]

            rc.addBand(1, rc2.begin())

            it = rcl.band(2)
            c = np.fromiter(it, np.float, it.box().size().linearSize())
            itNew = rc2.begin()
            for y in range(rc2.size().linearSize()):
                itNew[y] = c[y]

            rc.addBand(2, rc2.begin())

            rc.store(workingDir + "/newRaster", "GTiff", "gdal")

    #@ut.skip("temporarily")
    class TestBaby(ut.TestCase):
        def setUp(self):
            try:
                disconnectIssueLogger()
                Engine.setWorkingCatalog(workingDir + babyDir)
                connectIssueLogger()
            except IlwisException:
                self.skipTest("could not set working directory!")

        #@ut.skip("temporarily")
        def test_helloRaster(self):
            rc = RasterCoverage("n000302.mpr")
            res = Engine.do("aggregateraster", rc, "Avg", 10, True)
            res.store(workingDir + babyDir + "/avg_n000302", "map", "ilwis3")

        def test_helloFeature(self):
            fc_soils = FeatureCoverage("aafsoils.shp")
            fc_soils.setCoordinateSystem(CoordinateSystem("wgs.csy"))
            count = 0
            fc_soils.addColumn("selected", "boolean")
            for feature in fc_soils:
                if feature["AREA"] == 0.123:
                    count += 1
                    self.assertRegex(str(feature.geometry()), r"POLYGON\s\(\([\s\.\-\,0-9]*\)\)",
                                     msg="wrong WKT representation of geometry!")
                    feature["selected"] = True
                else:
                    feature["selected"] = False
            self.assertEqual(count, 3, msg="wrong count value!")
            fc_soils.store(workingDir + babyDir + "/soils_select", "vectormap", "ilwis3")

    #@ut.skip("temporarily")
    class TestWorld(ut.TestCase):
        def setUp(self):
            try:
                disconnectIssueLogger()
                Engine.setWorkingCatalog(workingDir + worldDir)
                connectIssueLogger()
            except IlwisException:
                self.skipTest("could not set working directory!")

        #@ut.skip("temporarily")
        def test_halloWorld(self):
            world = FeatureCoverage("countries.mpa")
            if bool(world) and not world.isInternal():
                population_ranking = {}
                self.assertEqual(177, world.featureCount())
                for country in world:
                    name = country["iso_a2"]
                    if name not in population_ranking:
                        population_ranking[name] = country["pop_est"]
                # print(sorted(population.items(), key=lambda x: x[1]))
                self.assertEqual(
                    {'OM': 3418085.0, 'HU': 9905596.0, 'HT': 9035536.0, 'HR': 4489409.0, 'ZW': 12619600.0,
                     'HN': 7792854.0, 'RW': 10473282.0, 'CH': 7604467.0, 'JM': 2825928.0, 'RS': 7379339.0,
                     'RO': 22215421.0, 'JP': 127078679.0, 'GY': 772298.0, 'GT': 13276517.0, 'CO': 45644023.0,
                     'GQ': 650702.0, 'GR': 10737428.0, 'GL': 57600.0, 'GM': 1782893.0, 'GN': 10057975.0,
                     'GH': 23832495.0, 'GE': 4615807.0, 'GA': 1514993.0, 'GB': 62262000.0, 'US': 313973000.0,
                     'GW': 1533964.0, 'MN': 3041142.0, 'ML': 12666987.0, 'MM': 48137741.0, 'UZ': 27606007.0,
                     'UY': 3494382.0, 'MG': 20653556.0, 'MD': 4320748.0, 'ME': 672180.0, 'MZ': 21669278.0,
                     'MX': 111211789.0, 'UA': 45700395.0, 'VE': 26814843.0, 'MR': 3129486.0, 'MW': 14268711.0,
                     'FR': 64057792.0, 'ZM': 11862740.0, 'FK': 3140.0, 'FJ': 944720.0, 'FI': 5250275.0,
                     'LV': 2231503.0, 'LB': 4017095.0, 'PY': 6995655.0, '-99': 265100.0, 'PT': 10707924.0,
                     'PS': 4119083.0, 'PR': 3971020.0, 'PL': 38482919.0, 'PH': 97976603.0, 'PK': 176242949.0,
                     'PE': 29546963.0, 'PG': 6057263.0, 'PA': 3360474.0, 'EC': 14573101.0, 'EG': 83082869.0,
                     'EE': 1299371.0, 'EH': -99.0, 'ER': 5647168.0, 'ES': 40525002.0, 'ZA': 49052489.0,
                     'ET': 85237338.0, 'SY': 20178485.0, 'SZ': 1123913.0, 'MK': 2066718.0, 'SR': 481267.0,
                     'SS': 10625176.0, 'SV': 7185218.0, 'SI': 2005692.0, 'SK': 5463046.0, 'SL': 6440053.0,
                     'SN': 13711597.0, 'SO': 9832017.0, 'SA': 28686633.0, 'SB': 595613.0, 'SD': 25946220.0,
                     'SE': 9059651.0, 'LR': 3441790.0, 'DE': 82329758.0, 'LU': 491775.0, 'DK': 5500510.0,
                     'DJ': 516055.0, 'DO': 9650054.0, 'LT': 3555179.0, 'DZ': 34178188.0, 'NC': 227436.0,
                     'NA': 2108665.0, 'MA': 34859364.0, 'NG': 149229090.0, 'NE': 15306252.0, 'VN': 86967524.0,
                     'NI': 5891199.0, 'NO': 4676305.0, 'IT': 58126212.0, 'NL': 16715999.0, 'NP': 28563377.0,
                     'NZ': 4213418.0, 'KP': 22665345.0, 'CI': 20617068.0, 'KR': 48508972.0, 'CL': 16601707.0,
                     'CM': 18879301.0, 'CN': 1338612970.0, 'KW': 2691158.0, 'CA': 33487208.0, 'KZ': 15399437.0,
                     'CD': 68692542.0, 'CF': 4511488.0, 'CG': 4012809.0, 'CY': 531640.0, 'CZ': 10211904.0,
                     'MY': 25715819.0, 'KE': 39002772.0, 'KG': 5431747.0, 'KH': 14494293.0, 'CR': 4253877.0,
                     'CU': 11451652.0, 'VU': 218519.0, 'UG': 32369558.0, 'JO': 6342948.0, 'RU': 140041247.0,
                     'QA': 833285.0, 'BG': 7204687.0, 'BF': 15746232.0, 'BE': 10414336.0, 'BD': 156050883.0,
                     'BA': 4613414.0, 'BO': 9775246.0, 'BN': 388190.0, 'BJ': 8791832.0, 'BI': 8988091.0,
                     'BW': 1990876.0, 'BT': 691141.0, 'BS': 309156.0, 'BR': 198739269.0, 'BZ': 307899.0,
                     'BY': 9648533.0, 'LK': 21324791.0, 'TR': 76805524.0, 'TT': 1310000.0, 'TW': 22974347.0,
                     'LA': 6834942.0, 'TZ': 41048532.0, 'LY': 6310434.0, 'IL': 7233701.0, 'YE': 23822783.0,
                     'TD': 10329208.0, 'TG': 6019877.0, 'TF': 140.0, 'TH': 65905410.0, 'LS': 2130819.0,
                     'TJ': 7349145.0, 'TM': 4884887.0, 'TL': 1131612.0, 'TN': 10486339.0, 'AO': 12799293.0,
                     'AL': 3639453.0, 'AM': 2967004.0, 'IR': 66429284.0, 'IS': 306694.0, 'IQ': 31129225.0,
                     'AF': 28400000.0, 'AE': 4798491.0, 'ID': 240271522.0, 'IE': 4203200.0, 'AZ': 8238672.0,
                     'IN': 1166079220.0, 'AT': 8210281.0, 'AU': 21262641.0, 'AR': 40913584.0, 'AQ': 3802.0},
                    population_ranking)
            else:
                self.skipTest("countries.mpa is missing")

        #@ut.skip("temporarily")
        def test_IlwisObject(self):
            fc = FeatureCoverage()
            fc.name("newFC")
            self.assertEqual("newFC", fc.name())
            fc.name("newName")
            self.assertEqual("newName", fc.name())
            self.assertTrue(fc.isInternal())
            fc.open(workingDir + worldDir + "/countries.mpa", "vectormap", "ilwis3")
            #fc = FeatureCoverage("countries.mpa")
            self.assertFalse(fc.isInternal())
            self.assertEqual("countries.mpa", fc.name())
            #fc.setOutputConnection(workingDir + worldDir + "/countries.shp", "ESRI Shapefile", "gdal")
            #fc.store()

        def test_AttributeTable(self):
            table = Table("countries.tbt")
            self.assertTrue(bool(table))
            self.assertFalse(table.isInternal())
            value = str(table.cell("iso_a2", 4))
            self.assertEqual("AR", value)
            table.setCell("iso_a2", 4, value.upper())
            recCount = table.recordCount()
            self.assertEqual(177, recCount)
            colCount = table.columnCount()
            self.assertEqual(64, colCount)
            columns = table.columns()  # ('column1','column2',...)
            self.assertEqual(64, len(columns))
            self.assertEqual(42, table.columnIndex("iso_a2"))
            self.assertEqual(Const.iUNDEF, table.columnIndex("ihfg"))
            column = table.column(42)
            self.assertEqual(177, len(column))
            column1 = table.column("iso_a2")
            self.assertEqual(column, column1)
            record = table.record(4)
            self.assertEqual(64, len(record))
            table.addColumn("newCol", "value")
            self.assertEqual(65, table.columnCount())

    #@ut.skip("temporarily")
    class TestIntegration(ut.TestCase):
        def setUp(self):
            try:
                disconnectIssueLogger()
                Engine.setWorkingCatalog(workingDir + integrationDir)
                connectIssueLogger()
            except IlwisException:
                self.skipTest("could not set working directory!")

        def test_numpy(self):
            try:
                import numpy as np
                import matplotlib.pyplot as plt
            except ImportError:
                self.skipTest("numpy not available")

            raster = RasterCoverage("n0.mpr")
            self.assertFalse(raster.isInternal())
            it = iter(raster)
            npRaster = np.fromiter(it, np.float, it.box().size().linearSize())
            # (num, bins) = np.histogram(npRaster, bins=10, range=(0, 255))
            # nu = [143870, 130523,  56858,  32878,  36515,  45334,  55831,  21598, 1657, 802040]
            # self.assertTrue(all(nu[i] == num[i] for i in range(len(nu))))
            # bi = [0., 25.5, 51., 76.5, 102., 127.5, 153., 178.5, 204., 229.5, 255.]
            # self.assertTrue(all(bi[i] == bins[i] for i in range(len(bi))))

            # plt.figure()
            # plt.hist(npRaster, 255, (0, 253))
            # plt.show()

            # n = 5
            # menMeans = (20, 35, 30, 35, 27)
            # menStd =   (2, 3, 4, 1, 2)
            #
            # ind = np.arange(n)  # the x locations for the groups
            # width = 0.35       # the width of the bars
            #
            # fig, ax = plt.subplots()
            # rects = ax.bar(ind, menMeans, width, color='r', yerr=menStd)
            #
            # # add some
            # # ax.set_ylabel('')
            # # ax.set_title('')
            # ax.set_xticks(ind+width)
            # ax.set_xticklabels( ('G1', 'G2', 'G3', 'G4', 'G5') )
            #
            # plt.show()

    #@ut.skip("temporarily")
    class TestNumericDomain(ut.TestCase):
        def setUp(self):
            disconnectIssueLogger()
            connectIssueLogger()


        def test_containementInt(self):
            nr = NumericRange(5, 60, 5)
            childnumdom = NumericDomain("numbers")
            childnumdom.setRange(nr)
            nr2 = NumericRange(0, 80, 5)
            parentnumdom = NumericDomain("parentnumbers")
            parentnumdom.setRange(nr2)

            childnumdom.setParent(parentnumdom)
            childnumdom.setStrict(False)

            self.assertEqual(childnumdom.contains(20), "cSELF")
            self.assertEqual(childnumdom.contains(80), "cPARENT")
            self.assertEqual(childnumdom.contains(100), "cNONE")
            self.assertEqual(childnumdom.contains(31), "cNONE")

            self.assertFalse(childnumdom.isStrict())
            childnumdom.setStrict(True)
            self.assertTrue(childnumdom.isStrict())

            self.assertEqual(childnumdom.contains(80), "cNONE")

        def test_containementDouble(self):
            nr = NumericRange(5.8, 60.2, 0.2)
            childnumdom = NumericDomain("numbers")
            childnumdom.setRange(nr)
            nr2 = NumericRange(0, 80.4, 0.2)
            parentnumdom = NumericDomain("parentnumbers")
            parentnumdom.setRange(nr2)

            childnumdom.setParent(parentnumdom)
            childnumdom.setStrict(False)

            self.assertEqual(childnumdom.contains(12.8), "cSELF")
            self.assertEqual(childnumdom.contains(72.2), "cPARENT")
            self.assertEqual(childnumdom.contains(100), "cNONE")

            self.assertFalse(childnumdom.isStrict())
            childnumdom.setStrict(True)
            self.assertTrue(childnumdom.isStrict())

            self.assertEqual(childnumdom.contains(72.5), "cNONE")

        def test_boundaries(self):
            nr = NumericRange(5.8, 60.2, 0.2)
            childnumdom = NumericDomain("numbers")
            childnumdom.setRange(nr)

            self.assertEqual(childnumdom.contains(6.0), "cSELF")
            self.assertEqual(childnumdom.contains(5.6), "cNONE")
            self.assertEqual(childnumdom.contains(60.2), "cSELF")
            self.assertEqual(childnumdom.contains(60.4), "cNONE")

        def test_parents(self):
            nr = NumericRange(5.8, 60.2, 0.2)
            childnumdom = NumericDomain("numbers")
            childnumdom.setRange(nr)
            nr2 = NumericRange(0, 80.4, 0.2)
            parentnumdom = NumericDomain("parentnumbers")
            parentnumdom.setRange(nr2)

            with self.assertRaises(IlwisException, msg="No parent domain found"):
                childnumdom.parent()

            childnumdom.setParent(parentnumdom)
            self.assertTrue(childnumdom.parent())

    #@ut.skip("temporarily")
    class TestInterval(ut.TestCase):
        def setUp(self):
            disconnectIssueLogger()
            connectIssueLogger()

        def test_containement(self):
            interrange = NumericItemRange()
            interrange.add(("sealevel", 40.0, 100.0))
            interrange.add(("dijks", 101.0, 151.0))

            childdom = ItemDomain(interrange)

            self.assertEqual(interrange.listAll(), [('sealevel', 'numericrange:40.0|100.0'), ('dijks', 'numericrange:101.0|151.0')])

            interrange.add(("by the sea", 152.0, 181.0, 5.0))
            parentdom = ItemDomain(interrange)

            childdom.setParent(parentdom)

            self.assertEqual(childdom.contains(40.0), "cSELF")
            self.assertEqual(childdom.contains(39.9), "cNONE")
            self.assertEqual(childdom.contains(161.0), "cNONE")

            childdom.setStrict(False)
            self.assertEqual(childdom.contains(161.0), "cPARENT")

        def test_parents(self):
            nir = NumericItemRange()
            nir.add(("sealevel", 40.0, 100.0, 5.0))
            childdom = ItemDomain(nir)

            nir.add(("by the sea", 151.0, 181.0, 5.0))
            parentdom = ItemDomain(nir)

            with self.assertRaises(IlwisException, msg="No parent domain found"):
                childdom.parent()

            childdom.setParent(parentdom)
            self.assertTrue(childdom.parent())

        def test_removeAndCount(self):
            nir = NumericItemRange()
            nir.add(("sealevel", 40.0, 100.0, 5.0))
            nir.add(("by the sea", 151.0, 181.0, 5.0))
            childdom = ItemDomain(nir)

            self.assertEqual(childdom.count(), 2)
            childdom.removeItem("sealevel")
            self.assertEqual(childdom.count(), 1)
            childdom.addItem(("sealevel", 185.0, 250.0, 5.0))
            self.assertEqual(childdom.count(), 2)

        def test_theme(self):
            nir = NumericItemRange()
            nir.add(("sealevel", 40.0, 100.0, 5.0))
            nir.add(("by the sea", 151.0, 181.0, 5.0))
            childdom = ItemDomain(nir)

            childdom.setTheme("Seastuff")
            self.assertEqual(childdom.theme(), "Seastuff")

    #@ut.skip("temporarily")
    class TestThematicDomain(ut.TestCase):
        def test_containement(self):
            tr = ThematicRange()
            tr.add("hound", "3.1", "Fierce doggy")
            tr.add("greyhound", "0.32", "the fast one")

            self.assertEqual(tr.listAll(), [("hound", "3.1", "Fierce doggy"), ("greyhound", "0.32", "the fast one")])

            td = ItemDomain(tr)

            tr.add(("foxhound", "2.4", "hunting foxes"))

            td2 = ItemDomain(tr)
            td.setParent(td2)
            td.setStrict(False)

            self.assertEqual(td.contains("hound"), "cSELF")
            self.assertEqual(td.contains("greyhound"), "cSELF")
            self.assertEqual(td.contains("foxhound"), "cPARENT")
            self.assertEqual(td.contains("ghosthound"), "cNONE")

        def test_parents(self):
            tr = ThematicRange()
            tr.add(("hound", "3.1", "Fierce doggy"))
            tr.add(("greyhound", "0.32", "the fast one"))

            td = ItemDomain(tr)

            with self.assertRaises(IlwisException, msg="No parent domain found"):
                td.parent()

            tr.add(("foxhound", "2.4", "hunting foxes"))
            td2 = ItemDomain(tr)
            td.setParent(td2)
            self.assertTrue(td.parent())

        def test_removeAndCount(self):
            tr = ThematicRange()
            tr.add(("hound", "3.1", "Fierce doggy"))
            tr.add(("greyhound", "0.32", "the fast one"))

            td = ItemDomain(tr)
            self.assertEqual(td.count(), 2)
            td.removeItem("hound")
            self.assertEqual(td.count(), 1)
            td.addItem(("fox", "34", "wait a second"))
            self.assertEqual(td.count(), 2)

        def test_theme(self):
            tr = ThematicRange()
            tr.add(("hound", "3.1", "Fierce doggy"))
            tr.add(("greyhound", "0.32", "the fast one"))

            td = ItemDomain(tr)

            td.setTheme("Hounds")
            self.assertEqual(td.theme(), "Hounds")

    #@ut.skip("temporarily")
    class TestNamedIdentifierDomain(ut.TestCase):
        def test_containement(self):
            nr = NamedItemRange()
            nr.add("Perth")
            nr.add("Darwin")

            self.assertEqual(nr.listAll(), ['Perth', 'Darwin'])

            nchild = ItemDomain(nr)

            nr.add("Broome")
            nparent = ItemDomain(nr)

            nchild.setParent(nparent)
            nchild.setStrict(False)

            self.assertEqual(nchild.contains("Perth"), "cSELF")
            self.assertEqual(nchild.contains("Darwin"), "cSELF")
            self.assertEqual(nchild.contains("Broome"), "cPARENT")
            self.assertEqual(nchild.contains("Adelaide"), "cNONE")

            nchild.setStrict(True)
            self.assertEqual(nchild.contains("Broome"), "cNONE")

        def test_parents(self):

            nr = NamedItemRange()
            nr.add("Perth")
            nr.add("Darwin")

            nchild = ItemDomain(nr)

            with self.assertRaises(IlwisException, msg="No parent domain found"):
                nchild.parent()

            nr.add("Broome")
            nparent = ItemDomain(nr)
            nchild.setParent(nparent)
            self.assertTrue(nchild.parent())

        def test_removeAndCount(self):
            nr = NamedItemRange()
            nr.add("Perth")
            nr.add("Darwin")

            namedDom = ItemDomain(nr)
            self.assertEqual(namedDom.count(), 2)
            namedDom.removeItem("Perth")
            self.assertEqual(namedDom.count(), 1)
            namedDom.addItem("Childers")
            self.assertEqual(namedDom.count(), 2)

        def test_theme(self):
            nr = NamedItemRange()
            nr.add("Perth")
            nr.add("Darwin")

            namedRange = ItemDomain(nr)

            namedRange.setTheme("Australian Cities")
            self.assertEqual(namedRange.theme(), "Australian Cities")

    # #@ut.skip("temporarily")
    # class TestIndexedIdentifierDomain(ut.TestCase):
    #     def test_containement(self):
    #         nr = IndexedItemRange()
    #         nr.add(("Perth", 1))
    #         nr.add(("Darwin", 2))
    #
    #         nchild = ItemDomain(nr)
    #
    #         nr.add(("Broome", 3))
    #
    #         nparent = ItemDomain(nr)
    #         nchild.setParent(nparent)
    #         nchild.setStrict(False)
    #
    #         self.assertEqual(nchild.contains("Perth"), "cSELF")
    #         self.assertEqual(nchild.contains("Darwin"), "cSELF")
    #         self.assertEqual(nchild.contains("Broome"), "cPARENT")
    #         self.assertEqual(nchild.contains("Adelaide"), "cNONE")
    #
    #         nchild.setStrict(True)
    #         self.assertEqual(nchild.contains("Broome"), "cNONE")
    #
    #     def test_parents(self):
    #
    #         nr = IndexedItemRange()
    #         nr.add(("Perth", 1))
    #         nr.add(("Darwin", 2))
    #
    #         nchild = ItemDomain(nr)
    #
    #         with self.assertRaises(IlwisException, msg="No parent domain found"):
    #             nchild.parent()
    #
    #         nr.add(("Broome", 3))
    #         nparent = ItemDomain(nr)
    #         nchild.setParent(nparent)
    #         self.assertTrue(nchild.parent())
    #
    #     def test_removeAndCount(self):
    #         nr = IndexedItemRange()
    #         nr.add(("Perth", 1))
    #         nr.add(("Darwin", 2))
    #
    #         namedDom = ItemDomain(nr)
    #         self.assertEqual(namedDom.count(), 2)
    #         namedDom.removeItem("Perth")
    #         self.assertEqual(namedDom.count(), 1)
    #         namedDom.addItem(("Childers", 4))
    #         self.assertEqual(namedDom.count(), 2)
    #
    #     def test_theme(self):
    #         nr = IndexedItemRange()
    #         nr.add(("Perth", 1))
    #         nr.add(("Darwin", 2))
    #
    #         namedDom = ItemDomain(nr)
    #
    #         namedDom.setTheme("Australian Cities")
    #         self.assertEqual(namedDom.theme(), "Australian Cities")

    #@ut.skip("temporarily")
    class TestColorDomain(ut.TestCase):
        def test_colorcontainement(self):
            color1 = Color(ColorModel.cmRGBA, (220.0, 20.0, 30.0, 200.0))
            color2 = Color(ColorModel.cmRGBA, (255.0, 80.0, 60.0, 240.0))
            color3 = Color(ColorModel.cmRGBA, (230.0, 60.0, 50.0, 240.0))

            col = ContinuousColorRange(color1, color2)
            self.assertTrue(bool(col))

            col2 = col.clone()
            self.assertTrue(bool(col2))

            col.defaultColorModel(ColorModel.cmRGBA)
            self.assertEqual(col.defaultColorModel(), ColorModel.cmRGBA)

            colDom = ColorDomain("testdomain")
            colDom.setRange(col)
            self.assertEqual(colDom.containsColor(color3), "cSELF")

        def test_colorCYMKA(self):
            color1 = Color(ColorModel.cmCYMKA, (0.6, 0.2, 0.16, 0.6, 1.0))
            color2 = Color(ColorModel.cmCYMKA, (0.9, 0.7, 0.5, 0.9, 1.0))
            color3 = Color(ColorModel.cmCYMKA, (0.77, 0.5, 0.4, 0.7, 1.0))

            col = ContinuousColorRange(color1, color2)
            self.assertTrue(bool(col))

            col2 = col.clone()
            self.assertTrue(bool(col2))

            col.defaultColorModel(ColorModel.cmCYMKA)
            self.assertEqual(col.defaultColorModel(), ColorModel.cmCYMKA)

            colDom = ColorDomain("testdomain")
            colDom.setRange(col)
            self.assertEqual(colDom.containsColor(color3), "cSELF")

        def test_colorHSLA(self):
            color1 = Color(ColorModel.cmHSLA, (100.0, 0.2, 0.16, 1.0))
            color2 = Color(ColorModel.cmHSLA, (300.0, 0.7, 0.5, 1.0))
            color3 = Color(ColorModel.cmHSLA, (177.0, 0.5, 0.4, 1.0))

            col = ContinuousColorRange(color1, color2)
            self.assertTrue(bool(col))

            col2 = col.clone()
            self.assertTrue(bool(col2))

            col.defaultColorModel(ColorModel.cmHSLA)
            self.assertEqual(col.defaultColorModel(), ColorModel.cmHSLA)

            colDom = ColorDomain("testdomain")
            colDom.setRange(col)
            self.assertEqual(colDom.containsColor(color3), "cSELF")

    class TestColorPalette(ut.TestCase):

        def test_range(self):
            col1 = Color(ColorModel.cmRGBA, (220.0, 20.0, 30.0, 200.0))
            col2 = Color(ColorModel.cmRGBA, (255.0, 80.0, 60.0, 240.0))
            col3 = Color(ColorModel.cmRGBA, (255.0, 80.0, 69.0, 240.0))

            colPal = ColorPalette()
            colPal.add(col1)
            self.assertEqual(colPal.count(), 1)
            colPal.add(col2)
            self.assertEqual(colPal.count(), 2)
            self.assertEqual(str(colPal.item(0)), 'RGBA(0.86,0.08,0.12,0.78)')
            name1 = colPal.itemByOrder(1).getName()
            self.assertFalse(colPal.containsColor(col3))
            colPal.add(col3)
            self.assertTrue(colPal.containsColor(col3))
            self.assertEqual(colPal.count(), 3)
            colPal.remove(name1)
            self.assertEqual(colPal.count(), 2)
            self.assertEqual(str(colPal.color(1)), 'RGBA(1,0.31,0.27,0.94)')

            colPal.clear()
            self.assertEqual(colPal.count(), 0)
            self.assertEqual(str(colPal.valueAt(1, colPal)), str(colPal.color(1)))


    #@ut.skip("temporarily")
    class TestTimeDomain(ut.TestCase):
        def test_containement(self):
            ti = TimeInterval(date(2014, 2, 17), date(2016, 2, 17))
            td = TimeDomain(ti)
            self.assertEqual(td.contains(date(2014, 5, 17)), "cSELF")
            self.assertEqual(td.contains(date(2014, 1, 17)), "cNONE")

        def test_parent(self):
            ti = TimeInterval(date(2014, 2, 17), date(2016, 2, 17))
            td = TimeDomain(ti)

            date1 = date(2014, 2, 17) - timedelta(365)
            date2 = date(2016, 2, 17) + timedelta(365)
            tip = TimeInterval(date1, date2)
            tdp = TimeDomain(tip)
            td.setParent(tdp)

            td.setStrict(False)
            self.assertEqual(td.contains(date(2013, 5, 17)), "cPARENT")

        def test_begin_end(self):
            date1 = date(2014, 2, 17) - timedelta(365)
            date2 = date(2016, 2, 17) + timedelta(365)
            tip = TimeInterval(date1, date2)
            self.assertEqual(str(tip.begin()), "2013-02-17 00:00:00")
            self.assertEqual(str(tip.end()), "2017-02-16 00:00:00")

            tip.begin(date(2011, 6, 23))
            tip.end(date(2015, 2, 10))
            self.assertEqual(str(tip.begin()), "2011-06-23 00:00:00")
            self.assertEqual(str(tip.end()), "2015-02-10 00:00:00")

    class TestDataDefinition(ut.TestCase):
        def setUp(self):
            try:
                disconnectIssueLogger()
                Engine.setWorkingCatalog(workingDir + rasterDir)
                connectIssueLogger()
            except IlwisException:
                self.skipTest("could not set working directory!")

        def test_datdefNumeric(self):
            numRan = NumericRange(23.0, 3453.4, 0.2)
            numDom = NumericDomain()
            numDom.setRange(numRan)
            datdef1 = DataDefinition(numDom, numRan)
            datdef2 = DataDefinition(numDom)

            self.assertEqual(str(datdef1), str(datdef2))

            self.assertTrue(bool(datdef1))
            self.assertTrue(bool(datdef2))
            self.assertTrue(bool(datdef2.range()))

            self.assertEqual(numRan.ilwisType(), datdef1.range().ilwisType())
            self.assertEqual(str(numRan), str(datdef1.range()))
            self.assertEqual(str(numDom), str(datdef1.domain()))
            self.assertEqual(str(numRan), str(datdef2.range()))
            self.assertEqual(str(numDom), str(datdef2.domain()))

            numRanDiff = NumericRange(1.0, 200.0)
            numDomDiff = NumericDomain()
            numDomDiff.setRange(numRan)
            datdef3 = DataDefinition(numDomDiff)
            datdef2 = datdef3
            self.assertEqual(str(datdef3), str(datdef2))

            datdef1.range(numRanDiff)
            datdef1.domain(numDomDiff)
            self.assertEqual(str(datdef1), str(datdef3))

        def test_datdefThem(self):
            themRan = ThematicRange()
            themRan.add("Sea", "SE", "Area covered by the sea")
            themRan.add("Sand", "SA", "Area covered by sand")
            itemDom = ItemDomain(themRan)
            datdef1 = DataDefinition(itemDom, themRan)
            datdef2 = DataDefinition(itemDom)

            self.assertEqual(str(datdef1), str(datdef2))

            self.assertTrue(bool(datdef1))
            self.assertTrue(bool(datdef2))
            self.assertTrue(bool(datdef2.range()))

            self.assertEqual(themRan.ilwisType(), datdef1.range().ilwisType())
            self.assertEqual(str(themRan), str(datdef1.range()))
            self.assertEqual(str(itemDom), str(datdef1.domain()))
            self.assertEqual(str(themRan), str(datdef2.range()))
            self.assertEqual(str(itemDom), str(datdef2.domain()))

            themRanDiff = ThematicRange()
            themRanDiff.add("Blabla", "BB", "Two Blas")
            themRanDiff.add("Blablabla", "BBB", "Three Blas")
            itemDomDiff = ItemDomain(themRanDiff)
            datdef3 = DataDefinition(itemDomDiff)
            datdef2 = datdef3
            self.assertEqual(str(datdef3), str(datdef2))

            datdef1.range(themRanDiff)
            datdef1.domain(itemDomDiff)
            self.assertEqual(str(datdef1), str(datdef3))

        def test_datdefIdent(self):
            identRan = NamedItemRange()
            identRan.add("Perth")
            identRan.add("Darwin")
            itemDom = ItemDomain(identRan)
            datdef1 = DataDefinition(itemDom, identRan)
            datdef2 = DataDefinition(itemDom)

            self.assertEqual(str(datdef1), str(datdef2))

            self.assertTrue(bool(datdef1))
            self.assertTrue(bool(datdef2))
            self.assertTrue(bool(datdef2.range()))

            self.assertEqual(identRan.ilwisType(), datdef1.range().ilwisType())
            self.assertEqual(str(identRan), str(datdef1.range()))
            self.assertEqual(str(itemDom), str(datdef1.domain()))
            self.assertEqual(str(identRan), str(datdef2.range()))
            self.assertEqual(str(itemDom), str(datdef2.domain()))

            identRanDiff = NamedItemRange()
            identRanDiff.add("Berlin")
            identRanDiff.add("Hamburg")
            itemDomDiff = ItemDomain(identRanDiff)
            datdef3 = DataDefinition(itemDomDiff)
            datdef2 = datdef3
            self.assertEqual(str(datdef3), str(datdef2))

            datdef1.range(identRanDiff)
            datdef1.domain(itemDomDiff)
            self.assertEqual(str(datdef1), str(datdef3))

        #@ut.skip("temporairly")
        def test_datdefInterval(self):
            interrange = NumericItemRange()
            interrange.add(("sealevel", 40.0, 100.0))
            interrange.add(("dijks", 100.0, 151.0))
            itemDom = ItemDomain(interrange)
            datdef1 = DataDefinition(itemDom, interrange)
            datdef2 = DataDefinition(itemDom)

            self.assertEqual(str(datdef1), str(datdef2))

            self.assertTrue(bool(datdef1))
            self.assertTrue(bool(datdef2))
            self.assertTrue(bool(datdef2.range()))

            self.assertEqual(interrange.ilwisType(), datdef1.range().ilwisType())
            self.assertEqual(str(interrange), str(datdef1.range()))
            self.assertEqual(str(itemDom), str(datdef1.domain()))
            self.assertEqual(str(interrange), str(datdef2.range()))
            self.assertEqual(str(itemDom), str(datdef2.domain()))

            interRanDiff = NumericItemRange()
            interRanDiff.add(("low", 0.0, 50.0))
            interRanDiff.add(("high", 50.0, 250.0))
            itemDomDiff = ItemDomain(interRanDiff)
            datdef3 = DataDefinition(itemDomDiff)
            datdef2 = datdef3
            self.assertEqual(str(datdef3), str(datdef2))

            datdef1.range(interRanDiff)
            datdef1.domain(itemDomDiff)
            self.assertEqual(str(datdef1), str(datdef3))

        def test_datdefTime(self):
            interval = TimeInterval(date(2014, 2, 17), date(2016, 2, 17))
            timeDom = TimeDomain(interval)
            datdef1 = DataDefinition(timeDom, interval)
            datdef2 = DataDefinition(timeDom)

            self.assertEqual(str(datdef1), str(datdef2))

            self.assertTrue(bool(datdef1))
            self.assertTrue(bool(datdef2))
            self.assertTrue(bool(datdef2.range()))

            self.assertEqual(interval.ilwisType(), datdef1.range().ilwisType())
            self.assertEqual(str(interval), str(datdef1.range()))
            self.assertEqual(str(timeDom), str(datdef1.domain()))
            self.assertEqual(str(interval), str(datdef2.range()))
            self.assertEqual(str(timeDom), str(datdef2.domain()))

            intervalDiff = TimeInterval(date(2012, 8, 17), date(2013, 7, 10))
            timeDomDiff = TimeDomain(intervalDiff)
            datdef3 = DataDefinition(timeDomDiff)
            datdef2 = datdef3
            self.assertEqual(str(datdef3), str(datdef2))

            datdef1.range(intervalDiff)
            datdef1.domain(timeDomDiff)
            self.assertEqual(str(datdef1), str(datdef3))

        def test_datdefColor(self):
            color1 = Color(ColorModel.cmRGBA, (220.0, 20.0, 30.0, 200.0))
            color2 = Color(ColorModel.cmRGBA, (255.0, 80.0, 60.0, 240.0))
            col = ContinuousColorRange(color1, color2)
            colDom = ColorDomain()
            colDom.setRange(col)

            datdef1 = DataDefinition(colDom, col)
            datdef2 = DataDefinition(colDom)

            self.assertEqual(str(datdef1), str(datdef2))

            self.assertTrue(bool(datdef1))
            self.assertTrue(bool(datdef2))
            self.assertTrue(bool(datdef2.range()))

            self.assertEqual(col.ilwisType(), datdef1.range().ilwisType())
            self.assertEqual(str(col), str(datdef1.range()))
            self.assertEqual(str(colDom), str(datdef1.domain()))
            self.assertEqual(str(col), str(datdef2.range()))
            self.assertEqual(str(colDom), str(datdef2.domain()))

            color3 = Color(ColorModel.cmRGBA, (130.0, 0.0, 10.0, 100.0))
            color4 = Color(ColorModel.cmRGBA, (230.0, 60.0, 50.0, 240.0))
            colDiff = ContinuousColorRange(color3, color4)
            colDomDiff = ColorDomain()
            colDomDiff.setRange(colDiff)
            datdef3 = DataDefinition(colDomDiff)
            datdef2 = datdef3
            self.assertEqual(str(datdef3), str(datdef2))

            datdef1.range(colDiff)
            datdef1.domain(colDomDiff)
            self.assertEqual(str(datdef1), str(datdef3))

    #@ut.skip("temporarily")
    class TestSubDefinitionRaster(ut.TestCase):
        def setUp(self):
            try:
                disconnectIssueLogger()
                Engine.setWorkingCatalog(workingDir + rasterDir)
                connectIssueLogger()
            except IlwisException:
                self.skipTest("could not set working directory!")

        #@ut.skip("temporarily")
        def test_numericIndex(self):
            rc = RasterCoverage("average_monthly_temperature.mpl")

            rc2 = RasterCoverage()

            numRan = NumericRange(0.0, 100.0)
            numDom = NumericDomain()
            numDom.setRange(numRan)
            rc2.setStackDefinition(numDom, (0, 1))

            band3 = rc.band(3)
            band4 = rc.band(4)

            rc2.addBand(0, band3)
            rc2.addBand(1, band4)
            self.assertEqual(str(rc2.indexes()), "('0', '1')")

            # vals1 = []
            vals1 = 0
            for val1 in band3:
            #    vals1.append(val1)
                vals1 += val1

            # vals2 = []
            vals2 = 0
            rc2Band0 = rc2.band(0)
            for val2 in rc2Band0:
            #    vals2.append(val2)
                vals2 += val2

            self.assertTrue(vals1 == vals2)

            vals1 = None
            vals2 = None
            band3 = None
            band4 = None
            rc2Band0 = None
            rc2 = None
            rc = None
            numRan = None
            numDom = None
            gc.collect()

        def test_timeIndex(self):
            rc = RasterCoverage("average_monthly_temperature.mpl")

            rc2 = RasterCoverage()

            interval = TimeInterval(date(2014, 2, 17), date(2016, 2, 17))
            timeDom = TimeDomain(interval)
            rc2.setStackDefinition(timeDom, (date(2015, 2, 17), date(2015, 3, 17)))

            band3 = rc.band(3)
            band4 = rc.band(4)

            rc2.addBand(date(2015, 2, 17), band3)
            rc2.addBand(date(2015, 3, 17), band4)
            #self.assertEqual(str(rc2.indexes()), "(datetime.date(2015, 2, 17), datetime.date(2015, 3, 17))")

            # vals1 = []
            vals1 = 0
            for val1 in band3:
            #    vals1.append(val1)
                vals1 += val1

            # vals2 = []
            vals2 = 0
            rc2Band0 = rc2.band(date(2015, 2, 17))
            for val2 in rc2Band0:
            #    vals2.append(val2)
                vals2+= val2

            self.assertTrue(vals1 == vals2)

            vals1 = None
            vals2 = None
            band3 = None
            band4 = None
            rc2Band0 = None
            rc2 = None
            rc = None
            numRan = None
            numDom = None
            gc.collect()


    #@ut.skip("temporarily")
    class TestSpreadsheet(ut.TestCase):
        def setUp(self):
            try:
                disconnectIssueLogger()
                Engine.setWorkingCatalog(workingDir + spreadDir)
                connectIssueLogger()
            except IlwisException:
                self.skipTest("could not set working directory!")

        def test_columns(self):
            opt = IOOptions("headerline", 0)
            tab = Table("simpleTest1.xlsx", opt)
            self.assertEqual(tab.columns(), ('column1', 'column2', 'column3'))
            self.assertEqual(tab.columnCount(), 3)
            self.assertEqual(tab.column(0), tab.column("column1"))
            self.assertEqual(tab.column(0), ('cell11', 'cell21', 'cell31', 'cell41'))
            self.assertEqual(tab.columnIndex("column3"), 2)

        def test_records(self):
            opt = IOOptions("headerline", 0)
            tab = Table("simpleLibre.ods", opt)
            self.assertEqual(tab.record(1), ('cell21', 'cell22', 345.3))
            self.assertEqual(tab.recordCount(), 3)

        def test_access(self):
            opt = IOOptions("headerline", 0)
            tab = Table("simpleTest1.xlsx", opt)

    class TestVertexIterator(ut.TestCase):
        def setUp(self):
            try:
                disconnectIssueLogger()
                Engine.setWorkingCatalog(workingDir + featureDir)
                connectIssueLogger()
            except IlwisException:
                connectIssueLogger()
                self.skipTest("could not set working directory!")

        def test_vertexiterator(self):
            fc = FeatureCoverage("drainage.shp")

            for feat in fc:
                baseIt = VertexIterator(feat.geometry())
                beginIt = baseIt.begin()
                endIt = baseIt.end()

                self.assertTrue(beginIt < endIt)
                self.assertTrue(beginIt <= endIt)
                self.assertFalse(beginIt > endIt)
                self.assertFalse(beginIt >= endIt)

                listA = []
                while beginIt != endIt:
                    listA.append(beginIt.current())
                    beginIt += 1

                self.assertTrue(beginIt == endIt)

                vertIt = iter(feat)
                listB = []
                for vert in vertIt:
                    listB.append(vert)

            self.assertTrue(len(listA) > 0)
            self.assertTrue(listB == listA)


    #here you can chose which test case will be executed
    if __name__ == "__main__":
        ut.main(defaultTest=None, verbosity=2)

except ImportError as e:
    print(e)
