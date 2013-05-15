#-------------------------------------------------
#
# Project created by QtCreator 2013-03-29T09:57:39
#
#-------------------------------------------------

CONFIG += plugin

QT       -= gui

TARGET = proj4impl
TEMPLATE = lib

include(global.pri)

DESTDIR = $$PWD/../libraries/$$PLATFORM$$CONF/$$TARGET
DLLDESTDIR = $$PWD/../output/$$PLATFORM$$CONF/bin/extensions/$$TARGET

DEFINES += PROJECTIONIMPLPROJ4_LIBRARY

SOURCES += \
    projectionimplproj4/prjimplproj4.cpp \
    projectionimplproj4/prjimplfactoryproj4.cpp \
    ../external/proj4/vector1.c \
    ../external/proj4/rtodms.c \
    ../external/proj4/proj_rouss.c \
    ../external/proj4/proj_mdist.c \
    ../external/proj4/proj_etmerc.c \
    ../external/proj4/pj_zpoly1.c \
    ../external/proj4/PJ_wink2.c \
    ../external/proj4/PJ_wink1.c \
    ../external/proj4/PJ_wag7.c \
    ../external/proj4/PJ_wag3.c \
    ../external/proj4/PJ_wag2.c \
    ../external/proj4/PJ_vandg4.c \
    ../external/proj4/PJ_vandg2.c \
    ../external/proj4/PJ_vandg.c \
    ../external/proj4/pj_utils.c \
    ../external/proj4/PJ_urmfps.c \
    ../external/proj4/PJ_urm5.c \
    ../external/proj4/pj_units.c \
    ../external/proj4/pj_tsfn.c \
    ../external/proj4/pj_transform.c \
    ../external/proj4/PJ_tpeqd.c \
    ../external/proj4/PJ_tmerc.c \
    ../external/proj4/PJ_tcea.c \
    ../external/proj4/PJ_tcc.c \
    ../external/proj4/PJ_sts.c \
    ../external/proj4/pj_strerrno.c \
    ../external/proj4/PJ_sterea.c \
    ../external/proj4/PJ_stere.c \
    ../external/proj4/PJ_somerc.c \
    ../external/proj4/PJ_sconics.c \
    ../external/proj4/PJ_rpoly.c \
    ../external/proj4/PJ_robin.c \
    ../external/proj4/pj_release.c \
    ../external/proj4/pj_qsfn.c \
    ../external/proj4/PJ_putp6.c \
    ../external/proj4/PJ_putp5.c \
    ../external/proj4/PJ_putp4p.c \
    ../external/proj4/PJ_putp3.c \
    ../external/proj4/PJ_putp2.c \
    ../external/proj4/pj_pr_list.c \
    ../external/proj4/PJ_poly.c \
    ../external/proj4/pj_phi2.c \
    ../external/proj4/pj_param.c \
    ../external/proj4/PJ_ortho.c \
    ../external/proj4/pj_open_lib.c \
    ../external/proj4/PJ_omerc.c \
    ../external/proj4/PJ_oea.c \
    ../external/proj4/PJ_ocea.c \
    ../external/proj4/PJ_ob_tran.c \
    ../external/proj4/PJ_nzmg.c \
    ../external/proj4/PJ_nsper.c \
    ../external/proj4/PJ_nocol.c \
    ../external/proj4/PJ_nell_h.c \
    ../external/proj4/PJ_nell.c \
    ../external/proj4/PJ_natearth.c \
    ../external/proj4/pj_mutex.c \
    ../external/proj4/pj_msfn.c \
    ../external/proj4/PJ_moll.c \
    ../external/proj4/PJ_mod_ster.c \
    ../external/proj4/pj_mlfn.c \
    ../external/proj4/PJ_mill.c \
    ../external/proj4/PJ_merc.c \
    ../external/proj4/PJ_mbtfpq.c \
    ../external/proj4/PJ_mbtfpp.c \
    ../external/proj4/PJ_mbt_fps.c \
    ../external/proj4/pj_malloc.c \
    ../external/proj4/PJ_lsat.c \
    ../external/proj4/PJ_loxim.c \
    ../external/proj4/pj_log.c \
    ../external/proj4/pj_list.c \
    ../external/proj4/PJ_lcca.c \
    ../external/proj4/PJ_lcc.c \
    ../external/proj4/pj_latlong.c \
    ../external/proj4/PJ_lask.c \
    ../external/proj4/PJ_larr.c \
    ../external/proj4/PJ_lagrng.c \
    ../external/proj4/PJ_laea.c \
    ../external/proj4/PJ_labrd.c \
    ../external/proj4/PJ_krovak.c \
    ../external/proj4/PJ_isea.c \
    ../external/proj4/pj_inv.c \
    ../external/proj4/pj_initcache.c \
    ../external/proj4/pj_init.c \
    ../external/proj4/PJ_imw_p.c \
    ../external/proj4/PJ_igh.c \
    ../external/proj4/PJ_healpix.c \
    ../external/proj4/PJ_hatano.c \
    ../external/proj4/PJ_hammer.c \
    ../external/proj4/PJ_gstmerc.c \
    ../external/proj4/pj_gridlist.c \
    ../external/proj4/pj_gridinfo.c \
    ../external/proj4/PJ_goode.c \
    ../external/proj4/PJ_gnom.c \
    ../external/proj4/PJ_gn_sinu.c \
    ../external/proj4/PJ_gins8.c \
    ../external/proj4/PJ_geos.c \
    ../external/proj4/pj_geocent.c \
    ../external/proj4/pj_gauss.c \
    ../external/proj4/PJ_gall.c \
    ../external/proj4/pj_fwd.c \
    ../external/proj4/PJ_fouc_s.c \
    ../external/proj4/PJ_fahey.c \
    ../external/proj4/pj_factors.c \
    ../external/proj4/pj_errno.c \
    ../external/proj4/PJ_eqdc.c \
    ../external/proj4/PJ_eqc.c \
    ../external/proj4/pj_ellps.c \
    ../external/proj4/pj_ell_set.c \
    ../external/proj4/PJ_eck5.c \
    ../external/proj4/PJ_eck4.c \
    ../external/proj4/PJ_eck3.c \
    ../external/proj4/PJ_eck2.c \
    ../external/proj4/PJ_eck1.c \
    ../external/proj4/pj_deriv.c \
    ../external/proj4/PJ_denoy.c \
    ../external/proj4/pj_datums.c \
    ../external/proj4/pj_datum_set.c \
    ../external/proj4/pj_ctx.c \
    ../external/proj4/PJ_crast.c \
    ../external/proj4/PJ_collg.c \
    ../external/proj4/PJ_chamb.c \
    ../external/proj4/PJ_cea.c \
    ../external/proj4/PJ_cc.c \
    ../external/proj4/PJ_cass.c \
    ../external/proj4/PJ_bonne.c \
    ../external/proj4/PJ_boggs.c \
    ../external/proj4/PJ_bipc.c \
    ../external/proj4/PJ_bacon.c \
    ../external/proj4/pj_auth.c \
    ../external/proj4/PJ_august.c \
    ../external/proj4/pj_apply_vgridshift.c \
    ../external/proj4/pj_apply_gridshift.c \
    ../external/proj4/PJ_aitoff.c \
    ../external/proj4/PJ_airy.c \
    ../external/proj4/PJ_aeqd.c \
    ../external/proj4/PJ_aea.c \
    ../external/proj4/p_series.c \
    ../external/proj4/nad_intr.c \
    ../external/proj4/nad_init.c \
    ../external/proj4/nad_cvt.c \
    ../external/proj4/mk_cheby.c \
    ../external/proj4/geod_set.c \
    ../external/proj4/geod_inv.c \
    ../external/proj4/geod_for.c \
    ../external/proj4/geocent.c \
    ../external/proj4/gen_cheb.c \
    ../external/proj4/emess.c \
    ../external/proj4/dmstor.c \
    ../external/proj4/biveval.c \
    ../external/proj4/bchgen.c \
    ../external/proj4/bch2bps.c \
    ../external/proj4/adjlon.c \
    ../external/proj4/aasincos.c \
    projectionimplproj4/projectionmodule.cpp

HEADERS +=\
    projectionimplproj4/projectionmodule.h \
    projectionimplproj4/prjimplproj4.h \
    projectionimplproj4/prjmplfactoryproj4.h \
    ../external/proj4/projects.h \
    ../external/proj4/proj_api.h \
    ../external/proj4/pj_list.h \
    ../external/proj4/geodesic.h \
    ../external/proj4/geocent.h \
    ../external/proj4/emess.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

OTHER_FILES += \
    sources/projectionimplproj4/proj4/proj_config.h.in \
    sources/projectionimplproj4/projectionImplProj4.json \
    ../external/proj4/proj_config.h.in \
    projectionimplproj4/projectionImplProj4.json
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libraries/$$PLATFORM$$CONF/core/ -lilwiscore

INCLUDEPATH += $$PWD/projectionimplproj4 \
                $$PWD/../external/proj4
DEPENDPATH += $$PWD/projectionimplproj4
