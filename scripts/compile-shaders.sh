# argument 1: source shader path (.png 1024x1024)
# argument 2: target shaders path

shaders_input_directory=$1
shaders_output_directory=$2

# delete and recreate shader output directory
[ -d $shaders_output_directory ] && rm -r $shaders_output_directory
mkdir $shaders_output_directory

spv_extension="spv"
directory_length=${#shaders_input_directory}

IFS=$'\n'; set -f
for file in $(find $shaders_input_directory -name "*.vert" -or -name "*.frag"); do
  file_name_with_extension="${file:$directory_length+1}" # trim the start of the file path
  file_extension="${file_name_with_extension##*.}"

  # input: folder/shader.vert, output: folder_shader_vert.spv
  output_file_name=${file_name_with_extension//\//_} # replace / with _
  output_file_name=${output_file_name%%.*} # remove extension
  output_file_name="$output_file_name"_"$file_extension"."$spv_extension" # add file extension and vert / frag identifier

  output_file_path="$shaders_output_directory"/"$output_file_name"

  glslc $file -o $output_file_path

  echo "compiled shader to: ${output_file_path}"

done
unset IFS; set +f