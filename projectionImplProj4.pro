#-------------------------------------------------
#
# Project created by QtCreator 2013-03-29T09:57:39
#
#-------------------------------------------------

CONFIG += plugin

TARGET = proj4impl
TEMPLATE = lib

include(global.pri)

QMAKE_CXXFLAGS += -Wno-unused-parameter

DEFINES += PROJECTIONIMPLPROJ4_LIBRARY

SOURCES += \
    projectionimplproj4/prjimplproj4.cpp \
    projectionimplproj4/prjimplfactoryproj4.cpp \
    projectionimplproj4/projectionmodule.cpp \
    projectionimplproj4/proj4/src/aasincos.c \
    projectionimplproj4/proj4/src/adjlon.c \
    projectionimplproj4/proj4/src/bch2bps.c \
    projectionimplproj4/proj4/src/bchgen.c \
    projectionimplproj4/proj4/src/biveval.c \
    projectionimplproj4/proj4/src/cs2cs.c \
    projectionimplproj4/proj4/src/dmstor.c \
    projectionimplproj4/proj4/src/emess.c \
    projectionimplproj4/proj4/src/gen_cheb.c \
    projectionimplproj4/proj4/src/geocent.c \
    projectionimplproj4/proj4/src/geod_for.c \
    projectionimplproj4/proj4/src/geod_inv.c \
    projectionimplproj4/proj4/src/geod_set.c \
    projectionimplproj4/proj4/src/hypot.c \
    projectionimplproj4/proj4/src/mk_cheby.c \
    projectionimplproj4/proj4/src/nad_cvt.c \
    projectionimplproj4/proj4/src/nad_init.c \
    projectionimplproj4/proj4/src/nad_intr.c \
    projectionimplproj4/proj4/src/p_series.c \
    projectionimplproj4/proj4/src/PJ_aea.c \
    projectionimplproj4/proj4/src/PJ_aeqd.c \
    projectionimplproj4/proj4/src/PJ_airy.c \
    projectionimplproj4/proj4/src/PJ_aitoff.c \
    projectionimplproj4/proj4/src/pj_apply_gridshift.c \
    projectionimplproj4/proj4/src/pj_apply_vgridshift.c \
    projectionimplproj4/proj4/src/PJ_august.c \
    projectionimplproj4/proj4/src/pj_auth.c \
    projectionimplproj4/proj4/src/PJ_bacon.c \
    projectionimplproj4/proj4/src/PJ_bipc.c \
    projectionimplproj4/proj4/src/PJ_boggs.c \
    projectionimplproj4/proj4/src/PJ_bonne.c \
    projectionimplproj4/proj4/src/PJ_cass.c \
    projectionimplproj4/proj4/src/PJ_cc.c \
    projectionimplproj4/proj4/src/PJ_cea.c \
    projectionimplproj4/proj4/src/PJ_chamb.c \
    projectionimplproj4/proj4/src/PJ_collg.c \
    projectionimplproj4/proj4/src/PJ_crast.c \
    projectionimplproj4/proj4/src/pj_ctx.c \
    projectionimplproj4/proj4/src/pj_datum_set.c \
    projectionimplproj4/proj4/src/pj_datums.c \
    projectionimplproj4/proj4/src/PJ_denoy.c \
    projectionimplproj4/proj4/src/pj_deriv.c \
    projectionimplproj4/proj4/src/PJ_eck1.c \
    projectionimplproj4/proj4/src/PJ_eck2.c \
    projectionimplproj4/proj4/src/PJ_eck3.c \
    projectionimplproj4/proj4/src/PJ_eck4.c \
    projectionimplproj4/proj4/src/PJ_eck5.c \
    projectionimplproj4/proj4/src/pj_ell_set.c \
    projectionimplproj4/proj4/src/pj_ellps.c \
    projectionimplproj4/proj4/src/PJ_eqc.c \
    projectionimplproj4/proj4/src/PJ_eqdc.c \
    projectionimplproj4/proj4/src/pj_errno.c \
    projectionimplproj4/proj4/src/pj_factors.c \
    projectionimplproj4/proj4/src/PJ_fahey.c \
    projectionimplproj4/proj4/src/PJ_fouc_s.c \
    projectionimplproj4/proj4/src/pj_fwd.c \
    projectionimplproj4/proj4/src/PJ_gall.c \
    projectionimplproj4/proj4/src/pj_gauss.c \
    projectionimplproj4/proj4/src/pj_geocent.c \
    projectionimplproj4/proj4/src/PJ_geos.c \
    projectionimplproj4/proj4/src/PJ_gins8.c \
    projectionimplproj4/proj4/src/PJ_gn_sinu.c \
    projectionimplproj4/proj4/src/PJ_gnom.c \
    projectionimplproj4/proj4/src/PJ_goode.c \
    projectionimplproj4/proj4/src/pj_gridinfo.c \
    projectionimplproj4/proj4/src/pj_gridlist.c \
    projectionimplproj4/proj4/src/PJ_gstmerc.c \
    projectionimplproj4/proj4/src/PJ_hammer.c \
    projectionimplproj4/proj4/src/PJ_hatano.c \
    projectionimplproj4/proj4/src/PJ_healpix.c \
    projectionimplproj4/proj4/src/PJ_igh.c \
    projectionimplproj4/proj4/src/PJ_imw_p.c \
    projectionimplproj4/proj4/src/pj_init.c \
    projectionimplproj4/proj4/src/pj_initcache.c \
    projectionimplproj4/proj4/src/pj_inv.c \
    projectionimplproj4/proj4/src/PJ_isea.c \
    projectionimplproj4/proj4/src/PJ_krovak.c \
    projectionimplproj4/proj4/src/PJ_labrd.c \
    projectionimplproj4/proj4/src/PJ_laea.c \
    projectionimplproj4/proj4/src/PJ_lagrng.c \
    projectionimplproj4/proj4/src/PJ_larr.c \
    projectionimplproj4/proj4/src/PJ_lask.c \
    projectionimplproj4/proj4/src/pj_latlong.c \
    projectionimplproj4/proj4/src/PJ_lcc.c \
    projectionimplproj4/proj4/src/PJ_lcca.c \
    projectionimplproj4/proj4/src/pj_list.c \
    projectionimplproj4/proj4/src/pj_log.c \
    projectionimplproj4/proj4/src/PJ_loxim.c \
    projectionimplproj4/proj4/src/PJ_lsat.c \
    projectionimplproj4/proj4/src/pj_malloc.c \
    projectionimplproj4/proj4/src/PJ_mbt_fps.c \
    projectionimplproj4/proj4/src/PJ_mbtfpp.c \
    projectionimplproj4/proj4/src/PJ_mbtfpq.c \
    projectionimplproj4/proj4/src/PJ_merc.c \
    projectionimplproj4/proj4/src/PJ_mill.c \
    projectionimplproj4/proj4/src/pj_mlfn.c \
    projectionimplproj4/proj4/src/PJ_mod_ster.c \
    projectionimplproj4/proj4/src/PJ_moll.c \
    projectionimplproj4/proj4/src/pj_msfn.c \
    projectionimplproj4/proj4/src/pj_mutex.c \
    projectionimplproj4/proj4/src/PJ_natearth.c \
    projectionimplproj4/proj4/src/PJ_nell.c \
    projectionimplproj4/proj4/src/PJ_nell_h.c \
    projectionimplproj4/proj4/src/PJ_nocol.c \
    projectionimplproj4/proj4/src/PJ_nsper.c \
    projectionimplproj4/proj4/src/PJ_nzmg.c \
    projectionimplproj4/proj4/src/PJ_ob_tran.c \
    projectionimplproj4/proj4/src/PJ_ocea.c \
    projectionimplproj4/proj4/src/PJ_oea.c \
    projectionimplproj4/proj4/src/PJ_omerc.c \
    projectionimplproj4/proj4/src/pj_open_lib.c \
    projectionimplproj4/proj4/src/PJ_ortho.c \
    projectionimplproj4/proj4/src/pj_param.c \
    projectionimplproj4/proj4/src/pj_phi2.c \
    projectionimplproj4/proj4/src/PJ_poly.c \
    projectionimplproj4/proj4/src/pj_pr_list.c \
    projectionimplproj4/proj4/src/PJ_putp2.c \
    projectionimplproj4/proj4/src/PJ_putp3.c \
    projectionimplproj4/proj4/src/PJ_putp4p.c \
    projectionimplproj4/proj4/src/PJ_putp5.c \
    projectionimplproj4/proj4/src/PJ_putp6.c \
    projectionimplproj4/proj4/src/pj_qsfn.c \
    projectionimplproj4/proj4/src/pj_release.c \
    projectionimplproj4/proj4/src/PJ_robin.c \
    projectionimplproj4/proj4/src/PJ_rpoly.c \
    projectionimplproj4/proj4/src/PJ_sconics.c \
    projectionimplproj4/proj4/src/PJ_somerc.c \
    projectionimplproj4/proj4/src/PJ_stere.c \
    projectionimplproj4/proj4/src/PJ_sterea.c \
    projectionimplproj4/proj4/src/pj_strerrno.c \
    projectionimplproj4/proj4/src/PJ_sts.c \
    projectionimplproj4/proj4/src/PJ_tcc.c \
    projectionimplproj4/proj4/src/PJ_tcea.c \
    projectionimplproj4/proj4/src/PJ_tmerc.c \
    projectionimplproj4/proj4/src/PJ_tpeqd.c \
    projectionimplproj4/proj4/src/pj_transform.c \
    projectionimplproj4/proj4/src/pj_tsfn.c \
    projectionimplproj4/proj4/src/pj_units.c \
    projectionimplproj4/proj4/src/PJ_urm5.c \
    projectionimplproj4/proj4/src/PJ_urmfps.c \
    projectionimplproj4/proj4/src/pj_utils.c \
    projectionimplproj4/proj4/src/PJ_vandg.c \
    projectionimplproj4/proj4/src/PJ_vandg2.c \
    projectionimplproj4/proj4/src/PJ_vandg4.c \
    projectionimplproj4/proj4/src/PJ_wag2.c \
    projectionimplproj4/proj4/src/PJ_wag3.c \
    projectionimplproj4/proj4/src/PJ_wag7.c \
    projectionimplproj4/proj4/src/PJ_wink1.c \
    projectionimplproj4/proj4/src/PJ_wink2.c \
    projectionimplproj4/proj4/src/pj_zpoly1.c \
    projectionimplproj4/proj4/src/proj_etmerc.c \
    projectionimplproj4/proj4/src/proj_mdist.c \
    projectionimplproj4/proj4/src/proj_rouss.c \
    projectionimplproj4/proj4/src/rtodms.c \
    projectionimplproj4/proj4/src/vector1.c

