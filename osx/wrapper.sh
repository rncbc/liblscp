export WITH_INSTALL=1
export BUILD_BASE_DIR=$PWD/../temp_build
export CONFIG_OPTIONS="--disable-shared"
export CFLAGS="-I$BUILD_BASE_DIR/$BUILD_STYLE/local/include"
export CXXFLAGS=$CFLAGS
export PKG_CONFIG_PATH="$BUILD_BASE_DIR/$BUILD_STYLE/local/lib/pkgconfig"
export UB_PRODUCTS=lib/liblscp.a
source $PROJECT_DIR/autoconf_builder.sh
