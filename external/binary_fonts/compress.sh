#!/bin/sh

# Needs cURL

# Needs https://github.com/ocornut/imgui/blob/master/misc/fonts/$COMPRESS.cpp
# g++ -O3 imgui/misc/fonts/$COMPRESS.cpp -o $COMPRESS
COMPRESS="`pwd`/binary_to_compressed_c"

rm *.inl
mkdir -p temp && cd temp

# Roboto
curl -o Roboto.zip "https://fonts.google.com/download?family=Roboto"
unzip -oj Roboto.zip
$COMPRESS Roboto-Thin.ttf RobotoThin > ../RobotoThin.inl
$COMPRESS Roboto-ThinItalic.ttf RobotoThinItalic > ../RobotoThinItalic.inl
$COMPRESS Roboto-Light.ttf RobotoLight > ../RobotoLight.inl
$COMPRESS Roboto-LightItalic.ttf RobotoLightItalic. > ../RobotoLightItalic.inl
$COMPRESS Roboto-Regular.ttf RobotoRegular > ../RobotoRegular.inl
$COMPRESS Roboto-Italic.ttf RobotoItalic > ../RobotoItalic.inl
$COMPRESS Roboto-Medium.ttf RobotoMedium > ../RobotoMedium.inl
$COMPRESS Roboto-MediumItalic.ttf RobotoMediumItalic > ../RobotoMediumItalic.inl
$COMPRESS Roboto-Bold.ttf RobotoBold > ../RobotoBold.inl
$COMPRESS Roboto-BoldItalic.ttf RobotoBoldItalic > ../RobotoBoldItalic.inl
$COMPRESS Roboto-Black.ttf RobotoBlack > ../RobotoBlack.inl
$COMPRESS Roboto-BlackItalic.ttf RobotoBlackItalic > ../RobotoBlackItalic.inl

# Material Design icons
curl -o materialdesignicons-webfont.ttf "https://github.com/Templarian/MaterialDesign-Webfont/blob/master/fonts/materialdesignicons-webfont.ttf"
$COMPRESS materialdesignicons-webfont.ttf MaterialDesignIcons > ../MaterialDesignIcons.inl

# Clean up
cd ..
rm -rf temp