OTHER_FILES += \
    projectionimplproj4/proj4/src/proj_config.h.in \
    projectionimplproj4/projectionImplProj4.json

LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lilwiscore

win32{
    DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
    LIBS += -L$$GEOSLIB -llibgeos
}

linux{
    LIBS +=  -L$$PWD/../libraries/$$PLATFORM$$CONF/ -lgeos-3.4.2
}

INCLUDEPATH +=  $$PWD/../external/geos \
                $$PWD/projectionimplproj4 \
                $$PWD/projectionimplproj4/proj4/src
DEPENDPATH +=   $$PWD/../external/geos \
                $$PWD/projectionimplproj4

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/extensions/$$TARGET

HEADERS += \
    projectionimplproj4/projectionmodule.h \
    projectionimplproj4/prjimplproj4.h \
    projectionimplproj4/prjmplfactoryproj4.h \
    projectionimplproj4/proj4/src/emess.h \
    projectionimplproj4/proj4/src/geocent.h \
#    projectionimplproj4/proj4/src/geod_interface.h \
    projectionimplproj4/proj4/src/geodesic.h \
    projectionimplproj4/proj4/src/nad_list.h \
    projectionimplproj4/proj4/src/org_proj4_PJ.h \
    projectionimplproj4/proj4/src/org_proj4_Projections.h \
    projectionimplproj4/proj4/src/pj_list.h \
    projectionimplproj4/proj4/src/proj_api.h \
    projectionimplproj4/proj4/src/projects.h


linux{
    dependencies.files = $$GEOSLIB/libgeos-3.4.2.so
    dependencies.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

    target.path = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET
    INSTALLS += target dependencies
}
