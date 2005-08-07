
;--------------------------------
; Product information

  !define PRODUCT_NAME       "iNFO"
  !define PRODUCT_SHORT_NAME "info"
  !define PRODUCT_VERSION    "1.0.2"
  !define PRODUCT_PUBLISHER  "Daniel Pérez Álvarez"
  !define PRODUCT_WEB_SITE   "http://info.sf.net/"

  !define PRODUCT_BIN_FOLDER "release_unicode"

  Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
  OutFile "${PRODUCT_SHORT_NAME}_${PRODUCT_VERSION}_winnt.exe"

;--------------------------------
; Include template

  !include "installer.nsh"
