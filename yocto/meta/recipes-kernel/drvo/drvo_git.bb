SUMMARY = "DRVO final"
DESCRIPTION = "DRVO final"
HOMEPAGE = "https://github.com/Maxvdh5/eindopdracht_driver"
LICENSE="CLOSED"
LIC_FILES_CHKSUM=""

SRC_URI = "git://github.com/Maxvdh5/eindopdracht_driver;branch=master"
SRCREV = "43b39b3c651e0c0419a74cf1d856a70a0cdcbedf"

S = "${WORKDIR}/git"

inherit module kernel-module-split

DEPENDS = "virtual/kernel"

EXTRA_OEMAKE_append = " \
    KERNELDIR=${STAGING_KERNEL_DIR} \
    "

MAKE_TARGETS = "module"

MODULE_NAME = "ADC_driver"

PKG_${PN} = "kernel-module-${MODULE_NAME}"

module_do_install() {
    install -d ${D}/lib/modules/${KERNEL_VERSION}/kernel
    install -m 0644 ${MODULE_NAME}.ko \
    ${D}/lib/modules/${KERNEL_VERSION}/kernel/${MODULE_NAME}.ko
}
