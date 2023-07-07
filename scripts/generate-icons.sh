# argument 1: source icon path (.png 1024x1024)
# argument 2: destination

icon_path=$1
icon_directory=${icon_path%/*}
temp_directory_name="/temp.iconset"
target_directory=$icon_directory$temp_directory_name

mkdir $target_directory

sips -z 16 16 $icon_path --out $target_directory/icon_16x16.png
sips -z 32 32 $icon_path --out $target_directory/icon_16x16@2x.png
sips -z 32 32 $icon_path --out $target_directory/icon_32x32.png
sips -z 64 64 $icon_path --out $target_directory/icon_32x32@2x.png
sips -z 128 128 $icon_path --out $target_directory/icon_128x128.png
sips -z 256 256 $icon_path --out $target_directory/icon_128x128@2x.png
sips -z 256 256 $icon_path --out $target_directory/icon_256x256.png
sips -z 512 512 $icon_path --out $target_directory/icon_256x256@2x.png
sips -z 512 512 $icon_path --out $target_directory/icon_512x512.png
cp $icon_path $target_directory/icon_512x512@2x.png
iconutil -c icns --output $2 $target_directory
rm -r $target_directory